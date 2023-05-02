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
