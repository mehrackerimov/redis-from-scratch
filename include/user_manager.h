#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct User
{
    std::string username;
    std::string passwordHash;
    std::vector<std::string> permissions;
    bool enabled = true;
};
class UserManager
{
public:
    UserManager();

    bool createUser(
        const std::string &username,
        const std::string &password,
        const std::vector<std::string> &permissions);

    bool authenticate(
        const std::string &username,
        const std::string &password) const;

    bool userExists(const std::string &username) const;

    bool loadUsers();
    bool saveUsers() const;

    bool requiresPermission(const std::string &command) const;

    bool hasPermission(
        const std::string &username,
        const std::string &command) const;

    bool addPermission(
        const std::string &username,
        const std::string &permission);

    bool removePermission(
        const std::string &username,
        const std::string &permission);

    bool setUserEnabled(
        const std::string &username,
        bool enabled);

    bool setPassword(
        const std::string &username,
        const std::string &password);

private:
    std::unordered_map<std::string, User> users;
    std::string filePath = "data/users.json";
};