#include <iostream>

#include "database.h"
#include "user_manager.h"
#include "command_handler.h"
#include "session.h"

int main()
{
    Database database;
    UserManager userManager;

    CommandHandler commandHandler(database, userManager);

    Session session;

    userManager.createUser("user1", "password"); //test user creation

    std::string command;

    while (1)
    {
        std::cout << "> ";
        std::getline(std::cin, command);
        std::string response =
            commandHandler.handle(command, session);

        std::cout << response << '\n';
    }

    return 0;
}