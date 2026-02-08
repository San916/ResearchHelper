#include "unity.h"

#include "server.h"
#include "http_orchestrator.h"
#include "http_orchestrator_priv.h"
#include "router.h"
#include "test_server_utils.h"
#include "test_utils.h"
#include "handlers.h"
#include "json.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>
#include <windows.h>

// // Define all variables here
static int TEST_PORT_NUM = 6767;

static const char HOME_HTML[] = "..\\frontend\\index.html";
static const char HOME_CSS[] = "..\\frontend\\css\\main.css";
static const char HOME_JS[] = "..\\frontend\\js\\main.js";

static ServerContext* server_context = NULL;
static HANDLE server_thread = NULL;
static SOCKET test_client;

static Route routes[] = {
    {"/", handle_home_html},
    {"/css/main.css", handle_home_css},
    {"/js/main.js", handle_home_js},
    {"/submit", handle_submit},
    {"/content", handle_content_request},
};

void setUp(void) {
    // Setup server thread
    Server* server = create_server(TEST_PORT_NUM, 10, NULL, routes, NULL, 5);
    if (!server) {
        return;
    }
    Sleep(200);

    server_context = malloc(sizeof(ServerContext));
    if (!server_context) {
        return;
    }
    server_context->server = server;
    server_context->stop_flag = 0;

    server_thread = CreateThread(NULL, 0, run_server_for_testing, server_context, 0, NULL);

    // Create test client socket
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        return;
    }

    test_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (test_client == INVALID_SOCKET) {
        WSACleanup();
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(TEST_PORT_NUM);

    if (connect(test_client, (const struct sockaddr*)&server_addr, (int)sizeof(server_addr)) == SOCKET_ERROR) {
        WSACleanup();
        return;
    }
}

void tearDown(void) {
    if (server_context) {
        server_context->stop_flag = 1;
    }
    
    if (server_thread) {
        WaitForSingleObject(server_thread, 1000);
        CloseHandle(server_thread);
        server_thread = NULL;
    }

    if (server_context) {
        if (server_context->server) {
            destroy_server(server_context->server);
        }
        free(server_context);
        server_context = NULL;
    }

    closesocket(test_client);
    WSACleanup();
}

void get_frontend_files_over_one_connection(void) {
    const char get_html_request[] = 
        "GET / HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: keep-alive\r\n\r\n"
    ;

    TEST_ASSERT_NOT_EQUAL_INT(send(test_client, get_html_request, (int)strlen(get_html_request), 0), -1);

    char* expected_html = read_file(HOME_HTML);
    size_t html_len = strlen(expected_html) + HEADER_SIZE_ESTIMATE;
    
    char* buffer_html = calloc(1, html_len + 1);
    size_t bytes = (size_t)recv(test_client, buffer_html, html_len, 0);
    char* buffer_html_body = strstr(buffer_html, "\r\n\r\n");
    buffer_html_body = buffer_html_body + 4;

    TEST_ASSERT_EQUAL_INT(strcmp(buffer_html_body, expected_html), 0);
    free(buffer_html);
    free(expected_html);


    const char get_css_request[] = 
        "GET /css/main.css HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: keep-alive\r\n\r\n"
    ;

    TEST_ASSERT_NOT_EQUAL_INT(send(test_client, get_css_request, (int)strlen(get_css_request), 0), -1);

    char* expected_css = read_file(HOME_CSS);
    size_t css_len = strlen(expected_css) + HEADER_SIZE_ESTIMATE;
    
    char* buffer_css = calloc(1, css_len + 1);
    bytes = (size_t)recv(test_client, buffer_css, css_len, 0);
    char* buffer_css_body = strstr(buffer_css, "\r\n\r\n");
    buffer_css_body += 4;

    TEST_ASSERT_EQUAL_INT(strcmp(buffer_css_body, expected_css), 0);
    free(buffer_css);
    free(expected_css);


    const char get_js_request[] = 
        "GET /js/main.js HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n\r\n"
    ;

    TEST_ASSERT_NOT_EQUAL_INT(send(test_client, get_js_request, (int)strlen(get_js_request), 0), -1);

    char* expected_js = read_file(HOME_JS);
    size_t js_len = strlen(expected_js) + HEADER_SIZE_ESTIMATE;
    
    char* buffer_js = calloc(1, js_len + 1);
    bytes = (size_t)recv(test_client, buffer_js, js_len, 0);
    char* buffer_js_body = strstr(buffer_js, "\r\n\r\n");
    buffer_js_body += 4;

    TEST_ASSERT_EQUAL_INT(strcmp(buffer_js_body, expected_js), 0);
    free(buffer_js);
    free(expected_js);
}

void get_query_response(void) {
    const char get_query_request[] = 
        "POST /submit HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "Max-Num-Responses: 5\r\n\r\n"
        "user_input=How%20to%20use%20realloc()"
    ;

    TEST_ASSERT_NOT_EQUAL_INT(send(test_client, get_query_request, (int)strlen(get_query_request), 0), -1);
    
    char* buffer = calloc(1, MAX_RESPONSE_BODY_LEN);
    size_t bytes = (size_t)recv(test_client, buffer, MAX_RESPONSE_BODY_LEN - 1, 0);
    char* buffer_body = strstr(buffer, "\r\n\r\n");
    buffer_body += 4;

    size_t num_elements = 0;
    char** results_array = separate_array(buffer, &num_elements, 5);
    TEST_ASSERT_EQUAL_size_t(num_elements, 5);
    for (size_t i = 0; i < num_elements; i++) {
        free(results_array[i]);
    }
    free(results_array);

    free(buffer);
}

void send_chunked_request(void) {
    const char get_query_request[] = 
        "POST /submit HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "Max-Num-Responses: 5\r\n"
        "Transfer-Encoding: chunked\r\n\r\n"
        "b\r\n"
        "user_input=\r\n"
        "1a\r\n"
        "How%20to%20use%20realloc()\r\n"
        "0\r\n"
        "\r\n"
    ;

    TEST_ASSERT_NOT_EQUAL_INT(send(test_client, get_query_request, (int)strlen(get_query_request), 0), -1);
    
    char* buffer = calloc(1, MAX_RESPONSE_BODY_LEN);
    size_t bytes = (size_t)recv(test_client, buffer, MAX_RESPONSE_BODY_LEN - 1, 0);
    char* buffer_body = strstr(buffer, "\r\n\r\n");
    buffer_body += 4;

    size_t num_elements = 0;
    char** results_array = separate_array(buffer, &num_elements, 5);
    TEST_ASSERT_EQUAL_size_t(num_elements, 5);

    for (size_t i = 0; i < num_elements; i++) {
        free(results_array[i]);
    }
    free(results_array);
    free(buffer);
}

int main() {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(get_frontend_files_over_one_connection);
    RUN_TEST(get_query_response);
    RUN_TEST(send_chunked_request);

    return UNITY_END();
}
