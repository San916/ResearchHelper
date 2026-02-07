#include "server.h"
#include "http_orchestrator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <winsock2.h>

Server* create_server(int port, size_t initial_capacity, HttpHandle* http_handle, Route* routes, HandleRequest handle_request, size_t num_routes) {
    Server *server = malloc(sizeof(Server));
    if (!server) return NULL;
    server->running = false;
    server->port = port;
    server->num_clients = 0;
    server->capacity = initial_capacity;
    server->socket = INVALID_SOCKET;
    server->clients = malloc(sizeof(SOCKET) * initial_capacity);
    if (!server->clients) {
        free(server);
        return NULL;
    }
    if (http_handle) {
        server->http_handle = http_handle;
    } else {
        server->http_handle = get_http_handle();
    }
    if (routes) {
        set_http_handle_routes(server->http_handle, routes, num_routes);
    }
    if (handle_request) {
        server->handle_request = handle_request;
    }

    return server;
}

bool start_server(Server *server) {
    // Validate everything
    if (!server) return false;
	WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0) {
        WSACleanup();
        printf("WSAStartup failed!\n");
        return false;
    }

    server->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server->socket == INVALID_SOCKET) {
        WSACleanup();
        printf("Socket creation failed!\n");
        return false;
    }

    // Allow port reuse
    const char opt = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == SOCKET_ERROR) {
        WSACleanup();
        printf("Setsockopt() failed!\n");
        return false;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(server->port);

    if (bind(server->socket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        WSACleanup();
        printf("Bind() failed!\n");
        return false;
    }

    if (listen(server->socket, SOMAXCONN) == SOCKET_ERROR) {
        WSACleanup();
        printf("Listen() failed!\n");
        return false;
    }
    
    server->running = true;
    printf("Server started on port %d\n", server->port);
    return true;
}

void poll_server(Server *server, int timeout) {
    if (!server->running) return;

    // Add our socket and client sockets to the set
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server->socket, &readfds);

    for (size_t i = 0; i < server->num_clients && (i + 1) < FD_SETSIZE; i++) {
        FD_SET(server->clients[i], &readfds);
    }

    // Wait for activity on any socket with timeout
    struct timeval select_timeout = {timeout, 0};
    int ready = select(0, &readfds, NULL, NULL, &select_timeout);
    if (ready <= 0) return;

    if (FD_ISSET(server->socket, &readfds)) {
        SOCKET client_socket = accept(server->socket, NULL, NULL);
        printf("Accepted connection\n");
        if (client_socket != INVALID_SOCKET) {
            if (server->num_clients == server->capacity) {
                server->capacity *= 2;
                SOCKET *temp_clients = realloc(server->clients, sizeof(SOCKET) * server->capacity);
                if (!temp_clients) {
                    closesocket(client_socket);
                    return;
                }
                server->clients = temp_clients;
            }
            server->clients[server->num_clients++] = client_socket;
            printf("Client connected (now %zu total clients)\n", server->num_clients);
        }
    }

    for (size_t i = 0; i < server->num_clients; i++) {
        SOCKET client_socket = server->clients[i];
        if (client_socket == 0) continue; // FD_ISSET can't be called with 0 so skip
        if (!FD_ISSET(client_socket, &readfds)) continue;

        char buffer[MAX_REQUEST_SIZE];
        int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);

        if (bytes <= 0) {
            closesocket(client_socket);
            server->clients[i] = server->clients[server->num_clients - 1];
            server->num_clients--;
            i--;
            printf("Client socket closed (now %zu total clients)\n", server->num_clients);
        } else {
            buffer[bytes] = '\0';
            bool keep_alive = false;
            char* response = server->handle_request(server->http_handle, buffer, bytes, &keep_alive);
            
            send(client_socket, response, (int)strlen(response), 0);
            free(response);

            if (keep_alive) {
                closesocket(client_socket);
                server->clients[i] = server->clients[server->num_clients - 1];
                server->num_clients--;
                i--;
                printf("Client socket closed (now %zu total clients)\n", server->num_clients);
            } else {
                printf("Client socket stays open (now %zu total clients)\n", server->num_clients);
            }
        }
    }
}

void stop_server(Server *server) {
    if (!server || !server->running) return;
    server->running = false;

    for (size_t i = 0; i < server->num_clients; i++) {
        int fd = server->clients[i];
        if (fd <= 0) continue;
        closesocket(fd);
    }

    if (server->socket != INVALID_SOCKET) {
        closesocket(server->socket);
        server->socket = INVALID_SOCKET;
    }
    WSACleanup();
    printf("Server stopping...\n");
}

void destroy_server(Server *server) {
    if (!server) {
        return;
    }
    if (server->clients) {
        free(server->clients);
    }
    if (server->http_handle) {
        destroy_http_handle(server->http_handle);
    }
    free(server);
    printf("Server destroyed\n");
}