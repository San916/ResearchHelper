#include "unity.h"
#include "http.h"
#include "test_utils.h"
#include "handlers.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Define all variables here
static const char HOME_HTML[] = "..\\frontend\\index.html";
static const char HOME_CSS[] = "..\\frontend\\css\\main.css";
static const char HOME_JS[] = "..\\frontend\\js\\main.js";

static HttpResponse resp = {0};
static HttpRequest* req = NULL;

void setUp(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;
}

void tearDown(void) {
    if (req && req->body) {
        free(req->body);
        req->body = NULL;
    }
    if (req) {
        free(req);
        req = NULL;
    }
}


// ==================================
// handle_home_html
// ==================================
void test_handle_home_html(void) {
    strcpy(req->method, "GET");
    resp = handle_home_html(req);
    TEST_ASSERT_EQUAL_INT(resp.status_code, 200);

    char* expected = read_file(HOME_HTML);
    TEST_ASSERT_EQUAL_INT(strcmp(expected, resp.body), 0);
    free(expected);
}

// ==================================
// handle_home_css
// ==================================
void test_handle_home_css(void) {
    strcpy(req->method, "GET");
    resp = handle_home_css(req);
    TEST_ASSERT_EQUAL_INT(resp.status_code, 200);

    char* expected = read_file(HOME_CSS);
    TEST_ASSERT_EQUAL_INT(strcmp(expected, resp.body), 0);
    free(expected);
}

// ==================================
// handle_home_js
// ==================================
void test_handle_home_js(void) {
    strcpy(req->method, "GET");
    resp = handle_home_js(req);
    TEST_ASSERT_EQUAL_INT(resp.status_code, 200);

    char* expected = read_file(HOME_JS);
    TEST_ASSERT_EQUAL_INT(strcmp(expected, resp.body), 0);
    free(expected);
}

int main() {
    UNITY_BEGIN();

    // Tests
    // handle_home_html
    RUN_TEST(test_handle_home_html);

    // handle_home_css
    RUN_TEST(test_handle_home_css);

    // handle_home_js
    RUN_TEST(test_handle_home_js);
    
    return UNITY_END();
}
