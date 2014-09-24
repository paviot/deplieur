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
 
#include "ddd_unfold.hh"

#include <hashfunc.hh>
#include <PNet.h>

namespace cpn_ddd
{

  const size_t
  elts_hash_t::operator() (const std::vector<Element>* const elts) const
  {
    return ddd::knuth32_hash(reinterpret_cast<size_t>(elts));
  }

  unfold_ddd_t::unfold_ddd_t (const DDD& ddd,
			      variable_order_t* const order)
    : ddd_(ddd),
      order_(order)
  {}

  unfold_ddd_t::unfold_ddd_t (const unfold_ddd_t& to_copy)
    : ddd_(to_copy.ddd_),
      order_(to_copy.order_)
  {}

  unfold_ddd_t::~unfold_ddd_t ()
  {}

  unfold_ddd_t&
  unfold_ddd_t::operator= (const unfold_ddd_t& to_copy)
  {
    if (this != &to_copy)
      {
	this->ddd_ = to_copy.ddd_;
	this->order_ = to_copy.order_;
      }
    return *this;
  }

  variable_order_t* const
  unfold_ddd_t::order () const
  {
    return this->order_.get();
  }

  const DDD&
  unfold_ddd_t::ddd () const
  {
    return this->ddd_;
  }

  DDD&
  unfold_ddd_t::ddd ()
  {
    return this->ddd_;
  }

  net_t::net_t (PNet* const net) :
    places_(),
    transitions_(),
    net_(net),
    null_domain_(NULL),
    extract_cache_(net->LClasse.size())
  {}

  net_t::net_t (const net_t& to_copy) :
    places_(to_copy.places_),
    transitions_(to_copy.transitions_),
    net_(to_copy.net_),
    null_domain_(to_copy.null_domain_),
    extract_cache_()
  {}

  net_t::~net_t ()
  {}

  net_t&
  net_t::operator= (const net_t& to_copy)
  {
    if (this != &to_copy)
      {
	this->places_ = to_copy.places_;
	this->transitions_ = to_copy.transitions_;
	this->net_ = to_copy.net_;
	this->null_domain_ = to_copy.null_domain_;
	this->extract_cache_.clear();
      }
    return *this;
  }

  ddds_t&
  net_t::places ()
  {
    return this->places_;
  }

  ddds_t&
  net_t::transitions ()
  {
    return this->transitions_;
  }

  PNet* const
  net_t::net ()
  {
    return this->net_;
  }

  Domain* const
  net_t::null_domain ()
  {
    return this->null_domain_;
  }

  void
  net_t::null_domain (Domain* const domain)
  {
    if (this->null_domain_ != NULL)
      delete this->null_domain_;
    this->null_domain_ = domain;
  }

  const values_t
  net_t::extract_cached (const std::vector<Element>& elements) const
  {
    results_t::const_iterator i = this->extract_cache_.find(&elements);
    if (i != this->extract_cache_.end())
      return (*i).second;
    values_t result;
    for (std::vector<Element>::const_iterator i = elements.begin();
	 i != elements.end();
	 ++i)
      result.push_back((const_cast<Element&>(*i)).Id());
    this->extract_cache_.insert(make_pair(&elements, result));
    return result;
  }

  const values_t
  net_t::extract (const std::vector<Element>& elements) const
  {
    values_t result;
    for (std::vector<Element>::const_iterator i = elements.begin();
	 i != elements.end();
	 ++i)
      result.push_back((const_cast<Element&>(*i)).Id());
    return result;
  }

  unfold_hierarchy_t::unfold_hierarchy_t () :
    hierarchy_(),
    last_(identifiers_t::UNKNOWN_IDENTIFIER)
  {}

  unfold_hierarchy_t::unfold_hierarchy_t (const unfold_hierarchy_t& to_copy) :
    hierarchy_(to_copy.hierarchy_),
    last_(to_copy.last_)
  {}

  unfold_hierarchy_t::~unfold_hierarchy_t ()
  {}

  unfold_hierarchy_t&
  unfold_hierarchy_t::operator= (const unfold_hierarchy_t& to_copy)
  {
    if (this != &to_copy)
      {
	this->hierarchy_ = to_copy.hierarchy_;
	this->last_ = identifiers_t::UNKNOWN_IDENTIFIER;
      }
    return *this;
  }

  const identifier_t
  unfold_hierarchy_t::parent (const identifier_t id) const
  {
    if (this->last_ != identifiers_t::UNKNOWN_IDENTIFIER)
      {
	const ids_t& ids = (*this->hierarchy_.find(this->last_)).second;
	ids_t::const_iterator j = std::find(ids.begin(), ids.end(), id);
	if (j != ids.end())
	  return this->last_;
      }
    for (map_t::const_iterator i = this->hierarchy_.begin();
	 i != this->hierarchy_.end();
	 ++i)
      {
	ids_t::const_iterator j = std::find((*i).second.begin(), (*i).second.end(), id);
	if (j != (*i).second.end())
	  {
	    this->last_ = (*i).first;
	    return this->last_;
	  }
      }
    return identifiers_t::UNKNOWN_IDENTIFIER;
  }

  ids_t&
  unfold_hierarchy_t::unfolded (const identifier_t id)
  {
    return this->hierarchy_[id];
  }

  arcs_t::arcs_t (PNet* const net,
		  identifiers_t& identifiers) :
    net_(net),
    identifiers_(&identifiers),
    unfolded_places_(),
    unfolded_transitions_(),
    pre_transitions_arcs_(),
    post_transitions_arcs_(),
    pre_places_arcs_(),
    post_places_arcs_()
  {
    this->create_arcs();
  }

  arcs_t::arcs_t (const arcs_t& to_copy) :
    net_(to_copy.net_),
    identifiers_(to_copy.identifiers_),
    unfolded_places_(to_copy.unfolded_places_),
    unfolded_transitions_(to_copy.unfolded_transitions_),
    pre_transitions_arcs_(to_copy.pre_transitions_arcs_),
    post_transitions_arcs_(to_copy.post_transitions_arcs_),
    pre_places_arcs_(to_copy.pre_places_arcs_),
    post_places_arcs_(to_copy.post_places_arcs_)
  {}

  arcs_t::~arcs_t ()
  {}

  arcs_t&
  arcs_t::operator= (const arcs_t& to_copy)
  {
    if (this != &to_copy)
      {
	this->net_ = to_copy.net_;
	this->identifiers_ = to_copy.identifiers_;
	this->unfolded_places_ = to_copy.unfolded_places_;
	this->unfolded_transitions_ = to_copy.unfolded_transitions_;
	this->pre_transitions_arcs_ = to_copy.pre_transitions_arcs_;
	this->post_transitions_arcs_ = to_copy.post_transitions_arcs_;
	this->pre_places_arcs_ = to_copy.pre_places_arcs_;
	this->post_places_arcs_ = to_copy.post_places_arcs_;
      }
    return *this;
  }

  const arcs_t::infos_t
  arcs_t::pre (const identifier_t id,
	       const type_t type) const
  {
    const unfold_hierarchy_t* hierarchy;
    const arcs_map_t* arcs;
    switch (type)
      {
      case PLACE:
	hierarchy = &this->unfolded_places_;
	arcs = &this->pre_places_arcs_;
	break;
      case TRANSITION:
	hierarchy = &this->unfolded_transitions_;
	arcs = &this->pre_transitions_arcs_;
	break;
      default:
	hierarchy = NULL;
	arcs = NULL;
      }
    infos_t result;
    arcs_map_t::const_iterator i = arcs->find(id);
    if (i == arcs->end())
      i = arcs->find(hierarchy->parent(id));
    if (i == arcs->end())
      return result;
    return (*i).second;
  }

  const arcs_t::infos_t
  arcs_t::post (const identifier_t id,
		const type_t type) const
  {
    const unfold_hierarchy_t* hierarchy;
    const arcs_map_t* arcs;
    switch (type)
      {
      case PLACE:
	hierarchy = &this->unfolded_places_;
	arcs = &this->post_places_arcs_;
	break;
      case TRANSITION:
	hierarchy = &this->unfolded_transitions_;
	arcs = &this->post_transitions_arcs_;
	break;
      default:
	hierarchy = NULL;
	arcs = NULL;
      }
    infos_t result;
    arcs_map_t::const_iterator i = arcs->find(id);
    if (i == arcs->end())
      i = arcs->find(hierarchy->parent(id));
    if (i == arcs->end())
      return result;
    return (*i).second;
  }

  void
  arcs_t::create_arcs ()
  {
    std::list<Transition>& transitions = this->net_->LTrans.Lst();
    for (std::list<Transition>::iterator i = transitions.begin();
	 i != transitions.end();
	 ++i)
      {
	identifier_t t_id = this->identifiers_->add(&(*i));
	const std::list<Arc*> arcs_in = (*i).getArcIn();
	for (std::list<Arc*>::const_iterator j = arcs_in.begin();
	     j != arcs_in.end();
	     ++j)
	  {
	    Place* place = (*j)->getPlace();
	    identifier_t p_id = this->identifiers_->add(place);
	    this->pre_transitions_arcs_[t_id][p_id] = *j;
	    this->post_places_arcs_[p_id][t_id] = *j;
	  }
	const std::list<Arc*> arcs_out = (*i).getArcOut();
	for (std::list<Arc*>::const_iterator j = arcs_out.begin();
	     j != arcs_out.end();
	     ++j)
	  {
	    Place* place = (*j)->getPlace();
	    identifier_t p_id = this->identifiers_->add(place);
	    this->post_transitions_arcs_[t_id][p_id] = *j;
	    this->pre_places_arcs_[p_id][t_id] = *j;
	  }
      }
  }

  unfold_hierarchy_t&
  arcs_t::unfolded_places ()
  {
    return this->unfolded_places_;
  }

  unfold_hierarchy_t&
  arcs_t::unfolded_transitions ()
  {
    return this->unfolded_transitions_;
  }

}
