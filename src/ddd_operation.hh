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

#ifndef __CPN_DDD__DDD_OPERATION__
#define __CPN_DDD__DDD_OPERATION__

#include "ddd_common.hh"
#include "ddd_unfold.hh"

namespace cpn_ddd
{

  /**
   * \brief Base class for operations on symbolic net as Data Decision
   * Diagram.
   *
   * This base class is used to put operations in a queue.
   *
   * Operations change the "net_" reference when "transform" is
   * called.
   */
  class ddd_operation_t
  {
  private:
    
    /**
     * \brief Unavailable copy constructor.
     */
    ddd_operation_t (const ddd_operation_t& to_copy);

    /**
     * \brief Unavailable assignment operator.
     */
    ddd_operation_t& operator= (const ddd_operation_t& to_copy);

  protected:
    
    /**
     * \brief Symbolic net to use as input and output of operation.
     */
    net_t& net_;

    /**
     * \brief Colour classes to unfold.
     */    
    const classes_t& classes_;
    
    /**
     * \brief Association between identifiers in symbolic net and
     * concrete places and transitions in coloured net.
     */
    identifiers_t& identifiers_;
    
  public:
    
    /**
     * \brief Constructor.
     */
    ddd_operation_t (net_t& net,
		     const classes_t& classes,
		     identifiers_t& identifiers);
    
    /**
     * \brief Destructor.
     */
    virtual ~ddd_operation_t ();
    
    /**
     * \brief Pure virtual method to implement to create an
     * operation.
     *
     * \warning
     * Do NOT do modifications to "net_" outside this method (not in
     * constructor, nor in destructor) because operations are put in a
     * queue and only the call to "transform" is supposed to modify
     * the symbolic net.
     */
    virtual void transform () = 0;
    
  };

}

#endif //__CPN_DDD__DDD_OPERATION__
