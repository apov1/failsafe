#include "Modules/DLP/Handlers/GenericHandler.hpp"

#include "Modules/DLP/Operations/DLPOperationOnePath.hpp"
#include "Modules/DLP/Operations/DLPOperationTwoPath.hpp"

#include "spdlog/spdlog.h"

bool GenericHandler::HandleEvent( std::shared_ptr< FSMessage > event )
{
    switch ( event->EventType() ) {
    case FSH_EVENT_RENAME: {
        HandleRenameEvent( std::static_pointer_cast< RenameFSMessage >( std::move( event ) ) );
        return true;
    } break;
    case FSH_EVENT_UNLINK: {
        HandleUnlinkEvent( std::static_pointer_cast< UnlinkFSMessage >( std::move( event ) ) );
        return true;
    } break;
    default:
        return false;
    }
}

void GenericHandler::HandleRenameEvent( std::shared_ptr< RenameFSMessage > renameEvent )
{
    auto source = pathResolver_.ResolvePath( renameEvent->SourcePath() );
    auto destination = pathResolver_.ResolvePath( renameEvent->DestinationPath() );

    // We are not interested in files in hidden paths
    if ( source.IsHidden() && destination.IsHidden() )
        return;

    if ( !pathResolver_.IsInterestingFilePath( source ) &&
         !pathResolver_.IsInterestingFilePath( destination ) )
        return;

    spdlog::info( "Detected move event {} from {} to {}.", renameEvent->ToString(),
                  source.ToString(), destination.ToString() );

    auto moveOp = std::make_shared< DLPOperationTwoPath >(
        std::move( source ), std::move( destination ), std::move( renameEvent ) );

    auto action = GetActionForOperation( *moveOp );
    ExecuteAction( action, std::move( moveOp ) );
}

void GenericHandler::HandleUnlinkEvent( std::shared_ptr< UnlinkFSMessage > unlinkEvent )
{
    if ( unlinkEvent->FilePath().starts_with( "/run/udev" ) )
        return;

    auto path = pathResolver_.ResolvePath( unlinkEvent->FilePath() );

    // We are not interested in files in hidden paths
    if ( path.IsHidden() )
        return;

    if ( !pathResolver_.IsInterestingFilePath( path ) )
        return;

    spdlog::info( "Detected unlink event by {} of file {}.", unlinkEvent->ExecutablePath(),
                  path.ToString() );

    auto unlinkOp = std::make_shared< DLPOperationOnePath >(
        std::move( path ), DLPOperationType::Delete, std::move( unlinkEvent ) );

    auto action = GetActionForOperation( *unlinkOp );
    ExecuteAction( action, std::move( unlinkOp ) );
}