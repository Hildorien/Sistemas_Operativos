#include <vector>
#include <queue>
#include <algorithm>
#include "basesched.h"
#include "sched_psjf.h"
#include <iostream>
#include <string.h>

using namespace std;

SchedPSJF::SchedPSJF(vector<int> argn) {
	
}

SchedPSJF::~SchedPSJF() {

}


void SchedPSJF::load(int pid) {
	vector<int>* datos = tsk_params(pid);
	vector<int> trip(3);
	trip[0] = pid; 
	trip[1] = (*datos)[0]; 
	trip[2] = (*datos)[1]; // trip = [pid,prioridad,tiempo]
	this->queue_prior_time.push_back(trip);
	int i = this->queue_prior_time.size() -1 ;
// Ordenamos por prioridad (primer coordenada del elemento)	
	while ( i > 0 && this->queue_prior_time[i][1] > this->queue_prior_time[i-1][1]  ){ // Vamos a ordenar de derecha a izquierda ,es decir,
		swap(this->queue_prior_time[i-1][0],this->queue_prior_time[i][0]);			   // el ultimo elemento del vector es el mas prioritario
		swap(this->queue_prior_time[i-1][1],this->queue_prior_time[i][1]);             // con una variante de bubblesort
		swap(this->queue_prior_time[i-1][2],this->queue_prior_time[i][2]);
		i--;
	}
	

 //Ordenamos por cpu time (segunda coordenada del elemento)
	while(i > 0 && this->queue_prior_time[i][1] == this->queue_prior_time[i-1][1] && this->queue_prior_time[i][2] > this->queue_prior_time[i-1][2]  ){
		swap(this->queue_prior_time[i-1][0],this->queue_prior_time[i][0]);
		swap(this->queue_prior_time[i-1][1],this->queue_prior_time[i][1]);
		swap(this->queue_prior_time[i-1][2],this->queue_prior_time[i][2]);
		i--;
	}
	
}

void SchedPSJF::unblock(int pid) {
}

int SchedPSJF::tick(int cpu, const enum Motivo m) {
	vector<int>* datos = tsk_params(current_pid(cpu));
	if( m == EXIT ) {

		if(!queue_prior_time.empty())
		{
			vector<int> trip = (this->queue_prior_time)[queue_prior_time.size()-1];
			queue_prior_time.pop_back(); //Sacamos el ultimo elemento de la cola
			return trip[0];
		}
		return IDLE_TASK; // la cola esta vacia
	}
	
	if(m == TICK){
		if (!queue_prior_time.empty()){
			vector<int> trip = (this->queue_prior_time)[queue_prior_time.size()-1];
			if (current_pid(cpu) == IDLE_TASK ) {
				queue_prior_time.pop_back();
				return trip[0]; // En esta coordenada se guarda el pid
		
			} else {
				if((*datos)[0] > trip[1] || ((*datos)[0] == trip[1] && (*datos)[1] > trip[2]) ){   // Si hay otra tarea mas prioritaria
					vector<int> prior_trip = (this->queue_prior_time)[queue_prior_time.size()-1];  // vamos a cambiar la que esta corriendo
					(this->queue_prior_time)[queue_prior_time.size()-1][0] = current_pid(cpu);     // por la ultima ya que la desalojada volvera
					(this->queue_prior_time)[queue_prior_time.size()-1][1] = (*datos)[0];          // a correr despues
					(this->queue_prior_time)[queue_prior_time.size()-1][2] = (*datos)[1]; 
					return prior_trip[0];}
				else {
					return current_pid(cpu);
				}
			}
		}
		
		else {
				return current_pid(cpu);}
	}
}