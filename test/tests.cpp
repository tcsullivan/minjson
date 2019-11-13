#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "json.hpp"

const char *goodJson = R"( { "name": "Clyne" })";
const char *goodLongerJson = R"(
{
    "title": "Engineer",
    "salary": 80000.00,
    "additionalInfo": null
}
)";
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

const char *goodStringValue   = R"( { "name": "Clyne" } )";
const char *goodStringValue2  = R"( { "name": "Cly\"ne"})";
const char *goodNumberValue   = R"( { "number": 42 } )";
const char *goodNumberValue2  = R"( { "number": 42.054 } )";
const char *goodNumberValue3  = R"( { "number": -15.1 } )";
const char *goodObjectValue   = R"( { "Object": { "child?" : 6 } } )";
const char *goodArrayValue    = R"( { "Object": [ "child?" : 6 ] } )";
const char *goodBooleanValue  = R"( { "flag": true } )";
const char *goodBooleanValue2 = R"( { "flag": false } )";
const char *goodNullValue     = R"( { "dataOrNot": null } )";
const char *badStringValue    = R"( { "name": "Clyne } )";
const char *badValue          = R"( { "badbad": arstneio } )";

TEST_CASE("json::parser::determineType")
{
    json::parser test;

    auto goodTest = [&](const char *s,
                        std::size_t i,
                        std::pair<json::type, std::size_t> p)
    {
        test.start(s);
        auto result = test.determineType(i);
        REQUIRE(result);
        REQUIRE(*result == p);
    };
    auto badTest = [&](const char *s, std::size_t i) {
        test.start(s);
        auto result = test.determineType(i);
        REQUIRE(!result);
    };

    // First number is index before value, second number is index after value
    goodTest(goodStringValue,   8,  {json::type::string, 16});
    goodTest(goodStringValue2,  8,  {json::type::string, 18});
    goodTest(goodNumberValue,   10, {json::type::number, 13});
    goodTest(goodNumberValue2,  10, {json::type::number, 17});
    goodTest(goodNumberValue3,  10, {json::type::number, 16});
    goodTest(goodObjectValue,   10, {json::type::object, 27});
    goodTest(goodArrayValue,    10, {json::type::array, 27});
    goodTest(goodBooleanValue,  8,  {json::type::boolean, 13});
    goodTest(goodBooleanValue2, 8,  {json::type::boolean, 14});
    goodTest(goodNullValue,     13, {json::type::null, 18});
    badTest(badStringValue, 10);
    badTest(badValue, 12);
}

TEST_CASE("json::parser::getNextObject")
{
    json::parser test;

    REQUIRE(!test.getNextObject());

    test.start(goodLongerJson);
    auto object = test.getNextObject();
    REQUIRE(object);
    REQUIRE(object->name == "title");
    REQUIRE(object->type == json::type::string);
    object = test.getNextObject();
    REQUIRE(object);
    REQUIRE(object->name == "salary");
    REQUIRE(object->type == json::type::number);
    object = test.getNextObject();
    REQUIRE(object);
    REQUIRE(object->name == "additionalInfo");
    REQUIRE(object->type == json::type::null);
    REQUIRE(!test.getNextObject());
}

