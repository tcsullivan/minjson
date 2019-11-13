#include "json.hpp"

#include <iostream>

static const char *testJson = R"(
{
    "ID": "SGML",
    "SortAs": "SGML",
    "GlossTerm": "Standard Generalized Markup Language",
    "Acronym": "SGML",
    "Abbrev": "ISO 8879:1986",
    "GlossDef": {
        "para": "A meta-markup language, used to create markup languages such as DocBook.",
    	"GlossSeeAlso": ["GML", "XML"]
    },
    "GlossSee": "markup"
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
        std::cout << object->name << ": " << object->value << std::endl;
    }

    return 0;
}

