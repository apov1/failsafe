#pragma once

#include <memory>

#include "Modules/DLP/FSMessages/FSMessage.hpp"
#include "SharedComponents/RuleProvider/DLP/DLPOperationType.hpp"

class DLPOperation {
public:
    virtual ~DLPOperation() = default;

    DLPOperationType Operation() const
    {
        return operation_;
    }

    void Allow()
    {
        controlMessage_->AllowOp();
    }

    void Block()
    {
        controlMessage_->BlockOp();
    }

    uid_t UserId() const
    {
        return controlMessage_->Uid();
    }

    std::chrono::system_clock::time_point TimePoint() const
    {
        return controlMessage_->TimePoint();
    }

    std::string_view ExecutablePath() const
    {
        return controlMessage_->ExecutablePath();
    }

    std::string ToString() const
    {
        return controlMessage_->ToString();
    }

protected:
    DLPOperation( DLPOperationType operation, std::shared_ptr< FSMessage > controlMessage )
        : operation_( operation ),
          controlMessage_( std::move( controlMessage ) )
    {
    }

    DLPOperationType operation_;
    std::shared_ptr< FSMessage > controlMessage_;
};