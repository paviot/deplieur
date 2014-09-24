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
 
#include "ddd_compare_firstopt.hh"

#include <limits>
#include "hashfunc.hh"
#include <Variable.h>

namespace cpn_ddd
{

  remove_variable_valued_t::remove_variable_valued_t (const differences_t& differences,
						      const deletes_t& deletes,
						      const values_t& elements,
						      const value_t value) :
    differences_(differences),
    deletes_(deletes),
    elements_(elements),
    value_(value)
  {}
  
  remove_variable_valued_t::~remove_variable_valued_t ()
  {}
  
  GDDD
  remove_variable_valued_t::phiOne () const
  {
    return DDD::one;
  }
  
  GHom
  remove_variable_valued_t::phi (int var, int val) const
  {
    const differences_t::const_iterator i = this->differences_.find(var);
    if (i != this->differences_.end())
      if (val == get_succ(this->value_, (*i).second, this->elements_))
	if (this->deletes_.find(var) != this->deletes_.end())
	  return Hom(this);
	else
	  return Hom(var, val, this);
      else
	return Hom(DDD::null);
    else
      if (this->deletes_.find(var) != this->deletes_.end())
	return Hom(this);
      else
	return Hom(var, val, this);
  }
  
  size_t
  remove_variable_valued_t::hash () const
  {
    size_t result = 0;
    for (differences_t::const_iterator i = this->differences_.begin();
	 i != this->differences_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>((*i).first)) 
	^ ddd::knuth32_hash(static_cast<size_t>((*i).second));
    for (deletes_t::const_iterator i = this->deletes_.begin();
	 i != this->deletes_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i));
    for (values_t::const_iterator i = this->elements_.begin();
	 i != this->elements_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->value_));
    return result;
  }
  
  bool
  remove_variable_valued_t::operator== (const StrongHom& s) const
  {
    const remove_variable_valued_t* const other = dynamic_cast<const remove_variable_valued_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->value_ == other->value_)
	&& (this->differences_ == other->differences_)
	&& (this->deletes_ == other->deletes_)
	&& (this->elements_ == other->elements_);
  }

  _GHom*
  remove_variable_valued_t::clone() const
  {
    return new remove_variable_valued_t(*this);
  }

  remove_variable_t::remove_variable_t (const differences_t& differences,
					const deletes_t& deletes,
					const values_t& elements,
					const bool positive) :
    differences_(differences),
    deletes_(deletes),
    elements_(elements),
    positive_(positive)
  {}
  
  remove_variable_t::~remove_variable_t ()
  {}
  
  GDDD
  remove_variable_t::phiOne () const
  {
    return DDD::one;
  }
  
  GHom
  remove_variable_t::phi (int var, int val) const
  {
    const differences_t::const_iterator i = this->differences_.find(var);
    if (i != this->differences_.end())
      {
	value_t value;
	if (this->positive_)
	  value = val;
	else
	  value = get_succ(val, -(*i).second, this->elements_);
	const remove_variable_valued_t* const new_hom =
	  new remove_variable_valued_t(this->differences_, this->deletes_, this->elements_, value);
	if (this->deletes_.find(var) != this->deletes_.end())
	  return Hom(new_hom);
	else
	  if (this->positive_)
	    return Hom(var, val, new_hom);
	  else
	    return Hom(var, get_succ(val, -(*i).second, this->elements_), new_hom);
      }
    else
      if (this->deletes_.find(var) != this->deletes_.end())
	return Hom(this);
      else
	return Hom(var, val, this);
  }
  
  size_t
  remove_variable_t::hash () const
  {
    size_t result = 0;
    for (differences_t::const_iterator i = this->differences_.begin();
	 i != this->differences_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>((*i).first)) 
	^ ddd::knuth32_hash(static_cast<size_t>((*i).second));
    for (deletes_t::const_iterator i = this->deletes_.begin();
	 i != this->deletes_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i));
    for (values_t::const_iterator i = this->elements_.begin();
	 i != this->elements_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i)); 
    return result;
  }
  
  bool
  remove_variable_t::operator== (const StrongHom& s) const
  {
    const remove_variable_t* const other = dynamic_cast<const remove_variable_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->differences_ == other->differences_)
	&& (this->deletes_ == other->deletes_)
	&& (this->elements_ == other->elements_)
	&& (this->positive_ == other->positive_);
  }

  _GHom*
  remove_variable_t::clone() const
  {
    return new remove_variable_t(*this);
  }

  rename_full_t::rename_full_t (const names_t& names) :
    names_(names)
  {}

  rename_full_t::~rename_full_t ()
  {}

  GDDD
  rename_full_t::phiOne () const
  {
    return DDD::one;
  }

  GHom
  rename_full_t::phi (int var, int val) const
  {
    const names_t::const_iterator i = this->names_.find(var);
    if (i != this->names_.end())
      return Hom((*i).second, val, this);
    else
      return Hom(var, val, this);
  }

  size_t
  rename_full_t::hash () const
  {
    size_t result = 0;
    for (names_t::const_iterator i = this->names_.begin();
	 i != this->names_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>((*i).first)) 
	^ ddd::knuth32_hash(static_cast<size_t>((*i).second));
    return result;
  }
  
  bool
  rename_full_t::operator== (const StrongHom& s) const
  {
    const rename_full_t* const other = dynamic_cast<const rename_full_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->names_ == other->names_);
  }

  _GHom*
  rename_full_t::clone() const
  {
    return new rename_full_t(*this);
  }

  /**
   * This constant can be either "min()" or "max()" as identifiers are
   * signed integers.
   *
   * I choosed "max" because if there is a change to unsigned
   * integers, it needs no change.
   */
  const identifier_t complete_t::ONE_IDENTIFIER
  = numeric_limits<identifier_t>::max();

  complete_t::complete_t (const identifier_t next_id,
			  const identifier_t id,
			  const values_t values) :
    next_id_(next_id),
    id_(id),
    values_(values)
  {}

  complete_t::~complete_t ()
  {}

  GDDD
  complete_t::phiOne () const
  {
    if (this->next_id_ == this->ONE_IDENTIFIER)
      {

	values_t::const_iterator j = this->values_.begin();
	DDD to_add (this->id_, *j, DDD::one);
	while (++j != this->values_.end())
	  to_add = to_add + DDD(this->id_, *j, DDD::one);
	return to_add;
      }
    else
      return DDD::one;
  }

  GHom
  complete_t::phi (int var, int val) const
  {
    if (var == this->next_id_)
      {
	values_t::const_iterator j = this->values_.begin();
	Hom to_apply(this->id_, *j, Hom(var, val, Hom::id));
	while (++j != this->values_.end())
	  to_apply = to_apply + Hom(this->id_, *j, Hom(var, val, Hom::id));
	return to_apply;
      }
    else
      return Hom(var, val, this);
  }

  size_t
  complete_t::hash () const
  {
    size_t result = 0;
    for (values_t::const_iterator i = this->values_.begin();
	 i != this->values_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->next_id_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->id_));
    return result;
  }
  
  bool
  complete_t::operator== (const StrongHom& s) const
  {
    const complete_t* const other = dynamic_cast<const complete_t*>(&s);
    if (other == NULL)
      return false;
    return (this->next_id_ == other->next_id_)
      && (this->id_ == other->id_)
      && (this->values_ == other->values_);
  }

  _GHom*
  complete_t::clone() const
  {
    return new complete_t(*this);
  }

  /**
   * This constant can be either "min()" or "max()" as identifiers are
   * signed integers.
   *
   * I choosed "max" because if there is a change to unsigned
   * integers, it needs no change.
   */
  const identifier_t move_variable_t::ONE_IDENTIFIER
  = numeric_limits<identifier_t>::max();

  move_variable_t::move_variable_t (const identifier_t before,
				    const identifier_t variable) :
    before_(before),
    variable_(variable)
  {}

  move_variable_t::~move_variable_t ()
  {}

  GDDD
  move_variable_t::phiOne () const
  {
    return DDD::top;
  }

  GHom
  move_variable_t::phi (int var, int val) const
  {
    if (var == this->before_)
      return Hom(new up_t(var, val)) & Hom(new move_variable_up_t(this->variable_));
    else if (var == this->variable_)
      return Hom(new move_variable_down_t(this->before_, var, val));
    else
      return Hom(var, val, this);
  }
  
  size_t
  move_variable_t::hash () const
  {
    size_t result = 0;
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->before_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    return result;
  }
  
  bool
  move_variable_t::operator== (const StrongHom& s) const
  {
    const move_variable_t* const other = dynamic_cast<const move_variable_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->variable_ == other->variable_)
	&& (this->before_ == other->before_);
  }

  _GHom*
  move_variable_t::clone() const
  {
    return new move_variable_t(*this);
  }

  const identifier_t move_variable_up_t::ONE_IDENTIFIER
  = move_variable_t::ONE_IDENTIFIER;

  move_variable_up_t::move_variable_up_t (const identifier_t variable) :
    variable_(variable)
  {}

  move_variable_up_t::~move_variable_up_t ()
  {}

  GDDD
  move_variable_up_t::phiOne () const
  {
    return DDD::top;
  }

  GHom
  move_variable_up_t::phi (int var, int val) const
  {
    if (var == this->variable_)
      return Hom(var, val, Hom::id);
    else
      return Hom(new up_t(var, val)) & Hom(this);
  }
  
  size_t
  move_variable_up_t::hash () const
  {
    return ddd::knuth32_hash(static_cast<size_t>(this->variable_));
  }
  
  bool
  move_variable_up_t::operator== (const StrongHom& s) const
  {
    const move_variable_up_t* const other = dynamic_cast<const move_variable_up_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->variable_ == other->variable_);
  }

  _GHom*
  move_variable_up_t::clone() const
  {
    return new move_variable_up_t(*this);
  }

  move_variable_down_t::move_variable_down_t (const identifier_t before,
					      const identifier_t variable,
					      const value_t value) :
    before_(before),
    variable_(variable),
    value_(value)
  {}

  move_variable_down_t::~move_variable_down_t ()
  {}

  GDDD
  move_variable_down_t::phiOne () const
  {
    return DDD(this->variable_, this->value_, DDD::one);
  }

  GHom
  move_variable_down_t::phi (int var, int val) const
  {
    if (var == this->before_)
      return Hom(this->variable_, this->value_, Hom(var, val, Hom::id));
    else
      return Hom(var, val, this);
  }

  size_t
  move_variable_down_t::hash () const
  {    
    size_t result = 0;
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->before_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->value_));
    return result;
  }
  
  bool
  move_variable_down_t::operator== (const StrongHom& s) const
  {
    const move_variable_down_t* const other = dynamic_cast<const move_variable_down_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->before_ == other->before_)
	&& (this->variable_ == other->variable_)
	&& (this->value_ == other->value_);
  }

  _GHom*
  move_variable_down_t::clone() const
  {
    return new move_variable_down_t(*this);
  }

  up_t::up_t (const identifier_t id,
	      const value_t value) :
    id_(id),
    value_(value)
  {}

  up_t::~up_t ()
  {}

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
    size_t result = 0;
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->id_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->value_));
    return result;
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

  _GHom*
  up_t::clone() const
  {
    return new up_t(*this);
  }

  rename_t::rename_t (const identifier_t id) :
    id_(id)
  {}

  rename_t::~rename_t ()
  {}

  GDDD
  rename_t::phiOne () const
  {
    return DDD::top;
  }

  GHom
  rename_t::phi (int var, int val) const
  {
    return Hom(this->id_, val, Hom::id);
  }
  
  size_t
  rename_t::hash () const
  {
    return ddd::knuth32_hash(static_cast<size_t>(this->id_));
  }
  
  bool
  rename_t::operator== (const StrongHom& s) const
  {
    const rename_t* const other = dynamic_cast<const rename_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->id_ == other->id_);
  }

  _GHom*
  rename_t::clone() const
  {
    return new rename_t(*this);
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
		  const identifier_t rename,
		  const value_t modifier,
		  const values_t elements) :
    before_(before),
    id_(id),
    rename_(rename),
    modifier_(modifier),
    elements_(elements)
  {}

  copy_t::~copy_t ()
  {}

  GDDD
  copy_t::phiOne () const
  {
    return DDD::top;
  }

  GHom
  copy_t::phi (int var, int val) const
  {
    if ((var == this->before_) && (var == this->id_))
      return Hom(this->rename_, get_succ(val, this->modifier_, this->elements_), Hom(var, val, Hom::id));
    if (var == this->before_)
      return Hom(new rename_t(this->rename_)) & Hom(new copy_up_t(var, val, this->id_, this->modifier_, this->elements_));
    else if (var == this->id_)
      return Hom(var, val, new copy_down_t(this->before_, this->rename_, get_succ(val, this->modifier_, this->elements_)));
    else
      return Hom(var, val, this);
  }
  
  size_t
  copy_t::hash () const
  {
    size_t result = 0;
    for (values_t::const_iterator i = this->elements_.begin();
	 i != this->elements_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->before_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->id_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->rename_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->modifier_));
    return result;
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
	&& (this->rename_ == other->rename_)
	&& (this->modifier_ == other->modifier_)
	&& (this->elements_ == other->elements_);
  }

  _GHom*
  copy_t::clone() const
  {
    return new copy_t(*this);
  }

  const identifier_t copy_down_t::ONE_IDENTIFIER
  = copy_t::ONE_IDENTIFIER;

  copy_down_t::copy_down_t (const identifier_t before,
			    const identifier_t rename,
			    const value_t value) :
    before_(before),
    rename_(rename),
    value_(value)
  {}

  copy_down_t::~copy_down_t ()
  {}

  GDDD
  copy_down_t::phiOne () const
  {
    if (this->before_ == this->ONE_IDENTIFIER)
      return DDD(this->rename_, this->value_, DDD::one);
    else
      return DDD::one;
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
    size_t result = 0;
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->before_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->rename_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->value_));
    return result;
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

  _GHom*
  copy_down_t::clone() const
  {
    return new copy_down_t(*this);
  }

  const identifier_t copy_up_t::ONE_IDENTIFIER
  = copy_t::ONE_IDENTIFIER;

  copy_up_t::copy_up_t (const identifier_t variable,
			const value_t value,
			const identifier_t id,
			const identifier_t modifier,
			const values_t elements) :
    variable_(variable),
    value_(value),
    id_(id),
    modifier_(modifier),
    elements_(elements)
  {}

  copy_up_t::~copy_up_t ()
  {}

  GDDD
  copy_up_t::phiOne () const
  {
    return DDD::one;
  }

  GHom
  copy_up_t::phi (int var, int val) const
  {
    if (var == this->id_)
      return Hom(var, get_succ(val, this->modifier_, this->elements_), Hom(this->variable_, this->value_, Hom(var, val, Hom::id)));
    else
      return Hom(new up_t(this->variable_, this->value_)) & Hom(new copy_up_t(var, val, this->id_, this->modifier_, this->elements_));
  }
  
  size_t
  copy_up_t::hash () const
  {
    size_t result = 0;
    for (values_t::const_iterator i = this->elements_.begin();
	 i != this->elements_.end();
	 ++i)
      result ^= ddd::knuth32_hash(static_cast<size_t>(*i));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->variable_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->value_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->id_));
    result ^= ddd::knuth32_hash(static_cast<size_t>(this->modifier_));
    return result;
  }
  
  bool
  copy_up_t::operator== (const StrongHom& s) const
  {
    const copy_up_t* const other = dynamic_cast<const copy_up_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->id_ == other->id_)
	&& (this->modifier_ == other->modifier_)
	&& (this->elements_ == other->elements_)
	&& (this->variable_ == other->variable_)
	&& (this->value_ == other->value_);
  }  

  _GHom*
  copy_up_t::clone() const
  {
    return new copy_up_t(*this);
  }

  select_t::select_t (const selects_map_t& selects) :
    selects_(selects)
  {}

  select_t::~select_t ()
  {}

  GDDD
  select_t::phiOne () const
  {
    return DDD::one;
  }

  GHom
  select_t::phi (int var, int val) const
  {
    const selects_map_t::const_iterator i = this->selects_.find(var);
    if (i != this->selects_.end())
      if (std::find((*i).second.begin(), (*i).second.end(), val) != (*i).second.end())
	return Hom(var, val, this);
      else
	return Hom(DDD::null);
    else
      return Hom(var, val, this);
  }
  
  size_t
  select_t::hash () const
  {
    size_t result = 0;
    for (selects_map_t::const_iterator i = this->selects_.begin();
	 i != this->selects_.end();
	 ++i)
      {
	size_t subresult = 0;
	for (values_t::const_iterator j = (*i).second.begin();
	     j != (*i).second.end();
	     ++j)
	  subresult ^= ddd::knuth32_hash(static_cast<size_t>(*j));
	result ^= ddd::knuth32_hash(static_cast<size_t>((*i).first)) ^ subresult;
      }
    return result;
  }
  
  bool
  select_t::operator== (const StrongHom& s) const
  {
    const select_t* const other = dynamic_cast<const select_t*>(&s);
    if (other == NULL)
      return false;
    else
      return (this->selects_ == other->selects_);
  }

  _GHom*
  select_t::clone() const
  {
    return new select_t(*this);
  }

  ddd_compare_firstopt_t::ddd_compare_firstopt_t (net_t& net,
						  const classes_t& classes,
						  identifiers_t& identifiers,
						  arcs_t& arcs) :
    ddd_operation_t (net, classes, identifiers),
    arcs_(arcs)
  {}

  ddd_compare_firstopt_t::~ddd_compare_firstopt_t ()
  {}

  /**
   * For each place, build a DDD corresponding to its marking.
   * If "marked", select place DDD - marking DDD,
   * else select place DDD * marking DDD.
   */
  void
  ddd_compare_firstopt_t::remove_places (net_t& net,
					 bool marked)
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

  void
  ddd_compare_firstopt_t::select_variables (unfold_ddd_t& ddd,
					    selects_map_t& selects)
  {
    DDD& ddd_ = ddd.ddd();
    Hom hom(new select_t(selects));
    ddd_ = hom(ddd_);
  }

  /**
   * Place P is C1*C2*C1, linked to transition T by arc <i, 0, i++1>.
   * i++1 is already bound and the only variable to keep in P is the
   * first.
   */
  void
  ddd_compare_firstopt_t::remove_variables (unfold_ddd_t& destination,
					    const unfold_ddd_t& source,
					    const variables_map_t& variables,
					    const bool positive)
  {
    const variable_order_t* const order = source.order();
    const variable_order_t::infos_t infos = order->infos(this->classes_);
    const variable_order_t::conversion_t conversion = destination.order()->order(this->classes_);
    DDD& ddd = destination.ddd();
    // Find variables to delete :
    remove_variable_t::deletes_t deletes_;
    for (variable_order_t::conversion_t::const_iterator i = conversion.begin();
	 i != conversion.end();
	 ++i)
      deletes_.insert((*i).second);
    for (variables_map_t::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      for (modifiers_map_t::const_iterator j = (*i).second.begin();
	   j != (*i).second.end();
	   ++j)
	deletes_.erase((*j).first);
    // If no conversion between variables has to be done, just delete
    // unused variables.
    if (variables.empty())
      {
	Hom hom(new remove_variable_t(remove_variable_t::differences_t(), deletes_, values_t(), positive));
	ddd = hom(ddd);
      }
    // Else, delete variables already copied and compute modifiers.
    else
      for (variables_map_t::const_iterator i = variables.begin();
	   i != variables.end();
	   ++i)
	{
	  remove_variable_t::differences_t differences;
	  remove_variable_t::deletes_t deletes = deletes_;
	  values_t elements = this->net_.extract_cached(infos[(*i).first].class_->Elts());
	  bool found = false;
	  for (modifiers_map_t::const_iterator j = (*i).second.begin();
	       j != (*i).second.end();
	       ++j)
	    {
	      differences.insert(make_pair((*j).first, (*j).second));
	      if (found)
		deletes.insert((*j).first);
	      else
		{
		  found = true;
		  deletes.erase((*j).first);
		}
	    }
	  Hom hom(new remove_variable_t(differences, deletes, elements, positive));
	  ddd = hom(ddd);
	}
  }

  /**
   * For each variable in the destination DDD, find its equivalent in
   * source DDD, move it to the end and rename it.
   * This operation is done starting from the last variable in
   * destination DDD.
   */
  void
  ddd_compare_firstopt_t::reorder_variables (unfold_ddd_t& ddd,
					     const variables_map_t& variables)
  {
    identifier_t destination = move_variable_t::ONE_IDENTIFIER;
    DDD& ddd_ = ddd.ddd();
    for (variables_map_t::const_reverse_iterator i = variables.rbegin();
	 i != variables.rend();
	 ++i)
      {
	const identifier_t source = (*(*i).second.begin()).first;
	Hom hom(new move_variable_t(destination, source));
	ddd_ = hom(ddd_);
	destination = source;
      }
  }

  /**
   * For each variable in transition, if a variable is not in the arc
   * to place, complete the place DDD with the full possible range for
   * this variable.
   */
  void
  ddd_compare_firstopt_t::add_variables (unfold_ddd_t& destination,
					 const unfold_ddd_t& source,
					 const variables_map_t& variables)
  {
    const variable_order_t* const order = source.order();
    const variable_order_t::infos_t infos = order->infos(this->classes_);
    const variable_order_t::conversion_t conversion = order->order(this->classes_);
    variables_map_t::const_iterator it = variables.begin();
    DDD& ddd = destination.ddd();
    variable_order_t::conversion_t::const_iterator i;
    variable_order_t::infos_t::const_iterator j;
    for (i = conversion.begin(), j = infos.begin();
	 j != infos.end();
	 ++i, ++j)
      {
	if (variables.find((*i).second) == variables.end())
	  {
	    identifier_t destination;
	    if (it == variables.end())
	      destination = complete_t::ONE_IDENTIFIER;
	    else
	      destination = (*it).first;
	    Hom hom(new complete_t(destination, (*i).second, this->net_.extract_cached((*j).class_->Elts())));
	    ddd = hom(ddd);
	  }
	else
	  ++it;
      }
  }

  /**
   * For each variable in table, copy the variable to its destination
   * and remove it.
   * For each value in table, put it in destination.
   */
  void
  ddd_compare_firstopt_t::add_variables (unfold_ddd_t& destination,
					 const variables_map_t& variables,
					 const values_map_t& values,
					 const renames_map_t& renames)
  {
    const variable_order_t* const order = destination.order();
    const variable_order_t::infos_t infos = order->infos(this->classes_);
    remove_variable_t::deletes_t deletes;
    DDD& ddd = destination.ddd();
    identifier_t last = copy_t::ONE_IDENTIFIER;
    variables_map_t::const_reverse_iterator i;
    values_map_t::const_reverse_iterator j;
    for (i = variables.rbegin(), j = values.rbegin();
	 (i != variables.rend()) || (j != values.rend());)
      {
	bool use_value;
	if ((i != variables.rend()) && (j != values.rend()))
	  use_value = ((*j).first > (*i).first);
	else if (i != variables.rend())
	  use_value = false;
	else //(j != values.rend())
	  use_value = true;
	Hom hom;
	identifier_t name;
	if (use_value)
	  {
	    name = (*j).first;
	    hom = new complete_t(last, name,  (*j).second);
	  }
	else
	  {
	    name = (*i).first;
	    const identifier_t to_copy = (*(*i).second.begin()).first;
	    const int modifier = (*(*i).second.begin()).second;
	    const identifier_t rename = (*renames.find(to_copy)).second;
	    hom = new copy_t(last, rename, name, modifier, this->net_.extract_cached(infos[to_copy].class_->Elts()));
	    deletes.insert(rename);
	  }
	ddd = hom(ddd);
	deletes.erase(name);
	last = name;
	if (use_value)
	  ++j;
	else
	  ++i;
      }
    Hom delete_hom(new remove_variable_t(remove_variable_t::differences_t(), deletes, values_t(), true));
    ddd = delete_hom(ddd);
  }

  void
  ddd_compare_firstopt_t::rename_variables (unfold_ddd_t& ddd,
					    const variables_map_t& variables)
  {
    rename_full_t::names_t names;
    for (variables_map_t::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      names[(*(*i).second.begin()).first] = (*i).first;
    Hom hom(new rename_full_t(names));
    DDD& ddd_ = ddd.ddd();
    ddd_ = hom(ddd_);
  }

  void
  ddd_compare_firstopt_t::rename_variables (unfold_ddd_t& place,
					    const variables_map_t& variables,
					    renames_map_t& renames)
  {
    rename_full_t::names_t names;
    for (variables_map_t::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      {
	names[(*(*i).second.begin()).first] = numeric_limits<identifier_t>::min() + (*i).first;
	renames[(*(*i).second.begin()).first] = numeric_limits<identifier_t>::min() + (*i).first;
      }
    Hom hom(new rename_full_t(names));
    DDD& ddd = place.ddd();
    ddd = hom(ddd);
  }

  /**
   * For each element in term, if it is a variable, find
   * correspondance between place DDD and transition DDD, if it is a
   * constant, put it in the constants to select.
   */
  void
  ddd_compare_firstopt_t::convert_to_transition (const unfold_ddd_t& transition,
						 const unfold_ddd_t& place,
						 variables_map_t& variables,
						 selects_map_t& selects,
						 const CFuncTerm& term)
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
			  break;
			}
		  }
	      }
	  }
	else
	  selects[(*p_i).second] = this->net_.extract((*i).Elts());
      }
  }

  /**
   * For each element in term, if it is a variable, find
   * correspondance between place DDD and transition DDD, if it is a
   * constant, put it in the constants to select.
   */
  void
  ddd_compare_firstopt_t::convert_to_place (const unfold_ddd_t& transition,
					    const unfold_ddd_t& place,
					    variables_map_t& variables,
					    values_map_t& values,
					    const CFuncTerm& term)
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
			  break;
			}
		  }
	      }
	  }
	else if ((*p_i).second != variable_order_t::UNKNOWN_POSITION)
	  values[(*p_i).second] = this->net_.extract((*i).Elts());
      }
  }

}

