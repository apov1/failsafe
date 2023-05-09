#pragma once

#include "Modules/DLP/FSMessages/FSMessage.hpp"

class UnlinkFSMessage : public FSMessage {
public:
    UnlinkFSMessage( FSHookConnector *connector, FSHookMessage *msg );

    std::string_view FilePath() const;

    virtual std::string ToString() const override;
};