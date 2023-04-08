#include "headers.h"

int algo;

void addProcess (struct Node* head,struct Process* pNew)
{
    if(algo==1){
        if(!head)
        {
            head=newNode(pNew,pNew->priority);
        }
        push(& head,pNew,pNew->priority);
    }
    else if(algo==2){
        push(& head,pNew,pNew->remainingTime);
        if(!head)
        {
            head=newNode(pNew,pNew->remainingTime);
        }
    }
    else{
        push(& head,pNew,0);
        if(!head)
        {
            head=newNode(pNew,0);
        }
    }
    
}

int main(int argc, char * argv[])
{
    initClk();
    
    //TODO implement the scheduler :)
    //upon termination release the clock resources.

    struct Node* qHead=NULL; 
    
    
    destroyClk(true);
}
