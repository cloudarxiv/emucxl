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
    unsigned char *p_map;

    // fd = open("/dev/emucxl", O_RDWR);
    // if(fd < 0) {
    //     perror("open");
    //     exit(EXIT_FAILURE);
    // }

    // p_map = (unsigned char *)mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE,
    //         MAP_SHARED, fd, 0);
    // if(p_map == MAP_FAILED) {
    //     printf("mmap fail\n");
    //     goto here;
    // }
    emucxl_init();
    // allocate
    p_map = (unsigned char *)emucxl_alloc(PAGE_SIZE, LOCAL_MEMORY);
    printf("pmap address in user space memory : %p\n", p_map);
    for(i = 0; i < 10; i++)
        printf("%d ", p_map[i]);
    printf("\n\n");

    // // migrate 
    // p_map = (unsigned char *)emucxl_migrate((void*)p_map, REMOTE_MEMORY, PAGE_SIZE);
    // printf("pmap address in user space memory : %p\n", p_map);
    // for(i = 0; i < 10; i++)
    //     printf("%d ", p_map[i]);
    // printf("\n\n");

    // // resize
    // p_map = (unsigned char *)emucxl_resize((void*)p_map, REMOTE_MEMORY, PAGE_SIZE, PAGE_SIZE);
    // printf("pmap address in user space memory : %p\n", p_map);
    // for(i = 0; i < 10; i++)
    //     printf("%d ", p_map[i]);
    // printf("\n\n");

    // free
    emucxl_free((void*)p_map, PAGE_SIZE);
    printf("hi\n\n");
    emucxl_exit();
    printf("hi2\n\n");
    return 0;
}