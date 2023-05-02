#include <stdio.h> //if you don't use scanf/printf change this include
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
#include <math.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

//============== algo and quantum ======
int algo;
int quantum;
struct msgbuff
{
    long mtype;
    int request;
};

struct memUnit
{
    int startLoc;
    int size;
    int f;

    struct memUnit *next;
    struct memUnit *previous;
};

// ============================
//

double squareRoot(double n)
{
    // Max and min are used to take into account numbers less than 1
    double lo = (1 < n) ? 1 : n;
    double hi = (1 > n) ? 1 : n;
    double mid;

    // Update the bounds to be off the target by a factor of 10
    while (100 * lo * lo < n)
        lo *= 10;
    while (0.01 * hi * hi > n)
        hi *= 0.1;

    for (int i = 0; i < 100; i++)
    {
        mid = (lo + hi) / 2;
        if (mid * mid == n)
            return mid;
        if (mid * mid > n)
            hi = mid;
        else
            lo = mid;
    }
    return mid;
}

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
    char *currentState;
    int turnaroundTime;
    float weightedTATime;
    int startTime;
    int waitingTime;
    int pId;
    // for memory
    int memSize;
    int startLocation;
};

void Process__init(struct Process *self, int id, int ar, int run, int p,int memsize)
{
    self->arrivalTime = ar;
    self->priority = p;
    self->executionTime = run;
    self->id = id;
    self->remainingTime = run;
    self->currentState = "na";
    self->turnaroundTime = 0;
    self->weightedTATime = 0;
    self->startTime = -1;
    self->waitingTime = 0;
    self->pId = -1;
    self->memSize=memsize;
}

void changecurrentstate(struct Process *self, char *s)
{
    self->currentState = s;
}

struct Process *Process__create(int id, int ar, int run, int p,int ms)
{
    struct Process *result = (struct Process *)malloc(sizeof(struct Process));
    Process__init(result, id, ar, run, p,ms);
    return result;
}

void Process__reset(struct Process *self)
{
}

void Process__setFinishT(struct Process *self, int fTime)
{
    self->finishTime = fTime;
}

void Process__setRemainingT(struct Process *self, int rTime)
{
    self->remainingTime = rTime;
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

// ======================================
// =============  Queue =================
// ======================================

// Node
struct Node
{
    struct Process *data;

    // Lower values indicate higher priority
    int priority;

    struct Node *next;
};

// Function to Create A New Node
struct Node *newNode(struct Process *p, int x)
{
    struct Node *temp = (struct Node *)malloc(sizeof(struct Node));
    temp->data = p;
    temp->priority = x;
    temp->next = NULL;

    return temp;
}

// Return the value at head
struct Process *peek(struct Node **head)
{
    return (*head)->data;
}

// Removes the element with the
// highest priority from the list
void pop(struct Node **head)
{
    struct Node *temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

// Function to push according to priority
void push(struct Node **head, struct Process *P, int x)
{
    struct Node *start = (*head);

    // Create new Node
    struct Node *temp = newNode(P, x);

    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if ((*head)->priority > x)
    {

        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else
    {

        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
               start->next->priority < x)
        {
            start = start->next;
        }

        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

// Function to check is list is empty
int isEmpty(struct Node **head)
{
    return (*head) == NULL;
}
// ======================================
// ======================================
// ======================================

///==============================
// don't mess with this variable//
int *shmaddr; //
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
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
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

//===================================================================
//==================== sending algo and quantum =====================
//===================================================================
void setAlgoAndQuantum(int al, int qu)
{
    algo = al;
    quantum = qu;
}

int getAlgo()
{
    return algo;
}

int getQuantum()
{
    return quantum;
}
