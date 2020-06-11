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
#ifndef MINJSON_ARRAYOBJECT_HPP_
#define MINJSON_ARRAYOBJECT_HPP_

#include "objectbase.hpp"

#include <string_view>

namespace minjson
{
    /**
     * Acts as an iterator for entries in an object's contained array.
     */
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
            // Load the first entry
            nextObject();
        }

        constexpr bool isValid() const {
            return valid;
        }

        constexpr void rewind() {
            index = 0;
            nextObject();
        }

        constexpr arrayobject& nextObject();
    };
}

#include "parser.hpp"

namespace minjson
{
    constexpr arrayobject& arrayobject::nextObject()
    {
        if (index == npos) {
            valid = false;
        } else if (auto pair = parser::determineType(whole.substr(index)); pair) {
            // Found next entry in array; become it
            std::size_t next;
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

