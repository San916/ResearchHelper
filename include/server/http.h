#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>

#define HTTP_VERSION "HTTP/1.1"
#define MAX_METHOD_LEN 8
#define MAX_VERSION_LEN 16
#define MAX_PATH_LEN 256
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 256
#define MAX_HEADER_COUNT 16
#define MAX_REQUEST_SIZE 4096
#define HEADER_SIZE_ESTIMATE 1024

typedef struct HttpHeader {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} HttpHeader;

typedef struct HttpRequest {
    char version[MAX_VERSION_LEN];
    char method[MAX_METHOD_LEN];
    char path[MAX_PATH_LEN];

    HttpHeader headers[MAX_HEADER_COUNT];
    int num_headers;
    bool keep_alive;
} HttpRequest;

typedef struct HttpResponse {
    char *status_text;
    int status_code;

    HttpHeader headers[MAX_HEADER_COUNT];
    int num_headers;

    char *body;
    int body_length;
    bool close_connection;
} HttpResponse;

void free_http_response(HttpResponse* resp);
int set_header(HttpResponse* resp, const char* key, const char* val);
int parse_request_line(char* line, HttpRequest* req);
int parse_headers(HttpRequest* req, char** context);
HttpRequest* parse_http_request(const char* buffer, int buffer_len);
char* build_http_response(HttpResponse* resp);

#endif