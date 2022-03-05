#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "denbu_mem.h"




typedef struct __ourList_t{
    int size;
    struct __ourList_t *next;
} ourList_t; // our list structure

//head of list
ourList_t *head = NULL;


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
    head = memmptr;
    head->size = sizeOfRegion;
    head->next = NULL;
    
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
//Vivek's implementations below


typedef struct __header_t{
    int size;
}header_t; //header structure for each block of allocated memory


void *Denbu_Mem_Alloc(int size, int style)
{
    
    if(size%8 != 0)  //  memory to be allocated in 8 byte chunks
    {
        size = size + (8 - size%8);
    }

    header_t *hPointer = NULL; //initialize head pointer
    int newSize =0;
    void *memPointer = NULL;
    ourList_t *tmp = head;

    while(tmp)
    {
        if (tmp->size >= size + sizeof(header_t))
        {
            memPointer = tmp;
            break;
        }
        tmp= tmp->next;
    }
        

    if(memPointer != NULL)
    {
        tmp = (void *) memPointer;
        ourList_t *tmpNext = tmp->next; //place holder for next
        newSize = tmp->size - size - sizeof(header_t);
        memPointer = memPointer + tmp->size - size;
        hPointer = (void *)memPointer - sizeof(header_t);
        hPointer->size = size;
        tmp->size = newSize;
        tmp->next = tmpNext; //when you update size next changes so set it back
    }
    else if (memPointer == NULL)
    {
        m_error = E_NO_SPACE;
        return NULL;
    }
    printf("returned ptr from alloc: %p\n", memPointer);
    return (void *)memPointer;
}


void Denbu_Mem_Dump()
{
    printf("Printing free regions\n");
    ourList_t *tmp = head;
    while (tmp) {
        printf("Free Size: %d\tPointer: %p\n",tmp->size, tmp);
        tmp = tmp->next;
    }
}
