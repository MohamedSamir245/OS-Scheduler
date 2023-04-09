#include "headers.h"

int algo;

int id, arrTime, exTime, priority, finishTime, remainingTime;

struct Process *reader(int shmid)
{
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

    printf("Ismail  PID = %d, ExTime = %d, Arrival Time = %d, Priority = %d, Remaining Time = %d, Finish Time = %d\n", tmpProcess->id, tmpProcess->executionTime, tmpProcess->arrivalTime, tmpProcess->priority, tmpProcess->remainingTime, tmpProcess->finishTime);

    return tmpProcess;
}

void addProcess(struct Node *head, struct Process *pNew)
{
    if (algo == 1)
    {
        if (!head)
        {
            head = newNode(pNew, pNew->priority);
        }
        push(&head, pNew, pNew->priority);
    }
    else if (algo == 2)
    {
        push(&head, pNew, pNew->remainingTime);
        if (!head)
        {
            head = newNode(pNew, pNew->remainingTime);
        }
    }
    else
    {
        push(&head, pNew, 0);
        if (!head)
        {
            head = newNode(pNew, 0);
        }
    }
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

    printf("Ismail Enters read");
    // for (int i = 0; i < 5; i++)
    // {

    while (true)
    {
        struct Process *ttt = reader(schedulerShmId);
        if (ttt->executionTime != 0)
        {
            // do something
            // use data;
        }
    }
    // do
    // {
    //     reader(schedulerShmId);

    // } while (reader(schedulerShmId));

    // shmdt(schedulerShmId);
    printf("%d\n\n\n\n", schedulerShmId);
    shmctl(schedulerShmId, IPC_RMID, (struct shmid_ds *)0);
    printf("After%d\n\n\n\n", schedulerShmId);

    // }
    printf("Ismail exit read");

    struct Node *qHead = NULL;

    destroyClk(true);
}
