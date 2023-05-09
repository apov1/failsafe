#include "USBControl/Events/UCDeviceEvent.h"

#include <libudev.h>
#include <stdlib.h>

void UCDeviceEventFree( UCDeviceEvent *event )
{
    udev_device_unref( event->_internalCtx );

    if ( event->device != NULL )
        UCDeviceInfoRelease( event->device );

    free( event );
}