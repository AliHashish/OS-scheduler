#pragma once 
#include "circularQueue.h"
#include "priQ.h"

int MLFQ_quanta = 4;
int MLFQ_start;

priQ *queue;


// Casting function
void cast(void *type) {
    queue = (priQ *) type;
}


// Inserting a process to the priQ
bool MLFQ_addProcess(void *type,process *proc){
    cast(type);

    bool check = priQenqueue(queue,proc);
    if(!check) return false;
    return true;
}


// Checks if the process should preempt
bool MLFQ_preempt(void *type){
    int clk = getClk();
    if(current_running_process == NULL){
        MLFQ_start = clk;
        return true;
    }

    cast(type);

    if(clk - MLFQ_start == MLFQ_quanta){
        MLFQ_start = clk;

        return (queue->capacity > 1) && (current_running_process->remainingtime > 0);
    }

    return false;
}


// Returns the right process to run
process *MLFQ_getNextProcess(void *type){
    
    cast(type);

    for (int i = 1; i <= 10; i++)
    {
        process *temp = priQpeek(queue);
        if(temp->priority != i) continue;

        if(temp == current_running_process){
            process *proc = priQdequeue(queue);
            if(!proc) return NULL;

            if(proc->priority <10){
                proc->priority += 1; // decreasing it priority
            }

            //Pushing the process back to the queue with the right priority
            bool check = priQenqueue(queue,proc);
            if(!check) return NULL;
            else break;
        }
    }
    return priQpeek(queue);
}


// Removes the process from the pri queue
bool MLFQ_removeProcess(void *type,process *proc){
    cast(type);

    process *temp = priQremove(queue,proc);
    return temp == proc ? 1 : 0; 
}


// Frees the alocated resources created by the algorithm
bool MLFQ_free(void *type){
    cast(type);
    return priQfree(queue);
}


// Initialzing the algorithm
bool MLFQ_init(scheduling_algo * curr_algo){
    if(!curr_algo) return 0;  

    queue = (priQ *)malloc(sizeof(priQ));
    create_priQ(queue,100);

    *curr_algo = (scheduling_algo){
        queue,
        MLFQ_addProcess,
        MLFQ_preempt,
        MLFQ_getNextProcess,
        MLFQ_removeProcess,
        MLFQ_free
    };
    return 1;
}