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

#ifndef __CPN_DDD__DDD_COMMON__
#define __CPN_DDD__DDD_COMMON__

#include <DDD.h>

#include <list>

#include "variable_order.hh"

namespace cpn_ddd
{

  /**
   * \brief Type for identifiers.
   */
  typedef int identifier_t;

  /**
   * \brief Type for values.
   */
  typedef int value_t;

  /**
   * \brief Type for vectors of values.
   */
  typedef std::vector<value_t> values_t;

  /**
   * \brief Type for vectors of colour classes.
   */
  typedef std::vector<PNClass*> classes_t;

  /**
   * \brief Identifiers operations.
   *
   * This class defines operations to get a place or a transition from
   * its identifier, and to add a place or a transition and get its
   * unique identifier.
   *
   * \warning Identifiers used in this class are NOT identifiers from
   * libPetri.
   *
   * \warning This class does not check if identifiers get to maximum
   * and come back to first. It is easy to implement but needs extra
   * calculation time...
   */
  class identifiers_t
  {
  public:

    /**
     * \brief Type for a table associating identifiers with places.
     */
    typedef std::map<identifier_t, Place*> places_table_t;

    /**
     * \brief Type for a table associating identifiers with transitions.
     */
    typedef std::map<identifier_t, Transition*> transitions_table_t;

    /**
     * \brief Constant for an unknown identifier.
     */
    static const identifier_t UNKNOWN_IDENTIFIER;

  protected:

    /**
     * \brief Table of places.
     */
    places_table_t places_table_;

    /**
     * \brief Table of transitions.
     */
    transitions_table_t transitions_table_;

    /**
     * \brief Next place identifier available.
     */
    identifier_t place_identifier_;

    /**
     * \brief Next transition identifier available.
     */
    identifier_t transition_identifier_;

  public:

    /**
     * \brief Default constructor.
     */
    identifiers_t ();

    /**
     * \brief Copy constructor.
     *
     * Everything is copied, even next place and transition
     * identifiers.
     */
    identifiers_t (const identifiers_t& to_copy);

    /**
     * \brief Destructor.
     */
    ~identifiers_t ();

    /**
     * \brief Assignment operator.
     */
    identifiers_t& operator= (const identifiers_t& to_copy);

    /**
     * \brief Get places table.
     *
     * Places can be accessed by their identifiers using [] operator.
     *
     * \return Places table.
     */
    const places_table_t& places ();

    /**
     * \brief Get transitions table.
     *
     * Transitions can be accessed by their identifiers using []
     * operator.
     *
     * \return Transitions table.
     */
    const transitions_table_t& transitions ();

    /**
     * \brief Add a place.
     *
     * If "check" and "place" is already in places table, the
     * identifier of found place is returned.
     *
     * \param place The place to add.
     * \param check If true, check in all places table if place does
     * not exist.
     * \return Identifier of added place.
     */
    const identifier_t add (Place* const place,
			    bool check = true);

    /**
     * \brief Add a transition.
     *
     * If "check" and "transition" is already in transitions table,
     * the identifier of found transition is returned.
     *
     * \param transition The transition to add.
     * \param check If true, check in all transitions table if
     * transition does not exist.
     * \return Identifier of added transition.
     */
    const identifier_t add (Transition* const transition,
			    bool check = true);

    /**
     * \brief Get the identifier of a place.
     *
     * \param place The place to search.
     * \return Its identifier if "place" is in places table,
     * "UNKNOWN_IDENTIFIER" else.
     */
    const identifier_t place (Place* const place) const;

    /**
     * \brief Get the identifier of a transition.
     *
     * \param transition The transition to search.
     * \return Its identifier if "transition" is in transitions table,
     * "UNKNOWN_IDENTIFIER" else.
     */
    const identifier_t transition (Transition* const transition) const;

  };

  /**
   * \brief Type for a vector of identifiers.
   */
  typedef std::vector<identifier_t> ids_t;

  /**
   * \brief Get value of succ-nth successor of value "value" in
   * "elements".
   *
   * \warning This function needs values to be in a range [0 .. n]. If
   * it is not the case, change the body of the function !
   */
  const value_t get_succ (const value_t value,
			  const value_t succ,
			  const values_t& elements);

  /**
   * \brief Get value of succ-nth successor of value "value" in
   * "elements".
   *
   * \warning This function needs values to be in a range [0 .. n]. If
   * it is not the case, change the body of the function !
   */
  const value_t get_succ (const value_t value,
			  const value_t succ,
			  const std::vector<Element>* elements);

}

#endif //__CPN_DDD__DDD_COMMON__
