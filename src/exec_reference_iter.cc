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
#include "ddd_to_cpn.hh"
#include "ddd_operation.hh"
#include "ddd_guard.hh"
#include "ddd_syphon_reference.hh"
#include "ddd_marked_orphan.hh"
#include "ddd_count.hh"
#include "ddd_compare_firstopt.hh"

#include <sys/resource.h>
#include <limits>

using namespace std;
using namespace cpn_ddd;

typedef map<unsigned long, struct rusage> usages_t;
typedef vector< pair<string, ddd_operation_t*> > operations_t;

void exec (PNet* coloured_net,
	   classes_t unfold_classes,
	   usages_t& usages)
{
  unsigned long ri = numeric_limits<unsigned long>::min();
  if (usages.end() != usages.begin())
    ri = (*(--usages.end())).first + 1;
  getrusage(RUSAGE_SELF, &usages[ri++]);
  identifiers_t identifiers;
  arcs_t arcs(coloured_net, identifiers);
  cpn_to_ddd_t convert(coloured_net, unfold_classes, identifiers);
  net_t& net = convert.get_ddds();
  for (ddds_t::iterator i = net.places().begin();
       i != net.places().end();
       ++i)
    (*i).second.ddd() = DDD::one;
  for (ddds_t::iterator i = net.transitions().begin();
       i != net.transitions().end();
       ++i)
    (*i).second.ddd() = DDD::one;
  for (unsigned int i = 0; i < unfold_classes.size(); ++i)
    {
      identifiers_t part_identifiers = identifiers;
      arcs_t part_arcs(coloured_net, part_identifiers);
      classes_t part_classes(1, unfold_classes[i]);
      cpn_to_ddd_t part_convert(coloured_net, part_classes, part_identifiers);
      net_t& part_net = part_convert.get_ddds();

      vector< pair<string, ddd_operation_t*> > operations;
      operations.push_back
	(make_pair("guards",
		   new ddd_guard_t(part_net, part_classes, part_identifiers)));
      operations.push_back
	(make_pair("syphon",
		   new ddd_syphon_reference_t(part_net, part_classes, part_identifiers, part_arcs)));
      operations.push_back
	(make_pair("loose marked places",
		   new ddd_marked_orphan_t(part_net, part_classes, part_identifiers, part_arcs)));
      operations.push_back
	(make_pair("size",
		   new ddd_count_t(part_net, part_classes, part_identifiers)));
      for (vector< pair<string, ddd_operation_t*> >::iterator j = operations.begin();
	   j != operations.end();
	   ++j)
	{
	  clog << "Apply " << (*j).first
	       << " (" << part_classes[0]->Name()
	       << ")...";
	  clog.flush();
	  (*j).second->transform();
	  clog << "done." << endl;
	  getrusage(RUSAGE_SELF, &usages[ri++]);
	}
      for (vector< pair<string, ddd_operation_t*> >::iterator j = operations.begin();
	   j != operations.end();
	   ++j)
	delete (*j).second;

      // Add result to final DDDs :
      ddds_t& full_places = net.places();
      ddds_t& places = part_net.places();
      for (ddds_t::iterator j = places.begin();
	   j != places.end();
	   ++j)
	{
	  ddds_t::iterator k = full_places.find((*j).first);
	  assert(k != full_places.end());
	  rename_full_t::names_t names;
	  variable_order_t* full_order = (*k).second.order();
	  variable_order_t::infos_t full_infos = full_order->infos(unfold_classes);
	  variable_order_t::conversion_t full_conversion = full_order->order(unfold_classes);
	  variable_order_t* order = (*j).second.order();
	  variable_order_t::conversion_t conversion = order->order(part_classes);
	  unsigned int iter = 0;
	  for (unsigned int l = 0; l < full_infos.size(); ++l)
	    if (full_infos[l].class_ == part_classes[0])
	      names[conversion[iter++]] = full_conversion[l];
	  Hom rename(new rename_full_t(names));
	  (*k).second.ddd() = (*k).second.ddd() ^ rename((*j).second.ddd());
	}
      ddds_t& full_transitions = net.transitions();
      ddds_t& transitions = part_net.transitions();
      for (ddds_t::iterator j = transitions.begin();
	   j != transitions.end();
	   ++j)
	{
	  ddds_t::iterator k = full_transitions.find((*j).first);
	  assert(k != full_transitions.end());
	  rename_full_t::names_t names;
	  variable_order_t* full_order = (*k).second.order();
	  variable_order_t::infos_t full_infos = full_order->infos(unfold_classes);
	  variable_order_t::conversion_t full_conversion = full_order->order(unfold_classes);
	  variable_order_t* order = (*j).second.order();
	  variable_order_t::conversion_t conversion = order->order(part_classes);
	  unsigned int iter = 0;
	  for (unsigned int l = 0; l < full_infos.size(); ++l)
	    if (full_infos[l].class_ == part_classes[0])
	      names[conversion[iter++]] = full_conversion[l];
	  Hom rename(new rename_full_t(names));
	  (*k).second.ddd() = (*k).second.ddd() ^ rename((*j).second.ddd());
	}
      getrusage(RUSAGE_SELF, &usages[ri++]);
    }
  // Reorder
  for (ddds_t::iterator i = net.places().begin();
       i != net.places().end();
       ++i)
    {
      identifier_t before = move_variable_t::ONE_IDENTIFIER;
      variable_order_t::conversion_t conversion =
	(*i).second.order()->reverse_order(unfold_classes);
      for (variable_order_t::conversion_t::reverse_iterator j = conversion.rbegin();
	   j != conversion.rend();
	   ++j)
	{
	  Hom move (new move_variable_t(before, (*j).first));
	  (*i).second.ddd() = move((*i).second.ddd());
	  before = (*j).first;
	}
    }
  for (ddds_t::iterator i = net.transitions().begin();
       i != net.transitions().end();
       ++i)
    {
      identifier_t before = move_variable_t::ONE_IDENTIFIER;
      variable_order_t::conversion_t conversion =
	(*i).second.order()->reverse_order(unfold_classes);
      for (variable_order_t::conversion_t::reverse_iterator j = conversion.rbegin();
	   j != conversion.rend();
	   ++j)
	{
	  Hom move (new move_variable_t(before, (*j).first));
	  (*i).second.ddd() = move((*i).second.ddd());
	  before = (*j).first;
	}
    }
  //    MemoryManager::garbage();
  getrusage(RUSAGE_SELF, &usages[ri++]);
  vector< pair<string, ddd_operation_t*> > operations;
  operations.push_back
    (make_pair("guards",
	       new ddd_guard_t(net, unfold_classes, identifiers)));
  operations.push_back
    (make_pair("syphon",
	       new ddd_syphon_reference_t(net, unfold_classes, identifiers, arcs)));
  operations.push_back
    (make_pair("loose marked places",
	       new ddd_marked_orphan_t(net, unfold_classes, identifiers, arcs)));
  operations.push_back
    (make_pair("sizes",
	       new ddd_count_t(net, unfold_classes, identifiers)));
  operations.push_back
    (make_pair("conversion to libPetri",
	       new ddd_to_cpn_t(coloured_net, net, unfold_classes, identifiers, arcs)));
  for (vector< pair<string, ddd_operation_t*> >::iterator i = operations.begin();
       i != operations.end();
       ++i)
    {
      clog << "Apply " << (*i).first << "...";
      clog.flush();
      (*i).second->transform();
      clog << "done." << endl;
      getrusage(RUSAGE_SELF, &usages[ri++]);
    }
  for (vector< pair<string, ddd_operation_t*> >::iterator i = operations.begin();
       i != operations.end();
       ++i)
    delete (*i).second;
}
