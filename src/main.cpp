#include <iostream>
#include <string>
#include <thread>

#include <winsock2.h>

#include "database.h"
#include "user_manager.h"
#include "command_handler.h"
#include "session.h"
#include "server.h"
#include <command_parser.h>

int main()
{
    WSADATA wsaData;

    int result = WSAStartup(
        MAKEWORD(2, 2),
        &wsaData
    );

    if (result != 0)
    {
        std::cerr << "WSAStartup failed: "
                  << result << '\n';

        return 1;
    }

    Database database;
    UserManager userManager;

    userManager.createUser(
        "user1",
        "password"
    );

    CommandHandler commandHandler(
        database,
        userManager
    );

    Server server(
        6379,
        commandHandler
    );

    std::thread serverThread([&server]()
    {
        try
        {
            server.start();
        }
        catch (const std::exception& e)
        {
            std::cerr
                << "Server error: "
                << e.what()
                << '\n';
        }
    });

    serverThread.detach();

    Session session;

    std::cout << "Local console ready.\n";

    std::string command;

    while (true)
    {
        std::cout << "> ";

        if (!std::getline(std::cin, command))
            break;

        if (command == "QUIT")
            break;

        std::string response =
            commandHandler.handle(
                CommandParser().parse(command),
                session
            );

        std::cout << response << '\n';
    }

    WSACleanup();

    return 0;
}