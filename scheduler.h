#pragma once

#include "headers.h"
#include "HPF.h"
#include "RR.h"
#include "SJF.h"
#include "MLFQ.h"
#include "pcb.h"

scheduling_algo algo;
FILE *outputStats;      // File pointer to the output file, where stats will be written

int previousTimeUsage;  // Time when we last utilized the CPU
int idleTime = 0;       // Time CPU was idle in, initially = 0

int waitingTime = 0;    // waiting time of processes, initially = 0
int numProc = 0;        // number of processes, initially = 0
double avgWTA = 0;      // average weighted turnaround time, initially = 0

void schedulerIsForContextSwitch();
void schedulerFree(int);
void schedulerTermination(int);
bool schedulerReceiveMessage(int,msgBuf*);


/**
 * A function that gets the RR quantum size from the user 
 */
void getQuantumSize(){
    printf("Please enter quantum size: \n");
    scanf("%d",&RRquanta);
    if (RRquanta < 1)
    {
        printf("Invalid input entered. Taking default value of 2 instead.\n");
        RRquanta = 2;
    }
}



/*
    Handler for SIGCHLD, whenever a child dies it:
    1- Removes it from algorithm
    2- Calculates TA, WTA and new AvgWTA
    3- Prints process info
    4- Removes it from PCB, sets it to NULL and then checks for next process
*/

void schedulerTermination(int SIGNUM){
    
    // getting finished process
    int procStatus;
    int procID = wait(&procStatus);
    if (WIFEXITED(procStatus)) {
        int exitCode = WEXITSTATUS(procStatus);
    }

    
    process *proc = pcbGetProcess(procID);
    algo.removeProcess(algo.type,proc);  //removing process from algorithm queue

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
    if(proc->remainingtime<0) proc->remainingtime = 0;
    // Printing process info
    fprintf(outputStats, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
        getClk(), proc->id, proc->arrivaltime, proc->runtime, proc->remainingtime, proc->waitingtime, turnaroundTime, weightedTurnaroundTime);
    fflush(outputStats);

    pcbRemove(proc); //removing from process control block
    current_running_process = NULL;
    schedulerIsForContextSwitch();
}

// Scheduler intializer
bool schedulerInitialize(int algo_num,int *msgq_id){

    // Opening the file where we will output the process updates
    outputStats = fopen("scheduler.log", "w");

    // Printing the opening statement
    fprintf(outputStats, "#At time x process y state arr w total z remain y wait k\n");
    fflush(outputStats);

    /* sigaction to handle SIGCHLD signal
       whenever a process dies, it calls schedulerTermination
       (sigaction since SIGCHLD cannot be handled directly)
    */
    struct sigaction chld_handle;
    chld_handle.sa_handler = schedulerTermination;
    sigemptyset(&chld_handle.sa_mask);
    chld_handle.sa_flags = SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &chld_handle, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
  
    signal(SIGINT,schedulerFree);

    initClk();
    previousTimeUsage = 0;


    // Calling the initializer of each algorithm
    //SJF
    if(algo_num == 1)
    {
        SJFflag = 1;
        SJFInitialize(&algo);
    }
    //HPF
    else if (algo_num == 2)
    {
        HPFInitialize(&algo);
    }
    //RR
    else if (algo_num == 3)
    {   
        getQuantumSize();
        RRInitialize(&algo);
    }
    //multilevel
    else if (algo_num == 4)
    {
        MLFQ_init(&algo);
    }
    
    pcbInitialize(200);

    // Creating the message queue
    int msgq_idTemp = msgget(MSGQKEY, 0666 | IPC_CREAT);
    if (msgq_idTemp == -1)
    {
        printf("Error in create");
        return false;
    }
    *msgq_id = msgq_idTemp;

    return true;
}

/*
    Receives new processes from the process generator (doesn't wait)
    message mtype is 911.
*/

bool schedulerReceiveMessage(int msgq_id,msgBuf *message){
    
    // Doesn't wait for the process generator to send message
    int rec_val = msgrcv(msgq_id, message, sizeof(process), 911, IPC_NOWAIT);
    if(rec_val == -1){
        if (errno != ENOMSG) {
      perror("Error in message queue\n");
      exit(-1);
            }
    return false;
    }
    return true;
}


/*
    Forking a process whenever a new process arrives in message queue,
    inserting it into process control block and adding it to algorithm queue.
    It's initially stopped until it moves to running by the scheduler.
*/

void schedulerCreateProcess(msgBuf *msg_buffer){
    
    int pid = fork();
    // Child (process)
    if(pid == 0){
        char pRemainingTime[10];
        sprintf(pRemainingTime, "%d", msg_buffer->proc.remainingtime);
        execl("bin/process.out","process.out",pRemainingTime,(char *)NULL);
    }
    msg_buffer->proc.pid = pid;
    
    // Making the process stopped (once it enters) and check after if ready
    kill(pid,SIGTSTP);

    // insert in the PCB
    process *pcbProcessEntry = pcbInsert(&msg_buffer->proc);
    // Using the chosen algorithm on the created process
    algo.addProcess(algo.type,pcbProcessEntry);
    printf("Created process %d\n",msg_buffer->proc.id);
    schedulerIsForContextSwitch();
}


/*
    Function that preempts current running process (after having checked if preemption is necessary)
    It raises SIGTSTP on it, and prints updated stats,
    then updates last utilization time.
*/

void schedulerPreempt(process *proc){
    if(!proc) return;
    //printf("Prempted process %d\n",current_running_process->id);
    proc->status = 3; // Stopped process
    proc->StartedBefore = 1; // Marking the process as ran before
    kill(proc->pid,SIGTSTP); // Stopping the process

    // Printing process info
    fprintf(outputStats, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",
        getClk(), proc->id, proc->arrivaltime, proc->runtime, proc->remainingtime, proc->waitingtime);

    fflush(outputStats);
    
    previousTimeUsage = getClk();   // starting from now, till another process utilizes the CPU
                                    // that will be the idle time.
}

/*
    Resuming process, (or running a process for the first time)
    Raises SIGCONT on the process to update waiting time,
    and updates utilization time.
*/

void schedulerResume(process *proc){
    // Setting the current running process
    current_running_process = proc;
    if(!proc) return;

    proc->status = 2; // Marking the status as running

    // Continue the process, SIGCONT to update waiting times once the process is resumed
    kill(proc->pid,SIGCONT); 

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

    // Updating the idleTime for the scheduler
    idleTime += getClk() - previousTimeUsage;
    previousTimeUsage = getClk();
}


/*
    Function that checks if context switching is required at this instance,
    If we need to preempt current process, we get next process and then continue.
*/

void schedulerIsForContextSwitch(){
    bool check = algo.preempt(algo.type);

    if(check){
        schedulerPreempt(current_running_process);
        
        process* next = algo.getNextProcess(algo.type);
        schedulerResume(next);
    }
}



/*  Function called whenever the scheduler dies (is killed from terminal)
    It prints final performance stats to scheduler.perf file.
    Frees process control block, algorithm and removes message queue.
*/

void schedulerFree(int SIGNUM){
    // closing the opened file
    fclose(outputStats);

    // Calculating the utilization stats
    outputStats = fopen("schedulerPerf.perf", "w");
    double utilization = 1 - ((double) idleTime / getClk());   // calculates the utilization percentage
    double avgWaitingTime = (double)waitingTime / numProc;

    // Printing the utilization stats in another file
    fprintf(outputStats, "CPU utilization = %.2f%%\n", utilization * 100);
    fprintf(outputStats, "Avg WTA = %.2f\n", avgWTA);
    fprintf(outputStats, "Avg Waiting = %.2f\n", avgWaitingTime);
    fflush(outputStats);
    fclose(outputStats);

    // TODO: free the rest of the stuff here
    pcbFree(); 
    algo.free(algo.type); // free current algorithm

    // Removing message queue
    int msgq_id = msgget(MSGQKEY, 0666 | IPC_CREAT);
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);

    // Installing the signal handler again to trap SIGINT again
    signal(SIGINT,schedulerFree);
}