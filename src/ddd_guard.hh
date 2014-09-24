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

#ifndef __CPN_DDD__DDD_GUARD__
#define __CPN_DDD__DDD_GUARD__

#include <Guard.h>
#include <DDD.h>
#include <Hom.h>
#include "ddd_common.hh"
#include "ddd_unfold.hh"
#include "ddd_operation.hh"
#include "variable_order.hh"

namespace cpn_ddd
{

  /**
   * A traiter :
   * - OR // en dehors
   * - AND // en dehors
   * - NOT // en dehors
   * - VAR = VAR
   * - VAR != VAR
   * - VAR IN ...
   * - 
   * - TRUE
   * - FALSE
   */

  /**
   * \brief Constant homomorphism.
   *
   * This homomorphism always returns its value DDD.
   */
  class constant_t : public StrongHom
  {
  protected:

    /**
     * \brief DDD to return in phiOne and phi.
     */
    DDD value_;

  public:

    /**
     * \brief Constructor.
     *
     * \param value DDD to return.
     */
    constant_t (const DDD& value);
    
    ~constant_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;

    _GHom* clone() const;

  };

  /**
   * \brief Operation for comparisons.
   */
  typedef enum {equals, differs} operation_t;

  /**
   * \brief Homomorphism to select paths where values are in a domain.
   *
   * Only paths where value + succ is in domain are kept.
   */
  class guard_var_in_dom_t : public StrongHom
  {
  protected:

    /**
     * \brief Values in domain.
     */
    values_t domain_;

    /**
     * \brief Variable for which values must be checked.
     */
    identifier_t variable_;

    /**
     * \brief Modifier for value of variable.
     */
    value_t succ_;
    
    /**
     * \brief Values in colour class to compute vale + succ.
     */
    values_t values_;
    
    /**
     * \brief Check if value is in domain or not in domain.
     */
    operation_t operation_;
    
  public:

    /**
     * \brief Constructor.
     *
     * \param domain Values in domain
     * \param varaible Variable for which values must be checked
     * \param succ Modifier for value of variable
     * \param values Values in colour class to compute vale + succ
     * \param operation Check if value is in domain or not in domain
     */
    guard_var_in_dom_t (const values_t& domain,
			const identifier_t variable,
			const value_t succ,
			const values_t& values,
			const operation_t operation);
    
    ~guard_var_in_dom_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;

    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism to compare two variables.
   *
   * \warning This homomorphism is made to be used by another
   * homomorphism, not to be used directly.
   *
   * This homomorphism selects paths where arcs going from "variable"
   * are valued with "value".
   */
  class guard_var_compare_t : public StrongHom
  {
  protected:

    /**
     * \brief Variable to check.
     */
    identifier_t variable_;

    /**
     * \brief Value to keep in paths.
     */
    value_t value_;

    /**
     * \brief Operation to do : select paths where value is present or
     * select paths where value does not appear.
     */
    operation_t operation_;

  public:
    
    /**
     * \brief Constructor.
     *
     * \param variable Variable to check
     * \param value Value to keep in paths
     * \param operation Operation to do : select paths where value is
     * present or select paths where value does not appear
     */
    guard_var_compare_t (const identifier_t variable,
			 const value_t value,
			 const operation_t operation);

    ~guard_var_compare_t ();

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;

    _GHom* clone() const;
    
  };

  /**
   * \brief Homomorphism to compare two variables.
   *
   * This homomorphism keeps paths where value of variable1 + succ1 =
   * value of variable2 + succ2.
   */
  class guard_var_equals_t : public StrongHom
  {
  protected:

    /**
     * \brief First variable to compare.
     */
    identifier_t variable1_;

    /**
     * \brief Modifier for first variable.
     */
    value_t succ1_;
    
    /**
     * \brief Second variable to compare.
     */
    identifier_t variable2_;

    /**
     * \brief Modifier for second variable.
     */
    value_t succ2_;

    /**
     * \brief Values of colour class.
     */
    values_t values_;
    
    /**
     * \brief Operation to do : val1 + succ1 = val2 + succ2 or val1 +
     * succ1 != val2 + succ2.
     */
    operation_t operation_;

  public:
    
    /**
     * \brief Constructor.
     *
     * \param variable1 First variable to compare
     * \param succ1 Modifier for first variable
     * \param variable2 Second variable to compare
     * \param succ2 Modifier for second variable
     * \param values Values of colour class
     * \param operation Operation to do : val1 + succ1 = val2 + succ2
     * or val1 + succ1 != val2 + succ2
     */
    guard_var_equals_t (const identifier_t variable1,
			const value_t succ1,
			const identifier_t variable2,
			const value_t succ2,
			const values_t& values,
			const operation_t operation);

    ~guard_var_equals_t ();

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;
        
    _GHom* clone() const;

  };

  /**
   * \brief Operation to remove guards that are always "FALSE".
   */
  class ddd_guard_t : public ddd_operation_t
  {
  protected:
    
    static const char GUARD_OR;
    static const char GUARD_AND;
    static const char GUARD_NOT;
    static const char GUARD_VARIABLE;
    static const char GUARD_EQUALS;
    static const char GUARD_NOT_EQUALS;
    static const char GUARD_DOMAIN;
    static const char GUARD_HEAD;
    static const char GUARD_TRUE;
    static const char GUARD_FALSE;

    /**
     * \brief Variable order of current transition.
     */
    variable_order_t* order_;

    /**
     * \brief Get homomorphism corresponding to a guard.
     */
    const Hom eval_guard (const Guard* const guard,
			  bool not_modifier);

  public:

    /**
     * \brief Constructor.
     */
    ddd_guard_t (net_t& net,
		 classes_t& classes,
		 identifiers_t& identifiers);
    
    /**
     * \brief Destructor.
     */
    ~ddd_guard_t ();

    /**
     * \brief Apply operation.
     */
    void transform ();

  };

}

#endif //__CPN_DDD__DDD_GUARD__
