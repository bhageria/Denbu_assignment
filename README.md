# Denbu-SW-Engineering
Take home interview question for Denbu SW Enginee. Please Complete the question and share your Github repo to the Denbu email id. 

Objective:
Implement a Denbu memory allocator library to be used in an embedded system. 

Requirements: 
For this question, you will be implementing several different routines as part of a shared library. Note that you will not be writing a main() routine for the code that you handin (but you should implement one for your own testing). We have provided the prototypes for these functions in the file mem.h. you should include this header file in your code to ensure that you are adhering to the specification exactly. You should not change mem.h in any way! We now define each of these routines more precisely.

   int Denbu_Mem_Init(int sizeOfRegion): Denbu_Mem_Init is called one time by a process using your routines. sizeOfRegion is the number of bytes that will be requested from OS using mmap().
    For simplicity, i have prefilled the Denbu_Mem_Init to round up the size to pageSize granularity. You are not allowed to malloc(), or any other related function, in any of your routines! Similarly, you should not allocate global arrays. However, you may allocate a few global variables (e.g., a pointer to the head of your free list.)

Cases where Denbu_Mem_Init should return a failure: Denbu_Mem_Init is called more than once; sizeOfRegion is less than or equal to 0.

   void *Denbu_Mem_Alloc(int size, int style): Denbu_Mem_Alloc() is similar to the library function malloc(). Denbu_Mem_Alloc takes as input the size in bytes of the object to be allocated and returns a pointer to the start of that object. The function returns NULL if there is not enough contiguous free space within sizeOfRegion allocated by Denbu_Mem_Init to satisfy this request (and sets m_error to E_NO_SPACE).

   The style parameter determines how to look through the list for a free space. It can be set to BESTFIT (BF) for the best-fit policy, WORSTFIT (WF) for worst-fit, and FIRSTFIT (FF) for first-fit. BF simply looks through your free list and finds the first free space that is smallest in size (but still can hold the requested amount) and returns the requested size (the first part of the chunk) to the user, keeping the rest of the chunk in its free list; WF looks for the largest chunk and allocates the requested space out of that; FF looks for the first chunk that fits and returns the requested space out of that.

   For performance reasons, Denbu_Mem_Alloc() should return 8-byte aligned chunks of memory. For example if a user allocates 1 byte of memory, your Denbu_Mem_Alloc() implementation should return 8 bytes of memory so that the next free block will be 8-byte alligned too.

   int Denbu_Mem_Free(void *ptr): Denbu_Mem_Free() frees the memory object that ptr points to. Just like with the standard free(), if ptr is NULL, then no operation is performed. The function returns 0 on success, and -1 otherwise.

   Coalescing: Denbu_Mem_Free() should make sure to coalesce free space. Coalescing rejoins neighboring freed blocks into one bigger free chunk, thus ensuring that big chunks remain free for subsequent calls to Denbu_Mem_Alloc().

   void Denbu_Mem_Dump(): This is just a debugging routine for your own use. Have it print the regions of free memory to the screen.

You must provide these routines in a shared library named "libdenbu_mem.so". All of your library functions should be in denbu_mem.c. I have attached a Makefile in the directory, which compiles the library to "libdenbu_mem.so". You can use the main.c as the test code of the shared library to make sure the library works. 

Before you run "test", you will need to set the environment variable, LD_LIBRARY_PATH, so that the system can find your library at run-time. Assuming you always run test from this same directory, you can use the command:

export LD_LIBRARY_PATH=\`pwd\`:$LD_LIBRARY_PATH

Coding Envvironment: 
1. You can gitpod as your coding environment. if you want and then share the gitpod workspace to Denbu Email id (https://gitpod.io/#https://github.com/dinesh-rt/Denbu-SW-Engineering)
2. You can fork this github repo and do your development in your own machine and submit the working code with the github repository 

Steps to compile program:
1. make all -- build the library and compile main.c test file to create a binary called "test". You can run like ./test
2. make clean - delete the object files and shared library 

Verification:
1. We have our own tests to validate the library you are submitting. 
2. Test cases that will be validated 
       a. Allocation and free of memory using the library. Varying allocation sizes and order will be tested 
       b. All the allocation algorithms will be tested to see if bestfit, worstfit and firstfit are honored 
       c. More points will be awarded if the amount of memory wasted for each allocaton is lesser. 
