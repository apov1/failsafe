#include <catch2/catch.hpp>

#include "USBControl/UCDeviceEnumerate.h"

TEST_CASE( "UCDeviceEnumerateInit: UCDeviceEnumerateCtx is properly filled", "" )
{
    UCDeviceEnumerateCtx ctx;

    int rc = UCDeviceEnumerateInit( &ctx );
    REQUIRE( rc == 0 );

    CHECK( ctx.udevCtx != NULL );
    CHECK( ctx.enumerateCtx != NULL );

    UCDeviceEnumerateFree( &ctx );
}

TEST_CASE( "UCDeviceEnumerateInit: UCDeviceEnumerateCtx is NULL", "" )
{
    REQUIRE( UCDeviceEnumerateInit( NULL ) != 0 );
}

TEST_CASE( "UCDeviceEnumerateGetNext: UCDeviceEnumerateCtx is NULL", "" )
{
    REQUIRE( UCDeviceEnumerateGetNext( NULL ) == NULL );
}

TEST_CASE( "UCDeviceEnumerateGetNext: Return NULL when next entry is NULL", "" )
{
    UCDeviceEnumerateCtx ctx;
    ctx.entryList = NULL;

    REQUIRE( UCDeviceEnumerateGetNext( &ctx ) == NULL );
}

TEST_CASE( "UCDeviceEnumerateFree: UCDeviceEnumerateCtx is NULL", "" )
{
    UCDeviceEnumerateFree( NULL );
    // just test that we don't crash here
    REQUIRE( 1 == 1 );
}