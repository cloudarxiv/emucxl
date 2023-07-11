#include "kvs.h"
#include <stdio.h>
#include <stdlib.h>

#define LOCAL_MAX_SIZE 300
#define REMOTE_MAX_SIZE 1000

int main(int argc, char *argv[]){
    int policy = atoi(argv[1]);
    srand(atoi(argv[2])); /* set seed for random number generation */
    if (policy < 0 || policy > 2) {
        printf("Invalid policy\n");
        return 0;
    }
    printf("Testcase with policy %d\n", policy);
    kv_store store;

    kv_store_init(&store, policy, LOCAL_MAX_SIZE, REMOTE_MAX_SIZE);

    // Put 1000 key-value pairs {key1, value1}, {key2, value2}, ..., {key1000, value1000}
    for (int i = 1; i <= 1000; i++) {
        char key[10];
        char value[10];
        sprintf(key, "key%d", i);
        sprintf(value, "value%d", i);
        kv_store_put(&store, key, value);
    }
    int x = atoi(argv[3]);
    if (x == 0)
    {
        // random get requests
        for (int i = 0; i < 50000; i++) {
            int num = (rand() % 1000 + 1);
            char key[10];
            sprintf(key, "key%d", num);
            const char* value = kv_store_get(&store, key);
            if (value == NULL) {
                printf("Key %s not found\n", key);
            }
        }
        printf("Local fetch count:\t %d\n", store.local_fetch_count);
        printf("Remote fetch count:\t %d\n", store.remote_fetch_count);
        printf("Random get requests done\n");
    }
    else
    {
        int k = x * 10;
        // x% object gets 90% get requests
        for(int i = 1; i <= 45000; i++){
            int num = (rand() % k + 1);
            char key[10];
            sprintf(key, "key%d", num);
            const char* value = kv_store_get(&store, key);
            if (value == NULL) {
                printf("Key %s not found\n", key);
            }
        }

        // other 10% get requests
        for (int i = 1; i <= 5000; i++) {
            int num = (rand() % (1000 - k) + (k + 1));
            char key[10];
            sprintf(key, "key%d", num);
            const char* value = kv_store_get(&store, key);
            if (value == NULL) {
                printf("Key %s not found\n", key);
            }
        }
        printf("Local fetch count:\t %d\n", store.local_fetch_count);
        printf("Remote fetch count:\t %d\n", store.remote_fetch_count);
        printf("%d%% object gets 90%% get requests done\n", x);
    }

    kv_store_destroy(&store);

    return 0;
}