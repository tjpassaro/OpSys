#include "cpu.h"
#include <iostream>
#include "process.h"
#include <list>

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
		next_pop = rn;
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
		//Increments the next_pop field
		next_pop = rn+context_switch_time;
	}
}

void Cpu::runProcess(int rn){
	being_processed = context_in;
	context_in = NULL;
	being_processed->movedToCpu(rn);
	//Increments the next_pop field
	if(flag=='f'||flag=='s')
		next_pop = p->getRemainingTime()+rn;
	else //round-robin
		if(p->getRemainingTime()>t_slice)
			next_pop = t_slice + rn;
		else
			next_pop = p->getRemainingTime() + rn;
}

void Cpu::unloadOffCpu(int rn){
	context_out = being_processed;
	being_processed = NULL;
	context_out->movedFromCpu(rn);
	//Increments the next_pop field
	next_pop = rn+context_switch_time;
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
		//Increments the next_pop field
		if(flag=='f'||flag=='s')
			next_pop = p->getRemainingTime()+context_switch_time;
		else //round-robin
			if(p->getRemainingTime()>t_slice)
				next_pop = t_slice + context_switch_time;
			else
				next_pop = p->getRemainingTime() + context_switch_time;
	}
}*/

void Cpu::fcfs_rr_add(Process* p){
	ready.push_back(p);
}
void Cpu::srt_add(Process* p){
	for (list<Process*>::iterator itr=ready.begin(); itr != ready.end(); ++itr){
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