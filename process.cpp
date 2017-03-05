#include "process.h"

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
}

void Process::moveToReady(int cur_time){
	next_time = -1;
	start_time = cur_time;
	remaining_time = burst_time;
}

void Process::movedToCntxIn(int cur_time){
	wait += cur_time - start_time;
	turnaround += cur_time - start_time;
	start_time = cur_time;
}

void Process::movedToCpu(int cur_time){
	turnaround += cur_time - start_time;
	start_time = cur_time;
}

void Process::movedFromCpu(int cur_time){
	turnaround += cur_time-start_time;
	remaining_time -= cur_time-start_time;
	if(remaining_time == 0)
		bursts_left--;
	start_time = cur_time;
}

void Process::movedFromCntxOut(int cur_time){
	turnaround += cur_time - start_time;
	start_time = -1;
	next_time = cur_time + io_time;
}
/*
FCFS: first in first out queue
SRT: priority queue with compare function
RR: first in first out queue*/