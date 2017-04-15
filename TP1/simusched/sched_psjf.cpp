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
	
	while ( i > 0 && this->queue_prior_time[i][1] > this->queue_prior_time[i-1][1]  ){
		swap(this->queue_prior_time[i-1][0],this->queue_prior_time[i][0]);
		swap(this->queue_prior_time[i-1][1],this->queue_prior_time[i][1]);
		swap(this->queue_prior_time[i-1][2],this->queue_prior_time[i][2]);
		i--;
	}
	
	if( i== 0 || i == -1 || this->queue_prior_time[i-1][2] < this->queue_prior_time[i][2]){
		
		return;
	
	}else{

		while(i > 0 && this->queue_prior_time[i][2] > this->queue_prior_time[i-1][2]  ){
			swap(this->queue_prior_time[i-1][0],this->queue_prior_time[i][0]);
			swap(this->queue_prior_time[i-1][1],this->queue_prior_time[i][1]);
			swap(this->queue_prior_time[i-1][2],this->queue_prior_time[i][2]);
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
				//vector< vector<int> >::iterator it = this->queue_prior_time.begin();
				this->queue_prior_time.erase(this->queue_prior_time.begin() + i);
			}
		}
	}
	
	if(m == TICK){
		return this->queue_prior_time[0][0];
	}
}