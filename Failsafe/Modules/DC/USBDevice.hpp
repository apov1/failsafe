#pragma once

#include "USBControl/USBControl.h"

#include <optional>
#include <sstream>
#include <string>

class USBDevice {
public:
    static std::optional< USBDevice > FromUCDeviceInfo( UCDeviceInfo *info )
    {
        if ( info == nullptr )
            return std::nullopt;
        if ( info->deviceType != USB_DEVICE )
            return std::nullopt;

        return std::make_optional< USBDevice >( info );
    }

    USBDevice( const USBDevice & ) = delete;
    USBDevice( USBDevice &&o ) noexcept
    {
        device_ = o.device_;
        o.device_ = nullptr;
    }

    USBDevice &operator=( const USBDevice & ) = delete;
    USBDevice &operator=( USBDevice &&o )
    {

        device_ = o.device_;
        o.device_ = nullptr;

        return *this;
    }

    ~USBDevice()
    {
        if ( device_ != nullptr )
            UCDeviceInfoRelease( device_ );
    }

    void SetAuthorization( bool authorized ) noexcept
    {
        UCDeviceInfoSetBlocked( device_, static_cast< int >( authorized ) );
    }

    uint16_t ProductID() const noexcept
    {
        if ( device_ == nullptr )
            return 0;

        return device_->productId;
    }

    uint16_t VendorID() const noexcept
    {
        if ( device_ == nullptr )
            return 0;

        return device_->vendorId;
    }

    uint8_t DeviceClass() const noexcept
    {
        if ( device_ == nullptr )
            return 0;

        return device_->deviceClass;
    }

    uint8_t DeviceSubclass() const noexcept
    {
        if ( device_ == nullptr )
            return 0;

        return device_->deviceSubclass;
    }

    uint8_t DeviceProtocol() const noexcept
    {
        if ( device_ == nullptr )
            return 0;

        return device_->deviceProtocol;
    }

    std::string_view Serial() const noexcept
    {
        if ( device_ == nullptr || device_->serial == nullptr )
            return "";

        return std::string_view{ device_->serial };
    }

    std::string_view SysPath() const
    {
        if ( device_ == nullptr || device_->sysPath == nullptr )
            return "";

        return std::string_view{ device_->sysPath };
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << "Device (ProductID: " << ProductID() << ", VendorID: " << VendorID()
           << ", Serial: " << Serial() << ")";
        return ss.str();
    }

private:
    USBDevice( UCDeviceInfo *device )
        : device_( device )
    {
        UCDeviceInfoRetain( device_ );
    }

    UCDeviceInfo *device_;
};