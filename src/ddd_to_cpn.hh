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
 
#ifndef __CPN_DDD__DDD_TO_CPN__
#define __CPN_DDD__DDD_TO_CPN__

#include <PNet.h>
#include <Place.h>
#include <Transition.h>

#include <DDD.h>

#include <ext/hash_map>
#include "ddd_common.hh"
#include "ddd_operation.hh"
#include "ddd_unfold.hh"

namespace cpn_ddd
{

  class identifiers_hash_t
  {
  public:
    const size_t operator() (const identifier_t id) const;
  };

  class ddd_to_cpn_t : public ddd_operation_t
  {
  public:
    typedef std::vector< std::vector<bool> > elements_t;
  protected:
    typedef std::vector< std::pair<values_t, identifier_t> > places_t;
    typedef std::vector< std::pair<values_t, identifier_t> > transitions_t;
    typedef std::vector<values_t> result_t;
    typedef __gnu_cxx::hash_map<identifier_t, places_t, identifiers_hash_t> places_map_t;
    typedef __gnu_cxx::hash_map<identifier_t, transitions_t, identifiers_hash_t> transitions_map_t;
    typedef __gnu_cxx::hash_map<identifier_t, variable_order_t*, identifiers_hash_t> order_map_t;
    typedef std::map<PNClass*, PNClass*> classes_map_t;
    typedef std::map<Domain*, Domain*> domains_map_t;

    bool transformed_;
    PNet* from_pn_;
    PNet* to_pn_;
    arcs_t& arcs_;
    classes_map_t classes_map_;
    domains_map_t domains_map_;
    places_map_t places_;
    transitions_map_t transitions_;
    order_map_t places_order_;
    order_map_t transitions_order_;
    identifier_t id_;
    identifier_t macao_id_;
    bool print_result_;

    void create_places (result_t& result,
			values_t& values,
			unfold_ddd_t& ddd,
			DDD::const_iterator& i);

    void create_places (identifier_t id,
			unfold_ddd_t& ddd);

    void create_transitions (result_t& result,
			     values_t& values,
			     unfold_ddd_t& ddd,
			     DDD::const_iterator& i);

    void create_transitions (identifier_t id,
			     unfold_ddd_t& ddd);

    CFuncTerm create_function (const CFuncTerm& term);

    Guard* create_guard (Guard* guard,
			 variable_order_t* order,
			 values_t& values);

    //    std::vector<elements_t>
    elements_t
    eval_arc_from_transition (const CFuncTerm& term,
			      const unfold_ddd_t& place,
			      const std::map<variable_order_t::variable_id_t,	value_t>& variables);
    void create_arcs_from_transitions ();
    //    std::vector<elements_t>
    elements_t
    eval_arc_from_place (const CFuncTerm& term,
			 const unfold_ddd_t& place,
			 const unfold_ddd_t& transition,
			 const values_t& values);
    void create_arcs_from_places ();
    void create_arcs ();

    void create_marking (const Place* const from,
			 Place* const to,
			 const variable_order_t* const order,
			 const values_t& values);

    void create_domains ();
    void create_classes ();
    void create_variables ();
    void create_markings ();
    void create_places ();
    void create_transitions ();
    void create_petri_net ();
  public:
    ddd_to_cpn_t (PNet* petri_net,
		  net_t& net,
		  const classes_t& classes,
		  identifiers_t& identifiers,
		  arcs_t& arcs,
		  bool print_result);
    ~ddd_to_cpn_t ();
    void transform ();
    PNet* get ();
  };

}

#endif //__CPN_DDD__DDD_TO_CPN__
