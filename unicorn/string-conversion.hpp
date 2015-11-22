// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-property.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>

namespace Unicorn {

    namespace UnicornDetail {

        template <typename T, typename C>
        size_t string_to_integer(T& t, const basic_string<C>& str, size_t offset, int base) {
            PRI_STATIC_ASSERT(std::is_integral<T>::value);
            static const auto dec_chars = recode<C>(u8string("+-0123456789"));
            static const auto hex_chars = recode<C>(u8string("+-0123456789ABCDEFabcdef"));
            t = static_cast<T>(0);
            if (offset > str.size())
                return 0;
            size_t endpos = str.find_first_not_of(base == 16 ? hex_chars : dec_chars, offset);
            if (endpos == offset)
                return 0;
            if (endpos == npos)
                endpos = str.size();
            u8string fragment(endpos - offset, 0);
            std::transform(str.begin() + offset, str.begin() + endpos, fragment.begin(),
                [] (C c) { return char(c); });
            char* endptr = nullptr;
            if (std::is_signed<T>::value) {
                auto value = strtoll(fragment.data(), &endptr, base);
                value = clamp(value, std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
                t = static_cast<T>(value);
            } else {
                auto value = strtoull(fragment.data(), &endptr, base);
                value = clamp(value, 0, std::numeric_limits<T>::max());
                t = static_cast<T>(value);
            }
            return endptr - fragment.data();
        }

    }

    template <typename T, typename C>
    size_t str_to_integer(T& t, const basic_string<C>& str, size_t offset = 0) {
        return UnicornDetail::string_to_integer<T>(t, str, offset, 10);
    }

    template <typename T, typename C>
    T str_to_integer(const basic_string<C>& str, size_t offset = 0) {
        auto t = static_cast<T>(0);
        UnicornDetail::string_to_integer<T>(t, str, offset, 10);
        return t;
    }

    template <typename T, typename C>
    size_t hex_to_integer(T& t, const basic_string<C>& str, size_t offset = 0) {
        return UnicornDetail::string_to_integer<T>(t, str, offset, 16);
    }

    template <typename T, typename C>
    T hex_to_integer(const basic_string<C>& str, size_t offset = 0) {
        auto t = static_cast<T>(0);
        UnicornDetail::string_to_integer<T>(t, str, offset, 16);
        return t;
    }

    template <typename T, typename C>
    size_t str_to_real(T& t, const basic_string<C>& str, size_t offset = 0) {
        PRI_STATIC_ASSERT(std::is_floating_point<T>::value);
        static const auto float_chars = recode<C>(u8string("+-.0123456789Ee"));
        t = static_cast<T>(0);
        if (offset > str.size())
            return 0;
        size_t endpos = str.find_first_not_of(float_chars, offset);
        if (endpos == offset)
            return 0;
        if (endpos == npos)
            endpos = str.size();
        u8string fragment(endpos - offset, 0);
        std::transform(str.begin() + offset, str.begin() + endpos, fragment.begin(),
            [] (C c) { return char(c); });
        char* endptr = nullptr;
        auto value = strtold(fragment.data(), &endptr);
        value = clamp(value, - std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
        t = static_cast<T>(value);
        return endptr - fragment.data();
    }

    template <typename T, typename C>
    T str_to_real(const basic_string<C>& str, size_t offset = 0) {
        auto t = static_cast<T>(0);
        str_to_real(t, str, offset);
        return t;
    }

}
