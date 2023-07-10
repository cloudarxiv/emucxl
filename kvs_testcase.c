#include "kvs.h"
#include <stdio.h>

#define LOCAL_MAX_SIZE 100
#define REMOTE_MAX_SIZE 1000

void testcase(int policy)
{
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

    // Perform 2000 random get operations
    for (int i = 0; i < 2000; i++) {
        int num = (rand() % 1000 + 1);
        char key[10];
        sprintf(key, "key%d", num);
        const char* value = kv_store_get(&store, key);
        if (value == NULL) {
            printf("Key %s not found\n", key);
        }
    }
    // Now delete 500 key-value pairs {key501, value501}, {key502, value502}, ..., {key1000, value1000}
    for (int i = 501; i <= 1000; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        kv_store_delete(&store, key);
    }

    // Perform 2000 random get operations
    for (int i = 0; i < 2000; i++) {
        int num = (rand() % 500 + 1);
        char key[10];
        sprintf(key, "key%d", num);
        const char* value = kv_store_get(&store, key);
        if (value == NULL) {
            printf("Key %s not found\n", key);
        }
    }

    // Now delete 500 key-value pairs {key1, value1}, {key2, value2}, ..., {key500, value500}
    for (int i = 0; i < 500; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        kv_store_delete(&store, key);
    }

    // Print the count of fetches from local and remote
    printf("Local fetch count: %d\n", store.local_fetch_count);
    printf("Remote fetch count: %d\n", store.remote_fetch_count);

    // Destroy the kv_store
    kv_store_destroy(&store);
    return;
}

int main() {
    srand(10); /* set seed for random number generation */
    testcase(FETCH_FROM_REMOTE_TO_LOCAL);
    testcase(NO_FETCH);
    return 0;
}
