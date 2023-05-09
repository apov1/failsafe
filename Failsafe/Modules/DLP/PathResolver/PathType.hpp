#pragma once

#include <string>

enum class PathType {
    Local,
    USB,
    NetworkDrive,
    Dropbox,
};

PathType StringToPathType( const std::string &str );
std::string PathTypeToString( PathType type );