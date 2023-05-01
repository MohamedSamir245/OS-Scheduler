#include "headers.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//=====================================================================
//==================== Tree of buddy process management ===============
//=====================================================================
//=====================================================================
//==================== Tree of buddy process management ===============
//=====================================================================

int memory[1024] = {0};
void allocateMemoryFF(struct Process *p)
{
    int st = -1;
    int j, i;
    for (i = 0; i < 1024; i++)
    {
        if (memory[i] == 0)
        {
            for (j = i; j < 1024; j++)
            {

                if (memory[j] != 0)
                    break;

                if (j == i - 1 + p->memSize)
                {
                    st = i;
                    break;
                }
            }

            if (st != -1)
                break;
            i = j;
        }
    }
    if (st != -1)
    {
        for (i = st; i < st + p->memSize; i++)
            memory[i] = 1;
        p->startLocation = st;

        printf("Memory allocation FF done\n");
    }
    else
    {
        printf("Memory allocation FF Failed\n");
    }
}

void deallocateMemoryFF(struct Process *p)
{
    for (int i = p->startLocation; i < p->startLocation + p->memSize; i++)
    {
        memory[i] = 0;
    }
    p->startLocation = -1;
}

struct memUnit
{
    int startLoc;
    int size;
    int f;

    struct memUnit *next;
    struct memUnit *previous;
};

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

void allocateMemoryBSA(struct memUnit *mem, struct Process *process)
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
}

int main()
{
    // struct treeNode *root = (struct treeNode *)createTreeNode(0, 1024, 0, NULL);

    // printf("%d\n", root->id);

    // struct treeNode *array[1024];

    // int currSize = 1;

    // leaves[0] = root;
    // printf("%d\n", leaves[0]->start);
    // printf("%d\n", array[0]->size);

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

    // printf("size: %d, pSize: %d\n", currSize, psize1);

    // allocateMemory(array, currSize, psize1, 1);
    // printf("size: %d, pSize: %d\n", currSize, psize1);

    // printf("Allocated = %d, Now size is %d", allocateMemory(currSize, psize1, 1), currSize);
    // printf("Allocated = %d, Now size is %d", allocateMemory(array, currSize, psize2, 2), currSize);
    // printf("Allocated = %d, Now size is %d", allocateMemory(array, currSize, psize3, 3), currSize);
    // printf("Allocated = %d, Now size is %d", allocateMemory(array, currSize, psize4, 4), currSize);

    return 0;
}
