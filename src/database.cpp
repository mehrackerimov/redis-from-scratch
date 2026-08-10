#include <database.h>

bool Database::set(const std::string &key, const std::string &value, std::optional<int> ttl)
{
    std::lock_guard<std::mutex> lock(dbMutex);

    std::optional<std::chrono::steady_clock::time_point> expiresAt = std::nullopt;

    if (ttl.has_value())
    {
        if (ttl.value() <= 0)
        {
            return false;
        }
        expiresAt = std::chrono::steady_clock::now() + std::chrono::seconds(ttl.value());
    }

    db[key] = {value, expiresAt};
    return true;
}

std::optional<std::string> Database::get(const std::string &key)
{
    std::lock_guard<std::mutex> lock(dbMutex);

    auto it = db.find(key);
    if (it == db.end())
    {
        return std::nullopt;
    }

    if (it->second.expiresAt.has_value())
    {
        if (it->second.expiresAt < std::chrono::steady_clock::now())
        {
            db.erase(it);
            return std::nullopt;
        }
    }

    return it->second.value;
}

bool Database::del(const std::string &key)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    return db.erase(key) > 0;
}

bool Database::exists(const std::string &key)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    auto it = db.find(key);
    if (it == db.end())
    {
        return false;
    }

    if (it->second.expiresAt.has_value())
    {
        if (it->second.expiresAt < std::chrono::steady_clock::now())
        {
            db.erase(it);
            return false;
        }
    }

    return true;
}

bool Database::expire(const std::string &key, int seconds)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    auto it = db.find(key);
    if (it == db.end())
    {
        return false;
    }

    if (seconds <= 0)
    {
        return false;
    }

    it->second.expiresAt = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    return true;
}

int Database::ttl(const std::string &key)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    auto it = db.find(key);
    if (it == db.end())
    {
        return -2; // Key does not exist
    }

    if (it->second.expiresAt.has_value())
    {
        auto now = std::chrono::steady_clock::now();
        if (it->second.expiresAt < now)
        {
            db.erase(it);
            return -2; // Key has expired
        }
        else
        {
            auto remaining = std::chrono::duration_cast<std::chrono::seconds>(it->second.expiresAt.value() - now).count();
            return static_cast<int>(remaining);
        }
    }

    return -1; // Key exists but has no expiration
}

std::vector<std::optional<std::string>>
Database::mget(const std::vector<std::string>& keys)
{
    std::vector<std::optional<std::string>> results;

    for (const auto& key : keys)
    {
        results.push_back(get(key));
    }

    return results;
}

bool Database::mset(
    const std::vector<std::pair<std::string, std::string>>& pairs)
{
    std::lock_guard<std::mutex> lock(dbMutex);

    for (const auto& [key, value] : pairs)
    {
        db[key] = Entry{value, std::nullopt};
    }

    return true;
}