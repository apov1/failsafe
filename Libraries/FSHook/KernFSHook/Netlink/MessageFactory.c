#include "Netlink/MessageFactory.h"

#include <net/genetlink.h>

#include "Netlink/Connection.h"

#include "Common/EventType.h"
#include "Common/NetlinkFamily.h"

static int InitMessage( FSHookNLMsg *msg, uint32_t id, ProcessInfo *process )
{
    struct timespec64 ts;
    struct nlattr *nestedTimespec;
    struct nlattr *nestedProcess;

    int rc;

    rc = InitMessageHeaders( msg );
    if ( rc != 0 ) {
        pr_err( "InitMessage(): error creating message\n" );
        return -1;
    }

    nla_put_u32( msg->msg, FSH_NL_A_TL_ID, id );

    nestedTimespec = nla_nest_start( msg->msg, FSH_NL_A_TL_TIMESPEC );
    if ( nestedTimespec == NULL ) {
        pr_err( "InitMessage(): error creating timespec\n" );
        return -1;
    }

    ktime_get_real_ts64( &ts );

    nla_put_u64_64bit( msg->msg, FSH_NL_A_SEC, ts.tv_sec, 0 );
    nla_put_u64_64bit( msg->msg, FSH_NL_A_NSEC, ts.tv_nsec, 0 );

    nla_nest_end( msg->msg, nestedTimespec );

    nestedProcess = nla_nest_start( msg->msg, FSH_NL_A_TL_PROC );
    if ( nestedProcess == NULL ) {
        pr_err( "InitMessage(): error creating process\n" );
        return -1;
    }

    nla_put_s32( msg->msg, FSH_NL_A_PID, process->pid );
    nla_put_s32( msg->msg, FSH_NL_A_TID, process->tid );
    nla_put_u32( msg->msg, FSH_NL_A_UID, process->uid );
    nla_put_string( msg->msg, FSH_NL_A_EXE, process->exePath );

    nla_nest_end( msg->msg, nestedProcess );

    return 0;
}

int CreateOpenEventMsg( FSHookNLMsg *msg, uint32_t id, ProcessInfo *process, const char *path,
                        int flags )
{
    int rc;
    struct nlattr *nestedEvent;

    rc = InitMessage( msg, id, process );
    if ( rc != 0 ) {
        pr_err( "CreateOpenEventMsg(): error Initializing message\n" );
        return 1;
    }

    nla_put_u32( msg->msg, FSH_NL_A_TL_EVENT_TYPE, FSH_EVENT_OPEN );
    nestedEvent = nla_nest_start( msg->msg, FSH_NL_A_TL_EVENT );
    if ( nestedEvent == NULL ) {
        pr_err( "CreateOpenEventMsg(): error creating open event\n" );
        return 1;
    }

    nla_put_string( msg->msg, FSH_NL_A_OPEN_PATH, path );
    nla_put_s32( msg->msg, FSH_NL_A_OPEN_FLAGS, flags );
    nla_nest_end( msg->msg, nestedEvent );

    genlmsg_end( msg->msg, msg->head );
    return 0;
}

int CreateRenameEventMsg( FSHookNLMsg *msg, uint32_t id, ProcessInfo *process,
                          const char *sourcePath, const char *destPath )
{
    int rc;
    struct nlattr *nestedEvent;

    rc = InitMessage( msg, id, process );
    if ( rc != 0 ) {
        pr_err( "CreateRenameEventMsg(): error Initializing message\n" );
        return 1;
    }

    nla_put_u32( msg->msg, FSH_NL_A_TL_EVENT_TYPE, FSH_EVENT_RENAME );
    nestedEvent = nla_nest_start( msg->msg, FSH_NL_A_TL_EVENT );
    if ( nestedEvent == NULL ) {
        pr_err( "CreateRenameEventMsg(): error creating rename event\n" );
        return 1;
    }

    nla_put_string( msg->msg, FSH_NL_A_RENAME_SRC, sourcePath );
    nla_put_string( msg->msg, FSH_NL_A_RENAME_DST, destPath );
    nla_nest_end( msg->msg, nestedEvent );

    genlmsg_end( msg->msg, msg->head );
    return 0;
}

int CreateUnlinkEventMsg( FSHookNLMsg *msg, uint32_t id, ProcessInfo *process, const char *path )
{
    int rc;
    struct nlattr *nestedEvent;

    rc = InitMessage( msg, id, process );
    if ( rc != 0 ) {
        pr_err( "CreateUnlinkEventMsg(): error Initializing message\n" );
        return 1;
    }

    nla_put_u32( msg->msg, FSH_NL_A_TL_EVENT_TYPE, FSH_EVENT_UNLINK );
    nestedEvent = nla_nest_start( msg->msg, FSH_NL_A_TL_EVENT );
    if ( nestedEvent == NULL ) {
        pr_err( "CreateUnlinkEventMsg(): error creating open event\n" );
        return 1;
    }

    nla_put_string( msg->msg, FSH_NL_A_OPEN_PATH, path );
    nla_nest_end( msg->msg, nestedEvent );

    genlmsg_end( msg->msg, msg->head );
    return 0;
}
