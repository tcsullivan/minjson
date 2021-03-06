/**
 * Copyright (C) 2020 Clyne Sullivan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef MINJSON_OBJECTBASE_HPP_
#define MINJSON_OBJECTBASE_HPP_

#include "type.hpp"

#include <cctype>
#include <optional>
#include <string_view>
#include <type_traits>

namespace minjson
{
    /**
     * A numeric concept, for specializing objectbase::get for integers/floating-
     * points (and not bools).
     */
    template<typename T>
    concept numeric = !std::is_same_v<T, bool> and std::is_arithmetic_v<T>;

    class parser;
    class arrayobject;

    /**
     * Defines the base for a JSON object, specifying the object's type and value as a string.
     */
    class objectbase
    {
    protected:
        minjson::type m_type;
        std::string_view m_value;

    public:
        constexpr objectbase(minjson::type type = minjson::type::null,
                             std::string_view value = {}) :
            m_type(type), m_value(value) {}

        constexpr minjson::type type() const {
            return m_type;
        }

        /**
         * Default get() for reading the object's data. Returns nothing.
         */
        template<typename T>
        constexpr std::optional<std::enable_if_t<!numeric<T>, T>> get() const {
            return {};
        }

        /**
         * get() for number data, accepting both integers and floating-points.
         */
        template<typename T>
        constexpr std::optional<std::enable_if_t<numeric<T>, T>> get() const {
            if (m_type != type::number)
                return {};

            T n = 0;
            bool decimal = false;
            int decimalCount = 0;
            for (auto c : m_value) {
                if (isdigit(c)) {
                    n = n * 10 + (c - '0');
                    if (decimal)
                        decimalCount++;
                } else if (c == '.') {
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

        /**
         *  get() for string data. Trims off the quotes surrounding the string.
         */
        template<>
        constexpr std::optional<std::string_view> get<std::string_view>() const {
            if (m_type == type::string)
                return m_value.substr(1, m_value.size() - 2);
            else
                return {};
        }

        /**
         * get() for bool data.
         */
        template<>
        constexpr std::optional<bool> get<bool>() const {
            if (m_type == type::boolean)
               return m_value == "true";
            else
                return {};
        }

        /**
         * Returns a parser initialized with this object's data, should it have
         * the object type.
         */
        constexpr std::optional<parser> getObject() const;

        /**
         * Gets an arrayobject for the first entry in this object's array,
         * should this object be an array.
         */
        constexpr std::optional<arrayobject> getArrayFirst() const;
    };
}

#include "arrayobject.hpp"
#include "parser.hpp"

namespace minjson
{
    constexpr std::optional<parser> objectbase::getObject() const
    {
        if (m_type == type::object) {
            parser p;
            p.start(m_value);
            return p;
        } else {
            return {};
        }
    }

    constexpr std::optional<arrayobject> objectbase::getArrayFirst() const
    {
        if (m_type == type::array)
            return arrayobject(m_value.substr(1));
        else
            return {};
    }
}


#endif // MINJSON_OBJECTBASE_HPP_

