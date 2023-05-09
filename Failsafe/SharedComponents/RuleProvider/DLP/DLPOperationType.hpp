#pragma once

#include <string>

enum class DLPOperationType {
    Upload,
    IM,
    FileTransfer,
    Delete,
};

DLPOperationType DLPOperationFromString( const std::string &str );
std::string DLPOperationToString( DLPOperationType channel );