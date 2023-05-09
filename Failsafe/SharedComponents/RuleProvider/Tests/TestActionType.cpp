#include <catch2/catch.hpp>

#include "SharedComponents/RuleProvider/ActionType.hpp"

TEST_CASE( "ActionType: To string", "" )
{
    REQUIRE( ActionToString( ActionType::Ignore ) == "ignore" );
    REQUIRE( ActionToString( ActionType::Log ) == "log" );
    REQUIRE( ActionToString( ActionType::Block ) == "block" );
}

TEST_CASE( "ActionType: From string", "" )
{
    REQUIRE( ActionFromString( "ignore" ) == ActionType::Ignore );
    REQUIRE( ActionFromString( "log" ) == ActionType::Log );
    REQUIRE( ActionFromString( "block" ) == ActionType::Block );
}

TEST_CASE( "ActionType: Bad values", "" )
{
    REQUIRE_THROWS( ActionFromString( "test1" ) );
    REQUIRE_THROWS( ActionFromString( "badString" ) );
    REQUIRE_THROWS( ActionFromString( "blabla" ) );
    REQUIRE_THROWS( ActionFromString( "" ) );
    REQUIRE_THROWS( ActionFromString( "ignore1" ) );
    REQUIRE_THROWS( ActionFromString( "bloc" ) );

    REQUIRE( ActionToString( static_cast< ActionType >( 42 ) ) == "" );
}