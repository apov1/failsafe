#pragma once

#include <memory>

#include "Modules/DC/USBDevice.hpp"
#include "SharedComponents/RuleProvider/DC/DCOperationType.hpp"

class DCOperation {
public:
    DCOperation( DCOperationType operation, std::shared_ptr< USBDevice > device )
        : operation_( operation ),
          device_( std::move( device ) )
    {
    }

    DCOperationType Operation() const
    {
        return operation_;
    }

    const USBDevice &Device() const noexcept
    {
        return *device_;
    }

    void Allow()
    {
        if ( Operation() == DCOperationType::Remove )
            return;

        device_->SetAuthorization( true );
    }

    void Block()
    {
        if ( Operation() == DCOperationType::Remove )
            return;

        device_->SetAuthorization( false );
    }

    std::string ToString() const
    {
        return device_->ToString();
    }

protected:
    DCOperationType operation_;
    std::shared_ptr< USBDevice > device_;
};