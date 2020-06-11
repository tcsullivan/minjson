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
    class arrayobject : public objectbase
    {
    private:
        constexpr static auto npos = std::string_view::npos;

        std::string_view m_whole;
        std::size_t m_index;
        bool m_valid;

    public:
        constexpr arrayobject(std::string_view whole)
            : m_whole(whole), m_index(0), m_valid(false)
        {
            // Load the first entry
            next();
        }

        constexpr bool valid() const {
            return m_valid;
        }

        constexpr void rewind() {
            m_index = 0;
            next();
        }

        constexpr arrayobject& next();
    };
}

#include "parser.hpp"

namespace minjson
{
    constexpr arrayobject& arrayobject::next()
    {
        if (m_index == npos) {
            m_valid = false;
        } else if (auto pair = parser::determineType(m_whole.substr(m_index)); pair) {
            // Found next entry in array; become it
            std::size_t next;
            std::tie(m_type, next) = *pair;
            m_value = m_whole.substr(m_index, next);
            m_index = m_whole.find_first_not_of(" \t\n\r", m_index + next + 1);

            if (m_index != npos && m_whole[m_index] == ']')
                m_index = npos;
            m_valid = true;
        } else {
            m_valid = false;
        }

        return *this;
    }
}

#endif // MINJSON_ARRAYOBJECT_HPP_

