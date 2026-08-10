#include "user_manager.h"

bool UserManager::createUser(
    const std::string& username,
    const std::string& password)
{
    if (users.contains(username))
        return false;

    users[username] = {
        username,
        password
    };

    return true;
}

bool UserManager::authenticate(
    const std::string& username,
    const std::string& password) const
{
    auto it = users.find(username);

    if (it == users.end())
        return false;

    return it->second.password == password;
}

bool UserManager::userExists(
    const std::string& username) const
{
    return users.contains(username);
}