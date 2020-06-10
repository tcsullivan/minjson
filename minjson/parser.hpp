#ifndef MINJSON_PARSER_HPP_
#define MINJSON_PARSER_HPP_

#include "type.hpp"
#include "object.hpp"

#include <cctype>
#include <optional>
#include <string_view>
#include <tuple>

namespace minjson
{
    class parser
    {
    private:
        bool ready;
        std::size_t index;
        std::string_view body;

        constexpr static auto npos = std::string_view::npos;
    
    public:
        constexpr parser(void)
            : ready(false), index(0) {}
    
        constexpr bool start(std::string_view s) {
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
        
        constexpr bool isReady(void) const
        {
            return ready;
        }

        constexpr void rewind(void)
        {
            index = 0;
            ready = true;
        }

        constexpr std::optional<object> getNextObject(void) {
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

        constexpr static std::optional<std::pair<type, std::size_t>> determineType(std::string_view val)
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
                    int b = 0;
                    for (; valueStart < val.size() && b >= 0; valueStart++) {
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
                    int b = 0;
                    for (; valueStart < val.size() && b >= 0; valueStart++) {
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
}

#endif // MINJSON_PARSER_HPP_

