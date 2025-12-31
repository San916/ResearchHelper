#include "unity.h"
#include "http.h"
#include "test_http_utils.h"
#include "router.h"
#include <stdlib.h>
#include <string.h>

#define UNITY_PRINT_TEST_CONTEXT
#define UNITY_OUTPUT_COLOR 1

// Define all variables here
static char workingRequest[] =
        "GET /index.html HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "User-Agent: Mozilla/5.0\r\n"
        "Connection: keep-alive\r\n";

static HttpResponse resp = {0};
static const char* resp_str =
    "HTTP/1.1 200 OK\r\n"
    "Host: www.example.com\r\n"
    "Content-Type: text/html\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: 48\r\n"
    "\r\n"
    "<html><body><h1>Hello, World!</h1></body></html>";

static HttpRequest* req = NULL;

void setUp(void) {
    resp.close_connection = false;
    resp.status_code = 200;
    strcpy(resp.headers[0].key, "Host");
    strcpy(resp.headers[0].value, "www.example.com");
    strcpy(resp.headers[1].key, "Content-Type");
    strcpy(resp.headers[1].value, "text/html");
    strcpy(resp.headers[2].key, "Connection");
    strcpy(resp.headers[2].value, "keep-alive");
    resp.status_text = "OK";
    resp.num_headers = 3;
    resp.body = "<html><body><h1>Hello, World!</h1></body></html>";
    resp.body_length = strlen(resp.body);
}

void tearDown(void) {
    if (req) free(req);
    req = NULL; // Avoid dangling pointer
}

// ==================================
// set_header
// ==================================
void test_set_header_working(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char* key = "Host";
    char* value = "www.example.com";

    TEST_ASSERT_EQUAL_INT(req->num_headers, 0);
    TEST_ASSERT_EQUAL_INT(set_header(req->headers, &req->num_headers, key, value), 0);
    TEST_ASSERT_EQUAL_INT(req->num_headers, 1);
}

void test_set_header_key_too_large(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char* key = malloc(MAX_KEY_LEN + 1);
    if (!key) return;
    memset(key, 'A', MAX_KEY_LEN);
    key[MAX_KEY_LEN] = '\0';
    char* value = "www.example.com";

    TEST_ASSERT_EQUAL_INT(strlen(key), MAX_KEY_LEN);
    TEST_ASSERT_EQUAL_INT(req->num_headers, 0);
    TEST_ASSERT_EQUAL_INT(set_header(req->headers, &req->num_headers, key, value), PARSE_HEADER_ENTRY_TOO_LARGE);
    TEST_ASSERT_EQUAL_INT(req->num_headers, 0);
}

void test_set_header_value_too_large(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char* key = "Host";
    char* value = malloc(MAX_VALUE_LEN + 1);
    if (!value) return;
    memset(value, 'A', MAX_VALUE_LEN);
    value[MAX_VALUE_LEN] = '\0';

    TEST_ASSERT_EQUAL_INT(strlen(value), MAX_VALUE_LEN);
    TEST_ASSERT_EQUAL_INT(req->num_headers, 0);
    TEST_ASSERT_EQUAL_INT(set_header(req->headers, &req->num_headers, key, value), PARSE_HEADER_ENTRY_TOO_LARGE);
    TEST_ASSERT_EQUAL_INT(req->num_headers, 0);
}

void test_set_header_too_many_headers(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char* key = "Host";
    char* value = "www.example.com";

    for (int i = 0; i < MAX_HEADER_COUNT; i++) {
        TEST_ASSERT_EQUAL_INT(set_header(req->headers, &req->num_headers, key, value), 0);
    }

    TEST_ASSERT_EQUAL_INT(req->num_headers, MAX_HEADER_COUNT);
    TEST_ASSERT_EQUAL_INT(set_header(req->headers, &req->num_headers, key, value), PARSE_TOO_MANY_HEADERS);
    TEST_ASSERT_EQUAL_INT(req->num_headers, MAX_HEADER_COUNT);
}

// ==================================
// parse_request_line
// ==================================
// We don't dependency inject the requirements, such as max size for fields, and available fields, so we may run into issues in the future
void test_parse_request_line(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char line[] = "GET /index.html HTTP/1.1";
    TEST_ASSERT_EQUAL_INT(parse_request_line(line, req), PARSE_FIRST_LINE_OK);
    TEST_ASSERT_EQUAL_INT(strcmp(req->method, "GET"), 0);    
    TEST_ASSERT_EQUAL_INT(strcmp(req->path, "/index.html"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->version, "HTTP/1.1"), 0);

    char bad_line_1[] = "GET /index.html:HTTP/1.1";
    char bad_line_2[] = "EVILEVILEVIL /index.html HTTP/1.1";
    char bad_line_3[] = "GET /index.html EVIL_HTTP/1.1";
    char bad_line_4[] = "GETTOOLONGWAYTOOLONG /index.html EVIL_HTTP/1.1";
    char bad_line_5[300] = "GET ";
    for(int i = 4; i < 4 + MAX_PATH_LEN; i++) {
        bad_line_5[i] = 'A';
    }

    TEST_ASSERT_EQUAL_INT(parse_request_line(bad_line_1, req), PARSE_FIRST_LINE_INVALID_FORMAT);
    TEST_ASSERT_EQUAL_INT(parse_request_line(bad_line_2, req), PARSE_FIRST_LINE_INVALID_METHOD);
    TEST_ASSERT_EQUAL_INT(parse_request_line(bad_line_3, req), PARSE_FIRST_LINE_INVALID_VERSION);
    TEST_ASSERT_EQUAL_INT(parse_request_line(bad_line_4, req), PARSE_FIRST_LINE_INVALID_METHOD);
    TEST_ASSERT_EQUAL_INT(parse_request_line(bad_line_5, req), PARSE_FIRST_LINE_INVALID_PATH);
}

// ==================================
// parse_headers
// ==================================
void test_parse_headers_working(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char line[] =         
        "Host: www.example.com\r\n"
        "User-Agent: Mozilla/5.0\r\n"
        "Connection: keep-alive\r\n";

    char* context = line;
    TEST_ASSERT_EQUAL_INT(parse_headers(req, &context), PARSE_HEADERS_OK);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[0].key, "Host"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[0].value, "www.example.com"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[1].key, "User-Agent"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[1].value, "Mozilla/5.0"), 0);
    TEST_ASSERT_TRUE(req->keep_alive);
}

void test_parse_headers_connection_close(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char line[] =         
        "Host: www.example.com\r\n"
        "Connection: CLOSE\r\n";

    char* context = line;
    TEST_ASSERT_EQUAL_INT(parse_headers(req, &context), PARSE_HEADERS_OK);
    TEST_ASSERT_FALSE(req->keep_alive);
}

void test_parse_headers_connection_not_properly_defined(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char line[] =         
        "Host: www.example.com\r\n"
        "Connection: KEEP-ME-ALIVE\r\n";

    char* context = line;
    TEST_ASSERT_EQUAL_INT(parse_headers(req, &context), PARSE_HEADERS_OK);
    TEST_ASSERT_TRUE(req->keep_alive);
}

void test_parse_headers_invalid_format(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char line[] =         
        "Host:www.example.com\r\n";

    char* context = line;
    TEST_ASSERT_EQUAL_INT(parse_headers(req, &context), PARSE_HEADERS_INVALID_FORMAT);
}

void test_parse_headers_too_many_headers(void) {
    req = calloc(1, sizeof(HttpRequest));
    req->num_headers = MAX_HEADER_COUNT;
    if (!req) return;

    char line[] =         
        "Host: www.example.com\r\n";

    char* context = line;
    TEST_ASSERT_EQUAL_INT(parse_headers(req, &context), PARSE_TOO_MANY_HEADERS);
}

void test_parse_headers_two_hosts(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char line[] =         
        "Host: www.example.com\r\n"
        "Host: www.example.com\r\n";

    char* context = line;
    TEST_ASSERT_EQUAL_INT(parse_headers(req, &context), PARSE_MULTIPLE_HOST_HEADERS);
}

void test_parse_headers_no_hosts(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char line[] =         
        "User-Agent: Mozilla/5.0\r\n"
        "Connection: keep-alive\r\n";

    char* context = line;
    TEST_ASSERT_EQUAL_INT(parse_headers(req, &context), PARSE_NO_HOST_HEADERS);
}

// ==================================
// parse_http_request
// ==================================
void test_parse_http_request(void) {
    int status_code = 0;
    TEST_ASSERT_NULL(parse_http_request(workingRequest, 0, &status_code));
    TEST_ASSERT_EQUAL_INT(status_code, PARSE_REQUEST_TOO_BIG);
    TEST_ASSERT_NULL(parse_http_request(workingRequest, MAX_REQUEST_SIZE - 1, &status_code));
    TEST_ASSERT_EQUAL_INT(status_code, PARSE_REQUEST_TOO_BIG);
    TEST_ASSERT_NULL(parse_http_request("HII", 100, &status_code));
    TEST_ASSERT_EQUAL_INT(status_code, PARSE_REQUEST_TOO_BIG);

    req = parse_http_request(workingRequest, strlen(workingRequest), &status_code);
    TEST_ASSERT_EQUAL_INT(strcmp(req->method, "GET"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->path, "/index.html"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->version, "HTTP/1.1"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[0].key, "Host"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[0].value, "www.example.com"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[1].key, "User-Agent"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[1].value, "Mozilla/5.0"), 0);
    TEST_ASSERT_TRUE(req->keep_alive);
}

// ==================================
// build_http_response
// ==================================
void test_build_http_response_working(void) {
    const char* our_resp_str = build_http_response(&resp);
    TEST_ASSERT_NOT_NULL(our_resp_str);
    TEST_ASSERT_EQUAL_INT(strcmp(resp_str, our_resp_str), 0);
}

void test_build_http_response_body_length_disparity(void) {
    resp.body_length++;
    TEST_ASSERT_NULL(build_http_response(&resp));
    resp.body_length--;
}

void test_build_http_response_too_many_headers(void) {
    resp.num_headers = MAX_HEADER_COUNT;
    TEST_ASSERT_GREATER_THAN_INT(0, resp.body_length);
    TEST_ASSERT_NOT_NULL(resp.body);
    TEST_ASSERT_NULL(build_http_response(&resp));

    resp.num_headers = MAX_HEADER_COUNT + 1;
    resp.body_length = 0;
    resp.body = NULL;
    TEST_ASSERT_EQUAL_INT(0, resp.body_length);
    TEST_ASSERT_NULL(resp.body);
    TEST_ASSERT_NULL(build_http_response(&resp));
}

int main() {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_set_header_working);
    RUN_TEST(test_set_header_key_too_large);
    RUN_TEST(test_set_header_value_too_large);
    RUN_TEST(test_set_header_too_many_headers);

    RUN_TEST(test_parse_request_line);

    RUN_TEST(test_parse_http_request);

    RUN_TEST(test_parse_headers_working);
    RUN_TEST(test_parse_headers_connection_close);
    RUN_TEST(test_parse_headers_connection_not_properly_defined);
    RUN_TEST(test_parse_headers_invalid_format);
    RUN_TEST(test_parse_headers_too_many_headers);
    RUN_TEST(test_parse_headers_two_hosts);
    RUN_TEST(test_parse_headers_no_hosts);

    RUN_TEST(test_build_http_response_working);
    RUN_TEST(test_build_http_response_body_length_disparity);
    RUN_TEST(test_build_http_response_too_many_headers);

    return UNITY_END();
}