/*
Author:Assem Samy
File:memory_manage.c
comment:contains all functions definitions
*/
/-----------------includes section--------------/
#include "memory_manage.h"
/-----------------variables section-------------/
static char heap[HEAP_SIZE];  // Simulated heap area
static char *simulated_program_break=heap;
static blockheader_t *list = NULL;
static blockheader_t *real_list = NULL;
static char *real_program_break=NULL;
void *heap_start = NULL;  // Start of the heap
void *heap_end = NULL;    // End of the heap
/-----------section contains static functions definitions------------/
static void initialize_heap() {
    list = (blockheader_t *)heap;
    list->block_size = HEAP_SIZE ;
    list->allocation_status = UNALLOCATED_AREA; // Free
    list->next_block = NULL;
    list->prev_block = NULL;
    simulated_program_break = heap+START_SIZE ;
}
static void *HmmAlloc(size_t size) {
#if METHOD_OF_ALLOCATION==ALLOCATE_USING_MY_MALL0C
    

    if (size == 0) {
        //printf("Error: Attempt to allocate zero bytes\n");
        return NULL;
    }
    size += sizeof(blockheader_t); // Include space for the header
    if (simulated_program_break == heap) {
        initialize_heap();
        //printf("Heap start address is %p\n", heap);
    }

    blockheader_t *current = list;
    blockheader_t *prev = NULL;

    //printf("simulated_program_break address is %p\n", simulated_program_break);
    //printf("Requested size %zu\n", size);
    if(((char *)current+size) >heap+HEAP_SIZE){
    	//printf("can't allocate size as it passes heap limit\n");
    	return NULL;
    }
    while (current) {
        if ((char *)current + size > heap + HEAP_SIZE) {
            //printf("Cannot allocate size as it exceeds heap limit\n");
            return NULL;
        }
        if ((current->allocation_status == UNALLOCATED_AREA) && (current->block_size >= size) ) {
            
            // Calculate the pointer to the next block after allocation
            blockheader_t *next_block_node = (blockheader_t *)((char *)current + size);

            // Split the block if there is remaining space
            if (current->block_size > size && ((current->block_size - size) > sizeof(blockheader_t))){
                next_block_node->block_size = current->block_size - size;
                next_block_node->next_block = current->next_block;
                next_block_node->allocation_status = UNALLOCATED_AREA;
                next_block_node->prev_block = current;
		if (next_block_node->next_block) {
    			next_block_node->next_block->prev_block = next_block_node;
		}
                // Update current block to the allocated block
                current->block_size = size;
                current->allocation_status = ALLOCATED_AREA;
                current->next_block = next_block_node;
            } else {
                // Block is used completely
                current->allocation_status = ALLOCATED_AREA;
 
            }
            // Update simulated program break if needed
            if (((char *)current + current->block_size) > simulated_program_break) {
                simulated_program_break = (char *)current + size+DEFAULT_ADDED_SIZE;
                if (simulated_program_break > heap + HEAP_SIZE) {
                    simulated_program_break = heap + HEAP_SIZE;
                }
            }
            //printf("simulated_program_break address is %p after allocating\n", simulated_program_break);
            return (char *)current + sizeof(blockheader_t);
        }

        prev = current;
	if (current->next_block != NULL) {
	    current->next_block->prev_block = current;
	}
	current = current->next_block;
    }
    //printf("failed to allocate\n");
    // No suitable block found
    return NULL;
#elif METHOD_OF_ALLOCATION==ALLOCATE_USING_SBRK//system call of kernel it self
	if (size == 0) {
        	size=78;
    	}

    	size += sizeof(blockheader_t); // Include space for the header

    	// Check if the heap needs to be initialized
    	if (heap_start == NULL) {
        	// Allocate initial heap
        	heap_start = sbrk(DEFAULT_ADDED_SIZE);
        	if (heap_start == (void *)-1) {
            	return NULL; // sbrk failed
        	}
        	heap_end = (char *)heap_start + DEFAULT_ADDED_SIZE;

        	// Initialize the free list with one large block
        	blockheader_t *initial_block = (blockheader_t *)heap_start;
        	initial_block->block_size = DEFAULT_ADDED_SIZE;
        	initial_block->allocation_status = UNALLOCATED_AREA;
        	initial_block->next_block = NULL;
        	initial_block->prev_block = NULL;
        	real_list = initial_block;
    	}

    	// Search for a suitable block
    	blockheader_t *current = real_list;
    	blockheader_t *prev = NULL;

    	while (current != NULL) {
        	if (current->allocation_status == UNALLOCATED_AREA && current->block_size >= size) {
            		// Split the block if there is enough space left
            		if ((current->block_size > size) && ((current->block_size - size) > sizeof(blockheader_t))) {
                		blockheader_t *next_block = (blockheader_t *)((char *)current + size);
                		next_block->block_size = current->block_size - size;
                		next_block->allocation_status = UNALLOCATED_AREA;
                		next_block->next_block = current->next_block;
                		next_block->prev_block = current;

                		current->block_size = size;
                		current->allocation_status = ALLOCATED_AREA;
                		current->next_block = next_block;
                		if (next_block->next_block) {
                   			 next_block->next_block->prev_block = next_block;
                		}
            		} else {
                		current->allocation_status = ALLOCATED_AREA;
            		}

            		// Remove the block from the free list
            		if (prev) {
                		prev->next_block = current->next_block;
                		if (current->next_block) {
                    			current->next_block->prev_block = prev;
                		}
            		} else {
                		real_list = current->next_block;
                		if (real_list) {
                    			real_list->prev_block = NULL;
                		}
            		}

            		return (char *)current + sizeof(blockheader_t);
        	}

        	prev = current;
        	current = current->next_block;
    	}

    	// No suitable block found, expand the heap
    	size_t expand_size = (size > DEFAULT_ADDED_SIZE) ? size+DEFAULT_ADDED_SIZE : DEFAULT_ADDED_SIZE;
    	void *new_heap = sbrk(expand_size);
    	if (new_heap == (void *)-1) {
        	return NULL; // sbrk failed
    	}

    	heap_end = (char *)new_heap + expand_size;

    	// Initialize the new block
    	blockheader_t *new_block = (blockheader_t *)new_heap;
    	new_block->block_size = expand_size;
    	new_block->allocation_status = UNALLOCATED_AREA;
    	new_block->next_block = real_list;
    	new_block->prev_block = NULL;
    	if (real_list) {
        	real_list->prev_block = new_block;
    	}
    	real_list = new_block;

    	return (char *)new_block + sizeof(blockheader_t);
#else
//printf("invalid method of allocation\n");
#endif
}

static void HmmFree(void *ptr){
#if METHOD_OF_FREEING == FREEING_USING_MY_FREE

	if (ptr == NULL) {
        	//printf("Error: Attempt to free a NULL pointer\n");
        	return;
    	}
	char found=0;
	blockheader_t block_to_free=(blockheader_t *)((char)ptr-sizeof(blockheader_t));
	blockheader_t *current=list;//from start
	blockheader_t *prev=NULL;
	blockheader_t *next=list;
	//printf("simulated program break is %p before any free\n", simulated_program_break);
	if (block_to_free->allocation_status != ALLOCATED_AREA) {
        	//printf("Error: Double free detected or block already free\n");
        	return;
    	}

	while(current){
		
		if(current == block_to_free && current->allocation_status==ALLOCATED_AREA){
			
			found=1;
			size_t size=current->block_size;
			//printf("size of block to delete is %zu\n", current->block_size);
			current->allocation_status=UNALLOCATED_AREA;
			if (current->prev_block && current->prev_block->allocation_status == UNALLOCATED_AREA) {
			    	current->prev_block->block_size += current->block_size;
			    	current->prev_block->next_block = current->next_block;
			    if (current->next_block) {
					current->next_block->prev_block = current->prev_block;
			    }
			    	current = current->prev_block;  // Move back to the previous block
		        }

			if (current->next_block && current->next_block->allocation_status == UNALLOCATED_AREA) {
			        current->block_size += current->next_block->block_size;
			        current->next_block = current->next_block->next_block;
			        if (current->next_block) {
					current->next_block->prev_block = current;
			    	}	
			}

			if ((char *)current + current->block_size >= simulated_program_break) {	
		        	simulated_program_break = simulated_program_break-size-DEFAULT_SUBTRACTED_SIZE;
		        	if(simulated_program_break < heap+START_SIZE){
		        		simulated_program_break=heap+START_SIZE;
		        	}
		        	//printf("New simulated program break is %p as block was at end so decrementing by %zu of it's size added with any free block before if found\n", simulated_program_break,DEFAULT_SUBTRACTED_SIZE+size);
		        	
		    	} else {
		        	//printf("Block freed keeping simulated program break same address %p as freed in between blocks not at end\n", simulated_program_break);
		    	}
		    	return;
			
			
		}
		//printf("searching\n");
		if (current->next_block != NULL) {
			prev = current;
			current->next_block->prev_block = prev;
			current = current->next_block;
		} else {
			// Properly handle the case where next_block is NULL
			//printf("Reached the end of the list.\n");
			return;
		}
		
	}
	if(found==0){
		//printf("not found\n");
	}
	
#elif METHOD_OF_FREEING == FREEING_USING_SBRK//system call of kernel it self
	if (ptr == NULL) {
        	return;
    	}

    	blockheader_t *block_to_free = (blockheader_t *)((char *)ptr - sizeof(blockheader_t));
    	blockheader_t *real_current = real_list;
    	blockheader_t *real_prev = NULL;

    	// Ensure the block to free is valid
    	if (block_to_free->allocation_status != ALLOCATED_AREA) {
        	return; // Double free or invalid free
    	}

    	// Find the block to free in the list
    	while (real_current != NULL) {
        	if (real_current == block_to_free) {
            		// We found the block to free
            		break;
       	 	}
        	real_prev = real_current;
        	real_current = real_current->next_block;
   	 }

    	if (real_current == NULL) {
        	// Block to free was not found in the list
       	 	return;
    	}

    	// Mark the block as free
    	real_current->allocation_status = UNALLOCATED_AREA;

    	// Coalesce adjacent free blocks
    	if (real_current->prev_block && real_current->prev_block->allocation_status == UNALLOCATED_AREA) {
        	// Merge with previous block
        	real_current->prev_block->block_size += real_current->block_size;
        	real_current->prev_block->next_block = real_current->next_block;
        	if (real_current->next_block) {
            		real_current->next_block->prev_block = real_current->prev_block;
        	}
        	real_current = real_current->prev_block; // Move to the merged block
    	}

    	if (real_current->next_block && real_current->next_block->allocation_status == UNALLOCATED_AREA) {
        	// Merge with next block
        	real_current->block_size += real_current->next_block->block_size;
        	real_current->next_block = real_current->next_block->next_block;
        	if (real_current->next_block) {
           	 	real_current->next_block->prev_block = real_current;
        	}
    	}

    	// Check if we should shrink the heap
    	if ((char *)real_current + real_current->block_size >= (char *)real_program_break) {
        	// We can shrink the heap
        	sbrk(-(real_current->block_size + DEFAULT_SUBTRACTED_SIZE)); // Adjust the heap size
        	real_program_break = (char *)real_current; // Update program break
    	}
#else
//printf("invalid method of allocation\n");
#endif
	
}









/-----------section contains functions definitions------------/
void *malloc(size_t size) {
    if(size==0 || size<sizeof(blockheader_t)){
    	size=align_size(sizeof(blockheader_t)+1024);
    }else{
    	size = align_size(size+sizeof(blockheader_t) );
    }
    
    return HmmAlloc(size);
}

void free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    HmmFree((char *)ptr );
}

void *realloc(void *ptr, size_t size) {
    if (ptr == NULL || (size==0 && ptr==NULL)) {
    	size=align_size(sizeof(blockheader_t)+1024);
        return HmmAlloc(size);
    }

    if (size == 0) {
        HmmFree(ptr);
        return NULL;
    }
    align_size(size);
    blockheader_t *block_to_reallocate = (blockheader_t *)((char *)ptr - sizeof(blockheader_t));
    
    if (size == block_to_reallocate->block_size - sizeof(blockheader_t)) {
        // as no need to reallocate if the size is the same
        return ptr;
    } else if (size < block_to_reallocate->block_size - sizeof(blockheader_t)) {
        // decreasing the block size
        size_t new_size = size + sizeof(blockheader_t)+128;
        blockheader_t *next_block_node = (blockheader_t *)((char *)block_to_reallocate + new_size);

        if ((block_to_reallocate->block_size > new_size)) {
            next_block_node->block_size = block_to_reallocate->block_size - new_size;
            next_block_node->allocation_status = UNALLOCATED_AREA;
            next_block_node->next_block = block_to_reallocate->next_block;
            next_block_node->prev_block = block_to_reallocate;

            block_to_reallocate->block_size = new_size;
            block_to_reallocate->next_block = next_block_node;

            if (next_block_node->next_block != NULL) {
                next_block_node->next_block->prev_block = next_block_node;
            }
        }

        return ptr;
    } else if (size > (block_to_reallocate->block_size - sizeof(blockheader_t))){
        void *new_ptr = HmmAlloc(size);
        if (new_ptr) {
            // Copy the data 
            memcpy(new_ptr, ptr, block_to_reallocate->block_size - sizeof(blockheader_t));
            HmmFree(ptr);
            //printf("succeedded!!\n");
        }
        return new_ptr;
    }else{
    	//printf("invalid \n");
    	return NULL;
    }
}


void *calloc(size_t nmemb, size_t size) {
#if METHOD_OF_ALLOCATION==ALLOCATE_USING_MY_MALL0C
    if (nmemb != 0 && size > ULONG_MAX / nmemb) {//ULONG_MAX is number used to determine maximum number allowed for elements' multiplied by thier size 
        //printf("Error: Integer overflow in calloc\n");
        return NULL;
    }

    size_t total_size = align_size(nmemb * size + sizeof(blockheader_t)); // Include space for the header

    if (total_size == 0) {
        //printf("Error: Attempt to allocate zero bytes\n");
        total_size =align_size(sizeof(blockheader_t)+1024);
    }

    if (simulated_program_break == heap) {
        initialize_heap();
        //printf("Heap start address is %p\n", heap);
    }

    blockheader_t *current = list;
    blockheader_t *prev = NULL;

    //printf("simulated_program_break address is %p\n", simulated_program_break);
    //printf("Requested size %zu\n", total_size);
    align_size(total_size);
    while (current) {
        if ((char *)current + total_size > heap + HEAP_SIZE) {
            //printf("Cannot allocate size as it exceeds heap limit\n");
            return NULL;
        }
        if ((current->allocation_status == UNALLOCATED_AREA) && (current->block_size >= total_size)) {
            // Split the block if necessary
            blockheader_t *next_block_node = (blockheader_t *)((char *)current + total_size);

            if (current->block_size > total_size && (current->block_size-total_size > sizeof(blockheader_t))) {
                next_block_node->block_size = current->block_size - total_size;
                next_block_node->next_block = current->next_block;
                next_block_node->allocation_status = UNALLOCATED_AREA;
                next_block_node->prev_block = current;
                if (current->next_block) {
                    next_block_node->next_block = current->next_block;
                    next_block_node->next_block->prev_block = next_block_node;
                }
                current->block_size = total_size;
                current->allocation_status = ALLOCATED_AREA;
                current->next_block = next_block_node;
            } else {
                current->block_size = total_size;
                current->allocation_status = ALLOCATED_AREA;
                current->next_block = NULL;
            }

            // Set all allocated memory to zero
            memset((char *)current + sizeof(blockheader_t), 0, nmemb * size);

            // Update simulated program break if needed
            if (((char *)current + current->block_size) > simulated_program_break) {
                simulated_program_break = (char *)current + total_size + DEFAULT_ADDED_SIZE;
                if (simulated_program_break > heap + HEAP_SIZE) {
                    simulated_program_break = heap + HEAP_SIZE;
                }
            }

            //printf("simulated_program_break address is %p after allocating\n", simulated_program_break);
            return (char *)current + sizeof(blockheader_t);
        }

        prev = current;
        if (current->next_block != NULL) {
            current->next_block->prev_block = current;
        }
        current = current->next_block;
    }

    //printf("failed to allocate\n");
    // No suitable block found
    return NULL;
#elif METHOD_OF_ALLOCATION==ALLOCATE_USING_SBRK//system call of kernel it self
	if (nmemb != 0 && size > ULONG_MAX / nmemb) {//ULONG_MAX is number used to determine maximum number allowed for elements' multiplied by thier size 
        	//printf("Error: Integer overflow in calloc\n");
        	return NULL;
    	}

    	size_t total_size = align_size(nmemb * size + sizeof(blockheader_t)); // Include space for the header

    	if (total_size == 0) {
        	//printf("Error: Attempt to allocate zero bytes\n");
        	total_size =align_size(sizeof(blockheader_t)+1024);
    	}


    	// Check if the heap needs to be initialized
    	if (heap_start == NULL) {
        	// Allocate initial heap
        	heap_start = sbrk(DEFAULT_ADDED_SIZE);
        	if (heap_start == (void *)-1) {
            	return NULL; // sbrk failed
        	}
        	heap_end = (char *)heap_start + DEFAULT_ADDED_SIZE;

        	// Initialize the free list with one large block
        	blockheader_t *initial_block = (blockheader_t *)heap_start;
        	initial_block->block_size = DEFAULT_ADDED_SIZE;
        	initial_block->allocation_status = UNALLOCATED_AREA;
        	initial_block->next_block = NULL;
        	initial_block->prev_block = NULL;
        	real_list = initial_block;
    	}

    	// Search for a suitable block
    	blockheader_t *current = real_list;
    	blockheader_t *prev = NULL;

    	while (current != NULL) {
        	if (current->allocation_status == UNALLOCATED_AREA && current->block_size >= total_size ) {
            		// Split the block if there is enough space left
            		if ((current->block_size > total_size ) && ((current->block_size - total_size ) > sizeof(blockheader_t))) {
                		blockheader_t *next_block = (blockheader_t *)((char *)current + total_size );
                		next_block->block_size = current->block_size - total_size ;
                		next_block->allocation_status = UNALLOCATED_AREA;
                		next_block->next_block = current->next_block;
                		next_block->prev_block = current;

                		current->block_size = total_size ;
                		current->allocation_status = ALLOCATED_AREA;
                		current->next_block = next_block;
                		if (next_block->next_block) {
                   			 next_block->next_block->prev_block = next_block;
                		}
            		} else {
                		current->allocation_status = ALLOCATED_AREA;
            		}

            		// Remove the block from the free list
            		if (prev) {
                		prev->next_block = current->next_block;
                		if (current->next_block) {
                    			current->next_block->prev_block = prev;
                		}
            		} else {
                		real_list = current->next_block;
                		if (real_list) {
                    			real_list->prev_block = NULL;
                		}
            		}
			memset((char *)current + sizeof(blockheader_t), 0, nmemb * total_size );
            		return (char *)current + sizeof(blockheader_t);
        	}

        	prev = current;
        	current = current->next_block;
    	}

    	// No suitable block found, expand the heap
    	size_t expand_size = (size > DEFAULT_ADDED_SIZE) ? size+DEFAULT_ADDED_SIZE : DEFAULT_ADDED_SIZE;
    	void *new_heap = sbrk(expand_size);
    	memset((char *)new_heap + sizeof(blockheader_t), 0, nmemb * expand_size);
    	if (new_heap == (void *)-1) {
        	return NULL; // sbrk failed
    	}

    	heap_end = (char *)new_heap + expand_size;

    	// Initialize the new block
    	blockheader_t *new_block = (blockheader_t *)new_heap;
    	new_block->block_size = expand_size;
    	new_block->allocation_status = UNALLOCATED_AREA;
    	new_block->next_block = real_list;
    	new_block->prev_block = NULL;
    	if (real_list) {
        	real_list->prev_block = new_block;
    	}
    	real_list = new_block;

    	return (char *)new_block + sizeof(blockheader_t);
#else
//printf("invalid method of allocation\n");
#endif
}

