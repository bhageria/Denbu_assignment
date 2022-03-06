#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "denbu_mem.h"

// styles for free space search
#define BESTFIT               (1)
#define WORSTFIT              (2)
#define FIRSTFIT              (3)

int main(int argc, const char *argv[])
{
    assert(Denbu_Mem_Init(65536) == 0);
    void* ptr[4];
    
    Denbu_Mem_Dump(); 
    ptr[0] = Denbu_Mem_Alloc(8, FIRSTFIT);
    printf("Allocated memory pointer = %p\n",ptr[0]);
    printf("8 (+8) should have been allocated\n\n");
    Denbu_Mem_Dump(); 
    ptr[1] = Denbu_Mem_Alloc(16, FIRSTFIT);
    printf("16 (+8) should have been allocated\n\n");
    

    Denbu_Mem_Dump(); 
    Denbu_Mem_Free(ptr[0]);
    printf("8(+8) should have been freed\n\n");
    Denbu_Mem_Dump(); 
    //assert(Denbu_Mem_Free(ptr[0]) == 0);
    Denbu_Mem_Free(ptr[1]);
    printf("16 (+8) should have been freed\n");
    printf("all memory should be free\n\n");
    Denbu_Mem_Dump(); 
 
    ptr[2] = Denbu_Mem_Alloc(32, FIRSTFIT);
    printf("32 (+8) should have been allocated\n\n");
    Denbu_Mem_Dump(); 
    ptr[3] = Denbu_Mem_Alloc(8, FIRSTFIT);
    printf("8(+8) should have been allocated\n\n");
    Denbu_Mem_Dump(); 
    Denbu_Mem_Free(ptr[2]);
    printf("32 (+8)should have been freed\n\n");
    Denbu_Mem_Dump(); 
    assert(Denbu_Mem_Free(ptr[2]) == 0);
    Denbu_Mem_Free(ptr[3]);
    printf("8  (+8) should have been freed\n\n");
    Denbu_Mem_Dump(); 
    assert(Denbu_Mem_Free(ptr[3]) == 0);
}
