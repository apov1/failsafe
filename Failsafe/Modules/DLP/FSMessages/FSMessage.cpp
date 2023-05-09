#include "Modules/DLP/FSMessages/FSMessage.hpp"

#include <iostream>
#include <sstream>

FSMessage::FSMessage( FSHookConnector *connector, FSHookMessage *msg )
    : msg_( msg ),
      connector_( connector )
{
}

FSMessage::~FSMessage()
{
    AllowOp();

    FSHookFreeMessage( msg_ );
}

void FSMessage::AllowOp()
{
    if ( replied_ )
        return;

    FSHookReplyToEvent( connector_, msg_, true );
    replied_ = true;
}

void FSMessage::BlockOp()
{
    if ( replied_ )
        return;

    FSHookReplyToEvent( connector_, msg_, false );
    replied_ = true;
}

FSHookEventType FSMessage::EventType() const
{
    return msg_->eventType;
}

std::chrono::system_clock::time_point FSMessage::TimePoint() const
{
    auto duration =
        std::chrono::seconds{ msg_->time.tv_sec } + std::chrono::nanoseconds{ msg_->time.tv_nsec };

    return std::chrono::system_clock::time_point{
        std::chrono::duration_cast< std::chrono::system_clock::duration >( duration ) };
}

std::string_view FSMessage::ExecutablePath() const
{
    return std::string_view{ msg_->process.executablePath };
}

pid_t FSMessage::Pid() const
{
    return msg_->process.pid;
}

pid_t FSMessage::Tid() const
{
    return msg_->process.tid;
}

uid_t FSMessage::Uid() const
{
    return msg_->process.uid;
}

/* virtual */ std::string FSMessage::ToString() const
{
    std::stringstream ss;

    ss << "Process: (ExecutablePath: " << ExecutablePath() << ", PID: " << Pid()
       << ", TID: " << Tid() << ", UID: " << Uid() << ")\n";

    return ss.str();
}
