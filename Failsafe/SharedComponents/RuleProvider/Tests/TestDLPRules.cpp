#include <catch2/catch.hpp>

#include "SharedComponents/RuleProvider/DLP/DLPRule.hpp"
#include "nlohmann/json.hpp"

TEST_CASE( "DLPRule: Default values", "" )
{
    DLPRule rule;

    REQUIRE( rule.Action() == ActionType::Ignore );
    REQUIRE( rule.Name() == "unnamed" );
}

TEST_CASE( "DLPRule: Json deserialization" )
{
    nlohmann::json json = nlohmann::json::parse( R"({
        "name": "Test DLP Rule",
        "priority": 0,
        "operationType": "upload",
        "action": "block",
        "conditions": {
            "fileExtension": ".pdf"
        }
    })" );

    auto rule = json.get< DLPRule >();

    REQUIRE( rule.Name() == "Test DLP Rule" );
    REQUIRE( rule.Action() == ActionType::Block );
}

TEST_CASE( "DLPRule: Ordering" )
{
    nlohmann::json jsonLess = nlohmann::json::parse( R"({
        "name": "Test DLP Rule",
        "priority": 0,
        "operationType": "upload",
        "action": "block",
        "conditions": {
            "fileExtension": ".pdf"
        }
    })" );

    nlohmann::json jsonGreater = nlohmann::json::parse( R"({
        "name": "Test DLP Rule",
        "priority": 42,
        "operationType": "upload",
        "action": "block",
        "conditions": {
            "fileExtension": ".pdf"
        }
    })" );

    auto ruleLess = jsonLess.get< DLPRule >();
    auto ruleGreater = jsonGreater.get< DLPRule >();

    REQUIRE( jsonLess < jsonGreater );
}

TEST_CASE( "DLPRule: Bad jsons" )
{
    nlohmann::json json = nlohmann::json::parse( R"({
        "name": "Test DC Rule",
        "priority": 0,
        "action": "log",
        "operationType": "deviceRemove"
    })" );

    REQUIRE_THROWS( json.get< DLPRule >() );

    json = nlohmann::json::parse( R"({
        "name": "Test DLP Rule",
        "priority": 42,
        "operationType": "upload",
        "action": "bad",
        "conditions": {
            "fileExtension": ".pdf"
        }
    })" );

    REQUIRE_THROWS( json.get< DLPRule >() );

    json = nlohmann::json::parse( R"({
        "name": "Test DLP Rule",
        "action": "block",
        "conditions": {
            "fileExtension": ".pdf"
        }
    })" );

    REQUIRE_THROWS( json.get< DLPRule >() );
}

TEST_CASE( "DLPRule: Conditions mismatch", "" )
{
    nlohmann::json json = nlohmann::json::parse( R"({
        "name": "Test DLP Rule",
        "priority": 0,
        "operationType": "fileTransfer",
        "action": "block",
        "conditions": {
            "fileExtension": ".pdf",
            "sourceType": "local1"
        }
    })" );

    REQUIRE_THROWS( json.get< DLPRule >() );
}
