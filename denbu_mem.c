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
    struct __ourList_t *prev;
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


/*****************************BESTFIT*****************************/
ourList_t* find_bestfit(int sizeToAlloc)
{

    ourList_t* best_node;
    ourList_t * temp;
    temp=freeList;
    while(temp!=NULL)
    {
        if (((temp->size) >= sizeToAlloc) && ((best_node == NULL) || ((temp->size) < (best_node->size))))
        {
	    best_node = temp;
	}

	if((best_node!=NULL )&& ( (best_node->size) == sizeToAlloc))
	{
	    break;
	}
	temp = temp->next;
    }
    if(best_node == NULL)
    {
        m_error = E_NO_SPACE;
	printf("NOT enough space\n");

    }

    return  best_node;
}
/***************************BESTFIT END****************************/

/*****************************FIRSTFIT*****************************/
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

/***************************FIRSTFIT END***************************/

/*****************************WORSTFIT*****************************/
ourList_t* find_worstfit(int sizeToAlloc)
{
    ourList_t* temp;
    temp = freeList;
    ourList_t* worst_node = NULL;

    while(temp != NULL)
    {
        if (temp->size >= sizeToAlloc) 
        {
            if ((worst_node==NULL) || ( (worst_node!=NULL) && ((temp->size) > (worst_node->size) )))
            {
                worst_node = temp;

            }
        }

        temp = temp->next;
    }
    if(worst_node == NULL)
    {
        m_error = E_NO_SPACE;
        printf("NOT enough space\n");

    }

	return worst_node;
}

/****************************WORSTFIT END**************************/


void Denbu_Mem_Coalesce()
{


	ourList_t* temp;
	temp = freeList;

	if(temp != NULL)
	{
		while( temp->next != NULL)
		{
			temp = temp->next;

			if(((char*)(temp->prev) + ((temp->prev)->size)) == (char*)temp)
			{
				(temp->prev)->size += temp->size;

				//merge node to the tail node
				if(temp-> next == NULL)
				{
					tail = temp->prev;
					(temp->prev)->next = NULL;

					break;
				
				}
				else
				{
					//general case
					(temp->next)->prev = temp->prev;
					(temp->prev)->next = temp->next;

					temp = temp->prev;
				}
			}//end if

		}//end while
	}//end if
}//end coalesce


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
    
    /***************section added by Vivek****************/
    
    
    //initialize free list
    freeList = tail = (ourList_t*) memmptr;
    freeList->next = NULL;
    freeList->prev = NULL;
    freeList->size = sizeOfRegion;	
	
    //print init data
    printf("Head of initialized memory space = %p:\n",memmptr);
    printf("freeList->size is %d \n",freeList->size);
    printf("sizeof ourList_t ->size is %zu \n",sizeof(ourList_t));
    printf("sizeof header_t ->size is %zu \n\n",sizeof(header_t));
    printf("End of Mem_Init:\n\n");
    
    /***********section added by Vivek ends here***********/
	
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
	
    ourList_t* newfree;
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
	    temp = find_bestfit(adjusted_size);
	    break;

	case WORSTFIT:
	    temp = find_worstfit(adjusted_size);
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
    //check if there is a free list but if the size requested leaves
    //less that sizeof(header_t), do not allocate to avoid a seg fault
    if(temp == freeList && (freeList->next == NULL))
    {
        if (temp->size - adjusted_size < sizeof(ourList_t))
        {
            m_error=E_NO_SPACE;
            printf("NOT enough space\n");
            temp=NULL;
        }
    }

    //valid free node found
    if(temp != NULL)
    {
        ////////////////only one node in free list////////////////////////
        if(temp == freeList && (freeList->next == NULL))
        {
            //all of memory requested
            if( adjusted_size == freeList->size)
            {
                freeList = tail = NULL;
            }
            //only portion requested
            else
            {
                freeList = (ourList_t*)((char*) temp + adjusted_size);
                freeList->next=NULL;
                freeList->prev=NULL;
                tail = freeList;
                freeList->size = temp->size - adjusted_size;
            }

        }
        ////////////////more than one node in free list///////////////////////
        else
        {
            //size requested is the same size as free node-
            //in an a free list with more than 1
            if(temp->size == adjusted_size)
            {
                //temp is first node in free list
                if(temp == freeList && (freeList->next != NULL))
                {
                    freeList = temp->next;
                }
                //temp some middle node in the list
                else if(temp != tail)
                {
                    (temp->prev)->next = temp->next;
                    (temp->next)->prev = temp->prev;
                }
                //temp is the tail
                else
                {
                    (temp->prev)->next = NULL;
                    tail = temp->prev;
                }
                //sizeToAlloc is only a portion of a node in a list
                //more than one node
            }
            else
            {
                newfree =(ourList_t*) ((char*)temp + adjusted_size);
                newfree->size = temp->size - adjusted_size;
                //temp is first node in free list w/ more than one node
                if(temp == freeList && (freeList->next != NULL) )
                {
                    newfree->next = temp->next;
                    freeList = newfree;
                }
                //temp some middle node in the list w/ more than one
                else if(temp != tail)
                {
                    newfree->prev = temp->prev;
                    newfree->next = temp->next;
                    (temp->prev)->next=newfree;
                }
                //temp is the tail
                else
                {
                    //local variables
                    newfree->prev = temp->prev;
                    newfree->next = NULL;
                    (temp->prev)->next=newfree;
                    tail = newfree;
                }
            }
        }//endelse for (more than more node in list)

        //move temp ptr past the header (8bytes)
        //so it points to the beginning of the allocated
        //space
        temp = (ourList_t*)((char*)temp + sizeof(header_t));
        //initialise header pointer address
        headptr = (header_t*)((char*)temp - sizeof(header_t));
        printf("Mem_Alloc header address assigned is : %llx\n", (long long unsigned) headptr);
        //headerpointer: fill this space with the header
        *headptr = fillhead;
        //cast to void and return pointer to allocated space
        return (void *) temp;
    }
    else
    {
        return NULL;
    }
}


int Denbu_Mem_Free(void *ptr)
{
    printf("Entered Mem_Free()\n");
	
    // local variables
    ourList_t* newptr;
    ourList_t* temp ;

    //move to top of header
    header_t *hptr =(header_t*) ( (char*)ptr - sizeof(header_t));

    printf("hptr address in Free() is : %llx\n", (long long unsigned)hptr);

    printf(" Integrity num is %d\n",  hptr->integrity);
    printf("size of node to free is : %d\n", hptr->size);
    if( hptr->integrity == INTEGRITY)
    {
        //save size of header before changing type (data will be lost)
        int size_free;
        size_free=(hptr->size);
		
        //inititalize node to be added to free list
        newptr = (ourList_t*) hptr;
        newptr->size=size_free;
		
        //save address variable for comparison purposese
        char*addrToFree = (char*) newptr;

        //free list is empty
        if(freeList == NULL)
        {
            freeList = newptr;
        }
        //non empty free list
        else
        {
            temp = freeList;

            //addr before start of free list
            if(addrToFree < (char*)temp)
            {
                newptr->prev = NULL;
                temp->prev = (ourList_t*)newptr;
                newptr->next = temp;
                freeList = newptr;

            }
            //addr after the end of free list
            else if (addrToFree > (char*)tail)
            {
                newptr->next = NULL;
                newptr->prev = tail;
                tail->next = newptr;
                tail = newptr;
            }
            //search free list to put node
            //in sequential order
            else
            {
                while(temp != NULL)
                {
                    if( (char*)temp > addrToFree)
                    {
                        (temp->prev)->next = newptr;
                        newptr->prev = temp->prev;
                        newptr->next = temp;
                        temp->prev = newptr;
                        break;
                    }
                    temp = temp->next;
                }//end while
            }//end else
        }//end else of more than one node
        printf("Freed the Pointer Successfully\n\n");
        //Colesce Memory
        Denbu_Mem_Coalesce();
        return 0;
    }//end if magic number was valid
    else
    {
        m_error = E_BAD_POINTER;
        printf("m_error is %d, E_BAD_POINTER \n",m_error);
        return -1;
    }

}


void Denbu_Mem_Dump()
{
    printf("\n\nFREE LIST:------------------------------------------\n");
	ourList_t *tmp = freeList;
	int count=0;


	do{

		printf("Addr of block is %llx\n", (long long unsigned) tmp);	
		printf("Free Size block[%d]: %d\n", count,tmp->size);
		tmp = tmp->next;
		count++;

	}while (tmp != NULL);

	printf("--------------------------------------------------\n\n");
}
