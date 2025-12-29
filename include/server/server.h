#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <winsock2.h>

typedef struct Server {
    bool running;
    int port;
    int num_clients;
    int capacity;
    SOCKET socket;
    SOCKET *clients;
} Server;

Server* createServer(int port, int initial_capacity);
bool startServer(Server *server);
void pollServer(Server *server, int timeout);
void stopServer(Server *server);
void destroyServer(Server *server);

#endif