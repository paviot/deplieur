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
 
#include "ddd_syphon_firstopt.hh"

namespace cpn_ddd
{

  ddd_syphon_firstopt_t::ddd_syphon_firstopt_t (net_t& net,
						const classes_t& classes,
						identifiers_t& identifiers,
						arcs_t& arcs) :
    ddd_operation_t(net, classes, identifiers),
    ddd_compare_firstopt_t (net, classes, identifiers, arcs)
  {}
  
  ddd_syphon_firstopt_t::~ddd_syphon_firstopt_t ()
  {}
  
  bool
  ddd_syphon_firstopt_t::reduce_step (net_t& net, arcs_t& arcs)
  {
    ddds_t& transitions = net.transitions();
    ddds_t& places = net.places();
    bool changed = false;
    for (ddds_t::iterator i = transitions.begin();
	 i != transitions.end();
	 ++i)
      {
	DDD& full_transitions_ddd = (*i).second.ddd();
	DDD removed_transitions_ddd(DDD::null);
	arcs_t::infos_t pre_arcs = this->arcs_.pre((*i).first, arcs_t::TRANSITION);
	if (pre_arcs.empty())
	  {
	    removed_transitions_ddd = full_transitions_ddd;
	    full_transitions_ddd = DDD::null;
	  }
	else
	  {
	    DDD keep_transitions_ddd(DDD::one);
	    for (arcs_t::infos_t::iterator j = pre_arcs.begin();
		 j != pre_arcs.end();
		 ++j)
	      {
		ddds_t::iterator k = places.find((*j).first);
		Arc* arc = (*j).second;
		std::list<CFuncTerm> terms = arc->Valuation().Lst();
		for (std::list<CFuncTerm>::iterator l = terms.begin();
		     l != terms.end();
		     ++l)
		  {
		    unfold_ddd_t place_ddd = (*k).second;
		    ddd_compare_firstopt_t::selects_map_t selects;
		    ddd_compare_firstopt_t::variables_map_t variables;
		    this->convert_to_transition((*i).second, (*k).second, variables, selects, *l);
		    this->select_variables(place_ddd, selects);
		    this->remove_variables(place_ddd, (*i).second, variables, false);
		    this->reorder_variables(place_ddd, variables);
		    this->rename_variables(place_ddd, variables);
		    this->add_variables(place_ddd, (*i).second, variables);
		    /*
		      {
		      Hom convert(this->ddd_compare_steps_t::convert_to_transition((*i).second, (*k).second, *l));
		      DDD converted_ddd = convert((*k).second.ddd());
		      if (converted_ddd != place_ddd.ddd())
		      exit(1);
		      }
		    */
		    if (keep_transitions_ddd == DDD::one)
		      keep_transitions_ddd = place_ddd.ddd();
		    else
		      keep_transitions_ddd = keep_transitions_ddd + place_ddd.ddd();
		  }
	      }
	    removed_transitions_ddd = full_transitions_ddd - keep_transitions_ddd;
	    full_transitions_ddd = full_transitions_ddd * keep_transitions_ddd;
	  }
	bool changed_ = !(removed_transitions_ddd == DDD::null);
	changed = changed || changed_;

	if (changed_)
	  {
	    arcs_t::infos_t post_arcs = this->arcs_.post((*i).first, arcs_t::TRANSITION);
	    for (arcs_t::infos_t::iterator j = post_arcs.begin();
		 j != post_arcs.end();
		 ++j)
	      {
		ddds_t::iterator k = places.find((*j).first);
		DDD remove_places_ddd(DDD::one);
		Arc* arc = (*j).second;
		std::list<CFuncTerm> terms = arc->Valuation().Lst();
		for (std::list<CFuncTerm>::iterator l = terms.begin();
		     l != terms.end();
		     ++l)
		  {
		    ddd_compare_firstopt_t::variables_map_t variables;
		    ddd_compare_firstopt_t::values_map_t values;
		    ddd_compare_firstopt_t::renames_map_t renames;
		    this->convert_to_place((*i).second, (*k).second, variables, values, *l);
		    unfold_ddd_t transition_ddd((*i).second);
		    transition_ddd.ddd() = removed_transitions_ddd;
		    this->remove_variables(transition_ddd, (*k).second, variables, true);
		    this->rename_variables(transition_ddd, variables, renames);
		    this->add_variables(transition_ddd, variables, values, renames);
		    /*
		      {
		      Hom convert(this->ddd_compare_steps_t::convert_to_place((*k).second, (*i).second, *l));
		      DDD converted_ddd = convert(removed_transitions_ddd);
		      if (converted_ddd != transition_ddd.ddd())
		      exit(1);
		      }
		    */
		    if (remove_places_ddd == DDD::one)
		      remove_places_ddd = transition_ddd.ddd();
		    else
		      remove_places_ddd = remove_places_ddd + transition_ddd.ddd();
		  }
		(*k).second.ddd() = (*k).second.ddd() - remove_places_ddd;
	      }
	  }
      }
    return changed;
  }

  void
  ddd_syphon_firstopt_t::transform ()
  {
    net_t net(this->net_);
    this->ddd_compare_firstopt_t::remove_places(net, true);
    do
      ;
    while (this->reduce_step(net, this->arcs_));
    // Remove lock
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
