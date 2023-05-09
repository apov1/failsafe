#pragma once

#include "Modules/DLP/Handlers/HandlerBase.hpp"

class NautilusHandler : public HandlerBase {
public:
    NautilusHandler( RuleProvider &provider, ILogger &logger, PathResolver &pathResolver )
        : HandlerBase( provider, logger, pathResolver )
    {
    }
    virtual bool HandleEvent( std::shared_ptr< FSMessage > event ) override;

private:
    void HandleOpenEvent( std::shared_ptr< OpenFSMessage > openEvent );

    std::shared_ptr< OpenFSMessage > lastOpen_;
};