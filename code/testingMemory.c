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
