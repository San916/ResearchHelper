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

Route ROUTES[] = {
    {"/", handle_home_html},
    {"/css/main.css", handle_home_css},
    {"/js/main.js", handle_home_js},
    {"/submit", handle_submit},
    {"/content", handle_content_request},
};

void signal_handler(int signum) {
    keep_running = 0;
    printf("\nSignal received, shutting down...\n");
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    server = create_server(8080, NUM_INITIAL_CLIENTS, NULL, ROUTES, 5);
    if (!server) {
        fprintf(stderr, "Failed to create server!\n");
        return 1;
    }

    if (!start_server(server)) {
        fprintf(stderr, "Failed to start server!\n");
        destroy_server(server);
        return 1;
    }

    while (keep_running) {
        poll_server(server, TIMEOUT_SECS);
    }

    stop_server(server);
    destroy_server(server);

    return 0;
}