#include "json.hpp"

#include <iostream>

static const char *testJson = R"(
{
    "name": "Clyne",
    "favorite": 42
}
)";

int main(void)
{
    json::parser test;
    test.start(testJson);

    while (test.isReady()) {
        auto object = test.getNextObject();
        if (!object)
            break;
        std::cout << object->name << std::endl;
    }

    return 0;
}

