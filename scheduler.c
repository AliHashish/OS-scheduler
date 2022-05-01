#include "scheduler.h"

int main(int argc, char *argv[])
{
    initClk();

    // reading the selected algorithm
    int algo = atoi(argv[1]);
    int msgq_id;
    //TODO: implement the scheduler.
    schedulerInitialize(algo,&msgq_id);
    msgBuf msgq_buffer;
    msgq_buffer.mtype = 911;
    // Initializer for the algorithm type
    // takes algo

    //TODO: upon termination release the clock resources.
    destroyClk(true);
}
