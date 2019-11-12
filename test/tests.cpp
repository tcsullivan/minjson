#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "json.hpp"

const char *goodJson = R"( { "name": "Clyne" })";
const char *missingBeginningJson = R"( "name": "Clyne" })";
const char *missingEndJson = R"( { "name": "Clyne" )";

TEST_CASE("json::parser::start")
{
    json::parser test;
    REQUIRE(test.start(goodJson)             == true);
    REQUIRE(test.start(missingBeginningJson) == false);
    REQUIRE(test.start(missingEndJson)       == false);
    REQUIRE(test.start(goodJson)             == true);
}

TEST_CASE("json::parser::isReady")
{
    json::parser test;
    REQUIRE(test.isReady() == false);
    test.start(goodJson);
    REQUIRE(test.isReady() == true);
    test.start(missingBeginningJson);
    REQUIRE(test.isReady() == false);
}

