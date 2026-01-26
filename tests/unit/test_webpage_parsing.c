#include "unity.h"
#include "test_webpage_parsing_utils.h"
#include "webpage_parsing.h"

#include <stdlib.h>

// Define all variables here
static size_t max_num_comments = 10;
static size_t min_score = 0;

void setUp(void) {
}

void tearDown(void) {
}

// ==================================
// parse_reddit_content
// ==================================
void test_parse_reddit_content(void) {
    char* api_response = read_file("..\\tests\\data\\reddit_dummy_response.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    ParseWebpageContentError response_code = parse_reddit_content(api_response, content_list, max_num_comments, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 1);
    TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[0].content_body, "\"&lt;pre&gt;&lt;code&gt;int i = 0&lt;/code&gt;&lt;/pre&gt;\""), 0);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 9);

    free(api_response);
    free(content_list);
}

void test_parse_reddit_content_truncated(void) {
    char* api_response = read_file("..\\tests\\data\\reddit_dummy_response_5_items.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    ParseWebpageContentError response_code = parse_reddit_content(api_response, content_list, 4, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 4);
    for (size_t i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[i].content_body, "\"&lt;pre&gt;&lt;code&gt;int i = 0&lt;/code&gt;&lt;/pre&gt;\""), 0);
        TEST_ASSERT_EQUAL_INT(content_list->items[i].score, 9);
    }

    free(api_response);
    free(content_list);
}

void test_parse_reddit_content_max_num_comments_too_large(void) {
    char* api_response = read_file("..\\tests\\data\\reddit_dummy_response_5_items.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    ParseWebpageContentError response_code = parse_reddit_content(api_response, content_list, 6, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 5);
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[i].content_body, "\"&lt;pre&gt;&lt;code&gt;int i = 0&lt;/code&gt;&lt;/pre&gt;\""), 0);
        TEST_ASSERT_EQUAL_INT(content_list->items[i].score, 9);
    }

    content_list->num_items = 0;
    response_code = parse_reddit_content(api_response, content_list, 5, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 5);
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[i].content_body, "\"&lt;pre&gt;&lt;code&gt;int i = 0&lt;/code&gt;&lt;/pre&gt;\""), 0);
        TEST_ASSERT_EQUAL_INT(content_list->items[i].score, 9);
    }

    free(api_response);
    free(content_list);
}

void test_parse_reddit_content_score(void) {
    char* api_response = read_file("..\\tests\\data\\reddit_dummy_response_score_descending.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    ParseWebpageContentError response_code = parse_reddit_content(api_response, content_list, 3, 0);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[1].score, 2);
    TEST_ASSERT_EQUAL_INT(content_list->items[2].score, 1);

    content_list->num_items = 0;
    response_code = parse_reddit_content(api_response, content_list, 3, 1);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[1].score, 2);
    TEST_ASSERT_EQUAL_INT(content_list->items[2].score, 1);

    content_list->num_items = 0;
    response_code = parse_reddit_content(api_response, content_list, 3, 2);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 2);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[1].score, 2);

    content_list->num_items = 0;
    response_code = parse_reddit_content(api_response, content_list, 3, 3);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 1);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 3);

    content_list->num_items = 0;
    response_code = parse_reddit_content(api_response, content_list, 3, 4);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 0);

    free(api_response);
    free(content_list);
}

// ==================================
// parse_stackoverflow_content
// ==================================
void test_parse_stackoverflow_content(void) {
    char* api_response = read_file("..\\tests\\data\\stackoverflow_dummy_response.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    ParseWebpageContentError response_code = parse_stackoverflow_content(api_response, content_list, max_num_comments, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 2);
    TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[0].content_body, "\"\""), 0);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 42);
    TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[1].content_body, "\"<p>This is dummy code below:</p>\\n\\n<pre><code>int i = 0;</code></pre>\\n\""), 0);
    TEST_ASSERT_EQUAL_INT(content_list->items[1].score, 5);

    free(api_response);
    free(content_list);
}

void test_parse_stackoverflow_content_truncated(void) {
    char* api_response = read_file("..\\tests\\data\\stackoverflow_dummy_response_5_items.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    ParseWebpageContentError response_code = parse_stackoverflow_content(api_response, content_list, 4, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 4);
    for (size_t i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[i].content_body, "\"<p>This is dummy code below:</p>\\n\\n<pre><code>int i = 0;</code></pre>\\n\""), 0);
        TEST_ASSERT_EQUAL_INT(content_list->items[i].score, 5);
    }

    free(api_response);
    free(content_list);
}

void test_parse_stackoverflow_content_max_num_comments_too_large(void) {
    char* api_response = read_file("..\\tests\\data\\stackoverflow_dummy_response_5_items.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    ParseWebpageContentError response_code = parse_stackoverflow_content(api_response, content_list, 6, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 5);
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[i].content_body, "\"<p>This is dummy code below:</p>\\n\\n<pre><code>int i = 0;</code></pre>\\n\""), 0);
        TEST_ASSERT_EQUAL_INT(content_list->items[i].score, 5);
    }

    content_list->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, content_list, 5, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 5);
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(content_list->items[i].content_body, "\"<p>This is dummy code below:</p>\\n\\n<pre><code>int i = 0;</code></pre>\\n\""), 0);
        TEST_ASSERT_EQUAL_INT(content_list->items[i].score, 5);
    }

    free(api_response);
    free(content_list);
}

void test_parse_stackoverflow_content_score(void) {
    char* api_response = read_file("..\\tests\\data\\stackoverflow_dummy_response_score_descending.txt");
    ContentList* content_list = calloc(1, sizeof(ContentList));

    ParseWebpageContentError response_code = parse_stackoverflow_content(api_response, content_list, 3, 0);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[1].score, 2);
    TEST_ASSERT_EQUAL_INT(content_list->items[2].score, 1);

    content_list->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, content_list, 3, 1);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[1].score, 2);
    TEST_ASSERT_EQUAL_INT(content_list->items[2].score, 1);

    content_list->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, content_list, 3, 2);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 2);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(content_list->items[1].score, 2);

    content_list->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, content_list, 3, 3);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 1);
    TEST_ASSERT_EQUAL_INT(content_list->items[0].score, 3);

    content_list->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, content_list, 3, 4);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(content_list->num_items, 0);

    free(api_response);
    free(content_list);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    // parse_reddit_content
    RUN_TEST(test_parse_reddit_content);
    RUN_TEST(test_parse_reddit_content_truncated);
    RUN_TEST(test_parse_reddit_content_max_num_comments_too_large);
    RUN_TEST(test_parse_reddit_content_score);
    
    // parse_stackoverflow_content
    RUN_TEST(test_parse_stackoverflow_content);
    RUN_TEST(test_parse_stackoverflow_content_truncated);
    RUN_TEST(test_parse_stackoverflow_content_max_num_comments_too_large);
    RUN_TEST(test_parse_stackoverflow_content_score);

    return UNITY_END();
}