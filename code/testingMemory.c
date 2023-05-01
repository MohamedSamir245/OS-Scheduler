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
struct treeNode
{
    // id -1 for empty segment
    // position 0 => root  1 => right  -1 => left
    int id, start, size, position;
    struct treeNode *right;
    struct treeNode *left;
    struct treeNode *parent;
};

struct treeNode *createTreeNode(int start, int size, int position, struct treeNode *parent)
{
    struct treeNode *tempNode;
    tempNode = (struct treeNode *)malloc(sizeof(struct treeNode));
    tempNode->start = start;
    tempNode->size = size;
    tempNode->position = position;
    tempNode->parent = parent;
    tempNode->left = NULL;
    tempNode->right = NULL;
    return tempNode;
}

struct treeNode *mergeAfterDeleting(struct treeNode *deleted)
{
    struct treeNode *nodeParent;
    while (deleted->parent && deleted->id == -1)
    {
        nodeParent = deleted->parent;
        if ((deleted->position == 1 && nodeParent->left->id == -1) ||
            (deleted->position == -1 && nodeParent->right->id == -1)) // this is a right child
        {
            nodeParent->right = NULL;
            nodeParent->left = NULL;
            deleted = nodeParent;
        }
    }
    return deleted;
}

int allocateMemory(struct treeNode *leaves[], int size, int pSize, int PID)
{
    // Get Best Fit
    printf("inside allocateMemory");

    int bestIdx = -1;

    for (int i = 0; i < size; i++)
    {

        if (pSize == leaves[i]->size)
        {
            bestIdx = i;
            break;
        }
        else if (pSize < leaves[i]->size)
        {
            if (bestIdx == -1)
                bestIdx = i;
            else if (leaves[i]->size < leaves[bestIdx]->size)
                bestIdx = i;
        }
    }

    if (bestIdx == -1)
    {
        // No Empty Space
        // Think about what you will do
        return -1;
    }
    while (pSize <= leaves[bestIdx]->size / 2)
    {
        struct treeNode *l = createTreeNode(leaves[bestIdx]->start, leaves[bestIdx]->size / 2, -1, leaves[bestIdx]);
        struct treeNode *r = createTreeNode(leaves[bestIdx]->start + leaves[bestIdx]->size / 2, 1, leaves[bestIdx]->size / 2, leaves[bestIdx]);
        l->id = -1;
        r->id = -1;
        l->position = -1;
        r->position = 1;
        leaves[bestIdx]->left = l;
        leaves[bestIdx]->right = r;

        // add them to leaves array;
        //  bestIdx=array.size()-2
        // Think about how we will implement the array (you can declare with max size which is 1024 and use variable to indicate the actual size)
    }
    leaves[bestIdx]->id = PID;
    size++;

    return bestIdx;
}

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

int main()
{
    // struct treeNode *root = (struct treeNode *)createTreeNode(0, 1024, 0, NULL);

    // printf("%d\n", root->id);

    // struct treeNode *array[1024];

    // int currSize = 1;

    // array[0] = root;
    // printf("%d\n", array[0]->start);
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

    allocateMemoryFF(p1);
    allocateMemoryFF(p2);
    allocateMemoryFF(p3);
    allocateMemoryFF(p4);

    // printf("here\n");

    // printf("size: %d, pSize: %d\n", currSize, psize1);

    // allocateMemory(array, currSize, psize1, 1);
    // printf("size: %d, pSize: %d\n", currSize, psize1);

    // printf("Allocated = %d, Now size is %d", allocateMemory(array, currSize, psize1, 1), currSize);
    // printf("Allocated = %d, Now size is %d", allocateMemory(array, currSize, psize2, 2), currSize);
    // printf("Allocated = %d, Now size is %d", allocateMemory(array, currSize, psize3, 3), currSize);
    // printf("Allocated = %d, Now size is %d", allocateMemory(array, currSize, psize4, 4), currSize);

    return 0;
}
