#ifndef EMUCXL_KV_H
#define EMUCXL_KV_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "emucxl_lib.h"

#define MAX_KEY_LEN 100
#define MAX_VALUE_LEN 100

typedef struct {
    char key[MAX_KEY_LEN + 1]; // +1 for '\0'
    char value[MAX_VALUE_LEN + 1]; // +1 for '\0'
} emucxl_kv_pair;

typedef struct Node {
    emucxl_kv_pair* kv_pair;
    struct Node* prev;
    struct Node* next;
} emucxl_kv_node;

#define LOCAL_MAX_SIZE 2
#define REMOTE_MAX_SIZE 10
#define FETCH_FROM_REMOTE_TO_LOCAL 1 // 1: fetch, 0: no fetch
#define NO_FETCH 0

typedef struct {
    size_t local_size;
    size_t local_max_size;
    size_t remote_size;
    size_t remote_max_size;
    emucxl_kv_node* local_head;
    emucxl_kv_node* local_tail;
    emucxl_kv_node* remote_head;
    emucxl_kv_node* remote_tail;
} emucxl_kv_store;

void emucxl_kv_store_init(emucxl_kv_store* kvs);
void emucxl_kv_store_destroy(emucxl_kv_store* kvs);

void emucxl_kv_store_put(emucxl_kv_store* kvs, char* key, char* value);
/*
flag = 1: fetch from remote to local if not found in local
*/

const char* emucxl_kv_store_get(emucxl_kv_store* kvs, char* key, int flag); 

void emucxl_kv_store_delete(emucxl_kv_store* kvs, char* key);

void print_kv_store(emucxl_kv_store* kvs);

#endif