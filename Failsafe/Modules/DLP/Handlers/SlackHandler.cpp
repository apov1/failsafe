#include "Modules/DLP/Handlers/SlackHandler.hpp"

#include "Modules/DLP/FSMessages/OpenFSMessage.hpp"
#include "Modules/DLP/Operations/DLPOperationOnePath.hpp"

#include "spdlog/spdlog.h"

bool SlackHandler::HandleEvent( std::shared_ptr< FSMessage > event )
{
    switch ( event->EventType() ) {
    case FSH_EVENT_OPEN: {
        HandleOpenEvent( std::static_pointer_cast< OpenFSMessage >( std::move( event ) ) );
        return true;
    } break;
    default:
        return false;
    }
}

void SlackHandler::HandleOpenEvent( std::shared_ptr< OpenFSMessage > openEvent )
{
    if ( openEvent->IsDirectory() )
        return;

    auto path = pathResolver_.ResolvePath( openEvent->FilePath() );

    if ( path.IsHidden() )
        return;

    if ( !pathResolver_.IsInterestingFilePath( path ) )
        return;

    spdlog::info( "Detected IM Send by Slack of file {}", openEvent->FilePath() );

    auto imSendOp = std::make_shared< DLPOperationOnePath >(
        std::move( path ), DLPOperationType::IM, std::move( openEvent ) );

    auto action = GetActionForOperation( *imSendOp );
    ExecuteAction( action, std::move( imSendOp ) );
}