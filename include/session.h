#pragma once

#include <string>

struct Session
{
    bool authenticated = false;
    std::string username;
};