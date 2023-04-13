#include "headers.h"

void clearResources(int);

int shmid;
int schedulerShmId;

void writer(int shmid, int id, int arrTime, int exTime, int P, int finishTime, int remainingTime)
{
    struct Process *shmaddr = (struct Process *)shmat(shmid, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }

    struct Process *tmpProcess;

    tmpProcess = Process__create(id, arrTime, exTime, P);

    // tmpProcess.arrivalTime = arrTime;
    // tmpProcess.id = id;

    Process__setFinishT(tmpProcess, finishTime);

    Process__setRemainingT(tmpProcess, remainingTime);
    // tmpProcess.finishTime = finishTime;
    // tmpProcess.executionTime = exTime;
    // tmpProcess.priority = P;
    // tmpProcess.remainingTime = remainingTime;

    *shmaddr = *tmpProcess;

    // char snum[5];
    // itoa(id, snum, 10);
    // strcpy((char *)shmaddr, snum);
    // strcpy((char *)shmaddr, (char)arrTime);
    // strcpy((char *)shmaddr, (char)exTime);
    // strcpy((char *)shmaddr, (char)P);
    // strcpy((char *)shmaddr, (char)finishTime);
    // strcpy((char *)shmaddr, (char)remainingTime);

    // else
    // {
    //     printf("\nWriter: Shared memory attached at address %x\n", shmaddr);
    //     strcpy((char *)shmaddr, "Initial string!");
    // }
    // while (1)
    // {
    //     // if (!strcmp((char *)shmaddr, "quit"))
    //     // {
    //         // break;
    //     // }
    // }
    // printf("\nWriter Detaching...");
    // shmdt(shmaddr);
}
int mesq_id;
int main(int argc, char *argv[])
{

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

    shmid = shmget(SHKEY, 4, IPC_CREAT | 0644);
    schedulerShmId = shmget(250, 128, IPC_CREAT | 0644);

    mesq_id = msgget(250, 0666 | IPC_CREAT);

    if ((long)shmid == -1)
    {
        perror("Error in creating shm!");
        exit(-1);
    }

    if (mesq_id == -1)
    {
        perror("Error in creating MesQ!");
        exit(-1);
    }

    int *shmaddr = (int *)shmat(shmid, (void *)0, 0);

    if ((long)shmaddr == -1)
    {
        perror("Error in attaching the shm in clock!");
        exit(-1);
    }

    ///

    struct msgbuff message;

    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.//////////////check this
    // 4. Use this function after creating the clock process to initialize clock
    system("'./clkScript.sh' &");
    // system("'./schedulerScript.sh' &");

    initClk();
    // To get time use this
    int x = getClk();
    // printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    // destroyClk(true);

    printf("\nIam here\n");
    printf("\nScheduler Shared memory ID = %d\n", schedulerShmId);
    printf("Mesq_ID = %d\n", mesq_id);
    printf("Shared memory ID = %d\n", shmid);
    // printf("Ismail Enters writer\n");

    // writer(schedulerShmId, 7, 100, 2, 3, 4, 8000);

    // message.mtype = 1;
    // message.request = 1;

    // int send_val = msgsnd(mesq_id, &message, sizeof(message.request), !IPC_NOWAIT);
    // if (send_val == -1)
    //     perror("Errror in send");
    // // shmctl(schedulerShmId, IPC_RMID, (struct shmid_ds *)0);
    // // shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);

    // int rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, !IPC_NOWAIT);

    // sleep(2);
    // printf("FINISH\n");

    // writer(schedulerShmId, 5, 10, 20, 30, 40, 80);

    // message.mtype = 1;
    // message.request = 1;

    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), !IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
    // send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);

    // if (send_val == -1)
    //     perror("Errror in send");
    // writer(schedulerShmId, 50, 10, 20, 30, 40, 800);

    // printf("Ismail Enters writer\n");

    //
    int pIdx = 0;
    // printf("%d", processes[pIdx]->arrivalTime);

    int prevclk = getClk();

    while (true)
    {
        int x = getClk();

        if (x != prevclk)
        {

            sleep(1);

            printf("current time is %d\n", x);

            while (pIdx < 10 && processes[pIdx]->arrivalTime == x) // fix this
            {
                printf("PID = %d, arrTime = %d will run now\n", processes[pIdx]->id, processes[pIdx]->arrivalTime);
                // here we send it to the scheduler
                writer(schedulerShmId, processes[pIdx]->id, processes[pIdx]->arrivalTime, processes[pIdx]->executionTime, processes[pIdx]->priority, processes[pIdx]->executionTime, processes[pIdx]->remainingTime);

                message.mtype = 1;
                message.request = 1;

                int send_val = msgsnd(mesq_id, &message, sizeof(message.request), IPC_NOWAIT);
                if (send_val == -1)
                    perror("Errror in send");

                // int rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, !IPC_NOWAIT);

                pIdx++;
            }
            prevclk = x;
        }
    }
}

void clearResources(int signum)
{
    shmctl(schedulerShmId, IPC_RMID, NULL);
    // shmctl(mesq_id, IPC_RMID, NULL);

    signal(SIGINT, clearResources);
    exit(0);
    // TODO Clears all resources in case of interruption
}