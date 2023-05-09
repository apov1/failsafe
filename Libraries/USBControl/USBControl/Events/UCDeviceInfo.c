#include "USBControl/Events/UCDeviceInfo.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void UCDeviceInfoRetain( UCDeviceInfo *device )
{
    // Increase the internal refcount and the udev refcount
    ++( device->_refCount );
    udev_device_ref( device->_internalCtx );
}

void UCDeviceInfoRelease( UCDeviceInfo *device )
{
    // Decrease the internal refcount and the udev refcount
    // If we are at zero, free everything
    udev_device_unref( device->_internalCtx );
    if ( --( device->_refCount ) < 1 ) {
        free( device->serial );
        free( device );
    }
}

void UCDeviceInfoSetBlocked( UCDeviceInfo *device, int authorized )
{
    if ( authorized != 0 && authorized != 1 )
        return;

    // Write the blocked value to sysfs
    char value[ 2 ] = { 0 };
    snprintf( value, 2, "%d", authorized );

    udev_device_set_sysattr_value( device->_internalCtx, "authorized", value );
}

UCDeviceInfo *UCDeviceInfoTryGetUSBParentInfo( UCDeviceInfo *child )
{
    struct udev_device *parent = udev_device_get_parent_with_subsystem_devtype(
        ( struct udev_device * )child->_internalCtx, "usb", "usb_device" );
    if ( parent == NULL )
        return NULL;

    return _UCConstructDeviceInfo( parent );
}

UCDeviceInfo *UCDeviceInfoFromSysPath( const char *sysPath )
{
    struct udev *ctx = udev_new();
    UCDeviceInfo *device = _UCDeviceInfoFromSysPathWithContext( ctx, sysPath );
    udev_unref( ctx );

    return device;
}

bool _UCGetAttributeUint8( struct udev_device *dev, const char *attribute, uint8_t *out )
{
    const char *attr = udev_device_get_sysattr_value( dev, attribute );
    if ( attr == NULL )
        return false;

    uintmax_t val = strtoumax( attr, NULL, 10 );
    if ( val > UINT8_MAX )
        return false;

    *out = val;
    return true;
}

bool _UCGetAttributeUint16( struct udev_device *dev, const char *attribute, uint16_t *out )
{
    const char *attr = udev_device_get_sysattr_value( dev, attribute );
    if ( attr == NULL )
        return false;

    uintmax_t val = strtoumax( attr, NULL, 10 );
    if ( val > UINT16_MAX )
        return false;

    *out = val;
    return true;
}

bool _UCGetAttributeString( struct udev_device *dev, const char *attribute, char *out )
{
    const char *attr = udev_device_get_sysattr_value( dev, attribute );
    if ( attr == NULL )
        return false;

    strcpy( out, attr );
    return true;
}

bool _UCGetAttributeDevtype( struct udev_device *dev, UCDeviceType *out )
{
    const char *devType = udev_device_get_devtype( dev );
    if ( devType == NULL )
        return false;

    if ( strcmp( devType, "usb_device" ) == 0 ) {
        *out = USB_DEVICE;
        return true;
    }

    return false;
}

UCDeviceInfo *_UCConstructDeviceInfo( struct udev_device *dev )
{
    UCDeviceInfo *info = malloc( sizeof( UCDeviceInfo ) );
    if ( info == NULL )
        return NULL;

    info->_refCount = 1;
    info->_internalCtx = dev;
    udev_device_ref( dev );
    info->sysPath = udev_device_get_syspath( dev );
    if ( info->sysPath == NULL )
        goto bail;

    if ( !_UCGetAttributeUint8( dev, "bDeviceClass", &info->deviceClass ) )
        info->deviceClass = 0;

    if ( !_UCGetAttributeUint8( dev, "bDeviceSubClass", &info->deviceSubclass ) )
        info->deviceSubclass = 0;

    if ( !_UCGetAttributeUint8( dev, "bDeviceProtocol", &info->deviceProtocol ) )
        info->deviceProtocol = 0;

    if ( !_UCGetAttributeUint16( dev, "idVendor", &info->vendorId ) )
        info->vendorId = 0;

    if ( !_UCGetAttributeUint16( dev, "idProduct", &info->productId ) )
        info->productId = 0;

    info->serial = calloc( 256, sizeof( char ) );
    if ( !_UCGetAttributeString( dev, "serial", info->serial ) ) {
        free( info->serial );
        info->serial = NULL;
    }

    if ( !_UCGetAttributeDevtype( dev, &info->deviceType ) )
        info->deviceType = UNKNOWN;

    return info;

bail:
    udev_device_unref( dev );
    free( info );
    return NULL;
}

UCDeviceInfo *_UCDeviceInfoFromSysPathWithContext( struct udev *ctx, const char *sysPath )
{
    struct udev_device *device = udev_device_new_from_syspath( ctx, sysPath );
    if ( device == NULL )
        return NULL;

    UCDeviceInfo *info = _UCConstructDeviceInfo( device );

    udev_device_unref( device );
    return info;
}