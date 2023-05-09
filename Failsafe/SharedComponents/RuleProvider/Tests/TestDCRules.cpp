#include <catch2/catch.hpp>

#include "SharedComponents/RuleProvider/DC/DCRule.hpp"
#include "nlohmann/json.hpp"

TEST_CASE( "DCRule: Default values", "" )
{
    DCRule rule;

    REQUIRE( rule.Action() == ActionType::Ignore );
    REQUIRE( rule.Name() == "unnamed" );
}

TEST_CASE( "DCRule: Json deserialization" )
{
    nlohmann::json json = nlohmann::json::parse( R"({
        "name": "Test DC Rule",
        "priority": 11,
        "action": "log",
        "operationType": "deviceAdd",
        "conditions": {
            "ProductID": 11,
            "VendorID": 42
        }
    })" );

    auto rule = json.get< DCRule >();

    REQUIRE( rule.Name() == "Test DC Rule" );
    REQUIRE( rule.Action() == ActionType::Log );
}

TEST_CASE( "DCRule: Ordering" )
{
    nlohmann::json jsonLess = nlohmann::json::parse( R"({
        "name": "Test DC Rule",
        "priority": 0,
        "action": "log",
        "operationType": "deviceRemove",
        "conditions": {
            "ProductID": 11,
            "VendorID": 42
        }
    })" );

    nlohmann::json jsonGreater = nlohmann::json::parse( R"({
        "name": "Test DC Rule",
        "priority": 11,
        "action": "log",
        "operationType": "deviceRemove",
        "conditions": {
            "ProductID": 11,
            "VendorID": 42
        }
    })" );

    auto ruleLess = jsonLess.get< DCRule >();
    auto ruleGreater = jsonGreater.get< DCRule >();

    REQUIRE( jsonLess < jsonGreater );
}

TEST_CASE( "DCRule: Bad jsons" )
{
    nlohmann::json json = nlohmann::json::parse( R"({
        "name": "Test DC Rule",
        "priority": 0,
        "action": "log",
        "operationType": "deviceRemove"
    })" );

    REQUIRE_THROWS( json.get< DCRule >() );

    json = nlohmann::json::parse( R"({
        "name": "Test DC Rule",
        "priority": 0,
        "action": "log",
        "operationType": "blalad",
        "conditions": {
        } 
    })" );

    REQUIRE_THROWS( json.get< DCRule >() );

    json = nlohmann::json::parse( R"({
        "name": "Test DC Rule",
        "action": "log",
        "operationType": "deviceRemove",
        "conditions": {
        } 
    })" );

    REQUIRE_THROWS( json.get< DCRule >() );
}