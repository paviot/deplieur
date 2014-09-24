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
 
#include "ddd_guard.hh"
#include <Guard.h>
#include <Variable.h>

#include "hashfunc.hh"

namespace cpn_ddd
{

  constant_t::constant_t (const DDD& value) :
    value_(value)
  {}

  constant_t::~constant_t ()
  {}

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

  size_t
  constant_t::hash () const
  {
    return this->value_.set_hash();
  }

  bool
  constant_t::operator== (const StrongHom& s) const
  {
    const constant_t* const other = dynamic_cast<const constant_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->value_ == other->value_);
  }

  _GHom*
  constant_t::clone() const
  {
    return new constant_t(*this);
  }

  guard_var_in_dom_t::guard_var_in_dom_t (const values_t& domain,
					  const identifier_t variable,
					  const value_t succ,
					  const values_t& values,
					  const operation_t operation) :
    domain_(domain),
    variable_(variable),
    succ_(succ),
    values_(values),
    operation_(operation)
  {}
    
  guard_var_in_dom_t::~guard_var_in_dom_t ()
  {}

  GDDD
  guard_var_in_dom_t::phiOne () const
  {
    return GDDD::one;
  }

  GHom
  guard_var_in_dom_t::phi (int var, int val) const
  {
    if (this->variable_ == var)
      {
	const value_t value = get_succ(val, this->succ_, this->values_);
	values_t::const_iterator i = std::find(this->domain_.begin(), this->domain_.end(), value);
	switch (this->operation_)
	  {
	  case equals:
	    {
	      if (i != this->domain_.end())
		return Hom(var, val, Hom::id);
	      else
		return Hom(DDD::null);
	    }
	    break;
	  case differs:
	    {
	      if (i == this->domain_.end())
		return Hom(DDD::null);
	      else
		return Hom(var, val, Hom::id);
	    }
	    break;
	  default:
	    return Hom(DDD::top);
	  }
      }
    else
      return Hom(var, val, this);
  }

  size_t
  guard_var_in_dom_t::hash () const
  {
    size_t result = 0;
    for (values_t::const_iterator i = this->domain_.begin();
	 i != this->domain_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i));
    for (values_t::const_iterator i = this->values_.begin();
	 i != this->values_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->succ_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->operation_));
    return result;
  }

  bool
  guard_var_in_dom_t::operator== (const StrongHom& s) const
  {
    const guard_var_in_dom_t* const other = dynamic_cast<const guard_var_in_dom_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->domain_ == other->domain_)
	&& (this->variable_ == other->variable_)
	&& (this->succ_ == other->succ_)
	&& (this->values_ == other->values_)
	&& (this->operation_ == other->operation_);
  }

  _GHom*
  guard_var_in_dom_t::clone() const
  {
    return new guard_var_in_dom_t(*this);
  }

  guard_var_compare_t::guard_var_compare_t (const identifier_t variable,
					    const value_t value,
					    const operation_t operation) :
    variable_(variable),
    value_(value),
    operation_(operation)
  {}

  guard_var_compare_t::~guard_var_compare_t ()
  {}

  GHom
  guard_var_compare_t::phi (int var, int val) const
  {
    if (var == this->variable_)
      {
	switch (this->operation_)
	  {
	  case equals:
	    if (val == this->value_)
	      return Hom(var, val, Hom::id);
	    else
	      return Hom(DDD::null);
	    break;
	  case differs:
	    if (val != this->value_)
	      return Hom(var, val, Hom::id);
	    else
	      return Hom(DDD::null);
	    break;
	  }
       }
    return Hom(var, val, this);
  }

  size_t
  guard_var_compare_t::hash () const
  {
    size_t result = 0;
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->value_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->operation_));    
    return result;
  }

  bool
  guard_var_compare_t::operator== (const StrongHom& s) const
  {
    const guard_var_compare_t* const other = dynamic_cast<const guard_var_compare_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->variable_ == other->variable_)
	&& (this->value_ == other->value_)
	&& (this->operation_ == other->operation_);
  }

  _GHom*
  guard_var_compare_t::clone() const
  {
    return new guard_var_compare_t(*this);
  }

  guard_var_equals_t::guard_var_equals_t (const identifier_t variable1,
					  const value_t succ1,
					  const identifier_t variable2,
					  const value_t succ2,
					  const values_t& values,
					  const operation_t operation) :
    variable1_(variable1),
    succ1_(succ1),
    variable2_(variable2),
    succ2_(succ2),
    values_(values),
    operation_(operation)
  {}

  guard_var_equals_t::~guard_var_equals_t ()
  {}

  GHom
  guard_var_equals_t::phi (int var, int val) const
  {
    if (var == this->variable1_)
      {
	const value_t value = get_succ(get_succ(val, this->succ1_, this->values_), -this->succ2_, this->values_);
	return Hom(var, val, guard_var_compare_t(this->variable2_, value, this->operation_));
      }
    else if (var == this->variable2_)
      {
	const value_t value = get_succ(get_succ(val, this->succ2_, this->values_), -this->succ1_, this->values_);
	return Hom(var, val, guard_var_compare_t(this->variable1_, value, this->operation_));
      }
    else
      return Hom(var, val, this);
  }

  size_t
  guard_var_equals_t::hash () const
  {
    size_t result = 0;
    for (values_t::const_iterator i = this->values_.begin();
	 i != this->values_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->variable1_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->variable2_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->succ1_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->succ2_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->operation_));
    return result;
  }
  
  bool
  guard_var_equals_t::operator== (const StrongHom& s) const
  {
    const guard_var_equals_t* const other = dynamic_cast<const guard_var_equals_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->variable1_ == other->variable1_)
	&& (this->succ1_ == other->succ1_)
	&& (this->variable2_ == other->variable2_)
	&& (this->succ2_ == other->succ2_)
	&& (this->values_ == other->values_)
	&& (this->operation_ == other->operation_);
  }
  
  _GHom*
  guard_var_equals_t::clone() const
  {
    return new guard_var_equals_t(*this);
  }

  const char ddd_guard_t::GUARD_OR = '+';
  const char ddd_guard_t::GUARD_AND = '.';
  const char ddd_guard_t::GUARD_NOT = '-';
  const char ddd_guard_t::GUARD_VARIABLE = 'v';
  const char ddd_guard_t::GUARD_EQUALS = '=';
  const char ddd_guard_t::GUARD_NOT_EQUALS = '!';
  const char ddd_guard_t::GUARD_DOMAIN = 'd';
  const char ddd_guard_t::GUARD_HEAD = 'h';
  const char ddd_guard_t::GUARD_TRUE = 'T';
  const char ddd_guard_t::GUARD_FALSE = 'F';

  const Hom
  ddd_guard_t::eval_guard (const Guard* const guard,
			   bool not_modifier)
  {
    Hom result;
    switch (guard->getOp())
      {
      case GUARD_OR:
	{
	  const Guard* const fg = guard->getFg();
	  const Guard* const fd = guard->getFd();
	  if (not_modifier)
	    result = eval_guard(fg, true) & eval_guard(fd, true);
	  else
	    result = eval_guard(fg, false) + eval_guard(fd, false);
	}
	break;
      case GUARD_AND:
	{
	  const Guard* const fg = guard->getFg();
	  const Guard* const fd = guard->getFd();
	  if (not_modifier)
	    result = eval_guard(fg, true) + eval_guard(fd, true);
	  else
	    result = eval_guard(fg, false) & eval_guard(fd, false);
	}
	break;
      case GUARD_NOT:
	{
	  const Guard* const fg = guard->getFg();
	  result = eval_guard(fg, !not_modifier);
	}
	break;
      case GUARD_VARIABLE:
	// NEVER ACCESSED
	break;
      case GUARD_EQUALS:
      case GUARD_NOT_EQUALS:
	{
	  const Guard* const fg = guard->getFg();
	  const Guard* const fd = guard->getFd();
	  const variable_order_t::variable_id_t id1 = fg->getVar()->Id();
	  const variable_order_t::variable_id_t id2 = fd->getVar()->Id();
	  const variable_order_t::infos_t infos = this->order_->infos(this->classes_);
	  const variable_order_t::conversion_t conversion = this->order_->order(this->classes_);
	  variable_order_t::conversion_t::const_iterator position1 = conversion.begin();
	  variable_order_t::conversion_t::const_iterator position2 = conversion.begin();
	  for (variable_order_t::infos_t::const_iterator i = infos.begin();
	       i != infos.end();
	       ++i, ++position1)
	    if (id1 == (*i).id_)
	      break;
	  for (variable_order_t::infos_t::const_iterator i = infos.begin();
	       i != infos.end();
	       ++i, ++position2)
	    if (id2 == (*i).id_)
	      break;
	  if ((position1 != conversion.end()) && (position2 != conversion.end()))
	    {
	      const values_t elements = this->net_.extract_cached(fg->getVar()->PClass()->Elts());
	      const value_t succ1 = fg->getSucc();
	      const value_t succ2 = fd->getSucc();
	      operation_t operation;
	      switch (guard->getOp())
		{
		case GUARD_EQUALS:
		  if (! not_modifier)
		    operation = equals;
		  else
		    operation = differs;
		  break;
		case GUARD_NOT_EQUALS:
		  if (! not_modifier)
		    operation = differs;
		  else
		    operation = equals;
		  break;
		default:
		  operation = equals; // Should not happen
		}
	      result = guard_var_equals_t((*position1).second, succ1,
					      (*position2).second, succ2,
					      elements, operation);
	    }
	  else
	    result = Hom::id;
	}
	break;
      case GUARD_DOMAIN:
	{
	  const variable_order_t::variable_id_t var_id = guard->getVar()->Id();
	  const variable_order_t::infos_t infos = this->order_->infos(this->classes_);
	  const variable_order_t::conversion_t conversion = this->order_->order(this->classes_);
	  variable_order_t::conversion_t::const_iterator position = conversion.begin();
	  for (variable_order_t::infos_t::const_iterator i = infos.begin();
	       i != infos.end();
	       ++i, ++position)
	    if (var_id == (*i).id_)
	      break;
	  if (position != conversion.end())
	    {
	      const value_t succ = guard->getSucc();
	      const values_t values = this->net_.extract_cached(guard->getVar()->PClass()->Elts());
	      const values_t elements = this->net_.extract_cached(guard->getSet());
	      operation_t operation;
	      if (not_modifier)
		operation = differs;
	      else
		operation = equals;
	      result = guard_var_in_dom_t(elements, (*position).second, succ, values, operation);
	    }
	  else
	    result = Hom::id;
	}
	break;
      case GUARD_HEAD:
	{
	  const Guard* const fg = guard->getFg();
	  result = eval_guard(fg, not_modifier);
	}
	break;
      case GUARD_TRUE:
	{
	  if (not_modifier)
	    result = constant_t(DDD::null);
	  else
	    result = Hom::id;
	}
	break;
      case GUARD_FALSE:
	{
	  if (not_modifier)
	    result = Hom::id;
	  else
	    result = constant_t(DDD::null);
	}
	break;
      default:
	break;
      }
    return result;
  }
  
  ddd_guard_t::ddd_guard_t (net_t& net,
			    classes_t& classes,
			    identifiers_t& identifiers) :
    ddd_operation_t(net, classes, identifiers),
    order_(NULL)
  {}

  ddd_guard_t::~ddd_guard_t()
  {}

  void
  ddd_guard_t::transform ()
  {
    ddds_t& transitions = this->net_.transitions();
    for (ddds_t::iterator i = transitions.begin();
	 i != transitions.end();
	 ++i)
      {
	this->order_ = (*i).second.order();
	const Transition* const transition = (*this->identifiers_.transitions().find((*i).first)).second;
	const Guard* const guard = transition->getGuard();
	Hom hom(eval_guard(guard, false));
	(*i).second.ddd() = hom((*i).second.ddd());
      }
  }

}
