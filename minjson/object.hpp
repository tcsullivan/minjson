#ifndef MINJSON_OBJECT_HPP_
#define MINJSON_OBJECT_HPP_

#include "objectbase.hpp"

#include <string_view>

namespace minjson
{
    struct object : public objectbase
    {
        std::string_view name;
    };
}

#endif // MINJSON_OBJECT_HPP_

