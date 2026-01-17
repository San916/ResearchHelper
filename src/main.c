#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include "server.h"
#include "router.h"
#include "handlers.h"

#define NUM_INITIAL_CLIENTS 10
#define TIMEOUT_SECS 2

volatile sig_atomic_t keep_running = 1;
Server *server = NULL;

void signalHandler(int signum) {
    keep_running = 0;
    printf("\nSignal received, shutting down...\n");
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    Route routes[] = {
        {"/", handle_home_html},
        {"/css/main.css", handle_home_css},
        {"/js/main.js", handle_home_js},
        {"/submit", handle_submit},
        {"/content", handle_content_request},
        {"/about", handle_about},
    };
    int num_routes = sizeof(routes) / sizeof(Route);

    server = createServer(8080, NUM_INITIAL_CLIENTS, routes, num_routes);
    if (!server) {
        fprintf(stderr, "Failed to create server!\n");
        return 1;
    }

    if (!startServer(server)) {
        fprintf(stderr, "Failed to start server!\n");
        destroyServer(server);
        return 1;
    }

    while (keep_running) {
        pollServer(server, TIMEOUT_SECS);
    }

    stopServer(server);
    destroyServer(server);

    return 0;
}