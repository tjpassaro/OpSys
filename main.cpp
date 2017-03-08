#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include "io_queue.h"
#include "cpu.h"
#include <list>
#include "process.h"

using namespace std;

//clock_time allows the queue objects to access the time
int clock_time;
Cpu cpu('f');
list<Process*> incoming;

void processInfile(ifstream* in);
void statsToOutfile(Cpu* cpu, ifstream* out)

int main( int argc, char * argv[] ){
	ifstream fin;
	ifstream fout;

	if(argc != 3){
		cerr << "ERROR: incorrect usage\nUSAGE: ./a.out <input-file> <stats-output-file>" << endl;
		return EXIT_FAILURE;
	}

    fin.open( argv[1], ifstream::in);
    if(!fin){
    	cerr << "ERROR: file could not be openned" << endl;
    	return EXIT_FAILURE;
    }
    fout.open( argv[2], ifstream::out);
    processInfile(&fin);
    
//     Define n as the number of processes to simulate. Note that this is determined via the input
//     file described below.
//     Define m as the number of processors (i.e., cores) available within the CPU (use a default
//     value of 1). (v1.1) We may use this in a future project, but we will not use this in Project 1.
    
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
				cpu.add(p, clock_time);
				cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " completed I/O; added to ready queue " << cpu.printQueue();
			}
			//If a process is finished/pre-empted in CPU
			//the process is reassigned to the correct queue
			if(cpu.getNextAction() == clock_time){
				Process* p = cpu.nextCpuAction(clock_time);
				if(p){
					//finished CPU burst; move to I/O
					if(p->getRemainingTime() == 0){
						if(p->getNumBurstsLeft() == 0){
							cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " completed a CPU burst; " << p->getNumBurstsLeft() << " bursts to go " << cpu.printQueue();
							ioq.add(p);
						}
						else{
							cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " terminated" << cpu.printQueue();
							//cpu.takeInStats(p);
							p->del();
							delete p;
						}
					}
					//pre-empted; move to back of CPU queue
					else{
						cpu.preempt();
						cpu.add(p, clock_time);
					}
				}
			}
			clock_time++;
		}
		//statsToOutfile(&cpu, &fout);
	}
	return EXIT_SUCCESS;
}

void processInfile(ifstream* in){
	char id;
	int arrival;
	int burst;
	int num_bur;
	int io;
	string buf = "";
	while(getline(*in, buf)){ //until reach eof
		//ignore if comment
		if(buf[0] == '#')
			continue;
		//create process from line
		sscanf(buf.c_str(), "%c|%i|%i|%i|%i", &id, &arrival, &burst, &num_bur, &io);
		Process* temp = new Process(burst, io, id, num_bur, arrival); //create process
		
		if(incoming.empty()){
			incoming.push_front(temp);
		}
		else{
			//add to incoming Processes
			list<Process*>::iterator itr;
			for (itr=incoming.begin(); itr != incoming.end(); ++itr){
				if((*itr)->getStartActionTime() > temp->getStartActionTime()){
					incoming.insert(itr, temp);
					break;
				}
				// If equal the tie breaker is Process Id
				if((*itr)->getStartActionTime() == temp->getStartActionTime() && (*itr)->getProcessId() > temp->getProcessId()){
					incoming.insert(itr, temp);
					break;
				}
			}
			if(itr == incoming.end())
				incoming.push_back(temp);
		}
	}
}

void statsToOutfile(Cpu* cpu, ifstream* out)
{
	out << "Algorithm ";
	if(cpu->getFlag() == 'f')
		out << "FCFS" << endl;
	else if(cpu->getFlag() == 's')
		out << "SRT" << endl;
	else if(cpu->getFlag() == 'r')
		out << "RR" << endl;
	out << "-- average CPU burst time: " << cpu->getAverageCPUTime() << " ms" << endl;
	out << "-- average wait time: " << cpu->getAverageWaitTime()
	out << "-- average turnaround time: " << cpu->getAverageTurnaroundTime() << " ms" << endl;
	out << "-- total number of context switches: " << cpu->getSwitches() << endl;
	out << "-- total number of preemptions: " << cpu->getPreempts() << endl;
}