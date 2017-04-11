#ifndef __SCHED_Mistery__
#define __SCHED_Mistery__

#include <vector>
#include <queue>
#include <algorithm>
#include "basesched.h"

using namespace std;

class SchedMistery : public SchedBase {
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
