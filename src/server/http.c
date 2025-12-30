#include "http.h"
#include "router.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void free_http_response(HttpResponse* resp) {
    if (!resp) return;

    if (resp->body) {
        free(resp->body);
        resp->body = NULL;
    }

    // If we end up mallocing for status_text, make sure to update code here
}

int set_header(HttpResponse* resp, const char* key, const char* val) {
    if (strlen(key) >= MAX_KEY_LEN - 1 || strlen(val) >= MAX_VALUE_LEN - 1) return -1;

    strcpy(resp->headers[resp->num_headers].key, key);
    strcpy(resp->headers[resp->num_headers].value, val);

    resp->num_headers++;
    return 0;
}

static int parse_request_line(char* line, HttpRequest* req) {
    char* context = NULL;

    char* token = strtok_s(line, " ", &context);
    if (!token || strlen(token) >= MAX_METHOD_LEN - 1) return -1;
    strcpy(req->method, token);

    token = strtok_s(NULL, " ", &context);
    if (!token || strlen(token) >= MAX_PATH_LEN - 1) return -1;
    strcpy(req->path, token);

    token = strtok_s(NULL, " ", &context);
    if (!token || strlen(token) >= MAX_VERSION_LEN - 1) return -1;
    strcpy(req->version, token);

    return 0;
}

static int parse_headers(HttpRequest* req, char** context) {
    char* line;

    while ((line = strtok_s(NULL, "\r\n", context)) != NULL) {
        if (strlen(line) == 0) break; 
        if (req->num_headers >= MAX_HEADER_COUNT) break;

        char* key = line;
        char* val = strstr(line, ": ");
        if (!val) return -1;
        *val = '\0';
        val += 2;

        if (str_equals(key, "Connection", false)) {
            if (str_equals(val, "keep-alive", false)) req->keep_alive = true;
            else if (str_equals(val, "close", false)) req->keep_alive = false;
            continue;
        }

        if (strlen(key) >= MAX_KEY_LEN - 1 || strlen(val) >= MAX_VALUE_LEN - 1) return -1;
        strcpy(req->headers[req->num_headers].key, key);
        strcpy(req->headers[req->num_headers].value, val);

        req->num_headers++;
    }
    return 0;
}

HttpRequest* parse_http_request(const char* buffer, int buffer_len) {
    HttpRequest* req = calloc(1, sizeof(HttpRequest));
    if (!req) return NULL;
    if (buffer_len <= 0 || buffer_len > MAX_REQUEST_SIZE) {
        free(req);
        return NULL;
    }

    char *temp = malloc(buffer_len + 1);
    if (!temp) return NULL;

    memcpy(temp, buffer, buffer_len);
    temp[buffer_len] = '\0';

    char* context = NULL;
    char* first_line = strtok_s(temp, "\r\n", &context);
    if (!first_line || parse_request_line(first_line, req) != 0 || parse_headers(req, &context) != 0)  {
        free(req);
        free(temp);
        return NULL;
    }

    free(temp);
    return req;
}

char* build_http_response(HttpResponse* resp) {
    if (!resp) return NULL;

    int total_size = HEADER_SIZE_ESTIMATE + resp->body_length;

    char* output = calloc(1, total_size);
    int offset = 0;
    if (!output) return NULL;

    #define VERIFY_OFFSET() if (offset >= total_size) { free(output); return NULL; }

    // Add first line
    offset += snprintf(output + offset, total_size - offset, "%s %d %s\r\n",
        HTTP_VERSION, resp->status_code, resp->status_text);
    VERIFY_OFFSET()

    // Add headers
    for (int i = 0; i < resp->num_headers; i++) {
        offset += snprintf(output + offset, total_size - offset,
            "%s: %s\r\n",
            resp->headers[i].key,
            resp->headers[i].value);
        VERIFY_OFFSET()
    }

    // Add content-length
    offset += snprintf(output + offset, total_size - offset,
        "Content-Length: %d\r\n", resp->body_length);
    VERIFY_OFFSET() 

    // Add connection (close or keep-alive)
    const char* connection_val = resp->close_connection ? "close" : "keep-alive";
    offset += snprintf(output + offset, total_size - offset,
        "Connection: %s\r\n", connection_val);
    VERIFY_OFFSET()

    // One more \r\n to end headers
    offset += snprintf(output + offset, total_size - offset, "\r\n");
    VERIFY_OFFSET()

    // Response body
    if (resp->body_length > 0 && resp->body) {
        if (offset + resp->body_length > total_size) {
            free(output);
            return NULL;
        }
        memcpy(output + offset, resp->body, resp->body_length);
        offset += resp->body_length;
    }

    return output;
}