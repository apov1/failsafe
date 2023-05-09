#include "Modules/DLP/Handlers/ChromeHandler.hpp"

#include <fcntl.h>

#include "Modules/DLP/FSMessages/OpenFSMessage.hpp"
#include "Modules/DLP/Operations/DLPOperationOnePath.hpp"

#include "spdlog/spdlog.h"

bool ChromeHandler::HandleEvent( std::shared_ptr< FSMessage > event )
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

void ChromeHandler::HandleOpenEvent( std::shared_ptr< OpenFSMessage > openEvent )
{
    if ( openEvent->IsDirectory() )
        return;

    if ( openEvent->Flags() & O_TMPFILE )
        return;

    if ( !openEvent->HasReadOnlyFlag() )
        return;

    if ( openEvent->Pid() == openEvent->Tid() )
        return;

    auto path = pathResolver_.ResolvePath( openEvent->FilePath() );

    // We are not interested in files in hidden paths
    if ( path.IsHidden() )
        return;

    if ( !pathResolver_.IsInterestingFilePath( path ) )
        return;

    spdlog::info( "Detected upload by Google Chrome of file {}", openEvent->FilePath() );

    auto uploadOp = std::make_shared< DLPOperationOnePath >(
        std::move( path ), DLPOperationType::Upload, std::move( openEvent ) );

    auto action = GetActionForOperation( *uploadOp );
    ExecuteAction( action, std::move( uploadOp ) );
}