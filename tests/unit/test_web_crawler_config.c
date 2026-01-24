#include "unity.h"
#include "web_utils.h"
#include "web_crawler_config.h"
#include <string.h>
#include <stdlib.h>

// Define all variables here
static const char TEST_ENV_FILENAME[] = "..\\tests\\data\\test_env.env";

void setUp(void) {
}

void tearDown(void) {
}

void test_detect_website_type(void) {
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.stackoverflow.com/"), WEBSITE_STACKOVERFLOW);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.reddit.com/"), WEBSITE_REDDIT);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.stackexchange.com/"), WEBSITE_STACKOVERFLOW);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.github.com/"), WEBSITE_GITHUB);
}

void test_get_google_search_url(void) {
    load_env(TEST_ENV_FILENAME);

    char* expected_url = "https://www.googleapis.com/customsearch/v1?key=GOOGLE_SEARCH_API_VALUE&cx=GOOGLE_SEARCH_ENGINE_VALUE&q=How%20do%20birds%20fly%3F";
    char* search_url = get_google_search_url("How%20do%20birds%20fly%3F");
    TEST_ASSERT_NOT_NULL(search_url);
    TEST_ASSERT_EQUAL_INT(strcmp(search_url, expected_url), 0);
    free(search_url);
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

int main(void) {
    UNITY_BEGIN();

    // Test
    RUN_TEST(test_detect_website_type);
    RUN_TEST(test_get_google_search_url);
    RUN_TEST(test_get_google_search_url_truncation);

    return UNITY_END();
}