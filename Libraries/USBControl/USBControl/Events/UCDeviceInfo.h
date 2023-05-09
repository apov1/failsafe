#pragma once

#include <libudev.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enum representing the type of the device.
 */
typedef enum {
    USB_DEVICE,
    UNKNOWN,
} UCDeviceType;

/**
 * @brief Struct containing info about the device.
 *
 * deviceClass - class of the device retrieved from sysfs, can be 0 if not present
 * deviceSubclass - subclass of the device retrieved from sysfs, can be 0 if not present
 * deviceProtocol - protocol of the device retrieved from sysfs, can be 0 if not present
 * productId - product id of the device retrieved from sysfs, can be 0 if not present
 * vendorId - vendor id of the device retrieved from sysfs, can be 0 if not present
 * serial - serial string of the device retrieved from sysfs, can be NULL if not present
 * deviceType - type of the device
 * _internalCtx - internal context of the device, should not ve accesed directly
 * _refCount - reference count of the device, should not be accesed directly
 */
typedef struct {
    uint8_t deviceClass;
    uint8_t deviceSubclass;
    uint8_t deviceProtocol;
    uint16_t productId;
    uint16_t vendorId;
    char *serial;
    const char *sysPath;
    UCDeviceType deviceType;
    void *_internalCtx;
    int _refCount;
} UCDeviceInfo;

/**
 * @brief Retains the UCDeviceInfo by increasing its internal reference counter.
 *
 * @param device UCDeviceInfo to retain
 */
void UCDeviceInfoRetain( UCDeviceInfo *device );

/**
 * @brief Releases the UCDeviceInfo by decrementing its internal reference counter.
 * If the reference count reaches zero, the UCDeviceInfo is freed and is no longer valid.
 *
 * @param device UCDeviceInfo to release
 */
void UCDeviceInfoRelease( UCDeviceInfo *device );

/**
 * @brief Some devices can be authorized on the system. This function sets the authorization status
 * of the device from corresponding UCDeviceInfo. If the device is deauthorized, the system drivers
 * can no longer access it and is not visible in the system.
 *
 * @param device UCDeviceInfo to (de)authorize
 * @param authorized 1 if the device should be authorized, 0 if the device should be deauthorized
 */
void UCDeviceInfoSetBlocked( UCDeviceInfo *device, int authorized );

/**
 * @brief Tries to retrieve UCDeviceInfo of the parent device.
 *
 * @param child UCDeviceInfo of the child device, of which parent we are looking for
 * @return UCDeviceInfo* UCDeviceInfo of the parent device on success, NULL on failure
 */
UCDeviceInfo *UCDeviceInfoTryGetUSBParentInfo( UCDeviceInfo *child );

/**
 * @brief Creates UCDeviceInfo of the device on specified system path.
 *
 * @param sysPath System path of the device.
 * @return UCDeviceInfo* UCDeviceInfo of the device on the system path on success, NULL on failure.
 */
UCDeviceInfo *UCDeviceInfoFromSysPath( const char *sysPath );

/**
 * @brief Retrieves the attribute of the specified device of uint8_t type. THIS IS AN INTERNAL
 * FUNCTION AND SHOULD NOT BE USED DIRECTLY. ITS INTERFACE AND BEHAIOUR CAN CHANGE AT ANY TIME.
 *
 * @param dev udev device of which attribute to retireve
 * @param attribute name of attribute
 * @param out out parameter where the attribute is stored
 * @return true on success
 * @return false on failure
 */
bool _UCGetAttributeUint8( struct udev_device *dev, const char *attribute, uint8_t *out );

/**
 * @brief Retrieves the attribute of the specified device of uint16_t type. THIS IS AN INTERNAL
 * FUNCTION AND SHOULD NOT BE USED DIRECTLY. ITS INTERFACE AND BEHAIOUR CAN CHANGE AT ANY TIME.
 *
 * @param dev udev device of which attribute to retireve
 * @param attribute name of attribute
 * @param out out parameter where the attribute is stored
 * @return true on success
 * @return false on failure
 */
bool _UCGetAttributeUint16( struct udev_device *dev, const char *attribute, uint16_t *out );

/**
 * @brief Retrieves the attribute of the specified device of string type. THIS IS AN INTERNAL
 * FUNCTION AND SHOULD NOT BE USED DIRECTLY. ITS INTERFACE AND BEHAIOUR CAN CHANGE AT ANY TIME.
 *
 * @param dev udev device of which attribute to retireve
 * @param attribute name of attribute
 * @param out out parameter where the attribute is stored
 * @return true on success
 * @return false on failure
 */
bool _UCGetAttributeString( struct udev_device *dev, const char *attribute, char *out );

/**
 * @brief Retrieves the device type of the specified udev device. THIS IS AN INTERNAL
 * FUNCTION AND SHOULD NOT BE USED DIRECTLY. ITS INTERFACE AND BEHAIOUR CAN CHANGE AT ANY TIME.
 *
 * @param dev udev device of which type we are looking for
 * @param out out parameter where the type is stored
 * @return true if the device type was resolved successfully and if it is in our enum in known
 * device types
 * @return false otherwise
 */
bool _UCGetAttributeDevtype( struct udev_device *dev, UCDeviceType *out );

/**
 * @brief Constructs UCDeviceInfo from udev device. THIS IS AN INTERNAL
 * FUNCTION AND SHOULD NOT BE USED DIRECTLY. ITS INTERFACE AND BEHAIOUR CAN CHANGE AT ANY TIME.
 *
 * @param dev udev device
 * @return UCDeviceInfo* valid pointer to UCDeviceInfo with reference count of 1 on success. NULL on
 * failure.
 */
UCDeviceInfo *_UCConstructDeviceInfo( struct udev_device *dev );

/**
 * @brief Constructs UCDeviceInfo from system path. THIS IS AN INTERNAL
 * FUNCTION AND SHOULD NOT BE USED DIRECTLY. ITS INTERFACE AND BEHAIOUR CAN CHANGE AT ANY TIME.
 *
 * @param ctx udev context
 * @param sysPath system path of the device
 * @return UCDeviceInfo* valid pointer to UCDeviceInfo with reference count of 1 on success. NULL on
 * failure.
 */
UCDeviceInfo *_UCDeviceInfoFromSysPathWithContext( struct udev *ctx, const char *sysPath );

#ifdef __cplusplus
}
#endif