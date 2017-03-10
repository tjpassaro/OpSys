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
	//get methods
	void del();
	int getNextAction() const {return next_action;}
	bool isEmpty() const;
	string printQueue();
	char getFlag() const {return flag;}
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
	Process** being_processed;
	Process** context_in;
	Process** context_out;
	list<Process*> ready;
	list<int*> wait_times;
	list<int*> turnaround_times;
	int total_burst_time;
	int total_num_bursts;
	int context_switch_time; //Represents switching a process off OR on (half the given value)
	int next_action;
	int preempts;
	int switches;
	int t_slice;
	char flag;
		//f = fcfs
		//r = rr
		//s = srt
	
	//private methods
	void fcfs_rr_add(Process* p, bool preempt);
	void srt_add(Process* p, int rn, bool preempt);
	
	void loadOnCpu(int rn);
	void runProcess(int rn);
	void unloadOffCpu(int rn);
	Process* popFromCpu(int rn); //removes whats in being_processed and returns it
};

#endif