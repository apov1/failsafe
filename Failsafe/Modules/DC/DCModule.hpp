#pragma once

#include "Modules/DC/DCEvent.hpp"
#include "Modules/DC/Operations/DCOperation.hpp"
#include "Modules/DC/USBDevice.hpp"
#include "SharedComponents/IncidentLogger/ILogger.hpp"
#include "SharedComponents/RuleProvider/RuleProvider.hpp"
#include "USBControl/USBControl.h"
#include "Utils/ThreadPool.hpp"

class DCModule {
public:
    DCModule( RuleProvider &ruleProvider, ILogger &logger );
    ~DCModule();

private:
    friend void HandleDevice( UCDeviceMonitor *monitor, UCDeviceEvent *dev, void *data );
    void HandleUCDeviceEvent( UCDeviceMonitor *monitor, UCDeviceEvent *dev );
    void InitializationPhase();

    void HandleAddEvent( const DCEvent &event );
    void HandleRemoveEvent( const DCEvent &event );

    ActionType GetActionForOperation( const DCOperation &op ) const;
    void ExecuteAction( ActionType action, std::shared_ptr< DCOperation > op );

    RuleProvider &ruleProvider_;
    ILogger &logger_;
    ThreadPool threadPool_;
    UCDeviceMonitor monitor_;

    std::unordered_map< std::string, std::shared_ptr< USBDevice > > deviceCache_;
};