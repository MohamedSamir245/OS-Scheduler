#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300


// ======================================
// ===========  Process =================
// ======================================

struct Process
{
    int id;
    int executionTime;
    int priority;
    int arrivalTime;
    int finishTime;
    int remainingTime;
};

void Process__init(struct Process *self, int id, int ar, int run, int p)
{
    self->arrivalTime = ar;
    self->priority = p;
    self->executionTime = run;
    self->id = id;
    self->remainingTime = run;
}

struct Process *Process__create(int id, int ar, int run, int p)
{
    struct Process *result = (struct Process *)malloc(sizeof(struct Process));
    Process__init(result, id, ar, run, p);
    return result;
}

void Process__reset(struct Process *self)
{
}

void Process__destroy(struct Process *process)
{
    if (process)
    {
        Process__reset(process);
        free(process);
    }
}

int Process__excutionTime(struct Process *self)
{
    return self->executionTime;
}

int Process__arrivalTime(struct Process *self)
{
    return self->arrivalTime;
}

int Process__remainingTime(struct Process *self)
{
    return self->remainingTime;
}

int Process__finishTime(struct Process *self)
{
    return self->finishTime;
}

int Process__priority(struct Process *self)
{
    return self->priority;
}

int Process__id(struct Process *self)
{
    return self->id;
}

// ======================================
// ======================================
// ======================================


///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================



int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
