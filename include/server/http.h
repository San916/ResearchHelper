#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>

#define HTTP_VERSION "HTTP/1.1"
#define MAX_METHOD_LEN 8
#define MAX_VERSION_LEN 16
#define MAX_PATH_LEN 256
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 256
#define MAX_HEADER_COUNT 32
#define MAX_REQUEST_SIZE 4096
#define MAX_RESPONSE_BODY_LEN 65536
#define HEADER_SIZE_ESTIMATE 1024

static const char* VALID_HTTP_METHODS[] = {
    "GET",
    "POST",
};

static const char* VALID_HTTP_VERSIONS[] = {
    "HTTP/1.1",
};

typedef enum {
    REQUEST_OK = 0,
    MALLOC_ERROR = 10,
    PARSE_REQUEST_TOO_BIG = 20,
    REQUEST_BAD_FORMAT = 30,

    PARSE_FIRST_LINE_OK = 100,
    PARSE_FIRST_LINE_INVALID_FORMAT,
    PARSE_FIRST_LINE_INVALID_METHOD,
    PARSE_FIRST_LINE_INVALID_VERSION,
    PARSE_FIRST_LINE_INVALID_PATH,

    SET_HEADERS_OK = 200,
    SET_HEADERS_INVALID_FORMAT,
    SET_HEADER_ENTRY_TOO_LARGE,
    SET_TOO_MANY_HEADERS,
    SET_HEADERS_NO_END_LINE,

    PARSE_HEADERS_OK = 300,
    PARSE_MULTIPLE_HOST_HEADERS,
    PARSE_NO_HOST_HEADERS,

    PARSE_BODY_OK = 400,

} HttpRequestError;

typedef struct HttpHeader {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} HttpHeader;

typedef struct HttpRequest {
    char version[MAX_VERSION_LEN];
    char method[MAX_METHOD_LEN];
    char path[MAX_PATH_LEN];

    HttpHeader headers[MAX_HEADER_COUNT];
    char* body;
    int num_headers;
    int max_num_responses;
    int max_num_comments;
    int min_score;
    bool keep_alive;
} HttpRequest;

typedef struct HttpResponse {
    char* status_text;
    int status_code;

    HttpHeader headers[MAX_HEADER_COUNT];
    int num_headers;

    char* body;
    int body_length;
    bool close_connection;
} HttpResponse;

void free_http_response(HttpResponse* resp);
HttpRequestError set_header(HttpHeader* header, int* num_headers, const char* key, const char* val);
HttpRequestError parse_request_line(char* line, HttpRequest* req);
HttpRequestError set_headers(HttpRequest* req, char** context);
HttpRequestError parse_headers(HttpRequest* req);
HttpRequestError parse_body(HttpRequest* req, char** context);
HttpRequest* parse_http_request(const char* buffer, int buffer_len, int* status_code);
char* build_http_response(HttpResponse* resp);

#endif