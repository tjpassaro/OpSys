#include "io_queue.h"
#include "process.h"
#include <list>
#include <iterator>

using namespace std;

IO_Queue::IO_Queue(){
    next_pop = -1;
}

//Adds elements in ascending time order
void IO_Queue::add(Process* p){
    if(processes.empty()){
        processes.push_front(p);
        next_pop = processes.front()->getNextActionTime();
    }
    else{
        list<Process*>::iterator itr;
        for(itr = processes.begin(); itr != processes.end(); ++itr){
            //If it's time is up before the current element than insert 
            if((*itr)->getNextActionTime() > p->getNextActionTime()){
                processes.insert(itr, p);
                break;
            }
            //If tied go by Process Id
            if((*itr)->getNextActionTime() == p->getNextActionTime() && (*itr)->getProcessId()>p->getProcessId()){
                processes.insert(itr, p);
                break;
            }
        }
        //If it was added in the first element then set the new action time
        if(p == processes.front()){
            next_pop = processes.front()->getNextActionTime();
        }
        //If last push at back
        if(itr == processes.end()){
            processes.push_back(p);
        }
    }
}

//Pops first element and returns it
//Updates next_pop
Process* IO_Queue::popFront(){
    Process* p = processes.front();
    processes.pop_front();
    if(!processes.empty())
        next_pop = processes.front()->getNextActionTime();
    else
        next_pop = -1;
    return p;
}

void IO_Queue::printQueue(){
    cout << "IO_Queue[";
    for(list<Process*>::iterator itr = processes.begin(); itr != processes.end(); itr++){
        cout << (*itr)->getProcessId() << " time: " << (*itr)->getStartActionTime();
    }
    cout << "]"<< endl;
}