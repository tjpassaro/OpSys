#include "io_queue.h"
#include "cpu.h"
#include "process.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <list>
#include <fstream>

using namespace std;

//clock_time allows the queue objects to access the time
int clock_time;

list<Process> incoming;

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
		cout << "time " << clock_time << "ms: Simulator started for ";
		if(i == 0){
			cpu = Cpu('f');
			cout << "FCFS ";
		}
		else if(i == 1){
			cpu = Cpu('s');
			cout << "SRT ";
		}
		else{
			cpu = Cpu('r');
			cout << "RR ";
		}
		cout << cpu.printQueue();
		
		while( !(cpu.isEmpty()) || !(ioq.isEmpty()) || !(incoming.empty())){
			cout << incoming.front().getStartActionTime() << " _\n";
			//Adding in processes as they arrive
			while(incoming.front().getStartActionTime() == clock_time){
				Process p = incoming.front();//or *(incoming.begin())
				incoming.pop_front();
				cpu.add(p, clock_time);
				cout << "time " << clock_time << "ms: Process " << p.getProcessId() << " arrived and added to ready queue " << cpu.printQueue();
			}
			//Takes finished I/O Processes and puts them in the cpu or done
			while(ioq.getNextPop() == clock_time){
				cout << "ha\n";
				Process p = ioq.popFront();
				if(p.getNumBurstsLeft() != 0)
					cpu.add(p, clock_time);
				//else
					//PROCESS DONE
			}
			//If a process is finished/pre-empted 
			//the process is reassigned to the correct queue
			if(cpu.getNextPop() == clock_time){
				cout << "he\n";
				Process p = cpu.popFromCpu(clock_time);
				if(p.getRemainingTime() == 0)
					ioq.add(p);
				else
					cpu.add(p, clock_time);
			}
			clock_time++;
			break;///////////////////////////////////////////////
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
	char buf[100];
	while(in->getline(buf, 100)) //until reach eof
	{
		//ignore if comment
		if(buf[0] == '#')
			continue;
			
		//create process from line
		sscanf(buf, "%c|%i|%i|%i|%i", &id, &arrival, &burst, &num_bur, &io);
		Process temp(burst, io, id, num_bur, arrival); //create process

		//add to incoming Processes
		list<Process>::iterator itr;
		for (itr = incoming.begin(); itr != incoming.end(); ++itr){
			if(itr->getStartActionTime() > temp.getStartActionTime()){
				incoming.insert(itr, temp);
				break;
			}
			// If equal the tie breaker is Process Id
			if(itr->getStartActionTime() == temp.getStartActionTime() && itr->getProcessId() > temp.getProcessId()){
				incoming.insert(itr, temp);
				break;
			}
		}
		if(itr == incoming.end()){incoming.push_back(temp);}
	}
}