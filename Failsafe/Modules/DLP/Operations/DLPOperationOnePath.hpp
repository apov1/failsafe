#pragma once

#include "Modules/DLP/Operations/DLPOperation.hpp"
#include "Modules/DLP/PathResolver/ResolvedPath.hpp"

class DLPOperationOnePath : public DLPOperation {
public:
    DLPOperationOnePath( ResolvedPath path, DLPOperationType channel,
                         std::shared_ptr< FSMessage > controlMessage )
        : DLPOperation( channel, std::move( controlMessage ) ),
          path_( std::move( path ) )
    {
    }

    const ResolvedPath &Path() const noexcept
    {
        return path_;
    }

private:
    ResolvedPath path_;
};