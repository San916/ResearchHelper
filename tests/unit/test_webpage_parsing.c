#include "unity.h"
#include "test_webpage_parsing_utils.h"
#include "webpage_parsing.h"

#include <stdlib.h>

// Define all variables here

void setUp(void) {
}

void tearDown(void) {
}

void test_parse_reddit_content(void) {
    char* api_response = read_file("..\\tests\\data\\reddit_dummy_response.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    int response_code = parse_reddit_content(api_response, content_list);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 1);
    TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[0].content_body, "\"&lt;pre&gt;&lt;code&gt;int i = 0&lt;/code&gt;&lt;/pre&gt;\""), 0);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 9);
}

void test_parse_stackoverflow_content(void) {
    char* api_response = read_file("..\\tests\\data\\stackoverflow_dummy_response.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    int response_code = parse_stackoverflow_content(api_response, content_list);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 2);
    TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[0].content_body, "\"\""), 0);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 42);
    TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[1].content_body, "\"<p>This is dummy code below:</p>\\n\\n<pre><code>int i = 0;</code></pre>\\n\""), 0);
    TEST_ASSERT_EQUAL_INT(content_list->items[1].score, 5);

    free(api_response);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_parse_stackoverflow_content);
    RUN_TEST(test_parse_reddit_content);

    return UNITY_END();
}