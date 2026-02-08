#include "test_server_utils.h"
#include "server.h"

#include <winsock2.h>
#include <windows.h>

DWORD WINAPI run_server_for_testing(void* arg) {
    ServerContext* context = (ServerContext*)arg;
    Server* server = context->server;

    if (!start_server(server)) {
        destroy_server(server);
        return 1;
    }
    
    for (int i = 0; i < 10 && !context->stop_flag; i++) {
        poll_server(server, 100);
    }

    stop_server(server);

    return 0;
}
