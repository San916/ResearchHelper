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
// extract_stackoverflow_question_id
// ==================================
void test_extract_stackoverflow_question_id(void) {
    char* url = "https://stackoverflow.com/questions/21006707/proper-usage-of-realloc";
    char* question_id = extract_stackoverflow_question_id(url);
    TEST_ASSERT_EQUAL_INT(strcmp(question_id, "21006707"), 0);
    free(question_id);

    url = "https://stackoverflow.com/questions//proper-usage-of-realloc";
    question_id = extract_stackoverflow_question_id(url);
    TEST_ASSERT_NULL(question_id);

    url = "https://stackoverflow.com/questions/proper-usage-of-realloc";
    question_id = extract_stackoverflow_question_id(url);
    TEST_ASSERT_NULL(question_id);

    url = "https://stackoverflow.com/question/21006707/proper-usage-of-realloc";
    question_id = extract_stackoverflow_question_id(url);
    TEST_ASSERT_NULL(question_id);
}

// ==================================
// extract_reddit_question_id
// ==================================
void test_extract_reddit_question_id(void) {
    char* url = "https://www.reddit.com/r/cprogramming/comments/1lrkzjb/question_about_realloc/";
    char* question_id = extract_reddit_question_id(url);
    TEST_ASSERT_EQUAL_INT(strcmp(question_id, "1lrkzjb"), 0);
    free(question_id);

    url = "https://www.reddit.com/r/cprogramming/comments//question_about_realloc/";
    question_id = extract_reddit_question_id(url);
    TEST_ASSERT_NULL(question_id);

    url = "https://www.reddit.com/r/cprogramming/comments/question_about_realloc";
    question_id = extract_reddit_question_id(url);
    TEST_ASSERT_NULL(question_id);

    url = "https://www.reddit.com/r/cprogramming/comment/1lrkzjb/question_about_realloc/";
    question_id = extract_reddit_question_id(url);
    TEST_ASSERT_NULL(question_id);
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

    // extract_stackoverflow_question_id
    RUN_TEST(test_extract_stackoverflow_question_id);

    // extract_reddit_question_id
    RUN_TEST(test_extract_reddit_question_id);

    return UNITY_END();
}