#include "logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <filesystem>

Logger::Logger(LogLevel level)
    : level(level)
{
    std::filesystem::create_directories("logs");
}

void Logger::debug(const std::string& message)
{
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message)
{
    log(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message)
{
    log(LogLevel::WARN, message);
}

void Logger::error(const std::string& message)
{
    log(LogLevel::ERR, message);
}

void Logger::log(LogLevel messageLevel, const std::string& message)
{
    if (messageLevel < level)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);

    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};

    localtime_s(&localTime, &time);

    std::string levelString;

    switch (messageLevel)
    {
        case LogLevel::DEBUG:
            levelString = "DEBUG";
            break;

        case LogLevel::INFO:
            levelString = "INFO";
            break;

        case LogLevel::WARN:
            levelString = "WARN";
            break;

        case LogLevel::ERR:
            levelString = "ERROR";
            break;
    }

    std::ofstream file("logs/server.log", std::ios::app);

    if (!file)
    {
        std::cerr << "Failed to open log file\n";
        return;
    }

    file << '['
         << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S")
         << "] ["
         << levelString
         << "] "
         << message
         << '\n';
}