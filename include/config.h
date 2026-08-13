#pragma once

#include <string>
#include "logger.h"

class Config
{
public:
    explicit Config(const std::string& path);

    bool load();

    int port() const;
    int maxClients() const;
    LogLevel logLevel() const;
    bool persistenceEnabled() const;

private:
    std::string path;

    int serverPort = 6379;
    int maximumClients = 100;
    LogLevel loggingLevel = LogLevel::INFO;
    bool persistence = true;
};