#include "kvs.h"

void kv_store_init(kv_store *kvs, int global_policy, uint32_t local_max, uint32_t remote_max) {
    kvs->local_head = NULL;
    kvs->local_tail = NULL;
    kvs->remote_head = NULL;
    kvs->remote_tail = NULL;
    kvs->local_size = 0;
    kvs->remote_size = 0;
    kvs->local_max_size = local_max;
    kvs->remote_max_size = remote_max;
    kvs->policy = global_policy;
    kvs->local_fetch_count = 0;
    kvs->remote_fetch_count = 0;
    emucxl_init();
    return;
}

kvs_pair* get_kv_pair(kv_store* kvs, char* key, int ops)
{
    kvs_node* curr = kvs->local_head;

    while (curr != NULL) {
        if (strcmp(curr->kv_pair->key, key) == 0) {
            if (curr != kvs->local_head) {
                curr->prev->next = curr->next;
                if (curr->next != NULL) {
                    curr->next->prev = curr->prev;
                } else {
                    kvs->local_tail = curr->prev;
                }
                curr->prev = NULL;
                curr->next = kvs->local_head;
                kvs->local_head->prev = curr;
                kvs->local_head = curr;
            }
            if(ops == GET)
            {
                kvs->local_fetch_count++;
            }
            return curr->kv_pair;
        }
        curr = curr->next;
    }

    curr = kvs->remote_head;

    while (curr != NULL) {
        if (strcmp(curr->kv_pair->key, key) == 0) {

            if(kvs->policy == FETCH_FROM_REMOTE_TO_LOCAL)
            {
                if (kvs->local_size == kvs->local_max_size )
                {
                    kvs_node* temp = kvs->local_tail;
                    if (kvs->local_tail->prev != NULL)
                        kvs->local_tail->prev->next = NULL;
                    else 
                        kvs->local_head = NULL;

                    kvs->local_tail = kvs->local_tail->prev;
                    kvs->local_size--;
                    kvs_node* new_node = (kvs_node*)emucxl_alloc(sizeof(kvs_node), REMOTE_MEMORY);
                    new_node->kv_pair = (kvs_pair*)emucxl_alloc(sizeof(kvs_pair), REMOTE_MEMORY);
                    strcpy(new_node->kv_pair->key, temp->kv_pair->key);
                    strcpy(new_node->kv_pair->value, temp->kv_pair->value);
                    
                    new_node->prev = NULL;
                    new_node->next = kvs->remote_head;
                    if (kvs->remote_head != NULL)
                          kvs->remote_head->prev = new_node;
                    kvs->remote_head = new_node;
                    kvs->remote_size++;
                    
                    emucxl_free((void*)temp->kv_pair, sizeof(kvs_pair));
                    emucxl_free((void*)temp, sizeof(kvs_node));
                    
                }

                if(curr->prev != NULL)
                {
                    curr->prev->next = curr->next;
                }
                else
                {
                    kvs->remote_head = curr->next;
                }

                if (curr->next != NULL) {
                    curr->next->prev = curr->prev;
                } else {
                    kvs->remote_tail = curr->prev;
                }
                kvs->remote_size--;
                
                kvs_node* new_node = (kvs_node*)emucxl_alloc(sizeof(kvs_node), LOCAL_MEMORY);
                new_node->kv_pair = (kvs_pair*)emucxl_alloc(sizeof(kvs_pair), LOCAL_MEMORY);
                strcpy(new_node->kv_pair->key, curr->kv_pair->key);
                strcpy(new_node->kv_pair->value, curr->kv_pair->value);
                
                new_node->prev = kvs->local_tail;
                new_node->next = NULL;
                if (kvs->local_tail != NULL)
                     kvs->local_tail->next = new_node;
                kvs->local_tail = new_node;
                kvs->local_size++;

                // free the current node
                emucxl_free((void*)curr->kv_pair, sizeof(kvs_pair));
                emucxl_free((void*)curr, sizeof(kvs_node));
                curr = new_node;
            }
            if(ops == GET)
            {
                kvs->remote_fetch_count++;
            }
            return curr->kv_pair;
        }
        curr = curr->next;
    }
    return NULL;
}

const char* kv_store_get(kv_store* kvs, char* key) {
    kvs_pair* kv_pair = get_kv_pair(kvs, key, GET);
    if (kv_pair == NULL) {
        return NULL;
    }
    return kv_pair->value;
}


void kv_store_put(kv_store* kvs, char* key, char* value) {
    // Search the local list
    kvs_pair* kv_pair = get_kv_pair(kvs, key, PUT);

    if (kv_pair != NULL) {
        // Update the value
        strcpy(kv_pair->value, value);
        return;
    }

    // Create a new node
    kvs_node* new_node = (kvs_node*)emucxl_alloc(sizeof(kvs_node), LOCAL_MEMORY);
    new_node->kv_pair = (kvs_pair*)emucxl_alloc(sizeof(kvs_pair), LOCAL_MEMORY);
    strcpy(new_node->kv_pair->key, key);
    strcpy(new_node->kv_pair->value, value);
    new_node->prev = NULL;
    new_node->next = NULL;

    // Insert the node at the front (MRU position)
    if (kvs->local_head == NULL) {
        kvs->local_head = new_node;
        kvs->local_tail = new_node;
    } else {
        new_node->next = kvs->local_head;
        kvs->local_head->prev = new_node;
        kvs->local_head = new_node;
    }

    // Update the size
    kvs->local_size += 1;

    // Evict the LRU node if the size exceeds the limit
    if (kvs->local_size > kvs->local_max_size) {
        kvs_node* lru_node = kvs->local_tail;
        // create a new node
        kvs_node* new_node = (kvs_node*)emucxl_alloc(sizeof(kvs_node), REMOTE_MEMORY);
        new_node->kv_pair = (kvs_pair*)emucxl_alloc(sizeof(kvs_pair), REMOTE_MEMORY);
        strcpy(new_node->kv_pair->key, lru_node->kv_pair->key);
        strcpy(new_node->kv_pair->value, lru_node->kv_pair->value);
        new_node->prev = NULL;
        new_node->next = NULL;

        // Move the evicted node to the evicted list

        if (kvs->remote_head == NULL) {
            kvs->remote_head = new_node;
            kvs->remote_tail = new_node;
        } else {
            new_node->next = kvs->remote_head;
            kvs->remote_head->prev = new_node;
            kvs->remote_head = new_node;
        }
        kvs->remote_size += 1;

        kvs->local_tail = lru_node->prev;
        kvs->local_tail->next = NULL;
        emucxl_free((void*)lru_node->kv_pair, sizeof(kvs_pair));
        emucxl_free((void*)lru_node, sizeof(kvs_node));
        kvs->local_size -= 1;
    }
}

void kv_store_delete(kv_store* kvs, char* key) {
    // Search the local list
    kvs_node* curr = kvs->local_head;

    while (curr != NULL) {
        if (strcmp(curr->kv_pair->key, key) == 0) {
            // Remove the node from its current position
            if (curr->prev != NULL) {
                curr->prev->next = curr->next;
            } else {
                kvs->local_head = curr->next;
            }

            if (curr->next != NULL) {
                curr->next->prev = curr->prev;
            } else {
                kvs->local_tail = curr->prev;
            }

            // Update the size
            kvs->local_size -= 1;

            // Free the node
            // free(curr->kv_pair);
            // free(curr);
            emucxl_free((void*)curr->kv_pair, sizeof(kvs_pair));
            emucxl_free((void*)curr, sizeof(kvs_node));
            return;
        }
        curr = curr->next;
    }

    // Search the remote list
    curr = kvs->remote_head;

    while (curr != NULL) {
        if (strcmp(curr->kv_pair->key, key) == 0) {
            // Remove the node from its current position
            if (curr->prev != NULL) {
                curr->prev->next = curr->next;
            } else {
                kvs->remote_head = curr->next;
            }

            if (curr->next != NULL) {
                curr->next->prev = curr->prev;
            } else {
                kvs->remote_tail = curr->prev;
            }

            // Update the size
            kvs->remote_size -= 1;

            // Free the node
            emucxl_free((void*)curr->kv_pair, sizeof(kvs_pair));
            emucxl_free((void*)curr, sizeof(kvs_node));
            return;
        }
        curr = curr->next;
    }
}


void kv_store_destroy(kv_store *kvs)
{
    kvs_node* node = kvs->local_head;

    while (node != NULL) {
        kvs_node* next = node->next;
        // free(node);
        emucxl_free((void*)node->kv_pair, sizeof(kvs_pair));
        emucxl_free((void*)node, sizeof(kvs_node));
        node = next;
    }

    node = kvs->remote_head;
    while (node != NULL) {
        kvs_node* next = node->next;
        // free(node);
        emucxl_free((void*)node->kv_pair, sizeof(kvs_pair));
        emucxl_free((void*)node, sizeof(kvs_node));
        node = next;
    }
    emucxl_exit();
    return;
}

void print_kv_store(kv_store* kvs) {
    printf("Local list: \n");
    kvs_node* curr = kvs->local_head;
    while (curr != NULL) {
        printf("key: %s, value: %s\n", curr->kv_pair->key, curr->kv_pair->value);
        curr = curr->next;
    }
    printf("Remote list: \n");
    curr = kvs->remote_head;
    while (curr != NULL) {
        printf("key: %s, value: %s\n", curr->kv_pair->key, curr->kv_pair->value);
        curr = curr->next;
    }
    return;
}