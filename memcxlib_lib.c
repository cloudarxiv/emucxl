#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "memcxlib_lib.h"
int fd;
const char* dev_file = "/dev/memcxlib";
int reference_count = 0;
void memcxlib_init()
{
    memcxlib_arg_t q;
	if (reference_count == 0)
	{
		fd = open(dev_file, O_RDWR);
		if(fd < 0) {
			perror("open");
			exit(EXIT_FAILURE);
		}
	}
	reference_count++;
	printf("DEBUG: INIT reference count : %d\n", reference_count);

	if (ioctl(fd, MEMCXLIB_INIT, &q) < 0)
	{
		perror("memcxlib ioctl allocate");
	}
	else
	{
		printf("DEBUG: return value(0 means success) : %d\n", q.return_value);
	}
}

void memcxlib_exit()
{
	if (ioctl(fd, MEMCXLIB_EXIT) < 0)
	{
		perror("memcxlib ioctl exit");
	}
	reference_count--;
	printf("DEBUG: EXIT reference count : %d\n", reference_count);
	if (reference_count <= 0) close(fd);
}

void* memcxlib_alloc(size_t size, int node)
{
	unsigned char *p_map;
	#ifdef DEBUG
		memcxlib_lib_t q;
		q.size = size;
		q.numa_node = node;
		if (ioctl(fd, MEMCXLIB_ALLOC, &q) == -1)
		{
			perror("memcxlib ioctl allocate");
		}
	#endif

    p_map = (unsigned char *)mmap(0, size, PROT_READ | PROT_WRITE,
            MAP_SHARED, fd, node * PAGE_SIZE); // offset should be in multiple of page size
    if(p_map == MAP_FAILED) {
        printf("mmap fail\n");
        munmap(p_map, size);
		return NULL;
    }
	return (void*)p_map;
}

void memcxlib_free(void* ptr, size_t size)
{
	munmap((unsigned char*)ptr, size);

	#ifdef DEBUG
		printf("hi2\n\n");
		if (ioctl(fd, MEMCXLIB_FREE) < 0)
		{
			perror("memcxlib free allocate");
		}
		printf("hi32\n\n");
	#endif
}

void* memcxlib_resize(void* ptr, int node, size_t oldsize, size_t newsize)
{
	void* p_map;
	p_map = memcxlib_alloc(newsize, node);
	// copy data
	memmove(p_map, ptr, oldsize); // memmove is better store it in a temp buffer	and then copy it back. memcpy creates issue
								// if the memory regions overlap.
	memcxlib_free(ptr, oldsize); // issue data should not be changed
	return p_map;
}

void* memcxlib_migrate(void* ptr, int newnode, size_t size)
{
	void* p_map;
	p_map = memcxlib_alloc(size, newnode);
	// copy data
	memmove(p_map, ptr, size);
	memcxlib_free(ptr, size);
	return p_map;
}