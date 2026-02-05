#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <stdbool.h>
#include <winsock2.h>

typedef struct HttpHandle HttpHandle;
typedef struct Route Route;

typedef struct Server {
    bool running;
    int port;
    size_t num_clients;
    size_t capacity;
    SOCKET socket;
    SOCKET *clients;
    HttpHandle* http_handle;
} Server;

Server* create_server(int port, size_t initial_capacity, HttpHandle* http_handle, Route* routes, size_t num_routes);
bool start_server(Server *server);
void poll_server(Server *server, int timeout);
void stop_server(Server *server);
void destroy_server(Server *server);

#endif