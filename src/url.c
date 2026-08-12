#include <string.h>
#include <types.h>
#include <linked_list.h>
#include <hashmap.h>

typedef list path;

typedef struct {
    path path;
    char* queries;
    char* anchor;
} slug;

typedef struct {
    bool success;
    char* protocol;
    char* host_name;
    slug slug;
} url;

url Url(const char* url) {
    // The 'http://' or 'https://' is not relevant. It should not be there, but if there is, there cannot be any other than this
    int ans = strncmp(url, "http://", strlen("http://"));
}