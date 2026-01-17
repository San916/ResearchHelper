#include "unity.h"
#include "web_crawler.h"
#include <string.h>
#include <stdlib.h>

// Define all variables here

void setUp(void) {
}

void tearDown(void) {
}

void test_web_crawler_basic(void) {
    // int status_code = 0;
    // char* input = get_google_search_url("What%20is%20C%3F");
    // char* content = fetch_webpage_content(input, &status_code);
    // printf("CONTENT: %s\n", content);
    // free(input);
    // free(content);
}

void test_structure_query_response(void) {
    const char* input = 
    "{\"items\": ["
    "{"
    "\"title\": \"How to fly\","
    "\"link\": \"https://www.flyingforbeginners.com\""
    "},"
    "{"
    "\"title\": \"How to swim\","
    "\"link\": \"https://www.swimmingforbeginners.com\""
    "}"
    "]}";
    QueryResponse* response = structure_query_response(input);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[0].link, "\"https://www.flyingforbeginners.com\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[0].title, "\"How to fly\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[1].link, "\"https://www.swimmingforbeginners.com\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[1].title, "\"How to swim\""), 0);
    free(response);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_web_crawler_basic);
    RUN_TEST(test_structure_query_response);

    return UNITY_END();
}