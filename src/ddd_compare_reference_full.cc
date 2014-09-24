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
 
#include "ddd_compare_reference_full.hh"

#include <limits>
#include <algorithm>

#include <PNet.h>
#include <Variable.h>

namespace cpn_ddd
{

  namespace homomorphisms
  {

    class add_hash_t
    {
    public:
    
      size_t value_;
    
      add_hash_t (const size_t value) : value_(value) {}

      void operator() (const value_t& v)
      {this->value_ ^= ddd::knuth32_hash(static_cast<size_t>(v));}

    };

    constant_t::constant_t (const DDD& value) :
      value_(value)
    {
      //clog << "constant_t" << endl;
    }
  
    GDDD
    constant_t::phiOne () const
    {
      return this->value_;
    }
  
    GHom
    constant_t::phi (int var, int val) const
    {
      return Hom(this->value_);
    }

    bool
    constant_t::operator==(const StrongHom& s) const
    {
      const constant_t* const other =  dynamic_cast<const constant_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->value_ == other->value_);
    }
  
    size_t
    constant_t::hash () const
    {
      return this->value_.set_hash();
    }

    select_cst_t::select_cst_t (const identifier_t variable,
				const values_t& values) :
      variable_(variable),
      values_(values)
    {
      //clog << "select_cst_t" << endl;
    }
  
    GDDD
    select_cst_t::phiOne () const
    {
      return DDD::top;
    }
  
    GHom
    select_cst_t::phi (int var, int val) const
    {
      if (this->variable_ == var)
	if (std::find(this->values_.begin(), this->values_.end(), val) == this->values_.end())
	  return Hom(DDD::null);
	else
	  return Hom(var, val, Hom::id);
      else
	return Hom(var, val, this);
    }

    bool
    select_cst_t::operator==(const StrongHom& s) const
    {
      const select_cst_t* const other =  dynamic_cast<const select_cst_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->variable_ == other->variable_)
	  && (this->values_ == other->values_);
    }
  
    size_t
    select_cst_t::hash () const
    {
      add_hash_t values_hash(0);
      std::for_each(this->values_.begin(), this->values_.end(), values_hash);
      return values_hash.value_
	^ ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    }

    /*
    select_var_valued_t::select_var_valued_t (const identifier_t variable,
					      const value_t value) :
      variable_(variable),
      value_(value)
    {
      //clog << "select_var_valued_t" << endl;
    }
  
    GDDD
    select_var_valued_t::phiOne () const
    {
      return DDD::top;
    }
  
    GHom
    select_var_valued_t::phi (int var, int val) const
    {
      if (this->variable_ == var)
	if (this->value_ == val)
	  return Hom(var, val, Hom::id);
	else
	  return Hom(DDD::null);
      else
	return Hom(var, val, this);
    }

    bool
    select_var_valued_t::operator==(const StrongHom& s) const
    {
      const select_var_valued_t* const other =  dynamic_cast<const select_var_valued_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->variable_ == other->variable_)
	  && (this->value_ == other->value_);
    }
  
    size_t
    select_var_valued_t::hash () const
    {
      return ddd::knuth32_hash(static_cast<size_t>(this->variable_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->value_));
    }
    */

    select_var_t::select_var_t (const identifier_t base_variable,
				const identifier_t comp_variable,
				const value_t shift,
				const values_t& elements) :
      base_variable_(base_variable),
      comp_variable_(comp_variable),
      shift_(shift),
      elements_(elements)
    {
      //clog << "select_var_t" << endl;
    }
  
    GDDD
    select_var_t::phiOne () const
    {
      return DDD::top;
    }
  
    GHom
    select_var_t::phi (int var, int val) const
    {
      if (this->base_variable_ == var)
	{
	  values_t values(1, get_succ(val, this->shift_, this->elements_));
	  return Hom(var, val, 
		     Hom (new select_cst_t(this->comp_variable_, values)));
	}
      else
	return Hom(var, val, this);
    }

    bool
    select_var_t::operator==(const StrongHom& s) const
    {
      const select_var_t* const other =  dynamic_cast<const select_var_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->base_variable_ == other->base_variable_)
	  && (this->comp_variable_ == other->comp_variable_)
	  && (this->shift_ == other->shift_)
	  && (this->elements_ == other->elements_);
    }
    
    size_t
    select_var_t::hash () const
    {
      add_hash_t elements_hash(0);
      std::for_each(this->elements_.begin(), this->elements_.end(), elements_hash);
      return elements_hash.value_
	^ ddd::knuth32_hash(static_cast<size_t>(this->base_variable_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->comp_variable_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->shift_));
    }

    remove_t::remove_t (const identifier_t variable) :
      variable_(variable)
    {
      //clog << "remove_t" << endl;
    }
  
    GDDD
    remove_t::phiOne () const
    {
      return DDD::one;
    }
  
    GHom
    remove_t::phi (int var, int val) const
    {
      if (this->variable_ == var)
	return Hom::id;
      else
	return Hom(var, val, this);
    }

    bool
    remove_t::operator==(const StrongHom& s) const
    {
      const remove_t* const other =  dynamic_cast<const remove_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->variable_ == other->variable_);
    }
  
    size_t
    remove_t::hash () const
    {
      return ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    }

    shift_t::shift_t (const identifier_t variable,
		      const value_t shift,
		      const values_t& elements) :
      variable_(variable),
      shift_(shift),
      elements_(elements)
    {
      //clog << "shift_t" << endl;
    }
  
    GDDD
    shift_t::phiOne () const
    {
      return DDD::top;
    }
  
    GHom
    shift_t::phi (int var, int val) const
    {
      if (this->variable_ == var)
	return Hom(var, get_succ(val, this->shift_, this->elements_), Hom::id);
      else
	return Hom(var, val, this);
    }

    bool
    shift_t::operator==(const StrongHom& s) const
    {
      const shift_t* const other =  dynamic_cast<const shift_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->variable_ == other->variable_)
	  && (this->shift_ == other->shift_)
	  && (this->elements_ == other->elements_);
    }
  
    size_t
    shift_t::hash () const
    {
      add_hash_t elements_hash(0);
      std::for_each(this->elements_.begin(), this->elements_.end(), elements_hash);
      return elements_hash.value_
	^ ddd::knuth32_hash(static_cast<size_t>(this->variable_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->shift_));
    }

    rename_t::rename_t (const identifier_t old_variable,
			const identifier_t new_variable) :
      old_variable_(old_variable),
      new_variable_(new_variable)
    {
      //clog << "rename_t" << endl;
    }
  
    GDDD
    rename_t::phiOne () const
    {
      return DDD::top;
    }
  
    GHom
    rename_t::phi (int var, int val) const
    {
      if (this->old_variable_ == var)
	return Hom(this->new_variable_, val, Hom::id);
      else
	return Hom(var, val, this);
    }

    bool
    rename_t::operator==(const StrongHom& s) const
    {
      const rename_t* const other =  dynamic_cast<const rename_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->old_variable_ == other->old_variable_)
	  && (this->new_variable_ == other->new_variable_);
    }
  
    size_t
    rename_t::hash () const
    {
      return ddd::knuth32_hash(static_cast<size_t>(this->old_variable_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->new_variable_));
    }

    insert_t::insert_t (const identifier_t before,
			const identifier_t variable,
			const values_t& values) :
      before_(before),
      variable_(variable),
      values_(values)
    {
      //clog << "insert_t" << endl;
    }
  
    GDDD
    insert_t::phiOne () const
    {
      if (this->values_.empty())
	return DDD::one;
      DDD result(this->variable_, *this->values_.begin(), DDD::one);
      for (values_t::const_iterator i = ++this->values_.begin();
	   i != this->values_.end();
	   ++i)
	result = DDD(this->variable_, *i, DDD::one)
	  + result;
      return result;
    }
  
    GHom
    insert_t::phi (int var, int val) const
    {
      if (this->before_ == var)
	{
	  if (this->values_.empty())
	    return Hom::id;
	  Hom result(this->variable_, *this->values_.begin(), Hom(var, val, Hom::id));
	  for (values_t::const_iterator i = ++this->values_.begin();
	       i != this->values_.end();
	       ++i)
	    result = Hom(this->variable_, *i, Hom(var, val, Hom::id))
	      + result;
	  return result;
	}
      else
	return Hom(var, val, this);
    }

    bool
    insert_t::operator==(const StrongHom& s) const
    {
      const insert_t* const other =  dynamic_cast<const insert_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->before_ == other->before_)
	  && (this->variable_ == other->variable_)
	  && (this->values_ == other->values_);
    }
  
    size_t
    insert_t::hash () const
    {
      add_hash_t values_hash(0);
      std::for_each(this->values_.begin(), this->values_.end(), values_hash);
      return values_hash.value_
	^ ddd::knuth32_hash(static_cast<size_t>(this->before_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    }

    /**
     * This constant can be either "min()" or "max()" as identifiers are
     * signed integers.
     *
     * I choosed "max" because if there is a change to unsigned
     * integers, it needs no change.
     */
    const identifier_t move_t::ONE_IDENTIFIER
    = numeric_limits<identifier_t>::max();

    move_t::move_t (const identifier_t before,
		    const identifier_t variable) :
      before_(before),
      variable_(variable)
    {
      //clog << "move_t" << endl;
    }

    GDDD
    move_t::phiOne () const
    {
      return DDD::top;
    }

    GHom
    move_t::phi (int var, int val) const
    {
      if (var == this->before_)
	return Hom(new up_t(var, val)) & Hom(new move_up_t(this->variable_));
      else if (var == this->variable_)
	return Hom(new move_down_t(this->before_, var, val));
      else
	return Hom(var, val, this);
    }
  
    size_t
    move_t::hash () const
    {
      return ddd::knuth32_hash(static_cast<size_t>(this->before_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    }
  
    bool
    move_t::operator== (const StrongHom& s) const
    {
      const move_t* const other = dynamic_cast<const move_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->variable_ == other->variable_)
	  && (this->before_ == other->before_);
    }

    const identifier_t move_up_t::ONE_IDENTIFIER
    = move_t::ONE_IDENTIFIER;

    move_up_t::move_up_t (const identifier_t variable) :
      variable_(variable)
    {
      //clog << "move_up_t" << endl;
    }

    GDDD
    move_up_t::phiOne () const
    {
      return DDD::top;
    }

    GHom
    move_up_t::phi (int var, int val) const
    {
      if (var == this->variable_)
	return Hom(var, val, Hom::id);
      else
	return Hom(new up_t(var, val)) & Hom(this);
    }
  
    size_t
    move_up_t::hash () const
    {
      return ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    }
  
    bool
    move_up_t::operator== (const StrongHom& s) const
    {
      const move_up_t* const other = dynamic_cast<const move_up_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->variable_ == other->variable_);
    }

    const identifier_t move_down_t::ONE_IDENTIFIER
    = move_t::ONE_IDENTIFIER;

    move_down_t::move_down_t (const identifier_t before,
			      const identifier_t variable,
			      const value_t value) :
      before_(before),
      variable_(variable),
      value_(value)
    {
      //clog << "move_down_t" << endl;
    }

    GDDD
    move_down_t::phiOne () const
    {
      if (this->before_ == this->ONE_IDENTIFIER)
	return DDD(this->variable_, this->value_, DDD::one);
      else
	return DDD::top;
    }

    GHom
    move_down_t::phi (int var, int val) const
    {
      if (var == this->before_)
	return Hom(this->variable_, this->value_, Hom(var, val, Hom::id));
      else
	return Hom(var, val, this);
    }

    size_t
    move_down_t::hash () const
    {    
      return ddd::knuth32_hash(static_cast<size_t>(this->before_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->variable_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->value_));
    }
  
    bool
    move_down_t::operator== (const StrongHom& s) const
    {
      const move_down_t* const other = dynamic_cast<const move_down_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->before_ == other->before_)
	  && (this->variable_ == other->variable_)
	  && (this->value_ == other->value_);
    }

    up_t::up_t (const identifier_t id,
		const value_t value) :
      id_(id),
      value_(value)
    {
      //clog << "up_t" << endl;
    }

    GDDD
    up_t::phiOne () const
    {
      return DDD::top;
    }

    GHom
    up_t::phi (int var, int val) const
    {
      return Hom(var, val, Hom(this->id_, this->value_, Hom::id));
    }
  
    size_t
    up_t::hash () const
    {
      return ddd::knuth32_hash(static_cast<size_t>(this->id_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->value_));
    }
  
    bool
    up_t::operator== (const StrongHom& s) const
    {
      const up_t* const other = dynamic_cast<const up_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->id_ == other->id_)
	  && (this->value_ == other->value_);
    }

    /**
     * This constant can be either "min()" or "max()" as identifiers are
     * signed integers.
     *
     * I choosed "max" because if there is a change to unsigned
     * integers, it needs no change.
     */
    const identifier_t copy_t::ONE_IDENTIFIER
    = numeric_limits<identifier_t>::max();

    copy_t::copy_t (const identifier_t before,
		    const identifier_t id,
		    const identifier_t rename) :
      before_(before),
      id_(id),
      rename_(rename)
    {
      //clog << "copy_t" << endl;
    }

    GDDD
    copy_t::phiOne () const
    {
      return DDD::top;
    }

    GHom
    copy_t::phi (int var, int val) const
    {
      if ((var == this->before_) && (var == this->id_))
	return Hom(this->rename_, val, Hom(var, val, Hom::id));
      if (var == this->before_)
	return Hom(new rename_t(this->id_, this->rename_)) & Hom(new copy_up_t(var, val, this->id_));
      else if (var == this->id_)
	return Hom(var, val, new copy_down_t(this->before_, this->rename_, val));
      else
	return Hom(var, val, this);
    }
  
    size_t
    copy_t::hash () const
    {
      return ddd::knuth32_hash(static_cast<size_t>(this->before_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->id_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->rename_));
    }
  
    bool
    copy_t::operator== (const StrongHom& s) const
    {
      const copy_t* const other = dynamic_cast<const copy_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->before_ == other->before_)
	  && (this->id_ == other->id_)
	  && (this->rename_ == other->rename_);
    }

    const identifier_t copy_down_t::ONE_IDENTIFIER
    = copy_t::ONE_IDENTIFIER;

    copy_down_t::copy_down_t (const identifier_t before,
			      const identifier_t rename,
			      const value_t value) :
      before_(before),
      rename_(rename),
      value_(value)
    {
      //clog << "copy_down_t" << endl;
    }

    GDDD
    copy_down_t::phiOne () const
    {
      if (this->before_ == this->ONE_IDENTIFIER)
	return DDD(this->rename_, this->value_, DDD::one);
      else
	return DDD::top;
    }

    GHom
    copy_down_t::phi (int var, int val) const
    {
      if (var == this->before_)
	return Hom(this->rename_, this->value_, Hom(var, val, Hom::id));
      else
	return Hom(var, val, this);
    }
  
    size_t
    copy_down_t::hash () const
    {
      return ddd::knuth32_hash(static_cast<size_t>(this->before_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->rename_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->value_));
    }
  
    bool
    copy_down_t::operator== (const StrongHom& s) const
    {
      const copy_down_t* const other = dynamic_cast<const copy_down_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->before_ == other->before_)
	  && (this->rename_ == other->rename_)
	  && (this->value_ == other->value_);
    }

    const identifier_t copy_up_t::ONE_IDENTIFIER
    = copy_t::ONE_IDENTIFIER;

    copy_up_t::copy_up_t (const identifier_t variable,
			  const value_t value,
			  const identifier_t id) :
      variable_(variable),
      value_(value),
      id_(id)
    {
      //clog << "copy_up_t" << endl;
    }

    GDDD
    copy_up_t::phiOne () const
    {
      return DDD::one;
    }

    GHom
    copy_up_t::phi (int var, int val) const
    {
      if (var == this->id_)
	return Hom(this->variable_, this->value_, Hom(var, val, Hom::id));
      else
	return Hom(new up_t(this->variable_, this->value_)) & Hom(new copy_up_t(var, val, this->id_));
    }
  
    size_t
    copy_up_t::hash () const
    {
      return ddd::knuth32_hash(static_cast<size_t>(this->variable_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->value_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->id_));
    }
  
    bool
    copy_up_t::operator== (const StrongHom& s) const
    {
      const copy_up_t* const other = dynamic_cast<const copy_up_t*>(&s);
      if (other == NULL)
	return false;
      else
	return (this->id_ == other->id_)
	  && (this->variable_ == other->variable_)
	  && (this->value_ == other->value_);
    }  

  }

  /*
   * compare_reference_t
   */

  ddd_compare_reference_full_t::ddd_compare_reference_full_t (net_t& net,
							      const classes_t& classes,
							      identifiers_t& identifiers) :
    ddd_operation_t (net, classes, identifiers),
    arcs_cache_()
  {}
  
  ddd_compare_reference_full_t::~ddd_compare_reference_full_t ()
  {}

  Hom
  ddd_compare_reference_full_t::select_cst (const values_map_t& values) const
  {
    Hom result(Hom::id);
    for (values_map_t::const_iterator i = values.begin();
	 i != values.end();
	 ++i)
      result = Hom(new homomorphisms::select_cst_t((*i).first, (*i).second))
	& result;
    return result;
  }
  
  Hom
  ddd_compare_reference_full_t::select_var (const variables_map_t& variables,
				       const values_map_t& elements) const
  {
    Hom result(Hom::id);
    variables_map_t::const_iterator i;
    values_map_t::const_iterator e;
    for (i = variables.begin(), e = elements.begin();
	 i != variables.end();
	 ++i, ++e)
      {
	modifiers_map_t::const_iterator first = (*i).second.begin();
	for (modifiers_map_t::const_iterator j = ++(*i).second.begin();
	     j != (*i).second.end();
	     ++j)
	  result = Hom(new homomorphisms::select_var_t
		       ((*first).first, (*j).first,
			get_succ((*first).first, (*j).second - (*first).first, (*e).second),
			(*e).second))
	    & result;
      }
    return result;
  }

  Hom
  ddd_compare_reference_full_t::remove (const values_map_t& values) const
  {
    Hom result(Hom::id);
    for (values_map_t::const_iterator i = values.begin();
	 i != values.end();
	 ++i)
      result = Hom(new homomorphisms::remove_t((*i).first))
	& result;
    return result;
  }

  Hom
  ddd_compare_reference_full_t::remove (const removes_t& removes) const
  {
    Hom result(Hom::id);
    for (removes_t::const_iterator i = removes.begin();
	 i != removes.end();
	 ++i)
      result = Hom(new homomorphisms::remove_t(*i))
	& result;
    return result;
  }

  Hom
  ddd_compare_reference_full_t::shift_inc (const variables_map_t& variables,
				      const values_map_t& elements) const
  {
    Hom result(Hom::id);
    variables_map_t::const_iterator i;
    values_map_t::const_iterator e;
    for (i = variables.begin(), e = elements.begin();
	 i != variables.end();
	 ++i, ++e)
      {
	modifiers_map_t::const_iterator first = (*i).second.begin();
	result = Hom(new homomorphisms::shift_t((*i).first, (*first).second, (*e).second))
	  & result;
      }
    return result;
  }

  Hom
  ddd_compare_reference_full_t::shift_dec (const variables_map_t& variables,
				      const values_map_t& elements) const
  {
    Hom result(Hom::id);
    variables_map_t::const_iterator i;
    values_map_t::const_iterator e;
    for (i = variables.begin(), e = elements.begin();
	 i != variables.end();
	 ++i, ++e)
      {
	modifiers_map_t::const_iterator first = (*i).second.begin();
	result = Hom(new homomorphisms::shift_t((*first).first, -(*first).second, (*e).second))
	  & result;
      }
    return result;
  }

  Hom
  ddd_compare_reference_full_t::rename (const variables_map_t& variables) const
  {
    Hom result(Hom::id);
    for (variables_map_t::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      {
	modifiers_map_t::const_iterator first = (*i).second.begin();
	result = Hom(new homomorphisms::rename_t((*first).first, (*i).first))
	  & result;
      }
    return result;
  }

  Hom
  ddd_compare_reference_full_t::insert (const values_map_t& completes) const
  {
    Hom result(Hom::id);
    for (values_map_t::const_reverse_iterator i = completes.rbegin();
	 i != completes.rend();
	 ++i)
      result = Hom(new homomorphisms::insert_t((*i).first+1, (*i).first, (*i).second))
	& result;
    return result;
  }
  
  Hom
  ddd_compare_reference_full_t::move (const variables_map_t& variables) const
  {
    Hom result(Hom::id);
    identifier_t last = homomorphisms::move_t::ONE_IDENTIFIER;
    for (variables_map_t::const_reverse_iterator i = variables.rbegin();
	 i != variables.rend();
	 ++i)
      {
	result = Hom(new homomorphisms::move_t(last, (*i).first))
	  & result;
	last = (*i).first;
      }
    return result;
  }

  Hom
  ddd_compare_reference_full_t::copy (const variables_map_t& variables) const
  {
    typedef std::map< identifier_t, unsigned int> occurences_t;
    Hom result(Hom::id);
    occurences_t occs;
    for (variables_map_t::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      ++occs[(*(*i).second.begin()).first];
    identifier_t last = homomorphisms::copy_t::ONE_IDENTIFIER;
    for (variables_map_t::const_reverse_iterator i = variables.rbegin();
         i != variables.rend();
         ++i)
      {
	modifiers_map_t::const_iterator first = (*i).second.begin();
	result = Hom(new homomorphisms::copy_t(last, (*first).first, (*i).first))
	  & result;
	if ((*occs.find((*first).first)).second == 1)
	  result = Hom(new homomorphisms::remove_t((*first).first))
	    & result;
	else
	  --(*occs.find((*first).first)).second;
	last = (*i).first;
      }
    return result;
  }

  /**
   * For each place, build a DDD corresponding to its marking.
   * If "marked", select place DDD - marking DDD,
   * else select place DDD * marking DDD.
   */
  void
  ddd_compare_reference_full_t::remove_places (net_t& net,
					       bool marked) const
  {
    ddds_t& places = net.places();
    for (ddds_t::iterator i = places.begin();
	 i != places.end();
	 ++i)
      {
	const Place* place = (*this->identifiers_.places().find((*i).first)).second;
	const Marking* marking = place->getMarking();
	if (marking != NULL)
	  {
	    const variable_order_t* const order = (*i).second.order();
	    const variable_order_t::infos_t infos = order->infos(this->classes_);
	    const variable_order_t::conversion_t conversion = order->reverse_order(this->classes_);
	    const variable_order_t::conversion_t order_full = order->order_full(this->classes_);
	    DDD& current_ddd = (*i).second.ddd();
	    DDD mark_ddd(DDD::one); // DDD of the mark.
	    const std::list<Mark>& marks = marking->Lst();
	    bool mark_first = true;
	    for (std::list<Mark>::const_iterator j = marks.begin();
		 j != marks.end();
		 ++j)
	      {
		// If domain is empty and mark has a multiplier, the
		// place is not coloured and marked.
		if ((((*j).getDomain() == NULL) || ((*j).getDomain()->Elts().size() == 0))
		    && ((*j).Mult() != 0))
		  {
		    if (marked)
		      current_ddd = DDD::null;
		    // Else, current_ddd is not modified.
		    break;
		  }
		// Get DDD of a mark :
		const std::list< std::vector<Element> >& elements = (*j).Elts();
		DDD one_mark_ddd(DDD::one);
		DDD previous_ddd(DDD::one);
		// Create mark DDD from the last variable of the DDD :
		for (variable_order_t::conversion_t::const_reverse_iterator position =
		       conversion.rbegin();
		     position != conversion.rend();
		     ++position)
		  {
		    bool one_mark_first = true;
		    // Find the element corresponding to the DDD variable :
		    std::list< std::vector<Element> >::const_iterator elt =
		      elements.begin();
		    for (variable_order_t::conversion_t::const_iterator k = order_full.begin();
			 k != order_full.end();
			 ++k)
		      {
			if ((*k).second == (*position).first)
			  break;
			++elt;
		      }
		    // For each element, add it to the mark DDD :
		    for (std::vector<Element>::const_iterator k = (*elt).begin();
			 k != (*elt).end();
			 ++k)
		      if (one_mark_first)
			{
			  one_mark_ddd = DDD ((*position).first, (*k).Id(), previous_ddd);
			  one_mark_first = false;
			}
		      else
			one_mark_ddd = one_mark_ddd + DDD((*position).first, (*k).Id(), previous_ddd);
		    previous_ddd = one_mark_ddd;
		  }
		// Add mark DDDs :
		if (mark_first)
		  {
		    mark_ddd = one_mark_ddd;
		    mark_first = false;
		  }
		else
		  mark_ddd = mark_ddd + one_mark_ddd;
	      }
	    // Compute result :
	    if (marked)
	      current_ddd = current_ddd - mark_ddd;
	    else
	      current_ddd = current_ddd * mark_ddd;
	  }
      }
  }

  /**
   * For each element in term, if it is a variable, find
   * correspondance between place DDD and transition DDD, if it is a
   * constant, put it in the constants to select.
   */
  void
  ddd_compare_reference_full_t::convert_to_transition (const unfold_ddd_t& transition,
						       const unfold_ddd_t& place,
						       const CFuncTerm& term,
						       variables_map_t& variables,
						       values_map_t& values,
						       values_map_t& elements,
						       values_map_t& completes,
						       removes_t& removes) const
  {
    const variable_order_t* const place_order = place.order();
    const variable_order_t::conversion_t place_conversion = place_order->order_full(this->classes_);	
    const variable_order_t* const transition_order = transition.order();
    const variable_order_t::infos_t transition_infos = transition_order->infos(this->classes_);
    const variable_order_t::conversion_t transition_conversion = transition_order->order(this->classes_);
    const std::list<CFuncElt>& elts = term.Elts();
    std::list<CFuncElt>::const_iterator i;
    variable_order_t::conversion_t::const_iterator p_i;
    for (i = elts.begin(), p_i = place_conversion.begin();
	 i != elts.end();
	 ++i, ++p_i)
      {
	if ((*p_i).second != variable_order_t::UNKNOWN_POSITION)
	  if ((*i).IsVar())
	    {
	      const std::list< std::pair<Variable*, int> >& vars = (*i).getVars();
	      for (std::list< std::pair<Variable*, int> >::const_iterator j = vars.begin();
		   j != vars.end();
		   ++j)
		{
		  const PNClass* const vclass = (*j).first->PClass();
		  if (std::find(this->classes_.begin(), this->classes_.end(), vclass) != this->classes_.end())
		    {
		      const variable_order_t::variable_id_t id = (*j).first->Id();
		      variable_order_t::infos_t::const_iterator k;
		      variable_order_t::conversion_t::const_iterator pos;
		      for (k = transition_infos.begin(),
			     pos = transition_conversion.begin();
			   k != transition_infos.end();
			   ++k, ++pos)
			if ((*k).id_ == id)
			  {
			    variables[(*pos).second][(*p_i).second] = (*j).second;
			    elements[(*pos).second] = this->net_.extract_cached(vclass->Elts());
			    break;
			  }
		    }
		}
	    }
	  else
	    values[(*p_i).second] = this->net_.extract((*i).Elts());
      }
    // Add transition variables not bound to this arc :
    variable_order_t::conversion_t::const_iterator t_c;
    variable_order_t::infos_t::const_iterator t_i;
    for (t_c = transition_conversion.begin(), t_i = transition_infos.begin();
	 t_i != transition_infos.end();
	 ++t_c, ++t_i)
      if (variables.find((*t_c).second) == variables.end())
	completes.insert(make_pair((*t_c).second, this->net_.extract_cached((*t_i).class_->Elts())));
    // Add removed variables :
    for (variables_map_t::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      for (modifiers_map_t::const_iterator j = ++(*i).second.begin();
	   j != (*i).second.end();
	   ++j)
	removes.push_back((*j).first);
  }

  /**
   * For each element in term, if it is a variable, find
   * correspondance between place DDD and transition DDD, if it is a
   * constant, put it in the constants to select.
   */
  void
  ddd_compare_reference_full_t::convert_to_place (const unfold_ddd_t& place,
						  const unfold_ddd_t& transition,
						  const CFuncTerm& term,
						  variables_map_t& variables,
						  values_map_t& values,
						  values_map_t& elements,
						  removes_t& removes) const
  {
    const variable_order_t* const place_order = place.order();
    const variable_order_t::conversion_t place_conversion = place_order->order_full(this->classes_);	
    const variable_order_t* const transition_order = transition.order();
    const variable_order_t::infos_t transition_infos = transition_order->infos(this->classes_);
    const variable_order_t::conversion_t transition_conversion = transition_order->order(this->classes_);
    const std::list<CFuncElt>& elts = term.Elts();
    // All DDD variables must be removed at beginning :
    for (variable_order_t::conversion_t::const_iterator i = transition_conversion.begin();
	 i != transition_conversion.end();
	 ++i)
      removes.push_back((*i).second);
    //
    std::list<CFuncElt>::const_iterator i;
    variable_order_t::conversion_t::const_iterator p_i;
    for (i = elts.begin(), p_i = place_conversion.begin();
	 i != elts.end();
	 ++i, ++p_i)
      {
	if ((*p_i).second != variable_order_t::UNKNOWN_POSITION)
	  if ((*i).IsVar())
	    {
	      const std::list< std::pair<Variable*, int> >& vars = (*i).getVars();
	      for (std::list< std::pair<Variable*, int> >::const_iterator j = vars.begin();
		   j != vars.end();
		   ++j)
		{
		  const PNClass* const vclass = (*j).first->PClass();
		  if (std::find(this->classes_.begin(), this->classes_.end(), vclass) != this->classes_.end())
		    {
		      const variable_order_t::variable_id_t id = (*j).first->Id();
		      variable_order_t::infos_t::const_iterator k;
		      variable_order_t::conversion_t::const_iterator pos;
		      for (k = transition_infos.begin(),
			     pos = transition_conversion.begin();
			   k != transition_infos.end();
			   ++k, ++pos)
			if ((*k).id_ == id)
			  {
			    variables[(*p_i).second][(*pos).second] = (*j).second;
			    removes_t::iterator r = std::find(removes.begin(), removes.end(), (*pos).second);
			    if (r != removes.end())
			      removes.erase(r);
			    elements[(*p_i).second] = this->net_.extract_cached(vclass->Elts());
			    break;
			  }
		    }
		}
	    }
	  else// if ((*p_i).second != variable_order_t::UNKNOWN_POSITION)
	    values[(*p_i).second] = this->net_.extract((*i).Elts());
      }
    
  }

  Hom
  ddd_compare_reference_full_t::convert_to_transition (const unfold_ddd_t& transition,
						  const unfold_ddd_t& place,
						  const CFuncTerm& term) const
  {
    variables_map_t variables;
    values_map_t values;
    values_map_t elements;
    values_map_t completes;
    removes_t removes;
    this->convert_to_transition(transition, place, term,
				variables, values, elements, completes, removes);
    return this->insert(completes)
      & this->move(variables)
      & this->rename(variables)
      & this->shift_dec(variables, elements)
      & this->remove(removes)
      & this->select_var(variables, elements)
      & this->remove(values)
      & this->select_cst(values);
  }

  Hom
  ddd_compare_reference_full_t::convert_to_place (const unfold_ddd_t& place,
						  const unfold_ddd_t& transition,
						  const CFuncTerm& term) const
  {
    variables_map_t variables;
    values_map_t completes;
    values_map_t elements;
    removes_t removes;
    this->convert_to_place(place, transition, term,
			   variables, completes, elements, removes);
    return this->insert(completes)
      & this->shift_inc(variables, elements)
      & this->copy(variables)
      & this->remove(removes);
  }

  Hom
  ddd_compare_reference_full_t::convert_to_transition (const unfold_ddd_t& transition,
						  const unfold_ddd_t& place,
						  const Arc* const arc) const
  {
    arcs_cache_t::iterator z = this->arcs_cache_.find(arc);
    if (z != this->arcs_cache_.end())
      return (*z).second;
    const std::list<CFuncTerm> terms = arc->Valuation().Lst();
    Hom result(new homomorphisms::constant_t(DDD::null));
    for (std::list<CFuncTerm>::const_iterator i = terms.begin();
	 i != terms.end();
	 ++i)
      result = this->convert_to_transition(transition, place, *i)
	+ result;
    this->arcs_cache_.insert(make_pair(arc, result));
    return result;
  }
 
  Hom
  ddd_compare_reference_full_t::convert_to_place (const unfold_ddd_t& place,
					     const unfold_ddd_t& transition,
					     const Arc* const arc) const
  {
    arcs_cache_t::iterator z = this->arcs_cache_.find(arc);
    if (z != this->arcs_cache_.end())
      return (*z).second;
    const std::list<CFuncTerm> terms = arc->Valuation().Lst();
    Hom result(new homomorphisms::constant_t(DDD::null));
    for (std::list<CFuncTerm>::const_iterator i = terms.begin();
	 i != terms.end();
	 ++i)
      result = this->convert_to_place(place, transition, *i)
	+ result;
    this->arcs_cache_.insert(make_pair(arc, result));
    return result;
  }

}
