#include "Modules/DLP/PathResolver/CloudCache.hpp"

#include <algorithm>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <poll.h>
#include <sys/inotify.h>

#include "USBControl/USBControl.h"
#include "Utils/AtExit.hpp"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

CloudCache::CloudCache()
    : cloudSyncMonitorThread_( &CloudCache::MonitorCloudSync, this )
{
}

CloudCache::~CloudCache()
{
    isEnding_ = true;
    cloudSyncMonitorThread_.join();
}

bool CloudCache::IsInterestingPath( const std::filesystem::path &path ) const
{
    std::scoped_lock lock( mapMtx_ );
    auto it = std::find_if(
        cloudSyncCache_.begin(), cloudSyncCache_.end(), [ &path ]( const auto &item ) {
            const auto &[ mountPoint, _ ] = item;
            return path.string().compare( 0, mountPoint.length(), mountPoint ) == 0;
        } );

    return ( it != cloudSyncCache_.end() );
}

std::optional< ResolvedPath > CloudCache::TryMatchPath( std::filesystem::path &&path ) const
{
    std::scoped_lock lock( mapMtx_ );
    auto it = std::find_if(
        cloudSyncCache_.begin(), cloudSyncCache_.end(), [ &path ]( const auto &item ) {
            const auto &[ mountPoint, _ ] = item;
            return path.string().compare( 0, mountPoint.length(), mountPoint ) == 0;
        } );

    if ( it == cloudSyncCache_.end() )
        return std::nullopt;

    return std::make_optional< ResolvedPath >( it->second, std::move( path ) );
}

void CloudCache::MonitorCloudSync()
{
    LocateConfigFiles();
    LoadDropboxFile();

    int fd = inotify_init1( O_NONBLOCK );
    if ( fd == 0 ) {
        spdlog::error( "Failed to init inofity!" );
        return;
    }
    for ( const auto &configFile : configFiles_ ) {
        int rc = inotify_add_watch( fd, configFile.c_str(), IN_MODIFY );
        if ( rc < 0 )
            spdlog::error( "Failed to add watch on {}", configFile );
    }

    AtExit wrapUp( [ fd ]() { close( fd ); } );

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    char buff[ 256 ];

    int rc = 0;
    while ( ( rc = poll( &pfd, 1, 1000 ) ) >= 0 ) {
        if ( isEnding_ )
            return;

        if ( rc == 0 )
            continue;

        if ( pfd.revents & POLLIN ) {
            spdlog::info( "Detected dropbox change" );
            while ( read( fd, buff, 256 ) > 0 ) {
                buff[ 255 ] = 0;
                // just drop the event data
            }
            // wait for dropbox to modify the file
            std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
            LoadDropboxFile();
        }

        pfd.revents = 0;
    }
}

void CloudCache::LoadDropboxFile()
{
    std::scoped_lock lock( mapMtx_ );

    cloudSyncCache_.clear();

    for ( auto const &configFile : configFiles_ ) {
        try {
            std::ifstream config( configFile );

            auto json = nlohmann::json::parse( config );
            for ( const auto &entry : json ) {
                if ( entry.contains( "path" ) ) {
                    cloudSyncCache_.emplace( entry[ "path" ], PathType::Dropbox );
                    spdlog::info( "Added {} to dropbox cache.", entry[ "path" ] );
                }
            }
        }
        catch ( std::exception &e ) {
            spdlog::error( "Caught exception during Dropbox config processing {}", e.what() );
        }
    }
}

void CloudCache::LocateConfigFiles()
{
    for ( auto const &directory : std::filesystem::directory_iterator( "/home" ) ) {
        auto dropboxFile = directory.path() / ".dropbox/info.json";
        if ( !std::filesystem::exists( dropboxFile ) )
            continue;

        configFiles_.emplace( dropboxFile.string() );
    }
}