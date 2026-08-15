#pragma once
#include <types.h>

typedef struct {
    int _temp;
} map;

typedef struct {
    void* data;
    u64 data_len;
} map_item;

/* 
Enumerating patterns:
  - Iterator protocol: Provide an iter() function returning the stateful iterable object, provide a next() function to drive the iterator object
  // * Verbose code for user
  - Callback pattern: Provide a foreach() function that takes in its arguments a callback function pointer (of a specified signature), and then calls that function on each key
  // * Cannot manually control the loop
  - FlatList pattern: Provide a get_keys() function that returns a flat array of all the keys 
  // * Consumes memory 
 */

map Map();

void* map_set(map* map, char* key, u64 data_len); // Make sure it saves the key as \0 terminated
map_item map_get(map* map, char* key);
char** map_get_keys(map* map); // Make sure to return a 'null terminated array'. 

void free_map(map* map);
void free_map_item(map_item* item);