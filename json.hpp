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

    class parser;
    class arrayobject;

    struct objectbase
    {
        json::type type;
        std::string_view value;

        std::string_view getString(void) const
        {
            if (type != type::string)
                return "";
            return value.substr(1, value.size() - 2);
        }

        bool getBoolean(void) const
        {
            if (type != type::boolean)
                return false;
            return value == "true";
        }

        template<typename T>
        T getNumber(void) const
        {
            if (type != type::number)
                return 0;

            T n = 0;
            bool decimal = false;
            int decimalCount = 0;
            for (auto it = value.begin(); it != value.end(); it++) {
                if (isdigit(*it)) {
                    n = n * 10 + (*it - '0');
                    if (decimal)
                        decimalCount++;
                } else if (*it == '.') {
                    if constexpr (std::is_floating_point<T>::value)
                        decimal = true;
                    else
                        break;
                }
            }

            for (; decimalCount > 0; decimalCount--)
                n /= 10;
            return n;
        }

        parser getObject(void) const;
        arrayobject getArrayFirst(void) const;
    };

    struct arrayobject : public objectbase
    {
    private:
        constexpr static auto npos = std::string_view::npos;

    public:
        std::string_view whole;
        std::size_t index;
        bool valid;

        arrayobject(std::string_view _whole)
            : whole(_whole), index(0)
        {
            nextObject();
        }

        bool isValid(void) const
        {
            return valid;
        }

        arrayobject& nextObject(void);
    };

    struct object : public objectbase
    {
        std::string_view name;
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
                        auto pair = determineType(body.substr(valueStart + 1));
                        if (!pair)
                            return {};
                        o.type = pair->first;
                        o.value = body.substr(valueStart + 2,
                                              pair->second - 1);
                        index = body.find_first_not_of(" \t\n\r", valueStart + 1 + pair->second);
                        if (index == npos || body[index] != ',')
                            ready = false;

                        return o;
                    }
                }
            }

            return {};
        }

        static std::optional<std::pair<type, std::size_t>> determineType(std::string_view val)
        {
            std::pair<type, std::size_t> result;
            auto valueStart = val.find_first_not_of(" \t\r\n");
            if (valueStart != npos) {
                char c = val[valueStart];
                if (c == '\"') {
                    for (valueStart++;
                         valueStart < val.size() &&
                         (val[valueStart] != '\"' || val[valueStart - 1] == '\\');
                         valueStart++);
                    if (valueStart >= val.size())
                        return {};
                    result = {type::string, valueStart + 1};
                } else if (c == '-' || isdigit(c)) {
                    while (isdigit(val[++valueStart]) || val[valueStart] == '.');
                    return result = {type::number, valueStart};
                } else if (c == '{') {
                    valueStart++;
                    int b;
                    for (b = 0; valueStart < val.size() && b >= 0; valueStart++) {
                        if (val[valueStart] == '{')
                            b++;
                        else if (val[valueStart] == '}')
                            b--;
                    }
                    if (b != -1)
                        return {};
                    result = {type::object, valueStart};
                } else if (c == '[') {
                    valueStart++;
                    int b;
                    for (b = 0; valueStart < val.size() && b >= 0; valueStart++) {
                        if (val[valueStart] == '[')
                            b++;
                        else if (val[valueStart] == ']')
                            b--;
                    }
                    if (b != -1)
                        return {};
                    result = {type::array, valueStart};
                } else {
                    // Boolean or null
                    if (val.compare(valueStart, 4, "true") == 0)
                        result = {type::boolean, valueStart + 4};
                    else if (val.compare(valueStart, 5, "false") == 0)
                        result = {type::boolean, valueStart + 5};
                    else if (val.compare(valueStart, 4, "null") == 0)
                        result = {type::null, valueStart + 4};
                    else
                        return {};
                }
            }

            return result;
        }
    };

    parser objectbase::getObject(void) const
    {
        parser p;
        if (type == type::object)
            p.start(value);
        return p;
    }

    arrayobject objectbase::getArrayFirst(void) const
    {
        return arrayobject(value.substr(1));
    }

    arrayobject& arrayobject::nextObject(void)
    {
        if (index == npos) {
            valid = false;
        } else if (auto pair = parser::determineType(whole.substr(index)); pair) {
            std::size_t next;
            std::tie(type, next) = *pair;
            value = whole.substr(index, next);
            index = whole.find_first_not_of(" \t\n\r", index + next + 1);
            if (index != npos && whole[index] == ']')
                index = npos;
        } else {
            valid = false;
        }

        return *this;
    }
}

#endif // JSON_HPP_

