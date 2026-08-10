#pragma once

#include <string>
#include <unordered_map>

struct User
{
    std::string username;
    std::string password;
};

class UserManager
{
public:
    bool createUser(const std::string& username,
                    const std::string& password);

    bool authenticate(const std::string& username,
                      const std::string& password) const;

    bool userExists(const std::string& username) const;

private:
    std::unordered_map<std::string, User> users;
};