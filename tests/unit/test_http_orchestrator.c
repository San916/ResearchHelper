#include "unity.h"
#include "http.h"
#include "http_errors.h"
#include "http_orchestrator.h"
#include "http_orchestrator_priv.h"
#include "router.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Define all variables here
static HttpRequest* mock_parse_http_request_works(const char* buffer, size_t bytes, int* status_code) {
    HttpRequest* req = calloc(1, sizeof(HttpRequest));

    strcpy(req->path, "/mock");

    req->keep_alive = false;

    *status_code = REQUEST_OK;
    return req;
}

static HttpRequest* mock_parse_http_request_doesnt_work(const char* buffer, size_t bytes, int* status_code) {
    *status_code = -1;
    return NULL;
}

static HttpResponse mock_handle_parse_http_request_error(HttpRequest* req, int status_code) {
    HttpResponse resp = {0};
    resp.status_text = "Bad parse http request";
    return resp;
}

static void mock_find_route(Route* routes, size_t num_routes, HttpRequest* req, HttpResponse* resp) {
    HttpResponse stub_response = routes[0].handler(req);
    *resp = stub_response;
    return;
}

static char* mock_build_http_response_works(HttpResponse* resp) {
    char* response = calloc(1, strlen(resp->status_text) + 1);
    strcpy(response, resp->status_text);
    return response;
}

static char* mock_build_http_response_doesnt_work(HttpResponse* resp) {
    return NULL;
}

static char* mock_handle_build_http_response_error(HttpResponse* resp) {
    char* response = calloc(1, 1 + strlen("Bad build http response"));
    strcpy(response, "Bad build http response");
    return response;
}

static HttpResponse mock_handler(HttpRequest* req) {
    HttpResponse resp = {0};
    resp.status_code = 200;
    resp.status_text = "Good Request";
    return resp;
}

static Route routes[1] = {
    {"/mock", mock_handler},
};

static const char BUFFER[] = "Stub!";

static HttpHandle* handle = NULL;

void setUp(void) {
    handle = get_http_handle();

    set_http_handle_routes(handle, routes, 1);
        
    handle->parse_http_request = mock_parse_http_request_works;
    handle->handle_parse_http_request_error = mock_handle_parse_http_request_error;
    handle->find_route = mock_find_route;
    handle->build_http_response = mock_build_http_response_works;
    handle->handle_build_http_response_error = mock_handle_build_http_response_error;
}

void tearDown(void) {
    destroy_http_handle(handle);
    handle = NULL;
}

void test_handle_request(void) {
    bool keep_alive = true;
    char* response = handle_request(handle, BUFFER, strlen(BUFFER), &keep_alive);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_FALSE(keep_alive);
    TEST_ASSERT_EQUAL_INT(strcmp(response, "Good Request"), 0);
    free(response);
}

void test_handle_request_parse_http_request_doesnt_work(void) {
    handle->parse_http_request = mock_parse_http_request_doesnt_work;

    bool keep_alive = true;
    char* response = handle_request(handle, BUFFER, strlen(BUFFER), &keep_alive);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_FALSE(keep_alive);
    TEST_ASSERT_EQUAL_INT(strcmp(response, "Bad parse http request"), 0);
    free(response);
}

void test_handle_request_build_http_response_doesnt_work(void) {
    handle->build_http_response = mock_build_http_response_doesnt_work;

    bool keep_alive = true;
    char* response = handle_request(handle, BUFFER, strlen(BUFFER), &keep_alive);
    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_FALSE(keep_alive);
    TEST_ASSERT_EQUAL_INT(strcmp(response, "Bad build http response"), 0);
    free(response);
}

int main(void) {
    UNITY_BEGIN();

    // Tests
    // handle_request
    RUN_TEST(test_handle_request);
    RUN_TEST(test_handle_request_parse_http_request_doesnt_work);
    RUN_TEST(test_handle_request_build_http_response_doesnt_work);

    return UNITY_END();
}