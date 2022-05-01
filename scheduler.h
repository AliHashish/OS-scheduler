#pragma once

//include the algos

#include "headers.h"

scheduling_algo algo;


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
    int msgq_idTemp = msgget(key_id, 0666 | IPC_CREAT);
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
    int rec_val = msgrcv(msgq_id, message, sizeof(message.proc), 911, IPC_NOWAIT);
    if(rec_val == -1){return false;}
    return true;
}