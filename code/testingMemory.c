#include "headers.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int memory[1024] = {0};
int memAlgo;
struct memUnit *mem = create_mem_unit(0, 1024, 1, NULL, NULL);

//=====================================================================
//==================== Tree of buddy process management ===============
//=====================================================================
//=====================================================================
//==================== Tree of buddy process management ===============
//=====================================================================

struct memUnit *create_mem_unit(int st, int sz, int f, struct memUnit *n, struct memUnit *pre)
{
    struct memUnit *unit = (struct memUnit *)malloc(sizeof(struct memUnit));

    unit->startLoc = st;
    unit->next = n;
    unit->f = f;
    unit->size = sz;
    unit->previous = pre;

    return unit;
}

void divideUnit(struct memUnit *u)
{
    u->size = u->size / 2;
    struct memUnit *new = create_mem_unit(u->startLoc + u->size, u->size, 1, u->next, u);
    u->next = new;
}

bool allocateMemoryBSA(struct memUnit *mem, struct Process *process)
{
    int sz = process->memSize;
    int bestIdx = -1;
    int currIdx = 0;
    int bestsz = -1;
    struct memUnit *p;

    p = mem;
    while (p)
    {
        if (sz == p->size && p->f == 1)
        {
            bestIdx = currIdx;
            break;
        }
        if (sz < p->size && p->f == 1)
        {
            if (bestsz = -1)
            {
                bestsz = p->size;
                bestIdx = currIdx;
            }
            else if (p->size < bestsz)
            {
                bestsz = p->size;
                bestIdx = currIdx;
            }
        }
        currIdx++;
        if (bestIdx == -1 && p->size >= sz && p->f == 1)
        {
            bestIdx = currIdx;
        }
        p = p->next;
    }

    if (bestIdx == -1)
    {
        if (p && p->size < sz || !p)
            printf("Can't allocate memory BSA\n");

        return;
    }

    struct memUnit *bestfit = mem;

    for (int i = 0; i < bestIdx; i++)
    {
        bestfit = bestfit->next;
    }

    while (bestfit->size / 2 >= sz)
    {
        divideUnit(bestfit);
    }

    bestfit->f = 0;

    for (int i = bestfit->startLoc; i < bestfit->startLoc + process->memSize; i++)
    {
        memory[i] = 1;
    }

    printf("memory allocated BSA\n");
    printMemoryLog(getClk(), process->startLocation, process->id, process->startLocation, process->startLocation + process->memSize - 1);
}

void deallocateMemoryBSA(struct memUnit *mem, struct Process *process)
{
    struct memUnit *tmp = mem;
    int Idx = 0;
    while (tmp != process)
    {
        tmp = tmp->next;
        Idx++;
    }
    if (Idx == 0 && mem->next == NULL)
    {
        mem.f = 1;
        process->startLocation = -1;
        return;
    }
    if (Idx % 2 == 0)
}
int main()
{

    int psize1 = 300;
    int psize2 = 200;
    int psize3 = 256;
    int psize4 = 512;

    struct Process *p1 = Process__create(1, 2, 3, 4);
    struct Process *p2 = Process__create(2, 2, 3, 4);
    struct Process *p3 = Process__create(3, 2, 3, 4);
    struct Process *p4 = Process__create(4, 2, 3, 4);

    p1->memSize = psize1;
    p2->memSize = psize2;
    p3->memSize = psize3;
    p4->memSize = psize4;

    // allocateMemoryFF(p1);
    // allocateMemoryFF(p2);
    // allocateMemoryFF(p3);
    // allocateMemoryFF(p4);

    struct memUnit *mem = create_mem_unit(0, 1024, 1, NULL, NULL);

    allocateMemoryBSA(mem, p1);
    allocateMemoryBSA(mem, p2);
    allocateMemoryBSA(mem, p3);
    allocateMemoryBSA(mem, p4);

    printf("here\n");

    return 0;
}
