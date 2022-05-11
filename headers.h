#pragma once
#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

#define MSGQKEY 167

int selected_algo;
int SJFflag = 0;

///==============================
//don't mess with this variable//
int *shmaddr; //
//===============================


// stores the data of the processes
typedef struct process
{
    int id;
    int pid;
    int arrivaltime;
    int runtime;
    int remainingtime;
    int waitingtime;
    int priority;
    int StartedBefore; // 1 means it was started before, 0 means it has never been started before
    int status;
    // possible status values:
    // 1 ----> started
    // 2 ----> resumed
    // 3 ----> stopped
    // 4 ----> finished
} process;

// The running process for all files
process *current_running_process = NULL;

int getClk()
{
    return *shmaddr;
}

/*
 * All processes call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All processes call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

// Algorithms structer
typedef struct scheduling_algo{
    void *type;
    bool (*addProcess)(void *type, process *proc);
    bool (*preempt)(void *type);
    process *(*getNextProcess)(void *type);
    bool (*removeProcess)(void *type, process *proc);
    bool (*free)(void *type);
    void (*cast)(void *type);

}scheduling_algo;

// Message buffer for the process
typedef struct {
  long mtype;
  process proc;
} msgBuf;