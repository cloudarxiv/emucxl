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
data_t **data;


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

		data = dataAlloc();
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
	if (reference_count <= 0) {
		close(fd);
		dataFree(data);
	}
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
	addItem(data, p_map, size, node);
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
	deleteItem(data, ptr);
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

/*
* EXTRA API
*/

// Read data from memory to buffer
bool memcxlib_read(void* ptr, int start_index, void* buffer, size_t size)
{
 // Check for null pointers or zero size
    if (ptr == NULL || buffer == NULL || size == 0) {
        // Handle error appropriately
        return 0;
    }

    // Cast the address and buffer pointers to char* for byte-level manipulation
    char* addr = (char*)ptr;
    char* buf = (char*)buffer;

    // Read 'size' number of bytes from 'address' into 'buffer'
    for (size_t i = start_index; i < size; i++) {
        buf[i] = addr[i];
    }

	return 1;
}

// Write data from buffer to memory

bool memcxlib_write(void* ptr, int start_index, void* buffer, size_t size)
{
	 // Check for null pointers or zero size
	if (ptr == NULL || buffer == NULL || size == 0) {
		// Handle error appropriately
		return 0;
	}

	// Cast the address and buffer pointers to char* for byte-level manipulation
	char* addr = (char*)ptr;
	char* buf = (char*)buffer;

	// Write 'size' number of bytes from 'buffer' into 'address'
	for (size_t i = start_index; i < size; i++) {
		addr[i] = buf[i];
	}

	return 1;
}

// MEMSET
// OK memset only works for setting 0 or -1
void* memcxlib_memset(void* ptr, int start_index, int value, size_t size)
{
	 // Check for null pointers or zero size
	if (ptr == NULL || size == 0) {
		// Handle error appropriately
		return NULL;
	}

	// Cast the address and buffer pointers to char* for byte-level manipulation
	char* addr = (char*)ptr;

	// Write 'size' number of bytes from 'buffer' into 'address'
	for (size_t i = start_index; i < size; i++) {
		addr[i] = value;
	}

	return ptr;
}

// MEMCPY
void* memcxlib_memcpy(void* dest, const void* src, size_t size) {
    // Check for null pointers or zero size
    if (dest == NULL || src == NULL || size == 0) {
        return NULL;
    }

    // Cast the destination and source pointers to char* for byte-level manipulation
    char* dst = (char*)dest;
    const char* s = (const char*)src;

    // Copy 'size' number of bytes from 'src' to 'dest'
    for (size_t i = 0; i < size; i++) {
        dst[i] = s[i];
    }
	return dest;
}

void* memcxlib_memmove(void* dest, const void* src, size_t size) {
    // Check for null pointers or zero size
    if (dest == NULL || src == NULL || size == 0) {
        return NULL;
    }

	char *csrc = (char *)src; 
	char *cdest = (char *)dest; 
	
	// Create a temporary array to hold data of src 
	char *temp = (char *)malloc(size * sizeof(char));
	
	// Copy data from csrc[] to temp[] 
	for (int i=0; i< size; i++)
		temp[i] = csrc[i]; 
	
	// Copy data from temp[] to cdest[] 
	for (int i=0; i < size; i++)
		cdest[i] = temp[i]; 
	
	// Delete dynamically allocated memory
	free(temp);
    
}

data_t **dataAlloc(void)
{
	return malloc(sizeof(data_t)); // TODO: check if this is correct or not malloc(sizeof(data_t*));
}

void dataFree(data_t **data)
{
	free(data); // TODO: check if this is correct or not
}

void* getItem(data_t **data, void* address)
{
	data_t *temp = *data;
	while(temp != NULL)
	{
		if(temp->address == address)
		{
			return temp->address;
		}
		temp = temp->next;
	}
	return NULL;
}

void deleteItem(data_t **data, void* address)
{
	data_t *temp = *data;
	data_t *prev = NULL;
	while(temp != NULL)
	{
		if(temp->address == address)
		{
			if(prev == NULL)
			{
				*data = temp->next;
			}
			else
			{
				prev->next = temp->next;
			}
			free(temp);
			return;
		}
		prev = temp;
		temp = temp->next;
	}
}


void addItem(data_t **data, void* address, int size, int numa_node)
{
	deleteItem(data, address);
	data_t *temp = malloc(sizeof(data_t));
	temp->address = address;
	temp->size = size;
	temp->numa_node = numa_node;
	temp->next = *data;
	*data = temp;
}


size_t getSize(data_t **data, void* address)
{
	data_t *temp = *data;
	while(temp != NULL)
	{
		if(temp->address == address)
		{
			return temp->size;
		}
		temp = temp->next;
	}
	return 0;
}

int getNumaNode(data_t **data, void* address)
{
	data_t *temp = *data;
	while(temp != NULL)
	{
		if(temp->address == address)
		{
			return temp->numa_node;
		}
		temp = temp->next;
	}
	return -1;
}

int getSizeOfAllocatedMemory(data_t **data, int numa_node)
{
	data_t *temp = *data;
	int size = 0;
	while(temp != NULL)
	{
		if(temp->numa_node == numa_node)
		{
			size += temp->size;
		}
		temp = temp->next;
	}
	return size;
}


// Check if the memory address is local or remote
bool memcxlib_is_local(void* ptr)
{
	// Check for null pointers or zero size
	if (ptr == NULL) {
		// Handle error appropriately
		return 0;
	}

	int numa_node = memcxlib_get_numa_node(ptr);
	if(numa_node == 0)
	{
		return 1;
	}
	return 0;
}

// Get the numa node of the memory address
int memcxlib_get_numa_node(void* ptr)
{
	// Check for null pointers or zero size
	if (ptr == NULL) {
		// Handle error appropriately
		return -1;
	}

	int numa_node = getNumaNode(data, ptr);
	if(numa_node == -1)
	{
		return -1;
	}
	return numa_node;
}

// Get the size of the memory address

size_t memcxlib_get_size(void* ptr)
{
	// Check for null pointers or zero size
	if (ptr == NULL) {
		// Handle error appropriately
		return 0;
	}

	size_t size = getSize(data, ptr);
	if(size == 0)
	{
		return 0;
	}
	return size;
}

// Get the size of the allocated memory on the given numa node

size_t memcxlib_get_size_of_allocated_memory(int numa_node)
{
	// Check for null pointers or zero size
	if (numa_node < 0) {
		// Handle error appropriately
		return 0;
	}

	size_t size = getSizeOfAllocatedMemory(data, numa_node);
	if(size == 0)
	{
		return 0;
	}
	return size;
}