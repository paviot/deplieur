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
 
#include "ddd_syphon_add.hh"
#include <PNet.h>
#include <Arcs.h>

namespace cpn_ddd
{

  ddd_syphon_add_t::ddd_syphon_add_t (net_t& net,
				      const classes_t& classes,
				      identifiers_t& identifiers,
				      arcs_t& arcs) :
    ddd_operation_t(net, classes, identifiers),
    ddd_compare_add_t(net, classes, identifiers),
    arcs_(arcs)
  {
    this->arcs_cache_ = arcs_cache_t(net.net()->LArc.size());
  }
  
  ddd_syphon_add_t::~ddd_syphon_add_t ()
  {}
  
  bool
  ddd_syphon_add_t::reduce_step (net_t& net, arcs_t& arcs)
  {
    ddds_t& transitions = net.transitions();
    ddds_t& places = net.places();
    bool changed = false;
    for (ddds_t::iterator i = transitions.begin(),
	   ii = this->guarded_transitions_.begin();
	 i != transitions.end();
	 ++i, ++ii)
      {
	/*
	std::cerr << "------------------------------"
		  << "Transition "
		  << this->identifiers_.transitions().find((*i).first)->second->Name()
		  << std::endl;
	*/
	DDD& full_transitions_ddd = (*i).second.ddd();
	DDD previous = full_transitions_ddd;
	DDD added_transitions_ddd((*ii).second.ddd());
	/*
	std::cerr << "Initial :"
		  << added_transitions_ddd.nbStates()
		  << std::endl;
	*/
	const arcs_t::infos_t pre_arcs = this->arcs_.pre((*i).first, arcs_t::TRANSITION);
	if (! pre_arcs.empty())
	  {
	    for (arcs_t::infos_t::const_iterator j = pre_arcs.begin();
		 j != pre_arcs.end();
		 ++j)
	      {
		ddds_t::iterator k = places.find((*j).first);
		const std::list<CFuncTerm> terms = (*j).second->Valuation().Lst();
		for (std::list<CFuncTerm>::const_iterator term = terms.begin();
		     term != terms.end();
		     ++term)
		  {
		    Hom apply(this->convert_to_transition((*i).second, (*k).second, *term));
		    added_transitions_ddd = added_transitions_ddd * apply((*k).second.ddd());
		    /*
		    std::cerr << "Place "
			      << this->identifiers_.places().find((*k).first)->second->Name()
			      << ", term "
			      << *term
			      << " : "
			      << added_transitions_ddd.nbStates()
			      << std::endl;
		    */
		  }
		/*
		Hom apply(this->convert_to_transition((*i).second, (*k).second, (*j).second));
		added_transitions_ddd = added_transitions_ddd * apply((*k).second.ddd());
		std::cerr << "Place "
			  << this->identifiers_.places().find((*k).first)->second->Name()
			  << " : "
			  << added_transitions_ddd.nbStates()
			  << std::endl;
		*/
	      }
	  }
	full_transitions_ddd = full_transitions_ddd + added_transitions_ddd;
	/*
	std::cerr << "Transitions : "
		  << full_transitions_ddd.nbStates()
		  << std::endl;
	*/
	bool changed_ = !(full_transitions_ddd == previous);
	changed = changed || changed_;
	// Add post places :
	if (changed_)
	  {
	    const arcs_t::infos_t post_arcs = this->arcs_.post((*i).first, arcs_t::TRANSITION);
	    for (arcs_t::infos_t::const_iterator j = post_arcs.begin();
		 j != post_arcs.end();
		 ++j)
	      {
		ddds_t::iterator k = places.find((*j).first);
		Hom apply(this->convert_to_place((*k).second, (*i).second, (*j).second));
		(*k).second.ddd() = (*k).second.ddd() + apply(added_transitions_ddd);
		/*
		std::cerr << "Place "
			  << this->identifiers_.places().find((*k).first)->second->Name()
			  << " : "
			  << (*k).second.ddd().nbStates()
			  << std::endl;
		*/
	      }
	  }
	//	std::cerr << "========================================" << std::endl;
      }
    return changed;
  }

  void
  ddd_syphon_add_t::transform ()
  {
    this->guarded_transitions_ = this->net_.transitions();
    for (ddds_t::iterator i = this->net_.transitions().begin();
	 i != this->net_.transitions().end();
	 ++i)
      (*i).second.ddd() = DDD::null;
    this->remove_places(this->net_, false);
    /*
    for (ddds_t::iterator i = this->net_.places().begin();
	 i != this->net_.places().end();
	 ++i)
      cerr << *this->identifiers_.places().find((*i).first)->second
	   << " : "
	   << (*i).second.ddd().nbStates()
	   << std::endl;
    */
    do
      ;//      cerr << "Iteration" << endl;
    while (this->reduce_step(this->net_, this->arcs_));
  }
  
}
