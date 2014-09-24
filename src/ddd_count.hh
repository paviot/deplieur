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

#ifndef __CPN_DDD__DDD_COUNT__
#define __CPN_DDD__DDD_COUNT__

#include "ddd_operation.hh"

namespace cpn_ddd
{
  
  /**
   * \brief Count unfolded places and transitions operation.
   *
   * This operation prints to an output stream the number of unfolded
   * places or transitions for each coloured place and transition.
   */
  class ddd_count_t : public ddd_operation_t
  {
  private:

    /**
     * \brief Unavailable copy constructor.
     */
    ddd_count_t (const ddd_count_t& to_copy);

    /**
     * \brief Unavailable assignment operator.
     */
    ddd_count_t& operator= (const ddd_count_t& to_copy);

  public:

    /**
     * \brief Constructor.
     *
     * \param net The symbolic representation of Petri net to use.
     * \param classes The colour classes to use.
     * \param identifiers The identifiers to use.
     */
    ddd_count_t (net_t& net,
		 classes_t& classes,
		 identifiers_t& identifiers);

    /**
     * \brief Destructor.
     */
    ~ddd_count_t ();

    /**
     * \brief Concrete "transform" needed.
     *
     * This "transform" prints to an output stream the number of
     * unfolded places or transitions for each coloured place and
     * transition found in the source Petri net.
     */
    void transform ();

  };

}

#endif //__CPN_DDD__DDD_COUNT__
