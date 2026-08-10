#include "command_parser.h"
#include "tokenizer.h"

Command CommandParser::parse(const std::string& input)
{
    auto tokens = Tokenizer::tokenize(input);

    Command command;

    if (tokens.empty())
        return command;

    command.name = tokens[0];

    for (size_t i = 1; i < tokens.size(); ++i)
    {
        command.args.push_back(tokens[i]);
    }

    return command;
}