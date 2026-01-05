#include "unity.h"
#include "web_crawler.h"
#include <string.h>
#include <stdlib.h>

// Define all variables here

void setUp(void) {
}

void tearDown(void) {
}

void test_web_crawler_basic(void) {
    int status_code = 0;
    char *input = "what is 9 + 10";
    input_query(input, &status_code);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_web_crawler_basic);

    return UNITY_END();
}