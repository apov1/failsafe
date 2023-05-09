#pragma once

#include "Modules/DLP/FSMessages/FSMessage.hpp"

class RenameFSMessage : public FSMessage {
public:
    RenameFSMessage( FSHookConnector *connector, FSHookMessage *msg );

    std::string_view SourcePath() const;
    std::string_view DestinationPath() const;

    virtual std::string ToString() const override;
};