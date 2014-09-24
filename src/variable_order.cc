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

#include "variable_order.hh"

#include <Variable.h>

#include <algorithm>
#include <limits>

namespace cpn_ddd
{

  class compare_variables : public binary_function<Variable*, Variable*, bool>
  {
  public:
    bool operator() (const Variable* v1, const Variable* v2) const
    {
      return v1->Name() <= v2->Name();
    }
  };

  /**
   * This constant must be the highest possible, because positions are
   * unsigned integers, and "0" is used to reference first position.
   */
  const variable_order_t::position_t variable_order_t::UNKNOWN_POSITION
  = numeric_limits<variable_order_t::position_t>::max();

  /**
   * This constant can be either "min()" or "max()", because variable
   * identifiers are signed integers.
   *
   * I choosed "max" because if there is a change to unsigned
   * integers, it needs no change.
   */
  const variable_order_t::variable_id_t variable_order_t::PLACE_ID
  = numeric_limits<variable_order_t::variable_id_t>::max();

  variable_order_t::variable_order_t (const variable_order_t::infos_t& variables)
  {
    this->variables_ = variables;
  }

  variable_order_t::variable_order_t (const Place& place)
  {
    this->variables_ = this->get_variables(place.Dom());
  }

  variable_order_t::variable_order_t (const Transition& transition)
  {
    const std::list<Arc*> arcs_in = transition.getArcIn();
    const std::list<Arc*> arcs_out = transition.getArcOut();
    std::vector<Arc*> arcs;
    for (std::list<Arc*>::const_iterator j = arcs_in.begin();
	 j != arcs_in.end();
	 ++j)
      arcs.push_back(*j);
    for (std::list<Arc*>::const_iterator j = arcs_out.begin();
	 j != arcs_out.end();
	 ++j)
      arcs.push_back(*j);
    this->variables_ = this->get_variables(arcs);
  }

  variable_order_t::~variable_order_t ()
  {}

  variable_order_t&
  variable_order_t::operator= (const variable_order_t& to_copy)
  {
    if (this != &to_copy)
      {
	this->conversion_ = to_copy.conversion_;
	this->variables_ = to_copy.variables_;
      }
    return *this;
  }

  void
  variable_order_t::reorder ()
  {
    this->conversion_ = this->set_order(this->variables_);
  }

  variable_order_t::infos_t
  variable_order_t::get_variables (const std::vector<Arc*>& arcs) const
  {
    infos_t result;
    compare_variables comp;
    std::set<Variable*> vars;
    for (std::vector<Arc*>::const_iterator i = arcs.begin();
	 i != arcs.end();
	 ++i)
      {
	std::set<Variable*> v = (*i)->FindMentionedVars();
	set_union(vars.begin(), vars.end(),
		  v.begin(), v.end(),
		  inserter(vars, vars.begin()),
		  comp);
      }
    for (std::set<Variable*>::const_iterator i = vars.begin();
	 i != vars.end();
	 ++i)
      {
	info_t to_add = {(*i)->PClass(), (*i)->Id()};
	result.push_back(to_add);
      }
    return result;
  }

  variable_order_t::infos_t
  variable_order_t::get_variables (const Domain* domain) const
  {
    infos_t result;
    if (domain == NULL)
      return result;
    std::list<PNClass*> vars = domain->Elts();
    for (std::list<PNClass*>::iterator i = vars.begin();
	 i != vars.end();
	 ++i)
      {
	info_t to_add = {*i, variable_order_t::PLACE_ID};
	result.push_back(to_add);
      }
    return result;
  }

  variable_order_t::infos_t
  variable_order_t::infos (const std::vector<PNClass*> classes) const
  {
    infos_t result;
    for (infos_t::const_iterator i = this->variables_.begin();
	 i != this->variables_.end();
	 ++i)
      for (std::vector<PNClass*>::const_iterator j = classes.begin();
	   j != classes.end();
	   ++j)
	if ((*i).class_ == *j)
	  result.push_back(*i);
    return result;
  }

  variable_order_t::conversion_t
  variable_order_t::order (const std::vector<PNClass*> classes) const
  {
    const conversion_t reversed_result = this->reverse_order(classes);
    conversion_t result;
    for (conversion_t::const_iterator i = reversed_result.begin();
	 i != reversed_result.end();
	 ++i)
      result[(*i).second] = (*i).first;   
    return result;
  }

  variable_order_t::conversion_t
  variable_order_t::order_full (const std::vector<PNClass*> classes) const
  {
    conversion_t reduced = this->order(classes);
    conversion_t::const_iterator j;
    infos_t::const_iterator color_class;
    conversion_t result = this->conversion_;
    conversion_t::iterator i;
    for (i = result.begin(),
	   color_class = this->variables_.begin(),
	   j = reduced.begin();
	 i != result.end();
	 ++i, ++color_class)
      {
	if (std::find(classes.begin(), classes.end(), (*color_class).class_) != classes.end())
	  (*i).second = (*j++).second; 
	else
	  (*i).second = this->UNKNOWN_POSITION;
      }
    return result;
  }

  variable_order_t::conversion_t
  variable_order_t::reverse_order (const std::vector<PNClass*> classes) const
  {
    conversion_t result;
    position_t position = numeric_limits<position_t>::min();
    infos_t::const_iterator i;
    conversion_t::const_iterator z;
    for (i = this->variables_.begin(),
	   z = this->conversion_.begin();
	 i != this->variables_.end();
	 ++i, ++z)
      {
	std::vector<PNClass*>::const_iterator found =
	  std::find(classes.begin(), classes.end(), (*i).class_);
	if (found != classes.end())
	  result[(*z).second] = position++;
      }
    return result;
  }

  variable_order_t::conversion_t
  variable_order_t::other (const std::vector<PNClass*> classes) const
  {
    conversion_t result;
    position_t position = numeric_limits<position_t>::min();
    infos_t::const_iterator i;
    conversion_t::const_iterator z;
    for (i = this->variables_.begin(),
	   z = this->conversion_.begin();
	 i != this->variables_.end();
	 ++i, ++z)
      {
	std::vector<PNClass*>::const_iterator found =
	  std::find(classes.begin(), classes.end(), (*i).class_);
	if (found == classes.end())
	  result[(*z).second] = position++;
      }
    /*
    conversion_t result;
    position_t real_position = 0;
    position_t position = 0;
    for (variable_order_t::infos_t::const_iterator i = this->variables_.begin();
	 i != this->variables_.end();
	 ++i)
      {
	for (std::vector<PNClass*>::const_iterator j = classes.begin();
	     j != classes.end();
	     ++j)
	  if (! ((*i).class_ == *j))
	    result[position++] = (*this->conversion_.find(real_position)).second;
	++real_position;
      }
    */
    return result;
  }

  bool
  operator== (const variable_order_t::info_t& i1,
	      const variable_order_t::info_t& i2)
  {
    return (i1.class_ == i2.class_) && (i1.id_ == i2.id_);
  }

}
