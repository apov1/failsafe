#pragma once

#include "USBControl/Events/UCDeviceInfo.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Specifies the type of the UCDeviceEvent
 */
typedef enum {
    DM_ADD,
    DM_REMOVE,
} DMUCDeviceEventType;

/**
 * @brief Struct containing information about the event.
 *
 * sysPath is a string containing system path of the device, on which the event happened.
 *
 * eventType is representing a type of event.
 *
 * device is a pointer to UCDeviceInfo about the device, on which the event happened. This
 * UCDeviceInfo can be retained, to extend its lifetime after the destruction of the event.
 * This field is NULL in case of DM_REMOVE eventType.
 *
 * _internalCtx is the internal context of the event and should not be accessed directly.
 */
typedef struct {
    const char *sysPath;
    DMUCDeviceEventType eventType;
    UCDeviceInfo *device;
    void *_internalCtx;
} UCDeviceEvent;

/**
 * @brief Frees the UCDeviceEvent.
 *
 * @param event UCDeviceEvent to free. If NULL, this function is a noop.
 */
void UCDeviceEventFree( UCDeviceEvent *event );

#ifdef __cplusplus
}
#endif