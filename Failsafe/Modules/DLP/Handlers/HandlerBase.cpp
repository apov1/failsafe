#include "Modules/DLP/Handlers/HandlerBase.hpp"

#include "spdlog/spdlog.h"

ActionType HandlerBase::GetActionForOperation( const DLPOperation &op ) const
{
    auto rule = ruleProvider_.GetRule( op );

    if ( rule.has_value() ) {
        spdlog::info( "Action {} matched {}", op.ToString(), rule->ToString() );
        return rule->Action();
    }

    spdlog::info( "Action {} matched no rule!", op.ToString() );
    return ruleProvider_.DefaultAction();
}

void HandlerBase::ExecuteAction( ActionType action, std::shared_ptr< DLPOperation > op )
{
    if ( action == ActionType::Block ) {
        op->Block();
        spdlog::info( "Blocking the incident!" );
    }
    else {
        op->Allow();
    }

    if ( action == ActionType::Block || action == ActionType::Log ) {
        spdlog::info( "Logging the incident!" );
        logger_.SaveLog( action, std::move( op ) );
    }
}