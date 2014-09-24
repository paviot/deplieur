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
#include "ddd_syphon_firstopt.hh"
#include "ddd_marked_orphan.hh"
#include "ddd_count.hh"

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
  // Convert to symbolic represenatation :
  getrusage(RUSAGE_SELF, &usages[ri++]);
  identifiers_t identifiers;
  arcs_t arcs(coloured_net, identifiers);
  clog << "Convert to DDDs...";
  clog.flush();
  cpn_to_ddd_t convert(coloured_net, unfold_classes, identifiers);
  net_t& nd = convert.get_ddds();
  clog << "done." << endl;
  // Apply operations :
  getrusage(RUSAGE_SELF, &usages[ri++]);
  operations_t operations;
  operations.push_back
    (make_pair("sizes",
	       new ddd_count_t(nd, unfold_classes, identifiers)));
  operations.push_back
    (make_pair("guards",
	       new ddd_guard_t(nd, unfold_classes, identifiers)));
  operations.push_back
    (make_pair("syphon",
	       new ddd_syphon_firstopt_t(nd, unfold_classes, identifiers, arcs)));
  operations.push_back
    (make_pair("loose marked places",
	       new ddd_marked_orphan_t(nd, unfold_classes, identifiers, arcs)));
  operations.push_back
    (make_pair("sizes",
	       new ddd_count_t(nd, unfold_classes, identifiers)));
  operations.push_back
    (make_pair("conversion to libPetri",
	       new ddd_to_cpn_t(coloured_net, nd, unfold_classes, identifiers, arcs)));
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
  // Clean :
  for (vector< pair<string, ddd_operation_t*> >::iterator i = operations.begin();
       i != operations.end();
       ++i)
    delete (*i).second;
}
