/**
 * Copyright (C) 2019 Clyne Sullivan
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
#ifndef JSON_HPP_
#define JSON_HPP_

#include <cctype>
#include <optional>
#include <string_view>
#include <tuple>

namespace json
{
    enum class type
    {
        string,
        number,
        object,
        array,
        boolean,
        null
    };

    struct object
    {
        std::string_view name;
        json::type type;
        std::size_t valueIndex;
    };
    
    class parser
    {
    private:
        bool ready;
        std::string_view body;
        std::size_t index;

        constexpr static auto npos = std::string_view::npos;
    
    public:
        explicit parser(void)
            : ready(false) {}
    
        bool start(std::string_view s) {
            auto from = s.find_first_not_of(" \t\r\n");
            if (from != npos && s[from] == '{') {
                auto to = s.find_last_not_of(" \t\r\n");
                if (to != npos && s[to] == '}') {
                    ready = true;
                    body = s.substr(from + 1, to - from - 1);
                    index = 0;
                } else {
                    ready = false;
                }
            } else {
                ready = false;
            }
    
            return ready;
        }
        
        bool isReady(void) const {
            return ready;
        }

        std::optional<object> getNextObject(void) {
            if (!ready)
                return {};

            if (auto nameStart = body.find('\"', index); nameStart != npos) {
                if (auto nameEnd = body.find('\"', nameStart + 1);
                    nameEnd != npos && nameEnd > nameStart)
                {
                    if (auto valueStart = body.find(':', nameEnd + 1);
                        valueStart != npos)
                    {
                        object o;
                        o.name = body.substr(nameStart + 1,
                                             nameEnd - nameStart - 1);
                        auto pair = determineType(valueStart + 1);
                        if (!pair)
                            return {};
                        o.type = pair->first;
                        o.valueIndex = valueStart + 1;

                        index = body.find_first_not_of(" \t\n\r", pair->second);
                        if (index == npos || body[index] != ',')
                            ready = false;

                        return o;
                    }
                }
            }

            return {};
        }

        std::optional<std::pair<type, std::size_t>> determineType(std::size_t i)
        {
            std::pair<type, std::size_t> result;
            auto valueStart = body.find_first_not_of(" \t\r\n", i);
            if (valueStart != npos) {
                char c = body[valueStart];
                if (c == '\"') {
                    for (valueStart++;
                         valueStart < body.size() &&
                         (body[valueStart] != '\"' || body[valueStart - 1] == '\\');
                         valueStart++);
                    if (valueStart >= body.size())
                        return {};
                    result = {type::string, valueStart + 1};
                } else if (c == '-' || isdigit(c)) {
                    while (isdigit(body[++valueStart]) || body[valueStart] == '.');
                    return result = {type::number, valueStart};
                } else if (c == '{') {
                    valueStart++;
                    int b;
                    for (b = 0; valueStart < body.size() && b >= 0; valueStart++) {
                        if (body[valueStart] == '{')
                            b++;
                        else if (body[valueStart] == '}')
                            b--;
                    }
                    if (b != -1)
                        return {};
                    result = {type::object, valueStart};
                } else if (c == '[') {
                    valueStart++;
                    int b;
                    for (b = 0; valueStart < body.size() && b >= 0; valueStart++) {
                        if (body[valueStart] == '[')
                            b++;
                        else if (body[valueStart] == ']')
                            b--;
                    }
                    if (b != -1)
                        return {};
                    result = {type::array, valueStart};
                } else {
                    // Boolean or null
                    if (body.compare(valueStart, 4, "true") == 0)
                        result = {type::boolean, valueStart + 4};
                    else if (body.compare(valueStart, 5, "false") == 0)
                        result = {type::boolean, valueStart + 5};
                    else if (body.compare(valueStart, 4, "null") == 0)
                        result = {type::null, valueStart + 4};
                    else
                        return {};
                }
            }

            return result;
        }
    };
}

#endif // JSON_HPP_
