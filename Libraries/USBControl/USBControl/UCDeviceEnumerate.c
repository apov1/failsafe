#include "USBControl/UCDeviceEnumerate.h"

#include <stdlib.h>

int UCDeviceEnumerateInit( UCDeviceEnumerateCtx *ctx )
{
    // Handle invalid input
    if ( ctx == NULL )
        goto bail;

    // Initialize udev
    ctx->udevCtx = udev_new();
    if ( ctx->udevCtx == NULL )
        goto bail;

    // Initialize the enumeration
    ctx->enumerateCtx = udev_enumerate_new( ctx->udevCtx );
    if ( ctx->enumerateCtx == NULL )
        goto bail1;

    // Set fitler to USB devices only
    int rc = udev_enumerate_add_match_subsystem( ctx->enumerateCtx, "usb" );
    if ( rc < 0 )
        goto bail2;

    // Scan the enumeration with the specified filter and set the first entry
    rc = udev_enumerate_scan_devices( ctx->enumerateCtx );
    if ( rc < 0 )
        goto bail2;

    ctx->entryList = udev_enumerate_get_list_entry( ctx->enumerateCtx );

    return 0;

bail2:
    udev_enumerate_unref( ctx->enumerateCtx );
bail1:
    udev_unref( ctx->udevCtx );
bail:
    return 1;
}

UCDeviceInfo *UCDeviceEnumerateGetNext( UCDeviceEnumerateCtx *ctx )
{
    // Handle invalid input
    if ( ctx == NULL )
        return NULL;

    if ( ctx->entryList == NULL )
        return NULL;

    UCDeviceInfo *info = NULL;

    // Try get the device and parse it to device info
    // If anything fails, go to next device
    const char *devName = udev_list_entry_get_name( ctx->entryList );
    if ( devName == NULL )
        goto next;

    info = _UCDeviceInfoFromSysPathWithContext( ctx->udevCtx, devName );
    if ( info == NULL )
        goto next;

next:
    ctx->entryList = udev_list_entry_get_next( ctx->entryList );
    if ( info == NULL )
        return UCDeviceEnumerateGetNext( ctx );

    return info;
}

void UCDeviceEnumerateFree( UCDeviceEnumerateCtx *ctx )
{
    // Handle invalid input
    if ( ctx == NULL )
        return;

    // Free resources used by the enumerate context
    udev_enumerate_unref( ctx->enumerateCtx );
    udev_unref( ctx->udevCtx );
}
