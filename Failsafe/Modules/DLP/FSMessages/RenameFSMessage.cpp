#include "Modules/DLP/FSMessages/RenameFSMessage.hpp"

RenameFSMessage::RenameFSMessage( FSHookConnector *connector, FSHookMessage *msg )
    : FSMessage( connector, msg )
{
}

std::string_view RenameFSMessage::SourcePath() const
{
    return std::string_view{ msg_->event.rename.sourcePath };
}

std::string_view RenameFSMessage::DestinationPath() const
{
    return std::string_view{ msg_->event.rename.destPath };
}

/* virtual */ std::string RenameFSMessage::ToString() const
{
    std::stringstream ss;

    ss << FSMessage::ToString();
    ss << "EventInfo: (SourcePath: " << SourcePath() << ", DestinationPath:" << DestinationPath()
       << ")";

    return ss.str();
}