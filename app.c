#include "memory_manage.h"

#define NUM_ALLOCS 1000
#define MAX_SIZE 10240
#define MAX_ITERATIONS 10000

void random_alloc_free_test() {
    srand((unsigned int)time(NULL));
    
    void* pointers[NUM_ALLOCS] = {NULL};
    int realloc_t=0;
    int new_size=0;
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        int index = rand() % NUM_ALLOCS;
        if (pointers[index] == NULL) {
            // Allocate memory
            size_t size = (size_t)(rand() % MAX_SIZE) + 1;
            pointers[index] = malloc(size);
            if (pointers[index] != NULL) {
                printf("Allocated memory of size %zu at address %p\n", size, pointers[index]);
                /*here if you want to use reallocation part:
                //printf("do you want to reallocate any additional size press 1 if yes and 0 if no:\n");
                //scanf(" %i",&realloc_t);
                //if(1==realloc_t){
                	//printf("enter size you want new:\n");
                	//scanf("%i",&new_size);
                	//pointers[index]=realloc(pointers[index],new_size);
                
                //}else{
                	//printf("okay no reallocation\n");
                //}*/
            } else {
                fprintf(stderr, "Allocation failed for size %zu\n", size);
            }
        } else {
            // Free memory
            printf("Freeing memory at address %p\n", pointers[index]);
            free(pointers[index]);
            pointers[index] = NULL;
        }
    }
    
    // Free remaining allocated memory
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        if (pointers[i] != NULL) {
            printf("Freeing remaining memory at address %p\n", pointers[i]);
            free(pointers[i]);
            pointers[i] = NULL;
        }
    }
}

int main() {
    printf("Starting random allocation and deallocation test...\n");
    random_alloc_free_test();
    printf("Test complete.\n");
    return 0;
}
