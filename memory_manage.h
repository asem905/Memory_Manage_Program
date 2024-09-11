/*
Author:Assem Samy
File:memory_manage.h
comment:contains all functions declarartions used in memory_manage.c and app.c
*/

#ifndef MEMORY_MANAGE_H
#define MEMORY_MANAGE_H
/-----------------includes section--------------/
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <libgen.h> // Include for basename()
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <aio.h>
#include <error.h>
/----------------macros declarations-----------------/
#define HEAP_SIZE 				1024 * 1024 * 1024 
#define ALLOCATED_AREA				1
#define UNALLOCATED_AREA			0
#define DEFAULT_ADDED_SIZE			128 * 1024 
#define DEFAULT_SUBTRACTED_SIZE			12 * 1024 
#define MAX_ALLOC_SIZE 				(10*DEFAULT_ADDED_SIZE / 10)
#define START_SIZE				1024


#define FREEING_ALLOCATING_USING_MY_FREE_MALLOC 1
#define FREEING_ALLOCATINF_USING_SBRK		0
#define METHOD_OF_FREEING_ALLOCATING		FREEING_ALLOCATING_USING_SBRK

#define ALIGN_SIZE 8
#define align_size(size) (((size) + (ALIGN_SIZE - 1)) & ~(ALIGN_SIZE - 1)) //just to allign to 8 bytes size to ease access of it in memry

/-------------------data types------------------/
typedef struct blockheader{
	size_t block_size;
	char allocation_status;
	struct blockheader *next_block;
	struct blockheader *prev_block;
}blockheader_t;

/-----------section contains functions declarations used inshell.c file------------/

void *realloc(void *ptr,size_t size);
void* calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr) ;
#endif
