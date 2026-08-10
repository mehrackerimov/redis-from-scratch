#include "tokenizer.h"

#include <sstream>

std::vector<std::string> Tokenizer::tokenize(
    const std::string& input)
{
    std::vector<std::string> tokens;
    std::istringstream iss(input);

    std::string token;

    while (iss >> token)
    {
        tokens.push_back(token);
    }

    return tokens;
}