#include "usage.hh"
#include <sys/resource.h>
#include <fstream>

namespace cpn_ddd
{

  usage_t::usage_t (double time,
		    unsigned long long memory,
		    unsigned long long places,
		    unsigned long long transitions) :
    time_(time),
    memory_(memory),
    places_(places),
    transitions_(transitions)
  {}

  void get_usage (net_t& net, usages_t& usages)
  {
    struct rusage sys_usage;
    getrusage(RUSAGE_SELF, &sys_usage);
    // Get execution time :
    double ut = sys_usage.ru_utime.tv_sec * 1000000;
    ut += sys_usage.ru_utime.tv_usec;
    double st = sys_usage.ru_stime.tv_sec * 1000000;
    st += sys_usage.ru_stime.tv_usec;
    double time = (ut + st) / 1000000;
    unsigned long long memory = (sys_usage.ru_idrss + sys_usage.ru_isrss) / 1024;
    //    std::cout << "Memory : " << memory << std::endl;
    /*
    if (memory == 0)
      {
	// Get memory usage :
	stringstream proc_name;
	proc_name << "/proc/" << getpid() << "/status";
	ifstream proc(proc_name.str().c_str());
	string s;
	do
	  {
	    proc >> s;
	  } while (s.find("VmData:", 0) == string::npos);
	proc >> memory;
	memory /= 1024;
      }
    */
    // Get number of places and transitions :
    unsigned long long places = 0;
    for (ddds_t::iterator z = net.places().begin();
	 z != net.places().end();
	 ++z)
      places += static_cast<unsigned long long>((*z).second.ddd().nbStates());
    unsigned long long transitions = 0;
    for (ddds_t::iterator z = net.transitions().begin();
	 z != net.transitions().end();
	 ++z)
      transitions += static_cast<unsigned long long>((*z).second.ddd().nbStates());
    // Put usage in list :
    usages.push_back(usage_t(time, memory, places, transitions));
  }

}
