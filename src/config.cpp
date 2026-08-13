#include "config.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

Config::Config(const std::string &path)
    : path(path)
{
}

bool Config::load()
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::filesystem::create_directories("config");

        std::ofstream configFile(path);

        if (!configFile.is_open())
        {
            return false;
        }

        configFile << "port=6379\n";
        configFile << "max_clients=100 #not implemented\n";
        configFile << "log_level=INFO\n";
        configFile << "persistence=true #not implemented\n";

        configFile.close();

        return true;
    }
    
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::size_t separator = line.find('=');

        if (separator == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0, separator);
        std::string value = line.substr(separator + 1);

        key.erase(
            0,
            key.find_first_not_of(" \t"));

        key.erase(
            key.find_last_not_of(" \t") + 1);

        value.erase(
            0,
            value.find_first_not_of(" \t"));

        value.erase(
            value.find_last_not_of(" \t") + 1);

        if (key == "port")
        {
            serverPort = std::stoi(value);
        }
        else if (key == "max_clients")
        {
            maximumClients = std::stoi(value);
        }
        else if (key == "log_level")
        {
            if (value == "DEBUG")
            {
                loggingLevel = LogLevel::DEBUG;
            }
            else if (value == "INFO")
            {
                loggingLevel = LogLevel::INFO;
            }
            else if (value == "WARN")
            {
                loggingLevel = LogLevel::WARN;
            }
            else if (value == "ERROR")
            {
                loggingLevel = LogLevel::ERR;
            }
        }
        else if (key == "persistence")
        {
            persistence = (value == "true");
        }
    }

    return true;
}

int Config::port() const
{
    return serverPort;
}

int Config::maxClients() const
{
    return maximumClients;
}

LogLevel Config::logLevel() const
{
    return loggingLevel;
}

bool Config::persistenceEnabled() const
{
    return persistence;
}