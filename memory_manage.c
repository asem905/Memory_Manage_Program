/*
Author:Assem Samy
File:memory_manage.c
comment:contains all functions definitions
*/
/*-----------------includes section--------------*/
#include "memory_manage.h"
/*-----------------variables section-------------*/
static char heap[HEAP_SIZE];  // Simulated heap area
static char *simulated_program_break=heap;
static blockheader_t *list = NULL;

/*-----------section contains static functions definitions------------*/
static void initialize_heap() {
    list = (blockheader_t *)heap;
    list->block_size = HEAP_SIZE ;
    list->allocation_status = UNALLOCATED_AREA; // Free
    list->next_block = NULL;
    list->prev_block = NULL;
    simulated_program_break = heap ;
}
/*-----------section contains functions definitions------------*/

void *HmmAlloc(size_t size) {

    blockheader_t *prev = NULL;

    size += sizeof(blockheader_t); // Include space for the header

    if (simulated_program_break == heap) {
        initialize_heap();
    }
    blockheader_t *current = list;//we take from it address of first made block then loop
    printf("simulated_program_break address is %p before allocating\n",simulated_program_break);
    printf("size %d\n",size);
    while (current) {
        if (current->allocation_status == UNALLOCATED_AREA && current->block_size >= size) {
            // Calculate the pointer to the next block after allocation
            blockheader_t *next_block_node = (blockheader_t *)((char *)current + size);

            // Split the block if there is remaining space
            if (current->block_size > size) {
                next_block_node->block_size = current->block_size - size;
                next_block_node->next_block = current->next_block;
                next_block_node->allocation_status = UNALLOCATED_AREA;
		next_block_node->prev_block=current;
                // Update current block to the allocated block
                current->block_size = size;
                current->allocation_status = ALLOCATED_AREA;
                current->next_block = next_block_node;
            } else {
                //as size is same size of block so no need to split
                current->allocation_status = ALLOCATED_AREA;
            }

            // Update simulated program break if needed
            if (((char *)current + size) > simulated_program_break) {
                simulated_program_break = (char *)current + size+DEFAULT_ADDED_SIZE;
            }
	    printf("simulated_program_break address is %p after allocating\n",simulated_program_break);
            return (char *)current + sizeof(blockheader_t);
        }
        prev = current;
        current = current->next_block;
        current->prev_block=prev;
    }

    // No suitable block found
    return NULL;
}
void HmmFree(void *ptr){
	if (ptr == NULL) {
        	printf("Error: Attempt to free a NULL pointer\n");
        	return;
    	}
	char found=0;
	blockheader_t *block_to_free=(blockheader_t *)((char*)ptr-sizeof(blockheader_t));
	blockheader_t *current=list;//from start
	blockheader_t *prev=NULL;
	blockheader_t *next=list;
	printf("simulated program break is %p before any free\n", simulated_program_break);
	while(current){

		if(current == block_to_free && current->allocation_status==ALLOCATED_AREA){

			found=1;
			size_t size=current->block_size;
			printf("size of block to delete is %zu\n", current->block_size);
			current->allocation_status=UNALLOCATED_AREA;
			if (prev && prev->allocation_status == UNALLOCATED_AREA) {
                		prev->block_size += current->block_size;
                		prev->next_block = current->next_block;
                		if (current->next_block) {
                    			current->next_block->prev_block = prev;
                		}
                		current = prev;
            }
            if (current->next_block && current->next_block->allocation_status == UNALLOCATED_AREA) {
                    current->block_size += current->next_block->block_size;
                    current->next_block = current->next_block->next_block;
                    if (current->next_block) {
                            current->next_block->prev_block = current;
                    }
            }

			if ((char *)current + current->block_size >= simulated_program_break) {
		        	simulated_program_break = simulated_program_break-DEFAULT_SUBTRACTED_SIZE-size;
		        	printf("New simulated program break is %p as block was at end so decrementing by %d\n", simulated_program_break,DEFAULT_SUBTRACTED_SIZE-size);
            } else {
		        	printf("Block freed keeping simulated program break same address %p as freed in between blocks not at end\n", simulated_program_break);
            }

            printf("Block freed\n");
            return;


		}
		printf("searching\n");
		prev=current;
		current = current->next_block;
		current->prev_block=prev;

	}
	if(found==0){
		printf("not found\n");
	}

}



















