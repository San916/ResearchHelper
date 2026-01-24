#include "unity.h"
#include "web_utils.h"

#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

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

void test_create_curl_headers(void) {
    struct curl_slist* headers = create_curl_headers();
    TEST_ASSERT_NULL(headers);
}

void test_destroy_curl_handle(void) {
    destroy_curl_handle(NULL);

    CURL* curl_handle = create_curl_handle();
    TEST_ASSERT_NOT_NULL(curl_handle);

    destroy_curl_handle(curl_handle);
}

void test_destroy_curl_headers(void) {
    struct curl_slist* headers = create_curl_headers();
    TEST_ASSERT_NULL(headers);

    headers = curl_slist_append(headers, "Accept: application/json");
    TEST_ASSERT_NOT_NULL(headers);

    destroy_curl_headers(headers);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_load_env);
    RUN_TEST(test_create_curl_handle);
    RUN_TEST(test_create_curl_headers);
    RUN_TEST(test_destroy_curl_handle);
    RUN_TEST(test_destroy_curl_headers);
    RUN_TEST(test_write_memory_callback);

    return UNITY_END();
}