#ifndef __USAGE_HH__
#define __USAGE_HH__

#include <vector>
#include "ddd_unfold.hh"

namespace cpn_ddd
{

  class usage_t
  {
  public:
    double time_;
    unsigned long long memory_;
    unsigned long long places_;
    unsigned long long transitions_;
    usage_t (double time,
	     unsigned long long memory,
	     unsigned long long places,
	     unsigned long long transitions);
  };

  typedef std::vector<usage_t> usages_t;

  void get_usage (net_t& net,
		  usages_t& usages);

}

#endif //__USAGE_HH__
