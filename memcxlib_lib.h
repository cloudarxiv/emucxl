#ifndef MEMCXLIB_LIB_H
#define MEMCXLIB_LIB_H

#include <stddef.h>
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

extern const char* dev_file;

#define LOCAL_MEMORY 0
#define REMOTE_MEMORY 1
#define PAGE_SIZE 4096

#endif /* MEMORY_H */