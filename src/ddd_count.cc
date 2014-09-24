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
 
#include "ddd_count.hh"
#include <iostream>

namespace cpn_ddd
{

  ddd_count_t::ddd_count_t (net_t& net,
			    classes_t& classes,
			    identifiers_t& identifiers) :
    ddd_operation_t (net, classes, identifiers)
  {}

  ddd_count_t::~ddd_count_t ()
  {}

  void
  ddd_count_t::transform ()
  {
    clog << std::endl;
    ddds_t& places = this->net_.places();
    for (ddds_t::iterator i = places.begin();
	 i != places.end();
	 ++i)
      clog << "Place "
	   << (*this->identifiers_.places().find((*i).first)).second->Name()
	   << " : "
	   << (*i).second.ddd().nbStates()
	   << endl;
    ddds_t& transitions = this->net_.transitions();
    for (ddds_t::iterator i = transitions.begin();
	 i != transitions.end();
	 ++i)
      clog << "Transition "
	   << (*this->identifiers_.transitions().find((*i).first)).second->Name()
	   << " : "
	   << (*i).second.ddd().nbStates()
	   << endl;
  }

}
