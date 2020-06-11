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
#ifndef MINJSON_OBJECT_HPP_
#define MINJSON_OBJECT_HPP_

#include "objectbase.hpp"

#include <string_view>

namespace minjson
{
    /**
     * Structure to represent a JSON object, simply adds a 'name' field to
     * complete the name-value pair structure used by JSON.
     */
    class object : public objectbase
    {
    private:
        std::string_view m_name;

    public:
        constexpr object(std::string_view name = {},
                         minjson::type type = minjson::type::null,
                         std::string_view value = {}) :
            objectbase(type, value), m_name(name) {}

        constexpr std::string_view name() const {
            return m_name;
        }
    };
}

#endif // MINJSON_OBJECT_HPP_

