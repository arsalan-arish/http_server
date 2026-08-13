#pragma once
#include <types.h>
#include <map.h>
#include <linked_list.h>

typedef list path;

path Path(const char* path_str);

typedef struct {
    bool valid;
    char* host;
    path path;
    map queries;
} url;

url Url(const char* url);
