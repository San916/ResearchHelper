#include "unity.h"

#include "server.h"
#include "http_orchestrator.h"
#include "http_orchestrator_priv.h"
#include "router.h"
#include "test_server_utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>
#include <windows.h>

// Define all variables here
static int TEST_PORT_NUM = 6767;
static const char CLIENT_MESSAGE[] = "CLIENT MESSAGE";
static const char SERVER_MESSAGE[] = "Successfully received request!";

static char* mock_handle_request(HttpHandle* handle, const char* buffer, size_t bytes, bool* keep_alive) {
    if (strcmp(buffer, CLIENT_MESSAGE)) {
        char* response = calloc(1, strlen("Unsuccessfully received request!") + 1);
        strcpy(response, "Unsuccessfully received request!");
        return response; 
    } 
    char* response = calloc(1, strlen("Successfully received request!") + 1);
    strcpy(response, "Successfully received request!");
    return response; 
}

static ServerContext* server_context = NULL;
static HANDLE server_thread = NULL;
static SOCKET test_client;

void setUp(void) {
    // Setup server thread
    HttpHandle* handle = malloc(sizeof(HttpHandle));
    Route routes[] = {};

    Server* server = create_server(TEST_PORT_NUM, 10, handle, routes, mock_handle_request, 0);
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

void basic_server_polling_test(void) {
    TEST_ASSERT_NOT_EQUAL_INT(send(test_client, CLIENT_MESSAGE, (int)strlen(CLIENT_MESSAGE), 0), -1);

    char buffer[1024];
    int bytes = recv(test_client, buffer, sizeof(buffer) - 1, 0);
    TEST_ASSERT_GREATER_THAN_INT(0, bytes);
    buffer[bytes] = '\0';

    TEST_ASSERT_EQUAL_INT(strcmp(buffer, SERVER_MESSAGE), 0);
}

int main() {
    UNITY_BEGIN();

    // Tests
    RUN_TEST(basic_server_polling_test);

    return UNITY_END();
}
