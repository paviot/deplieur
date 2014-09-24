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
 
#include "ddd_compare_bubble.hh"

#include <limits>
#include <algorithm>

#include <PNet.h>
#include <Variable.h>

namespace cpn_ddd
{

  namespace homomorphisms
  {

    bubble_sort_t::bubble_sort_t (const std::vector<identifier_t>* order) :
      order_(order)
    {}
  
    GDDD
    bubble_sort_t::phiOne () const
    {
      return GDDD::one;
    }
  
    GHom
    bubble_sort_t::phi (int var, int val) const
    {
      return Hom(bubble_sort_down_t(this->order_, var, val)) & Hom(*this);
    }

    bool
    bubble_sort_t::operator==(const StrongHom& s) const
    {
      const bubble_sort_t* const other =  dynamic_cast<const bubble_sort_t*>(&s);
      if (other == NULL)
	return false;
      else
	return this->order_ == other->order_;
    }
  
    size_t
    bubble_sort_t::hash () const
    {
      return ddd::knuth32_hash(reinterpret_cast<size_t>(this->order_));
    }

    _GHom*
    bubble_sort_t::clone() const
    {
      return new bubble_sort_t(*this);
    }

    bubble_sort_down_t::bubble_sort_down_t (const std::vector<identifier_t>* order,
					    int var,
					    int val) :
      order_(order),
      var_(var),
      val_(val)
    {}
  
    GDDD
    bubble_sort_down_t::phiOne () const
    {
      return GDDD(this->var_, this->val_, GDDD::one);
    }
  
    GHom
    bubble_sort_down_t::phi (int var, int val) const
    {
      std::vector<identifier_t>::const_iterator pos =
	std::find(this->order_->begin(), this->order_->end(), var);
      assert(pos != this->order_->end());
      std::vector<identifier_t>::const_iterator prev_pos =
	std::find(this->order_->begin(), pos, this->var_);
      if (prev_pos == pos)
	return GHom(this->var_, this->val_, (GHom(var, val, GHom::id)));
      else
	return GHom(var, val, GHom(this->var_, this->val_, GHom::id));
    }

    bool
    bubble_sort_down_t::operator==(const StrongHom& s) const
    {
      const bubble_sort_down_t* const other =  dynamic_cast<const bubble_sort_down_t*>(&s);
      if (other == NULL)
	return false;
      else
	return this->order_ == other->order_
	  && this->var_ == other->var_
	  && this->val_ == other->val_;
    }
  
    size_t
    bubble_sort_down_t::hash () const
    {
      return ddd::knuth32_hash(reinterpret_cast<size_t>(this->order_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->var_))
	^ ddd::knuth32_hash(static_cast<size_t>(this->val_));
    }

    _GHom*
    bubble_sort_down_t::clone() const
    {
      return new bubble_sort_down_t(*this);
    }

  }

  /*
   * compare_bubble_t
   */

  ddd_compare_bubble_t::ddd_compare_bubble_t (net_t& net,
					      const classes_t& classes,
					      identifiers_t& identifiers) :
    ddd_operation_t (net, classes, identifiers),
    arcs_cache_()
  {}
  
  ddd_compare_bubble_t::~ddd_compare_bubble_t ()
  {}

  Hom
  ddd_compare_bubble_t::select_cst (const values_map_t& values) const
  {
    Hom result(Hom::id);
    for (values_map_t::const_iterator i = values.begin();
	 i != values.end();
	 ++i)
      result = Hom(homomorphisms::select_cst_t((*i).first, (*i).second))
	& result;
    return result;
  }
  
  Hom
  ddd_compare_bubble_t::select_var (const variables_map_t& variables,
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
	  result = Hom(homomorphisms::select_var_t
		       ((*first).first, (*j).first,
			get_succ((*first).first, (*j).second - (*first).first, (*e).second),
			(*e).second))
	    & result;
      }
    return result;
  }

  Hom
  ddd_compare_bubble_t::remove (const values_map_t& values) const
  {
    Hom result(Hom::id);
    for (values_map_t::const_iterator i = values.begin();
	 i != values.end();
	 ++i)
      result = Hom(homomorphisms::remove_t((*i).first))
	& result;
    return result;
  }

  Hom
  ddd_compare_bubble_t::remove (const variables_map_t& variables) const
  {
    Hom result(Hom::id);
    for (variables_map_t::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      for (modifiers_map_t::const_iterator j = ++(*i).second.begin();
	   j != (*i).second.end();
	   ++j)
	result = Hom(homomorphisms::remove_t((*j).first))
	  & result;
    return result;
  }

  Hom
  ddd_compare_bubble_t::shift_inc (const variables_map_t& variables,
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
	result = Hom(homomorphisms::shift_t((*i).first, (*first).second, (*e).second))
	  & result;
      }
    return result;
  }

  Hom
  ddd_compare_bubble_t::shift_dec (const variables_map_t& variables,
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
	result = Hom(homomorphisms::shift_t((*first).first, -(*first).second, (*e).second))
	  & result;
      }
    return result;
  }

  Hom
  ddd_compare_bubble_t::rename (const variables_map_t& variables) const
  {
    Hom result(Hom::id);
    renames_map_t renames;
    for (variables_map_t::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      {
	modifiers_map_t::const_iterator first = (*i).second.begin();
	identifier_t name = numeric_limits<identifier_t>::min() + (*first).first;
 	renames[name] = (*i).first;
	result = Hom(homomorphisms::rename_t((*first).first, name))
	  & result;
      }
    return this->rename(renames)
      & result;
  }

  Hom
  ddd_compare_bubble_t::insert (const values_map_t& completes) const
  {
    Hom result(Hom::id);
    for (values_map_t::const_reverse_iterator i = completes.rbegin();
	 i != completes.rend();
	 ++i)
      result = Hom(homomorphisms::insert_t((*i).first, (*i).second))
	& result;
    return result;
  }
  
  Hom
  ddd_compare_bubble_t::move (const variables_map_t& variables,
			      const values_map_t& completes) const
  {
    std::vector<identifier_t>* order = new std::vector<identifier_t>();
    variables_map_t::const_iterator i;
    values_map_t::const_iterator j;
    for (i = variables.begin(), j = completes.begin();
	 (i != variables.end()) || (j != completes.end());)
      {
	bool use_value;
	if ((i != variables.end()) && (j != completes.end()))
	  use_value = ((*j).first < (*i).first);
	else if (i != variables.end())
	  use_value = false;
	else //(j != values.rend())
	  use_value = true;
	identifier_t name;
	if (use_value)
	  name = (*j++).first;
	else
	  name = (*i++).first;
	order->push_back(name);
      }
    return fixpoint(homomorphisms::bubble_sort_t(order));
  }

  Hom
  ddd_compare_bubble_t::rename (const renames_map_t& renames) const
  {
    Hom result(Hom::id);
    for (renames_map_t::const_iterator i = renames.begin();
	 i != renames.end();
	 ++i)
      result = Hom(homomorphisms::rename_t((*i).first, (*i).second))
	& result;
    return result;
  }

  Hom
  ddd_compare_bubble_t::copy (const variables_map_t& variables,
				 const renames_map_t& renames) const
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
	if ((*occs.find((*first).first)).second == 1)
	  // Rename if no more copy of this variable is needed :
	  result = Hom(homomorphisms::rename_t((*renames.find((*first).first)).second, (*i).first))
	    & result;
	else
	  // Copy if more copies of this variable are needed :
	  {
	    result = Hom(homomorphisms::copy_t(last, (*renames.find((*first).first)).second, (*i).first))
	      & result;
	    --(*occs.find((*first).first)).second;
	  }
	last = (*i).first;
      }
    return result;
  }

  Hom
  ddd_compare_bubble_t::remove (const renames_map_t& renames) const
  {
    Hom result(Hom::id);
    for (renames_map_t::const_iterator i = renames.begin();
	 i != renames.end();
	 ++i)
      result = Hom(homomorphisms::remove_t((*i).second))
	& result;
    return result;
  }

  /**
   * For each place, build a DDD corresponding to its marking.
   * If "marked", select place DDD - marking DDD,
   * else select place DDD * marking DDD.
   */
  void
  ddd_compare_bubble_t::remove_places (net_t& net,
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
	    DDD mark_ddd(DDD::null); // DDD of the mark.
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
		    else
		      current_ddd = DDD::one;
		    mark_ddd = DDD::one;
		    break;
		  }
		// Get DDD of a mark :
		const std::list< std::vector<Element> >& elements = (*j).Elts();
		DDD one_mark_ddd(DDD::one);
		DDD previous_ddd(DDD::one);
		// Create mark DDD from the last variable of the DDD :
		for (variable_order_t::conversion_t::const_iterator position =
		       conversion.begin();
		     position != conversion.end();
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
	    //cerr << "Mark : " << std::endl << mark_ddd;
	    //cerr << "Current : " << std::endl << current_ddd;
	    // Compute result :
	    if (marked)
	      current_ddd = current_ddd - mark_ddd;
	    else
	      current_ddd = current_ddd * mark_ddd;
	    //	    cerr << "Final size : " << current_ddd.nbStates() << endl;
	  }
	else if (! marked)
	  (*i).second.ddd() = DDD::null;
      }
  }

  /**
   * For each element in term, if it is a variable, find
   * correspondance between place DDD and transition DDD, if it is a
   * constant, put it in the constants to select.
   */
  void
  ddd_compare_bubble_t::convert_to_transition (const unfold_ddd_t& transition,
						  const unfold_ddd_t& place,
						  const CFuncTerm& term,
						  variables_map_t& variables,
						  values_map_t& values,
						  values_map_t& elements,
						  values_map_t& completes) const
  {
    //    cerr << "term : " << term << endl;
    const variable_order_t* const place_order = place.order();
    const variable_order_t::conversion_t place_conversion = place_order->order_full(this->classes_);	
    const variable_order_t* const transition_order = transition.order();
    const variable_order_t::infos_t transition_infos = transition_order->infos(this->classes_);
    const variable_order_t::conversion_t transition_conversion = transition_order->order(this->classes_);
    const std::list<CFuncElt>& elts = term.Elts();
    std::list<CFuncElt>::const_iterator i;
    variable_order_t::conversion_t::const_iterator p_i;
    /*
    for (p_i = place_conversion.begin();
	 p_i != place_conversion.end();
	 ++p_i)
      cerr << p_i->first << " = " << p_i->second << endl;
    */
    for (i = elts.begin(),
	   p_i = place_conversion.begin();
	 i != elts.end();
	 ++i, ++p_i)
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
		  //		  cerr << *(j->first) << endl;
		  const variable_order_t::variable_id_t id = (*j).first->Id();
		  variable_order_t::infos_t::const_iterator k;
		  variable_order_t::conversion_t::const_iterator pos;
		  for (k = transition_infos.begin(),
			 pos = transition_conversion.begin();
		       k != transition_infos.end();
		       ++k, ++pos)
		    if ((*k).id_ == id)
		      {
			//			cerr << "VARIABLE " << (*pos).second << "," << (*p_i).second << endl;
			variables[(*pos).second][(*p_i).second] = (*j).second;
			elements[(*pos).second] = this->net_.extract_cached(vclass->Elts());
			break;
		      }
		}
	    }
	}
      else
	{
	  //	  cerr << "VALUES " << (*p_i).second << endl;
	  if ((*p_i).second != variable_order_t::UNKNOWN_POSITION)
	    values[(*p_i).second] = this->net_.extract((*i).Elts());
	}
    /*
    for (i = elts.begin(), p_i = place_conversion.begin();
	 i != elts.end();
	 ++i, ++p_i)
      {
	if ((*p_i).second != variable_order_t::UNKNOWN_POSITION)
	  if ((*i).IsVar())
	    {
	      cerr << "isvar " << *i << endl;
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
	    {
	      cerr << "VALUES " << (*p_i).second << endl;
	      values[(*p_i).second] = this->net_.extract((*i).Elts());
	    }
      }
    */
    // Add transition variables not bound to this arc :
    variable_order_t::conversion_t::const_iterator t_c;
    variable_order_t::infos_t::const_iterator t_i;
    for (t_c = transition_conversion.begin(), t_i = transition_infos.begin();
	 t_i != transition_infos.end();
	 ++t_c, ++t_i)
      {
	//	cerr << "COMPLETES? " << (*t_c).second << endl;
	if (variables.find((*t_c).second) == variables.end())
	  //	    && (values.find((*t_c).second) == values.end()))
	  {
	    //	    cerr << "COMPLETES " << (*t_c).second << endl;
	    completes.insert(make_pair((*t_c).second, this->net_.extract_cached((*t_i).class_->Elts())));
	  }
      }
  }

  /**
   * For each element in term, if it is a variable, find
   * correspondance between place DDD and transition DDD, if it is a
   * constant, put it in the constants to select.
   */
  void
  ddd_compare_bubble_t::convert_to_place (const unfold_ddd_t& place,
					     const unfold_ddd_t& transition,
					     const CFuncTerm& term,
					     variables_map_t& variables,
					     values_map_t& values,
					     values_map_t& elements,
					     renames_map_t& renames) const
  {
    //    std::cerr << "term : " << term << std::endl;
    const variable_order_t* const place_order = place.order();
    const variable_order_t::conversion_t place_conversion = place_order->order_full(this->classes_);	
    const variable_order_t* const transition_order = transition.order();
    const variable_order_t::infos_t transition_infos = transition_order->infos(this->classes_);
    const variable_order_t::conversion_t transition_conversion = transition_order->order(this->classes_);
    const std::list<CFuncElt>& elts = term.Elts();
    std::list<CFuncElt>::const_iterator i;
    variable_order_t::conversion_t::const_iterator p_i;
    for (i = elts.begin(),
	   p_i = place_conversion.begin();
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
			  elements[(*p_i).second] = this->net_.extract_cached(vclass->Elts());
			  break;
			}
		  }
	      }
	  }
	else if ((*p_i).second != variable_order_t::UNKNOWN_POSITION)
	  values[(*p_i).second] = this->net_.extract((*i).Elts());
      }
    for (variable_order_t::conversion_t::const_iterator i = transition_conversion.begin();
	 i != transition_conversion.end();
	 ++i)
      renames[(*i).second] = numeric_limits<identifier_t>::min() + (*i).second;
  }

  Hom
  ddd_compare_bubble_t::convert_to_transition (const unfold_ddd_t& transition,
						  const unfold_ddd_t& place,
						  const CFuncTerm& term) const
  {
    variables_map_t variables;
    values_map_t values;
    values_map_t elements;
    values_map_t completes;
    this->convert_to_transition(transition, place, term,
				variables, values, elements, completes);
    return this->move(variables, completes)
      & this->insert(completes)
      & this->rename(variables)
      & this->shift_dec(variables, elements)
      & this->remove(variables)
      & this->remove(values)
      & this->select_var(variables, elements)
      & this->select_cst(values);
  }

  /*
  Hom
  ddd_compare_bubble_t::convert_to_transition_temp (const unfold_ddd_t& transition,
						       const unfold_ddd_t& place,
						       const CFuncTerm& term) const
  {
    variables_map_t variables;
    values_map_t values;
    values_map_t elements;
    values_map_t completes;
    this->convert_to_transition(transition, place, term,
				variables, values, elements, completes);
    return //this->move(variables, completes)
      //& this->insert(completes)
      //& this->rename(variables)
      //& this->shift_dec(variables, elements)
      //& this->remove(variables)
      //& this->remove(values)
      this->select_var(variables, elements)
      & this->select_cst(values);
  }
  */

  Hom
  ddd_compare_bubble_t::convert_to_place (const unfold_ddd_t& place,
					     const unfold_ddd_t& transition,
					     const CFuncTerm& term) const
  {
    variables_map_t variables;
    values_map_t completes;
    values_map_t elements;
    renames_map_t renames;
    this->convert_to_place(place, transition, term,
			   variables, completes, elements, renames);
    return this->move(variables, completes)
      & this->shift_inc(variables, elements)
      & this->insert(completes)
      & this->remove(renames)
      & this->copy(variables, renames)
      & this->rename(renames);
  }

  Hom
  ddd_compare_bubble_t::convert_to_transition (const unfold_ddd_t& transition,
						  const unfold_ddd_t& place,
						  const Arc* const arc) const
  {
    arcs_cache_t::iterator z = this->arcs_cache_.find(arc);
    if (z != this->arcs_cache_.end())
      return (*z).second;
    const std::list<CFuncTerm> terms = arc->Valuation().Lst();
    Hom result(homomorphisms::constant_t(DDD::null));
    for (std::list<CFuncTerm>::const_iterator i = terms.begin();
	 i != terms.end();
	 ++i)
      result = this->convert_to_transition(transition, place, *i)
	+ result;
    this->arcs_cache_.insert(make_pair(arc, result));
    return result;
  }
 
  Hom
  ddd_compare_bubble_t::convert_to_place (const unfold_ddd_t& place,
					  const unfold_ddd_t& transition,
					  const Arc* const arc) const
  {
    arcs_cache_t::iterator z = this->arcs_cache_.find(arc);
    if (z != this->arcs_cache_.end())
      return (*z).second;
    const std::list<CFuncTerm> terms = arc->Valuation().Lst();
    Hom result(homomorphisms::constant_t(DDD::null));
    for (std::list<CFuncTerm>::const_iterator i = terms.begin();
	 i != terms.end();
	 ++i)
      result = this->convert_to_place(place, transition, *i)
	+ result;
    this->arcs_cache_.insert(make_pair(arc, result));
    return result;
  }

}
