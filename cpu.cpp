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
	preempts = 0;
	switches = 0;
	total_burst_time = 0;
	total_num_bursts = 0;
}


void Cpu::del(){
	for(list<int*>::iterator itr=wait_times.begin(); itr != wait_times.end(); itr++)
		delete[] (*itr);
	for(list<int*>::iterator itr=turnaround_times.begin(); itr != turnaround_times.end(); itr++)
		delete[] (*itr);
}
void Cpu::add(Process* p, int rn, bool preempt){
	p->moveToReady(rn);
	if(isEmpty()){
		next_action = rn;
	}
	if( flag == 'f' || flag == 'r')
		fcfs_rr_add(p, preempt);
	else
		srt_add(p, rn, preempt);
}

Process* Cpu::nextCpuAction(int rn){
	if(!context_in && !being_processed && !context_out)
		loadOnCpu(rn);
	
	else if(context_in && !being_processed && !context_out)
		runProcess(rn);
	
	else if(!context_in && being_processed && !context_out)
		unloadOffCpu(rn);
	
	else
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
	if(context_in->getRemainingTime() != context_in->getBurstTime())
		cout << "time " << rn << "ms: Process " << context_in->getProcessId() << " started using the CPU with " << context_in->getRemainingTime() << "ms remaining "<< printQueue();
	else
		cout << "time " << rn << "ms: Process " << context_in->getProcessId() << " started using the CPU " << printQueue();
	being_processed = context_in;
	context_in = NULL;
	being_processed->movedToCpu(rn);
	//Increments the next_action field
	if(flag=='f'){
		next_action = being_processed->getRemainingTime() + rn;
	}
	else if(flag=='s'){
			if(!ready.empty() && being_processed->getRemainingTime() > ready.front()->getRemainingTime()){
				unloadOffCpu(rn);
			}
			else
				next_action = being_processed->getRemainingTime() + rn;
	}
	else{ //round-robin
		if(being_processed->getRemainingTime()>t_slice)
			next_action = t_slice + rn;
		else
			next_action = being_processed->getRemainingTime() + rn;
	}
}

void Cpu::unloadOffCpu(int rn){
	being_processed->updateRemainingTime(rn);
	if(flag=='r' && ready.empty() && being_processed->getRemainingTime()!=0)
	{
		cout << "time " << rn << "ms: Time slice expired; no preemption because ready queue is empty " << printQueue();
		if(being_processed->getRemainingTime()>t_slice)
			next_action = t_slice + rn;
		else
			next_action = being_processed->getRemainingTime() + rn;
	}
	else{
		context_out = being_processed;
		being_processed = NULL;
		context_out->movedFromCpu(rn);
		if(context_out->getRemainingTime() == 0){
			if(context_out->getNumBurstsLeft()!=0){
				cout << "time " << rn << "ms: Process " << context_out->getProcessId() << " completed a CPU burst; " << context_out->getNumBurstsLeft() << " burst";
				if(context_out->getNumBurstsLeft() != 1)
					cout << "s";
				cout << " to go " << printQueue();
				cout << "time " << rn << "ms: Process " << context_out->getProcessId() << " switching out of CPU; will block on I/O until time " << rn+context_switch_time+context_out->getIOTime() << "ms " << printQueue();
			}
			else
				cout << "time " << rn << "ms: Process " << context_out->getProcessId() << " terminated " << printQueue();
		}
		else if(flag=='r'){
			cout << "time " << rn << "ms: Time slice expired; process " << context_out->getProcessId() << " preempted with " << context_out->getRemainingTime() << "ms to go " << printQueue();
		}
		//Increments the next_action field
		next_action = rn+context_switch_time;
	}
}


Process* Cpu::popFromCpu(int rn){
	Process* temp = context_out;
	context_out = NULL;
	next_action = -1;
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

void Cpu::fcfs_rr_add(Process* p, bool preempt){
	ready.push_back(p);
	if(!preempt)
		cout << "added to ready queue " << printQueue();
	else
		preempts++;
}
void Cpu::srt_add(Process* p, int rn, bool preempt){
	if(ready.empty()){
		if(being_processed){
			being_processed->updateRemainingTime(rn);
			if(being_processed->getRemainingTime() > p->getRemainingTime()){
				cout << "will preempt " << being_processed->getProcessId() << " " << printQueue();
				preempts++;
				ready.push_back(being_processed);
				being_processed = NULL;
				next_action = rn + 3; //not quite sure if this plus 3 makes it work in all casses
			}
			ready.push_front(p);
		}
		else if(!preempt){
			ready.push_front(p);
			cout << "added to ready queue " << printQueue();
		}
	}
	else{
		list<Process*>::iterator itr;
		for (itr=ready.begin(); itr != ready.end(); ++itr){
			if((*itr)->getRemainingTime() > p->getRemainingTime()){
				if(itr == ready.begin() && being_processed){
					being_processed->updateRemainingTime(rn);
					if(being_processed->getRemainingTime() > p->getRemainingTime()){
						cout << "and will preempt " << being_processed->getProcessId() << " " << printQueue();
						preempts++;
						next_action = rn;
					}
				}
				ready.insert(itr, p);
				break;
			}
			// If equal the tie breaker is Process Id
			if((*itr)->getRemainingTime() == p->getRemainingTime() && (*itr)->getProcessId() > p->getProcessId()){
				ready.insert(itr, p);
				if(!preempt)
					cout << "added to ready queue " << printQueue();
				break;
			}
		}
		if(itr == ready.end()){
			ready.push_back(p);
			if(!preempt)
				cout << "added to ready queue " << printQueue();
		}
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
	wait_times.push_back(p->getWaitStats());
	turnaround_times.push_back(p->getTurnStats());
	total_burst_time += p->getTotalBurstTime();
	total_num_bursts += p->getTotalNumBursts();
}

double Cpu::getAverageTurnaroundTime(){
	int times = 0;
	int bursts = 0;
	int size;
	
	for(list<int*>::iterator itr=turnaround_times.begin(); itr != turnaround_times.end(); itr++){
		size = (*itr)[0];
		for(int i = 1; i < size; i++){
			times += (*itr)[i];
			bursts++;
		}
	}
	return (double)times/bursts;
}

double Cpu::getAverageWaitTime(){
	int times = 0;
	int bursts = 0;
	int size;
	
	for(list<int*>::iterator itr=wait_times.begin(); itr != wait_times.end(); itr++){
		size = (*itr)[0];
		for(int i = 1; i < size; i++){
			times += (*itr)[i];
			bursts++;
		}
	}
	return (double)times/bursts;
}