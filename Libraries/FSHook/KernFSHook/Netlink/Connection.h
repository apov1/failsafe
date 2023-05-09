#pragma once

#include <net/genetlink.h>

#include "Netlink/NLMsg.h"

void NetlinkInitConnection( void ( *OnRegisterModuleCallback )( void ) );
void NetlinkUninitConnection( void );

int InitMessageHeaders( FSHookNLMsg *msg );
int NetlinkSendMessage( FSHookNLMsg *msg );
