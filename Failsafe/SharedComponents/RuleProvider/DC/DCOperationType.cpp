#include "SharedComponents/RuleProvider/DC/DCOperationType.hpp"

#include <stdexcept>

DCOperationType DCOperationFromString( const std::string &str )
{
    if ( str == "deviceAdd" )
        return DCOperationType::Add;
    if ( str == "deviceRemove" )
        return DCOperationType::Remove;

    throw std::runtime_error( "Unknown type " + str );
}

std::string DCOperationToString( DCOperationType type )
{
    switch ( type ) {
    case DCOperationType::Add:
        return "deviceAdd";
    case DCOperationType::Remove:
        return "deviceRemove";
    }

    return "unknown";
}