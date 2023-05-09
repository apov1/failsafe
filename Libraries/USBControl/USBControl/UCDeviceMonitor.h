#pragma once

#include <libudev.h>
#include <pthread.h>
#include <stdint.h>

#include "USBControl/Events/UCDeviceEvent.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UCDeviceMonitorStruct UCDeviceMonitor;

/**
 * @brief Type of callback to be called at every device event.
 */
typedef void ( *UCDeviceMonitorEventCallback )( UCDeviceMonitor *monitor, UCDeviceEvent *event,
                                                void *data );
/**
 * @brief Struct representing internal state of the UCDeviceMonitor.
 * The fields SHOULD NOT be accesed by the user and can be changed at any time.
 */
struct UCDeviceMonitorStruct {
    struct udev *udevCtx;
    struct udev_monitor *udevMonitor;
    pthread_t processingThread;
    UCDeviceMonitorEventCallback processingCallback;
    void *userData;
};

/**
 * @brief Initializes allocated instance of UCDeviceMonitor.
 *
 * @param monitor pointer to instance of UCDeviceMonitor to initialize. If it is NULL, the function
 * fails.
 * @param handler handler to call for each device event. If it is NULL, the function fails.
 * @param data custom user data passed to handler on each call. Can be NULL.
 * @return int 0 on success, positive value on failure
 */
int UCDeviceMonitorInit( UCDeviceMonitor *monitor, UCDeviceMonitorEventCallback handler,
                         void *data );

/**
 * @brief Starts the device monitor. After successfull call to this function, handler specified in
 * the call to UCDeviceMonitorInit will begin to be called.
 *
 * @param monitor Initialized instance of UCDeviceMonitor. If it is NULL, the function fails.
 * @return int 0 on success, positive value on failure
 */
int UCDeviceMonitorStart( UCDeviceMonitor *monitor );

/**
 * @brief Frees the previously initialized UCDeviceMonitor.
 *
 * @param monitor context to free. If it is NULL, the function is a noop.
 */
void UCDeviceMonitorFree( UCDeviceMonitor *monitor );

#ifdef __cplusplus
}
#endif