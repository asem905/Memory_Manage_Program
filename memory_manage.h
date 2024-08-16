/*
Author:Assem Samy
File:memory_manage.h
comment:contains all functions declarartions used in memory_manage.c and app.c
*/

#ifndef MEMORY_MANAGE_H
#define MEMORY_MANAGE_H
/*-----------------includes section--------------*/
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
/*----------------macros declarations-----------------*/
#define HEAP_SIZE 		1024 * 1024 *1024 
#define ALLOCATED_AREA		1
#define UNALLOCATED_AREA	0
#define DEFAULT_ADDED_SIZE	128 * 1024
#define DEFAULT_SUBTRACTED_SIZE	122 * 1024
/*-------------------data types------------------*/
typedef struct blockheader{
	size_t block_size;
	char allocation_status;
	struct blockheader *next_block;
	struct blockheader *prev_block;
}blockheader_t;
/*-----------section contains functions declarations used inshell.c file------------*/
void *HmmAlloc(size_t size);
void HmmFree(void *ptr);
#endif
