#ifndef EMUCXL_LIB_H
#define EMUCXL_LIB_H

#include <stddef.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include "emucxl_mmap.h"

void emucxl_init();
void* emucxl_alloc(size_t size, int node);
void emucxl_free(void* ptr, size_t size);
void* emucxl_resize(void* ptr, int node, size_t oldsize, size_t newsize);
void* emucxl_migrate(void* ptr, int newnode, size_t size);
void emucxl_exit();

extern const char* dev_file;

#define LOCAL_MEMORY 0
#define REMOTE_MEMORY 1
#define PAGE_SIZE 4096

#endif /* MEMORY_H */
