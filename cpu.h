#ifndef __cpu_h_
#define __cpu_h_
#include <iostream>
#include <list>
#include "process.h"

class Cpu
{
public:
	//Constructor
	Cpu(char algo_type);
	//get methods
	int getNextPop() const {return next_pop;}
	bool isEmpty() const {return (ready.size() == 0);}
	
	//other methods
	void add(Process p, int rn);
	void load_on(int rn);
	Process popFromCpu(int rn); //removes whats in being_processed and returns it
	void setContextSwitchTime(int t) {context_switch_time = t;}
	void setTimeSlice(int t) {t_slice = t;}
	char* printQueue();

private:
	Process being_processed;
	std::list<Process> ready;
	int context_switch_time; //Represents switching a process off OR on (half the given value)
	int next_pop;
	int t_slice;
	char flag;
		//f = fcfs
		//r = rr
		//s = srt
	
	//private methods
	void fcfs_rr_add(Process p);
	void srt_add(Process p);
};

#endif