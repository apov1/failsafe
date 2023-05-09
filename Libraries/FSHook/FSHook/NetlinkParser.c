#include "NetlinkParser.h"

#include <netlink/genl/genl.h>

#include "Common/NetlinkFamily.h"

int _ParseTimespec( FSHookMessage *msg, struct nlattr *recvAttr )
{
    if ( recvAttr == NULL )
        return 1;

    struct nlattr *nestedTS[ __FSH_TIMESPEC_MAX ];
    nla_parse_nested( nestedTS, __FSH_TIMESPEC_MAX - 1, recvAttr, NULL );
    if ( nestedTS[ FSH_NL_A_SEC ] == NULL || nestedTS[ FSH_NL_A_NSEC ] == NULL )
        return 1;

    msg->time.tv_sec = nla_get_u64( nestedTS[ FSH_NL_A_SEC ] );
    msg->time.tv_nsec = nla_get_u64( nestedTS[ FSH_NL_A_NSEC ] );

    return 0;
}

int _ParseProcessInfo( FSHookProcessInfo *proc, struct nlattr *recvAttr )
{
    if ( recvAttr == NULL )
        return 1;

    struct nlattr *nestedTS[ __FSH_PROC_MAX ];
    nla_parse_nested( nestedTS, __FSH_PROC_MAX - 1, recvAttr, NULL );

    if ( nestedTS[ FSH_NL_A_EXE ] == NULL )
        return 1;
    const char *exePath = nla_get_string( nestedTS[ FSH_NL_A_EXE ] );
    proc->executablePath = calloc( sizeof( char ), ( strlen( exePath ) + 1 ) );
    strcpy( proc->executablePath, exePath );

    if ( nestedTS[ FSH_NL_A_PID ] == NULL )
        return 1;
    proc->pid = nla_get_s32( nestedTS[ FSH_NL_A_PID ] );

    if ( nestedTS[ FSH_NL_A_TID ] == NULL )
        return 1;
    proc->tid = nla_get_s32( nestedTS[ FSH_NL_A_TID ] );

    if ( nestedTS[ FSH_NL_A_UID ] == NULL )
        return 1;
    proc->uid = nla_get_u32( nestedTS[ FSH_NL_A_UID ] );

    return 0;
}

int _ParseOpenEvent( FSHookEvent *event, struct nlattr *recvAttr )
{
    if ( recvAttr == NULL )
        return 1;

    struct nlattr *nestedTS[ __FSH_OPEN_MAX ];
    nla_parse_nested( nestedTS, __FSH_OPEN_MAX - 1, recvAttr, NULL );

    if ( nestedTS[ FSH_NL_A_OPEN_PATH ] == NULL )
        return 1;
    const char *openPath = nla_get_string( nestedTS[ FSH_NL_A_OPEN_PATH ] );

    event->open.path = calloc( sizeof( char ), ( strlen( openPath ) + 1 ) );
    strcpy( event->open.path, openPath );

    if ( nestedTS[ FSH_NL_A_OPEN_FLAGS ] == NULL )
        return 1;
    event->open.flags = nla_get_s32( nestedTS[ FSH_NL_A_OPEN_FLAGS ] );

    return 0;
}

int _ParseRenameEvent( FSHookEvent *event, struct nlattr *recvAttr )
{
    if ( recvAttr == NULL )
        return 1;

    struct nlattr *nestedTS[ __FSH_RENAME_MAX ];
    nla_parse_nested( nestedTS, __FSH_RENAME_MAX - 1, recvAttr, NULL );

    if ( nestedTS[ FSH_NL_A_RENAME_SRC ] == NULL )
        return 1;
    const char *srcPath = nla_get_string( nestedTS[ FSH_NL_A_RENAME_SRC ] );

    event->rename.sourcePath = calloc( sizeof( char ), ( strlen( srcPath ) + 1 ) );
    strcpy( event->rename.sourcePath, srcPath );

    if ( nestedTS[ FSH_NL_A_RENAME_DST ] == NULL )
        return 1;
    const char *dstPath = nla_get_string( nestedTS[ FSH_NL_A_RENAME_DST ] );

    event->rename.destPath = calloc( sizeof( char ), ( strlen( dstPath ) + 1 ) );
    strcpy( event->rename.destPath, dstPath );

    return 0;
}

int _ParseUnlinkEvent( FSHookEvent *event, struct nlattr *recvAttr )
{
    if ( recvAttr == NULL )
        return 1;

    struct nlattr *nestedTS[ __FSH_UNLINK_MAX ];
    nla_parse_nested( nestedTS, __FSH_UNLINK_MAX - 1, recvAttr, NULL );

    if ( nestedTS[ FSH_NL_A_UNLINK_PATH ] == NULL )
        return 1;
    const char *unlinkPath = nla_get_string( nestedTS[ FSH_NL_A_UNLINK_PATH ] );

    event->unlink.path = calloc( sizeof( char ), ( strlen( unlinkPath ) + 1 ) );
    strcpy( event->unlink.path, unlinkPath );

    return 0;
}

int ParseNetlinkMessage( FSHookMessage *msg, struct nl_msg *recvMsg )
{
    if ( msg == NULL )
        return 1;

    if ( recvMsg == NULL )
        return 1;

    struct nlmsghdr *retHdr = nlmsg_hdr( recvMsg );
    if ( retHdr->nlmsg_type == NLMSG_ERROR )
        return 1;

    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    genlmsg_parse( retHdr, 0, tbMsg, __FSH_NL_A_MAX - 1, NULL );
    if ( tbMsg[ FSH_NL_A_TL_ID ] == NULL )
        return 1;
    msg->id = nla_get_u32( tbMsg[ FSH_NL_A_TL_ID ] );

    int rc = _ParseTimespec( msg, tbMsg[ FSH_NL_A_TL_TIMESPEC ] );
    if ( rc != 0 )
        return 1;

    rc = _ParseProcessInfo( &msg->process, tbMsg[ FSH_NL_A_TL_PROC ] );
    if ( rc != 0 )
        return 1;

    if ( tbMsg[ FSH_NL_A_TL_EVENT_TYPE ] == NULL )
        return 1;
    msg->eventType = nla_get_u32( tbMsg[ FSH_NL_A_TL_EVENT_TYPE ] );

    switch ( msg->eventType ) {
    case FSH_EVENT_OPEN:
        return _ParseOpenEvent( &msg->event, tbMsg[ FSH_NL_A_TL_EVENT ] );
    case FSH_EVENT_RENAME:
        return _ParseRenameEvent( &msg->event, tbMsg[ FSH_NL_A_TL_EVENT ] );
    case FSH_EVENT_UNLINK:
        return _ParseUnlinkEvent( &msg->event, tbMsg[ FSH_NL_A_TL_EVENT ] );
    default:
        return 1;
    }
}
