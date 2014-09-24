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

#ifndef __CPN_DDD__VARIABLE_ORDER__
#define __CPN_DDD__VARIABLE_ORDER__

// libPetri headers :
#include <Domain.h>
#include <PNClass.h>
#include <Arc.h>
#include <Place.h>
#include <Transition.h>

// STL headers :
#include <string>
#include <map>
#include <vector>

namespace cpn_ddd
{

  /**
   * \brief Base class for variables orders.
   *
   * This class defines methods to do conversion between colour
   * classes and variables in a corresponding DDD.
   *
   * \warning Algorithms used internally in this class may not be
   * good. I wrote everything quickly. If someone can check, it would
   * be nice...
   */
  class variable_order_t
  {
  public:

    /**
     * \brief Type of a variable identifier.
     *
     * \warning This type must be compatible with the one used in
     * class Variable of libPetri.
     */
    typedef int variable_id_t;

    /**
     * \brief Constant for a variable identifier for a place.
     *
     * A places does not need variable identifier, because its domain
     * is explicitly defined. But "id_" is still needed. This constant
     * means that we are in a Place, not a Transition.
     */
    static const variable_id_t PLACE_ID;

    /**
     * \brief Association between a colour class and a variable identifier.
     */
    typedef struct
    {
      /**
       * \brief Colour class from libPetri.
       */
      PNClass* class_;

      /**
       * \brief Variable identifier.
       *
       * \warning For a place, its value is always "PLACE_ID".
       */
      variable_id_t id_;
    } info_t;

    /**
     * \brief Type of a collection of info_t.
     */
    typedef std::vector<info_t> infos_t;

    /**
     * \brief Type of a position in conversion tables.
     */
    typedef unsigned long position_t;

    /**
     * \brief Type of a conversion table.
     *
     * A conversion table is an association between positions.
     */
    typedef std::map<position_t, position_t> conversion_t;

    /**
     * \brief Constant for an unknown position in a conversion table.
     */
    static const position_t UNKNOWN_POSITION;

  private:

    /**
     * \brief Conversion table given by the specific variable order
     * class inherited from "variable_order_t".
     *
     * This conversion table does an association from position of
     * colour class in current model to its variable in corresponding
     * Data Decision Diagram.
     */
    conversion_t conversion_;

    /**
     * \brief Colour classes or variables extracted from place or
     * transition used.
     *
     * If the order is defined for a place, it is the colour classes
     * of the domain of the place.
     *
     * If the order is defined for a transition, it is the colour
     * classes of the variables of arcs linked to the transition.
     */
    infos_t variables_;

    /**
     * \brief Get variables from arcs.
     *
     * \param arcs The arcs to extract variables from.
     * \return The variables seen on arcs and their associated colour
     * classes.
     */
    infos_t get_variables (const std::vector<Arc*>& arcs) const;

    /**
     * \brief Get variables from a domain.
     *
     * \param domain The domain to extract colour classes from.
     * \return The colour classes of the domain.
     */
    infos_t get_variables (const Domain* domain) const;

  protected:

    /**
     * \brief Get conversion table from variables.
     *
     * \param variables The colour classes or variables to convert to
     * Data Decision Diagram.
     * \return A conversion table from a colour class position to its
     * variable in Data Decision Diagram.
     *
     * \warning This function must do the association for ALL
     * colour classes. A restriction to unfolded colour classes is
     * done somewhere else.
     */
    virtual conversion_t set_order (const infos_t variables) = 0;

    /**
     * \brief Procedure to call from constructors of derived classes,
     * to intialize "variables_" and "conversion_".
     */
    void reorder ();

  public:
    
    /**
     * \brief Constructor for a list of variables.
     */
    variable_order_t (const infos_t& variables);

    /**
     * \brief Constructor from a place.
     *
     * It extracts colour classes from the domain of "place".
     */
    variable_order_t (const Place& place);

    /**
     * \brief Constructor from a transition.
     *
     * It extracts variables and their colour classes from arcs linked
     * to "transition".
     */
    variable_order_t (const Transition& transition);

    /**
     * \brief Destructor.
     */
    virtual ~variable_order_t ();

    /**
     * \brief Assignment operator.
     */
    variable_order_t& operator= (const variable_order_t& to_copy);

    /**
     * \brief Get information about unfolded colour classes.
     *
     * \param classes The restricted colour classes to use (classes to
     * unfold).
     * \return The information about these colour classes, sorted to
     * be referenced by position in conversions tables obtained with
     * the same "classes" argument.
     */
    infos_t infos (const std::vector<PNClass*> classes) const;

    /**
     * \brief Get conversion table from colour class to variable in
     * Data Decision Diagram.
     * 
     * \param  classes The restricted colour classes to use (classes to
     * unfold).
     * \return The conversion table from restricted colour classes to
     * variable in Data Decision Diagram.
     */
    conversion_t order (const std::vector<PNClass*> classes) const;

    /**
     * \brief Get conversion table from colour class to variable in
     * Data Decision Diagram, but with UNKNOWN_POSITION for colour
     * classes not in "classes".
     *
     * \param classes The restricted colour classes to use.
     * \return The conversion table extended with UNKNOWN_POSITION.
     */
    conversion_t order_full (const std::vector<PNClass*> classes) const;

    /**
     * \brief Get conversion table from variable in Data Decision
     * Diagram to colour class.
     *
     * \param classes The restricted colour classes to use (classes to
     * unfold for example).
     * \return The conversion table from variable position in Data
     * Decision Diagram to restricted colour classes.
     */
    conversion_t reverse_order (const std::vector<PNClass*> classes) const;

    /**
     * \brief Get conversion table from colour class to variable in
     * Data Decision Diagram for colour classes not in "classes".
     *
     * \param classes Restricted classes not to show.
     * \return The conversion table from colour classes to variable
     * position in Data Decision Diagram where colour class in not in
     * "classes".
     */
    conversion_t other (const std::vector<PNClass*> classes) const;

  };

  /**
   * \brief Compare two colour information.
   */
  bool operator== (const variable_order_t::info_t& i1,
		   const variable_order_t::info_t& i2);

}

#endif //__CPN_DDD__VARIABLE_ORDER__
