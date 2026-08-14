#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct User
{
    std::string username;
    std::string passwordHash;
    std::vector<std::string> permissions;
};

class UserManager
{
public:
    UserManager();

    bool createUser(
        const std::string& username,
        const std::string& password
    );

    bool authenticate(
        const std::string& username,
        const std::string& password
    ) const;

    bool userExists(const std::string& username) const;

    bool loadUsers();
    bool saveUsers() const;

private:
    std::unordered_map<std::string, User> users;
    std::string filePath = "data/users.json";
};