#include "emucxl_kv.h"
#include <stdio.h>

int main() {
    emucxl_kv_store store;

    emucxl_kv_store_init(&store);

    emucxl_kv_store_put(&store, "key1", "value1");

    emucxl_kv_store_put(&store, "key2", "value2");

    emucxl_kv_store_put(&store, "key3", "value3");

    // Retrieve values
    const char* value1 = emucxl_kv_store_get(&store, "key1", 0);
    const char* value2 = emucxl_kv_store_get(&store, "key2", 0);
    const char* value3 = emucxl_kv_store_get(&store, "key3", 0);

    printf("Value1: %s\n", value1);
    printf("Value2: %s\n", value2);
    printf("Value3: %s\n", value3);
    printf("Local Size: %ld\n", store.local_size);
    printf("Remote Size: %ld\n", store.remote_size);

    // Delete key2
    emucxl_kv_store_delete(&store, "key2");

    // Retrieve value2 (should return NULL)
    const char* deletedValue = emucxl_kv_store_get(&store, "key2", 0);
    printf("Deleted Value: %s\n", deletedValue == NULL ? "NULL" : deletedValue);

    printf("Local Size: %ld\n", store.local_size);
    printf("Remote Size: %ld\n", store.remote_size);

    const char* rm_l = emucxl_kv_store_get(&store, "key1", 1);

    printf("Remote Value: %s\n", rm_l);
    printf("Local Size: %ld\n", store.local_size);
    printf("Remote Size: %ld\n", store.remote_size);

    emucxl_kv_store_put(&store, "key4", "value4");

     printf("Local Size: %ld\n", store.local_size);
    printf("Remote Size: %ld\n", store.remote_size);

    emucxl_kv_store_put(&store, "key5", "value5");

    const char* rm_r = emucxl_kv_store_get(&store, "key3", 1);

    printf("Remote Value: %s\n", rm_r);
    printf("Local Size: %ld\n", store.local_size);
    printf("Remote Size: %ld\n", store.remote_size);

    // print the kvs
    print_kv_store(&store);

    // Destroy the store and free memory
    emucxl_kv_store_destroy(&store);

    return 0;
}
