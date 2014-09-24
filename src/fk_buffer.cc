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
 
#include "fk_buffer.hh"

#include <cstdio>
#include <cstring>

#include <string>
#include <sstream>

#ifdef FRAMEKIT_SUPPORT

#include <FKCLSend.h>
#include <FKCLDecode.h>
#include <FKEnv.h>
#include <FKModel.h>
#include <FKResults.h>

namespace frame_kit
{

  stream_buffer_t::stream_buffer_t () :
    std::stringbuf ()
  {
    std::stringstream os;
    os << std::endl;
    const char* endline = os.str().c_str();
    this->endline_ = new char[strlen(endline)+1];
    this->endline_[strlen(endline)+1] = '\0';
    this->endline_ = strncpy(this->endline_, endline, strlen(endline));
  }
    
  stream_buffer_t::~stream_buffer_t ()
  {
    delete[] this->endline_;
  }

  int
  stream_buffer_t::sync ()
  {
    if (this->str() == this->endline_)
      return 0;
    const char* const c = (this->str().c_str());
    const size_t length = strlen(c);
    char* t = new char[length+1];
    t = std::strncpy(t, c, length);
    t[length] = '\0';
    char* token = std::strtok(t, this->endline_);
    while (token != NULL)
      {
	FkSendWaitingMessage(token);
	token = std::strtok(NULL, this->endline_);
      }
    delete[] t;
    this->str("");
    return 0;
  }

}

#endif
