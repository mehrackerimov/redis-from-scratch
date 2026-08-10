#include <iostream>
#include <string>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <optional>

struct Entry
{
    std::string value;
    std::optional<std::chrono::steady_clock::time_point> expiresAt;
};

std::unordered_map<std::string, Entry> db;

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

        db[key] = {value, expiresAt};

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

        auto it = db.find(key);
        if (it == db.end())
        {
            return "Error: Key not found";
        }

        if (it->second.expiresAt.has_value())
        {
            if (it->second.expiresAt < std::chrono::steady_clock::now())
            {
                db.erase(it);
                return "Error: Key has expired";
            }
        }

        return it->second.value;
    }
    else if (cmd == "DEL")
    {
        std::string key;
        iss >> key;
        if (key.empty())
        {
            return "Error: DEL command requires a key";
        }
        db.erase(key);
        return "OK";
    } else if(cmd == "EXPIRE") {
        std::string key;
        int ttl;
        iss >> key >> ttl;
        if (key.empty() || ttl <= 0)
        {
            return "Error: EXPIRE command requires a key and a positive TTL";
        }

        auto it = db.find(key);
        if (it == db.end())
        {
            return "Error: Key not found";
        }

        it->second.expiresAt = std::chrono::steady_clock::now() + std::chrono::seconds(ttl);
        return "OK";
    } else if(cmd == "TTL") {
        std::string key;
        iss >> key;
        if (key.empty())
        {
            return "Error: TTL command requires a key";
        }

        auto it = db.find(key);
        if (it == db.end())
        {
            return "Error: Key not found";
        }

        if (!it->second.expiresAt.has_value())
        {
            return "-1"; 
        }

        auto now = std::chrono::steady_clock::now();
        if (it->second.expiresAt < now)
        {
            db.erase(it);
            return "Error: Key has expired";
        }

        auto ttl = std::chrono::duration_cast<std::chrono::seconds>(it->second.expiresAt.value() - now).count();
        return std::to_string(ttl);
    } else if(cmd == "EXISTS") {
        std::string key;
        iss >> key;
        if (key.empty())
        {
            return "Error: EXISTS command requires a key";
        }

        auto it = db.find(key);
        if (it == db.end())
        {
            return "0";
        }

        if (it->second.expiresAt.has_value())
        {
            if (it->second.expiresAt < std::chrono::steady_clock::now())
            {
                db.erase(it);
                return "0";
            }
        }

        return "1";
    } else {
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