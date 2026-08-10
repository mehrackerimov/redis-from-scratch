#include <database.h>
#include <session.h>
#include <user_manager.h>

class CommandHandler
{
public:
    CommandHandler(Database &database, UserManager &userManager);
    
    std::string handle(const std::string &command, Session &session);

private:
    Database &database;
    UserManager& userManager;
};