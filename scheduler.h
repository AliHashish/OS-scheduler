#pragma once

//include the algos

#include "headers.h"
#include "pcb.h"

scheduling_algo algo;
FILE *outputStats;      // File pointer to the output file, where stats will be written

int previousTimeUsage;  // Time when we last utilized the CPU
int idleTime = 0;       // Time CPU was idle in, initially = 0

int waitingTime = 0;    // waiting time of processes, initially = 0
int numProc = 0;        // number of processes, initially = 0
double avgWTA = 0;      // average weighted turnaround time, initially = 0


void schedulerIsForContextSwitch();

// Scheduler intializer
bool schedulerInitialize(int algo,int *msgq_id){

    // Opening the file where we will output the process updates
    outputStats = fopen("OutputStats/scheduler.log", "w");

    // Printing the opening statement
    fprintf(outputStats, "#At time x process y state arr w total z remain y wait k\n");
    fflush(outputStats);

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

    // Printing process info
    fprintf(outputStats, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",
        getClk(), proc->id, proc->arrivaltime, proc->runtime, proc->remainingtime, proc->waitingtime);

    fflush(outputStats);
    
    previousTimeUsage = getClk();   // starting from now, till another process utilizes the CPU
                                    // that will be the idle time.
}


// Resume the preempted process
void schedulerResume(process *proc){
    // Setting the current running process
    current_running_process = proc;
    if(!proc) return;

    proc->status = 2; // Marking the status as running
    kill(proc->id,SIGCONT); // Continue the process

    // Printing process info
    
    // Remember:
    // int StartedBefore; // 1 means it was started before, 0 means it has never been started before
    bool FirstTime = false;  // a flag that shows whether process is resuming or starting
                             // false means resuming
    if (proc->StartedBefore == 0) // then it is now starting
    {
        FirstTime = true;
    }
    fprintf(outputStats, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
         getClk(), proc->id, (FirstTime)? "started" : "resumed", proc->arrivaltime, proc->runtime, proc->remainingtime, proc->waitingtime);
    fflush(outputStats);

    // Calculating how long it has been idle
    idleTime += getClk() - previousTimeUsage;

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

void schedulerTermination(int SIGNUM){
    // 7eta mn el termination, 8albn hashelha wa5od el farah ht3mlo
    int procStatus;
    int procID = wait(&procStatus);
    if (WIFEXITED(procStatus)) {
        int exitCode = WEXITSTATUS(procStatus);
        if (exitCode) {
        // Handle error
        }
    }
    process *proc = pcbGetProcess(procID);



    numProc++;  // increasing number of processes
    int turnaroundTime = getClk() - proc->arrivaltime;
    double weightedTurnaroundTime = turnaroundTime / (double)proc->runtime;

    // double avgWeightedTurnaroundTime = avgWTA;  // temporary holder
    avgWTA = (weightedTurnaroundTime + (numProc - 1) * avgWTA) / numProc;
    // Method of calucation: similar to that of calculating the GPA
    // getting the total WTA by multiplying the number of processes (excluding the terminating one)
    // by the avgWTA. Then we add the new WTA, and divide by the total number of processes.

    waitingTime += proc->waitingtime;
    previousTimeUsage = getClk(); // updating the utilization time

    // Printing process info
    fprintf(outputStats, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
        getClk(), proc->id, proc->arrivaltime, proc->runtime, proc->remainingtime, proc->waitingtime, turnaroundTime, weightedTurnaroundTime);
    fflush(outputStats);
}

void schedulerFree(int SIGNUM){
    // closing the opened file
    fclose(outputStats);

    // Calculating the utilization stats
    outputStats = fopen("OutputStats/scheduler.perf", "w");
    double utilization = 1 - ((double) idleTime / getClk());   // calculates the utilization percentage
    double avgWaitingTime = (double)waitingTime / numProc;

    // Printing the utilization stats in another file
    fprintf(outputStats, "CPU utilization = %.2f%%\n", utilization * 100);
    fprintf(outputStats, "Avg WTA = %.2f\n", avgWTA);
    fprintf(outputStats, "Avg Waiting = %.2f\n", avgWaitingTime);
    fflush(outputStats);
    fclose(outputStats);

    // TODO: free the rest of the stuff here
}