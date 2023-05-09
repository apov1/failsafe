#include "Modules/DLP/Handlers/NautilusHandler.hpp"

#include "Modules/DLP/FSMessages/OpenFSMessage.hpp"
#include "Modules/DLP/Operations/DLPOperationTwoPath.hpp"

#include "spdlog/spdlog.h"

bool NautilusHandler::HandleEvent( std::shared_ptr< FSMessage > event )
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

void NautilusHandler::HandleOpenEvent( std::shared_ptr< OpenFSMessage > openEvent )
{
    // We are not interested in opening directory
    if ( openEvent->IsDirectory() )
        return;

    auto resolvedPath = pathResolver_.ResolvePath( openEvent->FilePath() );

    // We are not interested in files in hidden paths
    if ( resolvedPath.IsHidden() )
        return;

    if ( !pathResolver_.IsInterestingFilePath( resolvedPath ) )
        return;

    // We cache last readonly open event in case open with write arrived, so we know where the
    // copy is from.
    if ( openEvent->HasReadOnlyFlag() ) {
        openEvent->AllowOp();
        std::swap( lastOpen_, openEvent );
        return;
    }

    // just in case
    if ( lastOpen_ == nullptr )
        return;

    auto source = pathResolver_.ResolvePath( lastOpen_->FilePath() );
    lastOpen_ = nullptr;

    spdlog::info( "Detected copy event by Nautilus from {} to {}.", source.ToString(),
                  resolvedPath.ToString() );

    auto copyOp = std::make_shared< DLPOperationTwoPath >(
        std::move( source ), std::move( resolvedPath ), std::move( openEvent ) );

    auto action = GetActionForOperation( *copyOp );
    ExecuteAction( action, std::move( copyOp ) );
}