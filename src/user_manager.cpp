#include "user_manager.h"
#include "password_hasher.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

using json = nlohmann::json;

UserManager::UserManager()
{
    loadUsers();
}

bool UserManager::createUser(
    const std::string& username,
    const std::string& password)
{
    if (username.empty() || password.empty())
    {
        return false;
    }

    if (userExists(username))
    {
        return false;
    }

    User user;
    user.username = username;
    user.passwordHash = PasswordHasher::hash(password);

    users[username] = user;

    return saveUsers();
}

bool UserManager::authenticate(
    const std::string& username,
    const std::string& password) const
{
    auto it = users.find(username);

    if (it == users.end())
    {
        return false;
    }

    return PasswordHasher::verify(
        password,
        it->second.passwordHash
    );
}

bool UserManager::userExists(const std::string& username) const
{
    return users.find(username) != users.end();
}

bool UserManager::loadUsers()
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::filesystem::create_directories("data");

        std::ofstream newFile(filePath);

        if (!newFile.is_open())
        {
            return false;
        }

        newFile << "{\n";
        newFile << "    \"users\": []\n";
        newFile << "}\n";

        newFile.close();

        return true;
    }

    try
    {
        json data;
        file >> data;

        if (!data.contains("users"))
        {
            return false;
        }

        for (const auto& userData : data["users"])
        {
            User user;

            user.username =
                userData.at("username").get<std::string>();

            user.passwordHash =
                userData.at("password_hash").get<std::string>();

            users[user.username] = user;
        }
    }
    catch (const json::exception&)
    {
        return false;
    }

    return true;
}

bool UserManager::saveUsers() const
{
    std::filesystem::create_directories("data");

    std::ofstream file(filePath);

    if (!file.is_open())
    {
        return false;
    }

    json data;
    data["users"] = json::array();

    for (const auto& [username, user] : users)
    {
        data["users"].push_back({
            {"username", user.username},
            {"password_hash", user.passwordHash}
        });
    }

    file << data.dump(4) << '\n';

    return true;
}