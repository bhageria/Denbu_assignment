#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "denbu_mem.h"

int main(int argc, const char *argv[]){
	printf("Testing out malloc...\n");

	int size = 65536;
    assert( Denbu_Mem_Init(size) == 0);

	exit(1); 
}
