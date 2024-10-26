// creating malloc and free from scratch

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MEM_SPACE 1024*1024 // 1 mb of space

char memory_pool[MEM_SPACE];
size_t current_offset = 0;

// bump alloc function
void* bump_alloc(size_t size){
	if (current_offset + size > MEM_SPACE){
		return NULL;
	}

	void* ptr = &memory_pool[current_offset];

	current_offset += size;

	return ptr;
}

//reset function
void reset_bump(){
	current_offset = 0;
}

//main
int main() {
    // Allocate memory
    int* array1 = (int*)bump_alloc(10 * sizeof(int)); // Allocate 10 integers
    if (array1 == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Use the allocated memory
    for (int i = 0; i < 10; i++) {
        array1[i] = i;
        printf("%d ", array1[i]);
    }
    printf("\n");

    // Allocate another block
    char* array2 = (char*)bump_alloc(20 * sizeof(char)); // Allocate 20 chars
    if (array2 == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Reset the memory pool
    reset_bump();

    return 0;
}

