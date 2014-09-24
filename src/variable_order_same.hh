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
 
/**
 * \file
 *
 * \author Alban Linard (2005), for LIP6-SRC (http://www-src.lip6.fr/)
 *
 * \version 1.0
 * \date 01/09/2005
 * First release of this file.
 */

#ifndef __CPN_DDD__VARIABLE_ORDER_SAME__
#define __CPN_DDD__VARIABLE_ORDER_SAME__

#include <Domain.h>
#include <PNClass.h>

#include "variable_order.hh"

namespace cpn_ddd
{

  /**
   * \brief Basic variable order.
   */
  class variable_order_same_t : public variable_order_t
  {
  protected:
    conversion_t set_order (const infos_t variables);
  public:
    variable_order_same_t (const infos_t variables);
    variable_order_same_t (const Transition& transition);
    variable_order_same_t (const Place& place);
    ~variable_order_same_t ();
  };

}

#endif //__CPN_DDD__VARIABLE_ORDER_SAME__
