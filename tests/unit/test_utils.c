#include "unity.h"
#include "utils.h"
#include <stdlib.h>

// Define all variables here

void setUp(void) {
}

void tearDown(void) {
}

void test_url_decoded_str_len(void) {
    char input1[] = "hihihi";
    TEST_ASSERT_EQUAL_INT(url_decoded_str_len(input1), 6);

    char input2[] = "Hello+world%3F";
    TEST_ASSERT_EQUAL_INT(url_decoded_str_len(input2), 12);
}

void test_decode_url(void) {
    char input1[] = "hihihi";
    char* output1 = malloc(strlen(input1) + 1);
    if (!output1) return;
    decode_url(output1, input1, 6);
    TEST_ASSERT_EQUAL_INT(strcmp(output1, "hihihi"), 0);

    char input2[] = "Hello+world%3F";
    char* output2 = malloc(strlen(input2) + 1);
    if (!output2) return;
    decode_url(output2, input2, 13);
    TEST_ASSERT_EQUAL_INT(strcmp(output2, "Hello world?"), 0);

    free(output1);
    free(output2);
}


void test_str_equals(void) {
    TEST_ASSERT_EQUAL_INT(str_equals(NULL, "Hi", false), 0);
    TEST_ASSERT_EQUAL_INT(str_equals("Hi", NULL, false), 0);
    TEST_ASSERT_EQUAL_INT(str_equals("a", "b", false), 0);
    TEST_ASSERT_EQUAL_INT(str_equals("a", "b", true), 0);
    TEST_ASSERT_EQUAL_INT(str_equals("Hi", "Hi", false), 1);
    TEST_ASSERT_EQUAL_INT(str_equals("Hi", "Hi", true), 1);
    TEST_ASSERT_EQUAL_INT(str_equals("Hii", "Hi", false), 0);
    TEST_ASSERT_EQUAL_INT(str_equals("Hii", "Hi", true), 0);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_url_decoded_str_len);
    RUN_TEST(test_decode_url);

    RUN_TEST(test_str_equals);

    return UNITY_END();
}