#include "unity.h"
#include "http.h"
#include "http_errors.h"
#include "handlers.h"
#include "router.h"
#include <string.h>

// Define all variables here


void setUp(void) {
}

void tearDown(void) {
}

void test_find_route(void) {
    HttpResponse resp = {0};
    find_route(NULL, 1, NULL, &resp);
    TEST_ASSERT_EQUAL_INT(resp.status_code, 404);
}


int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_find_route);

    return UNITY_END();
}