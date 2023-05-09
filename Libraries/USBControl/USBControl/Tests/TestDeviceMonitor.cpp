#include <catch2/catch.hpp>

#include "USBControl/UCDeviceMonitor.h"

static void DummyCallback( UCDeviceMonitor *monitor, UCDeviceEvent *event, void *data )
{
    ( void )monitor;
    ( void )event;
    ( void )data;
}

TEST_CASE( "UCDeviceMonitorInit: UCDeviceMonitor is properly filled", "" )
{
    UCDeviceMonitor monitor;

    int rc = UCDeviceMonitorInit( &monitor, DummyCallback, ( void * )0xdeadbeef );
    REQUIRE( rc == 0 );

    CHECK( monitor.udevCtx != NULL );
    CHECK( monitor.udevMonitor != NULL );
    CHECK( monitor.processingCallback == DummyCallback );
    CHECK( monitor.userData == ( void * )0xdeadbeef );

    UCDeviceMonitorFree( &monitor );
}

TEST_CASE( "UCDeviceMonitorInit: UCDeviceMonitor is NULL", "" )
{
    REQUIRE( UCDeviceMonitorInit( NULL, DummyCallback, NULL ) != 0 );
}

TEST_CASE( "UCDeviceMonitorInit: Processing callback is NULL", "" )
{
    UCDeviceMonitor monitor;
    REQUIRE( UCDeviceMonitorInit( &monitor, NULL, NULL ) != 0 );
}

TEST_CASE( "UCDeviceMonitorInit: UCDeviceMonitor and processing callback are NULL", "" )
{
    REQUIRE( UCDeviceMonitorInit( NULL, NULL, NULL ) != 0 );
}

TEST_CASE( "UCDeviceMonitorStart: UCDeviceMonitor is NULL", "" )
{
    REQUIRE( UCDeviceMonitorStart( NULL ) != 0 );
}

TEST_CASE( "UCDeviceMonitorFree: UCDeviceMonitor is NULL", "" )
{
    UCDeviceMonitorFree( NULL );
    // just test that we don't crash here
    REQUIRE( 1 == 1 );
}