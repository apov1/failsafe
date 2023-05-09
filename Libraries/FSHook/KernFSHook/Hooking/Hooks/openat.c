#include "Hooking/Hooks/openat.h"

#include <linux/fdtable.h>

#include "Hooking/Hooks/Utils.h"
#include "Hooking/ProcessInfo.h"
#include "Hooking/ResponseWaitMap.h"
#include "Hooking/Variables.h"
#include "Netlink/Connection.h"
#include "Netlink/MessageFactory.h"

asmlinkage long ( *OriginalOpenat )( int dfd, const char __user *filename,
                                     struct open_how *how ) = NULL;

asmlinkage long HookedOpenat( int dfd, const char __user *filename, struct open_how *how )
{
    int flags = how->flags;

    int rc;
    ProcessInfo process;
    char *rawPathBuffer = NULL;
    char *resolvedPath = NULL;

    WaitContext *waitCtx;
    FSHookNLMsg msg;

    GetProcessInfo( &process );
    if ( ShouldFilterProcess( &process ) || process.pid == g_UserPid )
        goto bail;

    rawPathBuffer = kmalloc( sizeof( char ) * PATH_MAX, GFP_KERNEL );
    if ( unlikely( rawPathBuffer == NULL ) )
        goto bail;

    resolvedPath = ResolvePath( dfd, filename, rawPathBuffer, PATH_MAX );
    if ( resolvedPath == NULL )
        goto bail1;

    if ( ShouldFilterPath( resolvedPath ) )
        goto bail1;

    waitCtx = InitWaitContext();
    if ( unlikely( waitCtx == NULL ) )
        goto bail1;

    rc = CreateOpenEventMsg( &msg, waitCtx->id, &process, resolvedPath, flags );
    if ( rc != 0 )
        goto bail1;

    rc = NetlinkSendMessage( &msg );
    if ( rc != 0 )
        goto bail1;

    rc = WaitForResponse( waitCtx );
    if ( rc == 0 ) {
        kfree( rawPathBuffer );
        FreeProcessInfo( &process );
        return -EACCES;
    }

bail1:
    kfree( rawPathBuffer );
bail:
    FreeProcessInfo( &process );
    return OriginalOpenat( dfd, filename, how );
}