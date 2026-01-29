#include "unity.h"
#include "web_crawler_config.h"
#include "web_utils.h"
#include <string.h>
#include <stdlib.h>

// Define all variables here
static const char TEST_ENV_FILENAME[] = "..\\tests\\data\\test_env.env";

void setUp(void) {
}

void tearDown(void) {
}

// ==================================
// detect_website_type
// ==================================
void test_detect_website_type(void) {
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.stackoverflow.com/"), WEBSITE_STACKOVERFLOW);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.reddit.com/"), WEBSITE_REDDIT);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.stackexchange.com/"), WEBSITE_STACKOVERFLOW);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.github.com/"), WEBSITE_GITHUB);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.smackoverflow.com/"), WEBSITE_UNKNOWN);
}

// ==================================
// get_google_search_url
// ==================================
void test_get_google_search_url(void) {
    load_env(TEST_ENV_FILENAME);

    char* expected_url = "https://www.googleapis.com/customsearch/v1?key=GOOGLE_SEARCH_API_VALUE&cx=GOOGLE_SEARCH_ENGINE_VALUE&q=How%20do%20birds%20fly%3F";
    char* search_url = get_google_search_url("How%20do%20birds%20fly%3F");
    TEST_ASSERT_NOT_NULL(search_url);
    TEST_ASSERT_EQUAL_INT(strcmp(search_url, expected_url), 0);
    free(search_url);
}

void test_get_google_search_url_env_not_loaded(void) {
    _putenv_s("GOOGLE_SEARCH_API_KEY", "");
    char* expected_url = "https://www.googleapis.com/customsearch/v1?key=GOOGLE_SEARCH_API_VALUE&cx=GOOGLE_SEARCH_ENGINE_VALUE&q=How%20do%20birds%20fly%3F";
    char* search_url = get_google_search_url("How%20do%20birds%20fly%3F");
    TEST_ASSERT_NULL(search_url);

    load_env(TEST_ENV_FILENAME);
    _putenv_s("GOOGLE_SEARCH_ENGINE", "");
    expected_url = "https://www.googleapis.com/customsearch/v1?key=GOOGLE_SEARCH_API_VALUE&cx=GOOGLE_SEARCH_ENGINE_VALUE&q=How%20do%20birds%20fly%3F";
    search_url = get_google_search_url("How%20do%20birds%20fly%3F");
    TEST_ASSERT_NULL(search_url);
}

void test_get_google_search_url_truncation(void) {
    load_env(TEST_ENV_FILENAME);

    char* url_start = "https://www.googleapis.com/customsearch/v1?key=GOOGLE_SEARCH_API_VALUE&cx=GOOGLE_SEARCH_ENGINE_VALUE&q=";
    size_t url_end_len = MAX_CURL_URL_LEN - strlen(url_start);
    char* url_end = calloc(1, url_end_len + 1);
    memset(url_end, 'A', url_end_len);

    char* search_url = get_google_search_url(url_end);
    TEST_ASSERT_NOT_NULL(search_url);
    TEST_ASSERT_EQUAL_INT(strlen(search_url), MAX_CURL_URL_LEN - 1);
    free(url_end);
    free(search_url);
}

// ==================================
// extract_reddit_question_id
// ==================================
void test_extract_reddit_question_id(void) {
    char* url = "https://www.reddit.com/r/cprogramming/comments/1lrkzjb/question_about_realloc/";
    char question_id[32] = {0};
    TEST_ASSERT_EQUAL_INT(extract_reddit_question_id(url, question_id, 32), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(question_id, "1lrkzjb"), 0);

    url = "https://www.reddit.com/r/cprogramming/comments//question_about_realloc/";
    memset(question_id, 0, strlen(question_id));
    TEST_ASSERT_EQUAL_INT(extract_reddit_question_id(url, question_id, 32), 0);
    TEST_ASSERT_EQUAL_INT(strlen(question_id), 0);

    url = "https://www.reddit.com/r/cprogramming/comments/question_about_realloc";
    memset(question_id, 0, strlen(question_id));
    TEST_ASSERT_EQUAL_INT(extract_reddit_question_id(url, question_id, 32), -1);
    TEST_ASSERT_EQUAL_INT(strlen(question_id), 0);

    url = "https://www.reddit.com/r/cprogramming/comment/1lrkzjb/question_about_realloc/";
    memset(question_id, 0, strlen(question_id));
    TEST_ASSERT_EQUAL_INT(extract_reddit_question_id(url, question_id, 32), -1);
    TEST_ASSERT_EQUAL_INT(strlen(question_id), 0);
}

// ==================================
// setup_reddit_url
// ==================================
void test_setup_reddit_url(void) {
    char* url = "https://www.reddit.com/r/cprogramming/comments/1lrkzjb/question_about_realloc/";
    CURL* curl_handle = create_curl_handle();
    struct curl_slist* headers = create_curl_headers();
    WebsiteType website_type = detect_website_type(url);
    int escaped = 0;

    load_env(TEST_ENV_FILENAME);
    size_t num_urls = 0;
    char* new_url = setup_reddit_url(url, curl_handle, &headers, &escaped, 10, &num_urls);
    char expected_url[256] = {0};
    snprintf(expected_url, 255, "https://www.reddit.com/comments/1lrkzjb.json?limit=10&depth=%d&sort=top", REDDIT_API_DEPTH);
    TEST_ASSERT_EQUAL_INT(strcmp(new_url, expected_url), 0);
    TEST_ASSERT_EQUAL_INT(escaped, 1);
    TEST_ASSERT_EQUAL_INT(num_urls, 1);
    free(new_url);
}

// ==================================
// extract_stackoverflow_question_id
// ==================================
void test_extract_stackoverflow_question_id(void) {
    char* url = "https://stackoverflow.com/questions/21006707/proper-usage-of-realloc";
    char question_id[32] = {0};
    TEST_ASSERT_EQUAL_INT(extract_stackoverflow_question_id(url, question_id, 32), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(question_id, "21006707"), 0);

    url = "https://stackoverflow.com/questions//proper-usage-of-realloc";
    memset(question_id, 0, strlen(question_id));
    TEST_ASSERT_EQUAL_INT(extract_stackoverflow_question_id(url, question_id, 32), 0);
    TEST_ASSERT_EQUAL_INT(strlen(question_id), 0);

    url = "https://stackoverflow.com/questions/proper-usage-of-realloc";
    memset(question_id, 0, strlen(question_id));
    TEST_ASSERT_EQUAL_INT(extract_stackoverflow_question_id(url, question_id, 32), -1);
    TEST_ASSERT_EQUAL_INT(strlen(question_id), 0);

    url = "https://stackoverflow.com/question/21006707/proper-usage-of-realloc";
    memset(question_id, 0, strlen(question_id));
    TEST_ASSERT_EQUAL_INT(extract_stackoverflow_question_id(url, question_id, 32), -1);
    TEST_ASSERT_EQUAL_INT(strlen(question_id), 0);
}

// ==================================
// extract_stackoverflow_site
// ==================================
void test_extract_stackoverflow_site(void) {
    char* url = "https://stackoverflow.com/questions/21006707/proper-usage-of-realloc";
    char site[32] = {0};
    TEST_ASSERT_EQUAL_INT(extract_stackoverflow_site(url, site, 32), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(site, "stackoverflow"), 0);

    url = "https://gamedev.stackexchange.com/questions/13436/glm-euler-angles-to-quaternion";
    memset(site, 0, strlen(site));
    TEST_ASSERT_EQUAL_INT(extract_stackoverflow_site(url, site, 32), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(site, "gamedev"), 0);
}

// ==================================
// setup_stackoverflow_url
// ==================================
void test_setup_stackoverflow_url(void) {
    char* url = "https://stackoverflow.com/questions/21006707/proper-usage-of-realloc";
    CURL* curl_handle = create_curl_handle();
    struct curl_slist* headers = create_curl_headers();
    WebsiteType website_type = detect_website_type(url);
    int escaped = 0;
    size_t num_urls = 0;

    char** new_urls = setup_stackoverflow_urls(url, curl_handle, &headers, &escaped, &num_urls);
    TEST_ASSERT_EQUAL_INT(strcmp(new_urls[1], "https://api.stackexchange.com/2.3/questions/21006707/answers?site=stackoverflow&order=desc&sort=votes&filter=withbody"), 0);
    TEST_ASSERT_EQUAL_INT(escaped, 0);
    TEST_ASSERT_EQUAL_INT(num_urls, 2);
    free(new_urls[0]);
    free(new_urls[1]);
    free(new_urls);
}

int main(void) {
    UNITY_BEGIN();

    // Test
    // detect_website_type
    RUN_TEST(test_detect_website_type);

    // get_google_search_url
    RUN_TEST(test_get_google_search_url);
    RUN_TEST(test_get_google_search_url_env_not_loaded);
    RUN_TEST(test_get_google_search_url_truncation);

    // extract_reddit_question_id
    RUN_TEST(test_extract_reddit_question_id);

    // setup_reddit_url
    RUN_TEST(test_setup_reddit_url);

    // extract_stackoverflow_question_id
    RUN_TEST(test_extract_stackoverflow_question_id);

    // extract_stackoverflow_site
    RUN_TEST(test_extract_stackoverflow_site);

    // setup_stackoverflow_url
    RUN_TEST(test_setup_stackoverflow_url);


    return UNITY_END();
}