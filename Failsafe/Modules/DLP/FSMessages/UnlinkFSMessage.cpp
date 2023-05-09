#include "Modules/DLP/FSMessages/UnlinkFSMessage.hpp"

UnlinkFSMessage::UnlinkFSMessage( FSHookConnector *connector, FSHookMessage *msg )
    : FSMessage( connector, msg )
{
}

std::string_view UnlinkFSMessage::FilePath() const
{
    return std::string_view{ msg_->event.unlink.path };
}

/* virtual */ std::string UnlinkFSMessage::ToString() const
{
    std::stringstream ss;

    ss << FSMessage::ToString();
    ss << "EventInfo: (FilePath: " << FilePath() << ")";

    return ss.str();
}