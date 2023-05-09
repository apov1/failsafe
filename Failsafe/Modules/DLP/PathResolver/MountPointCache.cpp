#include "Modules/DLP/PathResolver/MountPointCache.hpp"

#include <algorithm>
#include <fcntl.h>
#include <filesystem>
#include <limits.h>
#include <poll.h>
#include <stdexcept>

#include "USBControl/USBControl.h"
#include "Utils/AtExit.hpp"
#include "spdlog/spdlog.h"

MountPointCache::MountPointCache()
    : mountMonitorThread_( &MountPointCache::MonitorMountPoints, this )
{
}

MountPointCache::~MountPointCache()
{
    isEnding_ = true;
    mountMonitorThread_.join();
}

bool MountPointCache::IsInterestingPath( const std::filesystem::path &path ) const
{
    if ( CheckGVFSMount( path ) )
        return true;

    {
        std::scoped_lock lock( mapMtx_ );
        auto it = std::find_if(
            mountPointCache_.begin(), mountPointCache_.end(), [ &path ]( const auto &item ) {
                const auto &[ mountPoint, _ ] = item;
                return path.string().compare( 0, mountPoint.length(), mountPoint ) == 0;
            } );

        return ( it != mountPointCache_.end() );
    }
}

std::optional< ResolvedPath > MountPointCache::TryMatchPath( std::filesystem::path &&path ) const
{
    if ( CheckGVFSMount( path ) )
        return std::make_optional< ResolvedPath >( PathType::NetworkDrive, std::move( path ) );

    std::scoped_lock lock( mapMtx_ );
    auto it = std::find_if(
        mountPointCache_.begin(), mountPointCache_.end(), [ &path ]( const auto &item ) {
            const auto &[ mountPoint, _ ] = item;
            return path.string().compare( 0, mountPoint.length(), mountPoint ) == 0;
        } );

    if ( it == mountPointCache_.end() )
        return std::nullopt;

    std::string suffix = path.string().substr( it->first.length() + 1 );

    std::filesystem::path remappedPath = std::filesystem::path( it->first ) / suffix;
    return std::make_optional< ResolvedPath >( it->second, std::move( remappedPath ) );
}

std::string MountPointCache::ToString() const
{
    std::stringstream ss;

    ss << "MountPoints(";

    {
        std::scoped_lock lock( mapMtx_ );
        for ( const auto &mountPoint : mountPointCache_ ) {
            ss << "\n"
               << "dir: " << mountPoint.first
               << ", Type: " << PathTypeToString( mountPoint.second );
        }
    }

    return ss.str();
}

void MountPointCache::MonitorMountPoints()
{
    LoadMounts();

    struct pollfd pfd;

    pfd.fd = open( "/proc/mounts", O_RDONLY );
    if ( pfd.fd == 0 ) {
        spdlog::error( "Failed to monitor /proc/mounts!" );
        return;
    }

    pfd.events = POLLERR;
    pfd.revents = 0;

    AtExit wrapUp( [ &pfd ]() { close( pfd.fd ); } );

    while ( poll( &pfd, 1, 1000 ) >= 0 ) {
        if ( isEnding_ )
            return;

        if ( pfd.revents & POLLERR )
            LoadMounts();

        pfd.revents = 0;
    }
}

void MountPointCache::LoadMounts()
{
    spdlog::info( "Loading Mount points!" );
    FILE *procMount = setmntent( "/proc/mounts", "r" );
    if ( procMount == nullptr ) {
        spdlog::error( "Failed to open /proc/mounts" );
        return;
    }

    {
        std::scoped_lock lock( mapMtx_ );
        mountPointCache_.clear();
    }

    struct mntent entry;
    char buff[ 1024 * 8 + 1 ];

    while ( getmntent_r( procMount, &entry, buff, 1024 * 8 ) != NULL ) {
        if ( entry.mnt_fsname[ 0 ] != '/' )
            continue;

        auto fsType = std::string_view( entry.mnt_type );

        if ( fsType == "cifs" || fsType == "nfs" ) {
            // we are dealing with network mounted partition
            std::scoped_lock lock( mapMtx_ );
            mountPointCache_.emplace( entry.mnt_dir, PathType::NetworkDrive );
            continue;
        }

        if ( std::string_view( entry.mnt_fsname ).starts_with( "/dev/sd" ) ) {
            // we can be dealing with USB, lets check
            TryResolveUSBMount( entry );
        }
    }

    endmntent( procMount );

    spdlog::info( "MountPointsCache reloaded! {}", ToString() );
}

void MountPointCache::TryResolveUSBMount( const struct mntent &mountEntry )
{
    std::string fsName = mountEntry.mnt_fsname;
    std::string sysClassPath =
        std::string( "/sys/class/block" ) + fsName.substr( fsName.find_last_of( "/" ) );
    UCDeviceInfo *devnodeInfo = UCDeviceInfoFromSysPath( sysClassPath.c_str() );
    if ( devnodeInfo == NULL )
        return;

    UCDeviceInfo *usbInfo = UCDeviceInfoTryGetUSBParentInfo( devnodeInfo );
    if ( usbInfo != NULL ) {
        UCDeviceInfoRelease( usbInfo );

        {
            std::scoped_lock lock( mapMtx_ );
            mountPointCache_.emplace( mountEntry.mnt_dir, PathType::USB );
        }
    }

    UCDeviceInfoRelease( devnodeInfo );
}

bool MountPointCache::CheckGVFSMount( const std::filesystem::path &path ) const
{
    auto pathPart = path.begin();
    for ( const auto &gvfsPart : GVFS_MOUNTS ) {
        if ( pathPart == path.end() )
            return false;

        if ( gvfsPart.compare( "*" ) == 0 )
            continue;

        if ( gvfsPart != ( *pathPart ) )
            return false;
    }

    return true;
}
