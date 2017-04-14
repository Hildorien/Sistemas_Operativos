#include <vector>
#include <queue>
#include <algorithm>
#include "basesched.h"
#include "sched_nm.h"
#include <iostream>
#include <string.h>

using namespace std;

SchedNoMistery::SchedNoMistery(vector<int> argn) {
	
}

SchedNoMistery::~SchedNoMistery() {

}

void SchedNoMistery::load(int pid) {

	vector<int>* datos = tsk_params(pid);
	vector<int> trip(2);
	trip[0] = pid; 
	trip[1] = (*datos)[0]; 
	// trip = [pid,tiempo]
	this->queue_prior_time.push_back(trip);
	int i = this->queue_prior_time.size() -1 ;

	while ( i > 0 && this->queue_prior_time[i-1][1] < this->queue_prior_time[i][1] ){
		swap(this->queue_prior_time[i-1][0],this->queue_prior_time[i][0]);
		swap(this->queue_prior_time[i-1][1],this->queue_prior_time[i][1]);
		i--;
	}
	
	
	return;
	
}

void SchedNoMistery::unblock(int pid) {
}

int SchedNoMistery::tick(int cpu, const enum Motivo m) {
	if( m == EXIT ) {

		if(!queue_prior_time.empty())
		{
			vector<int> trip = (this->queue_prior_time)[queue_prior_time.size()-1];
			queue_prior_time.pop_back();
			return trip[0];
		}
		return IDLE_TASK;
	}
	
	if(m == TICK){
		if (current_pid(cpu) == IDLE_TASK && !queue_prior_time.empty()) {
			vector<int> trip = (this->queue_prior_time)[queue_prior_time.size()-1];
			queue_prior_time.pop_back();
			return trip[0];
		} else {
			return current_pid(cpu);
		}
	}

}