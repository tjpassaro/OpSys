#include "io_queue.h"
#include "process.h"
#include <list>
#include <iterator>

using namespace std;

IO_Queue::IO_Queue(){
    next_pop = -999;
}

void IO_Queue::add(Process* p){
    list<Process*>::iterator itr;
    for(itr = processes.begin(); itr != processes.end(); ++itr){
        if((*itr)->getNextActionTime() > p->getNextActionTime()){
            processes.insert(itr, p);
            break;
        }
        if((*itr)->getNextActionTime() == p->getNextActionTime() && (*itr)->getProcessId()>p->getNextActionTime()){
            processes.insert(itr, p);
            break;
        }
    }
    if(itr == processes.begin())
        next_pop = processes.front()-> getNextActionTime();
    if(itr == processes.end())
        processes.insert(itr, p);
}

Process* IO_Queue::popFront(){
    Process* p = processes.front();
    processes.pop_front();
    next_pop = processes.front()->getNextActionTime();
    return p;
}