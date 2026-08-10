#include <command_handler.h>

CommandHandler::CommandHandler(
    Database& database,
    UserManager& userManager
)
    : database(database),
      userManager(userManager)
{
}

std::string CommandHandler::handle(
    const std::string& command,
    Session& session)
{
    if (command == "PING")
    {
        return "PONG";
    }

    // LOGIN
    else if (command.rfind("LOGIN ", 0) == 0)
    {
        std::string username = command.substr(6);

        if (userManager.authenticate(username, "password"))
        {
            session.authenticated = true;
            session.username = username;

            return "OK";
        }

        return "ERR: Invalid username";
    }

    // Authentication gerektiren komutlar
    if (!session.authenticated)
    {
        return "ERR: Not authenticated";
    }

    // SET
    else if (command.rfind("SET ", 0) == 0)
    {
        std::istringstream iss(command);

        std::string cmd;
        std::string key;
        std::string value;
        std::string option;

        iss >> cmd >> key >> value;

        if (key.empty() || value.empty())
        {
            return "ERR: SET requires key and value";
        }

        iss >> option;

        std::optional<int> ttl = std::nullopt;

        if (option == "EX")
        {
            int seconds;

            if (!(iss >> seconds) || seconds <= 0)
            {
                return "ERR: EX requires a positive number";
            }

            ttl = seconds;
        }
        else if (!option.empty())
        {
            return "ERR: Unknown SET option";
        }

        if (database.set(key, value, ttl))
        {
            return "OK";
        }

        return "ERR: SET failed";
    }

    // GET
    else if (command.rfind("GET ", 0) == 0)
    {
        std::string key = command.substr(4);

        auto value = database.get(key);

        if (!value.has_value())
        {
            return "ERR: Key not found";
        }

        return value.value();
    }

    // DEL
    else if (command.rfind("DEL ", 0) == 0)
    {
        std::string key = command.substr(4);

        return database.del(key) ? "1" : "0";
    }

    // EXISTS
    else if (command.rfind("EXISTS ", 0) == 0)
    {
        std::string key = command.substr(7);

        return database.exists(key) ? "1" : "0";
    }

    // EXPIRE
    else if (command.rfind("EXPIRE ", 0) == 0)
    {
        std::istringstream iss(command);

        std::string cmd;
        std::string key;
        int seconds;

        iss >> cmd >> key >> seconds;

        if (key.empty() || iss.fail())
        {
            return "ERR: EXPIRE requires key and seconds";
        }

        return database.expire(key, seconds) ? "1" : "0";
    }

    // TTL
    else if (command.rfind("TTL ", 0) == 0)
    {
        std::string key = command.substr(4);

        return std::to_string(database.ttl(key));
    }

    return "ERR: Unknown command";
}