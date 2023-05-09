#pragma once

#include "Modules/DLP/FSMessages/FSMessage.hpp"

class OpenFSMessage : public FSMessage {
public:
    OpenFSMessage( FSHookConnector *connector, FSHookMessage *msg );

    std::string_view FilePath() const;
    int Flags() const;
    bool HasReadOnlyFlag() const;
    bool IsDirectory() const;

    virtual std::string ToString() const override;
};