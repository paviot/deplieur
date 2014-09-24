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

#ifndef __CPN_DDD__DDD_COMPARE_BUBBLE__
#define __CPN_DDD__DDD_COMPARE_BUBBLE__

#include "ddd_common.hh"
#include "ddd_unfold.hh"
#include "ddd_operation.hh"
#include "ddd_compare_reference.hh"
#include <DDD.h>
#include <Hom.h>
#include <ext/hash_map>
#include <hashfunc.hh>

namespace cpn_ddd
{

  namespace homomorphisms
  {

    class bubble_sort_t : public StrongHom
    {
    protected:
      const std::vector<identifier_t>* order_;

    public:
      
      // order: last -> first
      bubble_sort_t(const std::vector<identifier_t>* order);

      GDDD phiOne() const;
      
      GHom phi(int var, int val) const;

      size_t hash() const;
      
      bool operator==(const StrongHom& s) const;

      _GHom* clone() const;

    };

    class bubble_sort_down_t : public StrongHom
    {
    protected:
      const std::vector<identifier_t>* order_;
      int var_;
      int val_;

    public:
      
      bubble_sort_down_t(const std::vector<identifier_t>* order,
			 int var,
			 int val);

      GDDD phiOne() const;
      
      GHom phi(int var, int val) const;

      size_t hash() const;
      
      bool operator==(const StrongHom& s) const;

      _GHom* clone() const;

    };

  }

  class ddd_compare_bubble_t : public virtual ddd_operation_t
  {
  protected:

    typedef std::map<identifier_t, int> modifiers_map_t;

    typedef std::map<identifier_t, modifiers_map_t> variables_map_t;

    typedef std::map<identifier_t, values_t> values_map_t;

    typedef std::map<identifier_t, identifier_t> renames_map_t;

  public:
    
    /**
     * \brief Constructor.
     */
    ddd_compare_bubble_t (net_t& net,
			     const classes_t& classes,
			     identifiers_t& identifiers);

    /**
     * \brief Destructor.
     */
    ~ddd_compare_bubble_t ();

    Hom convert_to_place (const unfold_ddd_t& place,
			  const unfold_ddd_t& transition,
			  const Arc* const arc) const;
    
    Hom convert_to_transition (const unfold_ddd_t& transition,
			       const unfold_ddd_t& place,
			       const Arc* const arc) const;
    
    void remove_places (net_t& net,
			bool marked) const;

    /*
    Hom convert_to_transition_temp (const unfold_ddd_t& transition,
				    const unfold_ddd_t& place,
				    const CFuncTerm& term) const;
    */

  protected :

    typedef __gnu_cxx::hash_map<const Arc*, Hom, arcs_hash_t> arcs_cache_t;
    mutable arcs_cache_t arcs_cache_;

    void convert_to_transition (const unfold_ddd_t& transition,
				const unfold_ddd_t& place,
				const CFuncTerm& term,
				variables_map_t& variables,
				values_map_t& values,
				values_map_t& elements,
				values_map_t& completes) const;

    void convert_to_place (const unfold_ddd_t& place,
			   const unfold_ddd_t& transition,
			   const CFuncTerm& term,
			   variables_map_t& variables,
			   values_map_t& values,
			   values_map_t& elements,
			   renames_map_t& renames) const;

    Hom convert_to_transition (const unfold_ddd_t& transition,
			       const unfold_ddd_t& place,
			       const CFuncTerm& term) const;
    
    Hom convert_to_place (const unfold_ddd_t& place,
			  const unfold_ddd_t& transition,
			  const CFuncTerm& term) const;
    
    Hom select_cst (const values_map_t& values) const;
    
    Hom select_var (const variables_map_t& variables,
		    const values_map_t& elements) const;

    Hom remove (const values_map_t& values) const;

    Hom remove (const variables_map_t& variables) const;

    Hom shift_dec (const variables_map_t& variables,
		   const values_map_t& elements) const;
    
    Hom rename (const variables_map_t& variables) const;

    Hom insert (const values_map_t& completes) const;

    Hom move (const variables_map_t& variables,
	      const values_map_t& completes) const;

    Hom rename (const renames_map_t& renames) const;

    Hom copy (const variables_map_t& variables,
	      const renames_map_t& renames) const;

    Hom remove (const renames_map_t& renames) const;

    Hom shift_inc (const variables_map_t& variables,
		   const values_map_t& elements) const;
    
  };

}

#endif //__CPN_DDD__DDD_COMPARE_BUBBLE__
