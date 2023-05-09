#include "Hooking/Hooks/unlinkat.h"

#include <linux/slab.h>

#include "Hooking/Hooks/Utils.h"
#include "Hooking/ProcessInfo.h"
#include "Hooking/ResponseWaitMap.h"
#include "Hooking/Variables.h"
#include "Netlink/Connection.h"
#include "Netlink/MessageFactory.h"

asmlinkage int ( *OriginalUnlinkat )( int dfd, struct filename *name ) = NULL;

asmlinkage int HookedUnlinkat( int dfd, struct filename *name )
{
    int rc;
    char *rawPathBuffer;
    char *resolvedPath;

    ProcessInfo process;
    WaitContext *waitCtx;
    FSHookNLMsg msg;

    GetProcessInfo( &process );
    if ( ShouldFilterProcess( &process ) || process.pid == g_UserPid )
        goto bail;

    rawPathBuffer = kmalloc( sizeof( char ) * PATH_MAX, GFP_KERNEL );
    if ( unlikely( rawPathBuffer == NULL ) )
        goto bail;

    resolvedPath = ResolvePath( dfd, name->uptr, rawPathBuffer, PATH_MAX );
    if ( resolvedPath == NULL )
        goto bail1;

    if ( ShouldFilterPath( resolvedPath ) )
        goto bail1;

    waitCtx = InitWaitContext();
    if ( unlikely( waitCtx == NULL ) )
        goto bail1;

    rc = CreateUnlinkEventMsg( &msg, waitCtx->id, &process, resolvedPath );
    if ( rc != 0 )
        goto bail1;

    rc = NetlinkSendMessage( &msg );
    if ( rc != 0 )
        goto bail1;

    rc = WaitForResponse( waitCtx );
    if ( rc == 0 ) {
        FreeProcessInfo( &process );
        OurPutname( name );
        kfree( rawPathBuffer );
        return -EACCES;
    }

bail1:
    kfree( rawPathBuffer );
bail:
    FreeProcessInfo( &process );
    return OriginalUnlinkat( dfd, name );
}