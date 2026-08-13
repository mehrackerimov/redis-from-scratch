#pragma once

#include <string>
#include <mutex>
#include <fstream>
#include "logger.h"

enum class LogLevel
{
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERR = 3
};

class Logger
{
public:
    explicit Logger(LogLevel level);

    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

private:
    void log(LogLevel messageLevel, const std::string& message);

    LogLevel level;
    std::mutex mutex;
};