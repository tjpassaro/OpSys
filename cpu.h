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
	Cpu(char algo_type);
	//get methods
	int getNextAction() const {return next_action;}
	bool isEmpty() const {return ((ready.size() == 0) && !being_processed && !context_in && !context_out);}
	string printQueue();
	//other methods
	void add(Process* p, int rn);
	Process* nextCpuAction(int rn);
	void setContextSwitchTime(int t) {context_switch_time = t;}
	void setTimeSlice(int t) {t_slice = t;}

private:
	Process* being_processed;
	Process* context_in;
	Process* context_out;
	list<Process*> ready;
	int context_switch_time; //Represents switching a process off OR on (half the given value)
	int next_action;
	int t_slice;
	char flag;
		//f = fcfs
		//r = rr
		//s = srt
	
	//private methods
	void fcfs_rr_add(Process* p);
	void srt_add(Process* p);
	
	void loadOnCpu(int rn);
	void runProcess(int rn);
	void unloadOffCpu(int rn);
	Process* popFromCpu(int rn); //removes whats in being_processed and returns it
};

#endif