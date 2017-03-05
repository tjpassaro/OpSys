#ifndef __process_h_
#define __process_h_
#include <iostream>
#include <cstdlib>

class Process
{
public:
	//Constructor
	Process(int burst_t, int io_t, char process_id, int num_bursts, int arrival);
	Process();
	//From file
	int getBurstTime() const {return burst_time;}
	int getIOTime() const {return io_time;}
	char getProcessId() const {return id;}
	char* toString(){return str;}
	//Run time
	int getRemainingTime() const {return remaining_time;}
	int getTotalWait() const {return wait;}
	int getTurnaroundTime() const {return turnaround;}
	int getNumBurstsLeft() const {return bursts_left;}
	//In each queue
	int getNextActionTime() const {return next_time;}
	int getStartActionTime() const {return start_time;}
	void moveToReady(int cur_time);
	void movedToCpu(int cur_time, int context_switch_time);
	void movedFromCpu(int cur_time, int context_switch_time);
private:
	//From file
	int burst_time;
	int io_time;
	char id;
	//Run time
	int remaining_time;
	int wait;
	int turnaround;
	int bursts_left;
	//In each queue
	int next_time;
	int start_time;
	char str[100];
};

#endif