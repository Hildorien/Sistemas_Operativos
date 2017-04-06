#ifndef __SCHED_PSJF__
#define __SCHED_PSJF__

#include <vector>
#include <queue>
#include <algorithm>
#include "basesched.h"

using namespace std;

class SchedPSJF : public SchedBase {
	public:
		SchedPSJF(std::vector<int> argn);
        	~SchedPSJF();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:

		vector< vector<int> > queue_prior_time;  
		
		  
};

#endif