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
 
#define COL2ORDEQUIVDDD_VERSION "20/07/2006"

#include "fk_buffer.hh"
#include "ddd_common.hh"
#include "usage.hh"

#include <PNetIO.h>
#include <PNet.h>

#include <DDD.h>
#include <MemoryManager.h>

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <math.h>

#ifdef FRAMEKIT_SUPPORT
#include <FKMain.h>
#include <FKModel.h>
#include <FKEnv.h>
#include <FKCLSend.h>
#include "FKServVersions.h"
#endif //FRAMEKIT_SUPPORT

using namespace std;
using namespace cpn_ddd;

typedef set<string> opts_t;

void exec(PNet*, classes_t, const opts_t&, usages_t&);

void usage (string name)
{
  cerr << name
       << " [-cC0 .. -cCn | -ac] [-oO0 .. -oOm | -ao] input_file output_file"
       << endl
       << "Where :" << endl
       << "\tC0..Cn are names of colour classes to unfold," << endl
       << "\tinput_file is the name of input CAMI file," << endl
       << "\toutput_file is the name of output CAMI file." << endl;
}

#ifdef FRAMEKIT_SUPPORT
  FkEndStatus FkServiceMain(int argc, char* argv[])
#else
  int main (int argc, char* argv[])
#endif
{
  std::cerr << "col2ordequivDDD, version : "
	    << COL2ORDEQUIVDDD_VERSION
	    << std::endl;
  // Check arguments number :
#ifdef FRAMEKIT_SUPPORT
  if (g_fkStandalone)
    if (argc < 3)
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
  // Resource usages :
  usages_t usages;
  // Files :
  string model_file;
  string output_file;
  PNet* coloured_net;
  classes_t unfold_classes;
  opts_t optimizations;
#ifdef FRAMEKIT_SUPPORT
  if (g_fkStandalone)
    {
      model_file = argv[argc-2];
      output_file = argv[argc-1];
      coloured_net = parseCAMI(model_file);
      for (int narg = 1; narg < argc-2; ++narg)
	{
	  string arg(argv[narg]);
	  std::cerr << arg << std::endl;
	  if (arg == "-ao")
	    {
	      optimizations.insert("guard");
	      optimizations.insert("syphon");
	      optimizations.insert("syphon-add");
	      optimizations.insert("syphon-bubble");
	      optimizations.insert("orphan");
	      optimizations.insert("size");
	      optimizations.insert("pnet");
	      optimizations.insert("show");
	    }
	  else if (arg == "-ac")
	    {
	      for (list<PNClass>::iterator i =
		     coloured_net->LClasse.Lst().begin();
		   i != coloured_net->LClasse.Lst().end();
		   ++i)
		unfold_classes.push_back(&(*i));
	    }
	  else if (arg.find("-c", 0) == 0)
	    {
	      string name = arg.substr(2, arg.size()-2);
	      PNClass* c = coloured_net->LClasse.FindName(name);
	      if (c != NULL)
		unfold_classes.push_back(c);
	      else
		cerr << "Unknown colour class : " << name << endl;
	    }
	  else if (arg.find("-o", 0) == 0)
	    {
	      optimizations.insert(arg.substr(2, arg.size()-2));
	    }
	}
    }
  else
    {
      model_file = FkGetModelFileName();
      coloured_net = parseCAMI(model_file);
      list<PNClass>& model_classes = coloured_net->LClasse.Lst();
      vector<string> unfold_classes_names;
      ifstream classes_file(".pnet_unfolded_classes");
      ifstream optimizations_file(".pnet_optimizations");
      if (classes_file.is_open())
	{
	  while (! classes_file.eof())
	    {
	      string s;
	      classes_file >> s;
	      unfold_classes_names.push_back(s);
	    }
	  classes_file.close();
	  for (vector<string>::iterator i = unfold_classes_names.begin();
	       i != unfold_classes_names.end();
	       ++i)
	    {
	      list<PNClass>::iterator j;
	      for (j = model_classes.begin();
		   j != model_classes.end();
		   ++j)
		if ((*j).Name() == *i)
		  {
		    unfold_classes.push_back(&(*j));
		    break;
		  }
	      if (j == model_classes.end())
		cerr << "Unknown colour class : " << *i << endl;
	    }
	}
      if (optimizations_file.is_open())
	{
	  while (! optimizations_file.eof())
	    {
	      std::string s;
	      optimizations_file >> s;
	      optimizations.insert(s);
	    }
	  optimizations_file.close();
	}
    }
#else
  model_file = argv[argc-2];
  output_file = argv[argc-1];
  coloured_net = parseCAMI(model_file);
  for (int narg = 1; narg < argc-2; ++narg)
    {
      string arg(argv[narg]);
      if (arg == "-ao")
	{
	  optimizations.insert("guard");
	  optimizations.insert("syphon");
	  optimizations.insert("syphon-add");
	  optimizations.insert("syphon-bubble");
	  optimizations.insert("orphan");
	  optimizations.insert("size");
	  optimizations.insert("pnet");
	  optimizations.insert("show");
	}
      else if (arg == "-ac")
	{
	  for (list<PNClass>::iterator i =
		 coloured_net->LClasse.Lst().begin();
	       i != coloured_net->LClasse.Lst().end();
	       ++i)
	    unfold_classes.push_back(&(*i));
	}
      else if (arg.find("-c", 0) == 0)
	{
	  string name = arg.substr(2, arg.size()-2);
	  PNClass* c = coloured_net->LClasse.FindName(name);
	  if (c != NULL)
	    unfold_classes.push_back(c);
	  else
	    cerr << "Unknown colour class : " << name << endl;
	}
      else if (arg.find("-o", 0) == 0)
	{
	  optimizations.insert(arg.substr(2, arg.size()-2));
	}
    }
  /*
    model_file = argv[argc-2];
    output_file = argv[argc-1];
    coloured_net = parseCAMI(model_file);
    if (argc == 3)
    for (list<PNClass>::iterator i = coloured_net->LClasse.Lst().begin();
    i != coloured_net->LClasse.Lst().end();
    ++i)
    unfold_classes.push_back(&(*i));
    else
    for (unsigned char i = 1; i < argc-2; ++i)
    {
    string name = argv[i];
    PNClass* c = coloured_net->LClasse.FindName(name);
    if (c != NULL)
    unfold_classes.push_back(c);
    else
    cerr << "Unknown colour class " << name << endl;
    }
  */
#endif

  // Set cout, cerr and clog buffer :
#ifdef FRAMEKIT_SUPPORT
  if (g_fkStandalone)
    {
      filebuf* buf = new filebuf();
      buf->open(output_file.c_str(), ios_base::trunc | ios_base::out);
      cout.rdbuf(buf);
    }
  else
    {
      streambuf* buf = new frame_kit::stream_buffer_t();
      cout.rdbuf(buf);
      clog.rdbuf(buf);
    }
#else
 {
   filebuf* buf = new filebuf();
   buf->open(output_file.c_str(), ios_base::trunc | ios_base::out);
   cout.rdbuf(buf);
 }
#endif

 cerr.fill('0');
 cerr << model_file;
  cerr << *coloured_net;

  //  std::cerr << *coloured_net;

  exec(coloured_net, unfold_classes, optimizations, usages);
  //  MemoryManager::garbage();

  unsigned long long max_places = 0;
  unsigned long long max_transitions = 0;
  unsigned long long prev_places = 0;
  unsigned long long prev_transitions = 0;
  unsigned char char_representation = 9;
  long double max_representation = 1;
  long double min_representation = 0.5;
  unsigned long long delta_places = 0;
  unsigned long long delta_transitions = 0;
  for (unsigned char i = 0; i < char_representation; ++i)
    max_representation *= 10;
  if (usages.begin() != usages.end())
    {
      usages_t::const_iterator last = usages.end();
      --last;
      max_places = (*usages.begin()).places_;
      delta_places = (*usages.begin()).places_ - (*last).places_;
      max_transitions = (*usages.begin()).transitions_;
      delta_transitions = (*usages.begin()).transitions_ - (*last).transitions_;
    }
  for (usages_t::const_iterator i = usages.begin();
       i != usages.end();
       ++i)
    {
      if (i == usages.begin())
	{
          cerr << " & ";
	  if (static_cast<long double>((*i).places_) < max_representation)
	    cerr.precision(char_representation);
	  else
	    cerr.precision(char_representation-4);
          cerr << static_cast<double>((*i).places_) << " & ";
          if (static_cast<long double>((*i).transitions_) < max_representation)
            cerr.precision(char_representation);
          else
            cerr.precision(char_representation-4);
	  cerr << static_cast<double>((*i).transitions_) << " & ";
	}
      else
	{
	  if ((delta_places == 0) || ((*i).places_ == prev_places))
	    cerr << "0 & ";
	  else
	    {
	      unsigned long long nplace =
		max_places - (*i).places_ -  prev_places;
	      long double pplace = static_cast<long double>(nplace);
	      pplace /= static_cast<long double>(delta_places);
	      pplace *= 100.0;
	      if (pplace < min_representation)
		if (pplace == 0.0)
		  cerr << "0 & ";
		else
		  cerr << "$\\varepsilon$ & ";
	      else
		cerr << static_cast<unsigned long long>(round(pplace)) << " & ";
	      prev_places += nplace;
	    }
	  if ((delta_transitions == 0) || ((*i).transitions_ == prev_transitions))
	    cerr << "0 & ";
	  else
	    {
	      unsigned long long ntransition =
		max_transitions - (*i).transitions_ - prev_transitions;
	      long double ptransition = static_cast<long double>(ntransition);
	      ptransition /= static_cast<long double>(delta_transitions);
	      ptransition *= 100.0;
	      if (ptransition < min_representation)
		if (ptransition == 0.0)
		  cerr << "0 & ";
		else
		  cerr << "$\\varepsilon$ & ";
	      else
		cerr << static_cast<unsigned long long>(round(ptransition)) << " & ";
	      prev_transitions += ntransition;
	    }
	}
    }
  if (usages.begin() != usages.end())
    {
      usages_t::const_iterator last = usages.end();
      --last;
      cerr << (*last).time_ << " & ";
      cerr << (*last).memory_ << " & ";
      if ((*last).places_ < max_representation)
	cerr.precision(char_representation);
      else
	cerr.precision(char_representation-4);
      cerr << static_cast<double>((*last).places_) << " & ";
      if ((*last).transitions_ < max_representation)
	cerr.precision(char_representation);
      else
	cerr.precision(char_representation-4);
      cerr << static_cast<double>((*last).transitions_);
    }
  cerr << "\\\\" << endl;

  delete coloured_net;
#ifdef FRAMEKIT_SUPPORT
  return kFk_EndNoProblem;
#else
  return EXIT_SUCCESS;
#endif
}
