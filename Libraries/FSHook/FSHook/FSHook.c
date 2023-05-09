#include "FSHook.h"

#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>

#include "Common/NetlinkFamily.h"
#include "NetlinkParser.h"

static void *ProcessingRoutine( void *connector )
{
    FSHookConnector *casted = ( FSHookConnector * )connector;

    pthread_mutex_lock( &casted->startedMtx );
    do {
        pthread_cond_wait( &casted->startedCv, &casted->startedMtx );
    } while ( casted->familyId == -1 && !casted->ending );
    pthread_mutex_unlock( &casted->startedMtx );

    while ( true ) {
        pthread_mutex_lock( &casted->startedMtx );
        bool shouldExit = casted->ending;
        pthread_mutex_unlock( &casted->startedMtx );

        if ( shouldExit )
            break;

        int rc = nl_recvmsgs_default( ( ( FSHookConnector * )connector )->socket );
        if ( rc != 0 )
            break;
    }

    return NULL;
}

static int nlCallback( struct nl_msg *recvMsg, void *arg )
{
    FSHookConnector *connector = ( FSHookConnector * )arg;

    FSHookMessage *msg = ( FSHookMessage * )malloc( sizeof( FSHookMessage ) );
    int rc = ParseNetlinkMessage( msg, recvMsg );
    if ( rc != 0 )
        return NL_OK;

    connector->processingCallback( connector, msg, connector->data );

    return NL_OK;
}

static int nlErrorCallback( struct sockaddr_nl *nla, struct nlmsgerr *nlerr, void *arg )
{
    ( void )nla;
    ( void )nlerr;
    ( void )arg;

    return NL_OK;
}

int FSHookConnectorInit( FSHookConnector *connector, FSHookEventHandlerBlock handler, void *data )
{
    // Handle invalid input
    if ( connector == NULL )
        return 1;

    if ( handler == NULL )
        return 1;

    // Allocate the netlink socket and fill struct data
    struct nl_sock *socket = nl_socket_alloc();
    connector->familyId = -1;
    connector->socket = socket;
    connector->processingCallback = handler;
    connector->data = data;
    connector->ending = false;
    pthread_mutex_init( &connector->startedMtx, NULL );
    pthread_cond_init( &connector->startedCv, NULL );

    int rc = nl_socket_modify_cb( socket, NL_CB_MSG_IN, NL_CB_CUSTOM, nlCallback, connector );
    if ( rc < 0 )
        goto bail;

    rc = nl_socket_modify_err_cb( socket, NL_CB_CUSTOM, nlErrorCallback, connector );
    if ( rc < 0 )
        goto bail;

    nl_socket_disable_auto_ack( socket );

    // Create thread for the callback
    rc = pthread_create( &connector->processingThread, NULL, ProcessingRoutine, connector );
    if ( rc != 0 )
        goto bail;

    return 0;

bail:
    nl_socket_free( socket );
    return 1;
}

int FSHookRegister( FSHookConnector *connector )
{
    // Handle invalid input
    if ( connector == NULL )
        return 1;

    // Connect the socket resolve its family
    genl_connect( connector->socket );

    pthread_mutex_lock( &connector->startedMtx );
    connector->familyId = genl_ctrl_resolve( connector->socket, FAMILY_NAME );

    if ( connector->familyId < 0 ) {
        pthread_cond_signal( &connector->startedCv );
        pthread_mutex_unlock( &connector->startedMtx );
        return 1;
    }
    pthread_cond_signal( &connector->startedCv );
    pthread_mutex_unlock( &connector->startedMtx );

    // Send hello message
    struct nl_msg *msg = nlmsg_alloc();
    genlmsg_put( msg, NL_AUTO_PORT, NL_AUTO_SEQ, connector->familyId, 0, 0, FSH_NL_C_REGISTER, 1 );

    int rc = nl_send_auto( connector->socket, msg );
    nlmsg_free( msg );

    if ( rc < 0 )
        return rc;
    return 0;
}

void FSHookConnectorFree( FSHookConnector *connector )
{
    // Handle invalid input
    if ( connector == NULL )
        return;

    pthread_mutex_lock( &connector->startedMtx );
    connector->ending = true;
    pthread_mutex_unlock( &connector->startedMtx );
    pthread_cond_signal( &connector->startedCv );

    // Free the allocated resources
    pthread_join( connector->processingThread, NULL );
    nl_socket_free( connector->socket );
    pthread_cond_destroy( &connector->startedCv );
    pthread_mutex_destroy( &connector->startedMtx );
}

int FSHookReplyToEvent( FSHookConnector *connector, FSHookMessage *event, bool response )
{
    // Create netlink message
    struct nl_msg *msg = nlmsg_alloc();
    genlmsg_put( msg, NL_AUTO_PORT, NL_AUTO_SEQ, connector->familyId, 0, 0, FSH_NL_C_REPLY, 1 );

    // Fill it
    nla_put_u32( msg, FSH_NL_A_TL_ID, event->id );
    nla_put_u8( msg, FSH_NL_A_TL_ALLOW, response ? 1 : 0 );

    // Send it
    int rc = nl_send_auto( connector->socket, msg );
    nlmsg_free( msg );
    return rc;
}

void FSHookFreeMessage( FSHookMessage *event )
{
    // Handle invalid input
    if ( event == NULL )
        return;

    // Free allocated resources
    free( event->process.executablePath );
    switch ( event->eventType ) {
    case FSH_EVENT_OPEN:
        free( event->event.open.path );
        break;
    case FSH_EVENT_RENAME:
        free( event->event.rename.sourcePath );
        free( event->event.rename.destPath );
        break;
    case FSH_EVENT_UNLINK:
        free( event->event.unlink.path );
        break;
    }
    free( event );
}