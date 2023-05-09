#include "SharedComponents/RuleProvider/DLP/DLPOperationType.hpp"

#include <stdexcept>

DLPOperationType DLPOperationFromString( const std::string &str )
{
    if ( str == "upload" )
        return DLPOperationType::Upload;
    if ( str == "instantMessaging" )
        return DLPOperationType::IM;
    if ( str == "fileTransfer" )
        return DLPOperationType::FileTransfer;
    if ( str == "delete" )
        return DLPOperationType::Delete;

    throw std::runtime_error( "Unknown channel " + str );
}

std::string DLPOperationToString( DLPOperationType channel )
{
    switch ( channel ) {
    case DLPOperationType::Upload:
        return "upload";
    case DLPOperationType::IM:
        return "instantMessaging";
    case DLPOperationType::FileTransfer:
        return "fileTransfer";
    case DLPOperationType::Delete:
        return "delete";
    }

    return "";
}