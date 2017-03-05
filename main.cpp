#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include "io_queue.h"
#include "cpu.h"
#include <list>
#include <fstream>  
#include "process.h"

using namespace std;

//clock_time allows the queue objects to access the time
int clock_time;

list<Process*> incoming;

void processInfile(ifstream* in);

int main( int argc, char * argv[] )
{
	ifstream fin;
	ifstream fout;
	Cpu cpu('f');

	if(argc != 3)
	{
		cerr << "ERROR: incorrect usage\nUSAGE: ./a.out <input-file> <stats-output-file>" << endl;
		return EXIT_FAILURE;
	}

    fin.open( argv[1], ifstream::in);
    if(!fin)
    {
    	cerr << "ERROR: file could not be openned" << endl;
    	return EXIT_FAILURE;
    }
    fout.open( argv[2], ifstream::out);

    processInfile(&fin);
    
	for(int i = 0; i < 3; i++){
		clock_time = 0;
		IO_Queue ioq = IO_Queue();
		if(i == 0)
			cpu = Cpu('f');
		else if(i == 1)
			cpu = Cpu('s');
		else
			cpu = Cpu('r');
	
		while( !(cpu.isEmpty()) || !(ioq.isEmpty()) || !(incoming.empty()) ){
			
			//Adding in processes as they arrive
			while((*(incoming.begin()))->getStartActionTime() == clock_time){
				Process* p = *(incoming.begin());
				incoming.pop_front();
				cpu.add(p, clock_time);
				cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " arrived and added to ready queue " << cpu.printQueue();
			}
			//Takes finished I/O Processes and puts them in the cpu or done
			while(ioq.getNextPop() == clock_time){
				Process* p = ioq.popFront();
				if(p->getNumBurstsLeft() != 0){
					cpu.add(p, clock_time);
					cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " completed I/O; added to ready queue " << cpu.printQueue();
				}
				else
					cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " terminated" << cpu.printQueue();
			}
			//If a process is finished/pre-empted 
			//the process is reassigned to the correct queue
			if(cpu.getNextAction() == clock_time){
				Process* p = cpu.popFromCpu(clock_time);
				cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " completed a CPU burst; " << p->getNumBurstsLeft() << "bursts to go " << cpu.printQueue();
				if(p->getRemainingTime() == 0){
					ioq.add(p);
					cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " switching out of CPU; will block on I/0 until time " << clock_time+p->getIOTime() << "ms " << cpu.printQueue();
						//NOTE: may have to add context_switch_time too, not sure yet
				}
				else{
					cpu.add(p, clock_time);
				}
			}
			clock_time++;
		}
	}
	return EXIT_SUCCESS;
}

void processInfile(ifstream* in)
{
	char id;
	int arrival;
	int burst;
	int num_bur;
	int io;
	char* buf = NULL;
	while(in->getline(buf, 100)) //until reach eof
	{
		//ignore if comment
		if(*buf == '#')
			continue;
			
		//create process from line
		sscanf(buf, "%c|%i|%i|%i|%i", &id, &arrival, &burst, &num_bur, &io);
		Process temp = new Process(burst, io, id, num_bur, arrival); //create process

		//add to incoming Processes
		for (list<Process*>::iterator itr=incoming.begin(); itr != incoming.end(); ++itr){
			if((*itr)->getStartActionTime() > temp.getStartActionTime()){
				incoming.insert(itr, &temp);
				break;
			}
			// If equal the tie breaker is Process Id
			if((*itr)->getStartActionTime() == temp.getStartActionTime() && (*itr)->getProcessId() > temp.getProcessId()){
				incoming.insert(itr, &temp);
				break;
			}
		}
	}
}