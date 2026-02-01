#include "unity.h"

#include "web_crawler.h"
#include "web_utils.h"
#include "web_crawler_config.h"
#include "content_formatting.h"
#include "web_crawler_priv.h"

#include "test_webpage_parsing_utils.h"

#include <string.h>
#include <stdlib.h>

// Define all variables here
char* mock_content_list_fetch_webpage_content(const char* url, int* status_code, CURL* curl_handle, struct curl_slist* headers) {
    char* response = read_file("..\\tests\\data\\google_query_dummy_curl_response.txt");
    return response;
}

char* mock_content_item_fetch_webpage_content(const char* url, int* status_code, CURL* curl_handle, struct curl_slist* headers) {
    char* response = NULL;
    if (!strcmp(url, "https://api.stackexchange.com/2.3/questions/13748338/?site=stackoverflow&filter=withbody")) {
        response = read_file("..\\tests\\data\\get_content_item_dummy_curl_response_post.txt");
    } else if (!strcmp(url, "https://api.stackexchange.com/2.3/questions/13748338/answers?site=stackoverflow&order=desc&sort=votes&filter=withbody")) {
        response = read_file("..\\tests\\data\\get_content_item_dummy_curl_response_comments.txt");
    }
    return response;
}

static const char TEST_ENV_FILENAME[] = "..\\tests\\data\\test_env.env";

static struct GetContentListHandle* content_list_handle = NULL;
static struct GetContentItemHandle* content_item_handle = NULL;

void setUp(void) {
    content_list_handle = calloc(1, sizeof(struct GetContentListHandle));
    if (!content_list_handle) return;

    content_list_handle->max_content_length = 65536;
    content_list_handle->max_num_responses = 5;
    content_list_handle->env_path = TEST_ENV_FILENAME;
    content_list_handle->create_curl_handle = create_curl_handle;
    content_list_handle->load_env = load_env;
    content_list_handle->get_search_url = get_google_search_url;
    content_list_handle->fetch_webpage_content = mock_content_list_fetch_webpage_content;
    content_list_handle->structure_google_query_response = structure_google_query_response;
    content_list_handle->destroy_curl_handle = destroy_curl_handle;

    content_item_handle = calloc(1, sizeof(struct GetContentItemHandle));
    if (!content_list_handle) return;

    content_item_handle->max_content_length = 65536;
    content_item_handle->max_num_comments = 5;
    content_item_handle->min_score = 0;
    content_item_handle->env_path = TEST_ENV_FILENAME;
    content_item_handle->create_curl_handle = create_curl_handle;
    content_item_handle->create_curl_headers = create_curl_headers;
    content_item_handle->detect_website_type = detect_website_type;
    content_item_handle->load_env = load_env;
    content_item_handle->web_specific_setup = web_specific_setup;
    content_item_handle->fetch_webpage_content = mock_content_item_fetch_webpage_content;
    content_item_handle->structure_webpage_content_response = structure_webpage_content_response;
    content_item_handle->destroy_curl_handle = destroy_curl_handle;
    content_item_handle->destroy_curl_headers = destroy_curl_headers;
}

void tearDown(void) {
    if (content_list_handle) free(content_list_handle);
    content_list_handle = NULL;

    if (content_item_handle) free(content_item_handle);
    content_item_handle = NULL;
}

// ==================================
// get_content_list
// ==================================
void test_get_content_list(void) {
    const char* query = "How%20to%20use%20realloc()";
    int status_code = 0;

    char* response = get_content_list(query, &status_code, content_list_handle);
    TEST_ASSERT_NOT_NULL(response);

    char* expected_response = read_file("..\\tests\\data\\get_content_list_expected_response.txt");
    TEST_ASSERT_EQUAL_INT(strcmp(response, expected_response), 0);
    free(expected_response);
    free(response);
}

// ==================================
// get_content_item
// ==================================
void test_get_content_item(void) {
    const char* url = "https://stackoverflow.com/questions/13748338/how-to-use-realloc-in-a-function-in-c";
    int status_code = 0;
    int escaped = 0;

    char* response = get_content_item(url, &status_code, &escaped, content_item_handle);
    TEST_ASSERT_NOT_NULL(response);

    char* expected_response = read_file("..\\tests\\data\\get_content_item_expected_response.txt");
    TEST_ASSERT_EQUAL_INT(strcmp(response, expected_response), 0);

    free(expected_response);
    free(response);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    //get_content_list
    RUN_TEST(test_get_content_list);

    // get_content_item
    RUN_TEST(test_get_content_item);

    return UNITY_END();
}