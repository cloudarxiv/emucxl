#ifndef EMUCXL_LIB_H
#define EMUCXL_LIB_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include "emucxl_kernel.h"

void emucxl_init();
void* emucxl_alloc(size_t size, int node);
void emucxl_free(void* ptr, size_t size);
void* emucxl_resize(void* ptr, int node, size_t oldsize, size_t newsize);
void* emucxl_migrate(void* ptr, int newnode, size_t size);
void emucxl_exit();

/* 
* Extra API
*/

// Read data from memory to buffer -> start_index is the index of the first byte to be read
bool emucxl_read(void* ptr, int start_index, void* buffer, size_t size);

// Write data from buffer to memory -> start_index is the index of the first byte to be written
bool emucxl_write(void* ptr, int start_index, void* buffer, size_t size);

// Memset 
void* emucxl_memset(void* ptr, int start_index, int value, size_t size);

// Memcpy
void* emucxl_memcpy(void* dest, const void* src, size_t size);

// Memmove
void* emucxl_memmove(void* dest, const void* src, size_t size);

// Check if the memory address is local or remote
bool emucxl_is_local(void* ptr);

// Get the numa node of the memory address
int emucxl_get_numa_node(void* ptr);

// Get the size of the allocated memory
size_t emucxl_get_size(void* ptr);

// Get the size of the allocated memory on a specific numa node
size_t emucxl_get_size_of_allocated_memory(int numa_node);


extern const char* dev_file;

#define LOCAL_MEMORY 0
#define REMOTE_MEMORY 0
#define PAGE_SIZE 4096

typedef struct data
{
    void* address;
    int size;
    int numa_node; // 0 for local memory, 1 for remote memory

    struct data* next;
} data_t;

data_t **dataAlloc(void);
void dataFree(data_t **data);

void* getItem(data_t **data, void* address);

void deleteItem(data_t **data, void* address);

void addItem(data_t **data, void* address, int size, int numa_node);

size_t getSize(data_t **data, void* address);

int getNumaNode(data_t **data, void* address);

int getSizeOfAllocatedMemory(data_t **data, int numa_node);



// Structure definition for Key_value
typedef struct Key_value {
    char key[50];
    int value;
    struct Key_value *next;
} Key_value;



#endif /* MEMORY_H */