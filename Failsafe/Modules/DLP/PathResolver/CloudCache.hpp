#pragma once

#include <atomic>
#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "Modules/DLP/PathResolver/PathType.hpp"
#include "Modules/DLP/PathResolver/ResolvedPath.hpp"

class CloudCache {
public:
    CloudCache();
    ~CloudCache();

    bool IsInterestingPath( const std::filesystem::path &path ) const;
    std::optional< ResolvedPath > TryMatchPath( std::filesystem::path &&path ) const;

private:
    void MonitorCloudSync();

    void LoadDropboxFile();
    void LocateConfigFiles();

    std::atomic< bool > isEnding_ = false;
    mutable std::mutex mapMtx_;
    std::unordered_map< std::string, PathType > cloudSyncCache_;

    std::unordered_set< std::string > configFiles_;

    std::thread cloudSyncMonitorThread_;
};