Memory Management Functions Overview
This document provides an overview of the memory management functions defined in memory_manage.c. The file contains implementations of four key functions: HmmAlloc, HmmFree, Hmmrealloc, and Hmmcalloc. Each function is designed to manage memory in a simulated heap environment.

1. HmmAlloc
Purpose:
Allocates a block of memory of a specified size.

Functionality:

Input: size - The size of the memory block to allocate.
Output: Pointer to the allocated memory block or NULL if allocation fails.
Steps:

Adjust Size:

Adds space for the block header to the requested size.
Initialization:

If this is the first allocation, initializes the heap.
Traversal:

Iterates through the list of blocks to find a suitable free block.
Block Allocation:

Checks if the current block is large enough.
Splits the block if there’s extra space after allocation.
Updates the block's size and status.
Updates the simulated program break if the new block extends beyond it.
Return:

Returns a pointer to the allocated memory, excluding the block header.
Edge Cases:

Handles zero-size requests by returning an error message.
Checks for heap overflow conditions.

2. HmmFree
Purpose:
Frees a previously allocated block of memory.

Functionality:

Input: ptr - Pointer to the memory block to free.
Steps:

Validation:

Checks if the pointer is NULL.
Block Identification:

Determines the block to free by subtracting the size of the block header from the pointer.
Block Deallocation:

Marks the block as free.
Merges adjacent free blocks to avoid fragmentation.
Updates the simulated program break if the freed block is at the end of the heap.
Return:

Returns after completing the deallocation process or logs an error if the block is not found.
Edge Cases:

Handles the case where the block is not found in the list.
Properly updates pointers when merging adjacent free blocks.

3. Hmmrealloc
Purpose:
Resizes an existing block of memory to a new size.

Functionality:

Input: ptr - Pointer to the memory block to resize.
Input: size - New size of the memory block.
Steps:

Handle NULL Pointer:

Allocates a new block if the pointer is NULL.
Handle Zero Size:

Frees the block if the new size is zero.
Check Current Size:

Determines if the new size is smaller or larger than the current size.
Resize Block:

If smaller, adjusts the block size and splits it if needed.
If larger, allocates a new block, copies the data, and frees the old block.
Return:

Returns the pointer to the resized block or the new allocated block.
Edge Cases:

Handles cases where the new size is equal to, less than, or greater than the current block size.
Ensures proper copying and freeing of memory.

4. Hmmcalloc
Purpose:
Allocates and initializes a block of memory to zero.

Functionality:

Input: nmemb - Number of elements.
Input: size - Size of each element.
Steps:

Overflow Check:

Checks for integer overflow when calculating the total size.
Initialization:

Initializes the heap if it’s the first allocation.
Memory Allocation:

Allocates a block of memory large enough to hold the requested number of elements and initializes it to zero.
Return:

Returns a pointer to the allocated and zero-initialized memory.
Edge Cases:

Handles overflow and zero-size requests.
Properly initializes the memory to zero.

Common Concepts
Heap Simulation: A static array (heap) is used to simulate memory allocation, with the simulated_program_break tracking the end of allocated space.
Block Header: Each block of memory includes a blockheader_t structure to store metadata such as size and allocation status.
Allocation Status: Blocks can be either allocated or unallocated, with the ability to merge adjacent free blocks to reduce fragmentation.
These functions together provide a basic memory management system, handling dynamic memory allocation, deallocation, resizing, and zero-initialization within a simulated environment.
