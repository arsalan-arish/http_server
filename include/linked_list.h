/* Doubly Linked List implementation */
#pragma once
#include <types.h>

// Used by 'list' type struct to hold error status as state
typedef enum {
    INDEX_OUT_OF_RANGE = 1,
    HEAP_FAILURE,
} List_Error;

// node
typedef struct node {
    void* data;
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

void pop(list* list);
void remove(list* list, void* data);
void remove_index(list* list, i64 index);

void* get(list* list, i64 index);
u64 search(list* list, byte* data, u64 data_len);
i64 get_negative_index(list* list, u64 positive_index);

void free_list(list* list);