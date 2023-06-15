#ifndef MEMCXLIB_MMAP_H
#define MEMCXLIB_MMAP_H

#include <linux/ioctl.h>
#include <linux/types.h>

// typedef struct
// {
// 	int status, dignity, ego;
// } query_arg_t;

// typedef struct
// {
// 	int size;
// 	void* address;
// 	int ret;
// 	int numa_node;
// } memcxlib_arg_t;

// typedef struct
// {
// 	int* address;
// 	int index;
// 	int value;
// } memcxlib_memory_t;

typedef struct 
{ 
    int return_value;
} memcxlib_arg_t;

typedef struct
{
    int size;
    int numa_node;
} memcxlib_lib_t;


#define MEMCXLIB_INIT _IOW('e', 1, memcxlib_arg_t *)
#define MEMCXLIB_EXIT _IO('e', 2)
#define MEMCXLIB_FREE _IO('e', 4)
#define MEMCXLIB_ALLOC _IOR('e', 3, memcxlib_lib_t *)


#define FIRST_MINOR 0
#define MINOR_CNT 1
#define LOCAL_MEMORY 0
#define REMOTE_MEMORY 1

#endif