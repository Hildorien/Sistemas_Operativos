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
		if (q.empty()){
			if (current_pid(cpu) != IDLE_TASK) { //si no es la tarea idle actualiza el quantum, si no se mantiene en 0
				this->quantum_cpu[cpu] = (this->quantum_cpu[cpu] + 1) % this->endquantum_cpu[cpu];  
			}
			return current_pid(cpu); //siempre retorno el pid actual
		}
		else{
			if (current_pid(cpu) == IDLE_TASK) { //si estoy en idle y la cola no esta vacia, cambio automaticamente, el quantum ya esta en 0
				int sig = q.front(); q.pop();
				return sig;
			}
			else
			{	//si no, aumento en uno el quantum, si al hacerlo quantum actual % quantum del cpu es 0, entonces hay que hacer task switch
				this->quantum_cpu[cpu] = (this->quantum_cpu[cpu] + 1) % this->endquantum_cpu[cpu];
				if (this->quantum_cpu[cpu] == 0){
					int sig = q.front(); q.pop(); // guardamos el que va a correr
					q.push(current_pid(cpu));
					return sig;
				}
				return current_pid(cpu);
			}
		}
	}
		
	
}

