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
 
#include "ddd_syphon_reference.hh"
#include <PNet.h>
#include <Arcs.h>

namespace cpn_ddd
{

  ddd_syphon_reference_t::ddd_syphon_reference_t (net_t& net,
						  const classes_t& classes,
						  identifiers_t& identifiers,
						  arcs_t& arcs) :
    ddd_operation_t(net, classes, identifiers),
    ddd_compare_reference_t(net, classes, identifiers),
    arcs_(arcs)
  {
    this->arcs_cache_ = arcs_cache_t(net.net()->LArc.size());
  }
  
  ddd_syphon_reference_t::~ddd_syphon_reference_t ()
  {}
  
  bool
  ddd_syphon_reference_t::reduce_step (net_t& net, arcs_t& arcs)
  {
    ddds_t& transitions = net.transitions();
    ddds_t& places = net.places();
    bool changed = false;
    for (ddds_t::iterator i = transitions.begin();
	 i != transitions.end();
	 ++i)
      {
	//	std::cerr << "==============================" << std::endl;
	DDD& full_transitions_ddd = (*i).second.ddd();
	DDD removed_transitions_ddd(DDD::null);
	const arcs_t::infos_t pre_arcs = this->arcs_.pre((*i).first, arcs_t::TRANSITION);
	if (pre_arcs.empty())
	  {
	    removed_transitions_ddd = full_transitions_ddd;
	    full_transitions_ddd = DDD::null;
	  }
	else
	  {
	    DDD keep_transitions_ddd(DDD::one);
	    for (arcs_t::infos_t::const_iterator j = pre_arcs.begin();
		 j != pre_arcs.end();
		 ++j)
	      {
		ddds_t::iterator k = places.find((*j).first);
		/*
		const std::list<CFuncTerm> terms = (*j).second->Valuation().Lst();
		for (std::list<CFuncTerm>::const_iterator z = terms.begin();
		     z != terms.end();
		     ++z)
		  {
		    if (z->Mult() > 0)
		      {
			if (keep_transitions_ddd == DDD::one)
			  keep_transitions_ddd = this->convert_to_transition((*i).second, (*k).second, *z)((*k).second.ddd());
			else
			  keep_transitions_ddd = this->convert_to_transition((*i).second, (*k).second, *z)((*k).second.ddd())
			    + keep_transitions_ddd;
		      }
		  }
		for (std::list<CFuncTerm>::const_iterator z = terms.begin();
		     z != terms.end();
		     ++z)
		  if (z->Mult() < 0)
		    {
		      //		      std::cerr << "WARNING" << std::endl;
		      if (keep_transitions_ddd == DDD::one)
			keep_transitions_ddd = this->convert_to_transition((*i).second, (*k).second, *z)((*k).second.ddd());
		      else
			keep_transitions_ddd = this->convert_to_transition((*i).second, (*k).second, *z)((*k).second.ddd())
			  - keep_transitions_ddd;
		    }
		*/
		Hom apply(this->convert_to_transition((*i).second, (*k).second, (*j).second));
		if (keep_transitions_ddd == DDD::one)
		  keep_transitions_ddd = apply((*k).second.ddd());
		else
		  keep_transitions_ddd = keep_transitions_ddd + apply((*k).second.ddd());
		/*
		cerr << "Prev : " << endl << k->second.ddd();
		cerr << "Iter :" << endl;
		cerr << apply(k->second.ddd());
		*/
		/*
		if (apply(k->second.ddd()) == DDD::top)
		  {
		    Hom apply_dbg(this->convert_to_transition_dbg((*i).second, (*k).second, (*j).second));
		    cerr << "Debug :" << std::endl;
		    cerr << apply_dbg(k->second.ddd());
		  }
		*/
	      }
	    removed_transitions_ddd = full_transitions_ddd - keep_transitions_ddd;
	    full_transitions_ddd = full_transitions_ddd * keep_transitions_ddd;
	    /*
	    cerr << "pre :\t"
		 << "full :" << endl << full_transitions_ddd
		 << "keep :" << endl << keep_transitions_ddd
		 << "removed :" << endl << removed_transitions_ddd;
	    */
	    //	    if (keep_transitions_ddd == DDD::top)
	    //	      exit(EXIT_SUCCESS);
	    /*
	      {
		cerr << "Error in :" << endl;
		for (arcs_t::infos_t::const_iterator j = pre_arcs.begin();
		     j != pre_arcs.end();
		     ++j)
		  {
		    ddds_t::iterator k = places.find((*j).first);
		    const std::list<CFuncTerm> terms = (*j).second->Valuation().Lst();
		    for (list<CFuncTerm>::const_iterator z = terms.begin();
			 z != terms.end();
			 ++z)
		      {
			Hom apply(this->convert_to_transition_temp((*i).second, (*k).second, *z));
			cerr << "==>" << endl << apply((*k).second.ddd());
		      }
		  }
	      }
	    */
	  }
	bool changed_ = !(removed_transitions_ddd == DDD::null);
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
		/*
		DDD out_places(DDD::null);
		const std::list<CFuncTerm> terms = (*j).second->Valuation().Lst();
		for (std::list<CFuncTerm>::const_iterator z = terms.begin();
		     z != terms.end();
		     ++z)
		  if (z->Mult() > 0)
		    out_places = this->convert_to_place((*k).second, (*i).second, *z)(removed_transitions_ddd)
		      + out_places;
		for (std::list<CFuncTerm>::const_iterator z = terms.begin();
		     z != terms.end();
		     ++z)
		  if (z->Mult() < 0)
		    {
		      //		      std::cerr << "WARNING" << std::endl;
		      out_places = this->convert_to_transition((*i).second, (*k).second, *z)(removed_transitions_ddd)
			- out_places;
		    }
		(*k).second.ddd() = (*k).second.ddd() - out_places;
		*/
		Hom apply(this->convert_to_place((*k).second, (*i).second, (*j).second));
		(*k).second.ddd() = (*k).second.ddd() - apply(removed_transitions_ddd);
		/*
		cerr << "post :\t"
		     << (*k).second.ddd() << "\t"
		     << apply(removed_transitions_ddd) << "\t"
		     << (*k).second.ddd() - apply(removed_transitions_ddd) << "\t"
		     << endl;
		*/
		/*
		bool cont;
		cin >> cont;
		if (! cont)
		  {
		    Hom apply_dbg(this->convert_to_place_dbg((*k).second, (*i).second, (*j).second));
		    cerr << "post :\t"
			 << (*k).second.ddd() << "\t"
			 << apply_dbg(removed_transitions_ddd) << "\t"
			 << (*k).second.ddd() - apply(removed_transitions_ddd) << "\t"
			 << endl;
		    exit(EXIT_SUCCESS);
		  }
		*/
	      }
	  }
      }
    return changed;
  }

  void
  ddd_syphon_reference_t::transform ()
  {
    net_t net(this->net_);
    this->remove_places(net, true);
    do
      ; //cerr << "Iteration" << endl;
    while (this->reduce_step(net, this->arcs_));
    // Remove syphon
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
