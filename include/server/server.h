#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <winsock2.h>
#include "router.h"

typedef struct Server {
    bool running;
    int port;
    int num_clients;
    int capacity;
    SOCKET socket;
    SOCKET *clients;
    Route* routes;
    int num_routes;
} Server;

Server* createServer(int port, int initial_capacity, Route* routes, int num_routes);
bool startServer(Server *server);
void pollServer(Server *server, int timeout);
void stopServer(Server *server);
void destroyServer(Server *server);

#endif