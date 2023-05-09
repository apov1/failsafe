#include "Modules/DC/DCModule.hpp"

#include "spdlog/spdlog.h"

void HandleDevice( UCDeviceMonitor *monitor, UCDeviceEvent *dev, void *data )
{
    static_cast< DCModule * >( data )->HandleUCDeviceEvent( monitor, dev );
}

DCModule::DCModule( RuleProvider &ruleProvider, ILogger &logger )
    : ruleProvider_( ruleProvider ),
      logger_( logger ),
      threadPool_( 1 )
{
    spdlog::info( "DCModule: Starting initialization." );

    InitializationPhase();

    int rc = UCDeviceMonitorInit( &monitor_, HandleDevice, this );
    if ( rc != 0 )
        throw std::runtime_error( "UCDeviceMonitor initialization failed." );

    rc = UCDeviceMonitorStart( &monitor_ );
    if ( rc != 0 )
        throw std::runtime_error( "UCDeviceMonitor start failed." );

    spdlog::info( "DCModule: Initialized!" );
}

DCModule::~DCModule()
{
    spdlog::info( "DCModule: Started destroying." );
    spdlog::info( "DCModule: Reenabling all devices." );

    for ( auto &[ _, device ] : deviceCache_ )
        device->SetAuthorization( true );

    UCDeviceMonitorFree( &monitor_ );
}

void DCModule::InitializationPhase()
{
    spdlog::info( "DCModule: Starting device enumeration." );

    UCDeviceEnumerateCtx enumeration;
    int rc = UCDeviceEnumerateInit( &enumeration );
    if ( rc != 0 )
        throw std::runtime_error( "Device enumratation init failed." );

    UCDeviceInfo *info;
    while ( ( info = UCDeviceEnumerateGetNext( &enumeration ) ) != NULL ) {

        auto device = USBDevice::FromUCDeviceInfo( info );
        UCDeviceInfoRelease( info );
        if ( !device.has_value() )
            continue;

        auto devicePtr = std::make_shared< USBDevice >( std::move( device.value() ) );
        std::string sysPath = std::string( device->SysPath() );

        auto operation = std::make_shared< DCOperation >( DCOperationType::Add, devicePtr );
        auto action = GetActionForOperation( *operation );
        ExecuteAction( action, operation );

        spdlog::info( "Adding device at sysPath {} : {} to cache", sysPath, device->ToString() );
        deviceCache_.emplace( sysPath, std::move( devicePtr ) );
    }

    UCDeviceEnumerateFree( &enumeration );
    spdlog::info( "DCModule: Enumeration finished." );
}

void DCModule::HandleUCDeviceEvent( UCDeviceMonitor * /*monitor*/, UCDeviceEvent *dev )
{
    threadPool_.QueueTask( [ this, dev ]() {
        try {
            auto event = DCEvent( dev );

            switch ( event.Type() ) {
            case DM_ADD: {
                HandleAddEvent( event );
            } break;
            case DM_REMOVE: {
                HandleRemoveEvent( event );
            } break;
            }
        }
        catch ( std::exception &e ) {
            spdlog::critical( "Uncaught exception {}", e.what() );
        }
    } );
}

void DCModule::HandleAddEvent( const DCEvent &event )
{
    auto device = event.ConstructDevice();
    if ( !device.has_value() )
        return;

    auto devicePtr = std::make_shared< USBDevice >( std::move( device.value() ) );
    auto operation = std::make_shared< DCOperation >( DCOperationType::Add, devicePtr );

    spdlog::info( "Received add device event {}", operation->ToString() );

    auto action = GetActionForOperation( *operation );
    ExecuteAction( action, operation );

    deviceCache_.emplace(
        std::make_pair( std::string( event.SysPath() ), std::move( devicePtr ) ) );
}

void DCModule::HandleRemoveEvent( const DCEvent &event )
{
    auto it = deviceCache_.find( std::string( event.SysPath() ) );
    if ( it == deviceCache_.end() )
        return;

    auto operation = std::make_shared< DCOperation >( DCOperationType::Remove, it->second );
    spdlog::info( "Received remove device event {}", operation->ToString() );

    auto action = GetActionForOperation( *operation );
    ExecuteAction( action, operation );

    deviceCache_.erase( it );
}

ActionType DCModule::GetActionForOperation( const DCOperation &op ) const
{
    auto rule = ruleProvider_.GetRule( op );

    if ( rule.has_value() ) {
        spdlog::info( "Operation matched rule {}", rule->ToString() );
        return rule->Action();
    }

    return ruleProvider_.DefaultAction();
}

void DCModule::ExecuteAction( ActionType action, std::shared_ptr< DCOperation > op )
{
    if ( action == ActionType::Block ) {
        spdlog::info( "Blocking and logging the device incident!" );
        op->Block();
        logger_.SaveLog( action, std::move( op ) );
    }
    else if ( action == ActionType::Log ) {
        spdlog::info( "Logging the device incident!" );
        logger_.SaveLog( action, std::move( op ) );
    }
}