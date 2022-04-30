#include "headers.h"
#include "circularQueue.h"
#include "priQ.h"

void clearResources(int);

circularQ processes;    // global vairable carrying all the processes
priQ pri_processes;

void ReadInputFile()
{
    FILE* ptr = fopen("processes.txt", "r");
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

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    circularQInit(&processes, 50);   // can handle up to 50 processes at once
    create_priQ(&pri_processes,50);
    ReadInputFile();
    // priQprint(&pri_processes);
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.

    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock.

    //initClk();

    // To get time use this function. 
    //int x = getClk();
    //printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    //destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}




