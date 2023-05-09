#include <catch2/catch.hpp>

#include "Common/NetlinkFamily.h"
#include "FSHook/NetlinkParser.h"

#include <functional>
#include <netlink/attr.h>
#include <netlink/genl/genl.h>
#include <netlink/msg.h>

static struct nl_msg *InitMessage( std::function< void( struct nl_msg * ) > fill,
                                   struct nlattr **tbMsg )
{
    struct nl_msg *msg = nlmsg_alloc();
    CHECK( genlmsg_put( msg, NL_AUTO_PID, NL_AUTO_SEQ, 0, 0, 0, FSH_NL_C_REGISTER, 1 ) != NULL );

    fill( msg );

    struct nlmsghdr *retHdr = nlmsg_hdr( msg );
    CHECK( retHdr != NULL );
    CHECK( genlmsg_parse( retHdr, 0, tbMsg, __FSH_NL_A_MAX - 1, NULL ) == 0 );

    return msg;
}

static void FillMessageId( struct nl_msg *msg, uint32_t id )
{
    nla_put_u32( msg, FSH_NL_A_TL_ID, id );
}

static void FillTimespec( struct nl_msg *msg, uint64_t sec, uint64_t nsec )
{
    struct nlattr *nestedTimespec = nla_nest_start( msg, FSH_NL_A_TL_TIMESPEC );
    REQUIRE( nestedTimespec != NULL );

    if ( sec != ( uint64_t )-1 )
        nla_put_u64( msg, FSH_NL_A_SEC, sec );

    if ( nsec != ( uint64_t )-1 )
        nla_put_u64( msg, FSH_NL_A_NSEC, nsec );

    nla_nest_end( msg, nestedTimespec );
}

static void FillProcessInfo( struct nl_msg *msg, pid_t pid, pid_t tid, uid_t uid, const char *exe )
{
    struct nlattr *nestedProcinfo = nla_nest_start( msg, FSH_NL_A_TL_PROC );
    REQUIRE( nestedProcinfo != NULL );

    nla_put_s32( msg, FSH_NL_A_PID, pid );
    nla_put_s32( msg, FSH_NL_A_TID, tid );
    nla_put_u32( msg, FSH_NL_A_UID, uid );

    if ( exe != NULL )
        nla_put_string( msg, FSH_NL_A_EXE, exe );

    nla_nest_end( msg, nestedProcinfo );
}

static void FillOpenEvent( struct nl_msg *msg, const char *path, int flags )
{
    nla_put_u32( msg, FSH_NL_A_TL_EVENT_TYPE, FSH_EVENT_OPEN );

    struct nlattr *nestedEvent = nla_nest_start( msg, FSH_NL_A_TL_EVENT );
    REQUIRE( nestedEvent != NULL );

    if ( path != nullptr )
        nla_put_string( msg, FSH_NL_A_OPEN_PATH, path );
    nla_put_s32( msg, FSH_NL_A_OPEN_FLAGS, flags );
    nla_nest_end( msg, nestedEvent );
}

static void FillRenameEvent( struct nl_msg *msg, const char *srcPath, const char *dstPath )
{
    nla_put_u32( msg, FSH_NL_A_TL_EVENT_TYPE, FSH_EVENT_RENAME );

    struct nlattr *nestedEvent = nla_nest_start( msg, FSH_NL_A_TL_EVENT );
    REQUIRE( nestedEvent != NULL );

    if ( srcPath != nullptr )
        nla_put_string( msg, FSH_NL_A_RENAME_SRC, srcPath );

    if ( dstPath != nullptr )
        nla_put_string( msg, FSH_NL_A_RENAME_DST, dstPath );

    nla_nest_end( msg, nestedEvent );
}

static void FillUnlinkEvent( struct nl_msg *msg, const char *path )
{
    nla_put_u32( msg, FSH_NL_A_TL_EVENT_TYPE, FSH_EVENT_UNLINK );

    struct nlattr *nestedEvent = nla_nest_start( msg, FSH_NL_A_TL_EVENT );
    REQUIRE( nestedEvent != NULL );

    if ( path != nullptr )
        nla_put_string( msg, FSH_NL_A_UNLINK_PATH, path );

    nla_nest_end( msg, nestedEvent );
}

TEST_CASE( "_ParseTimespec: Valid", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg =
        InitMessage( []( struct nl_msg *msg ) { FillTimespec( msg, 42, 52 ); }, tbMsg );

    FSHookMessage fsMsg;

    CHECK( tbMsg[ FSH_NL_A_TL_TIMESPEC ] != NULL );

    int rc = _ParseTimespec( &fsMsg, tbMsg[ FSH_NL_A_TL_TIMESPEC ] );
    CHECK( rc == 0 );

    CHECK( fsMsg.time.tv_sec == 42 );
    CHECK( fsMsg.time.tv_nsec == 52 );

    nlmsg_free( msg );
}

TEST_CASE( "_ParseTimespec: Invalid attr NULL", "" )
{
    REQUIRE( _ParseTimespec( NULL, NULL ) != 0 );
}

TEST_CASE( "_ParseTimespec: Not filled completely", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg =
        InitMessage( []( struct nl_msg *msg ) { FillTimespec( msg, -1, 52 ); }, tbMsg );

    FSHookMessage fsMsg;

    CHECK( tbMsg[ FSH_NL_A_TL_TIMESPEC ] != NULL );

    int rc = _ParseTimespec( &fsMsg, tbMsg[ FSH_NL_A_TL_TIMESPEC ] );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "_ParseProcessInfo: Valid", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) { FillProcessInfo( msg, 1, 2, 3, "TESTHERE" ); }, tbMsg );

    FSHookProcessInfo procInfo;

    CHECK( tbMsg[ FSH_NL_A_TL_PROC ] != NULL );

    int rc = _ParseProcessInfo( &procInfo, tbMsg[ FSH_NL_A_TL_PROC ] );
    CHECK( rc == 0 );

    CHECK( procInfo.pid == 1 );
    CHECK( procInfo.tid == 2 );
    CHECK( procInfo.uid == 3 );
    CHECK( strcmp( procInfo.executablePath, "TESTHERE" ) == 0 );

    nlmsg_free( msg );
}

TEST_CASE( "_ParseProcessInfo: Invalid attr NULL", "" )
{
    REQUIRE( _ParseProcessInfo( NULL, NULL ) != 0 );
}

TEST_CASE( "_ParseProcessInfo: Not filled completely", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg =
        InitMessage( []( struct nl_msg *msg ) { FillProcessInfo( msg, 1, 2, 3, NULL ); }, tbMsg );

    FSHookProcessInfo procInfo;

    CHECK( tbMsg[ FSH_NL_A_TL_PROC ] != NULL );

    int rc = _ParseProcessInfo( &procInfo, tbMsg[ FSH_NL_A_TL_PROC ] );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "_ParseOpenEvent: Valid", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) { FillOpenEvent( msg, "testPath", 0xbadf00d ); }, tbMsg );

    FSHookEvent fsEvent;

    CHECK( tbMsg[ FSH_NL_A_TL_EVENT ] != NULL );
    CHECK( tbMsg[ FSH_NL_A_TL_EVENT_TYPE ] != NULL );

    int rc = _ParseOpenEvent( &fsEvent, tbMsg[ FSH_NL_A_TL_EVENT ] );
    CHECK( rc == 0 );

    CHECK( strcmp( fsEvent.open.path, "testPath" ) == 0 );
    CHECK( fsEvent.open.flags == 0xbadf00d );

    nlmsg_free( msg );
}

TEST_CASE( "_ParseOpenEvent: Invalid attr NULL", "" )
{
    REQUIRE( _ParseOpenEvent( NULL, NULL ) != 0 );
}

TEST_CASE( "_ParseOpenEvent: Not filled completely", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg =
        InitMessage( []( struct nl_msg *msg ) { FillOpenEvent( msg, nullptr, 0 ); }, tbMsg );

    FSHookEvent fsEvent;

    CHECK( tbMsg[ FSH_NL_A_TL_EVENT ] != NULL );
    CHECK( tbMsg[ FSH_NL_A_TL_EVENT_TYPE ] != NULL );

    int rc = _ParseOpenEvent( &fsEvent, tbMsg[ FSH_NL_A_TL_EVENT ] );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "_ParseRenameEvent: Valid", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillRenameEvent( msg, "testPathSource", "testPathDestination" );
        },
        tbMsg );

    FSHookEvent fsEvent;

    CHECK( tbMsg[ FSH_NL_A_TL_EVENT ] != NULL );
    CHECK( tbMsg[ FSH_NL_A_TL_EVENT_TYPE ] != NULL );

    int rc = _ParseRenameEvent( &fsEvent, tbMsg[ FSH_NL_A_TL_EVENT ] );
    CHECK( rc == 0 );

    CHECK( strcmp( fsEvent.rename.sourcePath, "testPathSource" ) == 0 );
    CHECK( strcmp( fsEvent.rename.destPath, "testPathDestination" ) == 0 );

    nlmsg_free( msg );
}

TEST_CASE( "_ParseRenameEvent: Invalid attr NULL", "" )
{
    REQUIRE( _ParseRenameEvent( NULL, NULL ) != 0 );
}

TEST_CASE( "_ParseRenameEvent: Not filled completely", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) { FillRenameEvent( msg, nullptr, "testPath" ); }, tbMsg );

    FSHookEvent fsEvent;

    CHECK( tbMsg[ FSH_NL_A_TL_EVENT ] != NULL );
    CHECK( tbMsg[ FSH_NL_A_TL_EVENT_TYPE ] != NULL );

    int rc = _ParseRenameEvent( &fsEvent, tbMsg[ FSH_NL_A_TL_EVENT ] );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "_ParseUnlinkEvent: Valid", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg =
        InitMessage( []( struct nl_msg *msg ) { FillUnlinkEvent( msg, "testPath" ); }, tbMsg );

    FSHookEvent fsEvent;

    CHECK( tbMsg[ FSH_NL_A_TL_EVENT ] != NULL );
    CHECK( tbMsg[ FSH_NL_A_TL_EVENT_TYPE ] != NULL );

    int rc = _ParseUnlinkEvent( &fsEvent, tbMsg[ FSH_NL_A_TL_EVENT ] );
    CHECK( rc == 0 );

    CHECK( strcmp( fsEvent.unlink.path, "testPath" ) == 0 );

    nlmsg_free( msg );
}

TEST_CASE( "_ParseUnlinkEvent: Invalid attr NULL", "" )
{
    REQUIRE( _ParseUnlinkEvent( NULL, NULL ) != 0 );
}

TEST_CASE( "_ParseUnlinkEvent: Not filled completely", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg =
        InitMessage( []( struct nl_msg *msg ) { FillUnlinkEvent( msg, nullptr ); }, tbMsg );

    FSHookEvent fsEvent;

    CHECK_FALSE( tbMsg[ FSH_NL_A_TL_EVENT ] != NULL );
    CHECK( tbMsg[ FSH_NL_A_TL_EVENT_TYPE ] != NULL );

    int rc = _ParseUnlinkEvent( &fsEvent, tbMsg[ FSH_NL_A_TL_EVENT ] );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Valid open event", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 42 );
            FillTimespec( msg, 1, 2 );
            FillProcessInfo( msg, 10, 11, 12, "testExe" );
            FillOpenEvent( msg, "testPath", 0xdeadbeef );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc == 0 );

    CHECK( fsMsg.id == 42 );
    CHECK( fsMsg.time.tv_sec == 1 );
    CHECK( fsMsg.time.tv_nsec == 2 );
    CHECK( fsMsg.process.pid == 10 );
    CHECK( fsMsg.process.tid == 11 );
    CHECK( strcmp( fsMsg.process.executablePath, "testExe" ) == 0 );
    CHECK( fsMsg.eventType == FSH_EVENT_OPEN );
    CHECK( strcmp( fsMsg.event.open.path, "testPath" ) == 0 );
    CHECK( fsMsg.event.open.flags == 0xdeadbeef );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Valid rename event", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 42 );
            FillTimespec( msg, 1, 2 );
            FillProcessInfo( msg, 10, 11, 12, "testExe" );
            FillRenameEvent( msg, "testPathSrc", "testPathDst" );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc == 0 );

    CHECK( fsMsg.id == 42 );
    CHECK( fsMsg.time.tv_sec == 1 );
    CHECK( fsMsg.time.tv_nsec == 2 );
    CHECK( fsMsg.process.pid == 10 );
    CHECK( fsMsg.process.tid == 11 );
    CHECK( strcmp( fsMsg.process.executablePath, "testExe" ) == 0 );
    CHECK( fsMsg.eventType == FSH_EVENT_RENAME );
    CHECK( strcmp( fsMsg.event.rename.sourcePath, "testPathSrc" ) == 0 );
    CHECK( strcmp( fsMsg.event.rename.destPath, "testPathDst" ) == 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Valid unlink event", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 42 );
            FillTimespec( msg, 1, 2 );
            FillProcessInfo( msg, 10, 11, 12, "testExe" );
            FillUnlinkEvent( msg, "testPath" );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc == 0 );

    CHECK( fsMsg.id == 42 );
    CHECK( fsMsg.time.tv_sec == 1 );
    CHECK( fsMsg.time.tv_nsec == 2 );
    CHECK( fsMsg.process.pid == 10 );
    CHECK( fsMsg.process.tid == 11 );
    CHECK( strcmp( fsMsg.process.executablePath, "testExe" ) == 0 );
    CHECK( fsMsg.eventType == FSH_EVENT_UNLINK );
    CHECK( strcmp( fsMsg.event.unlink.path, "testPath" ) == 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Invalid open event", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 42 );
            FillTimespec( msg, 1, 2 );
            FillProcessInfo( msg, 10, 11, 12, "testExe" );
            FillOpenEvent( msg, nullptr, 0xdeadbeef );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Invalid rename event", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 42 );
            FillTimespec( msg, 1, 2 );
            FillProcessInfo( msg, 10, 11, 12, "testExe" );
            FillRenameEvent( msg, nullptr, nullptr );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Invalid unlink event", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 42 );
            FillTimespec( msg, 1, 2 );
            FillProcessInfo( msg, 10, 11, 12, "testExe" );
            FillUnlinkEvent( msg, nullptr );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Missing msgId", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillTimespec( msg, 1, 2 );
            FillProcessInfo( msg, 10, 11, 12, "testExe" );
            FillUnlinkEvent( msg, "test" );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Missing timespec", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 1 );
            FillProcessInfo( msg, 10, 11, 12, "testExe" );
            FillUnlinkEvent( msg, "test" );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Invalid timespec", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 1 );
            FillTimespec( msg, -1, 2 );
            FillProcessInfo( msg, 10, 11, 12, "testExe" );
            FillUnlinkEvent( msg, "test" );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Missing ProcessInfo", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 1 );
            FillTimespec( msg, 1, 2 );
            FillUnlinkEvent( msg, "test" );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: Invalid ProcessInfo", "" )
{
    struct nlattr *tbMsg[ __FSH_NL_A_MAX ];
    struct nl_msg *msg = InitMessage(
        []( struct nl_msg *msg ) {
            FillMessageId( msg, 1 );
            FillTimespec( msg, 1, 2 );
            FillProcessInfo( msg, 10, 11, 12, nullptr );
            FillUnlinkEvent( msg, "test" );
        },
        tbMsg );

    FSHookMessage fsMsg;

    int rc = ParseNetlinkMessage( &fsMsg, msg );
    CHECK( rc != 0 );

    nlmsg_free( msg );
}

TEST_CASE( "ParseNetlinkMessage: NULL input structs", "" )
{
    int rc = ParseNetlinkMessage( nullptr, nullptr );
    REQUIRE( rc != 0 );
}