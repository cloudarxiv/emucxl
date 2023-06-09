#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "emucxl_lib.h"
int fd;
const char* dev_file = "/dev/emucxl";
void emucxl_init()
{
    emucxl_arg_t q;
    fd = open(dev_file, O_RDWR);
	if(fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

	if (ioctl(fd, EMUCXL_INIT, &q) < 0)
	{
		perror("emucxl ioctl allocate");
	}
	else
	{
		printf("DEBUG: return value(0 means success) : %d\n", q.return_value);
	}
}

void emucxl_exit()
{
	if (ioctl(fd, EMUCXL_EXIT) < 0)
	{
		perror("emucxl ioctl exit");
	}
	close(fd);
}

void* emucxl_alloc(size_t size, int node)
{
	unsigned char *p_map;
	emucxl_lib_t q;
	q.size = size;
	q.numa_node = node;
	if (ioctl(fd, EMUCXL_ALLOC, &q) == -1)
	{
		perror("emucxl ioctl allocate");
	}

    p_map = (unsigned char *)mmap(0, size, PROT_READ | PROT_WRITE,
            MAP_SHARED, fd, node * PAGE_SIZE); // offset should be in multiple of page size
    if(p_map == MAP_FAILED) {
        printf("mmap fail\n");
        munmap(p_map, size);
		return NULL;
    }
	return (void*)p_map;
}

void emucxl_free(void* ptr, size_t size)
{
	munmap((unsigned char*)ptr, size);
	printf("hi2\n\n");
	if (ioctl(fd, EMUCXL_FREE) < 0)
	{
		perror("emucxl free allocate");
	}
	printf("hi32\n\n");
}

void* emucxl_resize(void* ptr, int node, size_t oldsize, size_t newsize)
{
	void* p_map;
	p_map = emucxl_alloc(newsize, node);
	// copy data
	//memcpy(p_map, ptr, oldsize); // memmove is better store it in a temp buffer	and then copy it back. memcpy creates issue
								// if the memory regions overlap.
	emucxl_free(ptr, oldsize); // issue data should not be changed
	return p_map;
}

void* emucxl_migrate(void* ptr, int newnode, size_t size)
{
	void* p_map;
	p_map = emucxl_alloc(size, newnode);
	// copy data
	//memcpy(p_map, ptr, size);
	emucxl_free(ptr, size);
	return p_map;
}