#include <iostream>
#include <string>
#include <thread>

#include <winsock2.h>
#include <sodium.h>
#include "database.h"
#include "user_manager.h"
#include "command_handler.h"
#include "session.h"
#include "server.h"
#include "logger.h"
#include "config.h"
#include <command_parser.h>

int main()
{
    WSADATA wsaData;

    int result = WSAStartup(
        MAKEWORD(2, 2),
        &wsaData);

    if (result != 0)
    {
        std::cerr << "WSAStartup failed: "
                  << result << '\n';

        return 1;
    }

    Config config("config/server.conf");

    if (!config.load())
    {
        std::cerr << "Failed to load config\n";
        return 1;
    }

    Logger logger(config.logLevel());
    Database database;

    if (sodium_init() < 0)
    {
        return 1;
    }

    UserManager userManager;

    userManager.createUser("admin", "admin123");

    CommandHandler commandHandler(
        database,
        userManager, logger);

    Server server(
        config.port(),
        commandHandler,
        logger);

    logger.info("Server is starting on port " + std::to_string(config.port()) + "...");

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
        } });

    serverThread.detach();

    Session session;

    session.username = "system";
    session.authenticated = true;

    std::cout << "Local console ready.\n";

    std::string command;

    while (true)
    {
        std::cout << "> ";

        if (!std::getline(std::cin, command))
            break;

        if (command == "QUIT")
            break;

        CommandParser parser;

        Command parsedCommand = parser.parse(command);

        std::string response =
            commandHandler.handle(
                parsedCommand,
                session);

        std::cout << response << '\n';
    }

    WSACleanup();

    return 0;
}