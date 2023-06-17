#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "memcxlib_lib.h"

int main(int argc , char *argv[])
{
    //int fd;
    int i;
    unsigned char *p_map, *p_map2;

    memcxlib_init();
    // allocate
    sleep(10);
    p_map = (unsigned char *)memcxlib_alloc(PAGE_SIZE, LOCAL_MEMORY);
    printf("pmap address in user space memory : %p\n", p_map);

    printf("\n Read data \n");
    for(i = 0; i < 10; i++)
        printf("%d ", p_map[i]);
    
    printf("\n Write data \n");
    for(i = 0; i < 10; i++)
        p_map[i] = i;
    printf("\n\n");
    // sleep(10);
    // migrate 
    p_map = (unsigned char *)memcxlib_migrate((void*)p_map, REMOTE_MEMORY, PAGE_SIZE);
    printf("pmap address in user space memory : %p\n", p_map);
    for(i = 0; i < 10; i++)
        printf("%d ", p_map[i]);
    printf("\n\n");

    printf("\n Write data \n");
    for(i = 0; i < 10; i++)
        p_map[i] = i*2;
    printf("\n\n");

    // resize
    p_map = (unsigned char *)memcxlib_resize((void*)p_map, LOCAL_MEMORY, PAGE_SIZE, PAGE_SIZE);
    printf("pmap address in user space memory : %p\n", p_map);
    for(i = 0; i < 10; i++)
        printf("%d ", p_map[i]);
    printf("\n\n");

    p_map2 = (unsigned char *)memcxlib_alloc(PAGE_SIZE, REMOTE_MEMORY);

    printf("pmap2 address in user space memory : %p\n", p_map2);

    printf("Size of allocated memory on LOCAL_MEMORY : %ld\n", memcxlib_get_size_of_allocated_memory(LOCAL_MEMORY));

    printf("Size of allocated memory on REMOTE_MEMORY : %ld\n", memcxlib_get_size_of_allocated_memory(REMOTE_MEMORY));

    printf("Size of allocated memory of address %p : %ld\n", p_map, memcxlib_get_size((void*)p_map));

    printf("NUMA NODE of address %p : %d\n", p_map2, memcxlib_get_numa_node((void*)p_map2));


    printf("IS LOCAL of address %p : %d\n", p_map, memcxlib_is_local((void*)p_map));

    printf("IS LOCAL of address %p : %d\n", p_map2, memcxlib_is_local((void*)p_map2));
    // free
    memcxlib_free((void*)p_map, PAGE_SIZE);

    memcxlib_free((void*)p_map2, PAGE_SIZE);
    printf("hi\n\n");

    // Dynamically allocate memory for the Key_value object
    Key_value *kv = memcxlib_alloc(sizeof(Key_value), LOCAL_MEMORY);
    if (kv == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    // Initialize the Key_value object
    strncpy(kv->key, "Example Key", sizeof(kv->key) - 1);
    kv->value = 42;
    kv->next = NULL;

    // Print the Key_value's information
    printf("Key: %s\n", kv->key);
    printf("Value: %d\n", kv->value);
    printf("Next: %p\n", kv->next);

    // Free the dynamically allocated memory
    memcxlib_free(kv, sizeof(Key_value));


    memcxlib_exit();
    printf("hi2\n\n");
    return 0;
}