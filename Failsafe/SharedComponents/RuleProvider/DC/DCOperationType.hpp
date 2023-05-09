#pragma once

#include <string>

enum class DCOperationType {
    Add,
    Remove,
};

DCOperationType DCOperationFromString( const std::string &str );
std::string DCOperationToString( DCOperationType type );
