#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "Modules/DLP/PathResolver/CloudCache.hpp"
#include "Modules/DLP/PathResolver/MountPointCache.hpp"
#include "Modules/DLP/PathResolver/ResolvedPath.hpp"

class PathResolver {
public:
    bool IsInterestingFilePath( const ResolvedPath &path ) const;
    ResolvedPath ResolvePath( std::string_view path ) const;

private:
    void LoadCloudMounts();

    CloudCache cloudCache_;
    MountPointCache mountCache_;
};