#include "headers.h"
#include "circularQueue.h"
#include "priQ.h"
#include "pcb.h"
#include "scheduler.h"

void clearResources(int);

circularQ processes;    // global vairable carrying all the processes
priQ pri_processes;
int clk_pid;
int scheduler_pid;
char selected_algo;
int stat_loc;

void ReadInputFile(char* filename)
{
    FILE* ptr = fopen(filename, "r");
    if (ptr == NULL) {
        printf("no such file.");
        return;
    }
 
    char buf[100];
	
	// 	Format string explanation:
	// 	% is the character which each scanf format string starts with;
	//	* indicates to not put the found pattern anywhere (typically you save pattern found into parameters after the format string, in this case the parameter is NULL);
	// 	[^\n] means any character except newline;
	// 	\n means newline;

	// so the [^\n]\n means a full text line ending with newline.

    int id, arrival, run, priority;     // used in reading the input file
	
	char *p;
    while(fscanf(ptr, "%s", buf) == 1)  // stops when reaching the end of the file
	{
		
		// fscanf(ptr, "%s", buf);
		if (buf[0] == '#')
		{
			//printf("\n\nCommented line found\n"); // then skip this line
			fscanf(ptr, "%*[^\n]\n");   // skips the line (till \n)
		}
		else
		{
			//printf("\n\nReal Line found\n"); // yb2a h3mlo read
			printf("id: %s\n", buf);
			id = (int) strtol(buf, &p, 10); // converts first string into integer

			fscanf(ptr, "%d", &arrival);
			printf("arrival: %d\n", arrival);

			fscanf(ptr, "%d", &run);
			printf("run: %d\n", run);
            int remaining = run; // all processes initially start with remaining time = needed run time
			
			fscanf(ptr, "%d", &priority);
			printf("Priority: %d\n", priority);

            process *pcb = (process *)malloc(sizeof(process));
            *pcb = (process) {
                .arrivaltime = arrival,
                .id = id,
                .runtime = run,
                .remainingtime = run,    // initially, remaining time = needed run time
                .waitingtime = 0,        // initially, waiting time = 0
                .priority = priority,
                .StartedBefore = 0,      // initially, process has not started before
                .status = 3,   
            };

            // enqueues pcb to the processes Circular Queue
            circularQEnqueue(&processes, pcb);
            priQenqueue(&pri_processes, pcb);
		}
		
	}

}

void ReadScheduleAlgo(char* ScheduleAlgo)
{
    //SJF
    if(ScheduleAlgo[0] == '1')
    {
        
    }
    //HPF
    else if (ScheduleAlgo[0] == '2')
    {
        
    }
    //RR
    else if (ScheduleAlgo[0] == '3')
    {

    }
    //multilevel
    else if (ScheduleAlgo[0] == '4')
    {
        
    }
}


// The process generators creats the clock and the scheduler
void forkClkAndScheduler(){

    clk_pid = fork();
    if(clk_pid == -1){
        printf("Error while forking \n");
        exit(1);
    }
    // Child(clock proc)
    else if(clk_pid == 0){
        execl("bin/clk.out","clk.out",(char *)NULL);
    }
    // Parent(process generator)
    else{
        scheduler_pid =fork();
        if(scheduler_pid == -1){
        printf("Error while forking \n");
        exit(1);
        }
        // Child(Scheduler proc)
        else if (scheduler_pid == 0){
            execl("bin/scheduler.out","scheduler.out",selected_algo,(char *)NULL);
        }
    }
}


int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    circularQInit(&processes, 50);   // can handle up to 50 processes at once
    create_priQ(&pri_processes,50);
    ReadInputFile(argv[1]);
    // priQprint(&pri_processes);
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    
    // ReadScheduleAlgo(argv[3]);
    selected_algo = *argv[3];

    // 3. Initiate and create the scheduler and clock processes.
    
    // Creating message queue
    int msgq_id, send_val;
    msgq_id = msgget(MSGQKEY, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        printf("Error in create");
        exit(-1);
    }
    //Creating the clock and the scheduler
    forkClkAndScheduler();

    // 4. Use this function after creating the clock process to initialize clock.

    initClk();

    while(processes.occupied != 0){
        // Getting the current clock time
        int curr_time = getClk();
        printf("Current Time is %d\n", curr_time);

        // TODO Generation Main Loop
        // 5. Create a data structure for processes and provide it with its parameters.
        process *curr_proc = circularQFront(&processes);
        if(curr_proc->arrivaltime <= curr_time){
            curr_proc = circularQDequeue(&processes);

            // 6. Send the information to the scheduler at the appropriate time.
            msgBuf message;
            message.mtype = 911; //may be in #DEFINE

            message.proc = *curr_proc;
            send_val = msgsnd(msgq_id, &message, sizeof(message.proc), !IPC_NOWAIT);
            
            if(send_val == -1){ printf("There was error in sending the message"); }
            free(curr_proc); // Deallocating the dynamically created process
        }
    }

    // The process generator is waiting for the scheduler to terminate
    int temp_pid = wait(&stat_loc);

    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    destroyClk(true);

    int msgq_id = msgget(MSGQKEY, 0666 | IPC_CREAT);
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);

    exit(0); // terminating the process   
}




