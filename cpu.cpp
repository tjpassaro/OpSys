#include "cpu.h"
#include "process.h"
#include <iostream>
#include <list>
#include <stdio.h>
#include <string>

using namespace std;

//Constructor
Cpu::Cpu(char algo_type){
	if(algo_type != 'f' && algo_type != 'r' && algo_type != 's')
		cerr << "ERROR: incorrect flag\n\tf = fcfs\n\tr = rr\n\ts = srt" << endl;
	flag = algo_type;
	context_switch_time = 3; //half given
	if(flag == 'r')
		t_slice = 94;
	else
		t_slice = -1;
}

void Cpu::add(Process p, int rn){
	p.moveToReady(rn);
	if(ready.size() == 0){
		ready.push_front(p);
		load_on(rn);
	}
	if( flag == 'f' || flag == 'r')
		fcfs_rr_add(p);
	else
		srt_add(p);
}

//Call before context switch time
//Moves from the ready queue to the cpu
void Cpu::load_on(int rn){
	if(ready.size() != 0){
		Process p = ready.front();
		ready.pop_front();
		p.movedToCpu(rn, context_switch_time);
		cout << "time " << rn + context_switch_time << "ms: Process " << p.getProcessId() << " started using the CPU " << printQueue();
		being_processed = p;
		//Increments the next_pop field
		if(flag=='f'||flag=='s')
			next_pop = p.getRemainingTime()+context_switch_time;
		else //round-robin
			if(p.getRemainingTime()>t_slice)
				next_pop = t_slice + context_switch_time;
			else
				next_pop = p.getRemainingTime() + context_switch_time;
	}
}

//Call after context switch time
Process Cpu::popFromCpu(int rn){
	next_pop = -999;
	Process temp = being_processed;
	temp.movedFromCpu(rn, context_switch_time);
	load_on(rn);
	return temp;
}


void Cpu::fcfs_rr_add(Process p){
	ready.push_back(p);
}
void Cpu::srt_add(Process p){
	list<Process>::iterator itr;
	for (itr=ready.begin(); itr != ready.end(); ++itr){
		if(itr->getRemainingTime() > p.getRemainingTime()){
			ready.insert(itr, p);
			break;
		}
		// If equal the tie breaker is Process Id
		if(itr->getRemainingTime() == p.getRemainingTime() && itr->getProcessId() > p.getProcessId()){
			ready.insert(itr, p);
			break;
		}
	}
	if(itr == ready.end()){ready.push_back(p);}
}

string Cpu::printQueue(){
	string buf = "[Q";
	if(ready.empty())
		buf += " <empty>";
	else{
		for(list<Process>::iterator itr=ready.begin(); itr != ready.end(); itr++){
			buf += " ";
			buf += itr->getProcessId();
		}
	}
	buf += "]\n";
	return buf;
}