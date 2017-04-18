#include <vector>
#include <queue>
#include "sched_rr.h"
#include "basesched.h"
#include <iostream>
#include <string.h>

using namespace std;

SchedRR::SchedRR(vector<int> argn) {
	// Round robin recibe la cantidad de cores y sus quantum_cpu por parámetro
	
	for (int i = 0; i < argn[0]; i++ )
	{
		this->endquantum_cpu.push_back(argn[i+1]); //Guardo los quantums por core
		this->quantum_cpu.push_back(0); //Todos los cores comienzan con con el tick en 0
	}
}

SchedRR::~SchedRR() {

}


void SchedRR::load(int pid) {
	q.push(pid); // llegó una tarea nueva
}

void SchedRR::unblock(int pid) {
}

int SchedRR::tick(int cpu, const enum Motivo m) {
	
	if (m == EXIT) {
		// Si el pid actual terminó, sigue el próximo.
		if (q.empty()) {
			quantum_cpu[cpu] = 0;
			return IDLE_TASK;
		}
		else {
			int sig = q.front(); q.pop();
			quantum_cpu[cpu] = 0;
			return sig;
		}
	} else {
		// Siempre sigue el pid actual mientras no termine.
		if (current_pid(cpu) == IDLE_TASK && !q.empty()) {
			int sig = q.front(); q.pop();
			return sig;
		} else if (current_pid(cpu) == IDLE_TASK && q.empty()) {
			return IDLE_TASK;
		} else if (current_pid(cpu) != IDLE_TASK && q.empty()) {
			if (this->quantum_cpu[cpu] + 1 == this->endquantum_cpu[cpu]) {
				quantum_cpu[cpu] = 0;
				return current_pid(cpu);
			}
			else {
				this->quantum_cpu[cpu] = this->quantum_cpu[cpu] + 1;
				return current_pid(cpu); 
			}
			
		}else {
			if (this->quantum_cpu[cpu] + 1 == this->endquantum_cpu[cpu]) {
				int sig = q.front(); q.pop();
				q.push(current_pid(cpu));
				quantum_cpu[cpu] = 0;
				return sig;
			}
			else {
				this->quantum_cpu[cpu] = this->quantum_cpu[cpu] + 1;
				return current_pid(cpu); 
			}

		}
	}
}

