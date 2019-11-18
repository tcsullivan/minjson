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

void iterateParser(json::parser test)
{
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
        case json::type::object:
            std::cout << object->name << ": " << std::endl << "--------" << std::endl;
            iterateParser(object->getObject());
            std::cout << "--------" << std::endl;
            break;
        default:
            std::cout << object->name << "? " << object->value << std::endl;
            break;
        }
    }
}

int main(void)
{
    json::parser test;
    test.start(testJson);
    iterateParser(test);
    return 0;
}

