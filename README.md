# Memory Management Simulation

## Overview

This project provides a basic simulation of heap memory management using a custom allocator and deallocator. The code defines a simulated heap and manages memory allocation and deallocation using linked blocks. It includes basic memory management operations similar to those found in real-world systems, albeit in a simplified form.

## Files

- **memory_manage.c**: Contains the implementation of the memory management functions.
- **memory_manage.h**: (Assumed to exist) Header file with declarations for the memory management functions and definitions for constants and structures.

## Purpose

The primary purpose of this code is to simulate a heap memory area where memory blocks can be allocated and freed. This simulation helps in understanding how memory allocation and deallocation work at a low level and is useful for educational purposes.

## Functions

### `HmmAlloc(size_t size)`

Allocates a block of memory of at least the specified `size`. The size includes space for the memory block header. 

- **Parameters**: `size` - The size of the memory block to allocate.
- **Returns**: A pointer to the allocated memory block, or `NULL` if no suitable block is available.

#### Detailed Explanation:

1. **Adjust Size**: The requested `size` is increased by the size of the header to account for metadata.
2. **Initialize Heap**: If this is the first allocation, the heap is initialized with a single large free block.
3. **Find Suitable Block**: Traverse the list of memory blocks to find a free block that fits the requested size.
4. **Split Block**: If the found block is larger than needed, it is split into two blocks.
5. **Update Simulated Break**: Adjust the simulated program break pointer if the allocated block extends beyond the current break.
6. **Return Pointer**: Return a pointer to the memory after the block header.

### `HmmFree(void *ptr)`

Frees a previously allocated block of memory.

- **Parameters**: `ptr` - A pointer to the memory block to free.
- **Returns**: Nothing.

#### Detailed Explanation:

1. **Error Checking**: If the pointer is `NULL`, an error message is printed, and the function returns.
2. **Locate Block**: Calculate the block header pointer from the given pointer and find the corresponding block in the list.
3. **Merge Blocks**: Merge the freed block with adjacent free blocks (if any) to avoid fragmentation.
4. **Update Simulated Break**: Adjust the simulated program break pointer if the freed block was at the end of the heap.
5. **Return**: The function returns after freeing the block and merging adjacent blocks.

## How It Works

1. **Heap Initialization**: When the heap is first used, it is initialized with a single large block of free memory.
2. **Allocation**: The allocator traverses the list of blocks, finds a suitable free block, and either allocates it directly or splits it if it is too large.
3. **Deallocation**: The deallocator searches for the block to free, marks it as free, and merges it with adjacent free blocks if necessary.
4. **Simulated Program Break**: The simulated program break pointer helps track the end of allocated memory.

## Used Command to Build and Run
1.gcc -c app.c memory_manage.c memory_manage.h -o a.out
2../a.out <put number as it will skeep each time generated numbers of it in memory> <put number of blocks you want>



## Example Usage

Here is an example of how you might use these functions in a program:

```c
#include "memory_manage.h"
#include <stdio.h>

int main() {
    // Allocate a block of 100 bytes
    void *ptr = HmmAlloc(100);
    if (ptr) {
        printf("Allocated memory at %p\n", ptr);
    } else {
        printf("Allocation failed\n");
    }

    // Free the allocated block
    HmmFree(ptr);
    printf("Memory freed\n");

    return 0;
}
