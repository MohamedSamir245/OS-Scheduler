// ===============================================================================================
// =====================================    Libraries   ==========================================
// ===============================================================================================
#include "headers.h"

// ===============================================================================================
// =====================================        Functions       ==================================
// ===============================================================================================
struct Process *reader(int shmid);
void addProcess(struct Process *);
void myScheduler();
void switch_HPF();  // Algo 1
void switch_SRTN(); // Algo 2
void switch_RR();   // Algo 3
int runProcess(struct Process *);
int stopProcess(struct Process *);
int resumeProcess(struct Process *);
int finishProcess(struct Process *);
void printSchedulerPerf();
void openSchedulerLog();
void closeSchedulerLog();
void printSchedulerLog(int, int, char *, int, int, int, int);
void printSchedulerLog2(int, int, char *, int, int, int, int, int, double);
void printQueue();
void printRemainingTime();
void increaseWaitTime();

// ===============================================================================================
// =====================================    Global Variables    ==================================
// ===============================================================================================
int currentclk; // TODO: receive algorithim value

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
int quanta = 5;
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
    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    // -----------------------                           --------------------------------
    // -----------------------      Some intial lines    --------------------------------
    // -----------------------                           --------------------------------

    initClk();

    // algo = getAlgo();
    // printf("Algo %d", algo);
    // quanta = getQuantum();
    // printf("quanta %d", quanta);

    // ----------------------- Memory between process and scheduler
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
    // ---------------------------------------------------------------

    openSchedulerLog();

    int schedulerShmId = shmget(350, 128, IPC_CREAT | 0644);
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

    int rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, !IPC_NOWAIT);
    while (rec_val == -1)
    {
        perror("Errror in rec alg num");
        rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, !IPC_NOWAIT);
    }

    algo = message.request;
    printf("Algoooooooooooooooo %d", algo);

    if (algo == 3)
    {
        rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, !IPC_NOWAIT);
        while (rec_val == -1)
        {
            perror("Errror in rec quan num");
            rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, !IPC_NOWAIT);
        }
        quanta = message.request;
        printf("Quantum %d\n", quanta);
    }

    // for (int i = 0; i < 5; i++)
    // {
    struct Process *ttt;
    int prevclk = getClk();
    // printf("\nIam here\n");

    struct msqid_ds __buf;

    msgctl(mesq_id, IPC_STAT, &__buf);
    int prevqnum = __buf.msg_qnum;
    rec_val;

    // #######################
    // ####################### Main loop
    // #######################

    while (runningProcess || processesNum < numOfProcesses) // Main loop
    // while (getClk() < 43) // Temp for testing
    {
        if (prevclk != getClk())
        {
            printf("\n\n----------------    current clk = %d    ----------------\n", getClk());

            // ====================
            // Reading Process from SHM
            // ====================

            msgctl(mesq_id, IPC_STAT, &__buf);
            printf("169 - currentClk = %d\n", getClk());

            // printf("\nBefore\n%d\n", __buf.msg_qnum);

            int currqnum = __buf.msg_qnum;
            printf("174 - currentClk = %d\n", getClk());

            // while (currqnum != prevqnum) // TODO: FIX expected inf. loop.
            // while (currqnum != prevqnum && getClk() < 45) // Temp for testing
            // {
            //     printf("Curr Quan before rec     %d\n", currqnum);
            //     printf("Prev Quan before rec     %d\n", prevqnum);

            //     printf("Buffer QNUM  Before  %d\n", __buf.msg_qnum);
            // rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, IPC_NOWAIT);
            //     printf("Buffer QNUM  After  %d\n", __buf.msg_qnum);

            // __buf.msg_qnum--;
            // msgctl(mesq_id, IPC_SET, &__buf);
            // msgctl(mesq_id, IPC_RMID, NULL);

            // printf("\nAfter\n%d\n", __buf.msg_qnum);

            if (__buf.msg_qnum > 0)
            {
                printf("194 - currentClk = %d\n", getClk());
                rec_val = msgrcv(mesq_id, &message, sizeof(message.request), 0, IPC_NOWAIT);
                // if (rec_val) == -1)
                printf("197 - currentClk = %d\n", getClk());

                if (message.request == 1)
                {
                    printf("201 - currentClk = %d\n", getClk());
                    // printf("Ismail Enters read 999\n");
                    ttt = reader(schedulerShmId); // TODO: need modification to read all added processes
                                                  // it reads only one
                                                  // int send_val = msgsnd(mesq_id, &message, sizeof(message.request), !IPC_NOWAIT);
                    printf("206 - currentClk = %d\n", getClk());

                    // printf("Ismail exit read\n");
                    // printf("Isamil out read if\n"); // TODO: KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
                }
            }

            // printf("Isamil out read if 1\n");
            // prevqnum = currqnum;

            // printf("Curr Quan بعد rec     %d\n", currqnum);
            // printf("Prev Quan بعد rec     %d\n", prevqnum);
            // printf("Isamil out read if 2\n");
            // }
            // printf("Delete: i am before if\n");

            // ====================
            // Stroing new Process in Queue
            // ====================
            if (ttt && ttt->executionTime != 0 && ttt->id != prevAddedProcessId)
            {
                printf("227 - currentClk = %d\n", getClk());
                // printf("Delete: i am in if\n");
                // do something
                struct Process *pAdd;
                pAdd = Process__create(ttt->id, ttt->arrivalTime, ttt->executionTime, ttt->priority);
                // runningProcess->startTime = currentclk; //////////
                // printf("Current time from addProcess here for Process %d is %d", pNew->id, currentclk);
                printf("234 - currentClk = %d\n", getClk());

                addProcess(pAdd);
                printf("237 - currentClk = %d\n", getClk());
                prevAddedProcessId = ttt->id;
                printf("239 - currentClk = %d\n", getClk());

                if (ttt->id > processesNum)
                    processesNum = ttt->id;
                // use data;
                // printf("Delete: i am going out if\n");
                printf("245 - currentClk = %d\n", getClk());
            }

            // ====================
            // Run scheduling algorithim
            // ====================
            currentclk = getClk();
            printf("253 - currentClk = %d\n", getClk());

            myScheduler(); // TODO: Uncomment.
            printf("256 - currentClk = %d\n", getClk());

            // ====================
            // Some print after CPU
            // ====================
            if (runningProcess)
            {
                printf("263 - currentClk = %d\n", getClk());

                runningProcess->remainingTime = *processShmaddr;
                printf("Running Process id(wait):%d(%d) and remaining time is:%d\n", runningProcess->id, runningProcess->waitingTime, runningProcess->remainingTime);
                // printf("*processShmaddr = %d\n", *processShmaddr);
                printf("268 - currentClk = %d\n", getClk());
            }
            else
                printf("No running process now\n");
            printf("273 - currentClk = %d\n", getClk());

            // After schedule process.. increase waiting time to other processes
            increaseWaitTime();
            printf("277 - currentClk = %d\n", getClk());

            printQueue();
            printf("280 - currentClk = %d\n", getClk());

            printRemainingTime();
            printf("283 - currentClk = %d\n", getClk());

            prevclk = getClk();
            printf("286 - currentClk = %d\n", getClk());

            // sleep(2);
            if (runningProcess)
                utility_time++;
            printf("292 - currentClk = %d\n", getClk());
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

// Increases processes in queue(=waiting) waiting time by 1 & print processes in queue with waiting time.
// Ex out: "p(wait) : 3(5) 6(1) 2(27) 4(11)"
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

// Prints processes in queue with reamining time.
// Ex: "p(remain) : 3(5) 6(1) 2(27) 4(11)"
void printRemainingTime()
{
    struct Node *temp = qHead;

    if (temp)
    {
        printf("p(remain) : ");
        while (temp && temp->data != NULL)
        {
            printf("%d(%d) ", temp->data->id, temp->data->remainingTime);
            temp = temp->next;
        }
        printf("\n");
    }
}

// Prints processes queue.
// Ex: "qHead content : 3 6 2 4"
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

// Creates scheduler.perf file and opens it.
// Print scheduler.perf data.
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

// Print line in scheduler.log
// #At time x process y state arr w total z remain y wait k
void printSchedulerLog(int time, int pId, char *state, int arr, int total, int remain, int wait)
{
    fprintf(schedulerLog, "At time %d process %d %s arr %d total %d remain %d wait %d\n", time, pId, state, arr, total, remain, wait);
    printf("At time %d process %d %s arr %d total %d remain %d wait %d\n", time, pId, state, arr, total, remain, wait);
}

// Print line in scheduler.log (used when process finish)
// #At time x process y state arr w total z remain y wait k TA x WTA x
void printSchedulerLog2(int time, int pId, char *state, int arr, int total, int remain, int wait, int TA, double WTA)
{
    fprintf(schedulerLog, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f\n", time, pId, state, arr, total, remain, wait, TA, WTA);
    printf("At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f\n", time, pId, state, arr, total, remain, wait, TA, WTA);
}

// Creates scheduler.log file and opens it.
void openSchedulerLog()
{
    schedulerLog = fopen("scheduler.log", "w");
};

// Closes scheduler.log file
void closeSchedulerLog()
{
    fclose(schedulerLog);
};

// Reads process from Shared-Memory
struct Process *reader(int shmid)
{
    // printf("Delete: I am inside reader\n");
    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in reader");
        // printf("hhhhhhhhhhhhh");
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

// Adds *Process to Queue with respect to the Algorithim
void addProcess(struct Process *pNew)
{
    printf("Hello from addProcess, Adding process %d\n", pNew->id);

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
}

// Change process p state("resumed").
void resumeProcess(struct Process *p)
{
    *processShmaddr = p->remainingTime;
    p->currentState = "resumed";
    kill(p->pId, SIGCONT);
    printSchedulerLog(currentclk, p->id, p->currentState, p->arrivalTime, p->executionTime, p->remainingTime, p->waitingTime);
}

// Change process p state("stoped").
void stopProcess(struct Process *p)
{
    p->currentState = "stopped";
    kill(p->pId, SIGSTOP);
    printSchedulerLog(currentclk, p->id, p->currentState, p->arrivalTime, p->executionTime, p->remainingTime, p->waitingTime);
}

// Change process p state("finished").
void finishProcess(struct Process *p)
{

    clcStat(p);
    p->currentState = "finished";
    printSchedulerLog2(currentclk, p->id, p->currentState, p->arrivalTime, p->executionTime, p->remainingTime, p->waitingTime, p->turnaroundTime, p->weightedTATime);
    // Process__destroy(p); // TODO: Uncomment.
    runningProcess = NULL;
}

// Clc process p statistics (called on finish).
void clcStat(struct Process *p)
{
    runningProcess->turnaroundTime = currentclk - runningProcess->arrivalTime;
    runningProcess->weightedTATime = runningProcess->turnaroundTime * 1.0 / runningProcess->executionTime;
    TA_arr[kk] = runningProcess->weightedTATime;
    kk++;
    sum_times += runningProcess->weightedTATime;
    sum_waiting += runningProcess->waitingTime;
}

// Scheduler fn.
void myScheduler()
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

// HPF Algorithim
void switch_HPF()
{
    // Case: No running process & There exist qhead.
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
    // Case: Running process & It will finish.
    else if (runningProcess && !runningProcess->remainingTime)
    {
        finishProcess(runningProcess);
        // Case: There exit more process to schedule.
        if (qHead && peek(&qHead))
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
    // Case: No running process & There exist qhead.
    if (!runningProcess && qHead)
    {
        runningProcess = peek(&qHead);
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
    // Case: There is running process.
    else if (runningProcess)
    {
        // Case: There is a better schedule process
        if (qHead && peek(&qHead) && peek(&qHead)->remainingTime < runningProcess->remainingTime)
        {
            stopProcess(runningProcess);
            addProcess(runningProcess);
            runningProcess = peek(&qHead);
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
                runProcess(runningProcess);
            }
            else
            {
                resumeProcess(runningProcess);
            }
        }
        // Case: Running process will finish.
        else if (!runningProcess->remainingTime)
        {
            finishProcess(runningProcess);
            // Case: There exit more processes to schedule.
            if (qHead && peek(&qHead))
            {
                runningProcess = peek(&qHead);
                pop(&qHead);
                if (runningProcess->startTime == -1)
                {
                    runningProcess->startTime = currentclk;
                    runProcess(runningProcess);
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
    // Case: No running process & There exist qhead.
    if (!runningProcess && qHead)
    {
        runningProcess = peek(&qHead);
        if (runningProcess)
        {
            pop(&qHead);
            if (runningProcess->startTime == -1)
            {
                runningProcess->startTime = currentclk;
                runProcess(runningProcess);
            }
            RR_clock = currentclk;
        }
    }
    // Case: There is running process & it will finish.
    else if (runningProcess && !runningProcess->remainingTime)
    {
        finishProcess(runningProcess);
        // Case: There is more processes to schedule.
        if (qHead)
        {
            runningProcess = peek(&qHead);
            if (runningProcess)
            {
                pop(&qHead);
                if (runningProcess->startTime == -1)
                {
                    runningProcess->startTime = currentclk;
                    runProcess(runningProcess);
                }
                else
                {
                    resumeProcess(runningProcess);
                }
                RR_clock = currentclk;
            }
        }
    }
    // Case: There is running process & its quanta expired & There exit more processes to schedule.
    else if (runningProcess && currentclk - RR_clock == quanta)
    {
        printf("if 3\n");
        if (qHead && peek(&qHead))
        {
            stopProcess(runningProcess);
            addProcess(runningProcess);
            runningProcess = peek(&qHead);
            if (runningProcess)
            {
                pop(&qHead);
                if (runningProcess->startTime == -1)
                {
                    runningProcess->startTime = currentclk;
                    runProcess(runningProcess);
                }
                else
                {
                    resumeProcess(runningProcess);
                }
                RR_clock = currentclk;
            }
        }
    }
}

// Fork and run Process p.
// return process->pId on success | -1 on failure
int runProcess(struct Process *p)
{
    *processShmaddr = p->remainingTime;

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
    printSchedulerLog(currentclk, p->id, p->currentState, p->arrivalTime, p->executionTime, p->remainingTime, p->waitingTime);
    return p->pId;
}