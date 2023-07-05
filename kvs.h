#ifndef KV_H
#define KV_H
// #define DEBUG

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "emucxl_lib.h"

#define MAX_KEY_LEN 100
#define MAX_VALUE_LEN 100

typedef struct {
    char key[MAX_KEY_LEN + 1]; // +1 for '\0'
    char value[MAX_VALUE_LEN + 1]; // +1 for '\0'
} kvs_pair;

typedef struct Node {
    kvs_pair* kv_pair;
    struct Node* prev;
    struct Node* next;
} kvs_node;

#define LOCAL_MAX_SIZE 2
#define REMOTE_MAX_SIZE 10
#define FETCH_FROM_REMOTE_TO_LOCAL 1 // 1: fetch, 0: no fetch
#define NO_FETCH 0

typedef struct {
    size_t local_size;
    size_t local_max_size;
    size_t remote_size;
    size_t remote_max_size;
    kvs_node* local_head;
    kvs_node* local_tail;
    kvs_node* remote_head;
    kvs_node* remote_tail;
    int policy; // 1: fetch from remote to local if not found in local, 0: no fetch
} kv_store;

void kv_store_init(kv_store* kvs, int global_policy);
void kv_store_destroy(kv_store* kvs);

void kv_store_put(kv_store* kvs, char* key, char* value);

const char* kv_store_get(kv_store* kvs, char* key); 

void kv_store_delete(kv_store* kvs, char* key);

void print_kv_store(kv_store* kvs);

#endif