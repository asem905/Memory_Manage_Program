Certainly! Here’s a revised version of the README file with improved clarity and organization:

---

# Custom Memory Allocator

## Overview

This project provides a custom memory allocator implemented in C, demonstrating two methods of managing memory:

1. **Custom Allocation (`malloc`-based)**: Implements memory allocation directly within the allocator.
2. **System Call Allocation (`sbrk`-based)**: Uses the system call `sbrk` to manage heap memory.

## Contents

1. [Functionality](#functionality)
2. [Implementation Details](#implementation-details)
   - [Custom Allocation](#custom-allocation)
   - [System Call Using `sbrk`](#system-call-using-sbrk)
3. [Usage](#usage)
4. [How to implement on your shell](#how-to-implement-on-your-shell)

---

## Functionality

The custom memory allocator includes the following functions:

### `void *malloc(size_t size)`

Allocates memory of the requested size and returns a pointer to the allocated block. Supports both custom and `sbrk`-based allocation methods.

### `void free(void *ptr)`

Frees the memory block previously allocated by `malloc` or `realloc`. If `ptr` is `NULL`, the function performs no action.

### `void *realloc(void *ptr, size_t size)`

Resizes the memory block pointed to by `ptr` to the specified size. Handles cases where the new size is smaller, larger, or zero.

### `void *calloc(size_t nmemb, size_t size)`

Allocates memory for an array of `nmemb` elements, each of `size` bytes, and initializes the memory to zero.

## Implementation Details

### Custom Allocation

#### How It Works

1. **Initialization**:
   - **Heap**: Uses a static array to simulate the heap.
   - **Block Header**: Each block includes metadata like size and allocation status.
   - **Free List**: Manages a list of free blocks for allocation.

2. **Allocation (`HmmAlloc`)**:
   - **Size Calculation**: Adjusts the requested size to account for metadata.
   - **Initialization Check**: Initializes the simulated heap if it’s the first allocation request.
   - **Search for Free Block**: Looks through the free list to find a suitable block.
   - **Block Splitting**: Splits a free block if it’s larger than required.
   - **Simulated Program Break**: Updates a pointer to manage heap end.

3. **Freeing (`HmmFree`)**:
   - **Marking as Free**: Marks the block as free and adds it back to the free list.
   - **Coalescing**: Merges adjacent free blocks to reduce fragmentation.

#### Example Code

```c
static void *HmmAlloc(size_t size) {
    // Custom allocation logic here...
}
```

### System Call Using `sbrk`

#### How It Works

1. **Initialization**:
   - **Heap Start**: Uses `sbrk` to allocate and manage heap memory.
   - **Block Header**: Similar to custom allocation, each block includes metadata.
   - **Free List**: Maintains a list of free blocks.

2. **Allocation (`HmmAlloc`)**:
   - **Size Calculation**: Adjusts size for metadata and rounds as needed.
   - **Heap Initialization**: Initializes the heap using `sbrk` if it's the first request.
   - **Search for Free Block**: Finds a suitable block in the free list.
   - **Heap Expansion**: Expands the heap using `sbrk` if no suitable block is found.

3. **Freeing (`HmmFree`)**:
   - **Marking as Free**: Marks the block as free and coalesces adjacent free blocks.
   - **Shrinking Heap**: Reduces heap size using `sbrk` if the freed block is at the end.

#### Example Code

```c
static void *HmmAlloc(size_t size) {
    // sbrk-based allocation logic here...
}
```

## Usage

To use the custom memory allocator:

1. **Include Header**: Include the provided header file in your source code.
2. **Compile and Link**: Compile your source code and link against the allocator implementation.
3. **Call Functions**: Use `malloc`, `free`, `realloc`, and `calloc` as needed in your application.

## How to implement
1-as shared library:
 -gcc -FPIC -shared /pathoffile/libhmm.so memory_manage.c
 -LD_PRELOAD=/pathoffile/libhmm.so <command_you_want_to_run_withit>
 
2-run any test program as that as remove app.c if you ant to run your program:
 -gcc app.c memory_manage.c memory_manage.h -o a.out
 -LD_PRELOAD=/pathoffile/libhmm.so ./a.out

Feel free to adjust the content to better fit your needs or add more specifics if required.
