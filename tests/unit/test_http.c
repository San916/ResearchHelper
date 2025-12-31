#include "unity.h"
#include "http.h"
#include "test_http_utils.h"
#include "router.h"
#include <stdlib.h>

#define UNITY_PRINT_TEST_CONTEXT
#define UNITY_OUTPUT_COLOR 1

// Define all variables here
static const char* workingRequest =
        "GET /index.html HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "User-Agent: Mozilla/5.0\r\n"
        "Connection: keep-alive\r\n";

static HttpRequest* req = NULL;

void setUp(void) {
}

void tearDown(void) {
    if (req) free(req);
    req = NULL; // Avoid dangling pointer
}

void test_parse_request_line(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    char* line = "GET /index.html HTTP/1.1";
    parse_request_line(line, req);
    TEST_ASSERT_EQUAL_INT(strcmp(req->method, "GET"), 0);    
    TEST_ASSERT_EQUAL_INT(strcmp(req->path, "/index.html"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->version, "HTTP/1.1"), 0);
}

void test_parse_http_request(void) {
    TEST_ASSERT_NULL(parse_http_request(workingRequest, 0));

    req = parse_http_request(workingRequest, strlen(workingRequest));
    TEST_ASSERT_EQUAL_INT(strcmp(req->method, "GET"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->path, "/index.html"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->version, "HTTP/1.1"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[0].key, "Host"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[0].value, "www.example.com"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[1].key, "User-Agent"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(req->headers[1].value, "Mozilla/5.0"), 0);
    TEST_ASSERT_TRUE(req->keep_alive);
}

int main() {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_parse_request_line);
    RUN_TEST(test_parse_http_request);

    return UNITY_END();
}