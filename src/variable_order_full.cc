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

#include "variable_order_full.hh"

namespace cpn_ddd
{

  variable_order_t::position_t variable_order_full_t::next_position_=
  numeric_limits<position_t>::min();

  variable_order_full_t::variable_order_full_t (const variable_order_t::infos_t variables) :
    variable_order_t(variables)
  {
    this->reorder();
  }

  variable_order_full_t::variable_order_full_t (const Place& place) :
    variable_order_t(place)
  {
    this->reorder();
  }

  variable_order_full_t::variable_order_full_t (const Transition& transition) :
    variable_order_t(transition)
  {
    this->reorder();
  }

  variable_order_full_t::~variable_order_full_t ()
  {}

  variable_order_t::conversion_t
  variable_order_full_t::set_order (const variable_order_t::infos_t variables)
  {
    conversion_t result;
    position_t rpos = variables.size()-1;
    //    position_t rpos = 0;
    for (infos_t::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      {
	result[rpos] = this->next_position_;
	++this->next_position_;
	--rpos;
	//	++rpos;
      }
    return result;
  }

}
