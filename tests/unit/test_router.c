#include "unity.h"
#include "http.h"
#include "http_errors.h"
#include "handlers.h"
#include "router.h"
#include <string.h>
#include <stdlib.h>

// Define all variables here

// We're not testing our handler functions in router, simply use a stub to verify the routing works
static HttpResponse test_handle_home(HttpRequest* req) {
    HttpResponse resp = {0};
    resp.status_code = 200;
    resp.status_text = "OK";

    return resp;
}

static HttpResponse test_500_error_routing(HttpRequest* req) {
    HttpResponse resp = {0};
    resp.status_code = 200;
    resp.status_text = "OK";

    // SOMETHING BAD HAPPENED HERE!!! MUST RETURN 500 RESPONSE
    int something_bad_happened = 1;
    if (something_bad_happened) return handle_500();

    return resp;
}

static Route routes[2] = {
    {"/home", test_handle_home},
    {"/error", test_500_error_routing},
};

static HttpRequest* req = NULL;
static HttpResponse* resp = NULL;

void setUp(void) {
    req = calloc(1, sizeof(HttpRequest));
    if (!req) return;

    resp = calloc(1, sizeof(HttpResponse));
    if (!resp) return;
}

void tearDown(void) {
    if (req) free(req);
    req = NULL;

    if (resp) free(resp);
    resp = NULL;
}

void test_find_route_empty_req(void) {
    find_route(NULL, 0, NULL, resp);
    TEST_ASSERT_EQUAL_INT(resp->status_code, 404);
}

void test_find_route_not_found(void) {
    strcpy(req->path, "/HOME");
    find_route(routes, 2, req, resp);
    TEST_ASSERT_EQUAL_INT(resp->status_code, 404);
}

void test_find_route_correct_but_error(void) {
    strcpy(req->path, "/error");
    find_route(routes, 2, req, resp);
    TEST_ASSERT_EQUAL_INT(resp->status_code, 500);
}

void test_find_route_correct_no_error(void) {
    strcpy(req->path, "/home");
    find_route(routes, 2, req, resp);
    TEST_ASSERT_EQUAL_INT(resp->status_code, 200);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(test_find_route_empty_req);
    RUN_TEST(test_find_route_not_found);
    RUN_TEST(test_find_route_correct_but_error);
    RUN_TEST(test_find_route_correct_no_error);

    return UNITY_END();
}