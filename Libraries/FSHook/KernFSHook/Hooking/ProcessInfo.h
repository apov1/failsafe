#pragma once

#include <linux/types.h>

typedef struct {
    pid_t pid;
    pid_t tid;
    uid_t uid;
    const char *exePath;

    // internal
    char *_pathBuffer;
} ProcessInfo;

int GetProcessInfo( ProcessInfo *info );
void FreeProcessInfo( ProcessInfo *info );