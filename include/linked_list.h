/* Doubly Linked List implementation */
#pragma once
#include <types.h>


// Used by 'list' type struct to hold error status as state
typedef enum {
    INDEX_OUT_OF_RANGE = 1,
    POP_FROM_EMPTY_LIST,
    VALUE_NOT_FOUND,
    HEAP_FAILURE,
} List_Error;


// Each unit of data: item
typedef struct {
    void* data;
    u64 data_len;
} item; 

// node
typedef struct node {
    item item;
    struct node* next;
    struct node* prev;
} node;

// head 
typedef struct {
    node* first;
    node* last;
    u64 len;
    List_Error error; // if (!error) check is convenient
} list;



list List(); 

// These three insertion functions return pointers to buffers of the size specified.
// Then the user writes their actual data into the buffer
void* append(list* list, u64 data_len);
void* prepend(list* list, u64 data_len);
void* insert(list* list, u64 data_len, i64 index, bool replace);

item pop(list* list, bool _return);
void pop_index(list* list, i32 index);
void remove_value(list* list, void* data, u64 data_len);
void remove_index(list* list, i64 index); 

item get(list* list, i64 index);
u64 search(list* list, void* data, u64 data_len);
i64 get_inverted_index(list* list, i64 index);

void free_item(item* item); 
void free_list(list* list);
void _free_node(node* node);

i64 _optimize_index(list* list, i64 index);
void _unwire_node(list* list, node* node);
node* _create_node(u64 data_len, void** data_buffer);
node* _find_node_by_index(list* list, i64 index);
void _check_index_in_range(list* list, i64 index);