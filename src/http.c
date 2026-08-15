#include <corecrt.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>

#include <map.h>
#include <string.h>
#include <string_regex.h>
#include <http.h>


Request parse_http_request(char* request) {
    Request req;

    // Setup helping pointers & counter
    char* ptr1 = request;
    char* ptr2 = request;
    i32 counter;

    // Parse the method
    counter = 0;
    while (*ptr2 != ' ') {
        ptr2++;
        counter++;
    }
    *ptr2 = '\0';
    for (int i = 0; i < counter; i++) {
        if (!isupper(ptr1[i])) {} // Raise error 'HTTP METHOD MUST BE CAPTIAL CASE ONLY'
        // Also verify the right http method
    }
    req.method = malloc(counter + 1);
    /* if (request.method == NULL) Raise error mechanism */
    //! Make sure to call the cleanup functions before exiting after any error as above
    strcpy(req.method, ptr1);

    // Position the pointers
    ptr2++;
    ptr1 = ptr2;

    // Parse the path
    counter = 0;
    while (*ptr2 != ' ') {
        ptr2++;
        counter++;
    }
    *ptr2 = '\0';
    req.path = malloc(counter + 1);
    /* if (request.method == NULL) Raise error mechanism */
    strcpy(req.path, ptr1);

    // Position the pointers
    ptr2++;
    ptr1 = ptr2;

    // Parse the http version
    if (memcmp(ptr1, "HTTP/", 5)) {
        // raise error that the protocol has not been specified correctly
    }
    ptr2 += 5;
    ptr1 = ptr2;
    req.version = malloc(4);
    /* if (request.method == NULL) Raise error mechanism */
    if (memcmp(ptr1, "1.1", 3)) {
        memcpy(req.version, ptr1, 3);
        req.version[3] = '\0';
        ptr2 += 4;
    } else {
        memcpy(req.version, ptr1, 1);
        req.version[1] = '\0';
        ptr2 += 2;
    }
    ptr1 = ptr2;

    // Setup the map for headers
    req.headers = Map();
    // Now both pointers point at the first char of first header
    // Parse the headers
    while (*ptr1 != '\n') {

        while (*ptr2 != ':') {
            *ptr2 = tolower(*ptr2); // Normalize all headers to lowercase
            ptr2++;
        }
        *ptr2 = '\0'; // Temporarily have to do this because the set() function of map only supports \0 terminated strings
        ptr2 += 2;

        counter = 0;
        while (*ptr2 != '\n') {
            ptr2++;
            counter++;
        }
        *ptr2 = '\0';
        ptr2++;
        // At this point, the first ptr points at \0 terminated key, counter contains the length of the value, second_ptr points at the key of next header
        char* buff = map_set(&req.headers, ptr1, counter + 1); // +1 for \0
        ptr1 += strlen(ptr1) + 2;
        strcpy(buff, ptr1);
        ptr1 = ptr2;
    }
    ptr2++;
    ptr1 = ptr2;
    // Now both pointers point at the first char of the http body payload

    // get content length
    char* len_str = map_get(&req.headers, "content-length").data;
    char* endptr;
    u64 len = strtoull(len_str, &endptr, 10);
    if (*endptr != '\0') {} // Error in parsing content-length
    
    // Copy the payload
    req.payload = malloc(len + 1); // +1 for \0
    /* if (request.payload == NULL) Raise error mechanism */
    memcpy(req.payload, ptr1, len);
    ptr1[len] = '\0';

    // Set size
    set_http_request_size(&req);
    // Validate
    validate_http_request(&req); //! Manage this. Btw isn't it better to validate each thing sequentially right after parsing it??

    return req;
}

void set_http_request_size(Request* req) {

    u8 method_len = strlen(req->method);
    u16 path_len = strlen(req->path);
    u8 version_len = strlen(req->version) + 5; // Bcz 'HTTP/' is not included in the parsed object

    u64 headers_len;
    // Enumerate over every kv pair of map and sum up the strlen() of all
    char** hdrs = map_get_keys(&req->headers);
    char* key;
    u64 n = 0;
    while ((key = hdrs[n])) {
        headers_len += strlen(key);
        headers_len += 3;
        headers_len += map_get(&req->headers, key).data_len;
        n++;
    }
    free(hdrs);

    char* endptr;
    u64 payload_len = strtoull(map_get(&req->headers, "content-length").data, &endptr, 10);
    if (*endptr != '\0') {} // Error in parsing

    req->size = (
        method_len +
        1          +
        path_len   +
        1          +
        version_len+
        1          +
        headers_len+
        2          +
        payload_len
    );
}

void validate_http_request(Request* req) {

}

char* serialize_http_request(Request* req) {
    /* The user make sure to manually call free() on the char* returned once it has been used */
    validate_http_request(req);
    if (!req->size) set_http_request_size(req);
    char* buff = malloc(req->size);
    char* ptr = buff;
    
    strcpy(ptr, req->method);
    ptr += strlen(req->method);
    *ptr = ' ';
    ptr++;

    strcpy(ptr, req->path);
    ptr += strlen(req->path);
    *ptr = ' ';
    ptr++;

    strcpy(ptr, "HTTP/");
    ptr += 5;
    strcpy(ptr, req->version);
    ptr += strlen(req->version);
    *ptr = '\n';
    ptr++;

    char** hdrs = map_get_keys(&req->headers);
    char* key;
    u64 n = 0;
    while ((key = hdrs[n])) {
        char* value = map_get(&req->headers, key).data;
        strcpy(ptr, key);
        ptr += strlen(key);
        strcpy(ptr, ": ");
        ptr += 2;
        strcpy(ptr, value);
        ptr += strlen(value);
        *ptr = '\n';
        ptr++;
    }
    *ptr = '\n';
    ptr++;
    free(hdrs);

    strcpy(ptr, req->payload);

    return buff;
}


Response parse_http_response(char* response) {
    Response res;

    char* ptr = response;
    // Parse HTTP version
    if (memcmp(ptr, "HTTP/", 5)) {} // Raise error
    ptr += 5;
    if (memcmp(ptr, "1.1", 3)) {
        res.version = malloc(4);
        memcpy(res.version, ptr, 3);
        res.version[3] = '\0';
        ptr += 4;
    } else {
        res.version = malloc(2);
        memcpy(res.version, ptr, 1);
        res.version[1] = '\0';
        ptr += 2;
    }

    // Parse the status code
    res.status = malloc(4);
    memcpy(res.status, ptr, 3);
    res.status[3] = '\0';
    ptr += 4;

    // Parse the msg
    i8 n = 0;
    while (ptr[n] != '\n') {
        n++;
    }
    res.msg = malloc( n + 1);
    memcpy(res.msg, ptr, n);
    res.msg[n] = '\0';
    ptr += n + 1;

    // Parse the headers // * Same loop copied from parse_http_request()
    // Setup the map for headers
    res.headers = Map();
    char* ptr1 = ptr;
    char* ptr2 = ptr;
    // Now both pointers point at the first char of first header
    // Parse the headers
    while (*ptr1 != '\n') {

        while (*ptr2 != ':') {
            *ptr2 = tolower(*ptr2); // Normalize all headers to lowercase
            ptr2++;
        }
        *ptr2 = '\0'; // Temporarily have to do this because the set() function of map only supports \0 terminated strings
        ptr2 += 2;

        i32 counter = 0;
        while (*ptr2 != '\n') {
            ptr2++;
            counter++;
        }
        *ptr2 = '\0';
        ptr2++;
        // At this point, the first ptr points at \0 terminated key, counter contains the length of the value, second_ptr points at the key of next header
        char* buff = map_set(&res.headers, ptr1, counter + 1); // +1 for \0
        ptr1 += strlen(ptr1) + 2;
        strcpy(buff, ptr1);
        ptr1 = ptr2;
    }
    ptr2++;
    ptr1 = ptr2;
    // Now both pointers point at the first char of the http body payload

    // get content length
    char* len_str = map_get(&res.headers, "content-length").data;
    char* endptr;
    u64 len = strtoull(len_str, &endptr, 10);
    if (*endptr != '\0') {} // Error in parsing content-length
    
    // Copy the payload
    res.payload = malloc(len + 1); // +1 for \0
    /* if (res.payload == NULL) Raise error mechanism */
    memcpy(res.payload, ptr1, len);
    ptr1[len] = '\0';

    // Set size
    set_http_response_size(&res);
    validate_http_response(&res);

    return res;
}

char* serialize_http_response(Response* res) {
    /* The user make sure to manually call free() on the char* returned once it has been used */
    validate_http_response(res);
    if (!res->size) set_http_response_size(res);
    char* buff = malloc(res->size);
    char* ptr = buff;

    strcpy(ptr, "HTTP/");
    ptr += 5;
    strcpy(ptr, res->version);
    ptr += strlen(res->version);
    *ptr = ' ';
    ptr++;

    strcpy(ptr, res->status);
    ptr += strlen(res->status);
    *ptr = ' ';
    ptr++;

    strcpy(ptr, res->msg);
    ptr += strlen(res->msg);
    *ptr = '\n';
    ptr++;

    // * Copied from serialize_http_request()
    char** hdrs = map_get_keys(&res->headers);
    char* key;
    u64 n = 0;
    while ((key = hdrs[n])) {
        char* value = map_get(&res->headers, key).data;
        strcpy(ptr, key);
        ptr += strlen(key);
        strcpy(ptr, ": ");
        ptr += 2;
        strcpy(ptr, value);
        ptr += strlen(value);
        *ptr = '\n';
        ptr++;
    }
    *ptr = '\n';
    ptr++;
    free(hdrs);   

    strcpy(ptr, res->payload);
    
    return buff;
}

void free_http_request(Request* req) {
    free(req->method);
    free(req->path);
    free(req->version);
    free_map(&req->headers);
    free(req->payload);
}

void free_http_response(Response* res) {
    free(res->version);
    free(res->status);
    free(res->msg);
    free_map(&res->headers);
    free(res->payload);
}