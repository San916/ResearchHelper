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
    char** api_response = calloc(1, 1 * sizeof(char*));
    api_response[0] = read_file("..\\tests\\data\\reddit_dummy_response.txt");
    ContentList* comments = calloc(1, sizeof(ContentList));
    if (!comments) {
        return;
    }
    ContentItem* original_post = calloc(1, sizeof(ContentItem));
    if (!original_post) {
        free(comments);
        return;
    }

    TEST_ASSERT_EQUAL_INT(parse_reddit_content(api_response, 0, comments, original_post, max_num_comments, min_score), PARSE_WEBPAGE_CONTENT_BAD);
    TEST_ASSERT_EQUAL_INT(parse_reddit_content(api_response, 2, comments, original_post, max_num_comments, min_score), PARSE_WEBPAGE_CONTENT_BAD);

    ParseWebpageContentError response_code = parse_reddit_content(api_response, 1, comments, original_post, max_num_comments, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(strcmp(original_post->content_body, "\"why does &lt;code&gt;int i = 0&lt;/code&gt;?\""), 0);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 1);
    TEST_ASSERT_EQUAL_INT(strcmp(comments->items[0].content_body, "\"&lt;pre&gt;&lt;code&gt;int i = 0&lt;/code&gt;&lt;/pre&gt;\""), 0);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 9);

    free(api_response[0]);
    free(api_response);
    free(original_post);
    free(comments);
}

void test_parse_reddit_content_truncated(void) {
    char** api_response = calloc(1, 1 * sizeof(char*));
    api_response[0] = read_file("..\\tests\\data\\reddit_dummy_response_5_items.txt");
    ContentList* comments = calloc(1, sizeof(ContentList));
    if (!comments) {
        return;
    }
    ContentItem* original_post = calloc(1, sizeof(ContentItem));
    if (!original_post) {
        free(comments);
        return;
    }

    ParseWebpageContentError response_code = parse_reddit_content(api_response, 1, comments, original_post, 4, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 4);
    for (size_t i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(comments->items[i].content_body, "\"&lt;pre&gt;&lt;code&gt;int i = 0&lt;/code&gt;&lt;/pre&gt;\""), 0);
        TEST_ASSERT_EQUAL_INT(comments->items[i].score, 9);
    }

    free(api_response[0]);
    free(api_response);
    free(original_post);
    free(comments);
}

void test_parse_reddit_content_max_num_comments_too_large(void) {
    char** api_response = calloc(1, 1 * sizeof(char*));
    api_response[0] = read_file("..\\tests\\data\\reddit_dummy_response_5_items.txt");
    ContentList* comments = calloc(1, sizeof(ContentList));
    if (!comments) {
        return;
    }
    ContentItem* original_post = calloc(1, sizeof(ContentItem));
    if (!original_post) {
        free(comments);
        return;
    }

    ParseWebpageContentError response_code = parse_reddit_content(api_response, 1, comments, original_post, 6, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 5);
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(comments->items[i].content_body, "\"&lt;pre&gt;&lt;code&gt;int i = 0&lt;/code&gt;&lt;/pre&gt;\""), 0);
        TEST_ASSERT_EQUAL_INT(comments->items[i].score, 9);
    }

    comments->num_items = 0;
    response_code = parse_reddit_content(api_response, 1, comments, original_post, 5, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 5);
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(comments->items[i].content_body, "\"&lt;pre&gt;&lt;code&gt;int i = 0&lt;/code&gt;&lt;/pre&gt;\""), 0);
        TEST_ASSERT_EQUAL_INT(comments->items[i].score, 9);
    }

    free(api_response[0]);
    free(api_response);
    free(original_post);
    free(comments);
}

void test_parse_reddit_content_score(void) {
    char** api_response = calloc(1, 1 * sizeof(char*));
    api_response[0] = read_file("..\\tests\\data\\reddit_dummy_response_score_descending.txt");
    ContentList* comments = calloc(1, sizeof(ContentList));
    if (!comments) {
        return;
    }
    ContentItem* original_post = calloc(1, sizeof(ContentItem));
    if (!original_post) {
        free(comments);
        return;
    }

    ParseWebpageContentError response_code = parse_reddit_content(api_response, 1, comments, original_post, 3, 0);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[1].score, 2);
    TEST_ASSERT_EQUAL_INT(comments->items[2].score, 1);

    comments->num_items = 0;
    response_code = parse_reddit_content(api_response, 1, comments, original_post, 3, 1);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[1].score, 2);
    TEST_ASSERT_EQUAL_INT(comments->items[2].score, 1);

    comments->num_items = 0;
    response_code = parse_reddit_content(api_response, 1, comments, original_post, 3, 2);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 2);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[1].score, 2);

    comments->num_items = 0;
    response_code = parse_reddit_content(api_response, 1, comments, original_post, 3, 3);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 1);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 3);

    comments->num_items = 0;
    response_code = parse_reddit_content(api_response, 1, comments, original_post, 3, 4);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 0);

    free(api_response[0]);
    free(api_response);
    free(original_post);
    free(comments);
}

// ==================================
// parse_stackoverflow_content
// ==================================
void test_parse_stackoverflow_content(void) {
    char** api_response = calloc(1, 2 * sizeof(char*));
    api_response[0] = read_file("..\\tests\\data\\stackoverflow_dummy_post.txt");
    api_response[1] = read_file("..\\tests\\data\\stackoverflow_dummy_response.txt");
    ContentList* comments = calloc(1, sizeof(ContentList));
    if (!comments) {
        return;
    }
    ContentItem* original_post = calloc(1, sizeof(ContentItem));
    if (!original_post) {
        free(comments);
        return;
    }

    TEST_ASSERT_EQUAL_INT(parse_stackoverflow_content(api_response, 1, comments, original_post, max_num_comments, min_score), PARSE_WEBPAGE_CONTENT_BAD);
    TEST_ASSERT_EQUAL_INT(parse_stackoverflow_content(api_response, 3, comments, original_post, max_num_comments, min_score), PARSE_WEBPAGE_CONTENT_BAD);

    ParseWebpageContentError response_code = parse_stackoverflow_content(api_response, 2, comments, original_post, max_num_comments, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(strcmp(original_post->content_body, "\"why does <code>int i = 0</code>?\""), 0);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 2);
    TEST_ASSERT_EQUAL_INT(strcmp(comments->items[0].content_body, "\"\""), 0);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 42);
    TEST_ASSERT_EQUAL_INT(strcmp(comments->items[1].content_body, "\"<p>This is dummy code below:</p>\\n\\n<pre><code>int i = 0;</code></pre>\\n\""), 0);
    TEST_ASSERT_EQUAL_INT(comments->items[1].score, 5);

    free(api_response[0]);
    free(api_response);
    free(original_post);
    free(comments);
}

void test_parse_stackoverflow_content_truncated(void) {
    char** api_response = calloc(1, 2 * sizeof(char*));
    api_response[0] = read_file("..\\tests\\data\\stackoverflow_dummy_post.txt");
    api_response[1] = read_file("..\\tests\\data\\stackoverflow_dummy_response_5_items.txt");
    ContentList* comments = calloc(1, sizeof(ContentList));
    if (!comments) {
        return;
    }
    ContentItem* original_post = calloc(1, sizeof(ContentItem));
    if (!original_post) {
        free(comments);
        return;
    }

    ParseWebpageContentError response_code = parse_stackoverflow_content(api_response, 2, comments, original_post, 4, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 4);
    for (size_t i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(comments->items[i].content_body, "\"<p>This is dummy code below:</p>\\n\\n<pre><code>int i = 0;</code></pre>\\n\""), 0);
        TEST_ASSERT_EQUAL_INT(comments->items[i].score, 5);
    }

    free(api_response[0]);
    free(api_response);
    free(original_post);
    free(comments);
}

void test_parse_stackoverflow_content_max_num_comments_too_large(void) {
    char** api_response = calloc(1, 2 * sizeof(char*));
    api_response[0] = read_file("..\\tests\\data\\stackoverflow_dummy_post.txt");
    api_response[1] = read_file("..\\tests\\data\\stackoverflow_dummy_response_5_items.txt");
    ContentList* comments = calloc(1, sizeof(ContentList));
    if (!comments) {
        return;
    }
    ContentItem* original_post = calloc(1, sizeof(ContentItem));
    if (!original_post) {
        free(comments);
        return;
    }

    ParseWebpageContentError response_code = parse_stackoverflow_content(api_response, 2, comments, original_post, 6, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 5);
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(comments->items[i].content_body, "\"<p>This is dummy code below:</p>\\n\\n<pre><code>int i = 0;</code></pre>\\n\""), 0);
        TEST_ASSERT_EQUAL_INT(comments->items[i].score, 5);
    }

    comments->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, 2, comments, original_post, 5, min_score);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 5);
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_INT(strcmp(comments->items[i].content_body, "\"<p>This is dummy code below:</p>\\n\\n<pre><code>int i = 0;</code></pre>\\n\""), 0);
        TEST_ASSERT_EQUAL_INT(comments->items[i].score, 5);
    }

    free(api_response[0]);
    free(api_response);
    free(original_post);
    free(comments);
}

void test_parse_stackoverflow_content_score(void) {
    char** api_response = calloc(1, 2 * sizeof(char*));
    api_response[0] = read_file("..\\tests\\data\\stackoverflow_dummy_post.txt");
    api_response[1] = read_file("..\\tests\\data\\stackoverflow_dummy_response_score_descending.txt");
    ContentList* comments = calloc(1, sizeof(ContentList));
    if (!comments) {
        return;
    }
    ContentItem* original_post = calloc(1, sizeof(ContentItem));
    if (!original_post) {
        free(comments);
        return;
    }

    ParseWebpageContentError response_code = parse_stackoverflow_content(api_response, 2, comments, original_post, 3, 0);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[1].score, 2);
    TEST_ASSERT_EQUAL_INT(comments->items[2].score, 1);

    comments->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, 2, comments, original_post, 3, 1);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[1].score, 2);
    TEST_ASSERT_EQUAL_INT(comments->items[2].score, 1);

    comments->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, 2, comments, original_post, 3, 2);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 2);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 3);
    TEST_ASSERT_EQUAL_INT(comments->items[1].score, 2);

    comments->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, 2, comments, original_post, 3, 3);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 1);
    TEST_ASSERT_EQUAL_INT(comments->items[0].score, 3);

    comments->num_items = 0;
    response_code = parse_stackoverflow_content(api_response, 2,  comments, original_post, 3, 4);
    TEST_ASSERT_EQUAL_INT(response_code, PARSE_WEBPAGE_CONTENT_OK);
    TEST_ASSERT_EQUAL_INT(comments->num_items, 0);

    free(api_response[0]);
    free(api_response);
    free(original_post);
    free(comments);
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