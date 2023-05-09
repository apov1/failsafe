#include "Hooking/Hooks/Utils.h"

#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/slab.h>

char *ResolvePath( int dfd, const char __user *openPathRaw, char *buffer, size_t bufferSize )
{
    int rc;
    struct path openPath;
    struct path *pOpenPath = &openPath;

    // try resolve if the path exists
    rc = user_path_at( dfd, openPathRaw, 0, pOpenPath );
    if ( rc != 0 ) {
        rc = strncpy_from_user( buffer, openPathRaw, strlen( openPathRaw ) + 1 );
        return buffer;
    }

    return d_path( pOpenPath, buffer, bufferSize );
}

void OurPutname( struct filename *name )
{
    if ( IS_ERR( name ) )
        return;

    BUG_ON( name->refcnt <= 0 );

    if ( --name->refcnt > 0 )
        return;

    if ( name->name != name->iname ) {
        __putname( name->name );
        kfree( name );
    }
    else
        __putname( name );
}

int ShouldFilterPath( const char *path )
{
    return ( strncmp( path, "/proc", 5 ) == 0 || strncmp( path, "/sys", 4 ) == 0 ||
             strncmp( path, "/dev", 4 ) == 0 );
}

int ShouldFilterProcess( ProcessInfo *process )
{
    return process->pid < 10;
}