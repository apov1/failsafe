#pragma once

#include <string>

enum class ActionType {
    Ignore,
    Log,
    Block,
};

ActionType ActionFromString( const std::string &str );
std::string ActionToString( ActionType action );