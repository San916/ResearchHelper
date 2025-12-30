#include "server.h"
#include "http.h"
#include "http_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <winsock2.h>

Server* createServer(int port, int initial_capacity, Route* routes, int num_routes) {
    Server *server = malloc(sizeof(Server));
    if (!server) return NULL;
    server->running = false;
    server->port = port;
    server->num_clients = 0;
    server->capacity = initial_capacity;
    server->socket = INVALID_SOCKET;
    server->clients = malloc(sizeof(SOCKET) * initial_capacity);
    server->routes = routes;
    server->num_routes = num_routes;
    if (!server->clients) {
        free(server);
        return NULL;
    }
    return server;
}

bool startServer(Server *server) {
    // Validate everything
    if (!server) return false;
	WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0) {
        printf("WSAStartup failed!\n");
        return false;
    }

    server->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server->socket == INVALID_SOCKET) {
        printf("Socket creation failed!\n");
        return false;
    }

    // Allow port reuse
    const char opt = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("Setsockopt() failed!\n");
        return false;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(server->port);

    if (bind(server->socket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("Bind() failed!\n");
        return false;
    }

    if (listen(server->socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen() failed!\n");
        return false;
    }
    
    server->running = true;
    printf("Server started on port %d\n", server->port);
    return true;
}

void pollServer(Server *server, int timeout) {
    if (!server->running) return;

    // Add our socket and client sockets to the set
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server->socket, &readfds);

    for (int i = 0; i < server->num_clients && (i + 1) < FD_SETSIZE; i++) {
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
            printf("Client connected (now %d total clients)\n", server->num_clients);
        }
    }

    for (int i = 0; i < server->num_clients; i++) {
        SOCKET client_socket = server->clients[i];
        if (client_socket == 0) continue; // FD_ISSET can't be called with 0 so skip
        if (!FD_ISSET(client_socket, &readfds)) continue;

        char buffer[1024];
        int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);

        if (bytes <= 0) {
            closesocket(client_socket);
            server->clients[i] = server->clients[server->num_clients - 1];
            server->num_clients--;
            i--;
            printf("Client socket closed (now %d total clients)\n", server->num_clients);
        } else {
            bool build_http_request_failure = false;
            buffer[bytes] = '\0';

            HttpRequest* req = parse_http_request(buffer, bytes);
            HttpResponse resp = {0};

            if (!req) resp = handle_400(req);
            else find_route(server->routes, server->num_routes, req, &resp);

            char* response = build_http_response(&resp);
            if (!response) {
                build_http_request_failure = true;
                send(client_socket, MINIMAL_500_RESPONSE, (int)strlen(MINIMAL_500_RESPONSE), 0);
            } else {
                send(client_socket, response, (int)strlen(response), 0);
                free(response);
            }

            bool keep_alive = (req ? !req->keep_alive : false) || resp.close_connection || build_http_request_failure;
            if (req) free(req);
            free_http_response(&resp);
            
            if (keep_alive) {
                closesocket(client_socket);
                server->clients[i] = server->clients[server->num_clients - 1];
                server->num_clients--;
                i--;
                printf("Client socket closed (now %d total clients)\n", server->num_clients);
            } else {
                printf("Client socket stays open (now %d total clients)\n", server->num_clients);
            }
        }
    }
}

void stopServer(Server *server) {
    if (!server || !server->running) return;
    server->running = false;

    for (int i = 0; i < server->num_clients; i++) {
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

void destroyServer(Server *server) {
    if (!server) return;
    if (server->clients) free(server->clients);
    free(server);
    printf("Server destroyed\n");
}