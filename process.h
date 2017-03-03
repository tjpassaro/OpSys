#ifndef __process_h_
#define __process_h_
#include <iostream>

class Process
{
public:
	//Constructor
	Process(int bt, int iot, int p, int bl, int w, int bsl, int nt, int st);
	//From file
	int getBurstTime() const {return burst_time;}
	int IOTime() const {return io_time;}
	int getPid() const {return pid;}
	//Run time
	int getBurstLeft() const {return burst_left;}
	int getTotalWait() const {return wait;}
	int getNumBurstsLeft() const {return bursts_left;}
	//In each queue
	int getNextActionTime() const {return next_time;}
	int getStartActionTime() const {return start_time;}
	bool operator<(const Process& lhs, const Process& rhs) const; 
};

#endif