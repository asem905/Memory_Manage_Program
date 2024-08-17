/*
Author: Assem Samy
File: memory_manage.c
comment: contains all functions definitions
*/
/-----------------includes section--------------/
#include "memory_manage.h"  // Include the header file which contains the function declarations and type definitions

/-----------------variables section-------------/
static char heap[HEAP_SIZE];  // Define a simulated heap of size HEAP_SIZE
static char *simulated_program_break = heap;  // Initialize the simulated program break pointer to the start of the heap
static blockheader_t *list = NULL;  // Initialize the pointer to the list of free blocks as NULL

/-----------section contains static functions definitions------------/
static void initialize_heap() {
    list = (blockheader_t *)heap;  // Set the start of the heap as the first block in the list
    list->block_size = HEAP_SIZE;  // Set the size of this block to the total heap size
    list->allocation_status = UNALLOCATED_AREA; // Mark the block as unallocated (free)
    list->next_block = NULL;  // There are no additional blocks yet
    list->prev_block = NULL;  // This is the only block, so no previous block
    simulated_program_break = heap + START_SIZE;  // Set the initial program break to a position after the start of the heap
}

/-----------section contains functions definitions------------/

void *HmmAlloc(size_t size) {
    blockheader_t *prev = NULL;  // Pointer to keep track of the previous block during iteration

    size += sizeof(blockheader_t); // Adjust the requested size to include space for the block header

    if (simulated_program_break == NULL) {
        printf("Error: Simulated program break is NULL\n");  // Error message if the simulated program break is not set
        return NULL;
    } else if (simulated_program_break == heap) {
        initialize_heap();  // Initialize the heap if it has not been done yet
        printf("Heap start address is %p \n", heap);  // Print the start address of the heap
    }

    if (size == 0) {
        printf("Error: Attempt to allocate zero bytes\n");  // Error message if trying to allocate zero bytes
        return simulated_program_break;  // Return the current program break pointer
    }

    blockheader_t *current = list;  // Start from the first block in the list
    printf("simulated_program_break address is %p as there is start size that before allocating must be there \n", simulated_program_break);  // Print the address of the simulated program break
    printf("size %zu\n", size);  // Print the size to be allocated
    
    while (current) {
        if (current->allocation_status == UNALLOCATED_AREA && current->block_size >= size) {
            // Check if the current block is free and large enough for the requested size

            // Calculate the address of the next block after allocation
            blockheader_t *next_block_node = (blockheader_t *)((char *)current + size);

            // Split the block if there is remaining space
            if (current->block_size > size) {
                next_block_node->block_size = current->block_size - size;  // Set the size of the new block
                next_block_node->next_block = current->next_block;  // Set the next block of the new block
                next_block_node->allocation_status = UNALLOCATED_AREA;  // Mark the new block as free
                next_block_node->prev_block = current;  // Set the previous block of the new block
                // Update the current block to reflect the allocation
                current->block_size = size;  // Set the size of the allocated block
                current->allocation_status = ALLOCATED_AREA;  // Mark the block as allocated
                current->next_block = next_block_node;  // Set the next block of the allocated block
            } else {
                // If the block size matches the requested size, no need to split
                current->allocation_status = ALLOCATED_AREA;  // Mark the block as allocated
            }

            // Update the simulated program break if the allocated block extends beyond it
            if (((char *)current + size) > simulated_program_break) {
                simulated_program_break = (char *)current + size + DEFAULT_ADDED_SIZE;  // Adjust the simulated program break
            }
            printf("simulated_program_break address is %p after allocating\n", simulated_program_break);  // Print the updated address of the simulated program break
            return (char *)current + sizeof(blockheader_t);  // Return a pointer to the allocated memory, skipping the header
        }
        prev = current;  // Update the previous block pointer
        current = current->next_block;  // Move to the next block in the list
        if (current) {
            current->prev_block = prev;  // Update the previous block pointer of the current block
        }
    }

    // If no suitable block was found
    return NULL;  // Return NULL to indicate allocation failure
}

void HmmFree(void *ptr) {
    if (ptr == NULL) {
        printf("Error: Attempt to free a NULL pointer\n");  // Error message if trying to free a NULL pointer
        return;
    }
    char found = 0;  // Flag to indicate if the block to be freed was found
    blockheader_t *block_to_free = (blockheader_t *)((char *)ptr - sizeof(blockheader_t));  // Get the block header from the pointer
    blockheader_t *current = list;  // Start from the first block in the list
    blockheader_t *prev = NULL;  // Previous block pointer
    blockheader_t *next = list;  // Next block pointer

    printf("simulated program break is %p before any free\n", simulated_program_break);  // Print the address of the simulated program break before freeing

    while (current) {
        if (current == block_to_free && current->allocation_status == ALLOCATED_AREA) {
            // Check if the block to be freed matches the current block and is allocated

            found = 1;  // Set the flag to indicate that the block was found
            size_t size = current->block_size;  // Get the size of the block to be freed
            printf("size of block to delete is %zu\n", current->block_size);  // Print the size of the block to be freed
            current->allocation_status = UNALLOCATED_AREA;  // Mark the block as free

            // Merge with previous block if it is also free
            if (prev && prev->allocation_status == UNALLOCATED_AREA) {
                prev->block_size += current->block_size;  // Combine sizes of adjacent free blocks
                prev->next_block = current->next_block;  // Update the next block pointer
                if (current->next_block) {
                    current->next_block->prev_block = prev;  // Update the previous block pointer of the next block
                }
                size += prev->block_size;  // Update the size to include the combined blocks
                current = prev;  // Move the current pointer to the previous block
            }

            // Merge with next block if it is also free
            if (current->next_block && current->next_block->allocation_status == UNALLOCATED_AREA) {
                current->block_size += current->next_block->block_size;  // Combine sizes of adjacent free blocks
                current->next_block = current->next_block->next_block;  // Update the next block pointer
                if (current->next_block) {
                    current->next_block->prev_block = current;  // Update the previous block pointer of the next block
                }
            }

            // Adjust the simulated program break if the block was at the end of the heap
            if ((char *)current + current->block_size >= simulated_program_break) {
                simulated_program_break = simulated_program_break + DEFAULT_SUBTRACTED_SIZE - size - DEFAULT_ADDED_SIZE;  // Adjust the simulated program break
                if (simulated_program_break <= heap + START_SIZE) {
                    simulated_program_break = heap + START_SIZE;  // Reset the program break if it goes below the start of the heap
                }
                printf("New simulated program break is %p as block was at end so decrementing by %zu of it's size added with any free block before if found\n", simulated_program_break, -DEFAULT_SUBTRACTED_SIZE + size);  // Print the updated address of the simulated program break
            } else {
                printf("Block freed keeping simulated program break same address %p as freed in between blocks not at end\n", simulated_program_break);  // Print the program break address if block was not at the end
            }

            printf("Block freed\n");  // Confirm that the block was freed
            return;
        }
        printf("searching\n");  // Indicate that the search is continuing
        prev = current;  // Update previous block pointer
        current = current->next_block;  // Move to the next block in the list
        if (current) {
            current->prev_block = prev;  // Update the previous block pointer of the current block
        }
    }
    if (found == 0) {
        printf("not found\n");  // Indicate that the block to be freed was not found
    }
}
