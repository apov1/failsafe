#include <catch2/catch.hpp>

#include "SharedComponents/RuleProvider/RuleProvider.hpp"

TEST_CASE( "RuleProvider: Default state", "" )
{
    RuleProvider provider;

    REQUIRE( provider.DefaultAction() == ActionType::Ignore );
    REQUIRE( provider.NumDLPRules() == 0 );
    REQUIRE( provider.NumDCRules() == 0 );
}

TEST_CASE( "RuleProvider: DefaultAction", "" )
{
    RuleProvider provider;

    provider.UpdateRules( R"({
        "defaultAction": "ignore",
        "DLPRules": [],
        "DCRules": []
    })" );

    REQUIRE( provider.DefaultAction() == ActionType::Ignore );

    provider.UpdateRules( R"({
        "defaultAction": "log",
        "DLPRules": [],
        "DCRules": []
    })" );

    REQUIRE( provider.DefaultAction() == ActionType::Log );

    provider.UpdateRules( R"({
        "defaultAction": "block",
        "DLPRules": [],
        "DCRules": []
    })" );

    REQUIRE( provider.DefaultAction() == ActionType::Block );
}

TEST_CASE( "RuleProvider: Empty rules", "" )
{
    RuleProvider provider;

    provider.UpdateRules( R"({
        "defaultAction": "block",
        "DLPRules": [],
        "DCRules": []
    })" );

    REQUIRE( provider.DefaultAction() == ActionType::Block );
    REQUIRE( provider.NumDLPRules() == 0 );
    REQUIRE( provider.NumDCRules() == 0 );
}

TEST_CASE( "RuleProvider: Additional keys ", "" )
{
    RuleProvider provider;

    REQUIRE_NOTHROW( provider.UpdateRules( R"({
        "defaultAction": "block",
        "DLPRules": [],
        "DCRules": [],
        "testqweqwe": "wontFail"
    })" ) );
}

TEST_CASE( "RuleProvider: Missing keys", "" )
{
    RuleProvider provider;

    REQUIRE_THROWS( provider.UpdateRules( R"({
        "DCRules": []
    })" ) );

    REQUIRE_THROWS( provider.UpdateRules( R"({
        "defaultAction": "log"
        "DLPRules": []
    })" ) );
}

TEST_CASE( "RuleProvider: Invalid default action", "" )
{
    RuleProvider provider;

    REQUIRE_THROWS( provider.UpdateRules( R"({
        "defaultAction": "shouldThrow",
        "DLPRules": [],
        "DCRules": [],
    })" ) );
}

TEST_CASE( "RuleProvider: Invalid rules", "" )
{
    RuleProvider provider;

    REQUIRE_THROWS( provider.UpdateRules( R"({
        "defaultAction": "ignore",
        "DLPRules": "bad",
        "DCRules": [],
    })" ) );

    REQUIRE_THROWS( provider.UpdateRules( R"({
        "defaultAction": "ignore",
        "DLPRules": [
            {"bad": "I am a Bad rule"}
        ],
        "DCRules": [],
    })" ) );
}

TEST_CASE( "RuleProvider: Valid DLP rules", "" )
{
    RuleProvider provider;

    provider.UpdateRules( R"({
        "defaultAction": "ignore",
        "DLPRules": [
            {
                "name": "Test DLP Rule",
                "priority": 0,
                "operationType": "upload",
                "action": "block",
                "conditions": {
                    "fileExtension": ".pdf"
                }
            },
            {
                "name": "Test DLP Rule 2",
                "priority": 0,
                "operationType": "fileTransfer",
                "action": "block",
                "conditions": {}
            }
        ],
        "DCRules": []
    })" );

    REQUIRE( provider.NumDLPRules() == 2 );
    REQUIRE( provider.NumDCRules() == 0 );
}

TEST_CASE( "RuleProvider: Valid DC rules", "" )
{
    RuleProvider provider;

    provider.UpdateRules( R"({
        "defaultAction": "ignore",
        "DLPRules": [],
        "DCRules": [
            {
                "name": "Test DC Rule",
                "priority": 11,
                "action": "log",
                "operationType": "deviceRemove",
                "conditions": {
                    "ProductID": 11,
                    "VendorID": 42
                }
            }
        ]
    })" );

    REQUIRE( provider.NumDLPRules() == 0 );
    REQUIRE( provider.NumDCRules() == 1 );
}
