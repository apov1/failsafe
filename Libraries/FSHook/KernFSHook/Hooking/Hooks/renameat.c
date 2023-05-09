#include "Hooking/Hooks/renameat.h"

#include "Hooking/Hooks/Utils.h"
#include "Hooking/ProcessInfo.h"
#include "Hooking/ResponseWaitMap.h"
#include "Hooking/Variables.h"
#include "Netlink/Connection.h"
#include "Netlink/MessageFactory.h"

asmlinkage int ( *OriginalRenameat )( int olddfd, struct filename *from, int newdfd,
                                      struct filename *to, unsigned int flags );

asmlinkage int HookedRenameat( int olddfd, struct filename *from, int newdfd, struct filename *to,
                               unsigned int flags )
{
    int rc;
    char *rawSourcePathBuffer;
    char *resolvedSourcePath;

    char *rawDestPathBuffer;
    char *resolvedDestPath;

    ProcessInfo process;
    WaitContext *waitCtx;
    FSHookNLMsg msg;

    GetProcessInfo( &process );
    if ( ShouldFilterProcess( &process ) || process.pid == g_UserPid )
        goto bail;

    rawSourcePathBuffer = kmalloc( sizeof( char ) * PATH_MAX, GFP_KERNEL );
    if ( unlikely( rawSourcePathBuffer == NULL ) )
        goto bail;

    resolvedSourcePath = ResolvePath( olddfd, from->uptr, rawSourcePathBuffer, PATH_MAX );
    if ( resolvedSourcePath == NULL )
        goto bail1;

    rawDestPathBuffer = kmalloc( sizeof( char ) * PATH_MAX, GFP_KERNEL );
    if ( unlikely( rawDestPathBuffer == NULL ) )
        goto bail1;

    resolvedDestPath = ResolvePath( newdfd, to->uptr, rawDestPathBuffer, PATH_MAX );
    if ( resolvedDestPath == NULL )
        goto bail2;

    if ( ShouldFilterPath( resolvedSourcePath ) && ShouldFilterPath( resolvedDestPath ) )
        goto bail2;

    waitCtx = InitWaitContext();
    if ( unlikely( waitCtx == NULL ) )
        goto bail2;

    rc = CreateRenameEventMsg( &msg, waitCtx->id, &process, resolvedSourcePath, resolvedDestPath );
    if ( rc != 0 )
        goto bail2;

    rc = NetlinkSendMessage( &msg );
    if ( rc != 0 )
        goto bail2;

    rc = WaitForResponse( waitCtx );
    if ( rc == 0 ) {
        FreeProcessInfo( &process );
        OurPutname( from );
        OurPutname( to );
        kfree( rawDestPathBuffer );
        kfree( rawSourcePathBuffer );
        return -EACCES;
    }

bail2:
    kfree( rawDestPathBuffer );
bail1:
    kfree( rawSourcePathBuffer );
bail:
    FreeProcessInfo( &process );
    return OriginalRenameat( olddfd, from, newdfd, to, flags );
}