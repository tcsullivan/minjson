#include "json.hpp"

#include <iostream>

static const char *testJson = R"(
{
    "name": "Clyne"
}
)";

int main(void)
{
    json::parser test;
    test.start(testJson);

    auto object = test.getNextObject();

    std::cout << (object ? object->name : "bad!") << std::endl;

    return 0;
}

