#include <corecrt.h>
#include <map.h>
#include <types.h>

typedef struct {
    u64 size;
    char* method;
    char* path;
    char* version; // "1.1" | "2" | "3"

    map headers;
    char* payload;
} Request;

typedef struct {
    bool success;
    enum {
        WRONG_HTTP_METHOD,
        WRONG_HTTP_VERSION,
        WRONG_SYNTAX,
    } _errno_;
    u32 lineno;
} req_err_info;

typedef struct {
    u64 size;
    char* version;
    char* status;
    char* msg;
    
    map headers;
    char* payload;
} Response;


Request parse_http_request(char* req);
void set_http_request_size(Request* req);
char* serialize_http_request(Request* req);
void validate_http_request(Request* req);

Response parse_http_response(char* res);
void set_http_response_size(Response* res);
char* serialize_http_response(Response* res);
void validate_http_response(Response* res);

void free_http_request(Request* req);
void free_http_response(Response* res);


/*

// Opaque iterator structure
typedef struct {
    const struct hashmap *map; // Pointer to your hashmap struct
    size_t bucket_index;       // Current index in the underlying array
    const void *current_node;  // Current node in a collision chain (linked list)
} hashmap_iter_t;

// Functions to control the loop
hashmap_iter_t hashmap_iter_init(const struct hashmap *map);
const char*    hashmap_iter_next(hashmap_iter_t *iter);

hashmap_iter_t iter = hashmap_iter_init(my_map);
const char *key;


while ((key = hashmap_iter_next(&iter)) != NULL) {
    printf("Key: %s\n", key);
    if (strcmp(key, "stop") == 0) break; // Completely safe to break!
}

*/


/*

// Callback function prototype definition
typedef void (*hashmap_callback_t)(const char *key, void *value, void *user_data);

// The enumeration function
void hashmap_foreach(const struct hashmap *map, hashmap_callback_t callback, void *user_data);


void print_key(const char *key, void *value, void *user_data) {
    int *counter = (int *)user_data;
    printf("[%d] Key: %s\n", (*counter)++, key);
}

// In main or another function:
int count = 0;
hashmap_foreach(my_map, print_key, &count);

*/


/*

// Returns a dynamically allocated array of strings, NULL-terminated at the end
const char** hashmap_get_keys(const struct hashmap *map, size_t *out_count);

size_t count;
const char **keys = hashmap_get_keys(my_map, &count);

for (size_t i = 0; i < count; i++) {
    printf("Key at index %zu: %s\n", i, keys[i]);
}

free(keys); // Critical! If forgotten, this causes a major memory leak.

*/



/*

#include <stdint.h>
#include <stddef.h>

// 1. The Dense Data Entry
typedef struct {
    char *key;
    void *value;
    uint32_t hash; // Storing the hash speeds up resizing
} hashmap_entry_t;

// 2. The Main Hashmap Struct
typedef struct {
    int32_t *indices;         // Sparse array of index pointers (initialized to -1)
    size_t indices_capacity;  // Power of 2 (e.g., 16, 32, 64...)
    
    hashmap_entry_t *entries; // Dense array preserving insertion order
    size_t entries_count;     // How many items are actually in the map
    size_t entries_capacity;  // Capcity of the dense array
} ordered_hashmap_t;


// Implementation of Pattern 1 (External Iterator) from earlier
const char* hashmap_iter_next(hashmap_iter_t *iter, ordered_hashmap_t *map) {
    // Simply walk the dense entries array linearly!
    if (iter->bucket_index < map->entries_count) {
        const char *key = map->entries[iter->bucket_index].key;
        iter->bucket_index++;
        return key;
    }
    return NULL; // Reached the end
}

*/
