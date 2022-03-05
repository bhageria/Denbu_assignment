#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "denbu_mem.h"

int main(int argc, const char *argv[])
{
    int size = 65536;
    int *ptr = NULL;
    assert( Denbu_Mem_Init(size) == 0);
    printf("Alloc memory = %p\n\n",Denbu_Mem_Alloc(4096,3));
    printf("Alloc memory = %p\n\n",Denbu_Mem_Alloc(4096,3));
    ptr = Denbu_Mem_Alloc(4096,1);
    printf("Alloc memory = %p\n\n",Denbu_Mem_Alloc(4090,1));
    
    
    Denbu_Mem_Free(ptr);
    Denbu_Mem_Dump();

	exit(1); 
}
