/*
Author: Assem Samy
File: memory_manage.c
comment: Contains all functions definitions
*/

/-----------------includes section--------------/
#include "memory_manage.h"  // Include the header file for declarations related to memory management

/-----------------variables section-------------/
static char heap[HEAP_SIZE];  // Simulated heap area of fixed size
static char *simulated_program_break = heap;  // Pointer to the end of allocated memory (initially the start of heap)
static blockheader_t *list = NULL;  // Pointer to the start of the list of memory blocks

/-----------section contains static functions definitions------------/
static void initialize_heap() {
    // Initialize the simulated heap with one large free block
    list = (blockheader_t *)heap;  // Set the start of the block list to the start of heap
    list->block_size = HEAP_SIZE;  // Set the size of the initial block to the entire heap size
    list->allocation_status = UNALLOCATED_AREA; // Mark the block as free
    list->next_block = NULL;  // No next block initially
    list->prev_block = NULL;  // No previous block initially
    simulated_program_break = heap;  // Set the simulated program break to the start of the heap
}

/-----------section contains functions definitions------------/

void *HmmAlloc(size_t size) {
    blockheader_t *prev = NULL;  // Previous block pointer (used for traversing the list)

    size += sizeof(blockheader_t); // Adjust size to include the header

    if (simulated_program_break == heap) {
        initialize_heap();  // Initialize heap if this is the first allocation
    }
    blockheader_t *current = list;  // Start with the first block in the list
    printf("simulated_program_break address is %p before allocating\n", simulated_program_break);  // Print the address of the simulated program break before allocation
    printf("size %zu\n", size);  // Print the adjusted size needed

    while (current) {
        if (current->allocation_status == UNALLOCATED_AREA && current->block_size >= size) {
            // Found a free block with enough size

            // Calculate the pointer to the next block after the allocation
            blockheader_t *next_block_node = (blockheader_t *)((char *)current + size);

            // Split the block if there is remaining space
            if (current->block_size > size) {
                next_block_node->block_size = current->block_size - size;  // Set size of the next block
                next_block_node->next_block = current->next_block;  // Link the next block
                next_block_node->allocation_status = UNALLOCATED_AREA;  // Mark as free
                next_block_node->prev_block = current;  // Set previous block pointer
                // Update current block to the allocated block
                current->block_size = size;  // Set the size of the allocated block
                current->allocation_status = ALLOCATED_AREA;  // Mark as allocated
                current->next_block = next_block_node;  // Link to the next block
            } else {
                // Block size exactly matches the requested size, no need to split
                current->allocation_status = ALLOCATED_AREA;  // Mark as allocated
            }

            // Update simulated program break if needed
            if (((char *)current + size) > simulated_program_break) {
                simulated_program_break = (char *)current + size + DEFAULT_ADDED_SIZE;  // Move the simulated program break
            }
            printf("simulated_program_break address is %p after allocating\n", simulated_program_break);  // Print the address of the simulated program break after allocation
            return (char *)current + sizeof(blockheader_t);  // Return pointer to the allocated memory (after the header)
        }
        prev = current;  // Move previous block pointer to current
        current = current->next_block;  // Move to the next block in the list
        current->prev_block = prev;  // Set the previous block pointer for the next block
    }

    // No suitable block found
    return NULL;  // Return NULL if no block was found that fits the requested size
}

void HmmFree(void *ptr) {
    if (ptr == NULL) {
        printf("Error: Attempt to free a NULL pointer\n");  // Error handling for NULL pointer
        return;
    }

    char found = 0;  // Flag to indicate if the block to free was found
    blockheader_t *block_to_free = (blockheader_t *)((char *)ptr - sizeof(blockheader_t));  // Get the block header from the pointer
    blockheader_t *current = list;  // Start with the first block in the list
    blockheader_t *prev = NULL;  // Previous block pointer
    blockheader_t *next = list;  // Next block pointer

    printf("simulated program break is %p before any free\n", simulated_program_break);  // Print the address of the simulated program break before freeing

    while (current) {
        if (current == block_to_free && current->allocation_status == ALLOCATED_AREA) {
            // Found the block to free

            found = 1;  // Set the flag to indicate the block was found
            size_t size = current->block_size;  // Get the size of the block to free
            printf("size of block to delete is %zu\n", current->block_size);  // Print the size of the block to be freed
            current->allocation_status = UNALLOCATED_AREA;  // Mark the block as free

            // Merge with previous block if it's also free
            if (prev && prev->allocation_status == UNALLOCATED_AREA) {
                prev->block_size += current->block_size;  // Merge size with previous block
                prev->next_block = current->next_block;  // Link to the next block
                if (current->next_block) {
                    current->next_block->prev_block = prev;  // Update previous pointer of the next block
                }
                current = prev;  // Move current pointer to the previous block
            }

            // Merge with next block if it's also free
            if (current->next_block && current->next_block->allocation_status == UNALLOCATED_AREA) {
                current->block_size += current->next_block->block_size;  // Merge size with next block
                current->next_block = current->next_block->next_block;  // Link to the next block
                if (current->next_block) {
                    current->next_block->prev_block = current;  // Update previous pointer of the next block
                }
            }

            // Update simulated program break if the freed block was at the end
            if ((char *)current + current->block_size >= simulated_program_break) {
                simulated_program_break = simulated_program_break - DEFAULT_SUBTRACTED_SIZE - size;  // Move the simulated program break
                printf("New simulated program break is %p as block was at end so decrementing by %d\n", simulated_program_break, DEFAULT_SUBTRACTED_SIZE - size);  // Print the new simulated program break address
            } else {
                printf("Block freed keeping simulated program break same address %p as freed in between blocks not at end\n", simulated_program_break);  // Block was freed in between, so no change to simulated program break
            }

            printf("Block freed\n");  // Print message indicating the block has been freed
            return;  // Return from the function
        }
        printf("searching\n");  // Print message indicating that the search is ongoing
        prev = current;  // Move previous block pointer to current
        current = current->next_block;  // Move to the next block in the list
        current->prev_block = prev;  // Set the previous block pointer for the next block
    }

    if (found == 0) {
        printf("not found\n");  // Print message if the block to free was not found
    }
}
