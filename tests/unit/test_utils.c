#include "unity.h"
#include "utils.h"

// Define all variables here

void setUp(void) {
}

void tearDown(void) {
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
    RUN_TEST(test_str_equals);

    return UNITY_END();
}