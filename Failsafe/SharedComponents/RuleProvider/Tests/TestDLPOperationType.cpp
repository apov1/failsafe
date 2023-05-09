#include <catch2/catch.hpp>

#include "SharedComponents/RuleProvider/DLP/DLPOperationType.hpp"

TEST_CASE( "DLPOperationType: To string", "" )
{
    REQUIRE( DLPOperationToString( DLPOperationType::Upload ) == "upload" );
    REQUIRE( DLPOperationToString( DLPOperationType::IM ) == "instantMessaging" );
    REQUIRE( DLPOperationToString( DLPOperationType::FileTransfer ) == "fileTransfer" );
    REQUIRE( DLPOperationToString( DLPOperationType::Delete ) == "delete" );
}

TEST_CASE( "DLPOperationType: From string", "" )
{
    REQUIRE( DLPOperationFromString( "upload" ) == DLPOperationType::Upload );
    REQUIRE( DLPOperationFromString( "instantMessaging" ) == DLPOperationType::IM );
    REQUIRE( DLPOperationFromString( "fileTransfer" ) == DLPOperationType::FileTransfer );
    REQUIRE( DLPOperationFromString( "delete" ) == DLPOperationType::Delete );
}

TEST_CASE( "DLPOperationType: Bad values", "" )
{
    REQUIRE_THROWS( DLPOperationFromString( "test1" ) );
    REQUIRE_THROWS( DLPOperationFromString( "badString" ) );
    REQUIRE_THROWS( DLPOperationFromString( "blabla" ) );
    REQUIRE_THROWS( DLPOperationFromString( "" ) );
    REQUIRE_THROWS( DLPOperationFromString( "upload1" ) );
    REQUIRE_THROWS( DLPOperationFromString( "fileTransf" ) );

    REQUIRE( DLPOperationToString( static_cast< DLPOperationType >( 42 ) ) == "" );
}