#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "denbu_mem.h"

// styles for free space search
#define BESTFIT               (1)
#define WORSTFIT              (2)
#define FIRSTFIT              (3)


#define INTEGRITY 5050

typedef struct __header_t{
    int size;
    int integrity;
}header_t; //header structure for each block of allocated memory


typedef struct __ourList_t{
    int size;
    struct __ourList_t *next;
    struct __OURlist_t *prev;
} ourList_t; // our list structure

//head of list
ourList_t *head = NULL;
ourList_t *freeList = NULL;
ourList_t *tail = NULL;


int m_error;
// number of calls made to mem_init
int callsToInit = 0;
// size of page in virtual memory
int pageSize;
int availableSize;
int totalSize;

/*************************FIRSTFIT*****************************/

ourList_t* find_firstfit(int sizeToAlloc)
{
    ourList_t* ff=NULL;
    ourList_t *temp;
    temp = freeList;

    do
    {
        if( (temp->size) >= sizeToAlloc)
        {
	    ff=temp;
	    break;
	}
	temp = temp->next;

    }while(temp != NULL);

    if(ff == NULL)
    {
        m_error = E_NO_SPACE ;
	printf("NOT enough space\n");
    }
    if (ff!=NULL)
	totalSize =-sizeToAlloc;
    return ff;
}

/*************************FIRSTFIT END*****************************/

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
    
    //initialize free list
    freeList = tail = (ourList_t*) memmptr;
    freeList->next = NULL;
    freeList->prev = NULL;
    freeList->size = sizeOfRegion;

    //	freep->size = sizeOfRegion - sizeof(list_t);
    // test    total_size=sizeOfRegion;
	
	
    //print init data
    printf("Head of initialized memory space = %p:\n",memmptr);
    printf("freep->size is %d \n",freeList->size);
    printf("sizeof list_t ->size is %zu \n",sizeof(ourList_t));
    printf("sizeof header_t ->size is %zu \n\n",sizeof(header_t));
    printf("End of Mem_Init:\n\n");
    
	
	
    callsToInit++;

    // TODO: you need to assume the library will manage the memory
    // allocated by mmap here (memptr)
    return 0;
}

// 
// TODO: Library should fill the rest of the functions
// Denbu_Mem_Alloc, Denbu_Mem_Free, Denbu_Mem_Dump
//Vivek's implementations below





void *Denbu_Mem_Alloc(int size, int style)
{
    if(size<=0)
    {
        m_error = E_BAD_ARGS;
	printf("Bad size args\n");
	return NULL;
    }
    
    if(size%8 != 0)  //  memory to be allocated in 8 byte chunks
    {
        size = size + (8 - size%8);
    }

    //add header size to 8 bit aligned size, header is 
    //also 8 bit aligned
    int adjusted_size;
    adjusted_size = size + sizeof(header_t);
	
    //error check if there is no free list 
    if(freeList->next == NULL &&  (adjusted_size > (freeList->size - sizeof(ourList_t))  ))
    {
        m_error = E_NO_SPACE;
	return NULL;
    }
	
    ourList_t * newfree;
    //will return this as a ptr to the allocated spac
    ourList_t *temp = NULL;
    //declare pointer for the header space
    header_t *headptr;


    //allocate new header
    header_t fillhead;
    fillhead.size = adjusted_size;
    fillhead.integrity = INTEGRITY;
	
	
    switch (style)
    {
	case BESTFIT:
	    //temp = find_bestfit(adjusted_size);
	    break;

	case WORSTFIT:
	    //temp = find_worstfit(adjusted_size);
	    break;
	    
	case FIRSTFIT:
	    temp = find_firstfit(adjusted_size);
	    break;
	    
	default:
	    m_error = E_BAD_ARGS; 
	    printf("bad style args\n");
	    return NULL;           
	    break;
    }
    return temp;
}


int Denbu_Mem_Free(void *ptr)
{
    printf("inside Denbu_Mem_Free()\n");
    
    // CHECK FOR NULL POINTER
    if (ptr == NULL) {
        m_error = E_BAD_POINTER;
        return -1;
    }
    
    header_t *hptr = (void *) ptr - sizeof(header_t);
    
    if (hptr->integrity != INTEGRITY) {
        m_error = E_BAD_POINTER;
        return -1;
    }
    
    // FIND SIZE OF FREE REGION
    int freeSize = (hptr->size) + sizeof(header_t);
    printf("Freed Region Size: %d\n", freeSize);
    
    
    // ADD FREED NODE TO FREE LIST
    ourList_t *tmp = head; // keep ref to head
    head = (void *)hptr; // make head point to new freed chunk
    head->next = tmp; // make new head point to old head
    head->size = freeSize;
    
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
