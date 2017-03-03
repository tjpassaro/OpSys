/*Process:

//From file
int burst_time;
int io_time;
pid_t pid;
//Run time
int burst_left;
int wait;
int bursts_left;
//In each queue
int next_time;
int start_time;


compare(p1, p2){
	if p1.burst_left < p2.burst_left
		return -1;
	else if p1.burst_left < p2.burst_left
		return 1;
	else
		if p1.pid < p2.pid
			return -1;
		else
			return 1;
}

FCFS: first in first out queue
SRT: priority queue with compare function
RR: first in first out queue*/