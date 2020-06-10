#ifndef MINJSON_OBJECTBASE_HPP_
#define MINJSON_OBJECTBASE_HPP_

#include "type.hpp"

#include <cctype>
#include <optional>
#include <string_view>
#include <type_traits>

namespace minjson
{
    template<typename T>
    concept numeric = !std::is_same_v<T, bool> and std::is_arithmetic_v<T>;

    class parser;
    struct arrayobject;

    /**
     * Defines the base for a JSON object, specifying the object's type and value as a string.
     */
    struct objectbase
    {
        minjson::type type;
        std::string_view value;

        constexpr objectbase() : type(type::null) {}

        template<typename T>
        constexpr std::optional<std::enable_if_t<!numeric<T>, T>> get() const {
            return {};
        }

        template<typename T>
        constexpr std::optional<std::enable_if_t<numeric<T>, T>> get() const {
            T n = 0;
            bool decimal = false;
            int decimalCount = 0;
            for (auto it = value.begin(); it != value.end(); it++) {
                if (isdigit(*it)) {
                    n = n * 10 + (*it - '0');
                    if (decimal)
                        decimalCount++;
                } else if (*it == '.') {
                    if (std::is_floating_point_v<T>)
                        decimal = true;
                    else
                        break; // Better than failing
                }
            }

            for (; decimalCount > 0; decimalCount--)
                n /= 10;
            return n;
        }

        template<>
        constexpr std::optional<std::string_view> get<std::string_view>() const {
            if (type == type::string)
                return value.substr(1, value.size() - 2);
            else
                return {};
        }

        template<>
        constexpr std::optional<bool> get<bool>() const {
            if (type == type::boolean)
               return value == "true";
            else
                return {};
        }

        constexpr parser getObject() const;
        constexpr arrayobject getArrayFirst() const;
    };
}

#include "arrayobject.hpp"
#include "parser.hpp"

namespace minjson
{
    constexpr parser objectbase::getObject(void) const
    {
        parser p;
        if (type == type::object)
            p.start(value);
        return p;
    }

    constexpr arrayobject objectbase::getArrayFirst(void) const
    {
        return arrayobject(value.substr(1));
    }
}


#endif // MINJSON_OBJECTBASE_HPP_

