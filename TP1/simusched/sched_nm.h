#ifndef __SCHED_NM__
#define __SCHED_NM_

#include <vector>
#include <queue>
#include <algorithm>
#include "basesched.h"

using namespace std;

class SchedNoMistery : public SchedBase {
	public:
		SchedNoMistery(std::vector<int> argn);
        	~SchedNoMistery();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:

		vector< vector<int> > queue_prior_time;  
		int time;
		  
};

#endif