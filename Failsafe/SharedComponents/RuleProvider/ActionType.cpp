#include "SharedComponents/RuleProvider/ActionType.hpp"

#include <stdexcept>

ActionType ActionFromString( const std::string &str )
{
    if ( str == "ignore" )
        return ActionType::Ignore;
    else if ( str == "log" )
        return ActionType::Log;
    else if ( str == "block" )
        return ActionType::Block;

    throw std::runtime_error( std::string( "Unknown action type " ) + str );
}

std::string ActionToString( ActionType action )
{
    switch ( action ) {
    case ActionType::Ignore:
        return "ignore";
    case ActionType::Log:
        return "log";
    case ActionType::Block:
        return "block";
    }

    return "";
}