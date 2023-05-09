#pragma once

#include "Modules/DLP/Operations/DLPOperation.hpp"
#include "Modules/DLP/PathResolver/ResolvedPath.hpp"

class DLPOperationTwoPath : public DLPOperation {
public:
    DLPOperationTwoPath( ResolvedPath sourcePath, ResolvedPath destPath,
                         std::shared_ptr< FSMessage > controlMessage )
        : DLPOperation( DLPOperationType::FileTransfer, std::move( controlMessage ) ),
          sourcePath_( std::move( sourcePath ) ),
          destPath_( std::move( destPath ) )
    {
    }

    const ResolvedPath &SourcePath() const noexcept
    {
        return sourcePath_;
    }
    const ResolvedPath &DestinationPath() const noexcept
    {
        return destPath_;
    }

private:
    ResolvedPath sourcePath_;
    ResolvedPath destPath_;
};