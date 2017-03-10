#include "cpu.h"
#include <iostream>
#include "process.h"
#include <list>
#include <string>

using namespace std;

//Constructor
Cpu::Cpu(int num_cores, char algo_type){
	if(algo_type != 'f' && algo_type != 'r' && algo_type != 's')
		cerr << "ERROR: incorrect flag\n\tf = fcfs\n\tr = rr\n\ts = srt" << endl;
	flag = algo_type;
	context_switch_time = 3; //half given

	if(flag == 'r')
		t_slice = 94;
	else
		t_slice = -1;
	
	being_processed = new Process*[num_cores];
	context_in = new Process*[num_cores];
	context_out = new Process*[num_cores];
	
	being_processed[0] = NULL;
	context_in[0] = NULL;
	context_out[0] = NULL;
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
	delete[] being_processed;
	delete[] context_in;
	delete[] context_out;
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

bool Cpu::isEmpty() const{
	return ready.empty() && !being_processed[0] && !context_in[0] && !context_out[0];
}

Process* Cpu::nextCpuAction(int rn){
	if(!context_in[0] && !being_processed[0] && !context_out[0]){
		loadOnCpu(rn);
	}
	
	else if(context_in[0] && !being_processed[0] && !context_out[0])
		runProcess(rn);
	
	else if(!context_in[0] && being_processed[0] && !context_out[0])
		unloadOffCpu(rn);
	
	else
		return popFromCpu(rn);
	
	return NULL;
}

void Cpu::loadOnCpu(int rn){
	if(ready.size() != 0){
		context_in[0] = ready.front();
		ready.pop_front();
		context_in[0]->movedToCntxIn(rn);
		//Increments the next_action field
		next_action = rn+context_switch_time;
		switches++;
	}
}

void Cpu::runProcess(int rn){
	if(context_in[0]->getRemainingTime() != context_in[0]->getBurstTime())
		cout << "time " << rn << "ms: Process " << context_in[0]->getProcessId() << " started using the CPU with " << context_in[0]->getRemainingTime() << "ms remaining "<< printQueue();
	else
		cout << "time " << rn << "ms: Process " << context_in[0]->getProcessId() << " started using the CPU " << printQueue();
	being_processed[0] = context_in[0];
	context_in[0] = NULL;
	being_processed[0]->movedToCpu(rn);
	//Increments the next_action field
	if(flag=='f'){
		next_action = being_processed[0]->getRemainingTime() + rn;
	}
	else if(flag=='s'){
			if(!ready.empty() && being_processed[0]->getRemainingTime() > ready.front()->getRemainingTime()){
				unloadOffCpu(rn);
			}
			else
				next_action = being_processed[0]->getRemainingTime() + rn;
	}
	else{ //round-robin
		if(being_processed[0]->getRemainingTime()>t_slice)
			next_action = t_slice + rn;
		else
			next_action = being_processed[0]->getRemainingTime() + rn;
	}
}

void Cpu::unloadOffCpu(int rn){
	being_processed[0]->updateRemainingTime(rn);
	if(flag=='r' && ready.empty() && being_processed[0]->getRemainingTime()!=0)
	{
		cout << "time " << rn << "ms: Time slice expired; no preemption because ready queue is empty " << printQueue();
		if(being_processed[0]->getRemainingTime()>t_slice)
			next_action = t_slice + rn;
		else
			next_action = being_processed[0]->getRemainingTime() + rn;
	}
	else{
		context_out[0] = being_processed[0];
		being_processed[0] = NULL;
		context_out[0]->movedFromCpu(rn);
		if(context_out[0]->getRemainingTime() == 0){
			if(context_out[0]->getNumBurstsLeft()!=0){
				cout << "time " << rn << "ms: Process " << context_out[0]->getProcessId() << " completed a CPU burst; " << context_out[0]->getNumBurstsLeft() << " burst";
				if(context_out[0]->getNumBurstsLeft() != 1)
					cout << "s";
				cout << " to go " << printQueue();
				cout << "time " << rn << "ms: Process " << context_out[0]->getProcessId() << " switching out of CPU; will block on I/O until time " << rn+context_switch_time+context_out[0]->getIOTime() << "ms " << printQueue();
			}
			else
				cout << "time " << rn << "ms: Process " << context_out[0]->getProcessId() << " terminated " << printQueue();
		}
		else if(flag=='r'){
			cout << "time " << rn << "ms: Time slice expired; process " << context_out[0]->getProcessId() << " preempted with " << context_out[0]->getRemainingTime() << "ms to go " << printQueue();
		}
		//Increments the next_action field
		next_action = rn+context_switch_time;
	}
}


Process* Cpu::popFromCpu(int rn){
	Process* temp = context_out[0];
	context_out[0] = NULL;
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
		being_processed[0] = p;
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
		if(being_processed[0]){
			being_processed[0]->updateRemainingTime(rn);
			if(being_processed[0]->getRemainingTime() > p->getRemainingTime()){
				cout << "will preempt " << being_processed[0]->getProcessId() << " " << printQueue();
				preempts++;
				ready.push_back(being_processed[0]);
				being_processed[0] = NULL;
				next_action = rn + context_switch_time; //not quite sure if this plus 3 makes it work in all casses
				ready.push_front(p);
			}
			else{
				ready.push_front(p);
				cout << "added to ready queue " << printQueue();
			}
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
				if(itr == ready.begin() && being_processed[0]){
					being_processed[0]->updateRemainingTime(rn);
					if(being_processed[0]->getRemainingTime() > p->getRemainingTime()){
						cout << "and will preempt " << being_processed[0]->getProcessId() << " " << printQueue();
						preempts++;
						next_action = rn;
					}
				}
				ready.insert(itr, p);
				cout << "added to ready queue " << printQueue();
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