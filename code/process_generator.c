#include "headers.h"

void clearResources(int);


int main(int argc, char *argv[])
{
    // Test Area

    struct Process *processes[10];

    FILE *fp;

    char con[1000];
    fp = fopen("processes.txt", "r");

    if (!fp)
    {
        return 1;
    }
    int id, arTime, runTime, Prio;
    // fgets(con, 1000, fp);

    int i = 0;
    while (fscanf(fp, "%d %d %d %d", &id, &arTime, &runTime, &Prio) == 4)
    {
        printf("%d %d %d %d\n", id, arTime, runTime, Prio);

        processes[i++] = Process__create(id, arTime, runTime, Prio);

        // here create a process and add it to the array
    }

    for (int i = 0; i < 10; i++)
    {
        printf("PID = %d, ExTime = %d, Arrival Time = %d, Priority = %d, Remaining Time = %d, Finish Time = %d\n", processes[i]->id, processes[i]->executionTime, processes[i]->arrivalTime, processes[i]->priority, processes[i]->remainingTime, processes[i]->finishTime);
    }

    // printf("%d", processes[0]->arrivalTime);

    fclose(fp);

    printf("Choose the algorithm\n");
    printf("1 = HPF, 2 = SRTN, 3 = RR\n");
    int al = 0;

    while (al != 1 && al != 2 && al != 3)
    {
        scanf("%d", &al);
        if (al != 1 && al != 2 && al != 3)
        {
            printf("Choose from 1, 2 or 3\n");
        }
    }
    int Quantum = -1;

    if (al == 3)
    {
        printf("Enter the Quantum\n");
        while (Quantum <= 0)
        {
            scanf("%d", &Quantum);
            if (Quantum <= 0)
            {
                printf("Quantum must be greater than ZERO\n");
            }
        }
    }

    //

    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.//////////////check this
    // 4. Use this function after creating the clock process to initialize clock
    system("'./clkScript.sh' &");
    initClk();
    // To get time use this
    int x = getClk();
    // printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    // destroyClk(true);

    int pIdx = 0;
    printf("%d", processes[pIdx]->arrivalTime);
    while (true)
    {
        int x = getClk();
        sleep(1);

        printf("current time is %d\n", x);

        if (processes[pIdx]->arrivalTime == x)
        {
            printf("PID = %d, arrTime = %d will run now\n", processes[pIdx]->id, processes[pIdx]->arrivalTime);
            pIdx++;
        }
    }
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
}
