#include "Modules/DLP/PathResolver/PathType.hpp"

#include <stdexcept>

PathType StringToPathType( const std::string &str )
{
    if ( str == "local" )
        return PathType::Local;
    if ( str == "usb" )
        return PathType::USB;
    if ( str == "networkDrive" )
        return PathType::NetworkDrive;
    if ( str == "dropbox" )
        return PathType::Dropbox;

    throw std::runtime_error( "Unknown path type " + str );
}

std::string PathTypeToString( PathType type )
{
    switch ( type ) {
    case PathType::Local:
        return "local";
    case PathType::USB:
        return "usb";
    case PathType::NetworkDrive:
        return "networkDrive";
    case PathType::Dropbox:
        return "dropbox";
    }

    return "";
}