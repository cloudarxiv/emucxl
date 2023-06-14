#ifndef EMUCXL_MMAP_H
#define EMUCXL_MMAP_H

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
// } emucxl_arg_t;

// typedef struct
// {
// 	int* address;
// 	int index;
// 	int value;
// } emucxl_memory_t;

typedef struct 
{ 
    int return_value;
} emucxl_arg_t;

typedef struct
{
    int size;
    int numa_node;
} emucxl_lib_t;


#define EMUCXL_INIT _IOW('e', 1, emucxl_arg_t *)
#define EMUCXL_EXIT _IO('e', 2)
#define EMUCXL_FREE _IO('e', 4)
#define EMUCXL_ALLOC _IOR('e', 3, emucxl_lib_t *)


#define FIRST_MINOR 0
#define MINOR_CNT 1
#define LOCAL_MEMORY 0
#define REMOTE_MEMORY 1

#endif