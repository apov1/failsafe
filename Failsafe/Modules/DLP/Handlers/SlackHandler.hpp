#pragma once

#include "Modules/DLP/Handlers/HandlerBase.hpp"

class SlackHandler : public HandlerBase {
public:
    SlackHandler( RuleProvider &provider, ILogger &logger, PathResolver &pathResolver )
        : HandlerBase( provider, logger, pathResolver )
    {
    }
    virtual bool HandleEvent( std::shared_ptr< FSMessage > event ) override;

private:
    void HandleOpenEvent( std::shared_ptr< OpenFSMessage > openEvent );
};