#pragma once

#include <memory>

#include "Modules/DLP/FSMessages/OpenFSMessage.hpp"
#include "Modules/DLP/FSMessages/RenameFSMessage.hpp"
#include "Modules/DLP/FSMessages/UnlinkFSMessage.hpp"
#include "Modules/DLP/Operations/DLPOperation.hpp"
#include "Modules/DLP/PathResolver/PathResolver.hpp"
#include "SharedComponents/IncidentLogger/ILogger.hpp"
#include "SharedComponents/RuleProvider/RuleProvider.hpp"

class HandlerBase {
public:
    HandlerBase( RuleProvider &provider, ILogger &logger, PathResolver &pathResolver )
        : ruleProvider_( provider ),
          logger_( logger ),
          pathResolver_( pathResolver )
    {
    }

    virtual ~HandlerBase() = default;

    virtual bool HandleEvent( std::shared_ptr< FSMessage > event ) = 0;

protected:
    RuleProvider &ruleProvider_;
    ILogger &logger_;
    PathResolver &pathResolver_;

    ActionType GetActionForOperation( const DLPOperation &op ) const;
    void ExecuteAction( ActionType action, std::shared_ptr< DLPOperation > op );
};