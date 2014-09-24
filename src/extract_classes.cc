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
 
#include <PNetIO.h>
#include <PNet.h>
#include <PNClass.h>

#include <iostream>
#include <cstdlib>

#ifdef FRAMEKIT_SUPPORT
#include <FKMain.h>
#include <FKModel.h>
#include <FKEnv.h>
#include <FKCLSend.h>
#include "FKServVersions_ExtractClasses.h"
#endif //FRAMEKIT_SUPPORT

using namespace std;

typedef vector<PNClass*> classes_t;

void usage (string name)
{
#ifdef FRAMEKIT_SUPPORT
  if (g_fkStandalone)
    cerr << name << " input_file output_file" << endl
	 << "Where :" << endl
	 << "\tinput_file is the name of input CAMI file," << endl
	 << "\toutput_file is the name of output file," << endl;
  else
    cerr << name << endl;
#else
  cerr << name << " input_file output_file" << endl
       << "Where :" << endl
       << "\tinput_file is the name of input CAMI file," << endl
       << "\toutput_file is the name of output file," << endl;
#endif
}

#ifdef FRAMEKIT_SUPPORT
  FkEndStatus FkServiceMain(int argc, char* argv[])
#else
  int main (int argc, char* argv[])
#endif
{
  // Check arguments number :
#ifdef FRAMEKIT_SUPPORT
  if (g_fkStandalone)
    {
      if (argc < 3)
	{
	  usage(argv[0]);
	  return kFk_EndOnError;
	}
    }
  else
    if (argc < 2)
      {
	usage(argv[0]);
	return kFk_EndOnError;
      }
#else
  if (argc < 3)
    {
      usage(argv[0]);
      return EXIT_FAILURE;
    }
#endif
  // Files :
  string model_file;
  string output_file;
  PNet* coloured_net;
  classes_t classes;
#ifdef FRAMEKIT_SUPPORT
  if (g_fkStandalone)
    {
      model_file = argv[1];
      output_file = argv[2];
      coloured_net = parseCAMI(model_file);
    }
  else
    {
      model_file = FkGetModelFileName();
      output_file = ".pnet_classes";
      coloured_net = parseCAMI(model_file);
    }
#else
  model_file = argv[1];
  output_file = argv[2];
  coloured_net = parseCAMI(model_file);
#endif
  ofstream output(output_file.c_str(), ios_base::out | ios_base::trunc);
  for (list<PNClass>::iterator i = coloured_net->LClasse.Lst().begin();
       i != coloured_net->LClasse.Lst().end();
       ++i)
    output << (*i).Name() << endl;
  delete coloured_net;
#ifdef FRAMEKIT_SUPPORT
  return kFk_EndNoProblem;
#else
  return EXIT_SUCCESS;
#endif
}
