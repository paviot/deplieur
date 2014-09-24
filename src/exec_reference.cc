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
#include "ddd_syphon_add.hh"
#include "ddd_syphon_bubble.hh"
#include "ddd_marked_orphan.hh"
#include "ddd_count.hh"
#include "usage.hh"

#include <sys/resource.h>

using namespace std;
using namespace cpn_ddd;

typedef std::vector< std::pair< std::pair<string, string>, ddd_operation_t*> > operations_t;

void exec (PNet* coloured_net,
	   classes_t unfold_classes,
	   const std::set<std::string>& optimizations,
	   usages_t& usages)
{
  /*
    cout << "Places : " << coloured_net->LPlace.size()
    << "\tTransitions : " << coloured_net->LTrans.size()
    << "\tArcs : " << coloured_net->LArc.size()
    << endl;
  */
  // Convert to symbolic represenatation :
  identifiers_t identifiers;
  arcs_t arcs(coloured_net, identifiers);
  cerr << "Convert to DDDs...";
  cerr.flush();
  cpn_to_ddd_t convert(coloured_net, unfold_classes, identifiers);
  net_t& nd = convert.get_ddds();
  cerr << "done." << endl;
  //  get_usage(nd, usages);
  // Apply operations :
  operations_t operations;
  if (optimizations.find("size") != optimizations.end())
    operations.push_back
      (make_pair(make_pair("show sizes", "done"),
		 new ddd_count_t(nd, unfold_classes, identifiers)));
  if (optimizations.find("guard") != optimizations.end())
    operations.push_back
      (make_pair(make_pair("transitions with false guards", "done"),
		 new ddd_guard_t(nd, unfold_classes, identifiers)));
  if (optimizations.find("syphon") != optimizations.end())
    operations.push_back
      (make_pair(make_pair("maximal unmarked syphon (reference)", "done"),
		 new ddd_syphon_reference_t(nd, unfold_classes, identifiers, arcs)));
  else if (optimizations.find("syphon-add") != optimizations.end())
    operations.push_back
      (make_pair(make_pair("maximal unmarked syphon (add)", "done"),
		 new ddd_syphon_add_t(nd, unfold_classes, identifiers, arcs)));
  else if (optimizations.find("syphon-bubble") != optimizations.end())
    operations.push_back
      (make_pair(make_pair("maximal unmarked syphon (bubble)", "done"),
		 new ddd_syphon_bubble_t(nd, unfold_classes, identifiers, arcs)));
  if (optimizations.find("orphan") != optimizations.end())
    operations.push_back
      (make_pair(make_pair("orphaned marked places", "done"),
		 new ddd_marked_orphan_t(nd, unfold_classes, identifiers, arcs)));
  if (optimizations.find("size") != optimizations.end())
    operations.push_back
      (make_pair(make_pair("show sizes", "done"),
		 new ddd_count_t(nd, unfold_classes, identifiers)));
  if (optimizations.find("pnet") != optimizations.end())
    {
      ddd_operation_t* pnet_opt = NULL;
      if (optimizations.find("show") != optimizations.end())
	pnet_opt = new ddd_to_cpn_t(coloured_net, nd, unfold_classes, identifiers, arcs, true);
      else
	pnet_opt = new ddd_to_cpn_t(coloured_net, nd, unfold_classes, identifiers, arcs, false);
      operations.push_back
	(make_pair(make_pair("conversion to libPetri",
#ifdef FRAMEKIT_SUPPORT
			     "please wait..."),
#else
		             "done."),
#endif
	 pnet_opt));
    }
  for (operations_t::iterator i = operations.begin();
       i != operations.end();
       ++i)
    {
      cerr << "Apply " << (*i).first.first << "...";
      cerr.flush();
      (*i).second->transform();
      cerr << (*i).first.second << endl;
      //      get_usage(nd, usages);
    }
  // Clean :
  for (operations_t::iterator i = operations.begin();
       i != operations.end();
       ++i)
    delete (*i).second;
}
