#pragma once

/**
 * @brief Event types of the received events from the kernel.
 */
typedef enum {
    FSH_EVENT_OPEN,
    FSH_EVENT_RENAME,
    FSH_EVENT_UNLINK,
} FSHookEventType;