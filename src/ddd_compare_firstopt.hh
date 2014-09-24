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

#ifndef __CPN_DDD__DDD_COMPARE_FIRSTOPT__
#define __CPN_DDD__DDD_COMPARE_FIRSTOPT__

#include "ddd_common.hh"
#include "ddd_unfold.hh"
#include "ddd_operation.hh"
#include <DDD.h>
#include <Hom.h>

namespace cpn_ddd
{

  /**
   * \brief Homomorphism to remove variables that do not have a good
   * value.
   *
   * \warning This homomorphism is used by another homomorphism. Do
   * NOT use it unless you really know how to use it.
   */
  class remove_variable_valued_t : public StrongHom
  {
  public:

    /**
     * \brief Association between a variable identifier and a variable
     * modifier (for example : "i++2" is represented as
     * <variable(i), 2>).
     */
    typedef std::map<identifier_t, value_t> differences_t;

    /**
     * \brief Association between a variable identifier and its state
     * (found means the variable must be deleted, not found means the
     * variable must be kept in DDD).
     */
    typedef std::set<identifier_t> deletes_t;

  protected:
    
    /**
     * \brief Modifiers for variables.
     */
    differences_t differences_;
    
    /**
     * \brief State (keep or delete) for variables.
     */
    deletes_t deletes_;

    /**
     * \brief Elements of colour class to compute values with
     * modifiers.
     */
    values_t elements_;

    /**
     * \brief Value of the variable, to compute values with modifiers.
     */
    value_t value_;

  public:

    /**
     * \brief Constructor.
     *
     * \param differences The differences map to compute values with
     * modifiers
     * \param deletes The state of each DDD variable (remove or keep)
     * \param elements The elemets of colour class to compute values
     * with modifiers
     * \param value The value of the arc variable on the current DDD
     * path, to compute values with modifiers.
     */
    remove_variable_valued_t (const differences_t& differences,
			      const deletes_t& deletes,
			      const values_t& elements,
			      const value_t value);

    ~remove_variable_valued_t ();

    GDDD phiOne () const;
   
    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;
    
    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism to remove paths where a value is not kept in
   * all the path.
   *
   * For example, transition T is defined on variable i and linked
   * with an arc <i, i++1>. The DDD looks like
   *  var0 --x-> var1 --y-> 1.
   * This homomorphism is done to make sure "y = x + 1" and to remove
   * var1 representing "y".
   */
  class remove_variable_t : public StrongHom
  {
  public:

    /**
     * \brief Association between a variable identifier and a variable
     * modifier (for example : "i++2" is represented as
     * <variable(i), 2>).
     */
    typedef remove_variable_valued_t::differences_t differences_t;

    /**
     * \brief Association between a variable identifier and its state
     * (found means the variable must be deleted, not found means the
     * variable must be kept in DDD).
     */
    typedef remove_variable_valued_t::deletes_t deletes_t;

  protected:
    
    /**
     * \brief Modifiers for variables.
     */
    differences_t differences_;

    /**
     * \brief State (keep or delete) for variables.
     */
    deletes_t deletes_;

    /**
     * \brief Elements of colour class to compute values with
     * modifiers.
     */
    values_t elements_;

    /**
     * \brief If true, add modifier, else substract.
     */
    bool positive_;

  public:

    /**
     * \brief Constructor.
     *
     * \param differences The differences map to compute values with
     * modifiers
     * \param deletes The state of each DDD variable (remove or keep)
     * \param elements The elemets of colour class to compute values
     * with modifiers
     */
    remove_variable_t (const differences_t& differences,
		       const deletes_t& deletes,
		       const values_t& elements,
		       const bool positive);

    ~remove_variable_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;
    
    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism to rename all variables.
   *
   * This homomoprhism can be used to rename all the variables of a
   * DDD, or only some names.
   */
  class rename_full_t : public StrongHom
  {
  public:
    
    /**
     * \brief Association between old and new name of variables.
     *
     * First identifier is old name, second is new name.
     */
    typedef std::map<identifier_t, identifier_t> names_t;

  protected:

    /**
     * \brief Translation table of variable names.
     */
    names_t names_;

  public:

    /**
     * \brief Constructor.
     *
     * If no identifier is found for a variable in translation table,
     * the name is unchanged.
     *
     * \param names The translation table for names.
     */
    rename_full_t (const names_t& names);

    /**
     * \brief Destructor.
     */
    ~rename_full_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;
    
    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism to insert values.
   *
   * This homomorphism creates a new variable before a specified
   * variable of the DDD and creates paths with values from this new
   * variable to next variable.
   */
  class complete_t : public StrongHom
  {
  public:

    /**
     * \brief Constant meaning to insert before the "1" terminal.
     */
    static const identifier_t ONE_IDENTIFIER;

  protected:

    /**
     * \brief Identifier of variable to insert values before.
     */
    identifier_t next_id_;

    /**
     * \brief Identifier of variable to create.
     */
    identifier_t id_;

    /**
     * \brief Values to insert.
     */
    values_t values_;
    
  public:

    /**
     * \brief Constructor.
     *
     * \param next_id Identifier of variable to insert new variable
     * before
     * \param id Identifier of new created variable
     * \param values Paths to create from new created variable to next
     * variable
     */
    complete_t (const identifier_t next_id,
		const identifier_t id,
		const values_t values);

    ~complete_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;    

    _GHom* clone() const;

  };

  /**
   * \brief Homomoprhism to move a variable.
   */
  class move_variable_t : public StrongHom
  {
  public:

    /**
     * \brief Constant meaning destination is the "1" terminal.
     */
    static const identifier_t ONE_IDENTIFIER;

  protected:

    /**
     * \brief Identifier of destination variable.
     */
    identifier_t before_;

    /**
     * \brief Identifier of variable to move.
     */
    identifier_t variable_;

  public:

    /**
     * \brief Constructor.
     *
     * \param before Identifier of destination variable.
     * \param variable Identifier of variable to move.
     */
    move_variable_t (const identifier_t before,
		     const identifier_t variable);

    ~move_variable_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;

    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism used by move_variable_t.
   *
   * \warning This homomorphism is not made to be used directly.
   * \warning This homomorphism has not been tested yet
   * \warning This homomorphism should not be used : it takes more
   * memory than move_variable_down_t.
   */
  class move_variable_up_t : public StrongHom
  {
  public:

    /**
     * \brief Constant meaning destination is the "1" terminal.
     */
    static const identifier_t ONE_IDENTIFIER;

  protected:

    /**
     * \brief Identifier of variable to move.
     */
    identifier_t variable_;

  public:

    /**
     * \brief Constructor.
     *
     * \param variable Identifier of variable to move.
     */
    move_variable_up_t (const identifier_t variable);

    ~move_variable_up_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;

    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism used by move_variable_t.
   *
   * \warning This homomorphism is not made to be used directly.
   */
  class move_variable_down_t : public StrongHom
  {
  protected:

    /**
     * \brief Identifier of destination variable.
     */
    identifier_t before_;

    /**
     * \brief Variable to add.
     */
    identifier_t variable_;

    /**
     * \brief Value of variable in the path.
     */
    value_t value_;

  public:

    /**
     * \brief Constructor.
     *
     * \param before Identifier of variable to add before
     * \param variable Identifier of variable to move
     * \param value Value of "variable" in the path
     */
    move_variable_down_t (const identifier_t before,
			  const identifier_t variable,
			  const value_t value);

    ~move_variable_down_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;

    _GHom* clone() const;

  };

  /**
   * \brief Generic Homomorphism to put a variable before another.
   */
  class up_t : public StrongHom
  {
  protected:

    /**
     * \brief Identifier of variable to push before.
     */
    identifier_t id_;
    
    /**
     * \brief Value of variable in the path.
     */
    value_t value_;

  public:

    /**
     * \brief Constructor.
     *
     * \param id Identifier of variable to push before
     * \param value Value of variable "id" in the path
     */
    up_t (const identifier_t id,
	  const value_t value);

    ~up_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;    

    _GHom* clone() const;

  };

  /**
   * \brief Generic homomorphism to rename the first variable in a
   * DDD.
   */
  class rename_t : public StrongHom
  {
  protected:

    /**
     * \brief New identifier of the variable.
     */
    identifier_t id_;
    
  public:

    /**
     * \brief Constructor.
     *
     * \param id The new identifier of the renamed variable.
     */
    rename_t (const identifier_t id);
    
    ~rename_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;    

    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism to copy a variable before another variable
   * and rename the copy.
   *
   * \warning It is much more efficient to copy the variable to the
   * terminals of the DDD, not to the root.
   */
  class copy_t : public StrongHom
  {
  public:

    /**
     * \brief Constant meaning the destination is before the "1"
     * terminal.
     */
    static const identifier_t ONE_IDENTIFIER;

  protected:

    /**
     * \brief Identifier of destination variable (before which to
     * copy).
     */
    identifier_t before_;

    /**
     * \brief Identifier of variable to copy.
     */
    identifier_t id_;

    /**
     * \brief Identifier of the copied variable.
     */
    identifier_t rename_;

    /**
     * \brief Modifier (like "2" in "i++2") of the copied variable.
     */
    value_t modifier_;

    /**
     * \brief Elements of the colour class to compute the modified
     * value of the copied variable.
     */
    values_t elements_;
    
  public:

    /**
     * \brief Constructor.
     *
     * \param before Identifier of destination variable
     * \param id Identifier of variable to copy
     * \param rename Identifier of the copied variable
     * \param modifier Modifier of the copied variable
     * \param elements Elements of the colour class to compute the
     * modified value of the copied variable
     */
    copy_t (const identifier_t before,
	    const identifier_t id,
	    const identifier_t rename,
	    const value_t modifier,
	    const values_t elements);

    ~copy_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;    

    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism used by copy_t.
   *
   * \warning This homomorphism is not made to be used directly.
   */
  class copy_down_t : public StrongHom
  {
  public:

    /**
     * \brief Constant meaning the destination is before the "1"
     * terminal.
     */
    static const identifier_t ONE_IDENTIFIER;

  protected:

    /**
     * \brief Identifier of destination variable (before which to
     * copy).
     */
    identifier_t before_;

    /**
     * \brief Identifier of the copied variable.
     */
    identifier_t rename_;

    /**
     * \brief Value of the variable to insert in the path.
     */
    value_t value_;
    
  public:

    /**
     * \brief Constructor.
     *
     * \param before Identifier of destination variable
     * \param rename Identifier of the copied variable
     * \param value Value of the variable to insert in the path
     */
    copy_down_t (const identifier_t before,
		 const identifier_t rename,
		 const value_t value);

    ~copy_down_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;    

    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism used by copy_t.
   *
   * \warning This homomorphism is not made to be used directly.
   * \warning This homomorphism should not be used : it takes more
   * memory than move_variable_down_t.
   */
  class copy_up_t : public StrongHom
  {
  public:

    /**
     * \brief Constant meaning the destination is before the "1"
     * terminal.
     */
    static const identifier_t ONE_IDENTIFIER;

  protected:

    /**
     * \brief Identifier of variable to push up.
     */
    identifier_t variable_;

    /**
     * \brief Value of variable to push up.
     */
    value_t value_;

    /**
     * \brief Identifier of variable to copy.
     */
    identifier_t id_;

    /**
     * \brief Modifier (like "2" in "i++2") of the copied variable.
     */
    value_t modifier_;

    /**
     * \brief Elements of the colour class to compute the modified
     * value of the copied variable.
     */
    values_t elements_;
    
  public:

    /**
     * \brief Constructor.
     *
     * \param variable Variable to push up
     * \param value Value of variable to push up
     * \param id Identifier of variable to copy
     * \param modifier Modifier of the copied variable
     * \param elements Elements of the colour class to compute the
     * modified value of the copied variable
     */
    copy_up_t (const identifier_t variable,
	       const value_t value,
	       const identifier_t id,
	       const value_t modifier,
	       const values_t elements);

    ~copy_up_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;    

    _GHom* clone() const;

  };

  /**
   * \brief Homomorphism to select variables by their values.
   *
   * A variable is kept in a path only if its value is found in the
   * values of the homomorphism.
   */
  class select_t : public StrongHom
  {
  public:

    /**
     * \brief Association between variable identifiers and their
     * accepted values.
     *
     * If a variable identifier is not found, then the path is kept.
     */
    typedef std::map<identifier_t, values_t> selects_map_t;

  protected:

    /**
     * \brief Association between variable identifiers and their
     * accepted values.
     *
     * If a variable identifier is not found, then the path is kept.
     */
    selects_map_t selects_;
    
  public:

    /**
     * \brief Constructor.
     *
     * \param selects The values accepted for each variable.
     * If a variable identifier is not found, then the path is kept.
     */
    select_t (const selects_map_t& selects);

    ~select_t ();

    GDDD phiOne () const;

    GHom phi (int var, int val) const;

    size_t hash () const;

    bool operator== (const StrongHom& s) const;    

    _GHom* clone() const;

  };

  /**
   * \brief Useful operations to compare a place and a transition.
   *
   * This class defines methods that can be used to do conversion
   * between a place DDD and a transition DDD linked by an arc.
   */
  class ddd_compare_firstopt_t : public virtual ddd_operation_t
  {
  public:
    
    /**
     * \brief Constructor.
     */
    ddd_compare_firstopt_t (net_t& net,
			    const classes_t& classes,
			    identifiers_t& identifiers,
			    arcs_t& arcs);

    /**
     * \brief Destructor.
     */
    ~ddd_compare_firstopt_t ();

  protected:

    /**
     * \brief Arcs of Petri net.
     */
    arcs_t& arcs_;

    typedef std::map<variable_order_t::position_t, int> modifiers_map_t;

    typedef std::map<variable_order_t::position_t, modifiers_map_t> variables_map_t;

    typedef std::map<variable_order_t::position_t, values_t> values_map_t;

    typedef std::map<identifier_t, identifier_t> renames_map_t;

    typedef select_t::selects_map_t selects_map_t;

    /**
     * \brief Remove marked or non-marked places from the symbolic
     * Petri net.
     *
     * \param net The symbolic Petri net.
     * \param marked If true, only non marked places are kept, else only
     * marked places.
     */
    void remove_places (net_t& net,
			bool marked);

    /**
     * \brief Remove from DDD unused variables.
     *
     * \param ddd The DDD to use.
     * \param selects The variables to select.
     */
    void select_variables (unfold_ddd_t& ddd,
			   selects_map_t& selects);

    /**
     * \brief Remove unused variables in the destination DDD.
     *
     * Variables in the destination place DDD that are not bound to
     * variables in source transition DDD are removed.
     *
     * \param source The source DDD
     * \param destination The destination DDD
     * \param variables The association between variables in input DDD
     * (the second part of the map) and output DDD (the first part of
     * the map).
     */
    void remove_variables (unfold_ddd_t& destination,
			   const unfold_ddd_t& source,
			   const variables_map_t& variables,
			   const bool positive);

    /**
     * \brief Reorder the variables of a DDD.
     *
     * \param ddd The DDD to reodrer.
     * \param variables The association between variables in input DDD
     * (the second part of the map) and output DDD (the first part of
     * the map).
     */
    void reorder_variables (unfold_ddd_t& ddd,
			    const variables_map_t& variables);
    
    /**
     * \brief Fill place DDD with source transition variables that do
     * not appear in the destination place DDD.
     *
     * \param destination The destination DDD
     * \param source The source DDD
     * \param variables The association between variables in input DDD
     * (the second part of the map) and output DDD (the first part of
     * the map).
     */
    void add_variables (unfold_ddd_t& destination,
			const unfold_ddd_t& source,
			const variables_map_t& variables);

    /**
     * \brief Add variables and values to a source transition DDD.
     *
     * \param destination The destination DDD.
     * \param variables The association between variables in input DDD
     * (the second part of the map) and output DDD (the first part of
     * the map).
     * \param values The values to add.
     * \param renames The names conversion to use.
     */
    void add_variables (unfold_ddd_t& destination,
			const variables_map_t& variables,
			const values_map_t& values,
			const renames_map_t& renames);

    /**
     * \brief Rename variables of a DDD.
     *
     * \param ddd The DDD to use.
     * \param variables The association between variables in input DDD
     * (the second part of the map) and output DDD (the first part of
     * the map).
     */
    void rename_variables (unfold_ddd_t& ddd,
			   const variables_map_t& variables);

    /**
     * \brief Rename variables of a DDD using temporary names.
     *
     * \param ddd The DDD to use.
     * \param variables The association between variables in input DDD
     * (the second part of the map) and output DDD (the first part of
     * the map).
     * \param renames An output table of <old name, new name>.
     *
     * \warning This methid may have to be modified if identifier_t is
     * unsigned or if identifiers are not always >= 0.
     */
    void rename_variables (unfold_ddd_t& ddd,
			   const variables_map_t& variables,
			   renames_map_t& renames);

    /**
     * \brief Get information for conversion from place to transition.
     *
     * \param transition The transition
     * \param place The place
     * \param selects The constants on arcs to use.
     * \param term The function term on the arc linking place to
     * transition.
     */
    
    void convert_to_transition (const unfold_ddd_t& transition,
				const unfold_ddd_t& place,
				variables_map_t& variables,
				selects_map_t& selects,
				const CFuncTerm& term);

    /**
     * \brief Get information for conversion from transition to place.
     *
     * \param transition The transition
     * \param place The place
     * \param term The function term on the arc linking transition to
     * place.
     */
    void convert_to_place (const unfold_ddd_t& transition,
			   const unfold_ddd_t& place,
			   variables_map_t& variables,
			   values_map_t& values,
			   const CFuncTerm& term);

  };

}

#endif //__CPN_DDD__DDD_COMPARE_FIRSTOPT__
