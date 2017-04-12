#include <vector>
#include <queue>
#include <algorithm>
#include "basesched.h"
#include "sched_nm.h"
#include <iostream>
#include <string.h>
#include "simu.cpp"
using namespace std;

SchedNoMistery::SchedNoMistery(vector<int> argn) {
	
}

SchedNoMistery::~SchedNoMistery() {

}

void SchedNoMistery::load(int pid) {
	vector<int>* datos = tsk_params(pid);
	vector<int> trip(3);
	trip[0] = pid; 
	trip[1] = (*datos)[0]; 
	trip[2] = time; // trip = [pid,tiempo,hace cuanto entro]
	this->queue_prior_time.push_back(trip);
	int i = this->queue_prior_time.size() -1 ;
	
	while (this->queue_prior_time[i-1][1] < this->queue_prior_time[i][1] && i > 0){
		swap(this->queue_prior_time[i-1][1],this->queue_prior_time[i][1]);
		i--;
	}
	
	
	return;
	
}

void SchedNoMistery::unblock(int pid) {
}

int SchedNoMistery::tick(int cpu, const enum Motivo m) {
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
	time++;
}