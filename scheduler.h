#pragma once

//include the algos

#include "headers.h"

scheduling_algo algo;

void schedulerIsForContextSwitch();

// Scheduler intializer
bool schedulerInitialize(int algo,int *msgq_id){

    // Calling the initializer of each algorithm
    //SJF
    if(algo == 1)
    {
        
    }
    //HPF
    else if (algo == 2)
    {
        
    }
    //RR
    else if (algo == 3)
    {

    }
    //multilevel
    else if (algo == 4)
    {
        
    }
    
    // Creating the message queue
    int msgq_idTemp = msgget(MSGQKEY, 0666 | IPC_CREAT);
    if (msgq_idTemp == -1)
    {
        printf("Error in create");
        return false;
    }
    msgq_id = &msgq_idTemp;

    return true;
}

// Receives new processes from the process generator
bool schedulerReceiveMessage(int msgq_id,msgBuf *message){
    
    // Doesn't wait for the process generator to send message
    int rec_val = msgrcv(msgq_id, message, sizeof(process), 911, IPC_NOWAIT);
    if(rec_val == -1){return false;}
    return true;
}


// Creates a process
void schedulerCreateProcess(msgBuf *msg_buffer){
    
    int pid = fork();
    msg_buffer->proc.id = pid;
    // Child (process)
    if(pid == 0){
        execl("bin/process.out","process.out",msg_buffer->proc.remainingtime,(char *)NULL);
    }
    // Making the process stopped and check after if ready
    kill(pid,SIGTSTP);

    // insert in the PCB
    // Using the chosen algorithm on the created process
    algo.addProcess(algo.type,&msg_buffer->proc);
}


// Preempts a process
void schedulerPreempt(process *proc){
    if(!proc) return;

    proc->status = 3; // Stopped process
    proc->StartedBefore = 1; // Marking the process as ran before
    kill(proc->id,SIGTSTP); // Stopping the process

    // etba3 info

}


// Resume the preempted process
void schedulerResume(process *proc){
    // Setting the current running process
    current_running_process = proc;
    if(!proc) return;

    proc->status = 2; // Marking the status as running
    kill(proc->id,SIGCONT); // Continue the process

    // etba3 info
}


// Checks if the process needs context switch
void schedulerIsForContextSwitch(){
    bool check = algo.preempt(algo.type);

    if(check){
        schedulerPreempt(current_running_process);
        
        process* next = algo.getNextProcess(algo.type);
        schedulerResume(next);
    }
}