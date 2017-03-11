#ifndef __cpu_h_
#define __cpu_h_
#include <iostream>
#include <list>
#include <string>
#include "process.h"

using namespace std;

class Cpu
{
public:
	//Constructor
	Cpu(int num_cores,char algo_type);
	//Deletes dynamically allocated elements
	void del();
	//Gets and other accessor methods
	bool isEmpty() const;
	string printQueue();
	char getFlag() const {return flag;}
	int getNextAction() const {return next_action;}
	double getAverageWaitTime();
	double getAverageTurnaroundTime();
	double getAverageCPUTime() const {return (double)total_burst_time/total_num_bursts;}
	int getPreempts() const {return preempts;}
	int getSwitches() const {return switches;}
	//other methods
	void add(Process* p, int clock_time, bool preempt);
	Process* nextCpuAction(int rn);
	void setContextSwitchTime(int t) {context_switch_time = t;}
	void setTimeSlice(int t) {t_slice = t;}
	void takeInStats(Process* p);

private:
	//The each array index represents a core
	Process** being_processed; //Array of processes in cores
	Process** context_in; //Array of processes switching into cores
	Process** context_out; //Array of processes switching out of cores
	//Ready queue
	list<Process*> ready;
	//Wait times and turnaround times of terminated processes
	list<int*> wait_times;
	list<int*> turnaround_times;
	int total_burst_time;
	int total_num_bursts;
	int context_switch_time; //Represents switching a process off OR on (half the given value)
	int next_action; //Next time the cpu makes an action
	int preempts;
	int switches;
	int t_slice;
	char flag;
		//f = fcfs
		//r = rr
		//s = srt
	
	//private methods
	void fcfs_rr_add(Process* p, bool preempt); //Add for fcfs and rr (first in first out)
	void srt_add(Process* p, int rn, bool preempt); //Add for srt (mininum to maximum)
	
	void loadOnCpu(int rn); //Puts a process into context_in
	void runProcess(int rn); //Moves process from context_in to being_processed
	void unloadOffCpu(int rn); //Moves process from being_processed to context_out
	Process* popFromCpu(int rn); //removes whats in context_out and returns it
};

#endif