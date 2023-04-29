// ===============================================================================================
// =====================================    Libraries   ==========================================
// ===============================================================================================
// #pragma once
#include "headers.h"
// #include "math.h"

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
int runProcess(struct Process *);
void printSchedulerLog(int, int, char *, int, int, int, int);
void printQueue();

// ===============================================================================================
// =====================================    Global Variables    ==================================
// ===============================================================================================
int algo = 1, currentclk; // TODO: receive algorithim value

int utility_time = 0;
double processesNum = 0;
int numOfProcesses = 10; // TODO: get correct number from scheduler
float sum_times = 0;
int sum_waiting = 0;

float TA_arr[5000] = {0};
int kk = 0;

struct Node *qHead = NULL;
int id, arrTime, exTime, priority, finishTime, remainingTime;
struct Process *runningProcess = NULL;

// For Round-Robin
int clock = 0;
int quanta = 0;
int RR_clock = 0;
// For addProcess()
int prevAddedProcessId = -1;
// For schedulerLog
FILE *schedulerLog;
// For process shared memory
int processShmid;
int *processShmaddr;

// ===============================================================================================
// =====================================    Code   ===============================================
// ===============================================================================================

int main(int argc, char *argv[])
{
    // #######################
    // ####################### Some intial lines
    // #######################

    initClk();

    // ####################### Memory between process and scheduler
    key_t key_id;
    key_id = ftok("keyfile", 65);
    processShmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    if (processShmid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nProcess Shared memory ID = %d\n", processShmid);

    processShmaddr = (int *)shmat(processShmid, (void *)0, 0);
    if (processShmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }
    else
    {
        printf("\nWriter: Shared memory attached at address %x\n", processShmaddr);
    }

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

    // #######################
    // ####################### Main loop
    // #######################

    while (runningProcess || processesNum < numOfProcesses) // Main loop
    // while (getClk() < 43) // Temp for testing
    {
        if (prevclk != getClk())
        {
            printf("\n\n----------------    current clk = %d    ----------------\n", getClk());
            if (runningProcess)
            {
                runningProcess->remainingTime = *processShmaddr;
                printf("Running Process id:%d and remaining time is:%d\n", runningProcess->id, runningProcess->remainingTime);
                // printf("*processShmaddr = %d\n", *processShmaddr);
            }
            else
                printf("No running process now\n");
            // ====================
            // Reading Process from SHM
            // ====================

            msgctl(mesq_id, IPC_STAT, &__buf);

            // printf("\nBefore\n%d\n", __buf.msg_qnum);

            int currqnum = __buf.msg_qnum;
            while (currqnum != prevqnum) // TODO: FIX expected inf. loop.
            // while (currqnum != prevqnum && getClk() < 45) // Temp for testing
            {
                rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, !IPC_NOWAIT);
                // __buf.msg_qnum--;
                // msgctl(mesq_id, IPC_SET, &__buf);
                // msgctl(mesq_id, IPC_RMID, NULL);

                // printf("\nAfter\n%d\n", __buf.msg_qnum);

                if (message.request == 1)
                {

                    // printf("Ismail Enters read 999\n");
                    ttt = reader(schedulerShmId); // TODO: need modification to read all added processes
                                                  // it reads only one
                    // int send_val = msgsnd(mesq_id, &message, sizeof(message.request), !IPC_NOWAIT);

                    // printf("Ismail exit read\n");
                    // printf("Isamil out read if\n"); // TODO: KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
                }

                // printf("Isamil out read if 1\n");
                prevqnum = currqnum;
                // printf("Isamil out read if 2\n");
            }
            // printf("Delete: i am before if\n");

            // ====================
            // Stroing new Process in Queue
            // ====================
            if (ttt && ttt->executionTime != 0 && ttt->id != prevAddedProcessId)
            {
                // printf("Delete: i am in if\n");
                // do something
                struct Process *pAdd;
                pAdd = Process__create(ttt->id, ttt->arrivalTime, ttt->executionTime, ttt->priority);
                // runningProcess->startTime = currentclk; //////////
                // printf("Current time from addProcess here for Process %d is %d", pNew->id, currentclk);

                addProcess(pAdd);
                prevAddedProcessId = ttt->id;
                printQueue();

                if (ttt->id > processesNum)
                    processesNum = ttt->id;
                // use data;
                // printf("Delete: i am going out if\n");
            }

            // ====================
            // Run scheduling algorithim
            // ====================
            currentclk = getClk();
            Central_Processing_Unit(); // TODO: Uncomment.
            // After schedule process.. increase waiting time to other processes
            increaseWaitTime();
            prevclk = getClk();

            // sleep(2);
            if (runningProcess) // TODO: correct?!
                utility_time++;
        }
    }

    printSchedulerPerf();

    printf("%d\n\n\n\n", schedulerShmId);
    shmctl(schedulerShmId, IPC_RMID, (struct shmid_ds *)0);
    printf("After%d\n\n\n\n", schedulerShmId);

    // Memory between process and scheduler
    printf("\nWriter Detaching...");
    shmdt(shmaddr);
    printf("\nAbout to destroy the shared memory area !\n");
    shmctl(processShmid, IPC_RMID, (struct shmid_ds *)0);

    // }
    closeSchedulerLog();
    destroyClk(true);
}

// ===============================================================================================
// =====================================    Functions Implementation    ==========================
// ===============================================================================================

void increaseWaitTime()
{
    struct Node *temp = qHead;

    if (temp)
    {
        printf("p(wait) : ");
        while (temp && temp->data != NULL)
        {
            temp->data->waitingTime++;
            printf("%d(%d) ", temp->data->id, temp->data->waitingTime);
            temp = temp->next;
        }
        printf("\n");
    }
}

void printQueue()
{
    struct Node *temp = qHead;

    if (temp)
    {
        printf("qHead content : ");
        while (temp && temp->data != NULL)
        {
            printf("%d ", temp->data->id);

            temp = temp->next;
        }
        printf("\n");
    }
}

void printSchedulerPerf()
{
    FILE *schedulerPerf;
    schedulerPerf = fopen("schedular.perf", "w");

    printf("utility_time = %d/ clk = %d\n", utility_time, getClk()); // Delete: Testing
    fprintf(schedulerPerf, "CPU utilization = %.2f%%\n", (100.0 * utility_time) / getClk());
    printf("CPU utilization = %.2f%%\n", (100.0 * utility_time) / getClk());

    double avg_wTA = 1.0 * sum_times / processesNum;
    fprintf(schedulerPerf, "Avg WTA = %.2f\n", avg_wTA);
    printf("Avg WTA = %.2f\n", avg_wTA);

    fprintf(schedulerPerf, "Avg Waiting = %.2f\n", 1.0 * sum_waiting / processesNum);
    printf("Avg Waiting = %.2f\n", 1.0 * sum_waiting / processesNum);

    double squareSum = 0;
    for (int i = 0; i < kk; i++)
    {
        squareSum += (TA_arr[i] - avg_wTA) * (TA_arr[i] - avg_wTA);
    }
    double std = squareRoot(squareSum / processesNum); // TODO: Uncomment.
    fprintf(schedulerPerf, "Std WTA = %.2f\n", std);   // TODO: Uncommnet.
    printf("Std WTA = %.2f\n", std);

    fclose(schedulerPerf);
}

// #At time x process y state arr w total z remain y wait k
void printSchedulerLog(int time, int pId, char *state, int arr, int total, int remain, int wait)
{
    fprintf(schedulerLog, "At time %d process %d %s arr %d total %d remain %d wait %d\n", time, pId, state, arr, total, remain, wait);
    printf("At time %d process %d %s arr %d total %d remain %d wait %d\n", time, pId, state, arr, total, remain, wait);
}

void printSchedulerLog2(int time, int pId, char *state, int arr, int total, int remain, int wait, int TA, double WTA)
{
    fprintf(schedulerLog, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f\n", time, pId, state, arr, total, remain, wait, TA, WTA);
    printf("At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f\n", time, pId, state, arr, total, remain, wait, TA, WTA);
}

void openSchedulerLog()
{
    schedulerLog = fopen("scheduler.log", "w");
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

    // printf("Ismail  PID = %d, ExTime = %d, Arrival Time = %d, Priority = %d, Remaining Time = %d, Finish Time = %d\n", tmpProcess->id, tmpProcess->executionTime, tmpProcess->arrivalTime, tmpProcess->priority, tmpProcess->remainingTime, tmpProcess->finishTime);
    // printf("Delete: I am going outside reader\n");

    return tmpProcess;
}

// addProcess: add *Process to Queue with respect to the Algorithim

void addProcess(struct Process *pNew)
{
    printf("Hello from addProcess Adding process %d\n", pNew->id);

    // printf("Delete: I am inside addProcess\n");
    // printf("Current time from addProcess for Process %d is %d", pNew->id, currentclk);
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

void resumeProcess(struct Process *p)
{
    printf("Hello from resumeProcess\n");

    p->currentState = "resumed";
    kill(p->pId, SIGCONT);
    printSchedulerLog(currentclk, p->id, p->currentState, p->arrivalTime, p->executionTime, p->remainingTime, p->waitingTime);
}

void stopProcess(struct Process *p)
{
    printf("Hello from stopProcess\n");

    p->currentState = "stopped";
    kill(p->pId, SIGSTOP);
    printSchedulerLog(currentclk, p->id, p->currentState, p->arrivalTime, p->executionTime, p->remainingTime, p->waitingTime);
}

void finishProcess(struct Process *p)
{

    // printf("Hello from finishProcess\n");
    clcStat(p);
    p->currentState = "finished";
    printSchedulerLog2(currentclk, p->id, p->currentState, p->arrivalTime, p->executionTime, p->remainingTime, p->waitingTime, p->turnaroundTime, p->weightedTATime);
    // Process__destroy(p); // TODO: Uncomment.
    runningProcess = NULL; // TODO: Uncomment.
}

void clcStat(struct Process *p)
{
    // printf("Hello from calc_stat\n");
    // TODO: Uncomment.
    runningProcess->turnaroundTime = currentclk - runningProcess->arrivalTime;
    runningProcess->weightedTATime = runningProcess->turnaroundTime * 1.0 / runningProcess->executionTime;
    TA_arr[kk] = runningProcess->weightedTATime;
    kk++;
    sum_times += runningProcess->weightedTATime;
    runningProcess->waitingTime = runningProcess->startTime - runningProcess->arrivalTime;
    sum_waiting += runningProcess->waitingTime;
    // runningProcess = NULL;
}

// Scheduler fn.
void Central_Processing_Unit()
{
    // printf("Hello from CPU\n");
    // printf("Delete: I am going to switch_algo_ossama\n");
    switchAlgo();
    // printf("Delete: I am now out switch_algo_ossama\n");
}

// HPF Algorithim
void switch_HPF()
{
    // printf("Hello from switch_HPF\n");

    if (!runningProcess && qHead)
    {
        runningProcess = peek(&qHead);
        printf("remaining time = %d\n", runningProcess->remainingTime);
        if (runningProcess)
        {
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
                runProcess(runningProcess);
            }
        }
    }
    else if (runningProcess && !runningProcess->remainingTime)
    {
        finishProcess(runningProcess);
        if (qHead && peek(qHead))
        {
            runningProcess = peek(&qHead);
            printf("remaining time = %d\n", runningProcess->remainingTime);
            if (runningProcess)
            {

                pop(&qHead);
                if (runningProcess->startTime == -1)
                {
                    runningProcess->startTime = currentclk;
                    runProcess(runningProcess);
                }
            }
        }
    }
}

// SRTN Algorithim
void switch_SRTN()
{
    // printf("Hello from switch_SRTN\n");

    if (!runningProcess && qHead)
    {
        runningProcess = peek(&qHead);
        if (runningProcess)
        {
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
                runProcess(&runningProcess);
            }
        }
    }
    else if (runningProcess && qHead)
    {
        if (peek(qHead) && peek(&qHead)->remainingTime < runningProcess->remainingTime)
        {
            stopProcess(runningProcess);
            runningProcess->startTime = currentclk; //////////
            addProcess(runningProcess);
            runningProcess = peek(&qHead);
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
                runProcess(&runningProcess);
            }
            else
            {
                resumeProcess(runningProcess);
            }
        }
        else if (!runningProcess->remainingTime)
        {
            finishProcess(runningProcess);
            if (peek(qHead))
            {
                runningProcess = peek(&qHead);
                pop(&qHead);
                if (runningProcess->startTime == -1)
                {
                    runningProcess->startTime = currentclk;
                    runProcess(&runningProcess);
                }
                else
                {
                    resumeProcess(runningProcess);
                }
            }
        }
    }
}

// RR Algorithim
void switch_RR()
{
    // printf("Hello from switch_RR\n");

    if (!runningProcess && qHead)
    {
        runningProcess = peek(&qHead);
        pop(&qHead);
        if (runningProcess->startTime == -1)
        {
            runningProcess->startTime = currentclk;
            runProcess(&runningProcess);
        }
        RR_clock = currentclk;
    }
    else if (!runningProcess->remainingTime)
    {
        finishProcess(runningProcess);
        runningProcess = peek(&qHead);
        if (runningProcess)
        {
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
                runProcess(&runningProcess);
            }
            else
            {
                resumeProcess(runningProcess);
            }
            RR_clock = currentclk;
        }
    }
    else if (currentclk - RR_clock == quanta)
    {
        stopProcess(runningProcess);
        runningProcess->startTime = currentclk; //////////
        addProcess(runningProcess);
        runningProcess = peek(&qHead);
        if (runningProcess)
        {
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
                runProcess(&runningProcess);
            }
            else
            {
                resumeProcess(runningProcess);
            }
            RR_clock = currentclk;
        }
    }
}

void switchAlgo()
{
    // printf("Hello from switchAlgo\n");
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
int runProcess(struct Process *p)
{
    // printf("Hello from runProcess\n");
    // printf("p->remainingTime = %d\n", p->remainingTime);
    *processShmaddr = p->remainingTime;
    // printf("*processShmaddr = %d\n", *processShmaddr);

    // p->startTime = currentclk;

    p->pId = fork();

    if (p->pId == -1) // Forking error
    {
        printf("Error in process forking");
        return -1;
    }
    else if (p->pId == 0) // Child
    {
        // Run process.out file
        execl("./process.out", "process", NULL);
    }
    p->currentState = "started";
    // Print line in scheduler.log
    printSchedulerLog(currentclk, p->id, p->currentState, p->arrivalTime, p->executionTime, p->remainingTime, p->waitingTime);
    return p->pId;
}