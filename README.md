## Memory Management Functions Overview
This document provides an overview of the memory management functions defined in memory_manage.c. The file contains implementations of four key functions: HmmAlloc, HmmFree, Hmmrealloc, and Hmmcalloc. Each function is designed to manage memory in a simulated heap environment.



## `HmmAlloc` Function

**Purpose:**  
Allocates a block of memory of a specified size from the simulated heap.

**Functionality:**  
Allocates memory, including space for a block header. Initializes the heap if necessary, finds a suitable free block, and updates the simulated program break if required.

**Steps:**

1. **Adjust Size:**
   - Add space for the block header to the requested size.

2. **Heap Initialization:**
   - If this is the first allocation, initialize the heap and print the start address.

3. **Traversal:**
   - Iterate through the list of blocks to find a free block.
   - Check if the current block plus the requested size exceeds the heap limits.

4. **Block Allocation:**
   - If a suitable block is found:
     - **Split Block (if necessary):** Create a new block if there is remaining space after allocation.
     - **Update Block Header:** Set the size and allocation status of the current block.
     - **Update Simulated Program Break:** Adjust if the block extends beyond the current break.

5. **Return:**
   - Return a pointer to the allocated memory, excluding the header.

**Edge Cases:**
- Handles zero-size requests with an error message.
- Checks for heap overflow conditions.



## `HmmFree` Function

**Purpose:**  
Frees a previously allocated block of memory.

**Functionality:**  
Frees the specified memory block, merges adjacent free blocks if necessary, and updates the simulated program break if the freed block is at the end.

**Steps:**

1. **Validation:**
   - Check if the pointer is `NULL`. Return immediately if it is.

2. **Block Identification:**
   - Calculate the block header pointer from the given pointer.

3. **Block Deallocation:**
   - Mark the block as free.
   - Merge with adjacent free blocks if needed.
   - Update the simulated program break if the freed block is at the end of the heap.

4. **Return:**
   - Finish after updating the list of blocks or log an error if the block is not found.

**Edge Cases:**
- Handles cases where the block to free is not found.
- Properly updates pointers when merging adjacent free blocks.



## `Hmmrealloc` Function

**Purpose:**  
Resizes an existing block of memory to a new size.

**Functionality:**  
Resizes the memory block, handling cases where the new size is smaller, larger, or the same. Allocates a new block if necessary and copies data.

**Steps:**

1. **Handle NULL Pointer:**
   - Allocate a new block if the pointer is `NULL`.

2. **Handle Zero Size:**
   - Free the block if the new size is zero.

3. **Check Current Size:**
   - If the new size is smaller, adjust the block size and potentially split it.
   - If the new size is larger, allocate a new block, copy the data, and free the old block.

4. **Return:**
   - Return the pointer to the resized block or the newly allocated block.

**Edge Cases:**
- Handles cases where the new size is the same, smaller, or larger than the current block size.
- Ensures data is copied correctly and old blocks are freed.



## `Hmmcalloc` Function

**Purpose:**  
Allocates and initializes a block of memory to zero.

**Functionality:**  
Allocates a memory block for a specified number of elements, initializes it to zero, and updates the simulated program break if necessary.

**Steps:**

1. **Overflow Check:**
   - Check for integer overflow when calculating the total size.

2. **Initialization:**
   - Initialize the heap if it's the first allocation.

3. **Memory Allocation:**
   - Allocate a block of memory large enough to hold the requested number of elements.
   - Initialize the allocated memory to zero.

4. **Return:**
   - Return a pointer to the zero-initialized memory.

**Edge Cases:**
- Handles integer overflow and zero-size requests.
- Ensures the memory is properly zeroed out.

