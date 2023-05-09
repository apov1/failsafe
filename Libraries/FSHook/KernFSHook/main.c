#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "Hooking/Hooks.h"
#include "Hooking/Variables.h"
#include "Netlink/Connection.h"

MODULE_AUTHOR( "Adam Povazanec" );
MODULE_LICENSE( "GPL" );
MODULE_VERSION( "0.1" );

module_param_named( pid, g_UserPid, int, 0 );

static int __init FSHookInit( void )
{
    NetlinkInitConnection( &FSHookInitAllHooks );
    pr_info( "FSHook loaded successfully!\n" );
    return 0;
}

static void __exit FSHookExit( void )
{
    NetlinkUninitConnection();
    FSHookRemoveAllHooks();
    pr_info( "FSHook unloaded successfully!\n" );
}

module_init( FSHookInit );
module_exit( FSHookExit );
