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

#define json minjson

void iterateParser(json::parser test);

void iterateArray(json::objectbase array)
{
    for (auto iter = *array.getArrayFirst(); iter.valid(); iter.next()) {
        switch (iter.type()) {
        case json::type::string:
            std::cout << "string: " << *iter.get<std::string_view>() << std::endl;
            break;
        case json::type::number:
            std::cout << "number: " << *iter.get<double>() << std::endl;
            break;
        case json::type::boolean:
            std::cout << "bool: " << (*iter.get<bool>() ? "true" : "false") << std::endl;
            break;
        case json::type::object:
            std::cout << "object: " << std::endl;
            iterateParser(*iter.getObject());
            break;
        case json::type::array:
            std::cout << "array: " << std::endl;
            iterateArray(iter);
            break;
        case json::type::null:
            std::cout << "null." << std::endl;
            break;
        }
    }
}

void iterateParser(json::parser test)
{
    while (test.ready()) {
        auto object = test.next();
        if (!object)
            break;
        switch (object->type()) {
        case json::type::string:
            std::cout << object->name() << ": " << *object->get<std::string_view>() << std::endl;
            break;
        case json::type::number:
            std::cout << object->name() << ": " << *object->get<double>() << std::endl;
            break;
        case json::type::boolean:
            std::cout << object->name() << ": " << (*object->get<bool>() ? "true" : "false") << std::endl;
            break;
        case json::type::object:
            std::cout << object->name() << ": " << std::endl;
            iterateParser(*object->getObject());
            break;
        case json::type::array:
            std::cout << object->name() << ": " << std::endl;
            iterateArray(*object);
            break;
        case json::type::null:
            std::cout << object->name() << ": null" << std::endl;
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

