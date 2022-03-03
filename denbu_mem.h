#ifndef __denbu_mem_h__
#define __denbu_mem_h__

#define E_NO_SPACE            (1)
#define E_CORRUPT_FREESPACE   (2)
#define E_PADDING_OVERWRITTEN (3)
#define E_BAD_ARGS            (4)
#define E_BAD_POINTER         (5)

extern int m_error;

int Denbu_Mem_Init(int sizeOfRegion);
void *Denbu_Mem_Alloc(int size, int style);
int Denbu_Mem_Free(void *ptr);
void Denbu_Mem_Dump();


#endif // __denbu_mem_h__
