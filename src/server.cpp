#include "server.h"

#include <iostream>
#include <stdexcept>
#include <thread>
#include <string>

#include "command_handler.h"
#include "command_parser.h"

Server::Server(int port, CommandHandler& commandHandler)
    : port(port),
      commandHandler(commandHandler)
{
}

void Server::start()
{
    setupSocket();
    acceptClients();
}

void Server::setupSocket()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET)
    {
        throw std::runtime_error(
            "Failed to create socket. Error: " +
            std::to_string(WSAGetLastError())
        );
    }

    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(static_cast<u_short>(port));

    if (bind(
        serverSocket,
        reinterpret_cast<sockaddr*>(&serverAddr),
        sizeof(serverAddr)
    ) == SOCKET_ERROR)
    {
        int error = WSAGetLastError();

        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;

        throw std::runtime_error(
            "Failed to bind socket. Error: " +
            std::to_string(error)
        );
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR)
    {
        int error = WSAGetLastError();

        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;

        throw std::runtime_error(
            "Failed to listen on socket. Error: " +
            std::to_string(error)
        );
    }

    std::cout << "Server listening on port "
              << port << '\n';
}

void Server::acceptClients()
{
    while (true)
    {
        sockaddr_in clientAddr{};

        SOCKET clientSocket = accept(
            serverSocket,
            reinterpret_cast<sockaddr*>(&clientAddr),
            nullptr
        );

        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr
                << "Failed to accept client. Error: "
                << WSAGetLastError()
                << '\n';

            continue;
        }

        std::thread(
            &Server::handleClient,
            this,
            clientSocket
        ).detach();
    }
}

void Server::handleClient(SOCKET clientSocket)
{
    Session session;

    char buffer[1024];

    while (true)
    {
        int bytesRead = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytesRead <= 0)
        {
            break;
        }

        buffer[bytesRead] = '\0';

        std::string command(buffer);

        std::string response =
            commandHandler.handle(CommandParser().parse(command), session);

        send(
            clientSocket,
            response.c_str(),
            static_cast<int>(response.size()),
            0
        );
    }

    closesocket(clientSocket);
}