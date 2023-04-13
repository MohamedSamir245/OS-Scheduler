#include "headers.h"
void addProcess(struct Process *);
void switch_algo_ossama();
int algo = 1, currentclk; // TODO: receive algorithim value
struct Node *qHead = NULL;
int id, arrTime, exTime, priority, finishTime, remainingTime;
struct Process *running_process_ossama = NULL;

// reader: read processes from "Shared Memory"
struct Process *reader(int shmid)
{
    // printf("Delete: I am inside reader\n");
    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }
    // printf("\nReader: Shared memory attached at address %x\n", shmaddr);
    // int i = 0;
    // for (i; i < 3; ++i)
    // {
    //     sleep(2);
    //     printf("\nData found = %s\n", (char *)shmaddr);
    // // }
    // strcpy((char *)shmaddr, "quit");
    // sleep(5);
    // printf("\nAbout to destroy the shared memory area !\n");
    // shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
    // for (int i = 0; i < 6; i++)
    // {
    // }
    // id = atoi((char *)shmaddr);
    // printf("%d\n", id);
    // // shmaddr++;
    // arrTime = atoi((char *)shmaddr);
    // printf("%d\n", arrTime);

    // // shmaddr++;
    // exTime = atoi((char *)shmaddr);
    // printf("%d\n", exTime);

    // // shmaddr++;
    // priority = atoi((char *)shmaddr);
    // printf("%d\n", priority);

    // // shmaddr++;
    // finishTime = atoi((char *)shmaddr);
    // printf("%d\n", finishTime);

    // // shmaddr++;
    // remainingTime = atoi((char *)shmaddr);
    // printf("%d\n", remainingTime);

    // // shmaddr++;
    struct Process *tmpProcess = (struct Process *)shmaddr;

    // addProcess(tmpProcess);

    printf("Ismail  PID = %d, ExTime = %d, Arrival Time = %d, Priority = %d, Remaining Time = %d, Finish Time = %d\n", tmpProcess->id, tmpProcess->executionTime, tmpProcess->arrivalTime, tmpProcess->priority, tmpProcess->remainingTime, tmpProcess->finishTime);
    // printf("Delete: I am going outside reader\n");

    return tmpProcess;
}

// addProcess: add *Process to Queue with respect to the Algorithim

void addProcess(struct Process *pNew)
{
    // printf("Delete: I am inside addProcess\n");
    if (algo == 1)
    {
        if (!qHead)
        {
            qHead = newNode(pNew, pNew->priority);
        }
        else
        push(&qHead, pNew, pNew->priority);
    }
    else if (algo == 2)
    {
        if (!qHead)
        {
            qHead = newNode(pNew, pNew->remainingTime);
        }
        else
        push(&qHead, pNew, pNew->remainingTime);
    }
    else
    {
        if (!qHead)
        {
            qHead = newNode(pNew, 0);
        }
        else
        push(&qHead, pNew, 0);
    }
    // printf("Delete: I am going outside addProcess\n");
}

// Scheduler fn.
void Central_Processing_Unit()
{
    printf("Hello from CPU\n");
    printf("Delete: I am going to switch_algo_ossama\n");
    switch_algo_ossama();
    printf("Delete: I am now out switch_algo_ossama\n");
    if (running_process_ossama)
    {
        running_process_ossama->remainingTime--;
        if (!running_process_ossama->remainingTime)
            running_process_ossama = NULL;
    }
}

// HPF Algorithim
void switch_HPF()
{
    if (!running_process_ossama)
    {
        printf("Delete: HPF - in peek\n");
        running_process_ossama = peek(qHead);
        printf("Delete: HPF - out peek\n");
        printf("Delete: HPF - in pop\n");
        printf("Running process address is %x\n  ", running_process_ossama);
        printf("remaining time = %d\n", running_process_ossama->remainingTime);
        if (running_process_ossama)
        {
            pop(qHead);
        }
        printf("Delete: HPF - out pop\n");
    }
}

// SRTN Algorithim
void switch_SRTN()
{
    if (!running_process_ossama && peek(qHead))
    {
        running_process_ossama = peek(qHead);
        pop(qHead);
    }
    else if (running_process_ossama && peek(qHead))
    {
        if (peek(qHead)->remainingTime < running_process_ossama->remainingTime)
        {
            addProcess(running_process_ossama);
            running_process_ossama = peek(qHead);
            pop(qHead);
        }
    }
}

int clock = 0;
int quanta = 0;
int RR_clock = 0;
// RR Algorithim
void switch_RR()
{
    if (!running_process_ossama->remainingTime)
    {
        running_process_ossama = peek(qHead);
        pop(qHead);
        RR_clock = currentclk;
    }
    else if (currentclk - RR_clock == quanta)
    {
        addProcess(running_process_ossama);
        running_process_ossama = peek(qHead);
        if (running_process_ossama)
        {
            pop(qHead);
            RR_clock = currentclk;
        }
    }
}

void switch_algo_ossama()
{
    if (algo == 1)
    {
        switch_HPF();
    }
    else if (algo == 2)
    {
        switch_SRTN();
    }
    else
        switch_RR();
}

int main(int argc, char *argv[])
{
    initClk();

    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    int schedulerShmId = shmget(250, 4, IPC_CREAT | 0644);
    if ((long)schedulerShmId == -1)
    {
        perror("Error in creating shm!");
        exit(-1);
    }
    struct Process *shmaddr = (struct Process *)shmat(schedulerShmId, (void *)0, 0);

    // for (int i = 0; i < 5; i++)
    // {
    struct Process *ttt;
    int prevclk = getClk();
    while (1)
    {
        if (prevclk != getClk())
        {
            printf("Ismail Enters read\n");
            ttt = reader(schedulerShmId); // TODO: need modification to read all added processes
                                          // it reads only one

            printf("Ismail exit read");
            if (ttt->executionTime != 0)
            {
                // do something
                addProcess(ttt);
                // use data;
            }
            printf("Prevclk = %d | current clk = %d\n", prevclk, getClk());
            currentclk = getClk();
            Central_Processing_Unit();
            prevclk = getClk();
        }
    }
    //    Central_Processing_Unit();
    //    Central_Processing_Unit();
    //    Central_Processing_Unit();

    printf("%d\n\n\n\n", schedulerShmId);
    shmctl(schedulerShmId, IPC_RMID, (struct shmid_ds *)0);
    printf("After%d\n\n\n\n", schedulerShmId);

    // }

    destroyClk(true);
}
