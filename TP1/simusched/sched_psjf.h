#ifndef __SCHED_PSJF__
#define __SCHED_PSJF__

#include <vector>
#include <queue>
#include <algorithm>
#include "basesched.h"

using namespace std;

class SchedPSJF : public SchedBase {
	public:
		SchedMistery(std::vector<int> argn);
        	~SchedMistery();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		// Completar
};

#endif