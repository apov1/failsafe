#pragma once

#include <netlink/attr.h>
#include <netlink/netlink.h>

#include "Messages/Message.h"

#ifdef __cplusplus
extern "C" {
#endif

int ParseNetlinkMessage( FSHookMessage *msg, struct nl_msg *recv_msg );

int _ParseOpenEvent( FSHookEvent *event, struct nlattr *recvAttr );
int _ParseRenameEvent( FSHookEvent *event, struct nlattr *recvAttr );
int _ParseUnlinkEvent( FSHookEvent *event, struct nlattr *recvAttr );

int _ParseProcessInfo( FSHookProcessInfo *proc, struct nlattr *recvAttr );
int _ParseTimespec( FSHookMessage *msg, struct nlattr *recvAttr );

#ifdef __cplusplus
}
#endif