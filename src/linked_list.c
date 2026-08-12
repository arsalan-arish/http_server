#include <corecrt_search.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <types.h>
#include <linked_list.h>


list List() {

    return (list) {
        .first = NULL,
        .last = NULL,
        .len = 0,
        .error = 0,
    };
}


node* _create_node(u64 data_len, void** data_buffer) {
    node* new = malloc(sizeof(node));
    if (new == NULL) return new;
    *data_buffer = malloc(data_len);
    new->data = data_buffer;

    return new;
}

void* append(list* list, u64 data_len) {
    // create node & data buffer
    void* data_buffer;
    node* new = _create_node(data_len, &data_buffer);
    if (new == NULL) list->error = INDEX_OUT_OF_RANGE; return NULL;
    new->next = NULL;
    // Attach the node
    if (list->len == 0) {
        list->first = new;
    } else {
        list->last->next = new;
        new->prev = list->last;
    }
    list->last = new;
    // update list metadata
    list->len++;

    return data_buffer;
}

void* prepend(list* list, u64 data_len) {
    // create node & data buffer
    void* data_buffer;
    node* new = _create_node(data_len, &data_buffer);
    if (new == NULL) list->error = INDEX_OUT_OF_RANGE; return NULL;
    new->prev = NULL;
    // Attach the node
    if (list->len == 0) {
        list->last = new;
    } else {
        new->next = list->first;
        list->first->prev = new;
    }
    list->first = new;
    // update list metadata
    list->len++;

    return data_buffer;
}

void* insert(list* list, u64 data_len, i64 index, bool replace) {
    // Check if index out of range
    if (index > 0) {
        if (!(index <= list->len)) list->error = INDEX_OUT_OF_RANGE; return NULL;
    } else {
        if (llabs(index+1) <= list->len) list->error = INDEX_OUT_OF_RANGE; return NULL;
    }

    // Iterate over list
    node* currentNode;
    if (index >= 0) {
        // Traverse the list
        currentNode = list->first;
        for (u64 i = 0; i < index; i++) {
            currentNode = currentNode->next;
        }
    } else {
        // Traverse the list from the end
        currentNode = list->last;
        for (i64 i = -1; i > index; i--) {
            currentNode = currentNode->prev;
        }
    } 
    
    void* data_buffer;
    node* new = _create_node(data_len, &data_buffer);
    if (new == NULL) list->error = INDEX_OUT_OF_RANGE; return NULL;

    // Override currentNode / push it forward depending on 'replace' parameter
    if (replace) {
        new->next = currentNode->next;
        new->prev = currentNode->prev;
        currentNode->prev->next = new;
        currentNode->next->prev = new;
    } else {
        new->next = currentNode;
        new->prev = currentNode->prev;
        currentNode->prev = new;
    }

    // update list metadata
    list->len++;

    return data_buffer;
}

void pop(list* list) {
    // Rewire
    node* toRemove = list->last;
    list->last = toRemove->prev;
    toRemove->prev->next = NULL;
    // Free
    free(toRemove->data);
    free(toRemove);
    // update metadata
    list->len--;
}

void remove(list* list, void* data) {
// TODO
}

void remove_index(list* list, i64 index) {
// TODO
}

void* get(list* list, i64 index) {
    node* currentNode;
    if (index >= 0) {
        // Traverse the list
        currentNode = list->first;
        for (u64 i = 0; i < index; i++) {
            currentNode = currentNode->next;
        }
    } else {
        // Traverse the list backwards
        currentNode = list->last;
        for (i64 i = -1; i > index; i--) {
            currentNode = currentNode->prev;
        }
    }
    return currentNode->data;
}

u64 search(list* list, byte* data, u64 data_len) {
    /* O(n) Linear Search */
    node* currentNode = list->first;
    for (u64 i = 0; i < list->len; i++) {
        if (!memcmp(currentNode->data, data, data_len)) return i;
        currentNode = currentNode->next;
    }
    return -1;
}

i64 get_negative_index(list* list, u64 positive_index) {
    return -(i64) (list->len - positive_index);
}


void free_list(list* list) {
    // Free all the nodes from the heap
    node* currentNode = list->first;
    node* nextNode;
    for (u64 i = 0; i < list->len; i++) {
        nextNode = currentNode->next;
        free(currentNode->data);
        free(currentNode);
        currentNode = nextNode;
    }
}