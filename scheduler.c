#include "scheduler.h"

int main(int argc, char *argv[])
{

    // reading the selected algorithm
    int algo  = atoi(argv[1]);
    int msgq_id;

    //TODO: implement the scheduler.
    int  x = schedulerInitialize(algo,&msgq_id);
    msgBuf msgq_buffer;
    msgq_buffer.mtype = 911;

    int now, previous_time = -1;
    while(true) {

        now = getClk();

        /* Check if a new process was received, 
           if so add it to the queue and update pcb
           and check context switching.
        */
        if(schedulerReceiveMessage(msgq_id,&msgq_buffer)==false) {
           // printf("Message not received\n");
        }
        else {
            schedulerCreateProcess(&msgq_buffer);
            while(schedulerReceiveMessage(msgq_id,&msgq_buffer))
            {
                schedulerCreateProcess(&msgq_buffer);
            }
            if(now > previous_time) {
                pcbRefresh();
            }
            schedulerIsForContextSwitch();
            if(now > previous_time) {
                previous_time = now;
            }
        }
        /* Each second update pcb and check for context switching */
        if(now > previous_time) {
            pcbRefresh();
            schedulerIsForContextSwitch();
            previous_time = now;
        }

    } 
    
    //TODO: upon termination release the clock resources.
    destroyClk(true);
    return 0;
}
