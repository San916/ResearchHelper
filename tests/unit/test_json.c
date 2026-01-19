#include "unity.h"
#include "json.h"
#include <stdlib.h>
#include <stdio.h>

// Define all variables here

void setUp(void) {
}

void tearDown(void) {
}

void test_get_json_value_number(void) {
    const char* json_number = "\"key\": 123123";
    char* result = get_json_value(json_number, "key");
    TEST_ASSERT_EQUAL_INT(strcmp(result, "123123"), 0);
    free(result);
}

void test_get_json_value_null(void) {
    const char* json_null = "\"key\": null";
    char* result = get_json_value(json_null, "key");
    TEST_ASSERT_EQUAL_INT(strcmp(result, "null"), 0);
    free(result);
}

void test_get_json_value_bool(void) {
    const char* json_bool = "\"key\": true";
    const char* json_bool_false = "\"key\": false";

    char* result = get_json_value(json_bool, "key");
    TEST_ASSERT_EQUAL_INT(strcmp(result, "true"), 0);
    free(result);

    result = get_json_value(json_bool, "key1");
    TEST_ASSERT_NULL(result);

    result = get_json_value(json_bool_false, "key");
    TEST_ASSERT_EQUAL_INT(strcmp(result, "false"), 0);
    free(result);
}

void test_get_json_value_string(void) {
    const char* json_string = "\"key\": \"This is a string\"";
    const char* json_string_2 = "\"key\": \"This \\\"is\\\" a string\"";
    const char* json_string_bad = "\"key\": \"This is a string";

    char* result = get_json_value(json_string, "key");
    TEST_ASSERT_EQUAL_INT(strcmp(result, "\"This is a string\""), 0);
    free(result);

    result = get_json_value(json_string_2, "key");
    TEST_ASSERT_EQUAL_INT(strcmp(result, "\"This \\\"is\\\" a string\""), 0);
    free(result);

    result = get_json_value(json_string_bad, "key");
    TEST_ASSERT_NULL(result);
}

void test_get_json_value_string_weird_whitespace(void) {
    const char* json_string = "\n\t\"key\"\n\t: \"This is a string\"";
    const char* json_string_2 = "\"key\": \n\t\"This is a string\"\n\t";
    const char* json_string_3 = "\n\t\"key\"\n\t: \n\t\"This is a string\"\n\t";

    char* result = get_json_value(json_string, "key");
    TEST_ASSERT_EQUAL_INT(strcmp(result, "\"This is a string\""), 0);
    free(result);

    result = get_json_value(json_string_2, "key");
    TEST_ASSERT_EQUAL_INT(strcmp(result, "\"This is a string\""), 0);
    free(result);

    result = get_json_value(json_string_3, "key");
    TEST_ASSERT_EQUAL_INT(strcmp(result, "\"This is a string\""), 0);
    free(result);
}


void test_get_json_value_array(void) {
    const char* json_array = 
        "{"
        "   \"array\": ["
        "       {\"key\": 123123},"
        "       {\"key\": \"qweqwe\"}"
        "   ]"
        "}";
    const char* json_array_contents = 
        "["
        "       {\"key\": 123123},"
        "       {\"key\": \"qweqwe\"}"
        "   ]";
    char* result = get_json_value(json_array, "array");
    TEST_ASSERT_EQUAL_INT(strcmp(result, json_array_contents), 0);
    free(result);
}

void test_get_json_value_array_bad(void) {
    const char* json_array = 
        "{"
        "\"array\":"
        "["
        "   ["
        "       {\"key\": 123123},"
        "       {\"key\": \"qweqwe\"}"
        "   ],"
        "   ["
        "       {\"key\": 123123},"
        "       {\"key\": \"qweqwe\"}"
        "   "
        "]"
        "}";
    char* result = get_json_value(json_array, "array");
    TEST_ASSERT_NULL(result);
}

void test_get_json_value_object(void) {
    const char* json_object = 
        "\"object\":"
        "{"
        "   \"array\": ["
        "       {\"key\": 123123},"
        "       {\"key\": \"qweqwe\"}"
        "   ]"
        "}";
    const char* json_object_contents = 
        "{"
        "   \"array\": ["
        "       {\"key\": 123123},"
        "       {\"key\": \"qweqwe\"}"
        "   ]"
        "}";
    char* result = get_json_value(json_object, "object");
    TEST_ASSERT_EQUAL_INT(strcmp(result, json_object_contents), 0);
    free(result);
}

void test_get_json_value_object_bad(void) {
    const char* json_object = 
        "\"object\":"
        "{"
        "   \"array\": ["
        "       {\"key\": 123123},"
        "       {\"key\": \"qweqwe\"}"
        "   ]"
        "";
    char* result = get_json_value(json_object, "object");
    TEST_ASSERT_NULL(result);
}

void test_separate_array(void) {
    char* json_array =
        "["
        "["
        "    {\"ab\": 12},"
        "    {\"cd\": 34}"
        "],"
        "["
        "    {\"ef\": 56},"
        "    {\"gh\": 78}"
        "]"
        "]";
    const char* json_array_separated[2] = {
        "["
        "    {\"ab\": 12},"
        "    {\"cd\": 34}"
        "]",
        "["
        "    {\"ef\": 56},"
        "    {\"gh\": 78}"
        "]"
    };
    int num_elems = 0;
    char** result = separate_array(json_array, &num_elems, 2);
    TEST_ASSERT_EQUAL_INT(strcmp(result[0], json_array_separated[0]), 0);
    TEST_ASSERT_EQUAL_INT(strcmp(result[1], json_array_separated[1]), 0);
    free(result[0]);
    free(result[1]);
    free(result);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_get_json_value_bool);
    RUN_TEST(test_get_json_value_null);
    RUN_TEST(test_get_json_value_number);
    RUN_TEST(test_get_json_value_string);
    RUN_TEST(test_get_json_value_string_weird_whitespace);

    RUN_TEST(test_get_json_value_array);
    RUN_TEST(test_get_json_value_array_bad);
    RUN_TEST(test_get_json_value_object);
    RUN_TEST(test_get_json_value_object_bad);

    RUN_TEST(test_separate_array);

    return UNITY_END();
}