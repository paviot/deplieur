// Copyright (C) 2003, 2004  Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
//
// This file is part of col2ordequivDDD, an unfolder from Well-Formed
// to Well-Formed or uncolored Petri nets.
//
// col2ordequivDDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// col2ordequivDDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Spot; see the file COPYING.  If not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
 
#include "ddd_syphon_transitions.hh"

namespace cpn_ddd
{

  ddd_syphon_transitions_t::ddd_syphon_transitions_t (net_t& net,
						      const classes_t& classes,
						      identifiers_t& identifiers,
						      arcs_t& arcs) :
    ddd_operation_t(net, classes, identifiers),
    ddd_compare_reference_t(net, classes, identifiers),
    arcs_(arcs)
  {}
  
  ddd_syphon_transitions_t::~ddd_syphon_transitions_t ()
  {}
  
  bool
  ddd_syphon_transitions_t::reduce_step (net_t& net, arcs_t& arcs)
  {
    ddds_t& transitions = net.transitions();
    ddds_t& places = net.places();
    bool changed = false;
    for (ddds_t::iterator i = transitions.begin();
	 i != transitions.end();
	 ++i)
      {
	DDD removed_transitions_ddd(DDD::null);
	const arcs_t::infos_t pre_arcs = this->arcs_.pre((*i).first, arcs_t::TRANSITION);
	if (pre_arcs.empty())
	  removed_transitions_ddd = (*i).second.ddd();
	else
	  {
	    DDD keep_transitions_ddd(DDD::one);
	    for (arcs_t::infos_t::const_iterator j = pre_arcs.begin();
		 j != pre_arcs.end();
		 ++j)
	      {
		ddds_t::iterator k = places.find((*j).first);
		Hom apply(this->convert_to_transition((*i).second, (*k).second, (*j).second));
		if (keep_transitions_ddd == DDD::one)
		  keep_transitions_ddd = apply((*k).second.ddd());
		else
		  keep_transitions_ddd = keep_transitions_ddd + apply((*k).second.ddd());
	      }
	    removed_transitions_ddd = (*i).second.ddd() - keep_transitions_ddd;
	  }
	bool changed_(false);
	if (this->old_values_[(*i).first] != removed_transitions_ddd)
	  {
	    changed_ = true;
	    this->old_values_[(*i).first] = removed_transitions_ddd;
	  }
	changed = changed || changed_;
	// Suppress post places :
	if (changed_)
	  {
	    const arcs_t::infos_t post_arcs = this->arcs_.post((*i).first, arcs_t::TRANSITION);
	    for (arcs_t::infos_t::const_iterator j = post_arcs.begin();
		 j != post_arcs.end();
		 ++j)
	      {
		ddds_t::iterator k = places.find((*j).first);
		Hom apply(this->convert_to_place((*k).second, (*i).second, (*j).second));
		(*k).second.ddd() = (*k).second.ddd() - apply(removed_transitions_ddd);
	      }
	  }
      }
    return changed;
  }

  void
  ddd_syphon_transitions_t::remove_transitions (net_t& net, arcs_t& arcs)
  {
    ddds_t& transitions = net.transitions();
    ddds_t& places = net.places();
    for (ddds_t::iterator i = transitions.begin();
	 i != transitions.end();
	 ++i)
      {
	DDD keep_transitions_ddd(DDD::one);
	const arcs_t::infos_t pre_arcs = this->arcs_.pre((*i).first, arcs_t::TRANSITION);
	for (arcs_t::infos_t::const_iterator j = pre_arcs.begin();
	     j != pre_arcs.end();
	     ++j)
	  {
	    ddds_t::iterator k = places.find((*j).first);
	    Hom apply(this->convert_to_transition((*i).second, (*k).second, (*j).second));
	    if (keep_transitions_ddd == DDD::one)
	      keep_transitions_ddd = apply((*k).second.ddd());
	    else
	      keep_transitions_ddd = keep_transitions_ddd + apply((*k).second.ddd());
	  }
	(*i).second.ddd() = (*i).second.ddd() * keep_transitions_ddd;
      }    
  }

  void
  ddd_syphon_transitions_t::transform ()
  {
    net_t net(this->net_);
    for (ddds_t::iterator i = net.transitions().begin();
	 i != net.transitions().end();
	 ++i)
      this->old_values_[(*i).first] = DDD::null;
    this->remove_places(net, true);
    do
      ;
    while (this->reduce_step(net, this->arcs_));
    this->remove_transitions(net, this->arcs_);
    // Remove lock :
    ddds_t::iterator t_destination;
    ddds_t::iterator t_source;
    for (t_destination = this->net_.transitions().begin(),
	   t_source = net.transitions().begin();
	 t_destination !=  this->net_.transitions().end();
	 ++t_destination, ++t_source)
      (*t_destination).second.ddd() = (*t_destination).second.ddd() - (*t_source).second.ddd();
    ddds_t::iterator p_destination;
    ddds_t::iterator p_source;
    for (p_destination = this->net_.places().begin(),
	   p_source = net.places().begin();
	 p_destination !=  this->net_.places().end();
	 ++p_destination, ++p_source)
      (*p_destination).second.ddd() = (*p_destination).second.ddd() - (*p_source).second.ddd();
  }
  
}
