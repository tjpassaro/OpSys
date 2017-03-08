#include "process.h"
#include <string>
#include <stdio.h>

using namespace std;

//Constructor
Process::Process(int burst_t, int io_t, char process_id, int num_bursts, int arrival){
	burst_time = burst_t;
	total_num_bursts = num_bursts;
	io_time = io_t;
	id = process_id;
	//Run time
	remaining_time = burst_time;
	wait = 0;
	turnaround = 0;
	bursts_left = num_bursts;
	wait_times = new int[num_bursts+1];
		wait_times[0] = num_bursts; //first element is array size
	turnaround_times = new int[num_bursts+1];
		turnaround_times[0] = num_bursts; //first element is array size
	//In each queue
	next_time = arrival;
	start_time = arrival;
	sprintf(str, "%c|%d|%d|%d|%d", id, start_time, burst_time, num_bursts, io_time);
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
	start_time = cur_time;
}

void Process::movedFromCntxOut(int cur_time){
	turnaround += cur_time - start_time;
	if(remaining_time == 0){
		//Add wait time to array
		wait_times[total_num_bursts - bursts_left] = wait;
		turnaround_times[total_num_bursts - bursts_left] = turnaround;
		wait = 0;
		turnaround = 0;
		bursts_left--;
	}
	start_time = -1;
	next_time = cur_time + io_time;
}
/*
FCFS: first in first out queue
SRT: priority queue with compare function
RR: first in first out queue*/