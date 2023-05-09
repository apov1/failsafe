#include "KModuleHandler.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

int LoadKernelModule( const char *path )
{
    int fd = open( path, O_RDONLY );
    if ( !fd )
        return 1;

    pid_t currentPid = getpid();

    char args[ 128 ] = { 0 };
    sprintf( args, "pid=%d", currentPid );

    // this syscall doesn't have a glibc wrapper, we need to call it manually through syscall
    int rc = syscall( __NR_finit_module, fd, args, 0 );
    if ( rc != 0 )
        goto bail;

    return 0;
bail:
    close( fd );
    return 1;
}

int UnloadKernelModule( const char *name )
{
    // this syscall doesn't have a glibc wrapper, we need to call it manually through syscall
    return syscall( __NR_delete_module, name, O_NONBLOCK );
}