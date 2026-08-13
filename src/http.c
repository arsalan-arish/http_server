#include <stdlib.h>

#include <map.h>
#include <string.h>
#include <string_regex.h>
#include <types.h>

enum http_methods {
    GET,
    POST,
    PUT,
    DELETE,
};

enum http_versions {
    _1_1,
    _2,
    _3,
};

typedef struct {
    char* method;
    char* path;
    char* http_version; // "1.1" | "2" | "3"

    map headers;
    char* payload;
} Request;

Request parse_http_request(char* req) {
    Request request;

    // Setup helping pointers & counter
    char* first_ptr = req;
    char* second_ptr = req;
    i8 counter = 0;

    // Parse the method
    while (*second_ptr != ' ') {
        second_ptr++;
        counter++;
    }
    request.method = malloc(counter);
    /* if (request.method == NULL) Raise error mechanism */
    memcpy(request.method, first_ptr, counter);

    // Position the pointers
    second_ptr++;
    first_ptr = second_ptr;

    // Parse the path
    counter = 0;
    while (*second_ptr != ' ') {
        second_ptr++;
        counter++;
    }
    request.path = malloc(counter);
    /* if (request.method == NULL) Raise error mechanism */
    memcpy(request.path, first_ptr, counter);

    // Position the pointers
    second_ptr++;
    first_ptr = second_ptr;

    // Parse the http version
    if (memcmp(first_ptr, "http/", 5) || memcmp(first_ptr, "HTTP/", 5)) {
        // raise error that the protocol has not been specified correctly
    }
    second_ptr += 5;
    first_ptr = second_ptr;
    request.http_version = malloc(3);
    /* if (request.method == NULL) Raise error mechanism */
    if (memcmp(first_ptr, "1.1", 3)) {
        memcpy(request.http_version, first_ptr, 3);
        second_ptr += 4;
        first_ptr = second_ptr;
    } else {
        memcpy(request.http_version, first_ptr, 1);
        second_ptr += 2;
        first_ptr = second_ptr;
    }

    // Now the pointers point at the first char of first header
    // Setup the map for headers
    map hdrs = Map();
    // Parse the headers
    while (*second_ptr != ':') {
        second_ptr++;
    }
    *second_ptr = '\0'; // Temporarily have to do this because the set() function of map only supports \0 terminated strings
    second_ptr++;
    first_ptr = second_ptr;

    counter = 0;
    while (*second_ptr != '\n') {
        second_ptr++;
        counter++;
    }

    char* buff = set(&hdrs, first_ptr, counter);
    memcpy(buff, first_ptr, counter);

    second_ptr++;
    first_ptr = second_ptr;
    // Now both pointers point at the first char of the second header
    

    return request;
}

void free_http_request_memory(Request* req) {
    free(req->method);
    free(req->path);
    free(req->http_version);
    free_map(&req->headers);
    free(req->payload);
}