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
 
#include "cpn_to_ddd.hh"
#include "variable_order_same.hh"
#include "variable_order_reverse.hh"
#include <algorithm>

namespace cpn_ddd
{

  cpn_to_ddd_t::cpn_to_ddd_t (PNet* petri_net,
			      classes_t& unfold_classes,
			      identifiers_t& identifiers)
    : transformed_(false),
      pn_ddd_(petri_net),
      unfold_(unfold_classes),
      identifiers_(identifiers)
  {
    //    this->transform();
  }

  cpn_to_ddd_t::~cpn_to_ddd_t ()
  {}

  net_t&
  cpn_to_ddd_t::get_ddds ()
  {
    if (! this->transformed_)
      {
	this->transform();
	this->transformed_ = true;
      }
    return this->pn_ddd_;
  }

  void
  cpn_to_ddd_t::transform ()
  {
    DDD ddd_one = DDD::one;
    PNet* net = this->pn_ddd_.net();
    std::list<Place>& all_places = net->LPlace.Lst ();
    ddds_t& places = this->pn_ddd_.places();	
    ddds_t& transitions = this->pn_ddd_.transitions();
    for (std::list<Place>::iterator i = all_places.begin ();
	 i != all_places.end ();
	 ++i)
      {
	identifier_t id = this->identifiers_.add(&(*i));
	unfold_ddd_t new_ddd(ddd_one, new VARIABLE_ORDER (*i));
	this->transform(new_ddd);
	places.insert(make_pair(id,new_ddd));
      }
    std::list<Transition>& all_transitions = net->LTrans.Lst ();
    for (std::list<Transition>::iterator i = all_transitions.begin ();
	 i != all_transitions.end ();
	 ++i)
      {
	identifier_t id = this->identifiers_.add(&(*i));
	unfold_ddd_t new_ddd(ddd_one, new VARIABLE_ORDER (*i));
	this->transform(new_ddd);
	transitions.insert(make_pair(id, new_ddd));
      }
  }

  void
  cpn_to_ddd_t::transform (unfold_ddd_t& pt)
  {
    variable_order_t* order = pt.order();
    variable_order_t::infos_t infos = order->infos(this->unfold_);
    variable_order_t::conversion_t conversion = order->reverse_order(this->unfold_);
    DDD& current_ddd = pt.ddd();
    current_ddd = DDD::one;
    for (variable_order_t::conversion_t::const_iterator i =
	   conversion.begin();
	 i != conversion.end();
	 ++i)
      current_ddd = this->transform_class(infos[(*i).second].class_,
					  (*i).first,
					  current_ddd);
  }

  DDD
  cpn_to_ddd_t::transform_class (PNClass* c, identifier_t id, DDD& parent)
  {
    std::vector<Element>& elements = c->Elts();    
    std::vector<Element>::iterator first = elements.begin();
    std::vector<Element>::iterator last = elements.end();
    last--; // never first == last or error in model
    DDD result(id, (*first).Id(), (*last).Id(), parent);
    return result;
  }

}
