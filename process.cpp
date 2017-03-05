#include "process.h"
#include <cstdlib>
#include <stdio.h>

using namespace std;

//Constructor
Process::Process(int burst_t, int io_t, char process_id, int num_bursts, int arrival){
	burst_time = burst_t;
	io_time = io_t;
	id = process_id;
	//Run time
	remaining_time = burst_time;
	wait = 0;
	turnaround = 0;
	bursts_left = num_bursts;
	//In each queue
	next_time = arrival;
	start_time = arrival;
	sprintf(str, "%c|%i|%i|%i|%i", id, start_time, burst_time, bursts_left, io_time);
}

Process::Process(){}

void Process::moveToReady(int cur_time){
	next_time = -1;
	start_time = cur_time;
}

//Assumes this is called with cur_time BEFORE the context switch 
void Process::movedToCpu(int cur_time, int context_switch_time){
	wait += cur_time-start_time;
	turnaround += cur_time-start_time+context_switch_time;
	start_time = cur_time+context_switch_time;
}

//Assumes this is called with cur_time AFTER the context switch 
void Process::movedFromCpu(int cur_time, int context_switch_time){
	turnaround += cur_time-start_time;
	remaining_time = cur_time-context_switch_time-start_time;
	if(remaining_time == 0){
		bursts_left--;
		remaining_time = burst_time;
	}
	next_time = cur_time+io_time;
	start_time = -1;
}
/*
FCFS: first in first out queue
SRT: priority queue with compare function
RR: first in first out queue*/