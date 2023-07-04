#include "emucxl_kv.h"

void emucxl_kv_store_init(emucxl_kv_store *kvs)
{
    kvs->local_head = NULL;
    kvs->local_tail = NULL;
    kvs->remote_head = NULL;
    kvs->remote_tail = NULL;
    kvs->local_size = 0;
    kvs->remote_size = 0;
    kvs->local_max_size = LOCAL_MAX_SIZE;
    kvs->remote_max_size = REMOTE_MAX_SIZE;
    emucxl_init();
    return;
}

emucxl_kv_pair* emucxl_get_kv_pair(emucxl_kv_store* kvs, char* key, int flag)
{
    // printf("emucxl_get_kv_pair\n");
    // printf("key: %s\n", key);
    // printf("flag: %d\n", flag);
    // Search the local list
    emucxl_kv_node* curr = kvs->local_head;

    while (curr != NULL) {
        if (strcmp(curr->kv_pair->key, key) == 0) {
            // Move the node to the MRU position
            if (curr != kvs->local_head) {
                // Remove the node from its current position
                curr->prev->next = curr->next;
                if (curr->next != NULL) {
                    curr->next->prev = curr->prev;
                } else {
                    kvs->local_tail = curr->prev;
                }

                // Update the node
                curr->prev = NULL;
                curr->next = kvs->local_head;
                kvs->local_head->prev = curr;
                kvs->local_head = curr;
            }

            return curr->kv_pair;
        }
        curr = curr->next;
    }
    // Search the remote list
    curr = kvs->remote_head;

    while (curr != NULL) {
        if (strcmp(curr->kv_pair->key, key) == 0) {

            if(flag == FETCH_FROM_REMOTE_TO_LOCAL)
            {
                // Move the node from remote to local
                // Remove the node from its current position
                if(curr->prev != NULL)
                {
                    curr->prev->next = curr->next;
                }
                if (curr->next != NULL) {
                    curr->next->prev = curr->prev;
                } else {
                    kvs->remote_tail = curr->prev;
                }
                kvs->remote_head = curr->next;
                kvs->remote_size--;
                // put the node to the local list tail if the local list is not full
                // else remove the node from tail and put it to remote list
                if (kvs->local_size == LOCAL_MAX_SIZE )
                {
                    emucxl_kv_node* temp = kvs->local_tail;
                    // Remove the node from local tail
                    kvs->local_tail->prev->next = NULL;
                    kvs->local_tail = kvs->local_tail->prev;
                    kvs->local_size--;
                    // put the node to remote list head
                    // create a new node
                    emucxl_kv_node* new_node = (emucxl_kv_node*)emucxl_alloc(sizeof(emucxl_kv_node), REMOTE_MEMORY);
                    new_node->kv_pair = (emucxl_kv_pair*)emucxl_alloc(sizeof(emucxl_kv_pair), REMOTE_MEMORY);
                    strcpy(new_node->kv_pair->key, temp->kv_pair->key);
                    strcpy(new_node->kv_pair->value, temp->kv_pair->value);
                    new_node->prev = NULL;
                    new_node->next = kvs->remote_head;
                    kvs->remote_head->prev = new_node;
                    kvs->remote_head = new_node;
                    kvs->remote_size++;

                    emucxl_free((void*)temp->kv_pair, sizeof(emucxl_kv_pair));
                    emucxl_free((void*)temp, sizeof(emucxl_kv_node));
                }  
                // put the current node to local list tail
                // create a new node
                emucxl_kv_node* new_node = (emucxl_kv_node*)emucxl_alloc(sizeof(emucxl_kv_node), LOCAL_MEMORY);
                new_node->kv_pair = (emucxl_kv_pair*)emucxl_alloc(sizeof(emucxl_kv_pair), LOCAL_MEMORY);
                strcpy(new_node->kv_pair->key, curr->kv_pair->key);
                strcpy(new_node->kv_pair->value, curr->kv_pair->value);
                new_node->prev = kvs->local_tail;
                new_node->next = NULL;
                kvs->local_tail->next = new_node;
                kvs->local_tail = new_node;
                kvs->local_size++;

                // free the current node
                emucxl_free((void*)curr->kv_pair, sizeof(emucxl_kv_pair));
                emucxl_free((void*)curr, sizeof(emucxl_kv_node));
                curr = new_node;
            }
            return curr->kv_pair;
        }
        curr = curr->next;
    }
    return NULL;
}

const char* emucxl_kv_store_get(emucxl_kv_store* kvs, char* key, int flag) {
    emucxl_kv_pair* kv_pair = emucxl_get_kv_pair(kvs, key, flag);
    if (kv_pair == NULL) {
        return NULL;
    }
    return kv_pair->value;
}


void emucxl_kv_store_put(emucxl_kv_store* kvs, char* key, char* value) {
    // Search the local list
    emucxl_kv_pair* kv_pair = emucxl_get_kv_pair(kvs, key, 0);

    if (kv_pair != NULL) {
        // Update the value
        strcpy(kv_pair->value, value);
        return;
    }

    // Create a new node
    emucxl_kv_node* new_node = (emucxl_kv_node*)emucxl_alloc(sizeof(emucxl_kv_node), LOCAL_MEMORY);
    new_node->kv_pair = (emucxl_kv_pair*)emucxl_alloc(sizeof(emucxl_kv_pair), LOCAL_MEMORY);
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
        emucxl_kv_node* lru_node = kvs->local_tail;
        // create a new node
        emucxl_kv_node* new_node = (emucxl_kv_node*)emucxl_alloc(sizeof(emucxl_kv_node), REMOTE_MEMORY);
        new_node->kv_pair = (emucxl_kv_pair*)emucxl_alloc(sizeof(emucxl_kv_pair), REMOTE_MEMORY);
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
        emucxl_free((void*)lru_node->kv_pair, sizeof(emucxl_kv_pair));
        emucxl_free((void*)lru_node, sizeof(emucxl_kv_node));
        kvs->local_size -= 1;
    }
}

void emucxl_kv_store_delete(emucxl_kv_store* kvs, char* key) {
    // Search the local list
    emucxl_kv_node* curr = kvs->local_head;

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
            emucxl_free((void*)curr->kv_pair, sizeof(emucxl_kv_pair));
            emucxl_free(curr, sizeof(emucxl_kv_node));
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
            free(curr->kv_pair);
            free(curr);
            return;
        }
        curr = curr->next;
    }
}


void emucxl_kv_store_destroy(emucxl_kv_store *kvs)
{
    emucxl_kv_node* node = kvs->local_head;

    while (node != NULL) {
        emucxl_kv_node* next = node->next;
        // free(node);
        emucxl_free((void*)node->kv_pair, sizeof(emucxl_kv_pair));
        emucxl_free((void*)node, sizeof(emucxl_kv_node));
        node = next;
    }

    node = kvs->remote_head;
    while (node != NULL) {
        emucxl_kv_node* next = node->next;
        // free(node);
        emucxl_free((void*)node->kv_pair, sizeof(emucxl_kv_pair));
        emucxl_free((void*)node, sizeof(emucxl_kv_node));
        node = next;
    }
    emucxl_exit();
    return;
}

void print_kv_store(emucxl_kv_store* kvs) {
    printf("Local list: \n");
    emucxl_kv_node* curr = kvs->local_head;
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