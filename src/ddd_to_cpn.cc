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
 * \author Alban Linard, for LIP6-SRC (http://www-src.lip6.fr/)
 *
 * \version 0.1
 * \date 25/05/2005
 * Creation of this class.
 */

//#include "dotExporter.h"

#include "ddd_to_cpn.hh"
#include <hashfunc.hh>
#include <algorithm>

#ifdef FRAMEKIT_SUPPORT
#include <FKEnv.h>
#endif

namespace cpn_ddd
{

  const size_t
  identifiers_hash_t::operator() (const identifier_t id) const
  {
    return ddd::knuth32_hash(static_cast<size_t>(id));
  }

  ddd_to_cpn_t::ddd_to_cpn_t (PNet* petri_net,
			      net_t& net,
			      const classes_t& classes,
			      identifiers_t& identifiers,
			      arcs_t& arcs,
			      bool print_result) :
    ddd_operation_t(net, classes, identifiers),
    transformed_(false),
    from_pn_(petri_net),
    to_pn_(NULL),
    arcs_(arcs),
    classes_map_(),
    domains_map_(),
    places_(identifiers.places().size()),
    transitions_(identifiers.transitions().size()),
    places_order_(identifiers.places().size()),
    transitions_order_(identifiers.transitions().size()),
    id_(0),
    macao_id_(3),
    print_result_(print_result)
  {
    this->to_pn_ = new PNet();
  }

  ddd_to_cpn_t::~ddd_to_cpn_t ()
  {
    delete this->to_pn_;
  }

  void
  ddd_to_cpn_t::transform ()
  {
    if (! this->transformed_)
      {
	this->create_petri_net();
	this->to_pn_->GenerateStaticSub();
	this->transformed_ = true;
      }
#ifdef FRAMEKIT_SUPPORT
    if (g_fkStandalone)
      this->to_pn_->ExportToCami(cout);
    else
      this->to_pn_->ExportToMacao();
#else
    this->to_pn_->ExportToCami(cout);
#endif
    cerr << "Places : " << this->to_pn_->LPlace.size()
	 << "\tTransitions : " << this->to_pn_->LTrans.size()
	 << "\tArcs : " << this->to_pn_->LArc.size()
	 << endl;
    if (this->print_result_)
      cerr << *this->to_pn_;
  }

  PNet*
  ddd_to_cpn_t::get ()
  {
    if (! this->transformed_)
      {
	this->create_petri_net();
	this->to_pn_->reIndex();
	this->to_pn_->GenerateStaticSub();
	this->transformed_ = true;
      }
    return this->to_pn_;
  }

  void
  ddd_to_cpn_t::create_places (result_t& result, values_t& values, unfold_ddd_t& ddd, DDD::const_iterator& i)
  {
    values.push_back((*i).first);
    if ((*i).second == DDD::one)
      result.push_back(values);
    else
      for (DDD::const_iterator j = (*i).second.begin();
	   j != (*i).second.end();
	   ++j)
	this->create_places(result, values, ddd, j);
    values.pop_back();
  }

  void
  ddd_to_cpn_t::create_transitions (result_t& result, values_t& values, unfold_ddd_t& ddd, DDD::const_iterator& i)
  {
    values.push_back((*i).first);	  
    if ((*i).second == DDD::one)
      result.push_back(values);
    else
      for (DDD::const_iterator j = (*i).second.begin();
	   j != (*i).second.end();
	   ++j)
	this->create_transitions(result, values, ddd, j);
    values.pop_back();
  }

  void
  ddd_to_cpn_t::create_places (identifier_t id, unfold_ddd_t& ddd)
  {
    result_t values_result;
    places_t& result = this->places_[id];
    result.reserve(size_t(ddd.ddd().nbStates()));
    values_t values;
    // Find "from" place :
    Place* from = (*this->identifiers_.places().find(id)).second;
    variable_order_t* order = ddd.order();
    variable_order_t::infos_t infos = order->infos(this->classes_);
    variable_order_t::conversion_t conversion = order->reverse_order(this->classes_);
    // Find or build domain :
    Domain* domain = this->domains_map_[from->Dom()];
    if (domain == NULL)
      domain = this->net_.null_domain();
    DDD& d = ddd.ddd();
    if (d == DDD::one)
      values_result.push_back(values_t());
    // Create new places :
    for (DDD::const_iterator i = d.begin();
	 i != d.end();
	 ++i)
      this->create_places(values_result, values, ddd, i);
    std::list<Place>& places = this->to_pn_->LPlace.Lst();
    ids_t& ids = this->arcs_.unfolded_places().unfolded(id);
    for (result_t::iterator i = values_result.begin();
	 i != values_result.end();
	 ++i)
      {
	values_t stored_result;
	stored_result.reserve((*i).size());
	stringstream name;
	name << from->Name();	
	values_t::reverse_iterator j;
	variable_order_t::conversion_t::iterator k;
	for (j = (*i).rbegin(),
	       k = conversion.begin();
	     j != (*i).rend();
	     ++j, ++k)
	  {
	    stored_result.push_back(*j);
	    name << "_" << infos[(*k).second].class_->Elts()[*j].desc;
	  }
	//	Place p(this->id_++, this->macao_id_++, name.str(), domain);
	Place p(*from);
	p.Id(this->id_++);
	p.NumeroMacao(this->macao_id_++);
	p.Name(name.str());
	p.Dom(domain);
	p.setMarking(NULL);
	p.getArcIn() = std::list<Arc*>();
	p.getArcOut() = std::list<Arc*>();
	places.push_back(p);
	Place& ref_p = places.back();
	identifier_t id = this->identifiers_.add(&ref_p, false);
	result.push_back(make_pair(stored_result,id));
	ids.push_back(id);
      }
  }

  void
  ddd_to_cpn_t::create_transitions (identifier_t id, unfold_ddd_t& ddd)
  {
    result_t values_result;
    transitions_t& result = this->transitions_[id];
    result.reserve(size_t(ddd.ddd().nbStates()));
    values_t values;
    // Find "from" transition :
    const Transition* from = (*this->identifiers_.transitions().find(id)).second;
    variable_order_t* order = ddd.order();
    variable_order_t::infos_t infos = order->infos(this->classes_);
    variable_order_t::conversion_t conversion = order->reverse_order(this->classes_);
    // Find or build domain :
    DDD& d = ddd.ddd();
    if (d == DDD::one)
      values_result.push_back(values_t());
    // Create new transitions :
    for (DDD::const_iterator i = d.begin();
	 i != d.end();
	 ++i)   
      this->create_transitions(values_result, values, ddd, i);
    std::list<Transition>& transitions = this->to_pn_->LTrans.Lst();
    ids_t& ids = this->arcs_.unfolded_transitions().unfolded(id);
    for (result_t::iterator i = values_result.begin();
	 i != values_result.end();
	 ++i)
      {
	values_t stored_result;
	stored_result.reserve((*i).size());
	stringstream name;
	name << from->Name();
	values_t::reverse_iterator j;
	variable_order_t::conversion_t::iterator k;
	for (j = (*i).rbegin(),
	       k = conversion.begin();
	     j != (*i).rend();
	     ++j, ++k)
	  {
	    stored_result.push_back(*j);
	    name << "_" << infos[(*k).second].class_->Elts()[*j].desc;
	  }
	//	cerr << name.str() << endl;
	//	Transition t(this->id_++, this->macao_id_++, name.str(), priority);
	Transition t(*from);
	t.Id(this->id_++);
	t.NumeroMacao(this->macao_id_++);
	t.Name(name.str());
	t.getArcIn() = std::list<Arc*>();
	t.getArcOut() = std::list<Arc*>();
	t.setGuard("TRUE");
	/*
	Guard* guard = this->create_guard(from->getGuard(), order, *i);
	t.setGuard(guard);
	*/
	transitions.push_back(t);
	Transition& ref_t = transitions.back();
	identifier_t id = this->identifiers_.add(&ref_t, false);
	result.push_back(make_pair(stored_result, id));
	ids.push_back(id);
      }
  }

  void
  ddd_to_cpn_t::create_marking (const Place* const from,
				Place* const to,
				const variable_order_t* const order,
				const values_t& values)
  {
    // Get marking of place :
    Marking* from_marking = from->getMarking();
    if (from_marking == NULL)
      return;
    std::list<Mark>& from_marks = from_marking->Lst();
    std::list<Mark> to_marks;
    const variable_order_t::infos_t infos = order->infos(this->classes_);
    const variable_order_t::conversion_t conversion = order->order_full(this->classes_);
    // Get marks of marking :
    int uncoloured_mark = 0;
    for (std::list<Mark>::iterator i = from_marks.begin();
	 i != from_marks.end();
	 ++i)
      {
	bool found = true;
	Domain* domain = this->domains_map_[(*i).getDomain()];
	list< vector<Element> > to_elts = (*i).Elts();
	// Search for unfolded colours in Mark :
	variable_order_t::position_t position = 0;
	for (list< vector<Element> >::iterator j = to_elts.begin();
	     j != to_elts.end();)
	  {
	    if ((*conversion.find(position)).second != variable_order_t::UNKNOWN_POSITION)
	      {
		// Check mark :
		bool found_elt = false;
		for (vector<Element>::iterator k = (*j).begin();
		     k != (*j).end();
		     ++k)
		  if ((*k).Id() == values[(*conversion.find(position)).second])
		    {
		      found_elt = true;
		      break;
		    }
		found = found && found_elt;
		if (! found)
		  break;
		to_elts.erase(j++);
	      }
	    else
	      ++j;
	    ++position;
	  }
	if (found)
	  {	    
	    bool is_empty = true;
	    for (list< vector<Element> >::iterator j = to_elts.begin();
		 j != to_elts.end();
		 ++j)
	      if (! (*j).empty())
		{
		  is_empty = false;
		  break;
		}
	    if (is_empty)
// PATCH
        uncoloured_mark += (*i).Mult();
	    else
	      {
		Mark new_mark(domain, to_elts, (*i).Mult());
		to_marks.push_back(new_mark);
	      }
	  }
      }
    if (uncoloured_mark != 0)
      {
	list< vector<Element> > to_elts;
	//	to_marks.clear();
	to_marks.push_back(Mark(NULL, to_elts, uncoloured_mark));
	to->setMarking(new Marking(to_marks));
      }
    else if (to_marks.size() != 0)
      to->setMarking(new Marking(to_marks));
  }

  void
  ddd_to_cpn_t::create_domains ()
  {
    std::list<Domain>& to_domains = this->to_pn_->LDom.Lst();
    std::list<Domain>& from_domains = this->from_pn_->LDom.Lst();
    for (std::list<Domain>::iterator i = to_domains.begin();
	 i != to_domains.end();
	 ++i)
      if ((*i).Name() == "null")
	{
	  this->net_.null_domain(&(*i));
	  break;
	}
    for (std::list<Domain>::iterator i = from_domains.begin();
	 i != from_domains.end();
	 ++i)
      {
	Domain d((*i).Name());
	std::list<PNClass*>& to_classes = d.Elts();
	const std::list<PNClass*>& from_classes = (*i).Elts();
	for (std::list<PNClass*>::const_iterator j = from_classes.begin();
	     j != from_classes.end();
	     ++j)
	  {
	    bool found = false;
	    for (classes_t::const_iterator k = this->classes_.begin();
		 k != this->classes_.end();
		 ++k)
	      if (*k == *j)
		{
		  found = true;
		  break;
		}
	    if (! found)
	      to_classes.push_back(this->classes_map_[*j]);
	  }
	// Check if domain already exists :
	Domain* ref_exists = NULL;
	for (std::list<Domain>::iterator j = to_domains.begin();
	     j != to_domains.end();
	     ++j)
	  {
	    bool same = true;
	    std::list<PNClass*>::iterator iter_from = (*j).Elts().begin();
	    std::list<PNClass*>::iterator end_from = (*j).Elts().end();
	    std::list<PNClass*>::iterator iter_to = d.Elts().begin();
	    std::list<PNClass*>::iterator end_to = d.Elts().end();
	    while ((iter_from != end_from) && (iter_to != end_to))
	      {
		same = same && (*iter_from == *iter_to);
		++iter_from;
		++iter_to;
	      }
	    same = same && !((iter_from != end_from) || (iter_to != end_to));
	    if (same)
	      {
		ref_exists = &(*j);
		break;
	      }
	  }
	if (ref_exists != NULL)
	  this->domains_map_[&(*i)] = ref_exists;
	else
	  {
	    to_domains.push_back(d);
	    Domain& ref_d = to_domains.back();
	    this->domains_map_[&(*i)] = &ref_d;
	    if (ref_d.Name() == "null")
	      this->net_.null_domain(&ref_d);
	  }
      }
    if (this->net_.null_domain() == NULL)
      {
	to_domains.push_back(Domain("null"));
	this->net_.null_domain(&to_domains.back());
      }
  }

  void
  ddd_to_cpn_t::create_classes ()
  {
    std::list<PNClass>& to_classes = this->to_pn_->LClasse.Lst();
    std::list<PNClass>& from_classes = this->from_pn_->LClasse.Lst();
    for (std::list<PNClass>::iterator i = from_classes.begin();
	 i != from_classes.end();
	 ++i)
      {
	bool found = false;
	for (classes_t::const_iterator j = this->classes_.begin();
	     j != this->classes_.end();
	     ++j)
	  if (*i == *(*j))
	    {
	      found = true;
	      break;
	    }
	if (! found)
	  {
	    PNClass c(*i);
	    to_classes.push_back(c);
	    PNClass& ref_c = to_classes.back();
	    this->classes_map_[&(*i)] = &ref_c;
	  }
      }
  }

  void
  ddd_to_cpn_t::create_variables ()
  {
    std::list<Variable>& from_vars = this->from_pn_->LVar.Lst();
    std::list<Variable>& to_vars = this->to_pn_->LVar.Lst();
    for (std::list<Variable>::iterator i = from_vars.begin();
	 i != from_vars.end();
	 ++i)
      {
	bool found = false;
	PNClass* from_class = (*i).PClass();
	for (classes_t::const_iterator j = this->classes_.begin();
	     j != this->classes_.end();
	     ++j)
	  if (*j == from_class)
	    {
	      found = true;
	      break;
	    }
	if (! found)
	  {
	    Variable v((*i).Id(), (*i).Name(), this->classes_map_[from_class]);
	    to_vars.push_back(v);
	  }
      }
  }

  CFuncTerm
  ddd_to_cpn_t::create_function (const CFuncTerm& term)
  {
    Domain* dom = term.Dom();
    if (dom == NULL)
      dom = this->net_.null_domain();
    std::list<CFuncElt> empty_elts;
    CFuncTerm result(this->domains_map_.find(dom)->second, empty_elts, term.Mult());
    const std::list<PNClass*>& from_classes = dom->Elts();
    std::list<PNClass*>::const_iterator class_i = from_classes.begin();
    std::list<CFuncElt> from_elts = term.Elts();
    std::list<CFuncElt>& to_elts = result.Elts();
    for (std::list<CFuncElt>::iterator j = from_elts.begin();
	 j != from_elts.end();
	 ++j)
      {
	if ((*j).IsVar())
	  {
	    std::list<pair<Variable*, int> >& from_vars = (*j).getVars();
	    std::list<pair<Variable*, int> > to_vars;
	    for (std::list<pair<Variable*, int> >::iterator k = from_vars.begin();
		 k != from_vars.end();
		 ++k)
	      {
		Variable* found = this->to_pn_->LVar.Find((*k).first->Id());
		if (found != NULL)
		  to_vars.push_back(make_pair(found, (*k).second));
	      }
	    if (to_vars.size() != 0)
	      {
		std::list<pair<Variable*, int> >::iterator first = to_vars.begin();
		CFuncElt elt_to_add((*first).first, (*first).second);
		std::list<pair<Variable*, int> >& real_to_vars = elt_to_add.getVars();
		for (std::list<pair<Variable*, int> >::iterator k = ++first;
		     k != to_vars.end();
		     ++k)
		  real_to_vars.push_back(*k);
		to_elts.push_back(elt_to_add);
	      }
	  }
	else
	  {
	    bool found = false;
	    for (classes_t::const_iterator l = this->classes_.begin();
		 l != this->classes_.end();
		 ++l)
	      if (*class_i == *l)
		{
		  found = true;
		  break;
		}		    
	    if (! found)
	      {
		CFuncElt elt_to_add((*j).Elts());
		to_elts.push_back(elt_to_add);
	      }
	  }
	++class_i;
      }
    return result;
  }

  Guard*
  ddd_to_cpn_t::create_guard (Guard* guard, variable_order_t* order, values_t& values)
  {
    std::list<Variable*> variables = guard->FindMentionedVars();
    std::vector<Element>& elts_set = guard->getSet();
    // Add variables :
    variable_order_t::infos_t infos = order->infos(this->classes_);
    variable_order_t::conversion_t conversion = order->reverse_order(this->classes_);
    Guard* new_guard = guard->getFg();
    for (variable_order_t::position_t position = 0;
	 position < values.size();
	 ++position)
      {
	variable_order_t::variable_id_t id = infos[conversion[position]].id_;
	std::list<Variable*>::iterator var;
	for (var = variables.begin();
	     var != variables.end();
	     ++var)
	  if ((*var)->Id() == id)
	    break;
	if (var != variables.end())
	  {
	    std::vector<Element> elts;
	    for (std::vector<Element>::iterator i = elts_set.begin();
		 i != elts_set.end();
		 ++i)
	      if (values[position] == (*i).Id())
		{
		  elts.push_back(values[position]);
		  break;
		}
	    Guard* dom_guard = new Guard(*var, elts, 0);
	    new_guard = new Guard("AND", dom_guard, new_guard);
	    //new_guard = Guard::SimplifierArbre(new_guard);
	  }
      }
    return new_guard;
  }

  void
  ddd_to_cpn_t::create_markings ()
  {
    const identifiers_t::places_table_t& places = this->identifiers_.places();
    for (places_map_t::iterator i = this->places_.begin();
	 i != this->places_.end();
	 ++i)
      {
	const Place* from = (*places.find((*i).first)).second;
	variable_order_t* order = this->places_order_[(*i).first];
	for (places_t::iterator j = (*i).second.begin();
	     j != (*i).second.end();
	     ++j)
	  {
	    Place* to = (*places.find((*j).second)).second;
	    this->create_marking(from, to, order, (*j).first);
	  }
      }
  }

  void
  ddd_to_cpn_t::create_places ()
  {
    ddds_t p = this->net_.places();
    for (ddds_t::iterator i = p.begin();
	 i != p.end();
	 ++i)
      {
	this->create_places((*i).first, (*i).second);
	this->places_order_[(*i).first] = (*i).second.order();
      }
  }

  void
  ddd_to_cpn_t::create_transitions ()
  {
    ddds_t t = this->net_.transitions();
    for (ddds_t::iterator i = t.begin();
	 i != t.end();
	 ++i)
      {
	this->create_transitions((*i).first, (*i).second);
	this->transitions_order_[(*i).first] = (*i).second.order();
      }
  }

  class compare_bits_t
  {
  public:
    typedef std::vector< std::vector<bool> > elements_t;

    elements_t elts_;

    compare_bits_t (const elements_t& elts) :
      elts_(elts)
    {}

    ~compare_bits_t ()
    {}

    bool
    operator() (const std::pair<const cpn_ddd::values_t,
		cpn_ddd::identifier_t>& x) const
    {
      values_t::const_iterator i;
      elements_t::const_iterator j;
      //std::cerr << x.first.size() << ", " << this->elts_.size() << std::endl;
      for (i = x.first.begin(), j = this->elts_.begin();
	   i != x.first.end();
	   ++i, ++j)
	if (! ((*j)[*i]))
	  return false;
      return true;
    }
  };
  
  /*
  ddd_to_cpn_t::elements_t
  operator+(const ddd_to_cpn_t::elements_t& l,
	    const ddd_to_cpn_t::elements_t& r)
  {
    if (l.empty())
      return r;
    else if (r.empty())
      return l;
    // else
    assert(l.size() == r.size());
    compare_bits_t::elements_t result;
    for (compare_bits_t::elements_t::const_iterator i = l.begin(),
	   j = r.end();
	 i != l.end();
	 ++i, ++j)
      {
	assert(i->size() == j->size());
	std::vector<bool> to_add;
	for (std::vector<bool>::const_iterator bi = i->begin(),
	       bj = j->begin();
	     bi != i->end();
	     ++bi, ++bj)
	  to_add.push_back(*bi | *bj);
	result.push_back(to_add);
      }
    return result;
  }
  */

  //  std::vector<ddd_to_cpn_t::elements_t>
  ddd_to_cpn_t::elements_t
  ddd_to_cpn_t::eval_arc_from_transition(const CFuncTerm& term,
					 const unfold_ddd_t& place,
					 const std::map<variable_order_t::variable_id_t, value_t>& variables)
  {
    /*
    std::cerr << "In term "
	      << term
	      << " : ";
    for (std::map<variable_order_t::variable_id_t, value_t>::const_iterator i = variables.begin();
	 i != variables.end();
	 ++i)
      std::cerr << this->from_pn_->LVar.Find(i->first)->Name()
		<< "="
		<< i->second
		<< " ";
    std::cerr << std::endl;
    */
    elements_t result;
    variable_order_t* order = place.order();
    variable_order_t::conversion_t conversion = order->order_full(this->classes_);
    variable_order_t::infos_t infos = order->infos(this->classes_);
    std::map<identifier_t, vector<bool> > tmp_values;
    const std::list<CFuncElt>& elements = term.Elts();
    variable_order_t::conversion_t::iterator position;
    variable_order_t::infos_t::iterator info;
    std::list<CFuncElt>::const_iterator j;
    for (j = elements.begin(), position = conversion.begin(), info = infos.begin();
	 j != elements.end();
	 ++j, ++position)
      {
	if ((*position).second != variable_order_t::UNKNOWN_POSITION)
	  {
	    if ((*j).IsVar())
	      {
		const std::list< std::pair<Variable*, int> >& vars = (*j).getVars();
		for (std::list< std::pair<Variable*, int> >::const_iterator k = vars.begin();
		     k != vars.end();
		     ++k)
		  {
		    std::map<variable_order_t::variable_id_t, value_t>::const_iterator val = variables.find((*k).first->Id());
		    PNClass* var_class = (*k).first->PClass();
		    values_t elements = this->net_.extract_cached(var_class->Elts());
		    value_t value = get_succ((*val).second, (*k).second, elements);
		    vector<bool> tmp_vals = tmp_values[(*position).second];
		    vector<bool> vals((*info).class_->Elts().size(), false);
		    vals[value] = true;
		    tmp_values[(*position).second] = vals;
		  }
	      }
	    else
	      {
		std::vector<Element> vars = (*j).Elts();
		std::vector<bool> vals((*info).class_->Elts().size(), false);
		for (std::vector<Element>::iterator k = vars.begin();
		     k != vars.end();
		     ++k)
		  vals[(*k).Id()] = true;
		tmp_values[(*position).second] = vals;
	      }
	    ++info;
	  }
      }
    for (std::map<identifier_t, vector<bool> >::iterator j =
	   tmp_values.begin();
	 j != tmp_values.end();
	 ++j)
      result.push_back((*j).second);
    return result;
  }

  void
  ddd_to_cpn_t::create_arcs_from_transitions ()
  {
    // arcs cache :
    typedef std::map< std::pair<Place*, Transition*>, Arc*> arc_cache_t;
    arc_cache_t pre_arcs;
    arc_cache_t post_arcs;

    const identifiers_t::places_table_t& places_table =
      this->identifiers_.places();
    const identifiers_t::transitions_table_t& transitions_table =
      this->identifiers_.transitions();
    Arcs& arcs = this->to_pn_->LArc;
    for (transitions_map_t::iterator i = this->transitions_.begin();
	 i != this->transitions_.end();
	 ++i)
      {
	std::map<variable_order_t::variable_id_t, value_t> variables;
	variable_order_t* order = this->transitions_order_[(*i).first];
	variable_order_t::infos_t infos = order->infos(this->classes_);
	variable_order_t::conversion_t conversion = order->order(this->classes_);
	for (transitions_t::iterator j = (*i).second.begin();
	     j != (*i).second.end();
	     ++j)
	  {
	    Transition* transition = (*transitions_table.find((*j).second)).second;
	    /*
	    std::cerr << *transition
		      << std::endl;
	    */
	    variable_order_t::conversion_t::iterator position;
	    //	    variable_order_t::conversion_t::reverse_iterator position;
	    values_t::const_iterator k;
	    for (k = (*j).first.begin(), position = conversion.begin();
		 k != (*j).first.end();
		 ++k, ++position)
	      variables[infos[(*position).first].id_] = *k;
	    arcs_t::infos_t pre = this->arcs_.pre((*j).second, arcs_t::TRANSITION);
	    for (arcs_t::infos_t::iterator k = pre.begin();
		 k != pre.end();
		 ++k)
	      {
		places_t current_places = (*this->places_.find((*k).first)).second;
		ddds_t::iterator z = this->net_.places().find((*k).first);
		const CFunc& function = (*k).second->Valuation();
		const std::list<CFuncTerm>& terms = function.Lst();
		for (std::list<CFuncTerm>::const_iterator l = terms.begin();
		     l != terms.end();
		     ++l)
		  {
		    if ((*l).Mult() < 0)
		      {
			cerr << "WARNING : unfolding of the valuation term " << *l << " in arc " << *(*k).second << " should be checked because of its negative multiplier." << std::endl;
		      }
		    //else
		      {
			compare_bits_t comp(this->eval_arc_from_transition(*l, (*z).second, variables));
			for (places_t::iterator m =
			       find_if(current_places.begin(), current_places.end(), comp);
			     m != current_places.end();
			     m = find_if(m, current_places.end(), comp))
			  {
			    Place* p = (*places_table.find((*m).second)).second;
			    Arc* cached = pre_arcs[std::make_pair(p, transition)];
			    if (cached == NULL)
			      {
				Arc arc(this->id_++, p, transition, Place2Trans, "1", -1, (*k).second->IsInhibitor(), false);
				arc.NumeroMacao(this->macao_id_++);
				cached = arcs.Insert(arc);
				pre_arcs[std::make_pair(p, transition)] = cached;
				cached->Valuation().Lst().clear();
			      }
			    CFunc& valuation = cached->Valuation();
			    CFuncTerm x = this->create_function(*l);
			    bool found = false;
			    for (std::list<CFuncTerm>::iterator elt = valuation.Lst().begin();
				 elt != valuation.Lst().end();
				 ++elt)
			      {
				if (x.Elts() == (*elt).Elts())
				  {
				    (*elt).Mult() += x.Mult();
				    if ((*elt).Mult() == 0)
				      valuation.Lst().erase(elt);
				    found = true;
				    break;
				  }
			      }
			    if (! found)
			      valuation.Insert(x);
			    ++m;
			  }
		      }
		  }
	      }
	    arcs_t::infos_t post = this->arcs_.post((*j).second, arcs_t::TRANSITION);
	    for (arcs_t::infos_t::iterator k = post.begin();
		 k != post.end();
		 ++k)
	      {
		places_t current_places = (*this->places_.find((*k).first)).second;
		ddds_t::iterator z = this->net_.places().find((*k).first);
		const CFunc& function = (*k).second->Valuation();
		const std::list<CFuncTerm>& terms = function.Lst();
		for (std::list<CFuncTerm>::const_iterator l = terms.begin();
		     l != terms.end();
		     ++l)
		  {
		    if ((*l).Mult() < 0)
		      {
			cerr << "WARNING : unfolding of the valuation term " << *l << " in arc " << *(*k).second << " should be checked because of its negative multiplier." << std::endl;
		      }
		    //else
		      {
			compare_bits_t comp(this->eval_arc_from_transition(*l, (*z).second, variables));
			for (places_t::iterator m =
			       find_if(current_places.begin(), current_places.end(), comp);
			     m != current_places.end();
			     m = find_if(m, current_places.end(), comp))
			  {
			    Place* p = (*places_table.find((*m).second)).second;
			    Arc* cached = post_arcs[std::make_pair(p, transition)];
			    if (cached == NULL)
			      {
				Arc arc(this->id_++, p, transition, Trans2Place, "1", -1, (*k).second->IsInhibitor(), false);
				arc.NumeroMacao(this->macao_id_++);
				cached = arcs.Insert(arc);
				post_arcs[std::make_pair(p, transition)] = cached;
				cached->Valuation().Lst().clear();
			      }
			    CFunc& valuation = cached->Valuation();
			    CFuncTerm x = this->create_function(*l);
			    bool found = false;
			    for (std::list<CFuncTerm>::iterator elt = valuation.Lst().begin();
				 elt != valuation.Lst().end();
				 ++elt)
			      {
				if (x.Elts() == (*elt).Elts())
				  {
				    (*elt).Mult() += x.Mult();
				    if ((*elt).Mult() == 0)
				      valuation.Lst().erase(elt);
				    found = true;
				    break;
				  }
			      }
			    if (! found)
			      valuation.Insert(x);
			    ++m;
			  }
		      }
		  }
	      }
	  }
      }
  }

  // WARNING : BUGGED !!!!!
  //  std::vector<ddd_to_cpn_t::elements_t>
  ddd_to_cpn_t::elements_t
  ddd_to_cpn_t::eval_arc_from_place (const CFuncTerm& term,
				     const unfold_ddd_t& place,
				     const unfold_ddd_t& transition,
				     const values_t& values)
  {
    elements_t result;
    //    std::vector<elements_t> result;
    const variable_order_t* const place_order = place.order();
    const variable_order_t::infos_t place_infos = place_order->infos(this->classes_);
    const variable_order_t::conversion_t place_conversion = place_order->order_full(this->classes_);
    const variable_order_t::conversion_t place_conv = place_order->order(this->classes_);
    const variable_order_t* const transition_order = transition.order();
    const variable_order_t::infos_t transition_infos = transition_order->infos(this->classes_);
    const variable_order_t::conversion_t transition_conversion = transition_order->order(this->classes_);
    // Build default transition
    std::map<identifier_t, std::vector<bool> > default_transition;
    {
      variable_order_t::infos_t::const_iterator t_i;
      variable_order_t::conversion_t::const_iterator t_c;
      for (t_i = transition_infos.begin(), t_c = transition_conversion.begin();
	   t_i != transition_infos.end();
	   ++t_i, ++t_c)
	default_transition[(*t_c).second] = std::vector<bool>((*t_i).class_->Elts().size(), true);
    }
    // Build place variables
    std::map<identifier_t, value_t> default_place;
    {
      variable_order_t::conversion_t::const_iterator p_c;
      values_t::const_reverse_iterator p_v;
      for (p_c = place_conv.begin(), p_v = values.rbegin();
	   p_c != place_conv.end();
	   ++p_c, ++p_v)
	default_place[(*p_c).second] = *p_v;
    }
    /*
    const CFunc& function = arc.Valuation();
    const std::list<CFuncTerm>& terms = function.Lst();
    for (std::list<CFuncTerm>::const_iterator i = terms.begin();
	 i != terms.end();
	 ++i)
      {
    */
    std::map<identifier_t, vector<bool> > tmp_values = default_transition;
    const std::list<CFuncElt>& elements = term.Elts();
    variable_order_t::conversion_t::const_iterator place_position;
    std::list<CFuncElt>::const_iterator j;
    for (j = elements.begin(), place_position = place_conversion.begin();
	 j != elements.end();
	 ++j, ++place_position)
      {
	if ((*place_position).second != variable_order_t::UNKNOWN_POSITION)
	  if ((*j).IsVar())
	    {
	      const std::list< std::pair<Variable*, int> >& vars = (*j).getVars();
	      for (std::list< std::pair<Variable*, int> >::const_iterator k = vars.begin();
		   k != vars.end();
		   ++k)
		{
		  variable_order_t::conversion_t::const_iterator transition_position;
		  variable_order_t::infos_t::const_iterator transition_info;
		  for (transition_info = transition_infos.begin(),
			 transition_position = transition_conversion.begin();
		       transition_info != transition_infos.end();
		       ++transition_info, ++transition_position)
		    if ((*transition_info).id_ == (*k).first->Id())
		      break;
		  PNClass* var_class = (*k).first->PClass();
		  values_t elements = this->net_.extract_cached(var_class->Elts());
		  value_t value = get_succ(default_place[(*place_position).second], -(*k).second, elements);
		  vector<bool> tmp_vals = tmp_values[(*transition_position).second];
		  if (! tmp_vals[value])
		    {
		      tmp_values.clear();
		      break;
		    }
		  vector<bool> vals(elements.size(), false);
		  vals[value] = true;
		  tmp_values[(*transition_position).second] = vals;
		}
	    }
	  else
	    {
	      const value_t value = default_place[(*place_position).second];
	      const std::vector<Element> vars = (*j).Elts();
	      std::vector<Element>::const_iterator z;
	      for (z = vars.begin(); z != vars.end(); ++z)
		if ((*z).Id() == value)
		  break;
	      if (z == vars.end())
		{
		  tmp_values.clear();
		  break;
		}
	    }
	if (tmp_values.empty())
	  break;
      }
    for (std::map<identifier_t, vector<bool> >::const_iterator j =
	   tmp_values.begin();
	 j != tmp_values.end();
	 ++j)
      result.push_back((*j).second);
    return result;
  }

  void
  ddd_to_cpn_t::create_arcs_from_places ()
  {
    // arcs cache :
    typedef std::map< std::pair<Place*, Transition*>, Arc*> arc_cache_t;
    arc_cache_t pre_arcs;
    arc_cache_t post_arcs;

    const identifiers_t::places_table_t& places_table =
      this->identifiers_.places();
    const identifiers_t::transitions_table_t& transitions_table =
      this->identifiers_.transitions();
    Arcs& arcs = this->to_pn_->LArc;
    for (places_map_t::const_iterator i = this->places_.begin();
	 i != this->places_.end();
	 ++i)
      {
	const variable_order_t* const order = this->places_order_[(*i).first];
	const variable_order_t::infos_t infos = order->infos(this->classes_);
	const variable_order_t::conversion_t conversion = order->order(this->classes_);
	for (places_t::const_iterator j = (*i).second.begin();
	     j != (*i).second.end();
	     ++j)
	  {
	    Place* const place = (*places_table.find((*j).second)).second;
	    const arcs_t::infos_t pre = this->arcs_.pre((*j).second, arcs_t::PLACE);
	    for (arcs_t::infos_t::const_iterator k = pre.begin();
		 k != pre.end();
		 ++k)
	      {
		transitions_t current_transitions = (*this->transitions_.find((*k).first)).second;
		const CFunc& function = (*k).second->Valuation();
		const std::list<CFuncTerm>& terms = function.Lst();
		for (std::list<CFuncTerm>::const_iterator l = terms.begin();
		     l != terms.end();
		     ++l)
		  {
		    if ((*l).Mult() < 0)
		      {
			cerr << "WARNING : unfolding of the valuation term " << *l << " in arc " << *(*k).second << " should be checked because of its negative multiplier." << std::endl;
		      }
		    const elements_t values = this->eval_arc_from_place(*l, (*this->net_.places().find((*i).first)).second, (*this->net_.transitions().find((*k).first)).second, (*j).first);
		    if (! values.empty())
		      {
			compare_bits_t comp(values);
			for (transitions_t::iterator m =
			       find_if(current_transitions.begin(), current_transitions.end(), comp);
			     m != current_transitions.end();
			     m = find_if(m, current_transitions.end(), comp))
			  {
			    Transition* const t = (*transitions_table.find((*m).second)).second;
			    Arc* cached = pre_arcs[std::make_pair(place, t)];
			    if (cached == NULL)
			      {
				Arc arc(this->id_++, place, t, Trans2Place, "1", -1, (*k).second->IsInhibitor());
				arc.NumeroMacao(this->macao_id_++);
				cached = arcs.Insert(arc);
				pre_arcs[std::make_pair(place, t)] = cached;
				cached->Valuation().Lst().clear();
			      }
			    CFunc& valuation = cached->Valuation();
			    CFuncTerm x = this->create_function(*l);
			    bool found = false;
			    for (std::list<CFuncTerm>::iterator elt = valuation.Lst().begin();
				 elt != valuation.Lst().end();
				 ++elt)
			      {
				if (x.Elts() == (*elt).Elts())
				  {
				    (*elt).Mult() += x.Mult();
				    if ((*elt).Mult() == 0)
				      valuation.Lst().erase(elt);
				    found = true;
				    break;
				  }
			      }
			    if (! found)
			      valuation.Insert(x);
			    ++m;
			  }
		      }
		  }
	      }
	    arcs_t::infos_t post = this->arcs_.post((*j).second, arcs_t::PLACE);
	    for (arcs_t::infos_t::iterator k = post.begin();
		 k != post.end();
		 ++k)
	      {
		transitions_t current_transitions = (*this->transitions_.find((*k).first)).second;
		const CFunc& function = (*k).second->Valuation();
		const std::list<CFuncTerm>& terms = function.Lst();
		for (std::list<CFuncTerm>::const_iterator l = terms.begin();
		     l != terms.end();
		     ++l)
		  {
		    if ((*l).Mult() < 0)
		      {
			cerr << "WARNING : unfolding of the valuation term " << *l << " in arc " << *(*k).second << " should be checked because of its negative multiplier." << std::endl;
		      }
		    const elements_t values = this->eval_arc_from_place(*l, (*this->net_.places().find((*i).first)).second, (*this->net_.transitions().find((*k).first)).second, (*j).first);
		    if (! values.empty())
		      {
			compare_bits_t comp(values);
			for (transitions_t::iterator m =
			       find_if(current_transitions.begin(), current_transitions.end(), comp);
			     m != current_transitions.end();
			     m = find_if(m, current_transitions.end(), comp))
			  {
			    Transition* const t = (*transitions_table.find((*m).second)).second;
			    Arc* cached = post_arcs[std::make_pair(place, t)];
			    if (cached == NULL)
			      {
				Arc arc(this->id_++, place, t, Place2Trans, "1", -1, (*k).second->IsInhibitor());
				arc.NumeroMacao(this->macao_id_++);
				cached = arcs.Insert(arc);
				post_arcs[std::make_pair(place, t)] = cached;
				cached->Valuation().Lst().clear();
			      }
			    CFunc& valuation = cached->Valuation();
			    CFuncTerm x = this->create_function(*l);
			    bool found = false;
			    for (std::list<CFuncTerm>::iterator elt = valuation.Lst().begin();
				 elt != valuation.Lst().end();
				 ++elt)
			      {
				if (x.Elts() == (*elt).Elts())
				  {
				    (*elt).Mult() += x.Mult();
				    if ((*elt).Mult() == 0)
				      valuation.Lst().erase(elt);
				    found = true;
				    break;
				  }
			      }
			    if (! found)
			      valuation.Insert(x);
			    ++m;
			  }
		      }
		  }
	      }
	  }
      }
  }

  void
  ddd_to_cpn_t::create_arcs ()
  {
    unsigned long nb_places = 0;
    for (places_map_t::const_iterator i = this->places_.begin();
	 i != this->places_.end();
	 ++i)
      nb_places += (*i).second.size();
    unsigned long nb_transitions = 0;
    for (transitions_map_t::const_iterator i = this->transitions_.begin();
	 i != this->transitions_.end();
	 ++i)
      nb_transitions += (*i).second.size();
    //if (nb_places > nb_transitions)
    this->create_arcs_from_transitions();
    //else
    //this->create_arcs_from_places(); BUG
    for (std::list<Transition>::iterator i = this->to_pn_->LTrans.Lst().begin();
	 i != this->to_pn_->LTrans.Lst().end();
	 ++i)
      {
 	for (std::list<Arc*>::iterator j = (*i).getArcIn().begin();
	     j != (*i).getArcIn().end();)
	  if ((*j)->Valuation().Lst().empty())
 	    {
	      std::list<Arc*>::iterator remove = j++;
	      (*i).getArcIn().erase(remove);
	    }
	  else
	    ++j;
	for (std::list<Arc*>::iterator j = (*i).getArcOut().begin();
	     j != (*i).getArcOut().end();)
	  if ((*j)->Valuation().Lst().empty())
	    {
	      std::list<Arc*>::iterator remove = j++;
	      (*i).getArcOut().erase(remove);
	    }
	  else
	    ++j;
       }
    for (std::list<Place>::iterator i = this->to_pn_->LPlace.Lst().begin();
	 i != this->to_pn_->LPlace.Lst().end();
	 ++i)
      {
 	for (std::list<Arc*>::iterator j = (*i).getArcIn().begin();
	     j != (*i).getArcIn().end();)
	  if ((*j)->Valuation().Lst().empty())
	    {
	      std::list<Arc*>::iterator remove = j++;
	      (*i).getArcIn().erase(remove);
	    }
	  else
	    ++j;
	for (std::list<Arc*>::iterator j = (*i).getArcOut().begin();
	     j != (*i).getArcOut().end();)
	  if ((*j)->Valuation().Lst().empty())
	    {
	      std::list<Arc*>::iterator remove = j++;
	      (*i).getArcOut().erase(remove);
	    }
	  else
	    ++j;
      }
    for (std::set<Arc>::iterator i = this->to_pn_->LArc.get().begin();
	 i != this->to_pn_->LArc.get().end();)
      if ((*i).Valuation().Lst().empty())
	{
	  std::set<Arc>::iterator remove = i++;
	  this->to_pn_->LArc.get().erase(remove);
	}
      else
	++i;
  }

  void
  ddd_to_cpn_t::create_petri_net ()
  {
    /*
    SDD places_sdd(SDD::one);
    for (ddds_t::reverse_iterator i = this->net_.places().rbegin();
	 i != this->net_.places().rend();
	 ++i)
      places_sdd = SDD(0, (*i).second.ddd()) ^ places_sdd;
    exportDot(places_sdd, "places");
    SDD transitions_sdd;
    for (ddds_t::reverse_iterator i = this->net_.transitions().rbegin();
	 i != this->net_.transitions().rend();
	 ++i)
      transitions_sdd = SDD(0, (*i).second.ddd()) ^ transitions_sdd;
    exportDot(transitions_sdd, "transitions");
    */
    // Create classes :
    cerr << "Create classes...";
    cerr.flush();
    this->create_classes();
    cerr << "done." << endl;
    // Create domains :
    cerr << "Create domains...";
    cerr.flush();
    this->create_domains();
    cerr << "done." << endl;
    // Create variables :
    cerr << "Create variables...";
    cerr.flush();
    this->create_variables();
    cerr << "done." << endl;
    // Create unfolded places :
    cerr << "Create places...";
    cerr.flush();
    this->create_places();
    cerr << "done." << endl;
    // Create unfolded transitions :
    cerr << "Create transitions...";
    cerr.flush();
    this->create_transitions();
    cerr << "done." << endl;
    // Create markings :
    cerr << "Create markings...";
    cerr.flush();
    this->create_markings();
    cerr << "done." << endl;
    // Create arcs :
    cerr << "Create arcs...";
    cerr.flush();
    this->create_arcs();
    cerr << "done." << endl;
  }

}
