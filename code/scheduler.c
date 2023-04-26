// ===============================================================================================
// =====================================    Libraries   ==========================================
// ===============================================================================================
#pragma once
#include "headers.h"
#include "math.h"

// ===============================================================================================
// =====================================        Functions       ==================================
// ===============================================================================================
struct Process *reader(int shmid);
void addProcess(struct Process *);
void printSchedulerPerf();
void Central_Processing_Unit();
void switchAlgo();
void switch_HPF();
void switch_SRTN();
void switch_RR();
int runProcess(struct Process);
void printSchedulerLog(int, int, char *, int, int, int, int);

// ===============================================================================================
// =====================================    Global Variables    ==================================
// ===============================================================================================
int algo = 1, currentclk; // TODO: receive algorithim value

int utility_time = 0;
double processesNum = 0;
int sum_times = 0;
int sum_waiting = 0;

int TA_arr[5000] = {0};
int kk = 0;

struct Node *qHead = NULL;
int id, arrTime, exTime, priority, finishTime, remainingTime;
struct Process *runningProcess = NULL;

// For Round-Robin
int clock = 0;
int quanta = 0;
int RR_clock = 0;

FILE *schedulerLog;

// ===============================================================================================
// =====================================    Code   ===============================================
// ===============================================================================================

int main(int argc, char *argv[])
{
    initClk();
    openSchedulerLog();
    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    int schedulerShmId = shmget(250, 128, IPC_CREAT | 0644);
    int mesq_id = msgget(250, 0666 | IPC_CREAT);

    if ((long)schedulerShmId == -1)
    {
        perror("Error in creating shm!");
        exit(-1);
    }
    if (mesq_id == -1)
    {
        perror("Error in creating MesQ!");
        exit(-1);
    }

    printf("\nScheduler Shared memory ID = %d\n", schedulerShmId);
    printf("Mesq_ID = %d\n", mesq_id);
    // printf("Shared memory ID = %d\n", shmid);

    struct Process *shmaddr = (struct Process *)shmat(schedulerShmId, (void *)0, 0);
    struct msgbuff message;

    // for (int i = 0; i < 5; i++)
    // {
    struct Process *ttt;
    int prevclk = getClk();
    // printf("\nIam here\n");

    struct msqid_ds __buf;

    msgctl(mesq_id, IPC_STAT, &__buf);
    int prevqnum = __buf.msg_qnum;
    int rec_val;
    while (1)
    {
        if (prevclk != getClk())
        {
            //
            msgctl(mesq_id, IPC_STAT, &__buf);

            // printf("\nBefore\n%d\n", __buf.msg_qnum);

            int currqnum = __buf.msg_qnum;
            while (currqnum != prevqnum)
            {
                rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, !IPC_NOWAIT);
                // __buf.msg_qnum--;
                // msgctl(mesq_id, IPC_SET, &__buf);
                // msgctl(mesq_id, IPC_RMID, NULL);

                // printf("\nAfter\n%d\n", __buf.msg_qnum);

                if (message.request == 1)
                {

                    printf("Ismail Enters read\n");
                    ttt = reader(schedulerShmId); // TODO: need modification to read all added processes
                                                  // it reads only one
                    // int send_val = msgsnd(mesq_id, &message, sizeof(message.request), !IPC_NOWAIT);

                    printf("Ismail exit read\n");
                }

                prevqnum = currqnum;
            }

            if (ttt && ttt->executionTime != 0)
            {
                // printf("Delete: i am in if\n");
                // do something
                addProcess(ttt);
                if (ttt->id > processesNum)
                    processesNum = ttt->id;
                // use data;
                // printf("Delete: i am going out if\n");
            }
            printf("Prevclk = %d | current clk = %d\n", prevclk, getClk());
            currentclk = getClk();
            Central_Processing_Unit();
            prevclk = getClk();

            // sleep(2);
            utility_time++;
        }
    }

    printSchedulerPerf();

    printf("%d\n\n\n\n", schedulerShmId);
    shmctl(schedulerShmId, IPC_RMID, (struct shmid_ds *)0);
    printf("After%d\n\n\n\n", schedulerShmId);

    // }
    closeSchedulerLog();
    destroyClk(true);
}

// ===============================================================================================
// =====================================    Functions Implementation    ==========================
// ===============================================================================================

void printSchedulerPerf()
{
    FILE *schedulerPerf;
    schedulerPerf = fopen("schedular.perf", "w");

    fprintf(schedulerPerf, "CPU utilization = %d%%\n", 100.0 * utility_time / getClk());
    // printf("CPU utilization = %d%%\n", 100.0 * utility_time / getClk());

    double avg_wTA = 1.0 * sum_times / processesNum;
    fprintf(schedulerPerf, "Avg WTA = %d\n", avg_wTA);
    // printf("Avg WTA = %d\n", avg_wTA);

    fprintf(schedulerPerf, "Avg Waiting = %d\n", 1.0 * sum_waiting / processesNum);
    // printf("Avg Waiting = %d\n", 1.0 * sum_waiting / processes_num);

    double squareSum = 0;
    for (int i = 0; i < kk; i++)
    {
        squareSum += (TA_arr[i] - avg_wTA) * (TA_arr[i] - avg_wTA);
    }
    // double std = sqrt(squareSum / processesNum); // TODO: Uncomment.
    // fprintf(schedulerPerf, "Std WTA = %d\n", std); // TODO: Uncommnet.
    // printf("Std WTA = %d\n", std);

    fclose(schedulerPerf);
}

// #At time x process y state arr w total z remain y wait k
void printSchedulerLog(int time, int pId, char *state, int arr, int total, int remain, int wait)
{
    fprintf(schedulerLog, "At time %d process %d %s arr %d total %d remain %d wait %d\n", time, pId, state, arr, total, remain, wait);
    // printf("At time %d process %d %s arr %d total %d remain %d wait %d\n", time, pId, state, arr, total, remain, wait);
}

void openSchedulerLog()
{
    schedulerLog = fopen("schedular.log", "w");
};

void closeSchedulerLog()
{
    fclose(schedulerLog);
};

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
    switchAlgo();
    printf("Delete: I am now out switch_algo_ossama\n");
    if (runningProcess)
    {
        runningProcess->remainingTime--;
        if (!runningProcess->remainingTime)
        {
            runningProcess->turnaroundTime = currentclk - runningProcess->arrivalTime;
            runningProcess->weightedTATime = runningProcess->turnaroundTime / runningProcess->executionTime;
            TA_arr[kk] = runningProcess->weightedTATime;
            kk++;
            sum_times += runningProcess->weightedTATime;
            runningProcess->waitingTime = runningProcess->startTime - runningProcess->arrivalTime;
            sum_waiting += runningProcess->waitingTime;
            runningProcess = NULL;
        }
    }
}

// HPF Algorithim
void switch_HPF()
{
    if (!runningProcess && qHead)
    {
        printf("Delete: HPF - in peek\n");
        // printf("Delete: qhead = %x\n",qHead);
        // printf("Delete: qhead -> data = %x | qhead -> next = %x | qheaf -> priority =%d\n",qHead->data,qHead->next,qHead->priority);
        runningProcess = peek(&qHead);
        printf("Delete: HPF - out peek\n");
        printf("Delete: HPF - in pop\n");
        printf("Running process address is %x\n  ", runningProcess);
        printf("remaining time = %d\n", runningProcess->remainingTime);
        if (runningProcess)
        {
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
            }
        }
        printf("Delete: HPF - out pop\n");
    }
}

// SRTN Algorithim
void switch_SRTN()
{
    if (!runningProcess && peek(&qHead))
    {
        printf("Delete: SRTN - in peek\n");
        // printf("Delete: qhead = %x\n",qHead);
        // printf("Delete: qhead -> data = %x | qhead -> next = %x | qheaf -> priority =%d\n",qHead->data,qHead->next,qHead->priority);
        runningProcess = peek(&qHead);
        printf("Delete: SRTN - out peek\n");
        printf("Delete: SRTN - in pop\n");
        pop(&qHead);
        if (runningProcess->startTime == -1)
        {
            runningProcess->startTime = currentclk;
        }
        printf("Delete: SRTN - out pop\n");
        printf("Running process address is %x\n  ", runningProcess);
        printf("remaining time = %d\n", runningProcess->remainingTime);
    }
    else if (runningProcess && peek(&qHead))
    {
        if (peek(&qHead)->remainingTime < runningProcess->remainingTime)
        {
            addProcess(runningProcess);
            runningProcess = peek(&qHead);
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
            }
        }
    }
}

// RR Algorithim
void switch_RR()
{
    if (!runningProcess->remainingTime && qHead)
    {
        runningProcess = peek(&qHead);
        pop(&qHead);
        if (runningProcess->startTime == -1)
        {
            runningProcess->startTime = currentclk;
        }
        RR_clock = currentclk;
    }
    else if (currentclk - RR_clock == quanta)
    {
        addProcess(runningProcess);
        runningProcess = peek(&qHead);
        if (runningProcess)
        {
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
            }
            RR_clock = currentclk;
        }
    }
}

void switchAlgo()
{
    if (algo == 1)
    {
        switch_HPF();
    }
    else if (algo == 2)
    {
        switch_SRTN();
    }
    else // 3
        switch_RR();
}

// Run Process process.
// return pId on success | -1 on failure
int runProcess(struct Process p)
{
    int p.pId = fork();

    if (p.pId == -1) // Forking error
    {
        printf("Error in process forking");
        return -1;
    }
    else if (p.pId == 0) // Child
    {
        // Run process.out file
        execl("./process.out", "process", NULL);
    }

    // Print line in scheduler.log
    // printSchedulerLog(...); // TODO: what is the parameters.
    return p.pId;
}

void stopProcess()
{
    
}

void terminateProcess()
{}