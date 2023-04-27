#include "headers.h"

/* Modify this file as needed*/

int prevclk=-1;
bool start_process()
{
    int i = fork();
}

int main(int agrc, char *argv[])
{
    initClk();

    int shmid, pid;
    key_t key_id;
    key_id = ftok("keyfile", 65);
    shmid = shmget(key_id, 4096, IPC_CREAT | 0644);

    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }

    // TODO it needs to get the remaining time from somewhere
    int *remainingtime = (int*)shmaddr;
    while (*remainingtime > 0)
    {
        if(prevclk!=getClk()){
            *remainingtime--;
        }
        prevclk=getClk();
    }
    shmdt(shmaddr);
    destroyClk(false);

    return 0;
}
