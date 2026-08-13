#include <limits.h>
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
    if (index >= 0) {
        if (index == list->len) return append(list, data_len);
    } else {
        if (get_inverted_index(list, index) == list->len) return append(list, data_len);
    }

    node* currentNode = _find_node_by_index(list, index);
    if (list->error == INDEX_OUT_OF_RANGE) return NULL;
    
    void* data_buffer;
    node* new = _create_node(data_len, &data_buffer);
    if (new == NULL || data_buffer == NULL) list->error = INDEX_OUT_OF_RANGE; return NULL;

    // Override currentNode / push it forward depending on 'replace' parameter
    if (replace) {
        new->next = currentNode->next;
        new->prev = currentNode->prev;

        currentNode->prev == NULL ?
                ({list->first = new;})
            :   ({currentNode->prev->next = new;});
        currentNode->next == NULL ?
                ({list->first = new;})
            :   ({currentNode->prev->next = new;});
   } else {
        if (currentNode->prev == NULL) list->first = new; 
        new->next = currentNode;
        new->prev = currentNode->prev;
        currentNode->prev = new;
    }

    // update list metadata
    list->len++;

    return data_buffer;
}

item pop(list* list, bool _return) {
    /* If _return is true, the user will have to manually call free_item() function to free the item once it has been used */

    if (list->len == 0) list->error = POP_FROM_EMPTY_LIST; return (item) {0};
    
    // Rewire
    node* toRemove = list->last;
    _unwire_node(list, toRemove);

    // Free
    item returning_item;
    if (!_return) {
        _free_node(toRemove);
    } else {
        returning_item = toRemove->item;
        free(toRemove); // Manual free because we do not want to free the item.data
    }

    // update metadata
    list->len--;

    return returning_item;
}

void pop_index(list* list, i32 index) {
    node* node = _find_node_by_index(list, index);
    if (list->error == INDEX_OUT_OF_RANGE) return;
    _unwire_node(list, node);
    _free_node(node);
}

void remove_value(list* list, void* data, u64 data_len) {
    // Find the node
    node* currentNode = list->first;
    for (u64 i = 0; i < list->len; i++) {
        if (data_len != currentNode->item.data_len) continue;
        if (!memcmp(currentNode->item.data, data, data_len)) break;
        currentNode = currentNode->next;
    }
    // If value not found
    if (currentNode == NULL) list->error = VALUE_NOT_FOUND; return;

    _unwire_node(list, currentNode);
    _free_node(currentNode);

    // Update metadata
    list->len--;
}

void remove_index(list* list, i64 index) {
    node* node = _find_node_by_index(list, index);
    if (list->error == INDEX_OUT_OF_RANGE) return;

    _unwire_node(list, node);
    _free_node(node);
}

item get(list* list, i64 index) {

    node* currentNode = _find_node_by_index(list, index);
    if (list->error == INDEX_OUT_OF_RANGE) return (item) {0};
    
    return currentNode->item;
}

u64 search(list* list, void* data, u64 data_len) {
    /* O(n) Linear Search */
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

i64 _optimize_index(list* list, i64 index) {
    // Index optimization; if index is closer to the opposite side of the list (2 sides because -ive indexing is supported), invert it so that the list is traversed from the closer side, saving compute & time.
    if ((index > 0 && index > list->len / 2) || (index < -1 && llabs(index) > list->len / 2))
        return get_inverted_index(list, index);   
    return index;
}

void _unwire_node(list* list, node* node) {
    // Unwire the node
    if (node->prev == NULL) {
        list->first = node->next;
    } else {
        node->prev->next = node->next;
    }
    if (node->next == NULL) {
        list->last = node->prev;
    } else {
        node->next->prev = node->prev;
    }
}

void free_item(item *item) {
    free(item->data);
}

void _free_node(node *node) {
    free_item(&node->item);
    free(node);
}

void free_list(list* list) {
    // Free all the nodes from the heap
    node* currentNode = list->first;
    node* nextNode;
    for (u64 i = 0; i < list->len; i++) {
        nextNode = currentNode->next;
        _free_node(currentNode);
        currentNode = nextNode;
    }
}


node* _find_node_by_index(list* list, i64 index) {

    _check_index_in_range(list, index);
    if (list->error == INDEX_OUT_OF_RANGE) return NULL;

    index = _optimize_index(list, index);
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

    return currentNode;
}

void _check_index_in_range(list* list, i64 index) {
    // Check if index out of range
    if (index >= 0) {
        if (!(index < list->len)) list->error = INDEX_OUT_OF_RANGE; return;
    } else {
        if (!(llabs(index+1) < list->len)) list->error = INDEX_OUT_OF_RANGE; return;
    }
}