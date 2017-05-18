#ifndef __SCHED_RR__
#define __SCHED_RR__

#include <vector>
#include <queue>
#include "basesched.h"

using namespace std; 


class SchedRR : public SchedBase {
	public:
		SchedRR(std::vector<int> argn);
        ~SchedRR();
		virtual void load(int pid);
		virtual void unblock(int pid);
		virtual int tick(int cpu, const enum Motivo m);

	private:
		int quantum;
		std::queue<int> q; //cola global de los procesos
		vector<int> quantum_cpu; //contador de ticks por procesador para que termine el quantum
		vector<int> endquantum_cpu; //vector que indica por procesador, el quantum en el cual termina
};

#endif
