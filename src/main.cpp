#include <iostream>
#include <sstream>

#include "database.h"

Database db;

std::string handleCommand(const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "SET")
    {
        std::string key, value;
        iss >> key >> value;

        if (key.empty() || value.empty())
        {
            return "Error: SET command requires a key and a value";
        }

        std::string option;
        iss >> option;

        std::optional<std::chrono::steady_clock::time_point> expiresAt = std::nullopt;

        if (option == "EX")
        {
            int ttl;

            if (!(iss >> ttl) || ttl <= 0)
            {
                return "Error: EX requires a positive number";
            }

            expiresAt = std::chrono::steady_clock::now() + std::chrono::seconds(ttl);
        }
        else if (!option.empty())
        {
            return "Error: unknown SET option";
        }

        db.set(key, value, expiresAt.has_value() ? std::optional<int>(std::chrono::duration_cast<std::chrono::seconds>(expiresAt.value() - std::chrono::steady_clock::now()).count()) : std::nullopt);

        return "OK";
    }
    else if (cmd == "GET")
    {
        std::string key;
        iss >> key;

        if (key.empty())
        {
            return "Error: GET command requires a key";
        }

        auto it = db.get(key);

        return it.value();
    }
    else if (cmd == "DEL")
    {
        std::string key;
        iss >> key;
        if (key.empty())
        {
            return "Error: DEL command requires a key";
        }
        db.del(key);
        return "OK";
    }
    else if (cmd == "EXPIRE")
    {
        std::string key;
        int ttl;
        iss >> key >> ttl;
        if (key.empty() || ttl <= 0)
        {
            return "Error: EXPIRE command requires a key and a positive TTL";
        }

        if (!db.exists(key))
        {
            return "Error: Key not found";
        }

        db.expire(key, ttl);
        return "OK";
    }
    else if (cmd == "TTL")
    {
        std::string key;
        iss >> key;
        if (key.empty())
        {
            return "Error: TTL command requires a key";
        }

        if (!db.exists(key))
        {
            return "-2"; // Key does not exist
        }

        int ttl = db.ttl(key);
        return std::to_string(ttl);
    }
    else if (cmd == "EXISTS")
    {
        std::string key;
        iss >> key;
        if (key.empty())
        {
            return "Error: EXISTS command requires a key";
        }

        if (db.exists(key))
        {
            return "1";
        }

        return "0";
    }
    else
    {
        return "Error: unknown command";
    }

    return "Command processed";
}

int main()
{
    while (1)
    {
        std::string command;
        std::cout << "> ";

        std::getline(std::cin, command);

        std::string result = handleCommand(command);
        std::cout << result << std::endl;
    }
    return 0;
}