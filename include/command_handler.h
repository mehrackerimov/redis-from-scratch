#pragma once

#include <database.h>
#include <session.h>
#include <user_manager.h>
#include <command.h>

class CommandHandler
{
public:
    CommandHandler(Database &database, UserManager &userManager);
    
    std::string handle(const Command &command, Session &session);

private:
    Database &database;
    UserManager& userManager;
};