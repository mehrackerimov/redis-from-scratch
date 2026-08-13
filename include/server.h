#pragma once

#include <string>
#include "logger.h"
#include <winsock2.h>
#include "command_handler.h"

class Server
{
public:
    Server(int port, CommandHandler &commandHandler, Logger &logger);

    void start();

private:
    void setupSocket();
    void acceptClients();
    void handleClient(SOCKET clientSocket);

private:
    int port;
    SOCKET serverSocket = INVALID_SOCKET;
    Logger &logger;
    CommandHandler &commandHandler;
};