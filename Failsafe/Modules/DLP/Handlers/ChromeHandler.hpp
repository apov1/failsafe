#pragma once

#include "Modules/DLP/Handlers/HandlerBase.hpp"

class ChromeHandler : public HandlerBase {
public:
    ChromeHandler( RuleProvider &provider,ILogger& logger,  PathResolver &pathResolver )
        : HandlerBase( provider, logger, pathResolver )
    {
    }
    virtual bool HandleEvent( std::shared_ptr< FSMessage > event ) override;

private:
    void HandleOpenEvent( std::shared_ptr< OpenFSMessage > openEvent );
};