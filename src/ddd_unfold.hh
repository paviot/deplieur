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

#ifndef __CPN_DDD__DDD_UNFOLD__
#define __CPN_DDD__DDD_UNFOLD__

#include "ddd_common.hh"

#include <boost/shared_ptr.hpp>
#include <ext/hash_map>

namespace cpn_ddd
{

  class elts_hash_t
  {
  public:
    const size_t operator() (const std::vector<Element>* const elts) const;
  };
  
  /**
   * \brief Symbolic representation of unfolded places or transitions
   * from one place or transition.
   */
  class unfold_ddd_t
  {
  protected:

    /**
     * \brief Data Decision Diagram of the place or transition.
     */
    DDD ddd_;

    /**
     * \brief Variable order correspondance between DDD and coloured
     * place or transition.
     *
     * \warning I used a "shared_ptr" becase the copy constructor of
     * this class can be called, and "order_" must be freed when no
     * class points to it.
     */
    boost::shared_ptr<variable_order_t> order_;

  public:
    
    /**
     * \brief Constructor.
     */
    unfold_ddd_t (const DDD& ddd,
		  variable_order_t* const order);

    /**
     * \brief Copy constructor.
     *
     * \warning "order_" is shared between the two unfold_ddd_t.
     */
    unfold_ddd_t (const unfold_ddd_t& to_copy);

    /**
     * \brief Destructor.
     */
    ~unfold_ddd_t ();

    /**
     * \brief Assignment operator.
     */
    unfold_ddd_t& operator= (const unfold_ddd_t& to_copy);

    const DDD& ddd () const;

    /**
     * \brief Get the Data Decision Diagram.
     */
    DDD& ddd ();

    /**
     * \brief Variable order associated.
     */
    variable_order_t* const order () const;

  };

  /**
   * \brief Type of a vector of symbolic places or transitions.
   */
  typedef std::map<identifier_t, unfold_ddd_t> ddds_t;

  /**
   * \brief Symbolic representation of a Petri net.
   */
  class net_t
  {
  protected:

    /**
     * \brief Type for results extracted from cache.
     */
    typedef __gnu_cxx::hash_map<const std::vector<Element>*, values_t, elts_hash_t> results_t;

    /**
     * \brief Data Decision Diagrams representing places.
     */
    ddds_t places_;

    /**
     * \brief Data Decision Diagrams representing transitions.
     */
    ddds_t transitions_;

    /**
     * \brief Source Petri net using libPetri.
     */
    PNet* net_;

    /**
     * \brief Null domain extracted from libPetri or created if none
     * existed.
     */
    Domain* null_domain_;

    /**
     * \brief Cache of results for "extract" method.
     */
    mutable results_t extract_cache_;

  public:

    /**
     * \brief Constructor.
     *
     * It creates a symbolic represntation of a Petri net using Data
     * Decision Diagrams.
     *
     * \param net The Petri net to convert.
     */
    net_t (PNet* const net);

    /**
     * \brief Copy constructor.
     *
     * \warning Only "net_" and "null_domain_" are shared.
     */
    net_t (const net_t& to_copy);

    /**
     * \brief Destructor.
     *
     * It deletes the symbolic representation of a Petri net in Data
     * Decision Diagramss.
     *
     * All corresponding Data Decision Diagrams are deleted, but not
     * the Petri net from libPetri.
     */
    ~net_t ();

    /**
     * \brief Assignment operator.
     */
    net_t& operator= (const net_t& to_copy);

    /**
     * \brief Get symbolic representation of places.
     *
     * \return The symbolic representation of places.
     */
    ddds_t& places ();

    /**
     * \brief Get symbolic representation of transitions.
     *
     * \return The symbolic representation of transitions.
     */
    ddds_t& transitions ();

    /**
     * \brief Get source Petri net.
     *
     * \return Source Petri net.
     */
    PNet* const net ();

    /**
     * \brief Get NULL domain.
     *
     * \return NULL domain of source Petri net.
     */
    Domain* const null_domain ();

    /**
     * \brief Set NULL domain.
     *
     * \param domain NULL domain to use.
     *
     * \warning There is no check of presence of "domain" in source
     * Petri net.
     */
    void null_domain (Domain* const domain);

    /**
     * \brief Extract values from a vector of elements.
     *
     * \param elements Elements to extract from.
     * \return Values extracted from "elements".
     *
     * \warning This method uses a cache to remember results based on
     * "elements" addresses.
     */
    const values_t extract_cached (const std::vector<Element>& elements) const;

    /**
     * \brief Extract values from a vector of elements.
     *
     * \param elements Elements to extract from.
     * \return Values extracted from "elements".
     */
    const values_t extract (const std::vector<Element>& elements) const;

  };

  /**
   * \brief Hierarchy of unfolded places and transitions.
   *
   * This class has methods to access hierarchy between coloured
   * places and transitions from source Petri net and generated places
   * and transitions.
   */
  class unfold_hierarchy_t
  {
  protected:

    /**
     * \brief Type associating a source identifier (of a place or a
     * transition) to generated destination identifiers.
     */
    typedef std::map<identifier_t, ids_t> map_t;

    /**
     * \brief Hierarchy of places and transitions.
     */
    map_t hierarchy_;

    /**
     * \brief Result cache of "parent" method.
     */
    mutable identifier_t last_;

  public:

    /**
     * \brief Constructor.
     *
     * Hierarrchy is initially empty.
     */
    unfold_hierarchy_t ();

    /**
     * \brief Copy constructor.
     *
     * \warning Result cached of "parent" is not copied.
     */
    unfold_hierarchy_t (const unfold_hierarchy_t& to_copy);

    /**
     * \brief Destructor.
     */
    ~unfold_hierarchy_t ();

    /**
     * \brief Assignment operator.
     */
    unfold_hierarchy_t& operator= (const unfold_hierarchy_t& to_copy);

    /**
     * \brief Get parent identifier (source place or transition) of an
     * identifier (generated place or transition),
     *
     * \param id Identifier of a generated place or transition.
     * \return Identifier of source place or transition corresponding,
     * or UNKNOWN_IDENTIFIER if "id" was not found.
     */
    const identifier_t parent (const identifier_t id) const;

    /**
     * \brief Get identifiers of generated places or transitions from
     * a source identifier (place or transition).
     *
     * \param id Source identifier (place or transition).
     * \return Identifiers of generated places or transitions from
     * "id".
     */
    ids_t& unfolded (const identifier_t id);

  };

  /**
   * \brief Representation of the arcs of a Petri net.
   */
  class arcs_t
  {
  public:

    /**
     * \brief Type of source of an arc.
     */
    typedef enum
      {
	PLACE,
	TRANSITION
      } type_t;

    /**
     * \brief Type of places or transitions and arcs linked.
     */
    typedef std::map<identifier_t, Arc*> infos_t;

  private:

    /**
     * \brief Type of a table of arcs.
     */
    typedef std::map<identifier_t, infos_t> arcs_map_t;

  protected:

    /**
     * \brief Source Petri net.
     */
    PNet* net_;

    /**
     * \brief Identifiers of places and transitions.
     */
    identifiers_t* identifiers_;

    /**
     * \brief Hierarchy of unfolded places.
     */
    unfold_hierarchy_t unfolded_places_;

    /**
     * \brief Hierarchy of unfolded transitions.
     */
    unfold_hierarchy_t unfolded_transitions_;

    /**
     * \brief Arcs going from a place to a transition (search by transition).
     */
    arcs_map_t pre_transitions_arcs_;

    /**
     * \brief Arcs coming to a place from a transition (search by transition).
     */
    arcs_map_t post_transitions_arcs_;

    /**
     * \brief Arcs going to a place from a transition (search by place).
     */
    arcs_map_t pre_places_arcs_;

    /**
     * \brief Arcs coming from a place to a transition (search by place).
     */
    arcs_map_t post_places_arcs_;

    /**
     * \brief Generate arcs maps.
     */
    void create_arcs ();

  public:

    /**
     * \brief Constructor.
     */
    arcs_t (PNet* const net,
	    identifiers_t& identifiers);

    /**
     * \brief Copy constructor.
     */
    arcs_t (const arcs_t& to_copy);

    /**
     * \brief Destructor.
     */
    ~arcs_t ();

    /**
     * \brief Assignment operator.
     */
    arcs_t& operator= (const arcs_t& to_copy);

    /**
     * \brief Get links going to place or transition identified by
     * "id".
     *
     * \param id The place or transition identifier.
     * \param type The type of object identified by "id".
     * \return The arcs going to the place or transition "id".
     */
    const infos_t pre (const identifier_t id,
		       const type_t type) const;

    /**
     * \brief Get links coming from place or transition identified by
     * "id".
     *
     * \param id The place or transition identifier.
     * \param type The type of object identified by "id".
     * \return The arcs coming from the place or transition "id".
     */
    const infos_t post (const identifier_t id,
			const type_t type) const;

    /**
     * \brief Get places hierarchy.
     *
     * \return Places hierarchy.
     */
    unfold_hierarchy_t& unfolded_places();

    /**
     * \brief Get transitions hierarchy.
     *
     * \return Transitions hierarchy.
     */
    unfold_hierarchy_t& unfolded_transitions();

  };

}

#endif //__CPN_DDD__DDD_UNFOLD__
