#pragma once

#include "circularQueue.h"

// TODO
// Take it as input from user later on
int RR_quanta = 2;  // the length of the quanta
int RR_start;   // when the process started running

//circularQ* RRqueue;

bool RRaddProcess(void *type, process* p);
bool RRpreempt(void *type);
process* RRgetNextProcess(void *type);
bool RRremoveProcess(void *type, process* p);
bool RRfree(void *type);

void RRcast(void *type)   // casts the parameter into a circular queue pointer
{
    // queue = (circularQ *) type;
}


bool RRInitialize(scheduling_algo* current_algorithm) {
    if(!current_algorithm) return 0;

    circularQ* queue = (circularQ*)malloc(sizeof(circularQ));
    circularQInit(queue, 50);

    *current_algorithm = (scheduling_algo){
        queue,
        RRaddProcess,
        RRpreempt,
        RRgetNextProcess,
        RRremoveProcess,
        RRfree,
        RRcast,
    };

    return 1;
}



bool RRaddProcess(void *type, process* p) {
    //RRcast(type);
    circularQ *queue = (circularQ *)type;
    bool success = circularQEnqueue(queue,p) + 1;
    return success;
}


process* RRgetNextProcess(void *type) {
    //RRcast(type);
    circularQ *queue = (circularQ *)type;
    if (current_running_process == circularQFront(queue)) // This check needs to be done, to avoid skipping the first process.
    {
        process *proc = circularQDequeue(queue);    // dequeues the 1st process
        if (!proc)
        {
            return NULL;
        }
        circularQEnqueue(queue, proc);              // enqueues it at the end
        // the front now contains the new process
    }
    return circularQFront(queue);
}


bool RRpreempt(void *type) {
    int clk = getClk();         // gets current clock

    if(current_running_process == NULL)     // There is no process running
    {
        RR_start = clk;                     // sets the start time
        return true;
    }

    //RRcast(type);
    circularQ *queue = (circularQ *)type;
    if(clk - RR_start == RR_quanta)         // the process has taken its full quanta
    {
        RR_start = clk;                     // sets the start time of the new process

        // before preempting, we have to make sure there are other processes in the queue
        // and that the process is not terminating. If either options are false (i.e, there are no
        // processes in the queue, or process is terminating), then do not preempt.
        return (queue->occupied > 1) && (current_running_process->remainingtime > 0);
    }

    // if process has not taken its full quanta yet, do not preempt
    return false;
}


bool RRremoveProcess(void *type, process* p) {
    //RRcast(type);
    circularQ *queue = (circularQ *)type;
    process* removed_proc = circularQRemove(queue,p);
    if(removed_proc)    // then p was removed successfully
    {
        return true;
    }
    
    // reaching here means failure of removing p
    return false;
}


bool RRfree(void *type) {
    //RRcast(type);
    circularQ *queue = (circularQ *)type;
    int success = (circularQFree(queue))+ 1;
    // circularQFree returns 0 on success, -1 on failure
    // we need to turn that into 1 on success, 0 on failure
    // so we simply add one
    return success;
}
