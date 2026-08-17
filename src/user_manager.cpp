#include "user_manager.h"
#include "password_hasher.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>

using json = nlohmann::json;

UserManager::UserManager()
{
    loadUsers();
}

bool UserManager::createUser(
    const std::string& username,
    const std::string& password,
    const std::vector<std::string>& permissions)
{
    if (username.empty())
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
    user.permissions = permissions;
    user.enabled = true;

    users[username] = user;

    return saveUsers();
}

bool UserManager::authenticate(
    const std::string &username,
    const std::string &password) const
{
    auto it = users.find(username);

    if (it == users.end())
    {
        return false;
    }

    if (!it->second.enabled || it->second.passwordHash.empty())
    {
        return false;
    }

    return PasswordHasher::verify(
        password,
        it->second.passwordHash);
}

bool UserManager::userExists(const std::string &username) const
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

        for (const auto &userData : data["users"])
        {
            User user;

            user.username =
                userData.at("username").get<std::string>();

            user.passwordHash =
                userData.at("password_hash").get<std::string>();

            user.enabled = userData.value("enabled", true);

            if (userData.contains("permissions"))
            {
                user.permissions =
                    userData["permissions"]
                        .get<std::vector<std::string>>();
            }
            else
            {
                user.permissions = {};
            }

            users[user.username] = user;
        }
    }
    catch (const json::exception &)
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

    for (const auto &[username, user] : users)
    {
        data["users"].push_back({{"username", user.username},
                                 {"password_hash", user.passwordHash},
                                 {"permissions", user.permissions},
                                 {"enabled", user.enabled}});
    }

    file << data.dump(4) << '\n';

    return true;
}

bool UserManager::requiresPermission(
    const std::string &command) const
{
    return command != "LOGIN" && command != "PING";
}

bool UserManager::hasPermission(
    const std::string &username,
    const std::string &command) const
{
    auto it = users.find(username);

    if (it == users.end())
    {
        return false;
    }

    const User &user = it->second;

    if (std::find(
            user.permissions.begin(),
            user.permissions.end(),
            "*") != user.permissions.end())
    {
        return true;
    }

    return std::find(
               user.permissions.begin(),
               user.permissions.end(),
               command) != user.permissions.end();
}

bool UserManager::addPermission(
    const std::string &username,
    const std::string &permission)
{
    auto it = users.find(username);

    if (it == users.end() || permission.empty())
    {
        return false;
    }

    auto &permissions = it->second.permissions;

    if (std::find(
            permissions.begin(),
            permissions.end(),
            permission) != permissions.end())
    {
        return true;
    }

    permissions.push_back(permission);

    return saveUsers();
}

bool UserManager::removePermission(
    const std::string &username,
    const std::string &permission)
{
    auto it = users.find(username);

    if (it == users.end())
    {
        return false;
    }

    auto &permissions = it->second.permissions;

    auto permissionIt = std::find(
        permissions.begin(),
        permissions.end(),
        permission);

    if (permissionIt == permissions.end())
    {
        return false;
    }

    permissions.erase(permissionIt);

    return saveUsers();
}

bool UserManager::setUserEnabled(
    const std::string &username,
    bool enabled)
{
    auto it = users.find(username);

    if (it == users.end())
    {
        return false;
    }

    it->second.enabled = enabled;

    return saveUsers();
}

bool UserManager::setPassword(
    const std::string& username,
    const std::string& password)
{
    auto it = users.find(username);

    if (it == users.end())
    {
        return false;
    }

    it->second.passwordHash =
        PasswordHasher::hash(password);

    return saveUsers();
}