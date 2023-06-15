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
    unsigned char *p_map;

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
    sleep(10);
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

    // free
    memcxlib_free((void*)p_map, PAGE_SIZE);
    printf("hi\n\n");
    memcxlib_exit();
    printf("hi2\n\n");
    return 0;
}