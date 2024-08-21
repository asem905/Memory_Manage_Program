/*
Author:Assem Samy
File:memory_manage.c
comment:contains all functions definitions
*/

/-----------------includes section--------------/
// Include the header file for memory management functions
#include "memory_manage.h"

/-----------------variables section-------------/
// Simulated heap area for memory management
static char heap[HEAP_SIZE];  // Simulated heap area
// Pointer to track the end of the simulated heap
static char *simulated_program_break = heap;
// Pointer to the start of the free block list
static blockheader_t *list = NULL;

/-----------section contains static functions definitions------------/
// Function to initialize the heap
static void initialize_heap() {
    // Initialize the block list at the start of the heap
    list = (blockheader_t *)heap;
    // Set the size of the first block to the full heap size
    list->block_size = HEAP_SIZE;
    // Mark the first block as free
    list->allocation_status = UNALLOCATED_AREA; // Free
    // Set next and previous block pointers to NULL
    list->next_block = NULL;
    list->prev_block = NULL;
    // Set the simulated program break to after the initial block
    simulated_program_break = heap + START_SIZE;
}

/-----------section contains functions definitions------------/

// Function to allocate memory of a given size
void *HmmAlloc(size_t size) {
#if METHOD_OF_ALLOCATION == ALLOCATE_USING_MY_MALL0C
    size += sizeof(blockheader_t); // Include space for the header

    // Check if the requested size is zero
    if (size == 0) {
        printf("Error: Attempt to allocate zero bytes\n");
        return NULL;
    }

    // Initialize the heap if it is the first allocation
    if (simulated_program_break == heap) {
        initialize_heap();
        printf("Heap start address is %p\n", heap);
    }

    blockheader_t *current = list; // Pointer to traverse the list of blocks
    blockheader_t *prev = NULL; // Pointer to keep track of the previous block

    printf("simulated_program_break address is %p\n", simulated_program_break);
    printf("Requested size %zu\n", size);
    // Check if allocation would exceed heap limits
    if (((char *)current + size) > heap + HEAP_SIZE) {
        printf("can't allocate size as it passes heap limit\n");
        return NULL;
    }

    // Traverse the list of blocks to find a suitable block
    while (current) {
        // Check if allocation would exceed heap limits
        if ((char *)current + size > heap + HEAP_SIZE) {
            printf("Cannot allocate size as it exceeds heap limit\n");
            return NULL;
        }
        // Check if the current block is free and large enough
        if ((current->allocation_status == UNALLOCATED_AREA) && (current->block_size >= size)) {
            
            // Calculate the pointer to the next block after allocation
            blockheader_t *next_block_node = (blockheader_t *)((char *)current + size);

            // Split the block if there is remaining space
            if (current->block_size > size && ((current->block_size - size) > sizeof(blockheader_t))) {
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
                simulated_program_break = (char *)current + size + DEFAULT_ADDED_SIZE;
                if (simulated_program_break > heap + HEAP_SIZE) {
                    simulated_program_break = heap + HEAP_SIZE;
                }
            }
            printf("simulated_program_break address is %p after allocating\n", simulated_program_break);
            // Return a pointer to the memory (excluding the header)
            return (char *)current + sizeof(blockheader_t);
        }

        prev = current; // Update previous block pointer
        if (current->next_block != NULL) {
            current->next_block->prev_block = current;
        }
        current = current->next_block; // Move to the next block
    }
    printf("failed to allocate\n");
    // No suitable block found
    return NULL;
#elif METHOD_OF_ALLOCATION == ALLOCATE_USING_SBRK // System call of kernel itself

#else
    // Invalid method of allocation
    printf("invalid method of allocation\n");
#endif
}

// Function to free allocated memory
void HmmFree(void *ptr) {
#if METHOD_OF_FREEING == FREEING_USING_MY_FREE
    // Check if the pointer is NULL
    if (ptr == NULL) {
        printf("Error: Attempt to free a NULL pointer\n");
        return;
    }
    char found = 0; // Flag to check if block was found
    // Calculate the block header pointer from the given pointer
    blockheader_t *block_to_free = (blockheader_t *)((char *)ptr - sizeof(blockheader_t));
    blockheader_t *current = list; // Pointer to traverse the list of blocks
    blockheader_t *prev = NULL; // Pointer to keep track of the previous block
    blockheader_t *next = list; // Pointer to keep track of the next block
    printf("simulated program break is %p before any free\n", simulated_program_break);

    // Traverse the list of blocks to find the block to free
    while (current) {
        // Check if the current block matches the block to free
        if (current == block_to_free && current->allocation_status == ALLOCATED_AREA) {
            found = 1; // Block found
            size_t size = current->block_size; // Size of the block to free
            printf("size of block to delete is %zu\n", current->block_size);
            current->allocation_status = UNALLOCATED_AREA; // Mark block as free
            // Merge with previous block if it's free
            if (current->prev_block && current->prev_block->allocation_status == UNALLOCATED_AREA) {
                current->prev_block->block_size += current->block_size;
                current->prev_block->next_block = current->next_block;
                if (current->next_block) {
                    current->next_block->prev_block = current->prev_block;
                }
                current = current->prev_block; // Move back to the previous block
            }
            // Merge with next block if it's free
            if (current->next_block && current->next_block->allocation_status == UNALLOCATED_AREA) {
                current->block_size += current->next_block->block_size;
                current->next_block = current->next_block->next_block;
                if (current->next_block) {
                    current->next_block->prev_block = current;
                }
            }
            // Update simulated program break if needed
            if ((char *)current + current->block_size >= simulated_program_break) {
                simulated_program_break = simulated_program_break - size - DEFAULT_SUBTRACTED_SIZE;
                if (simulated_program_break < heap + START_SIZE) {
                    simulated_program_break = heap + START_SIZE;
                }
                printf("New simulated program break is %p as block was at end so decrementing by %zu of it's size added with any free block before if found\n", simulated_program_break, DEFAULT_SUBTRACTED_SIZE + size);
            } else {
                printf("Block freed keeping simulated program break same address %p as freed in between blocks not at end\n", simulated_program_break);
            }
            return;
        }
        printf("searching\n");
        if (current->next_block != NULL) {
            prev = current;
            current->next_block->prev_block = prev;
            current = current->next_block; // Move to the next block
        } else {
            // Properly handle the case where next_block is NULL
            printf("Reached the end of the list.\n");
            return;
        }
    }
    if (found == 0) {
        printf("not found\n");
    }
#elif METHOD_OF_FREEING == FREEING_USING_SBRK // System call of kernel itself

#else
    // Invalid method of freeing
    printf("invalid method of allocation\n");
#endif
}

// Function to reallocate memory to a new size
void *Hmmrealloc(void *ptr, size_t size) {
    // Handle the case where the pointer is NULL by allocating new memory
    if (ptr == NULL) {
        return HmmAlloc(size);
    }

    // Handle the case where the new size is zero by freeing the memory
    if (size == 0) {
        HmmFree(ptr);
        return NULL;
    }

    // Calculate the block header pointer from the given pointer
    blockheader_t *block_to_reallocate = (blockheader_t *)((char *)ptr - sizeof(blockheader_t));
    
    // Check if the new size is the same as the current block size
    if (size == block_to_reallocate->block_size - sizeof(blockheader_t)) {
        return ptr; // No need to reallocate if the size is the same
    } else if (size < block_to_reallocate->block_size - sizeof(blockheader_t)) {
        // Case where the new size is smaller than the current block size
        size_t new_size = size + sizeof(blockheader_t); // Calculate new size including header
        blockheader_t *next_block_node = (blockheader_t *)((char *)block_to_reallocate + new_size); // Pointer to the next block

        // Check if there's enough space to split the block
        if (block_to_reallocate->block_size > new_size) {
            // Set up the new block header for the remaining space
            next_block_node->block_size = block_to_reallocate->block_size - new_size;
            next_block_node->allocation_status = UNALLOCATED_AREA;
            next_block_node->next_block = block_to_reallocate->next_block;
            next_block_node->prev_block = block_to_reallocate;

            // Update the current block header
            block_to_reallocate->block_size = new_size;
            block_to_reallocate->next_block = next_block_node;

            if (next_block_node->next_block != NULL) {
                next_block_node->next_block->prev_block = next_block_node;
            }
        }

        return ptr; // Return the pointer to the resized memory block
    } else if (size > block_to_reallocate->block_size - sizeof(blockheader_t)) {
        // Case where the new size is larger than the current block size
        void *new_ptr = HmmAlloc(size); // Allocate new memory block
        if (new_ptr) {
            // Copy the data from the old block to the new block
            memcpy(new_ptr, ptr, block_to_reallocate->block_size - sizeof(blockheader_t));
            HmmFree(ptr); // Free the old block
            printf("succeeded!!\n");
        }
        return new_ptr; // Return the new pointer
    } else {
        printf("invalid \n"); // Invalid case
        return NULL;
    }
}

// Function to allocate memory and initialize it to zero
void *Hmmcalloc(size_t nmemb, size_t size) {
    // Check for integer overflow when calculating total size
    if (nmemb != 0 && size > ULONG_MAX / nmemb) { // ULONG_MAX is used to determine the maximum value for nmemb * size
        printf("Error: Integer overflow in calloc\n");
        return NULL;
    }

    size_t total_size = nmemb * size + sizeof(blockheader_t); // Calculate total size including header

    // Check if the requested size is zero
    if (total_size == 0) {
        printf("Error: Attempt to allocate zero bytes\n");
        return NULL;
    }

    // Initialize the heap if it is the first allocation
    if (simulated_program_break == heap) {
        initialize_heap();
        printf("Heap start address is %p\n", heap);
    }

    blockheader_t *current = list; // Pointer to traverse the list of blocks
    blockheader_t *prev = NULL; // Pointer to keep track of the previous block

    printf("simulated_program_break address is %p\n", simulated_program_break);
    printf("Requested size %zu\n", total_size);

    // Traverse the list of blocks to find a suitable block
    while (current) {
        // Check if allocation would exceed heap limits
        if ((char *)current + total_size > heap + HEAP_SIZE) {
            printf("Cannot allocate size as it exceeds heap limit\n");
            return NULL;
        }
        // Check if the current block is free and large enough
        if ((current->allocation_status == UNALLOCATED_AREA) && (current->block_size >= total_size)) {
            // Split the block if necessary
            blockheader_t *next_block_node = (blockheader_t *)((char *)current + total_size);

            if (current->block_size > total_size && (current->block_size - total_size > sizeof(blockheader_t))) {
                // Set up the new block header for the remaining space
                next_block_node->block_size = current->block_size - total_size;
                next_block_node->next_block = current->next_block;
                next_block_node->allocation_status = UNALLOCATED_AREA;
                next_block_node->prev_block = current;
                if (next_block_node->next_block) {
                    next_block_node->next_block->prev_block = next_block_node;
                }
                current->block_size = total_size;
                current->allocation_status = ALLOCATED_AREA;
                current->next_block = next_block_node;
            } else {
                // No need to split, use the entire block
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

            printf("simulated_program_break address is %p after allocating\n", simulated_program_break);
            // Return a pointer to the memory (excluding the header)
            return (char *)current + sizeof(blockheader_t);
        }

        prev = current; // Update previous block pointer
        if (current->next_block != NULL) {
            current->next_block->prev_block = current;
        }
        current = current->next_block; // Move to the next block
    }

    printf("failed to allocate\n");
    // No suitable block found
    return NULL;
}
