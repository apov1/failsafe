#pragma once

#include <chrono>
#include <sstream>
#include <string_view>

#include "FSHook/FSHook.h"

class FSMessage {
public:
    FSMessage( FSHookConnector *connector, FSHookMessage *msg );
    virtual ~FSMessage();

public: // control
    void AllowOp();
    void BlockOp();

public: // getters
    FSHookEventType EventType() const;
    std::chrono::system_clock::time_point TimePoint() const;
    std::string_view ExecutablePath() const;
    pid_t Pid() const;
    pid_t Tid() const;
    uid_t Uid() const;
    void *Ptr() const
    {
        return static_cast< void * >( msg_ );
    }

public:
    virtual std::string ToString() const;

protected:
    FSHookMessage *msg_;

private:
    bool replied_ = false;
    FSHookConnector *connector_;
};