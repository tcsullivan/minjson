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
    /**
     * Provides the functionality to parse a string of JSON data.
     */
    class parser
    {
    private:
        bool ready; // Set 'true' if data is available
        std::size_t index; // Index within the JSON data
        std::string_view body; // Contains the 'body' of the JSON data

        constexpr static auto npos = std::string_view::npos;
    
    public:
        constexpr parser() : ready(false), index(0) {}
    
        /**
         * Starts the parser with a given string of JSON data.
         * @param jstr String containing JSON data
         * @return True if ready and able to parse the data
         */
        constexpr bool start(std::string_view jstr) {
            auto from = jstr.find_first_not_of(" \t\r\n");
            if (from != npos && jstr[from] == '{') {
                auto to = jstr.find_last_not_of(" \t\r\n");

                ready = to != npos && jstr[to] == '}';
                if (ready) {
                    index = 0;
                    body = jstr.substr(from + 1, to - from - 1);
                }
            } else {
                ready = false;
            }
    
            return ready;
        }
        
        /**
         * Tells if the parser is 'ready'; that is, data is available.
         */
        constexpr bool isReady() const {
            return ready;
        }

        /**
         * Rewinds the parser to the beginning of the last given JSON data.
         */
        constexpr void rewind() {
            index = 0;
            ready = true;
        }

        /**
         * Attempts to read the next data object within the JSON data.
         * @return The next object, or nothing on error
         */
        constexpr std::optional<object> getNextObject() {
            if (!ready)
                return {};

            // Search for the name field (key of the key-value pair)
            if (auto nameStart = body.find('\"', index); nameStart != npos) {
                if (auto nameEnd = body.find('\"', nameStart + 1);
                    nameEnd != npos && nameEnd > nameStart)
                {
                    // Name found; next, find the value
                    if (auto valueStart = body.find(':', nameEnd + 1);
                        valueStart != npos)
                    {
                        // Construct the object
                        object o;
                        o.name = body.substr(nameStart + 1, nameEnd - nameStart - 1);
                        auto pair = determineType(body.substr(valueStart + 1));
                        if (!pair)
                            return {};
                        o.type = pair->first;
                        o.value = body.substr(valueStart + 2,
                                              pair->second - 1);

                        // Advance index to next object, or ready = false if
                        // this is the end.
                        index = body.find_first_not_of(" \t\n\r", valueStart + 1 + pair->second);
                        if (index == npos || body[index] != ',')
                            ready = false;

                        return o;
                    }
                }
            }

            return {};
        }

        /**
         * Attempts to determine the type of the given JSON value data.
         * @param val Value string to analyze
         * @return A pair of the type and data's size, or nothing on error
         */
        constexpr static std::optional<std::pair<type, std::size_t>>
            determineType(std::string_view val)
        {
            std::pair<type, std::size_t> result;

            if (auto valueStart = val.find_first_not_of(" \t\r\n");
                valueStart != npos)
            {
                char c = val[valueStart];

                if (c == '\"') {
                    // Should be a string: find the terminating quote
                    for (valueStart++; valueStart < val.size(); valueStart++) {
                        if (val[valueStart] == '\"' && val[valueStart - 1] != '\\')
                            break;
                    }

                    if (valueStart >= val.size())
                        return {};

                    result = {type::string, valueStart + 1};
                } else if (isdigit(c) || c == '-' || c == '.') {
                    // Number: find end of number
                    while (isdigit(val[++valueStart]) || val[valueStart] == '.');
                    result = {type::number, valueStart};
                } else if (c == '{') {
                    // Object: Find end of object
                    valueStart++;
                    int nestCount = 0;
                    bool quote = false;
                    for (; valueStart < val.size() && nestCount >= 0; valueStart++) {
                        if (val[valueStart] == '\"' && val[valueStart - 1] != '\\') {
                            // Enter or exit from quotation
                            quote ^= true;
                        } else if (!quote) {
                            // Change nestCount if entering/exiting layer of {}
                            if (val[valueStart] == '{')
                                nestCount++;
                            else if (val[valueStart] == '}')
                                nestCount--;
                        }
                    }

                    // Object opened with nestCount = 0; nestCount will become
                    // -1 if the object's {} is closed before the file's end.
                    if (nestCount != -1)
                        return {};

                    result = {type::object, valueStart};
                } else if (c == '[') {
                    // Array: find the end
                    valueStart++;
                    int nestCount = 0;
                    bool quote = false;
                    for (; valueStart < val.size() && nestCount >= 0; valueStart++) {
                        if (val[valueStart] == '\"' && val[valueStart - 1] != '\\') {
                            quote ^= true;
                        } else if (!quote) {
                            if (val[valueStart] == '[')
                                nestCount++;
                            else if (val[valueStart] == ']')
                                nestCount--;
                        }
                    }

                    if (nestCount != -1)
                        return {};

                    result = {type::array, valueStart};
                } else {
                    // Object is either boolean, null, or invalid JSON
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

