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
 
#ifndef __CPN_DDD__CPN_TO_DDD__
#define __CPN_DDD__CPN_TO_DDD__

#include <PNetIO.h>
#include <PNet.h>
#include <Place.h>
#include <Transition.h>
#include <Arc.h>
#include <Domain.h>
#include <PNClass.h>
#include <Element.h>
#include <Marking.h>

#include <vector>

#include "ddd_common.hh"
#include "ddd_unfold.hh"

namespace cpn_ddd
{

  class cpn_to_ddd_t
  {
  private:
    static DDD transform_class (PNClass* c, identifier_t id, DDD& parent);
    bool transformed_;
    void transform (unfold_ddd_t& pt);
    void transform ();
  protected:
    net_t pn_ddd_;
    classes_t unfold_;
    identifiers_t& identifiers_;
  public:
    cpn_to_ddd_t (PNet* petri_net, classes_t& unfold, identifiers_t& identifiers);
    virtual ~cpn_to_ddd_t ();
    net_t& get_ddds ();
  };

}

#endif //__CPN_DDD__CPN_TO_DDD__
