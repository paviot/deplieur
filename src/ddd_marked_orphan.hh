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

#ifndef __CPN_DDD__DDD_MARKED_ORPHAN__
#define __CPN_DDD__DDD_MARKED_ORPHAN__

#include "ddd_compare_reference.hh"

namespace cpn_ddd
{

  /**
   * \brief Remove marked places linked to no transition operation.
   *
   * A warning is printed to output stream.
   */
  class ddd_marked_orphan_t : public ddd_compare_reference_t
  {
  private:

    /**
     * \brief Unavailable copy constructor.
     */
    ddd_marked_orphan_t (const ddd_marked_orphan_t& to_copy);

    /**
     * \brief Unavailable assignment operator.
     */
    ddd_marked_orphan_t& operator= (const ddd_marked_orphan_t& to_copy);

  protected:

    /**
     * \brief Internal method doing the work.
     */
    void remove_pre_places (unfold_ddd_t& transition,
			    ddds_t& places,
			    arcs_t::infos_t& pre_arcs);
    
    /**
     * \brief Internal method doing the work.
     */
    void remove_post_places (unfold_ddd_t& transition,
			     ddds_t& places,
			     arcs_t::infos_t& post_arcs);
    
    /**
     * \brief Arcs.
     */
    arcs_t& arcs_;

  public:

    /**
     * \brief Constructor.
     *
     * \param net The symbolic representation of Petri net to use.
     * \param classes The colour classes to use.
     * \param identifiers The identifiers to use.
     * \param arcs The arcs information to use.
     * \param output The output stream to put result of transform.
     */
    ddd_marked_orphan_t (net_t& net,
			 const classes_t& classes,
			 identifiers_t& identifiers,
			 arcs_t& arcs);

    /**
     * \brief Destructor.
     */
    ~ddd_marked_orphan_t ();

    /**
     * \brief Remove marked places linked to no transition operation.
     *
     * A warning is printed to output stream.
     */
    void transform ();

  };

}

#endif //__CPN_DDD__DDD_MARKED_ORPHAN__
