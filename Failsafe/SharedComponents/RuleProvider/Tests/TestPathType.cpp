#include <catch2/catch.hpp>

#include "Modules/DLP/PathResolver/PathType.hpp"

TEST_CASE( "PathType: To string", "" )
{
    REQUIRE( PathTypeToString( PathType::Local ) == "local" );
    REQUIRE( PathTypeToString( PathType::USB ) == "usb" );
    REQUIRE( PathTypeToString( PathType::NetworkDrive ) == "networkDrive" );
    REQUIRE( PathTypeToString( PathType::Dropbox ) == "dropbox" );
}

TEST_CASE( "PathType: From string", "" )
{
    REQUIRE( StringToPathType( "local" ) == PathType::Local );
    REQUIRE( StringToPathType( "usb" ) == PathType::USB );
    REQUIRE( StringToPathType( "networkDrive" ) == PathType::NetworkDrive );
    REQUIRE( StringToPathType( "dropbox" ) == PathType::Dropbox );
}

TEST_CASE( "PathType: Bad values", "" )
{
    REQUIRE_THROWS( StringToPathType( "test1" ) );
    REQUIRE_THROWS( StringToPathType( "badString" ) );
    REQUIRE_THROWS( StringToPathType( "blabla" ) );
    REQUIRE_THROWS( StringToPathType( "" ) );
    REQUIRE_THROWS( StringToPathType( "ignore1" ) );
    REQUIRE_THROWS( StringToPathType( "networkdrive" ) );

    REQUIRE( PathTypeToString( static_cast< PathType >( 42 ) ) == "" );
}