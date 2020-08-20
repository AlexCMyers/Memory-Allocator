#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

int fd;
const int BIG = 1024;
const int PAGESIZE = 4096;
void* blocks[11] = {NULL};


__attribute__((constructor)) void start(void);
__attribute__((destructor)) void end(void);
void* malloc(size_t size); 
void free(void *ptr);
void *realloc(void* ptr, size_t newSize);
void *calloc(size_t n, size_t size);

//void* whichMem(int num);

void* malloc(size_t size){
	int i = 0;
	int index = 1;
	void* currMem = NULL;
	int32_t* blockSize = NULL;
	int16_t* numPages = NULL, *offset = NULL, *ptr = NULL;
	void* currBlock = NULL;
	void* tempv = NULL;

	if(size < 1){return NULL;}
	
	//setting up right index for blocks array
	if(size > BIG){
		tempv = mmap(NULL, size + 4, PROT_READ|PROT_WRITE,
								MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		//putting size of memory at beginning of first page
		blockSize = (int32_t*)tempv;
		*blockSize = size + 4;
		//returning memory beginning with memory after where size is located
		currMem = (void*)(((int64_t)tempv)|4);
	}else{
		for(i = 2; i < size; i*=2){
			index++;
		}
		currBlock = blocks[index];
		
		//if havent assigned memory of that chunk size yet 
		if(!blocks[index]){
			//request new page and set up metadata
			blocks[index] = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, 
											MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
			currBlock = blocks[index];
			//putting the size of chunks in this page at beginning of first page
			blockSize = (int32_t*)currBlock;
			*blockSize = (int32_t)pow(2.0, (double)index);
			//putting number of pages related to this size next
			offset = (int16_t*)(blockSize + 1);
			*offset = 0x008;
		
			//used first 6 bytes.Next 2 are for the number of pages of that
			//size. used to release the pages at the end of process
			numPages = offset + 1;
			*numPages = 1;
		
			currMem =  (void*)(((int64_t)currBlock)|*offset);
			//putting memory after new first block to hold next free blocks offset
			ptr = (int16_t*)(((int64_t)currBlock)|
													(*offset + *blockSize));
			*ptr = *offset + *blockSize + 2;
			*offset = *ptr;
		}
			//if blocks has started for that index
		else{
			blockSize = (int32_t*)(currBlock);
			numPages = (int16_t*)((int64_t)currBlock|0x6);
			offset = (int16_t*)((int64_t)currBlock|0x4);
			//finding right page that deals with memory chunks of size
			while(*offset == 0xFF8){
				//need the address stored as a number at the address of 
				//currBlock & FF8
				tempv = (void*)(*((int64_t*)(((int64_t)currBlock) | 0xFF8)));
				//if that page has been allocated already then it has an offset
				if(tempv){
					currBlock = tempv;
					offset = (int16_t*)((int64_t)currBlock|0x4);
				//next page not allocated yet
				}else{
					//getting another page
					tempv = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, 
													MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
					*((int64_t*)(((int64_t)currBlock)|0xFF8)) = (int64_t)tempv;
					currBlock = tempv;
					*((int32_t*)currBlock) = *blockSize;
	
					(*numPages)++;
					offset = (int16_t*)((int64_t)currBlock|0x4);
					*offset = 0x6;
				}
			}
			//current memory (to be returned)
			currMem = (void*)((int64_t)currBlock|*offset);	
			//the 2 bytes after currMem
			ptr = (int16_t*)((int64_t)currBlock|(*blockSize + *offset));
			//if this is the highest current physical address in this page
			if(*ptr == 0){
				//then it just points to the next highest block
				*ptr = *offset + *blockSize + 2;
			}
			//otherwise it is a previously freed memory so it points to the 
			//next freed memory
			*offset = *ptr;

			
		}
		//if offset is close enough to the ending FF8, need to make it FF8
		if((*offset + *blockSize + 2) >= 0xFF8){
			*offset = 0xFF8;
			*ptr = 0xFF8;
		}
	}
	return currMem;
}

void free(void* ptr){
	int16_t whichBlock = 0;
	int32_t* size = NULL;
	void* base = NULL;
	char* helper = NULL;
	int16_t* next = NULL;
	int16_t* currOff = NULL;

	
	if(ptr == NULL){return;}
	

	base = (void*)(((int64_t)ptr) & 0xFFFFFFFFFFFFF000);
	size = (int32_t*)base;
	if(*size > 1024){
		munmap(base, *size);
	}
	else{
		//set memory to 0 for future calloc requests
		memset(ptr, 0, *size);
		//the offset of the ptr
		whichBlock = (int16_t)((int64_t)ptr & 0xFFF);
		//makes my life a little easier
		helper = (char*)base;	
		//setting to location where offset is stored in the pages
		currOff = (int16_t*)(helper + 4);
		//setting to location after the block being freed
		next = (int16_t*)(helper + whichBlock + *size);
		//putting ptr onto top of freed linked list
		*next = *currOff;
		*currOff = whichBlock;
	}

}
	
void *realloc(void* ptr, size_t newSize){
	if(ptr){
		//oldsize is stored in first 32 bits of the page
		int32_t* oldSize = (int32_t*)(((int64_t)ptr) & 0xFFFFFFFFFFFFF000);
		//need a new pointer in appropriate memory size
		void* retPtr = malloc(newSize);
//		printf("oldsize: %d, newsize: %ld\n", *oldSize, newSize);
		//unsigned char* oldChar = ptr;
		//unsigned char* newChar = retPtr;
		//copying data from ptr to retPtr
		memcpy(retPtr, ptr, (*oldSize < newSize) ? *oldSize:newSize);
		free(ptr);
		return retPtr;
	}
	else{
		return malloc(newSize);
	}
}
//all memory starts as 0 and free sets it to 0
void *calloc(size_t n, size_t size){
	return(malloc(n * size));
}


__attribute((constructor))
void start(){
}


__attribute__((destructor)) void end(void){			
	int i = 0;
	int16_t* numPages;
	int16_t num = 0;
	int j = 0;
	void* currPg;
	void* nxtPg;
	//for each standard size
	for(i = 0; i < 11; i++){
		if(blocks[i]){
			numPages = (int16_t*)(((int64_t)blocks[i])|0x06);
			num = *numPages;
			nxtPg = blocks[i];
			currPg = blocks[i];
			for(j = 1; j < num; j++){
				nxtPg = (void*)(((int64_t)currPg)|0xFF8);
				munmap(currPg, PAGESIZE);
				currPg = nxtPg;
			}
		}
	}
}	







