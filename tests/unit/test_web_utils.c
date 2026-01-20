#include "unity.h"
#include "web_utils.h"
#include <string.h>
#include <stdlib.h>

// Define all variables here
static const char TEST_ENV_FILENAME[] = "..\\tests\\data\\test_env.env";

void setUp(void) {
}

void tearDown(void) {
}

void test_load_env(void) {
    load_env(TEST_ENV_FILENAME);
    TEST_ASSERT_EQUAL_INT(strcmp(getenv("TEST_API_KEY"), "TEST_API_VALUE"), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(getenv("TEST_API_KEY_2"), "TEST_API_VALUE_2"), 0);
}

void test_write_memory_callback(void) {
    char* null_terminated_string = "Content string!";
    size_t string_length = strlen(null_terminated_string);
    char* non_null_terminated_string = malloc(string_length);
    strncpy(non_null_terminated_string, null_terminated_string, string_length);

    WritebackData writeback = {0};
    writeback.data = malloc(1);
    writeback.size = 1;
    writeback.data[0] = '\0';

    TEST_ASSERT_EQUAL_INT(write_memory_callback(non_null_terminated_string, 1, string_length, &writeback), string_length);
    TEST_ASSERT_EQUAL_INT(strcmp(writeback.data, null_terminated_string), 0);
    TEST_ASSERT_EQUAL_INT(writeback.size, string_length + 1);

    free(writeback.data);
    free(non_null_terminated_string);
}

void test_create_curl_handle(void) {
    CURL* curl_handle = create_curl_handle();
    TEST_ASSERT_NOT_NULL(curl_handle);

    destroy_curl_handle(curl_handle);
}

void test_destroy_curl_handle(void) {
    destroy_curl_handle(NULL);

    CURL* curl_handle = create_curl_handle();
    TEST_ASSERT_NOT_NULL(curl_handle);

    destroy_curl_handle(curl_handle);
    destroy_curl_handle(curl_handle);
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

void test_detect_website_type(void) {
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.stackoverflow.com/"), WEBSITE_STACKOVERFLOW);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.reddit.com/"), WEBSITE_REDDIT);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.stackexchange.com/"), WEBSITE_STACKOVERFLOW);
    TEST_ASSERT_EQUAL_INT(detect_website_type("https://www.github.com/"), WEBSITE_GITHUB);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_load_env);
    RUN_TEST(test_create_curl_handle);
    RUN_TEST(test_destroy_curl_handle);
    RUN_TEST(test_write_memory_callback);
    RUN_TEST(test_get_google_search_url);
    RUN_TEST(test_get_google_search_url_truncation);
    RUN_TEST(test_detect_website_type);

    return UNITY_END();
}