#pragma once
#include <types.h>

typedef struct {
    int _temp;
} map;

map Map();

void* set(map* map, char* key, u64 data_len);

void free_map(map* map);