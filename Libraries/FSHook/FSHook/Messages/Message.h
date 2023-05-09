#pragma once

#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "Common/EventType.h"

/**
 * @brief Data corresponding to FSH_EVENT_OPEN
 *
 * path - path of file which is tried to be opened
 * flags - flags passed to the open syscall, for specific flags, consult man open(2)
 */
typedef struct {
    char *path;
    uint32_t flags;
} FSHookEventOpen;

/**
 * @brief Data corresponding to FSH_EVENT_RENAME
 *
 * sourcePath - path of file which is tried be renamed
 * destPath - path to which the file is trying to be renamed to
 */
typedef struct {
    char *sourcePath;
    char *destPath;
} FSHookEventRename;

/**
 * @brief Data corresponding to FSH_EVENT_UNLINK
 *
 * path - path of file which is tried be unlinked
 */
typedef struct {
    char *path;
} FSHookEventUnlink;

typedef union {
    FSHookEventOpen open;
    FSHookEventRename rename;
    FSHookEventUnlink unlink;
} FSHookEvent;

/**
 * @brief Process info
 *
 * pid - process id of the process
 * tid - thread id of thread which caused the event
 * uid - uid of the process which caused the event
 * executablePath - path to executable, which caused the event
 */
typedef struct {
    pid_t pid;
    pid_t tid;
    uid_t uid;
    char *executablePath;
} FSHookProcessInfo;

/**
 * @brief The struct representing message about filesystem event received from the kernel.
 *
 * id - internal identification of the message
 * process - info about the process, which caused the file system event
 * time - time at which the event happened in the form of timespec
 * eventType - type of event, all the available types are listed in enum FSHookEventType
 * event - union of event specific data
 */
typedef struct {
    uint32_t id;
    FSHookProcessInfo process;
    struct timespec time;

    FSHookEventType eventType;
    FSHookEvent event;
} FSHookMessage;