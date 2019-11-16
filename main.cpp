#include "json.hpp"

#include <iostream>

static const char *testJson = R"(
{
    "ID": "SGML",
    "SortAs": "SGML",
    "GlossTerm": "Standard Generalized Markup Language",
    "Acronym": "SGML",
    "Abbrev": 887.9,
    "GlossDef": {
        "para": "A meta-markup language, used to create markup languages such as DocBook.",
    	"GlossSeeAlso": ["GML", "XML"]
    },
    "GlossSee": false
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
        switch (object->type) {
        case json::type::string:
            std::cout << object->name << ": " << object->getString() << std::endl;
            break;
        case json::type::number:
            std::cout << object->name << ": " << object->getNumber<float>() << std::endl;
            break;
        case json::type::boolean:
            std::cout << object->name << ": " << (object->getBoolean() ? "true" : "false") << std::endl;
            break;
        default:
            std::cout << object->name << "? " << object->value << std::endl;
            break;
        }
    }

    return 0;
}

