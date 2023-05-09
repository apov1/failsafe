#include <catch2/catch.hpp>

#include "FSHook/FSHook.h"

static void DummyCallback( FSHookConnector *connector, FSHookMessage *event, void *data )
{
    ( void )connector;
    ( void )event;
    ( void )data;
}

TEST_CASE( "FSHookConnectorInit: FSHookConnector is properly filled", "" )
{
    FSHookConnector connector;

    int rc = FSHookConnectorInit( &connector, DummyCallback, ( void * )0xdeadbeef );
    REQUIRE( rc == 0 );

    CHECK( connector.socket != NULL );
    CHECK( connector.processingThread != 0 );
    CHECK( connector.processingCallback == DummyCallback );
    CHECK( connector.data == ( void * )0xdeadbeef );

    pthread_cancel( connector.processingThread );
    FSHookConnectorFree( &connector );
}

TEST_CASE( "FSHookConnectorInit: FSHookConnector is NULL", "" )
{
    REQUIRE( FSHookConnectorInit( NULL, DummyCallback, NULL ) != 0 );
}

TEST_CASE( "FSHookConnectorInit: Processing callback is NULL", "" )
{
    FSHookConnector connector;
    REQUIRE( FSHookConnectorInit( &connector, NULL, NULL ) != 0 );
}

TEST_CASE( "FSHookConnectorInit: FSHookConnector and processing callback are NULL", "" )
{
    REQUIRE( FSHookConnectorInit( NULL, NULL, NULL ) != 0 );
}

TEST_CASE( "FSHookRegister: FSHookConnector is NULL", "" )
{
    REQUIRE( FSHookRegister( NULL ) != 0 );
}

TEST_CASE( "FSHookConnectorFree: FSHookConnector is NULL", "" )
{
    FSHookConnectorFree( NULL );
    // just test that we don't crash here
    REQUIRE( 1 == 1 );
}