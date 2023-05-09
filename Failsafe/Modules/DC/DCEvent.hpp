#pragma once

#include <optional>

#include "Modules/DC/USBDevice.hpp"
#include "USBControl/USBControl.h"

class DCEvent {
public:
    DCEvent( UCDeviceEvent *event )
        : event_( event )
    {
    }

    ~DCEvent()
    {
        UCDeviceEventFree( event_ );
    }

    DMUCDeviceEventType Type() const
    {
        return event_->eventType;
    }

    std::string_view SysPath() const
    {
        return std::string_view{ event_->sysPath };
    }

    std::optional< USBDevice > ConstructDevice() const
    {
        return USBDevice::FromUCDeviceInfo( event_->device );
    }

private:
    UCDeviceEvent *event_;
};