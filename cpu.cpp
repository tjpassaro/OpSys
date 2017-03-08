#include "cpu.h"
#include <iostream>
#include "process.h"
#include <list>
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
	being_processed = NULL;
	context_in = NULL;
	context_out = NULL;
	
}

void Cpu::add(Process* p, int rn){
	p->moveToReady(rn);
	if(isEmpty()){
		next_action = rn;
	}
	if( flag == 'f' || flag == 'r')
		fcfs_rr_add(p);
	else
		srt_add(p);
}

Process* Cpu::nextCpuAction(int rn){
	if(!context_in && !being_processed && !context_out)
		loadOnCpu(rn);
		
	if(context_in && !being_processed && !context_out)
		runProcess(rn);
	
	if(!context_in && being_processed && !context_out)
		unloadOffCpu(rn);
	
	if(!context_in && !being_processed && context_out)
		return popFromCpu(rn);
	
	return NULL;
}

void Cpu::loadOnCpu(int rn){
	if(ready.size() != 0){
		context_in = ready.front();
		ready.pop_front();
		context_in->movedToCntxIn(rn);
		//Increments the next_action field
		next_action = rn+context_switch_time;
		switches++;
	}
}

void Cpu::runProcess(int rn){
	being_processed = context_in;
	context_in = NULL;
	being_processed->movedToCpu(rn);
	//Increments the next_action field
	if(flag=='f'||flag=='s'){
		next_action = being_processed->getRemainingTime()+rn;
		cout << "here" << endl;
	}
	else{ //round-robin
		if(being_processed->getRemainingTime()>t_slice)
			next_action = t_slice + rn;
		else
			next_action = being_processed->getRemainingTime() + rn;
	}
}

void Cpu::unloadOffCpu(int rn){
	switches++;
	context_out = being_processed;
	being_processed = NULL;
	cout << "time " << rn << "ms: Process " << context_out->getProcessId() << " switching out of CPU; will block on I/0 until time " << rn+context_switch_time+context_out->getIOTime() << "ms " << printQueue();
	context_out->movedFromCpu(rn);
	//Increments the next_action field
	next_action = rn+context_switch_time;
}


Process* Cpu::popFromCpu(int rn){
	Process* temp = context_out;
	context_out = NULL;
	temp->movedFromCntxOut(rn);
	loadOnCpu(rn);
	return temp;
}

//Call before context switch time
//Moves from the ready queue to the cpu
/*void Cpu::load_on(int rn){
	if(ready.size() != 0){
		Process* p = *(ready.begin());
		ready.pop_front();
		p->movedToCpu(rn, context_switch_time);
		being_processed = p;
		//Increments the next_action field
		if(flag=='f'||flag=='s')
			next_action = p->getRemainingTime()+context_switch_time;
		else //round-robin
			if(p->getRemainingTime()>t_slice)
				next_action = t_slice + context_switch_time;
			else
				next_action = p->getRemainingTime() + context_switch_time;
	}
}*/

void Cpu::fcfs_rr_add(Process* p){
	ready.push_back(p);
}
void Cpu::srt_add(Process* p){
	if(ready.empty()){
		ready.push_front(p);
	}
	else{
		list<Process*>::iterator itr;
		for (itr=ready.begin(); itr != ready.end(); ++itr){
			if((*itr)->getRemainingTime() > p->getRemainingTime()){
				ready.insert(itr, p);
				break;
			}
			// If equal the tie breaker is Process Id
			if((*itr)->getRemainingTime() == p->getRemainingTime() && (*itr)->getProcessId() > p->getProcessId()){
				ready.insert(itr, p);
				break;
			}
		}
		if(itr == ready.end())
			ready.push_back(p);
	}
}

string Cpu::printQueue(){
	string buf = "[Q";
	if(ready.empty())
		buf += " <empty>";
	else{
		for(list<Process*>::iterator itr=ready.begin(); itr != ready.end(); itr++){
			buf += " ";
			buf += (*itr)->getProcessId();
		}
	}
	buf += "]\n";
	return buf;
}

void Cpu::takeInStats(Process* p){
	wait_times.push_back(p->getTurnStats());
	turnaround_times.push_back(p->getWaitStats());
	total_burst_time += p->getTotalBurstTime();
	total_num_bursts += p->getTotalNumBursts();
}

int Cpu::getAverageTurnaroundTime(){
	int times;
	int bursts;
	int size;
	
	for(list<int*>::iterator itr=turnaround_times.begin(); itr != turnaround_times.begin(); itr++){
		size = (*itr)[0];
		for(int i = 1; i < size; i++){
			times += (*itr)[i];
			bursts++;
		}
	}
	return times/bursts;
}

int Cpu::getAverageWaitTime(){
	int times;
	int bursts;
	int size;
	
	for(list<int*>::iterator itr=wait_times.begin(); itr != wait_times.begin(); itr++){
		size = (*itr)[0];
		for(int i = 1; i < size; i++){
			times += (*itr)[i];
			bursts++;
		}
	}
	return times/bursts;
}