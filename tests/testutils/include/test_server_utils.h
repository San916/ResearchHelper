#ifndef TEST_SERVER_UTILS_H
#define TEST_SERVER_UTILS_H

#include <winsock2.h>
#include <windows.h>

typedef struct Server Server;

typedef struct ServerContext {
    Server* server;
    int stop_flag;
} ServerContext;

DWORD WINAPI run_server_for_testing(void* arg);

#endif