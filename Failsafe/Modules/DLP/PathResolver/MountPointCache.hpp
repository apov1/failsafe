#pragma once

#include <atomic>
#include <filesystem>
#include <mntent.h>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>

#include "Modules/DLP/PathResolver/PathType.hpp"
#include "Modules/DLP/PathResolver/ResolvedPath.hpp"

class MountPointCache {
public:
    MountPointCache();
    ~MountPointCache();

    bool IsInterestingPath(const std::filesystem::path &path) const;
    std::optional< ResolvedPath > TryMatchPath( std::filesystem::path &&path ) const;

    std::string ToString() const;

private:
    void MonitorMountPoints();
    void LoadMounts();
    void TryResolveUSBMount( const struct mntent &mountEntry );
    bool CheckGVFSMount( const std::filesystem::path &path ) const;

    std::atomic< bool > isEnding_ = false;
    mutable std::mutex mapMtx_;
    std::unordered_map< std::string, PathType > mountPointCache_;

    std::thread mountMonitorThread_;

    static const inline std::filesystem::path GVFS_MOUNTS = "/run/user/*/gvfs";
};