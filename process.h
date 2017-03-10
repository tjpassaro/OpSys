#ifndef __process_h_
#define __process_h_
#include <iostream>
#include <string>

class Process
{
public:
	//Constructor
	Process(int burst_t, int io_t, char process_id, int num_bursts, int arrival);
	Process();
	~Process();
	//From file
	void del();
	void allocateArrays();
	
	int getBurstTime() const {return burst_time;}
	int getIOTime() const {return io_time;}
	char getProcessId() const {return id;}
	char* toString() {return str;}
	//Accessors
	int getRemainingTime() const {return remaining_time;}
	int getTotalWait() const {return wait;}
	int getTurnaroundTime() const {return turnaround;}
	int getNumBurstsLeft() const {return bursts_left;}
	int* getTurnStats();
	int* getWaitStats();
	int getTotalBurstTime() const {return burst_time*total_num_bursts;}
	int getTotalNumBursts() const {return total_num_bursts;}
	//In each queue
	int getNextActionTime() const {return next_time;}
	int getStartActionTime() const {return start_time;}
	
	//Movement Methods
	void resetBurst() {remaining_time = burst_time;}
	void moveToReady(int cur_time);
	void movedToCntxIn(int cur_time);
	void movedToCpu(int cur_time);
	void movedFromCpu(int cur_time);
	void movedFromCntxOut(int cur_time);
	void updateRemainingTime(int cur_time);
private:
	//From file
	int burst_time;
	int total_num_bursts;
	int io_time;
	char id;
	int arrival_time;
	//Run time
	int remaining_time;
	int wait; //time spent in ready queue; DOES NOT include context switch time
	int turnaround;
	int* wait_times;
	int* turnaround_times;
	int bursts_left;
	//In each queue
	int next_time;
	int start_time;
	char str[50];
};

#endif