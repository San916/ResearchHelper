#include "unity.h"
#include "http.h"
#include "router.h"
#include "handlers.h"
#include "web_crawler.h"
#include "json.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Define all variables here
static HttpResponse resp = {0};
static HttpRequest* req = NULL;

void setUp(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;
}

void tearDown(void) {
    if (resp.body) {
        free(resp.body);
        resp.body = NULL;
    }
    if (req && req->body) {
        free(req->body);
        req->body = NULL;
    }
    if (req) {
        free(req);
        req = NULL;
    }
}

// ==================================
// handle_submit
// ==================================
void test_handle_submit(void) {
    strcpy(req->method, "POST");
    req->body = calloc(1, strlen("user_input=How%20to%20use%20realloc()") + 1);
    strcpy(req->body, "user_input=How%20to%20use%20realloc()");
    req->max_num_responses = 3;

    resp = handle_submit(req);
    TEST_ASSERT_EQUAL_INT(resp.status_code, 200);
    
    char* results = get_json_value(resp.body, "results");
    TEST_ASSERT_NOT_NULL(results);

    size_t num_elements = 0;
    char** results_array = separate_array(results, &num_elements, req->max_num_responses + 1);
    TEST_ASSERT_NOT_NULL(results_array);
    TEST_ASSERT_EQUAL_size_t(num_elements, req->max_num_responses);
    for (size_t i = 0; i < num_elements; i++) {
        char* title = get_json_value(results_array[i], "title");
        char* link = get_json_value(results_array[i], "link");

        TEST_ASSERT_NOT_NULL(title);
        TEST_ASSERT_NOT_NULL(link);

        free(title);
        free(link);
        free(results_array[i]);
    }
    free(results);
    free(results_array);
}

// ==================================
// handle_content_request
// ==================================
void test_handle_content_request(void) {
    strcpy(req->method, "GET");
    strcpy(req->path, "/content?url=https%3A%2F%2Fstackoverflow.com%2Fquestions%2F13748338%2Fhow-to-use-realloc-in-a-function-in-c");
    req->max_num_comments = 3;
    req->min_score = 0;
    
    resp = handle_content_request(req);
    TEST_ASSERT_EQUAL_INT(resp.status_code, 200);

    char* original_post = get_json_value(resp.body, "original_post");
    TEST_ASSERT_NOT_NULL(original_post);
    char* original_post_content_body = get_json_value(original_post, "content_body");
    char* original_post_score = get_json_value(original_post, "score");
    TEST_ASSERT_NOT_NULL(original_post_content_body);
    TEST_ASSERT_NOT_NULL(original_post_score);
    free(original_post_content_body);
    free(original_post_score);
    free(original_post);

    char* comment_count_string = get_json_value(resp.body, "comment_count");
    TEST_ASSERT_NOT_NULL(comment_count_string);
    char *end_ptr;
    size_t comment_count = strtoull(comment_count_string, &end_ptr, 10);
    TEST_ASSERT_EQUAL_size_t(comment_count, req->max_num_comments);
    free(comment_count_string);

    
    char* comments = get_json_value(resp.body, "comments");
    TEST_ASSERT_NOT_NULL(comments);

    size_t num_elements = 0;
    char** comments_array = separate_array(comments, &num_elements, comment_count);
    TEST_ASSERT_NOT_NULL(comments_array);
    TEST_ASSERT_EQUAL_size_t(num_elements, comment_count);

    for (size_t i = 0; i < num_elements; i++) {
        char* content_body = get_json_value(comments_array[i], "content_body");
        char* score = get_json_value(comments_array[i], "score");

        TEST_ASSERT_NOT_NULL(content_body);
        TEST_ASSERT_NOT_NULL(score);

        free(content_body);
        free(score);
        free(comments_array[i]);
    }

    free(comments);
    free(comments_array);
}

int main() {
    UNITY_BEGIN();

    // Tests
    // handle_submit
    RUN_TEST(test_handle_submit);

    // handle_content_request
    RUN_TEST(test_handle_content_request);

    return UNITY_END();
}