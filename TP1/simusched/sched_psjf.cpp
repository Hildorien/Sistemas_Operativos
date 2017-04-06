#ifndef __SCHED_PSJF__
#define __SCHED_PSJF__

#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <string.h>
#include "basesched.h"
#include "sched_psjf.h"

using namespace std;

SchedPSJF::SchedPSJF(vector<int> argn) {
	
}

SchedPSJF::~SchedPSJF() {

}


void SchedPSJF::load(int pid) {
	vector<int>* datos = task_params(pid);
	vector<int> trip(3);
	trip[0] = pid; trip[1] = datos[0]; trip[2] = datos[1]; // trip = [pid,prioridad,tiempo]
	this->queue_prior_time.push_back(trip);
	int i = this->queue_prior_time.size() -1 ;
	
	while (this->queue_prior_time[i-1][1] > this->queue_prior_time[i-1][1] && i > -1){
		swap(this->queue_prior_time[i-1][1],this->queue_prior_time[i][1]);
		i--;
	}
	
	if( i== 0 || i == -1 || this->queue_prior_time[i-1][1] < this->queue_prior_time[i-1][1]){
		
		return;
	
	}else{

		while(this->queue_prior_time[i-1][2] > this->queue_prior_time[i-1][1]  && i > -1 ){
			swap(this->queue_prior_time[i-1][1],this->queue_prior_time[i][1]);
			i--;
		}
	}
	
}

void SchedPSJF::unblock(int pid) {
}

int SchedPSJF::tick(int cpu, const enum Motivo m) {
	if( m == EXIT ) {
		int curpid = current_pid(cpu);
		for (int i = 0; i < this->queue_prior_time.size(); i++)
		{
			if(this->queue_prior_time[i][0] == curpid)
			{
				this->queue_prior_time.erase(i);
			}
		}
	}
	
	if(m == TICK){
		return this->queue_prior_time[0][0];
	}
}