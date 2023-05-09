#include "Modules/DLP/DLPModule.hpp"

#include "Modules/DLP/FSMessages/FSMessageFactory.hpp"
#include "Modules/DLP/Handlers/ChromeHandler.hpp"
#include "Modules/DLP/Handlers/NautilusHandler.hpp"
#include "Modules/DLP/Handlers/SlackHandler.hpp"

#include <iostream>

#include "FSHook/KModuleHandler.h"
#include "spdlog/spdlog.h"

static void FSEventHandler( FSHookConnector *connector, FSHookMessage *event, void *data )
{
    static_cast< DLPModule * >( data )->HandleFSEvent( connector, event );
}

DLPModule::DLPModule( std::filesystem::path pathToKmod, RuleProvider &ruleProvider,
                      ILogger &logger )
    : ruleProvider_( ruleProvider ),
      logger_( logger ),
      threadPool_( 1 ),
      genericHandler_( ruleProvider_, logger_, pathResolver_ ),
      pathToKmod_( std::move( pathToKmod ) )
{
    spdlog::info( "DLPModule: Started initialization." );
    spdlog::info( "DLPModule: Loading kernel module" );
    UnloadKernelModule( pathToKmod_.stem().c_str() );
    int rc = LoadKernelModule( pathToKmod_.c_str() );
    if ( rc != 0 )
        throw std::runtime_error( "DLPModule: Failed to load FSHook kernel module." );

    rc = FSHookConnectorInit( &fsHookConnector_, &FSEventHandler, this );
    if ( rc != 0 )
        throw std::runtime_error( "DLPModule: Failed to init FSHook." );

    spdlog::info( "DLPModule: Initializing handlers." );

    handlers_[ "nautilus" ] =
        std::make_unique< NautilusHandler >( ruleProvider_, logger_, pathResolver_ );
    handlers_[ "slack" ] =
        std::make_unique< SlackHandler >( ruleProvider_, logger_, pathResolver_ );
    handlers_[ "chrome" ] =
        std::make_unique< ChromeHandler >( ruleProvider_, logger_, pathResolver_ );

    spdlog::info( "DLPModule: Handlers initialized." );

    rc = FSHookRegister( &fsHookConnector_ );
    if ( rc != 0 )
        throw std::runtime_error( "DLPModule: Failed to register FSHook to kernel." );

    spdlog::info( "DLPModule: Initialized." );
}

DLPModule::~DLPModule()
{
    spdlog::info( "DLPModule: Started destroying." );
    FSHookConnectorFree( &fsHookConnector_ );
    UnloadKernelModule( pathToKmod_.stem().c_str() );
    spdlog::info( "DLPModule: Destroyed." );
}

void DLPModule::HandleFSEvent( FSHookConnector *connector, FSHookMessage *event )
{
    auto message = FSMessageFactory::Construct( connector, event );

    threadPool_.QueueTask( [ this, message = std::move( message ) ]() mutable {
        try {
            bool eventHandled = false;

            if ( message->ExecutablePath().starts_with( "/usr/lib/systemd/" ) ||
                 message->ExecutablePath().starts_with( "/usr/libexec/" ) )
                return;

            auto appName = message->ExecutablePath().substr(
                message->ExecutablePath().find_last_of( "/" ) + 1 );
            auto it = handlers_.find( appName );
            if ( it != handlers_.end() )
                eventHandled = it->second->HandleEvent( message );

            if ( !eventHandled )
                genericHandler_.HandleEvent( std::move( message ) );
        }
        catch ( std::exception &e ) {
            spdlog::critical( "Caught exception during event processing {}", e.what() );
        }
    } );
}
