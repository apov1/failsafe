#pragma once

#include "Modules/DLP/Handlers/HandlerBase.hpp"

class GenericHandler : public HandlerBase {
public:
    GenericHandler( RuleProvider &provider,ILogger& logger,  PathResolver &pathResolver )
        : HandlerBase( provider, logger, pathResolver )
    {
    }
    virtual bool HandleEvent( std::shared_ptr< FSMessage > event ) override;

private:
    void HandleRenameEvent( std::shared_ptr< RenameFSMessage > renameEvent );
    void HandleUnlinkEvent( std::shared_ptr< UnlinkFSMessage > unlinkEvent );
};