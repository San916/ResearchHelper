#include "unity.h"
#include "http.h"

// Define all variables here
char* workingRequest =
        "GET /index.html HTTP/1.1"
        "Host: www.example.com"
        "User-Agent: Mozilla/5.0"
        "Connection: keep-alive";

void setUp(void) {

}

void tearDown(void) {
}

void test_parse_http_request(void) {
    TEST_ASSERT_NULL(parse_http_request(workingRequest, 0));
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_parse_http_request);

    return UNITY_END();
}