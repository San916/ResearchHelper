#include <string.h>
#include <stdlib.h>

#include "unity.h"
#include "content_formatting.h"
#include "webpage_parsing.h"

// Define all variables here
static int MAX_RESPONSE_LENGTH = 1024;

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
                "\"content_body\":\"<code>int i = 0;</code>\","
                "\"score\":10"
            "}"
        "],"
        "\"count\":1"
    "}";


void setUp(void) {
}

void tearDown(void) {
}

// ==================================
// parse_google_query_response
// ==================================

void test_parse_google_query_response(void) {
    QueryResponse* response = parse_google_query_response(google_query_response, 10);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[0].link, "\"https://www.flyingforbeginners.com\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[0].title, "\"How to fly\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[1].link, "\"https://www.swimmingforbeginners.com\""), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(response->responses[1].title, "\"How to swim\""), 0);
    free(response);
}

void test_parse_google_query_response_bad_max_num_responses(void) {
    QueryResponse* response = parse_google_query_response(google_query_response, -1);
    TEST_ASSERT_NULL(response);

    const char* query_response_start = "{\"items\": [";
    const char* query_response_item = "{\"title\": \"How to fly\",\"link\": \"https://www.flyingforbeginners.com\"}";
    const char* query_response_end = "]}";
    
    size_t length = strlen(query_response_start) + (MAX_NUM_RESPONSES + 1) * strlen(query_response_item) + strlen(query_response_end) + 1;
    char* google_query_response_large = calloc(1, length);
    if (!google_query_response_large) {
        return;
    }

    strcpy(google_query_response_large, query_response_start);
    for (int i = 0; i < MAX_NUM_RESPONSES + 1; i++) {
        strcpy(google_query_response_large + strlen(google_query_response_large), query_response_item);
    }
    strcpy(google_query_response_large + strlen(google_query_response_large), query_response_end);

    response = parse_google_query_response(google_query_response_large, MAX_NUM_RESPONSES);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(response->num_responses, MAX_NUM_RESPONSES);
    for (int i = 0; i < MAX_NUM_RESPONSES; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(response->responses[i].link, "\"https://www.flyingforbeginners.com\""), 0);
        TEST_ASSERT_EQUAL_INT(strcmp(response->responses[i].title, "\"How to fly\""), 0);
    }
}

// ==================================
// stringify_google_query_response
// ==================================

void test_stringify_google_query_response(void) {
    QueryResponse* response = parse_google_query_response(google_query_response, 10);
    TEST_ASSERT_NOT_NULL(response);
    char* response_json = stringify_google_query_response(response, MAX_RESPONSE_LENGTH);
    TEST_ASSERT_NOT_NULL(response_json);
    TEST_ASSERT_EQUAL_INT(strcmp(response_json, expected_json_response), 0);
    free(response_json);
    free(response);
}

void test_stringify_google_query_response_max_length_too_small(void) {
    QueryResponse* response = parse_google_query_response(google_query_response, 10);
    TEST_ASSERT_NOT_NULL(response);

    char* expected_json_response_1 = 
        "{\"results\":"
            "["
                "{"
                    "\"title\":\"How to fly\","
                    "\"link\":\"https://www.flyingforbeginners.com\""
                "}"
            "]"
        "}";
    char* response_json = stringify_google_query_response(response, strlen(expected_json_response_1) + 1);
    TEST_ASSERT_NOT_NULL(response_json);
    TEST_ASSERT_EQUAL_INT(strcmp(response_json, expected_json_response_1), 0);
    free(response_json);

    response_json = stringify_google_query_response(response, strlen(expected_json_response_1));
    TEST_ASSERT_NULL(response_json);

    response_json = stringify_google_query_response(response, strlen(expected_json_response_1) + 5);
    TEST_ASSERT_NOT_NULL(response_json);
    TEST_ASSERT_EQUAL_INT(strcmp(response_json, expected_json_response_1), 0);
    free(response_json);

    free(response);
}

// ==================================
// structure_google_query_response
// ==================================

void test_structure_google_query_response(void) {
    char* response = structure_google_query_response(google_query_response, MAX_RESPONSE_LENGTH, 10);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(strcmp(response, expected_json_response), 0);
    free(response);
}

// ==================================
// parse_webpage_content
// ==================================

void test_parse_webpage_content(void) {
    ContentList* response = parse_webpage_content("", WEBSITE_STUB);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(strcmp(response->items[0].content_body, "\"<code>int i = 0;</code>\""), 0);
    TEST_ASSERT_EQUAL_INT(response->items[0].score, 10);
    free(response);
}

// ==================================
// stringify_content_response
// ==================================

void test_stringify_content_response(void) {
    ContentList* response = parse_webpage_content("", WEBSITE_STUB);
    TEST_ASSERT_NOT_NULL(response);
    char* response_json = stringify_content_response(response, MAX_RESPONSE_LENGTH);
    TEST_ASSERT_NOT_NULL(response_json);
    TEST_ASSERT_EQUAL_INT(strcmp(response_json, expected_json_content_response), 0);
    free(response);
    free(response_json);
}

// ==================================
// structure_webpage_content_response
// ==================================

void test_structure_webpage_content_response(void) {
    char* response = structure_webpage_content_response("", WEBSITE_STUB, MAX_RESPONSE_LENGTH);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_EQUAL_INT(strcmp(response, expected_json_content_response), 0);
    free(response);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    // parse_google_query_response
    RUN_TEST(test_parse_google_query_response);
    RUN_TEST(test_parse_google_query_response_bad_max_num_responses);

    // stringify_google_query_response
    RUN_TEST(test_stringify_google_query_response);
    RUN_TEST(test_stringify_google_query_response_max_length_too_small);

    // structure_google_query_response
    RUN_TEST(test_structure_google_query_response);


    // parse_webpage_content
    RUN_TEST(test_parse_webpage_content);

    // stringify_content_response
    RUN_TEST(test_stringify_content_response);

    // structure_webpage_content_response
    RUN_TEST(test_structure_webpage_content_response);

    return UNITY_END();
}