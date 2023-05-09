#pragma once

#include <filesystem>
#include <sstream>

#include "Modules/DLP/PathResolver/PathType.hpp"

class ResolvedPath {
public:
    ResolvedPath( PathType type, std::filesystem::path path )
        : type_( type ),
          path_( std::move( path ) )
    {
    }

    PathType Type() const
    {
        return type_;
    };

    bool IsHidden() const
    {
        return path_.string().find( "/." ) != std::string::npos ||
               path_.string().find( "/~" ) != std::string::npos;
    }

    const std::filesystem::path &Path() const
    {
        return path_;
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "Path(" << path_.string() << ", Type: " << PathTypeToString( type_ ) << ")";
        return ss.str();
    }

private:
    PathType type_;
    std::filesystem::path path_;
};