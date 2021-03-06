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
	int getNextPop() const {return next_pop;} //Returns next action
	bool isEmpty() {return processes.empty();}
	void add(Process* p); //Adds elements by time from first to last
	void printQueue(); //Prints IO Queue with a new line
	Process* popFront(); //Pops first element(closest to done)
private:
	int next_pop;
	std::list<Process*> processes;
};

#endif