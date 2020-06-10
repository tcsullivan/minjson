#ifndef MINJSON_ARRAYOBJECT_HPP_
#define MINJSON_ARRAYOBJECT_HPP_

#include "objectbase.hpp"

#include <string_view>

namespace minjson
{
    struct arrayobject : public objectbase
    {
    private:
        constexpr static auto npos = std::string_view::npos;

    public:
        std::string_view whole;
        std::size_t index;
        bool valid;

        constexpr arrayobject(std::string_view _whole)
            : whole(_whole), index(0), valid(false)
        {
            nextObject();
        }

        constexpr bool isValid() const {
            return valid;
        }

        constexpr void rewind() {
            index = 0;
            nextObject();
        }

        constexpr arrayobject& nextObject(void);
    };
}

#include "parser.hpp"

namespace minjson
{
    constexpr arrayobject& arrayobject::nextObject(void)
    {
        if (index == npos) {
            valid = false;
        } else if (auto pair = parser::determineType(whole.substr(index)); pair) {
            std::size_t next = 0;
            std::tie(type, next) = *pair;
            value = whole.substr(index, next);
            index = whole.find_first_not_of(" \t\n\r", index + next + 1);
            if (index != npos && whole[index] == ']')
                index = npos;
            valid = true;
        } else {
            valid = false;
        }

        return *this;
    }
}

#endif // MINJSON_ARRAYOBJECT_HPP_

