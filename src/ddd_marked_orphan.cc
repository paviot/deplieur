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
 
#include "ddd_marked_orphan.hh"
#include <PNet.h>
#include <Arcs.h>

namespace cpn_ddd
{

  ddd_marked_orphan_t::ddd_marked_orphan_t (net_t& net,
					    const classes_t& classes,
					    identifiers_t& identifiers,
					    arcs_t& arcs) :
    ddd_operation_t(net, classes, identifiers),
    ddd_compare_reference_t (net, classes, identifiers),
    arcs_(arcs)
  {
    this->arcs_cache_ = arcs_cache_t(net.net()->LArc.size());
  }

  ddd_marked_orphan_t::~ddd_marked_orphan_t ()
  {}

  void
  ddd_marked_orphan_t::remove_pre_places (unfold_ddd_t& transition,
					  ddds_t& places,
					  arcs_t::infos_t& pre_arcs)
  {
    for (arcs_t::infos_t::iterator j = pre_arcs.begin();
	 j != pre_arcs.end();
	 ++j)
      {
	ddds_t::iterator k = places.find((*j).first);
	Arc* arc = (*j).second;

	DDD to_remove(DDD::null);
	const std::list<CFuncTerm> terms = arc->Valuation().Lst();
	for (std::list<CFuncTerm>::const_iterator z = terms.begin();
	     z != terms.end();
	     ++z)
	  if (z->Mult() > 0)
	    to_remove = this->convert_to_place((*k).second, transition, *z)(transition.ddd())
	      + to_remove;
	for (std::list<CFuncTerm>::const_iterator z = terms.begin();
	     z != terms.end();
	     ++z)
	  if (z->Mult() < 0)
	    to_remove = this->convert_to_place((*k).second, transition, *z)(transition.ddd())
	      - to_remove;
	(*k).second.ddd() = (*k).second.ddd() - to_remove;

	/*
	Hom apply(this->convert_to_place((*k).second, transition, arc));
	(*k).second.ddd() = (*k).second.ddd() - apply(transition.ddd());
	*/
      }
  }

  void
  ddd_marked_orphan_t::remove_post_places (unfold_ddd_t& transition,
					   ddds_t& places,
					   arcs_t::infos_t& post_arcs)
  {
    for (arcs_t::infos_t::iterator j = post_arcs.begin();
	 j != post_arcs.end();
	 ++j)
      {
	ddds_t::iterator k = places.find((*j).first);
	Arc* arc = (*j).second;

	DDD to_remove(DDD::null);
	const std::list<CFuncTerm> terms = arc->Valuation().Lst();
	for (std::list<CFuncTerm>::const_iterator z = terms.begin();
	     z != terms.end();
	     ++z)
	  if (z->Mult() > 0)
	    to_remove = this->convert_to_place((*k).second, transition, *z)(transition.ddd())
	      + to_remove;
	for (std::list<CFuncTerm>::const_iterator z = terms.begin();
	     z != terms.end();
	     ++z)
	  if (z->Mult() < 0)
	    to_remove = this->convert_to_place((*k).second, transition, *z)(transition.ddd())
	      - to_remove;
	(*k).second.ddd() = (*k).second.ddd() - to_remove;

	/*
	Hom apply(this->convert_to_place((*k).second, transition, arc));
	(*k).second.ddd() = (*k).second.ddd() - apply(transition.ddd());
	*/
      }
  }

  void
  ddd_marked_orphan_t::transform ()
  {
    net_t net(this->net_);
    this->remove_places(net, false); // keep only marked places
    ddds_t& transitions = net.transitions();
    ddds_t& places = net.places();
    for (ddds_t::iterator i = transitions.begin();
	 i != transitions.end();
	 ++i)
      {
	arcs_t::infos_t post_arcs = this->arcs_.post((*i).first, arcs_t::TRANSITION);
	this->remove_post_places((*i).second, places, post_arcs);
	arcs_t::infos_t pre_arcs = this->arcs_.pre((*i).first, arcs_t::TRANSITION);
	this->remove_pre_places((*i).second, places, pre_arcs);
      }
    for (ddds_t::iterator i = places.begin();
	 i != places.end();
	 ++i)
	if ((*i).second.ddd().nbStates() != 0)
	  {
	    /*
	    clog << "WARNING : place "
		 << (*this->identifiers_.places().find((*i).first)).second->Name()
		 << " has " << (*i).second.ddd().nbStates()
		 << " unfolded places marked and not linked to any transition."
		 << endl;
	    */
	  }
    ddds_t::iterator p_destination;
    ddds_t::iterator p_source;
    for (p_destination = this->net_.places().begin(),
	   p_source = net.places().begin();
	 p_destination !=  this->net_.places().end();
	 ++p_destination, ++p_source)
      (*p_destination).second.ddd() = (*p_destination).second.ddd() - (*p_source).second.ddd();
  }

}
