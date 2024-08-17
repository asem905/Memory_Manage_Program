#include "memory_manage.h"

int main(int argc, char *argv[]) {
    /*example for testing as we randomly test by a random size to allocate then free*/
    int nloops;
    unsigned int seed;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <seed> <nloops>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    seed = atoi(argv[1]);
    nloops = atoi(argv[2]);

    srand(seed);

    void *allocated_blocks[nloops]; // Array to hold pointers to allocated blocks

    // Allocate blocks of random sizes
    for (int j = 0; j < nloops; j++) {
        size_t size = rand() % MAX_ALLOC_SIZE + 1; // Random size between 1 and MAX_ALLOC_SIZE
        void *ptr = HmmAlloc(size);
        allocated_blocks[j] = ptr; // Store pointer in array
        if (ptr) {
            printf("Allocated %zu bytes at %p\n", size, ptr);
        } else {
            printf("Failed to allocate %zu bytes\n", size);
        }
    }

    // Free allocated blocks from first block to last so simulated program break will be kept same till last block it will be decremented by size of last block and default subtracted size
    for (int j = 0; j < nloops; j++) {
        if (allocated_blocks[j]) {
            HmmFree(allocated_blocks[j]);
            printf("Freed memory at %p\n", allocated_blocks[j]);
        }
    }

    return EXIT_SUCCESS;
}
