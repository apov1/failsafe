#include <iostream>
#include <signal.h>

#include "FSHook/FSHook.h"
#include "FSHook/KModuleHandler.h"

int SleepUntilTerminationSignal()
{
    sigset_t sigset;
    sigemptyset( &sigset );
    sigaddset( &sigset, SIGINT );
    sigaddset( &sigset, SIGTERM );
    pthread_sigmask( SIG_BLOCK, &sigset, nullptr );

    int signum = 0;
    sigwait( &sigset, &signum );
    return signum;
}

static void FSEventHandler( FSHookConnector *connector, FSHookMessage *event, void * )
{
    FSHookReplyToEvent( connector, event, 1 );
    FSHookFreeMessage( event );
}

int main()
{
    UnloadKernelModule( "kern_fshook" );
    int rc = LoadKernelModule( "./fshook/KernFSHook/kern_fshook.ko" );
    if ( rc != 0 ) {
        std::cerr << "DLPModule: Failed to load FSHook kernel module." << std::endl;
        goto bail;
    }

    FSHookConnector connector;
    rc = FSHookConnectorInit( &connector, &FSEventHandler, nullptr );
    if ( rc != 0 ) {
        std::cerr << "DLPModule: Failed to init FSHook connector." << std::endl;
        goto bail;
    }

    rc = FSHookRegister( &connector );
    if ( rc != 0 ) {
        std::cerr << "DLPModule: Failed to register FSHook connector." << std::endl;
        goto bail1;
    }

    std::cout << "Running! Press CTRL+C to exit!" << std::endl;
    SleepUntilTerminationSignal();

bail1:
    FSHookConnectorFree( &connector );
bail:
    UnloadKernelModule( "kern_fshook" );
    return rc;
}