#include "headers.h"
#include "circularQueue.h"
//#include "priQ.h"
//#include "pcb.h"
//#include "scheduler.h"
//#include "HPF.h"

void clearResources(int);
void getQuantumSize();

circularQ processes;    // global vairable carrying all the processes
// pri_processes;
int clk_pid;
int scheduler_pid;
int stat_loc;

// Reads input file and fills pcb
void ReadInputFile(char* filename)
{
    FILE* ptr = fopen("processes.txt", "r");
    if (ptr == NULL) {
        printf("no such file.");
        exit(1);
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
			//printf("id: %s\n", buf);
			id = (int) strtol(buf, &p, 10); // converts first string into integer

			fscanf(ptr, "%d", &arrival);
			//printf("arrival: %d\n", arrival);

			fscanf(ptr, "%d", &run);
			//printf("run: %d\n", run);
            int remaining = run; // all processes initially start with remaining time = needed run time
			
			fscanf(ptr, "%d", &priority);
			//printf("Priority: %d\n", priority);

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

            // priQenqueue(&pri_processes, pcb); // 8albn kona 3amleen dyh 3lshan n-test bs? el mfrood nshelha a3taked
		}
		
	}

}

/**
 * A function that gets the algorithm from the user 
 */
void getAlgorithmFromUser(){
    printf("Enter the required algorithm \n");
    printf("1. SJF\n2. HPF\n3. RR\n4. MLFQ\n");

    do{
        scanf("%d",&selected_algo);
        if(selected_algo < 1 || selected_algo > 4){
            printf("Invalid choice! Please enter a number from 1 to 4\n");
        }
    } while(selected_algo < 1 || selected_algo > 4);

    if(selected_algo == 1) SJFflag = 1;
    else if(selected_algo == 3) getQuantumSize();
}

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

// The process generators creats the clock and the scheduler
void forkClkAndScheduler(){

    clk_pid = fork();
    if(clk_pid < 0){
        printf("Error while forking \n");
        exit(1);
    }
    // Child(clock proc)
    else if(clk_pid == 0){
        execl("bin/clk.out","clk.out",(char *)NULL);
    }
    // Parent(process generator)
    else{
        scheduler_pid = fork();
        if(scheduler_pid < 0){
        printf("Error while forking \n");
        exit(1);
        }
        // Child(Scheduler proc)
        else if (scheduler_pid == 0){
            char selectedAlgorithmChar[5];
            sprintf(selectedAlgorithmChar, "%d", selected_algo);
            execl("bin/scheduler.out","scheduler.out",selectedAlgorithmChar,(char *)NULL);
        }
    }
}


int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    circularQInit(&processes, 50);   // can handle up to 50 processes at once
    // priQcreate(&pri_processes,50); // 8albn kona 3amleen dyh 3lshan n-test bs? el mfrood nshelha a3taked
    ReadInputFile(argv[1]);

    getAlgorithmFromUser();
    // priQprint(&pri_processes); // 8albn kona 3amleen dyh 3lshan n-test bs? el mfrood nshelha a3taked
    // priQprint(&pri_processes);
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    
    // ReadScheduleAlgo(argv[3]);
    //selected_algo = *argv[3];
    //selected_algo = '2';

    // 3. Initiate and create the scheduler and clock processes.
    
    // Creating message queue
    int msgq_id, send_val;
    msgBuf message;
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
        //printf("Current Time is %d\n", curr_time);

        // TODO Generation Main Loop
        // 5. Create a data structure for processes and provide it with its parameters.
        process *curr_proc = circularQFront(&processes);
        if(curr_proc->arrivaltime <= curr_time){
            curr_proc = circularQDequeue(&processes);

            // 6. Send the information to the scheduler at the appropriate time.
            message.mtype = 911; //may be in #DEFINE
            message.proc = *curr_proc;
            send_val = msgsnd(msgq_id, &message, sizeof(message.proc), !IPC_NOWAIT);
            
            if(send_val == -1){ printf("There was error in sending the message"); }
            free(curr_proc); // Deallocating the dynamically created process
        }
    }

    // The process generator is waiting for the scheduler to terminate
    printf("Waiting for scheduler to finish\n");
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




