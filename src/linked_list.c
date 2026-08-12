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
    if (*data_buffer == NULL) return new;

    new->item.data = data_buffer;
    new->item.data_len = data_len;
    return new;
}

void* append(list* list, u64 data_len) {

    // create node & data buffer
    void* data_buffer;
    node* new = _create_node(data_len, &data_buffer);
    if (new == NULL || data_buffer == NULL) list->error = INDEX_OUT_OF_RANGE; return NULL;
    new->next = NULL;

    // Attach the node
    if (list->len == 0) {
        list->first = new;
        new->prev = NULL;
    } else {
        list->last->next = new;
        new->prev = list->last;
    }
    list->last = new;
    
    // update metadata
    list->len++;

    return data_buffer;
}

void* prepend(list* list, u64 data_len) {
    
    // create node & data buffer
    void* data_buffer;
    node* new = _create_node(data_len, &data_buffer);
    if (new == NULL || data_buffer == NULL) list->error = INDEX_OUT_OF_RANGE; return NULL;
    new->prev = NULL;
    
    // Attach the node
    if (list->len == 0) {
        list->last = new;
        new->next = NULL;
    } else {
        new->next = list->first;
        list->first->prev = new;
    }
    list->first = new;

    // update metadata
    list->len++;

    return data_buffer;
}

void* insert(list* list, u64 data_len, i64 index, bool replace) {
    /* Insert is only allowed within the current range (< len), or equal to len, in which case it will be appended */
    // Check if index out of range.
    if (index >= 0) {
        if (index == list->len) return append(list, data_len);
        if (!(index < list->len)) list->error = INDEX_OUT_OF_RANGE; return NULL;
    } else {
        if (get_inverted_index(list, index) == list->len) return append(list, data_len);
        if (!(llabs(index+1) < list->len)) list->error = INDEX_OUT_OF_RANGE; return NULL;
    }

    index = optimize_index(list, index);

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
    if (new == NULL || data_buffer == NULL) list->error = INDEX_OUT_OF_RANGE; return NULL;

    // TODO: Common; manage the exception if currentNode is the first or last node, or list->len == 0, 1, or 2
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
    // TODO: Common;
    node* toRemove = list->last;
    list->last = toRemove->prev;
    toRemove->prev->next = NULL;
    // Free
    free(toRemove->item.data);
    free(toRemove);
    // update metadata
    list->len--;
}

void remove_value(list* list, void* data, u64 data_len) {
    // Find the node
    // TODO: Common;
    node* currentNode = list->first;
    for (u64 i = 0; i < list->len; i++) {
        if (data_len != currentNode->item.data_len) continue;
        if (!memcmp(currentNode->item.data, data, data_len)) break;
        currentNode = currentNode->next;
    }
    // Unwire the node
    

    // Update metadata
    list->len--;
}

void remove_index(list* list, i64 index) { }

item get(list* list, i64 index) {

    // Check if index out of range
    if (index >= 0) {
        if (!(index < list->len)) list->error = INDEX_OUT_OF_RANGE; return (item) {0};
    } else {
        if (!(llabs(index+1) < list->len)) list->error = INDEX_OUT_OF_RANGE; return (item) {0};
    }

    index = optimize_index(list, index);
    // TODO: Common;
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
    return (item) {
        .data = currentNode->item.data,
        .data_len = currentNode->item.data_len,
    };
}

u64 search(list* list, void* data, u64 data_len) {
    /* O(n) Linear Search */
    // TODO: Common;
    node* currentNode = list->first;
    for (u64 i = 0; i < list->len; i++) {
        if (currentNode->item.data_len != data_len) continue;
        if (!memcmp(currentNode->item.data, data, currentNode->item.data_len)) return i;
        currentNode = currentNode->next;
    }
    return -1;
}

i64 get_inverted_index(list* list, i64 index) {
    return index > 0 ? - (list->len - index) : list->len + index;
}

i64 optimize_index(list* list, i64 index) {
    // Index optimization; if index is closer to the opposite side of the list (2 sides because -ive indexing is supported), invert it so that the list is traversed from the closer side, saving compute & time.
    if ((index > 0 && index > list->len / 2) || (index < -1 && llabs(index) > list->len / 2))
        return get_inverted_index(list, index);   
    return index;
}

void free_list(list* list) {
    // Free all the nodes from the heap
    node* currentNode = list->first;
    node* nextNode;
    for (u64 i = 0; i < list->len; i++) {
        nextNode = currentNode->next;
        free(currentNode->item.data);
        free(currentNode);
        currentNode = nextNode;
    }
}
