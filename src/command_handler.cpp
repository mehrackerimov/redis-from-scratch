#include <command_handler.h>

CommandHandler::CommandHandler(
    Database &database,
    UserManager &userManager)
    : database(database),
      userManager(userManager)
{
}

std::string CommandHandler::handle(
    const Command &command,
    Session &session)
{
    if (command.name == "PING")
    {
        return "PONG";
    }

    // LOGIN
    else if (command.name == "LOGIN")
    {
        if (command.args.size() < 2)
        {
            return "ERR: LOGIN requires username and password";
        }

        const std::string &username = command.args[0];
        const std::string &password = command.args[1];

        if (username.empty() || password.empty())
        {
            return "ERR: LOGIN requires username and password";
        }

        if (userManager.authenticate(username, password))
        {
            session.authenticated = true;
            session.username = username;

            return "OK";
        }

        return "ERR: Invalid username or password";
    }

    // Authentication gerektiren komutlar
    if (!session.authenticated)
    {
        return "ERR: Not authenticated";
    }

    // SET
    else if (command.name == "SET")
    {
        if (command.args.size() < 2)
        {
            return "ERR: SET requires key and value";
        }

        const std::string &key = command.args[0];
        const std::string &value = command.args[1];
        std::string option;

        if (command.args.size() > 2)
        {
            option = command.args[2];
        }

        std::optional<int> ttl = std::nullopt;

        if (option == "EX")
        {
            int seconds;

            if (!(std::istringstream(command.args[3]) >> seconds) || seconds <= 0)
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
    else if (command.name == "GET")
    {
        if (command.args.size() < 1)
        {
            return "ERR: GET requires a key";
        }

        const std::string &key = command.args[0];

        auto value = database.get(key);

        if (!value.has_value())
        {
            return "ERR: Key not found";
        }

        return value.value();
    }

    // DEL
    else if (command.name == "DEL")
    {
        if (command.args.size() < 1)
        {
            return "ERR: DEL requires a key";
        }

        const std::string &key = command.args[0];

        return database.del(key) ? "1" : "0";
    }

    // EXISTS
    else if (command.name == "EXISTS")
    {
        if (command.args.size() < 1)
        {
            return "ERR: EXISTS requires a key";
        }

        const std::string &key = command.args[0];

        return database.exists(key) ? "1" : "0";
    }

    // EXPIRE
    else if (command.name == "EXPIRE")
    {
        if (command.args.size() < 2)
        {
            return "ERR: EXPIRE requires key and seconds";
        }

        const std::string &key = command.args[0];
        int seconds;

        try
        {
            seconds = std::stoi(command.args[1]);
        }
        catch (const std::exception &)
        {
            return "ERR: EXPIRE requires a valid number of seconds";
        }

        if (key.empty() || seconds <= 0)
        {
            return "ERR: EXPIRE requires key and seconds";
        }

        return database.expire(key, seconds) ? "1" : "0";
    }

    // TTL
    else if (command.name == "TTL")
    {
        if (command.args.size() < 1)
        {
            return "ERR: TTL requires a key";
        }

        const std::string &key = command.args[0];

        return std::to_string(database.ttl(key));
    }

    // MSET

    else if (command.name == "MSET")
    {
        if (command.args.empty() || command.args.size() % 2 != 0)
        {
            return "ERR wrong number of arguments for 'MSET'";
        }

        std::vector<std::pair<std::string, std::string>> pairs;

        for (size_t i = 0; i < command.args.size(); i += 2)
        {
            pairs.emplace_back(
                command.args[i],
                command.args[i + 1]);
        }

        if (database.mset(pairs))
        {
            return "OK";
        }

        return "ERR MSET failed";
    }

    // MGET
    else if (command.name == "MGET")
    {
        if (command.args.empty())
        {
            return "ERR wrong number of arguments for 'MGET'";
        }

        auto results = database.mget(command.args);

        std::string response;

        for (size_t i = 0; i < results.size(); ++i)
        {
            if (results[i].has_value())
            {
                response += results[i].value();
            }
            else
            {
                response += "(nil)";
            }

            if (i + 1 < results.size())
            {
                response += "\n";
            }
        }

        return response;
    }

    return "ERR: Unknown command";
}