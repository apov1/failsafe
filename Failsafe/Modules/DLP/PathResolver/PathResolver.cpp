#include "Modules/DLP/PathResolver/PathResolver.hpp"

#include <filesystem>

#include "USBControl/USBControl.h"
#include "spdlog/spdlog.h"

bool PathResolver::IsInterestingFilePath( const ResolvedPath &path ) const
{
    if ( std::string_view{ path.Path().c_str() }.starts_with( "/home" ) )
        return true;

    return ( cloudCache_.IsInterestingPath( path.Path() ) ||
             mountCache_.IsInterestingPath( path.Path() ) );
}

ResolvedPath PathResolver::ResolvePath( std::string_view path ) const
{
    std::filesystem::path canonical = std::filesystem::weakly_canonical( path );

    auto cloudMappedPath = cloudCache_.TryMatchPath( std::move( canonical ) );
    if ( cloudMappedPath.has_value() )
        return cloudMappedPath.value();

    auto mountMappedPath = mountCache_.TryMatchPath( std::move( canonical ) );
    if ( mountMappedPath.has_value() )
        return mountMappedPath.value();

    return ResolvedPath( PathType::Local, std::move( canonical ) );
}
