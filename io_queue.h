#ifndef __io_queue_h_
#define __io_queue_h_
#include "process.h"
#include <iostream>
#include <list>

class IO_Queue
{
public:
	//Constructor
	IO_Queue();
	int getNextPop() const {return next_pop;}
	bool isEmpty() {return (processes.size() == 0);}
	void add(Process p);
	Process popFront();
private:
	int next_pop;
	std::list<Process> processes;
};

#endif