#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "denbu_mem.h"

int m_error;
// number of calls made to mem_init
int callsToInit = 0;
// size of page in virtual memory
int pageSize;

int Denbu_Mem_Init(int sizeOfRegion)
{
    void *memmptr;

    // example failure cases
    if (sizeOfRegion <= 0 || callsToInit > 0) {
        m_error = E_BAD_ARGS;
        return -1;
    }
    
    // rounding up the size to page size
    pageSize = getpagesize();
    printf("Page Size: %d\n", pageSize);
    // make sure region is evenly divisible by page size
    if ( (sizeOfRegion % pageSize) != 0) {
        sizeOfRegion += (pageSize - (sizeOfRegion % pageSize));
    }
    printf("Size of Region: %d\n", sizeOfRegion);
    
    
    // call to mmep to get memory from OS 
    int fd = open("/dev/zero", O_RDWR);
    memmptr = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (memmptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    // this will not free the memory we got from OS.
    close(fd);
    
    callsToInit++;

    // TODO: you need to assume the library will manage the memory
    // allocated by mmap here (memptr)
    return 0;
}

// 
// TODO: Library should fill the rest of the functions
// Denbu_Mem_Alloc, Denbu_Mem_Free, Denbu_Mem_Dump
//