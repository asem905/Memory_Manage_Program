/*
Author: Assem Samy
File: memory_manage.c
comment: contains all functions definitions
*/
/-----------------includes section--------------/
#include "memory_manage.h"  // Include the header file that declares the functions and data structures

/-----------------variables section-------------/
static char heap[HEAP_SIZE];  // Simulated heap area for memory management
static char *simulated_program_break = heap;  // Pointer to the current end of the simulated heap
static blockheader_t *list = NULL;  // Pointer to the start of the free block list

/-----------section contains static functions definitions------------/
static void initialize_heap() {
    list = (blockheader_t *)heap;  // Initialize the start of the heap as the first block
    list->block_size = HEAP_SIZE;  // Set the size of the first block to the total heap size
    list->allocation_status = UNALLOCATED_AREA; // Mark the block as free
    list->next_block = NULL;  // This is the only block, so no next block
    list->prev_block = NULL;  // No previous block
    simulated_program_break = heap + START_SIZE;  // Set the program break to a position after the start
}

/-----------section contains functions definitions------------/

void *HmmAlloc(size_t size) {
    blockheader_t *prev = NULL;  // Pointer to keep track of the previous block
    
    size += sizeof(blockheader_t); // Include space for the header in the size
    if (simulated_program_break == NULL) {
        printf("Error: Simulated program break is NULL\n");  // Error if the program break is not set
        return NULL;
    } else if (simulated_program_break == heap) {
        initialize_heap();  // Initialize the heap if it has not been done yet
    }
    if (size == 0) {
        printf("Error: Attempt to allocate zero bytes\n");  // Error if size requested is zero
        return simulated_program_break;
    }
    blockheader_t *current = list;  // Start with the first block in the list
    printf("Heap start address is %p \n", heap);  // Print the start address of the heap
    printf("simulated_program_break address is %p as there is start size that before allocating must be there \n", simulated_program_break);  // Print the current program break address
    printf("size %d\n", size);  // Print the requested allocation size
    
    while (current) {
        if (current->allocation_status == UNALLOCATED_AREA && current->block_size >= size) {
            // Check if the current block is free and large enough to allocate the requested size

            // Calculate the pointer to the next block after the current allocation
            blockheader_t *next_block_node = (blockheader_t *)((char *)current + size);

            // Split the block if there is remaining space
            if (current->block_size > size) {
                next_block_node->block_size = current->block_size - size;  // Set the size of the new block
                next_block_node->next_block = current->next_block;  // Set the next block of the new block
                next_block_node->allocation_status = UNALLOCATED_AREA;  // Mark the new block as free
                next_block_node->prev_block = current;  // Set the previous block of the new block
                // Update current block to the allocated block
                current->block_size = size;  // Set the size of the allocated block
                current->allocation_status = ALLOCATED_AREA;  // Mark the block as allocated
                current->next_block = next_block_node;  // Set the next block of the allocated block
            } else {
                // The block is exactly the size needed, no need to split
                current->allocation_status = ALLOCATED_AREA;  // Mark the block as allocated
            }

            // Update simulated program break if needed
            if (((char *)current + size) > simulated_program_break) {
                simulated_program_break = (char *)current + size + DEFAULT_ADDED_SIZE;  // Adjust the simulated program break
            }
            printf("simulated_program_break address is %p after allocating\n", simulated_program_break);  // Print the updated program break address
            return (char *)current + sizeof(blockheader_t);  // Return the pointer to the memory after the header
        }
        prev = current;  // Update previous block pointer
        current = current->next_block;  // Move to the next block
        current->prev_block = prev;  // Set the previous block of the current block
    }

    // No suitable block found
    return NULL;  // Return NULL if no block was found that fits the request
}

void HmmFree(void *ptr) {
    if (ptr == NULL) {
        printf("Error: Attempt to free a NULL pointer\n");  // Error if trying to free a NULL pointer
        return;
    }
    char found = 0;  // Flag to indicate if the block to free was found
    blockheader_t *block_to_free = (blockheader_t *)((char *)ptr - sizeof(blockheader_t));  // Get the block header from the pointer
    blockheader_t *current = list;  // Start with the first block in the list
    blockheader_t *prev = NULL;  // Previous block pointer
    blockheader_t *next = list;  // Next block pointer

    printf("simulated program break is %p before any free\n", simulated_program_break);  // Print the current program break address

    while (current) {
        if (current == block_to_free && current->allocation_status == ALLOCATED_AREA) {
            // Check if the block to free is found and is currently allocated

            found = 1;  // Block is found
            size_t size = current->block_size;  // Get the size of the block to free
            printf("size of block to delete is %zu\n", current->block_size);  // Print the size of the block
            current->allocation_status = UNALLOCATED_AREA;  // Mark the block as free
            
            // Merge with previous block if it is also free
            if (prev && prev->allocation_status == UNALLOCATED_AREA) {
                prev->block_size += current->block_size;  // Combine the size with the previous block
                prev->next_block = current->next_block;  // Update the next block pointer
                if (current->next_block) {
                    current->next_block->prev_block = prev;  // Update the previous block pointer of the next block
                }
                current = prev;  // Move the current pointer to the previous block
            }
            
            // Merge with next block if it is also free
            if (current->next_block && current->next_block->allocation_status == UNALLOCATED_AREA) {
                current->block_size += current->next_block->block_size;  // Combine the size with the next block
                current->next_block = current->next_block->next_block;  // Update the next block pointer
                if (current->next_block) {
                    current->next_block->prev_block = current;  // Update the previous block pointer of the next block
                }
            }

            // Adjust simulated program break if the block was at the end of the heap
            if ((char *)current + current->block_size >= simulated_program_break) {
                while (current && current->allocation_status == UNALLOCATED_AREA) {
                    if (prev && prev->allocation_status == UNALLOCATED_AREA) {
                        prev->block_size += current->block_size;  // Combine sizes of adjacent free blocks
                        prev->next_block = current->next_block;  // Update the next block pointer
                        if (current->next_block) {
                            current->next_block->prev_block = prev;  // Update the previous block pointer of the next block
                        }
                        current = prev;  // Move to the previous block
                        size = current->block_size;  // Update the size of the combined block
                    }
                    simulated_program_break = simulated_program_break - DEFAULT_SUBTRACTED_SIZE - size;  // Adjust the simulated program break with previous blocks 
                    if (simulated_program_break <= heap) {
                        simulated_program_break = heap + START_SIZE;  // Reset the program break if it goes below the heap start as at end of freing allblocks
                        break;
                    }
                }
                simulated_program_break = simulated_program_break - DEFAULT_SUBTRACTED_SIZE - size;  // Final adjustment of the program break
                printf("New simulated program break is %p as block was at end so decrementing by %d\n", simulated_program_break, DEFAULT_SUBTRACTED_SIZE - size);  // Print the updated program break address
            } else {
                printf("Block freed keeping simulated program break same address %p as freed in between blocks not at end\n", simulated_program_break);  // Print the program break address if block was not at the end
            }

            printf("Block freed\n");  // Confirm that the block was freed
            return;
        }
        printf("searching\n");  // Indicate that the search is continuing
        prev = current;  // Update previous block pointer
        current = current->next_block;  // Move to the next block
        current->prev_block = prev;  // Update the previous block pointer of the current block
    }
    if (found == 0) {
        printf("not found\n");  // Indicate that the block was not found in the list
    }
}
