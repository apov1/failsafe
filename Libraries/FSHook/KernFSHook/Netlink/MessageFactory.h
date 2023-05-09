#pragma once

#include <linux/types.h>

#include "Hooking/ProcessInfo.h"
#include "Netlink/NLMsg.h"

int CreateOpenEventMsg( FSHookNLMsg *msg, uint32_t id, ProcessInfo *process, const char *path,
                        int flags );

int CreateRenameEventMsg( FSHookNLMsg *msg, uint32_t id, ProcessInfo *process,
                          const char *sourcePath, const char *destPath );

int CreateUnlinkEventMsg( FSHookNLMsg *msg, uint32_t id, ProcessInfo *process, const char *path );
