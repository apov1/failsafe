#pragma once

#include "USBControl/Events/UCDeviceInfo.h"
#include <libudev.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Struct representing internal state of the UCDeviceEnumerator.
 * The fields SHOULD NOT be accesed by the user and can be changed at any time.
 */
typedef struct {
    struct udev *udevCtx;
    struct udev_enumerate *enumerateCtx;
    struct udev_list_entry *entryList;
} UCDeviceEnumerateCtx;

/**
 * @brief Initializes allocated instance of UCDeviceEnumerateCtx.
 *
 * @param ctx Previously allocated UCDeviceEnumerateCtx to inizialize. If NULL, the function fails.
 * @return int 0 on success, any positive value on failure.
 */
int UCDeviceEnumerateInit( UCDeviceEnumerateCtx *ctx );

/**
 * @brief Frees previously initialized UCDeviceEnumerateCtx.
 *
 * @param ctx UCDeviceEnumerateCtx to free. If NULL, the function is a noop.
 */
void UCDeviceEnumerateFree( UCDeviceEnumerateCtx *ctx );

/**
 * @brief Returns next device in the enumeration.
 *
 * @param ctx Context of the enumeration from which next device should be retrieved. If NULL, the
 * function fails.
 * @return UCDeviceInfo* If enumeration is not finished, the function returns valid pointer to
 * UCDeviceInfo with ref count of 1. If the enumeration is finished or the function fails, NULL is
 * returned.
 */
UCDeviceInfo *UCDeviceEnumerateGetNext( UCDeviceEnumerateCtx *ctx );

#ifdef __cplusplus
}
#endif