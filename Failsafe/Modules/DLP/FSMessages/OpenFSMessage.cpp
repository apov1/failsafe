#include "Modules/DLP/FSMessages/OpenFSMessage.hpp"

#include <fcntl.h>

static std::string OpenFlagsToString( int flags )
{
    std::stringstream ss;

    if ( flags & O_WRONLY )
        ss << "O_WRONLY ";
    else if ( flags & O_RDWR )
        ss << "O_RDWR ";
    else
        ss << "O_RDONLY ";

    if ( flags & O_CREAT )
        ss << "O_CREAT ";
    if ( flags & O_EXCL )
        ss << "O_EXCL ";
    if ( flags & O_NOCTTY )
        ss << "O_NOCTTY ";
    if ( flags & O_TRUNC )
        ss << "O_TRUNC ";
    if ( flags & O_APPEND )
        ss << "O_APPEND ";
    if ( flags & O_NONBLOCK )
        ss << "O_NONBLOCK ";
    if ( flags & O_DIRECTORY )
        ss << "O_DIRECTORY ";
    if ( flags & O_TMPFILE )
        ss << "O_TMPFILE ";

    return ss.str();
}

OpenFSMessage::OpenFSMessage( FSHookConnector *connector, FSHookMessage *msg )
    : FSMessage( connector, msg )
{
}

std::string_view OpenFSMessage::FilePath() const
{
    return std::string_view{ msg_->event.open.path };
}

int OpenFSMessage::Flags() const
{
    return msg_->event.open.flags;
}

bool OpenFSMessage::HasReadOnlyFlag() const
{
    bool hasWriteFlag = static_cast< bool >( msg_->event.open.flags & O_WRONLY );
    bool hasReadWriteFlag = static_cast< bool >( msg_->event.open.flags & O_RDWR );

    return !hasWriteFlag && !hasReadWriteFlag;
}

bool OpenFSMessage::IsDirectory() const
{
    return static_cast< bool >( msg_->event.open.flags & O_DIRECTORY );
}

/* virtual */ std::string OpenFSMessage::ToString() const
{
    std::stringstream ss;

    ss << FSMessage::ToString();
    ss << "EventInfo: (FilePath: " << FilePath() << ", Flags:" << OpenFlagsToString( Flags() )
       << ")";

    return ss.str();
}