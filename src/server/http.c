#include "http.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// MODIFIES/EFFECTS Frees every valid field in the given HttpResponse
void free_http_response(HttpResponse* resp) {
    if (!resp) return;

    if (resp->body) {
        free(resp->body);
        resp->body = NULL;
    }

    // If we end up mallocing for status_text, make sure to update code here
}

// REQUIRES: Pointer to start of HttpHeader array, number of headers, and key value pair
// MODIFIES/EFFECTS: Sets header with key and value pair, and increments num_headers. Returns status code or 0
HttpRequestError set_header(HttpHeader* headers, size_t* num_headers, const char* key, const char* val) {
    if (*num_headers >= MAX_HEADER_COUNT) return SET_TOO_MANY_HEADERS;
    if (strlen(key) >= MAX_KEY_LEN - 1 || strlen(val) >= MAX_VALUE_LEN - 1) return SET_HEADER_ENTRY_TOO_LARGE;

    strcpy(headers[*num_headers].key, key);
    strcpy(headers[*num_headers].value, val);

    (*num_headers)++;
    return 0;
}

// REQUIRES: method as string
// EFFECTS: Returns true if the method is a valid method (i.e "GET")
bool is_valid_method(char* method) {
    if (strlen(method) >= MAX_METHOD_LEN - 1) return false;
    for (size_t i = 0; i < sizeof(VALID_HTTP_METHODS) / sizeof(VALID_HTTP_METHODS[0]); i++) {
        if (strcmp(method, VALID_HTTP_METHODS[i]) == 0) {
            return true;
        }
    }
    return false;
}

// REQUIRES: version as string
// EFFECTS: Returns true if the version is a valid version (i.e "HTTP/1.1")
bool is_valid_version(char* version) {
    if (strlen(version) >= MAX_VERSION_LEN - 1) return false;
    for (size_t i = 0; i < sizeof(VALID_HTTP_VERSIONS) / sizeof(VALID_HTTP_VERSIONS[0]); i++) {
        if (strcmp(version, VALID_HTTP_VERSIONS[i]) == 0) {
            return true;
        }
    }
    return false;
}

// REQUIRES: Takes a request line and request
// MODIFIES: Modifies original string with '\0' in place of delimeter. Modifies request method, path, token
// EFFECTS: Reads the line given into req->method, path, version. Returns status code
HttpRequestError parse_request_line(char* line, HttpRequest* req) {
    char* context = NULL;

    char* token = strtok_s(line, " ", &context);
    if (!token) return PARSE_FIRST_LINE_INVALID_FORMAT;
    strcpy(req->method, token);
    if (!is_valid_method(req->method)) return PARSE_FIRST_LINE_INVALID_METHOD;

    token = strtok_s(NULL, " ", &context);
    if (!token || strlen(token) >= MAX_PATH_LEN - 1) return PARSE_FIRST_LINE_INVALID_PATH;
    strcpy(req->path, token);

    token = strtok_s(NULL, " ", &context);
    if (!token) return PARSE_FIRST_LINE_INVALID_FORMAT;
    strcpy(req->version, token);
    if (!is_valid_version(req->version)) return PARSE_FIRST_LINE_INVALID_VERSION;

    return PARSE_FIRST_LINE_OK;
}

// REQUIRES: HttpRequest, char** context pointing to the start of the headers section in the request
// MODIFIES: char** context to the end of headers
// EFFECTS: Parses through headers and adds them into the HttpRequest. Returns status code
HttpRequestError set_headers(HttpRequest* req, char** context) {
    char* line;

    char* end_of_headers = strstr(*context, "\r\n\r\n");
    if (!end_of_headers) return SET_HEADERS_NO_END_LINE;

    char saved_char = end_of_headers[4];
    end_of_headers[4] = '\0';

    while ((line = strtok_s(NULL, "\r\n", context)) != NULL) {
        if (req->num_headers >= MAX_HEADER_COUNT) return SET_TOO_MANY_HEADERS;

        char* key = line;
        char* value = strstr(line, ": ");
        if (!value) return SET_HEADERS_INVALID_FORMAT;
        *value = '\0';
        value += 2;

        int status_code = set_header(req->headers, &req->num_headers, key, value);
        if (status_code) return status_code;
    }
    end_of_headers[4] = saved_char;
    *context = end_of_headers + 4;
    return SET_HEADERS_OK;
}

// REQUIRES: HttpRequest, headers need to have been set
// EFFECTS: Parses through headers and adds them into corresponding member variables. Returns status code
// There must only be 1 Host key
// keep_alive defaults to true
HttpRequestError parse_headers(HttpRequest* req) {
    bool contains_host = false;
    bool keep_alive = true;

    for (size_t i = 0; i < req->num_headers; i++) {
        char* key = req->headers[i].key;
        char* value = req->headers[i].value;

        if (str_equals(key, "Connection", false)) {
            if (str_equals(value, "keep-alive", false)) keep_alive = true;
            else if (str_equals(value, "close", false)) keep_alive = false;
        } else if (str_equals(key, "Host", false)) {
            if (contains_host) return PARSE_MULTIPLE_HOST_HEADERS;
            contains_host = true;
        } else if (str_equals(key, "Max-Num-Responses", false)) {
            char *end_ptr;
            req->max_num_responses = strtoull(value, &end_ptr, 10);
        } else if (str_equals(key, "Max-Num-Comments", false)) {
            char *end_ptr;
            req->max_num_comments = strtoull(value, &end_ptr, 10);
        } else if (str_equals(key, "Min-Score", false)) {
            req->min_score = atoi(value);
        }
    }

    if (!contains_host) return PARSE_NO_HOST_HEADERS;
    req->keep_alive = keep_alive;
    return PARSE_HEADERS_OK;
}

// REQUIRES: HttpRequest, char** context pointing to the start of the body section in the request
// EFFECTS: Parses through body and adds it into the HttpRequest. Returns status code
HttpRequestError parse_body(HttpRequest* req, char** context) {
    if (context == NULL || *context == NULL || **context == '\0') return PARSE_BODY_OK;
    req->body = calloc(1, strlen(*context) + 1);
    if (!req->body) return MALLOC_ERROR;
    strcpy(req->body, *context);
    return PARSE_BODY_OK;
}

// REQUIRES: Takes buffer with buffer_len = len of str without null terminator (aka strlen(buffer))
// EFFECTS: Parses buffer into an HTTPRequest* and returns it, or return NULL on failure
// Caller must free the HttpRequest*
HttpRequest* parse_http_request(const char* buffer, size_t buffer_len, int* status_code) {
    HttpRequest* req = calloc(1, sizeof(HttpRequest));
    if (!req) {
        *status_code = MALLOC_ERROR;
        return NULL;
    }
    if (buffer_len <= 0 || buffer_len >= MAX_REQUEST_SIZE - 1 || buffer_len > strlen(buffer)) {
        free(req);
        *status_code = PARSE_REQUEST_TOO_BIG;
        return NULL;
    }

    char* temp = malloc(buffer_len + 1);
    if (!temp) {
        free(req);
        *status_code = MALLOC_ERROR;
        return NULL;
    }

    memcpy(temp, buffer, buffer_len);
    temp[buffer_len] = '\0';

    #define SET_STATUS_CODE_RETURN(code)  { *status_code = code; free(req); free(temp); return NULL; }

    char* context = NULL;
    char* first_line = strtok_s(temp, "\r\n", &context);
    if (!first_line) SET_STATUS_CODE_RETURN(REQUEST_BAD_FORMAT);

    int request_status = parse_request_line(first_line, req);
    if (request_status != PARSE_FIRST_LINE_OK) SET_STATUS_CODE_RETURN(request_status);

    int set_headers_status = set_headers(req, &context);
    if (set_headers_status != SET_HEADERS_OK) SET_STATUS_CODE_RETURN(set_headers_status);

    int parse_headers_status = parse_headers(req);
    if (parse_headers_status != PARSE_HEADERS_OK) SET_STATUS_CODE_RETURN(parse_headers_status);

    req->body = NULL;
    int body_status = parse_body(req, &context);
    if (body_status != PARSE_BODY_OK) SET_STATUS_CODE_RETURN(body_status);

    *status_code = REQUEST_OK;
    free(temp);

    return req;
}

// REQUIRES: Http response
// EFFECTS: Returns a stringified response made using the given HttpResponse. Returns NULL on fail
char* build_http_response(HttpResponse* resp) {
    if (!resp) return NULL;
    if (resp->num_headers > MAX_HEADER_COUNT) return NULL;
    if (resp->body_length >= MAX_RESPONSE_BODY_LEN - 1) return NULL;

    size_t total_size = HEADER_SIZE_ESTIMATE + resp->body_length;

    char* output = calloc(1, total_size);
    size_t offset = 0;
    if (!output) return NULL;

    #define VERIFY_OFFSET() if (offset >= total_size) { free(output); return NULL; }

    offset += snprintf(output + offset, total_size - offset, "%s %d %s\r\n",
        HTTP_VERSION, resp->status_code, resp->status_text);
    VERIFY_OFFSET()

    for (size_t i = 0; i < resp->num_headers; i++) {
        offset += snprintf(output + offset, total_size - offset,
            "%s: %s\r\n",
            resp->headers[i].key,
            resp->headers[i].value);
        VERIFY_OFFSET()
    }

    offset += snprintf(output + offset, total_size - offset, "\r\n");
    VERIFY_OFFSET()

    if (resp->body) {
        if (offset + resp->body_length > total_size) {
            free(output);
            return NULL;
        }
        memcpy(output + offset, resp->body, resp->body_length);
        offset += resp->body_length;
    }

    return output;
}