#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <chrono>
#include <mutex>

struct Entry
{
    std::string value;
    std::optional<std::chrono::steady_clock::time_point> expiresAt;
};

class Database
{
public:
    bool set(const std::string& key,
             const std::string& value,
             std::optional<int> ttl = std::nullopt);

    std::optional<std::string> get(const std::string& key);

    bool del(const std::string& key);

    bool exists(const std::string& key);

    bool expire(const std::string& key, int seconds);

    int ttl(const std::string& key);

private:
    std::unordered_map<std::string, Entry> db;
    std::mutex dbMutex;
};