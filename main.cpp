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
int n = 0; //number of processes to simulate
int m = 1; //number of processors (i.e., cores) available within the CPU; default is 1
Cpu cpu('f');
list<Process*> process_arrivals;

void processInfile(ifstream* in);
void statsToOutfile(Cpu* cpu, ofstream* out);

int main( int argc, char * argv[] ){
	ifstream fin;
	ofstream fout;

	if(argc != 3){
		cerr << "ERROR: incorrect usage\nUSAGE: ./a.out <input-file> <stats-output-file>" << endl;
		return EXIT_FAILURE;
	}

    fin.open( argv[1], ifstream::in);
    if(!fin){
    	cerr << "ERROR: infile could not be openned" << endl;
    	return EXIT_FAILURE;
    }
    fout.open( argv[2], ofstream::out);
    if(!fout){
    	cerr << "ERROR: outfile could not be openned" << endl;
    	return EXIT_FAILURE;
    }
    processInfile(&fin);
  
	for(int i = 0; i < 3; i++){//fails half way through on second run regardless of what first
		clock_time = 0;
		IO_Queue ioq = IO_Queue();
		list<Process*> incoming;
		
		//copy process_arrivals into incoming
		for(list<Process*>::iterator itr = process_arrivals.begin(); itr != process_arrivals.end(); itr++){
			Process *p = new Process();
			*p = **itr;
			incoming.push_back(p);
		}
		
		cout << "time " << clock_time << "ms: Simulator started for ";
		if(i == 0){
			cpu = Cpu('f');
			cout << "FCFS ";}
		else if(i == 1){
			cpu = Cpu('s');
			cout << "SRT ";}
		else{
			cpu = Cpu('r');
			cout << "RR ";}
		cout << cpu.printQueue();
		
		while( !(cpu.isEmpty()) || !(ioq.isEmpty()) || !(incoming.empty()) ){
			// if(i ==1){
			// 	for(list<Process*>::iterator itr = incoming.begin(); itr != incoming.end(); itr++){
			// 		cout << (*itr)->getProcessId() << " time: " << (*itr)->getStartActionTime() << endl;
			// 	}
			// 	break;
			// }
			
			
			//Adding in processes as they arrive
			while(!incoming.empty() && incoming.front()->getStartActionTime() == clock_time){
				Process* p = *(incoming.begin());
				incoming.pop_front();
				p->allocateArrays();
				cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " arrived and ";
				cpu.add(p, clock_time, false);
			}

			//Takes finished I/O Processes and puts them in the cpu or done
			while(ioq.getNextPop() == clock_time){
				Process* p = ioq.popFront();
				p->resetBurst();
				cout << "time " << clock_time << "ms: Process " << p->getProcessId() << " completed I/O; ";
				cpu.add(p, clock_time, false);
			}
			
			//If a process is finished/pre-empted in CPU the process is reassigned to the correct queue
			while(cpu.getNextAction() == clock_time){
				Process* p = cpu.nextCpuAction(clock_time);
				if(p){
					//finished CPU burst; move to I/O
					if(p->getRemainingTime() == 0){
						if(p->getNumBurstsLeft() != 0){
							ioq.add(p);
						}
						else{
							cpu.takeInStats(p);
						//	p->del();
							delete p;
						}
					}
					//pre-empted; move to back of CPU queue
					else{
						cout << "THIS IS HOW WE DO IT" << endl;
						cpu.add(p, clock_time, true);
					}
				}
			}
			clock_time++;
		}
		//simulator ended
		
		cout << "time " << --clock_time << "ms: Simulator ended for ";
		if(i == 0)
			cout << "FCFS";
		else if(i == 1)
			cout << "SRT";
		else
			cout << "RR";
		cout << endl << endl;
		
		statsToOutfile(&cpu, &fout);
		
		cpu.del();
	}
	
	fin.close();
	fout.close();
	
	//delete process_arrival list
	for(list<Process*>::iterator itr = process_arrivals.begin(); itr != process_arrivals.end(); itr++){
		delete (*itr);
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
		n++;
		//ignore if comment
		if(buf[0] == '#')
			continue;
		//create process from line
		sscanf(buf.c_str(), "%c|%i|%i|%i|%i", &id, &arrival, &burst, &num_bur, &io);
		Process* temp = new Process(burst, io, id, num_bur, arrival); //create process
		
		if(process_arrivals.empty()){//add first element
			process_arrivals.push_front(temp);
		}
		else{
			//add to process_arrivals in order of arrival time
			list<Process*>::iterator itr;
			for (itr=process_arrivals.begin(); itr != process_arrivals.end(); ++itr){
				if((*itr)->getStartActionTime() > temp->getStartActionTime()){
					process_arrivals.insert(itr, temp);
					break;
				}
				// If equal the tie breaker is Process Id
				if((*itr)->getStartActionTime() == temp->getStartActionTime() && (*itr)->getProcessId() > temp->getProcessId()){
					process_arrivals.insert(itr, temp);
					break;
				}
			}
			//add to end of list
			if(itr == process_arrivals.end())
				process_arrivals.push_back(temp);
		}
	}
}

void statsToOutfile(Cpu* cpu, ofstream* out)
{
	*out << "Algorithm ";
	if(cpu->getFlag() == 'f')
		*out << "FCFS" << endl;
	else if(cpu->getFlag() == 's')
		*out << "SRT" << endl;
	else if(cpu->getFlag() == 'r')
		*out << "RR" << endl;
	*out << "-- average CPU burst time: " << cpu->getAverageCPUTime() << " ms" << endl;
	*out << "-- average wait time: " << cpu->getAverageWaitTime() << " ms" << endl;
	*out << "-- average turnaround time: " << cpu->getAverageTurnaroundTime() << " ms" << endl;
	*out << "-- total number of context switches: " << cpu->getSwitches() << endl;
	*out << "-- total number of preemptions: " << cpu->getPreempts() << endl;
}