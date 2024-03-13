#include "kvs.h"
#include <stdio.h>

#define LOCAL_MAX_SIZE 3
#define REMOTE_MAX_SIZE 10

int main() {
    kv_store store;

    kv_store_init(&store, NO_FETCH, LOCAL_MAX_SIZE, REMOTE_MAX_SIZE);
    // kv_store_init(&store, FETCH_FROM_REMOTE_TO_LOCAL, LOCAL_MAX_SIZE, REMOTE_MAX_SIZE);

    kv_store_put(&store, "key1", "value1");

    kv_store_put(&store, "key2", "value2");

    kv_store_put(&store, "key3", "value3");

    // Retrieve values
    const char* value1 = kv_store_get(&store, "key1");
    const char* value2 = kv_store_get(&store, "key2");
    const char* value3 = kv_store_get(&store, "key3");

    printf("Value1: %s\n", value1);
    printf("Value2: %s\n", value2);
    printf("Value3: %s\n", value3);
    printf("Local Size: %u\n", store.local_size);
    printf("Remote Size: %u\n", store.remote_size);

    // Delete key2
    kv_store_delete(&store, "key2");

    // Retrieve value2 (shouu return NULL)
    const char* deletedValue = kv_store_get(&store, "key2" );
    printf("Deleted Value: %s\n", deletedValue == NULL ? "NULL" : deletedValue);

    printf("Local Size: %u\n", store.local_size);
    printf("Remote Size: %u\n", store.remote_size);

    const char* rm_l = kv_store_get(&store, "key1");

    printf("Remote Value: %s\n", rm_l);
    printf("Local Size: %u\n", store.local_size);
    printf("Remote Size: %u\n", store.remote_size);

    kv_store_put(&store, "key4", "value4");

    printf("Local Size: %u\n", store.local_size);
    printf("Remote Size: %u\n", store.remote_size);

    kv_store_put(&store, "key5", "value5");

    const char* rm_r = kv_store_get(&store, "key3");

    printf("Remote Value: %s\n", rm_r);
    printf("Local Size: %u\n", store.local_size);
    printf("Remote Size: %u\n", store.remote_size);

    // print the kvs
    print_kv_store(&store);

    // Destroy the store and free memory
    kv_store_destroy(&store);

    return 0;
}
