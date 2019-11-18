#include "json.hpp"

#include <iostream>

static const char *testJson = R"json(
{"menu": {
    "header": "SVG Viewer",
    "items": [
        {"id": "Open"},
        {"id": "OpenNew", "label": "Open New"},
        null,
        {"id": "ZoomIn", "label": "Zoom In"},
        {"id": "ZoomOut", "label": "Zoom Out"},
        {"id": "OriginalView", "label": "Original View"},
        null,
        {"id": "Quality"},
        {"id": "Pause"},
        {"id": "Mute"},
        null,
        {"id": "Find", "label": "Find..."},
        {"id": "FindAgain", "label": "Find Again"},
        {"id": "Copy"},
        {"id": "CopyAgain", "label": "Copy Again"},
        {"id": "CopySVG", "label": "Copy SVG"},
        {"id": "ViewSVG", "label": "View SVG"},
        {"id": "ViewSource", "label": "View Source"},
        {"id": "SaveAs", "label": "Save As"},
        null,
        {"id": "Help"},
        {"id": "About", "label": "About Adobe CVG Viewer..."}
    ]
}}
)json";

void iterateParser(json::parser test);

void iterateArray(json::objectbase array)
{
    for (auto iter = array.getArrayFirst(); iter.isValid(); iter.nextObject()) {
        switch (iter.type) {
        case json::type::string:
            std::cout << ": " << iter.getString() << std::endl;
            break;
        case json::type::number:
            std::cout << ": " << iter.getNumber<float>() << std::endl;
            break;
        case json::type::boolean:
            std::cout << ": " << (iter.getBoolean() ? "true" : "false") << std::endl;
            break;
        case json::type::object:
            std::cout << ": " << std::endl << "--------" << std::endl;
            iterateParser(iter.getObject());
            std::cout << "--------" << std::endl;
            break;
        case json::type::array:
            std::cout << ": " << std::endl << "========" << std::endl;
            iterateArray(iter);
            std::cout << "========" << std::endl;
            break;
        default:
            std::cout << "? " << iter.value << std::endl;
            break;
        }
    }
}

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
        case json::type::array:
            std::cout << object->name << ": " << std::endl << "========" << std::endl;
            iterateArray(*object);
            std::cout << "========" << std::endl;
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

