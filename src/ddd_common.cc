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
 
#include "ddd_common.hh"
#include "ddd_unfold.hh"

#include <DDD.h>
#include <Hom.h>

#include <limits>

namespace cpn_ddd
{
  
  /**
   * This constant can be either "min()" or "max()" because
   * identifiers are signed integers.
   *
   * I choosed "max" because if there is a change to unsigned
   * integers, it needs no change.
   */
  const identifier_t identifiers_t::UNKNOWN_IDENTIFIER
  = numeric_limits<identifier_t>::max();

  identifiers_t::identifiers_t () :
    places_table_(),
    transitions_table_(),
    place_identifier_(identifiers_t::UNKNOWN_IDENTIFIER + 1),
    transition_identifier_(identifiers_t::UNKNOWN_IDENTIFIER + 1)
  {}

  identifiers_t::identifiers_t(const identifiers_t& to_copy) :
    places_table_(to_copy.places_table_),
    transitions_table_(to_copy.transitions_table_),
    place_identifier_(to_copy.place_identifier_),
    transition_identifier_(to_copy.transition_identifier_)
  {}

  identifiers_t::~identifiers_t ()
  {}

  identifiers_t&
  identifiers_t::operator= (const identifiers_t& to_copy)
  {
    if (this != &to_copy)
      {
	this->places_table_ = to_copy.places_table_;
	this->transitions_table_ = to_copy.transitions_table_;
	this->place_identifier_ = to_copy.place_identifier_;
	this->transition_identifier_ = to_copy.transition_identifier_;
      }
    return *this;
  }

  const identifiers_t::places_table_t&
  identifiers_t::places ()
  {
    return this->places_table_;
  }

  const identifiers_t::transitions_table_t&
  identifiers_t::transitions ()
  {
    return this->transitions_table_;
  }

  const identifier_t
  identifiers_t::add (Place* const place,
		      bool check)
  {
    identifier_t result;
    if (check)
      {
	result = identifiers_t::UNKNOWN_IDENTIFIER;
	for (places_table_t::const_iterator i = this->places_table_.begin();
	     i != places_table_.end();
	     ++i)
	  if ((*i).second == place)
	    {
	      result = (*i).first;
	      break;
	    }
	if (result == identifiers_t::UNKNOWN_IDENTIFIER)
	  {
	    result = this->place_identifier_++;
	    this->places_table_[result] = place;
	  }
      }
    else
      {
	result = this->place_identifier_++;
	this->places_table_[result] = place;
      }
    return result;
  }

  const identifier_t
  identifiers_t::add (Transition* const transition,
		      bool check)
  {
    identifier_t result;
    if (check)
      {
	result = identifiers_t::UNKNOWN_IDENTIFIER;
	for (transitions_table_t::const_iterator i = this->transitions_table_.begin();
	     i != transitions_table_.end();
	     ++i)
	  if ((*i).second == transition)
	    {
	      result = (*i).first;
	      break;
	    }
	if (result == identifiers_t::UNKNOWN_IDENTIFIER)
	  {
	    result = this->transition_identifier_++;
	    this->transitions_table_[result] = transition;
	  }
      }
    else
      {
	result = this->transition_identifier_++;
	this->transitions_table_[result] = transition;
      }
    return result;
  }

  const identifier_t
  identifiers_t::place (Place* const place) const
  {
    identifier_t result = identifiers_t::UNKNOWN_IDENTIFIER;
    for (places_table_t::const_iterator i = this->places_table_.begin();
	 i != places_table_.end();
	 ++i)
      if ((*i).second == place)
	{
	  result = (*i).first;
	  break;
	}
    return result;
  }

  const identifier_t
  identifiers_t::transition (Transition* const transition) const
  {
    identifier_t result = identifiers_t::UNKNOWN_IDENTIFIER;
    for (transitions_table_t::const_iterator i = this->transitions_table_.begin();
	 i != transitions_table_.end();
	 ++i)
      if ((*i).second == transition)
	{
	  result = (*i).first;
	  break;
	}
    return result;
  }

  const value_t
  get_succ (const value_t value,
	    const value_t succ,
	    const std::vector<Element>* elements)
  {
    int value_pos = 0;
    unsigned int elements_size = elements->size();
    for (std::vector<Element>::const_iterator i = elements->begin();
	 i != elements->end();
	 ++i)
      {
	if ((*i).Id() == value)
	  break;
	++value_pos;
      }
    value_pos = value_pos + succ;
    if (value_pos < 0)
      {
	value_pos = (-value_pos) % elements_size;
	if (value_pos != 0)
	  value_pos = elements_size - value_pos;
      }
    else
      value_pos = value_pos % elements_size;
    return (*elements)[value_pos].Id();
  }

  const value_t
  get_succ(const value_t value,
	   const value_t succ,
	   const values_t& elements)
  {
    int value_pos = 0;
    unsigned int elements_size = elements.size();
    for (values_t::const_iterator i = elements.begin();
	 i != elements.end();
	 ++i)
      {
	if (*i == value)
	  break;
	++value_pos;
      }
    value_pos = value_pos + succ;
    if (value_pos < 0)
      {
	value_pos = (-value_pos) % elements_size;
	if (value_pos != 0)
	  value_pos = elements_size - value_pos;
      }
    else
      value_pos = value_pos % elements_size;
    return elements[value_pos];
  }

}
