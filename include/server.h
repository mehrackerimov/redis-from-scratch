#pragma once

#include <string>
#include <winsock2.h>
#include "command_handler.h"

class Server
{
public:
    Server(int port, CommandHandler& commandHandler);

    void start();

private:
    void setupSocket();
    void acceptClients();
    void handleClient(SOCKET clientSocket);

private:
    int port;
    SOCKET serverSocket = INVALID_SOCKET;

    CommandHandler& commandHandler;
};