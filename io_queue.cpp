#include "io_queue.h"
#include "process.h"
#include <list>
#include <iterator>

using namespace std;

IO_Queue::IO_Queue(){
    next_pop = -999;
}

void IO_Queue::add(Process* p){
    if(processes.empty()){
        processes.push_front(p);
        next_pop = processes.front()->getNextActionTime();
    }
    else{
        list<Process*>::iterator itr;
        for(itr = processes.begin(); itr != processes.end(); ++itr){
            //cout << "enter for loop\n";
            //cout << "check itr: " << (*itr)->toString() << endl;
            //cout << "check p: " << p->toString() << endl;
            if((*itr)->getNextActionTime() > p->getNextActionTime()){
                //cout << "enter if(itr > p)\npre insert\n";
                // if(itr == processes.begin()){
                //     //cout << "adding to front\n";
                //     processes.push_front(p);
                //     //cout << "added to front\n";
                // }
               // else 
                    processes.insert(itr, p);
                    //cout << "post insert \n";
                break;
            }
            //cout << "before tie breaker\n";
            if((*itr)->getNextActionTime() == p->getNextActionTime() && (*itr)->getProcessId()>p->getProcessId()){
                //cout << "enter tie breaker\npre insert\n";
                processes.insert(itr, p);
                //cout << "post insert\n";
                break;
            }
        }
        if(p == processes.front()){
            //cout << "p == process.front\n";
            next_pop = processes.front()->getNextActionTime();
        }
        if(itr == processes.end()){
            //cout << "itr reached end\n";
            processes.push_back(p);
        }
    }
}

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