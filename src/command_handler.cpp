#include <command_handler.h>

#include <iostream>
CommandHandler::CommandHandler(
    Database &database,
    UserManager &userManager, Logger &logger)
    : database(database),
      userManager(userManager),
      logger(logger)
{
}

bool CommandHandler::isValidCommand(const std::string &command) const
{
    return command == "GET" || command == "SET" || command == "MGET" || command == "MSET" || command == "DEL" || command == "PING" || command == "LOGIN" || command == "ACL" || command == "QUIT";
}

std::string CommandHandler::handle(
    const Command &command,
    Session &session)
{
    if (userManager.requiresPermission(command.name))
    {
        if (!session.authenticated)
        {
            return "-NOAUTH Authentication required\r\n";
        }

        if (!session.isSystem &&
            !userManager.hasPermission(
                session.username,
                command.name))
        {
            return "-NOPERM command not allowed\r\n";
        }
    }

    logger.debug("User '" + session.username + "' executed command '" + command.name + "'");

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

            logger.info("User '" + username + "' authenticated");

            return "OK";
        }

        return "ERR: Invalid username or password";
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

    else if (command.name == "LOGOUT")
    {
        if (!session.authenticated)
        {
            return "-NOAUTH Not authenticated\r\n";
        }

        session.authenticated = false;
        session.username.clear();

        logger.info("User logged out");

        return "OK";
    }

    else if (command.name == "ACL")
    {
        if (command.args.empty())
        {
            return "-ERR ACL requires a subcommand\r\n";
        }

        const std::string &subcommand = command.args[0];

        if (subcommand != "SETUSER")
        {
            return "-ERR unknown ACL subcommand\r\n";
        }

        if (command.args.size() < 2)
        {
            return "-ERR wrong number of arguments for 'acl|setuser' command\r\n";
        }

        const std::string &username = command.args[1];

        if (username.empty())
        {
            return "-ERR invalid username\r\n";
        }

        if (!userManager.userExists(username))
        {
            if (!userManager.createUser(username, "", {}))
            {
                return "-ERR failed to create user\r\n";
            }
        }

        for (size_t i = 2; i < command.args.size(); ++i)
        {
            const std::string &option = command.args[i];

            if (option == "on")
            {
                if (!userManager.setUserEnabled(username, true))
                {
                    return "-ERR failed to enable user\r\n";
                }
            }
            else if (option == "off")
            {
                if (!userManager.setUserEnabled(username, false))
                {
                    return "-ERR failed to disable user\r\n";
                }
            }
            else if (option.rfind(">", 0) == 0)
            {
                std::string password = option.substr(1);

                if (password.empty())
                {
                    return "-ERR password cannot be empty\r\n";
                }

                if (!userManager.setPassword(username, password))
                {
                    return "-ERR failed to set password\r\n";
                }
            }
            else if (option.rfind("+", 0) == 0)
            {
                std::string permission = option.substr(1);

                if (permission.empty())
                {
                    return "-ERR empty permission\r\n";
                }

                if (permission != "*" &&
                    !isValidCommand(permission))
                {
                    return "-ERR unknown command '" +
                           permission + "'\r\n";
                }

                if (!userManager.addPermission(
                        username,
                        permission))
                {
                    return "-ERR failed to add permission\r\n";
                }
            }
            else if (option.rfind("-", 0) == 0)
            {
                std::string permission = option.substr(1);

                if (permission.empty())
                {
                    return "-ERR empty permission\r\n";
                }

                if (permission != "*" &&
                    !isValidCommand(permission))
                {
                    return "-ERR unknown command '" +
                           permission + "'\r\n";
                }

                if (!userManager.removePermission(
                        username,
                        permission))
                {
                    return "-ERR failed to remove permission\r\n";
                }
            }
            else
            {
                return "-ERR unknown ACL option '" +
                       option +
                       "'\r\n";
            }
        }

        return "+OK\r\n";
    }

    return "-ERR unknown command\r\n";
}