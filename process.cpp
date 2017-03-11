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
		wait_times[0] = num_bursts+1; //first element is array size
	turnaround_times = new int[num_bursts+1];
		turnaround_times[0] = num_bursts+1; //first element is array size
	//In each queue
	next_time = arrival;
	start_time = arrival;
	arrival_time = arrival;
	sprintf(str, "%c|%d|%d|%d|%d", id, start_time, burst_time, num_bursts, io_time);
}

Process::Process(){
	burst_time = 0;
	total_num_bursts = 0;
	io_time = 0;
	id = '0';
	//Run time
	remaining_time = 0;
	wait = 0;
	turnaround = 0;
	bursts_left = 0;
	//In each queue
	next_time = 0;
	start_time = 0;
	arrival_time = 0;
	sprintf(str, "%c|%d|%d|%d|%d", '0', start_time, burst_time, total_num_bursts, io_time);
}

Process::~Process(){
	//Accounts for the master Pointer having it's arrays deleted by the one in simulation.
	if(wait_times)
		delete[] wait_times;
	if(turnaround_times)
		delete[] turnaround_times;
}

void Process::allocateArrays(){
	wait_times = new int[total_num_bursts+1];
		wait_times[0] = total_num_bursts+1; //first element is array size
	turnaround_times = new int[total_num_bursts+1];
		turnaround_times[0] = total_num_bursts+1; //first element is array size
}

void Process::moveToReady(int cur_time){
	next_time = -1;
	start_time = cur_time;
}

void Process::movedToCntxIn(int cur_time){
	wait += (cur_time - start_time);
	turnaround += (cur_time - start_time);
	start_time = cur_time;
}

void Process::movedToCpu(int cur_time){
	turnaround += (cur_time - start_time);
	start_time = cur_time;
}

void Process::movedFromCpu(int cur_time){
	updateRemainingTime(cur_time);
	if(remaining_time == 0)
		bursts_left--;
}

void Process::movedFromCntxOut(int cur_time){
	turnaround += (cur_time - start_time);
	if(remaining_time == 0){
		//Add wait time to array
		wait_times[total_num_bursts - bursts_left] = wait;
		turnaround_times[total_num_bursts - bursts_left] = turnaround;
		wait = 0;
		turnaround = 0;
	}
	start_time = -1;
	next_time = cur_time + io_time;
}

void Process::updateRemainingTime(int cur_time){
	turnaround += (cur_time-start_time);
	remaining_time -= (cur_time-start_time);
	start_time = cur_time;
}

int* Process::getWaitStats(){
	int size = wait_times[0];
	int* ret = new int[size];
	for(int i=0; i < size; i++)
		ret[i] = wait_times[i];
	return ret;
}

int* Process::getTurnStats(){
	int size = turnaround_times[0];
	int* ret = new int[size];
	for(int i=0; i < size; i++)
		ret[i] = turnaround_times[i];
	return ret;
}


bool Process::operator==(const Process& rhs) const{
	return (id == rhs.getProcessId() && start_time == rhs.getArrivalTime() && burst_time == rhs.getBurstTime() && total_num_bursts == rhs.getTotalNumBursts() && io_time == rhs.getIOTime());
}

/*
FCFS: first in first out queue
SRT: priority queue with compare function
RR: first in first out queue*/