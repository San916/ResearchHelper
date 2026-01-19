#include "unity.h"
#include "content_formatting.h"
#include <string.h>
#include <stdlib.h>

// Define all variables here
#define MAX_RESPONSE_LENGTH 1024

static char google_query_response[] = 
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

static char expected_json_response[] =
    "{\"results\":"
        "["
            "{"
                "\"title\":\"How to fly\","
                "\"link\":\"https://www.flyingforbeginners.com\""
            "},"
            "{"
                "\"title\":\"How to swim\","
                "\"link\":\"https://www.swimmingforbeginners.com\""
            "}"
        "]"
    "}";

static char expected_json_content_response[] =
    "{\"content\":"
        "["
            "{"
                "\"code\":\"```int i = 0;```\","
                "\"discussion\":\"This is a variable.\","
                "\"score\":10"
            "},"
            "{"
                "\"code\":\"\","
                "\"discussion\":\"There is no code, and no score.\","
                "\"score\":0"
            "}"
        "],"
        "\"count\":2"
    "}";


void setUp(void) {
}

void tearDown(void) {
}

void test_parse_google_query_response(void) {
    QueryResponse* response = parse_google_query_response(google_query_response);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[0].link, "\"https://www.flyingforbeginners.com\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[0].title, "\"How to fly\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[1].link, "\"https://www.swimmingforbeginners.com\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[1].title, "\"How to swim\""), 0);
    free(response);
}

void test_stringify_google_query_response(void) {
    QueryResponse* response = parse_google_query_response(google_query_response);
    TEST_ASSERT_NOT_NULL(response);
    char* response_json = stringify_google_query_response(response, MAX_RESPONSE_LENGTH);
    TEST_ASSERT_NOT_NULL(response_json);
    TEST_ASSERT_EQUAL_INT(strcmp(response_json, expected_json_response), 0);
    free(response);
    free(response_json);
}

void test_structure_google_query_response(void) {
    char* response = structure_google_query_response(google_query_response, MAX_RESPONSE_LENGTH);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(strcmp(response, expected_json_response), 0);
    free(response);
}

void test_parse_webpage_content(void) {
    ContentList* response = parse_webpage_content("", WEBSITE_STUB);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(strcmp(response->items[0].code, "\"```int i = 0;```\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->items[0].discussion, "\"This is a variable.\""), 0);
    TEST_ASSERT_EQUAL_INT(response->items[0].score, 10);
    TEST_ASSERT_EQUAL_INT(strcmp(response->items[1].code, "\"\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->items[1].discussion, "\"There is no code, and no score.\""), 0);
    TEST_ASSERT_EQUAL_INT(response->items[1].score, 0);
    free(response);
}

void test_stringify_content_response(void) {
    ContentList* response = parse_webpage_content("", WEBSITE_STUB);
    TEST_ASSERT_NOT_NULL(response);
    char* response_json = stringify_content_response(response, MAX_RESPONSE_LENGTH);
    TEST_ASSERT_NOT_NULL(response_json);
    TEST_ASSERT_EQUAL_INT(strcmp(response_json, expected_json_content_response), 0);
    free(response);
    free(response_json);
}

void test_structure_webpage_content_response(void) {
    char* response = structure_webpage_content_response("", WEBSITE_STUB, MAX_RESPONSE_LENGTH);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(strcmp(response, expected_json_content_response), 0);
    free(response);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_parse_google_query_response);
    RUN_TEST(test_stringify_google_query_response);
    RUN_TEST(test_structure_google_query_response);

    RUN_TEST(test_parse_webpage_content);
    RUN_TEST(test_stringify_content_response);
    RUN_TEST(test_structure_webpage_content_response);

    return UNITY_END();
}