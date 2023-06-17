#ifndef MEMCXLIB_LIB_H
#define MEMCXLIB_LIB_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include "memcxlib_mmap.h"

void memcxlib_init();
void* memcxlib_alloc(size_t size, int node);
void memcxlib_free(void* ptr, size_t size);
void* memcxlib_resize(void* ptr, int node, size_t oldsize, size_t newsize);
void* memcxlib_migrate(void* ptr, int newnode, size_t size);
void memcxlib_exit();

/* 
* Extra API
*/

// Read data from memory to buffer -> start_index is the index of the first byte to be read
bool memcxlib_read(void* ptr, int start_index, void* buffer, size_t size);

// Write data from buffer to memory -> start_index is the index of the first byte to be written
bool memcxlib_write(void* ptr, int start_index, void* buffer, size_t size);

// Memset 
void* memcxlib_memset(void* ptr, int start_index, int value, size_t size);

// Memcpy
void* memcxlib_memcpy(void* dest, const void* src, size_t size);

// Memmove
void* memcxlib_memmove(void* dest, const void* src, size_t size);

// Check if the memory address is local or remote
bool memcxlib_is_local(void* ptr);

// Get the numa node of the memory address
int memcxlib_get_numa_node(void* ptr);

// Get the size of the allocated memory
size_t memcxlib_get_size(void* ptr);

// Get the size of the allocated memory on a specific numa node
size_t memcxlib_get_size_of_allocated_memory(int numa_node);


extern const char* dev_file;

#define LOCAL_MEMORY 0
#define REMOTE_MEMORY 1
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