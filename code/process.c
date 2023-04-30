#include "headers.h"

/* Modify this file as needed*/

int prevclk = -1;
bool start_process()
{
    int i = fork();
}

int main(int agrc, char *argv[])
{
    initClk();

    int shmid;
    key_t key_id;
    key_id = ftok("keyfile", 65);
    shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    if (shmid == -1)
    {
        perror("Process.c: Error in create");
        exit(-1);
    }
    // else
    // printf("\nShared memory ID = %d\n", shmid);

    int *shmaddr = (int *)shmat(shmid, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Process.c: Error in attach in reader");
        exit(-1);
    }
    // printf("\nReader: Shared memory attached at address %x\n", shmaddr);

    // TODO it needs to get the remaining time from somewhere
    // int *remainingtime = (int*)shmaddr;
    while (*shmaddr > 0)
    {
        if (prevclk != getClk())
        {
            (*shmaddr)--;
            prevclk = getClk();
        }
    }

    shmdt(shmaddr);
    destroyClk(false);

    return 0;
}
