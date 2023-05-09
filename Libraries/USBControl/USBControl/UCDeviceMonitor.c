#include "USBControl/UCDeviceMonitor.h"

#include <libudev.h>
#include <poll.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool ConstructAddEvent( struct udev_device *dev, UCDeviceEvent *event )
{
    event->eventType = DM_ADD;

    event->sysPath = udev_device_get_syspath( dev );
    if ( event->sysPath == NULL )
        return false;

    event->device = _UCConstructDeviceInfo( dev );
    if ( event->device == NULL )
        return false;

    return true;
}

static bool ConstructRemoveEvent( struct udev_device *dev, UCDeviceEvent *event )
{
    event->eventType = DM_REMOVE;
    event->device = NULL;
    event->sysPath = udev_device_get_syspath( dev );

    return ( event->sysPath != NULL );
}

static bool ProcessUCDeviceEvent( struct udev_device *dev, UCDeviceEvent *event )
{
    event->_internalCtx = dev;

    const char *action = udev_device_get_action( dev );
    if ( action == NULL )
        return false;

    if ( strcmp( action, "add" ) == 0 )
        return ConstructAddEvent( dev, event );
    else if ( strcmp( action, "remove" ) == 0 )
        return ConstructRemoveEvent( dev, event );
    else // we are not interested in any other event
        return false;
}

static void *ProcessingRoutine( void *monitor )
{
    UCDeviceMonitor *dm = ( UCDeviceMonitor * )monitor;

    // We will be polling the udev monitor file descriptor, this way the thread sleeps
    // when no events are to be processed
    struct pollfd items[ 1 ];
    items[ 0 ].fd = udev_monitor_get_fd( dm->udevMonitor );
    items[ 0 ].events = POLLIN;
    items[ 0 ].revents = 0;

    while ( poll( items, 1, -1 ) > 0 ) {
        struct udev_device *dev = udev_monitor_receive_device( dm->udevMonitor );

        // Allocate and process event
        UCDeviceEvent *event = ( UCDeviceEvent * )malloc( sizeof( UCDeviceEvent ) );
        if ( event == NULL ) {
            udev_device_unref( dev );
            continue;
        }

        if ( !ProcessUCDeviceEvent( dev, event ) ) {
            udev_device_unref( dev );
            free( event );
            continue;
        }

        // Everything was successfull, pass the event to the user callback
        dm->processingCallback( dm, event, dm->userData );
    }

    return NULL;
}

int UCDeviceMonitorInit( UCDeviceMonitor *monitor, UCDeviceMonitorEventCallback handler,
                         void *data )
{
    // Handle invalid input
    if ( monitor == NULL )
        return 1;

    if ( handler == NULL )
        return 1;

    // Populate monitor
    monitor->processingCallback = handler;
    monitor->userData = data;
    monitor->processingThread = 0;

    // Initialize udev
    monitor->udevCtx = udev_new();
    if ( monitor->udevCtx == NULL )
        return 1;

    monitor->udevMonitor = udev_monitor_new_from_netlink( monitor->udevCtx, "udev" );
    if ( monitor->udevMonitor == NULL )
        goto bail1;

    // We are only interested in events from usb udev subsystem
    int rc = udev_monitor_filter_add_match_subsystem_devtype( monitor->udevMonitor, "usb", NULL );
    if ( rc < 0 )
        goto bail2;

    return 0;

bail2:
    udev_monitor_unref( monitor->udevMonitor );
bail1:
    udev_unref( monitor->udevCtx );
    return 1;
}

int UCDeviceMonitorStart( UCDeviceMonitor *monitor )
{
    // Handle invalid input
    if ( monitor == NULL )
        return 1;

    // Create thread in which the callback will be called
    int rc = pthread_create( &monitor->processingThread, NULL, ProcessingRoutine, monitor );
    if ( rc != 0 )
        return 1;

    // Enable the udev monitor
    rc = udev_monitor_enable_receiving( monitor->udevMonitor );
    if ( rc < 0 )
        return 1;

    return 0;
}

void UCDeviceMonitorFree( UCDeviceMonitor *monitor )
{
    // Handle invalid input
    if ( monitor == NULL )
        return;

    // Release all the resources of the monitor
    if ( monitor->processingThread != 0 )
        pthread_cancel( monitor->processingThread );

    udev_monitor_unref( monitor->udevMonitor );
    udev_unref( monitor->udevCtx );
    pthread_join( monitor->processingThread, NULL );
}
