#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <winsock2.h>
#include "router.h"

typedef struct Server {
    bool running;
    int port;
    size_t num_clients;
    size_t capacity;
    SOCKET socket;
    SOCKET *clients;
    Route* routes;
    size_t num_routes;
} Server;

Server* createServer(int port, size_t initial_capacity, Route* routes, size_t num_routes);
bool startServer(Server *server);
void pollServer(Server *server, int timeout);
void stopServer(Server *server);
void destroyServer(Server *server);

#endif