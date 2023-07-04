#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "emucxl_lib.h"

int main(int argc , char *argv[])
{
    //int fd;
    int i;
    unsigned char *p_map, *p_map2;

    emucxl_init();
    // allocate
    p_map = (unsigned char *)emucxl_alloc(PAGE_SIZE, LOCAL_MEMORY);
    if(p_map == NULL)
    {
        printf("mmap error\n");
        exit(1);
    }
    printf("pmap address in user space memory : %p\n", p_map);
    printf("Read data \n");
    for(i = 0; i < 10; i++)
        printf("%d ", p_map[i]);
    
    printf("\n Write data \n");
    for(i = 0; i < 10; i++)
        p_map[i] = i;
    printf("Migrate\n");
    p_map = (unsigned char *)emucxl_migrate((void*)p_map, REMOTE_MEMORY, PAGE_SIZE);
    printf("pmap address in user space memory : %p\n", p_map);
    for(i = 0; i < 10; i++)
        printf("%d ", p_map[i]);

    printf("\n Write data \n");
    for(i = 0; i < 10; i++)
        p_map[i] = i*2;
    printf("Resize \n");

    // resize
    p_map = (unsigned char *)emucxl_resize((void*)p_map, LOCAL_MEMORY, PAGE_SIZE, PAGE_SIZE);
    printf("pmap address in user space memory : %p\n", p_map);
    for(i = 0; i < 10; i++)
        printf("%d ", p_map[i]);
    printf("Alloc Remote\n");

    p_map2 = (unsigned char *)emucxl_alloc(PAGE_SIZE, REMOTE_MEMORY);

    printf("pmap2 address in user space memory : %p\n", p_map2);
    printf("Size of allocated memory on LOCAL_MEMORY : %ld\n", emucxl_stats(LOCAL_MEMORY));
    printf("Size of allocated memory on REMOTE_MEMORY : %ld\n", emucxl_stats(REMOTE_MEMORY));
    printf("Size of allocated memory of address %p : %ld\n", p_map, emucxl_get_size((void*)p_map));
    printf("NUMA NODE of address %p : %d\n", p_map2, emucxl_get_numa_node((void*)p_map2));


    printf("IS LOCAL of address %p : %d\n", p_map, emucxl_is_local((void*)p_map));
    printf("IS LOCAL of address %p : %d\n", p_map2, emucxl_is_local((void*)p_map2));
    // free
    emucxl_free((void*)p_map, PAGE_SIZE);
    emucxl_free((void*)p_map2, PAGE_SIZE);

    // Dynamically allocate memory for the Key_value object
    Key_value *kv = emucxl_alloc(sizeof(Key_value), LOCAL_MEMORY);
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
    emucxl_free(kv, sizeof(Key_value));
    emucxl_exit();
    printf("Exiting from direct app\n\n");
    return 0;
}