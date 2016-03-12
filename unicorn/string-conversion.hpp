// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-property.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace Unicorn {

    namespace UnicornDetail {

        template <typename T, typename C>
        UtfIterator<C> convert_str_to_int(T& t, const UtfIterator<C>& start, uint32_t flags, int base) {
            static const auto dec_chars = recode<C>(u8string("+-0123456789"));
            static const auto hex_chars = recode<C>(u8string("+-0123456789ABCDEFabcdef"));
            const auto& src(start.source());
            size_t offset = start.offset();
            if (offset >= src.size()) {
                if (flags & err_throw)
                    throw std::invalid_argument("Invalid integer (empty string)");
                t = T(0);
                return utf_end(src);
            }
            size_t endpos = src.find_first_not_of(base == 16 ? hex_chars : dec_chars, offset);
            if (endpos == offset) {
                if (flags & err_throw)
                    throw std::invalid_argument("Invalid integer: " + quote(to_utf8(start.str())));
                t = T(0);
                return start;
            }
            if (endpos == npos)
                endpos = src.size();
            u8string fragment(endpos - offset, '\0');
            std::transform(src.begin() + offset, src.begin() + endpos, fragment.begin(),
                [] (C c) { return char(c); });
            UtfIterator<C> stop;
            if (std::is_signed<T>::value) {
                static constexpr auto min_value = static_cast<long long>(std::numeric_limits<T>::min());
                static constexpr auto max_value = static_cast<long long>(std::numeric_limits<T>::max());
                char* endptr = nullptr;
                errno = 0;
                long long value = strtoll(fragment.data(), &endptr, base);
                int err = errno;
                size_t len = endptr - fragment.data();
                stop = utf_iterator(src, offset + len);
                if (len == 0) {
                    if (flags & err_throw)
                        throw std::invalid_argument("Invalid integer: " + quote(fragment));
                    t = T(0);
                } else if (err == ERANGE || value < min_value || value > max_value) {
                    if (flags & err_throw)
                        throw std::range_error("Integer out of range: " + quote(fragment));
                    t = sign_of(value) == 1 ? max_value : min_value;
                } else {
                    t = value;
                }
            } else {
                static constexpr auto max_value = static_cast<unsigned long long>(std::numeric_limits<T>::max());
                char* endptr = nullptr;
                errno = 0;
                unsigned long long value = strtoull(fragment.data(), &endptr, base);
                int err = errno;
                size_t len = endptr - fragment.data();
                stop = utf_iterator(src, offset + len);
                if (len == 0) {
                    if (flags & err_throw)
                        throw std::invalid_argument("Invalid integer: " + quote(fragment));
                    t = T(0);
                } else if (err == ERANGE || value > max_value) {
                    if (flags & err_throw)
                        throw std::range_error("Integer out of range: " + quote(fragment));
                    t = max_value;
                } else {
                    t = value;
                }
            }
            return stop;
        }

        template <typename T>
        struct FloatConversionTraits {
            static constexpr long double huge_val = HUGE_VALL;
            static long double str_to_t(const char* p, char** ep) noexcept { return strtold(p, ep); }
        };

        template <>
        struct FloatConversionTraits<float> {
            static constexpr float huge_val = HUGE_VALF;
            static float str_to_t(const char* p, char** ep) noexcept { return strtof(p, ep); }
        };

        template <>
        struct FloatConversionTraits<double> {
            static constexpr double huge_val = HUGE_VAL;
            static double str_to_t(const char* p, char** ep) noexcept { return strtod(p, ep); }
        };

    }

    template <typename T, typename C>
    size_t str_to_int(T& t, const basic_string<C>& str, size_t offset = 0, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, utf_iterator(str, offset), flags, 10).offset() - offset;
    }

    template <typename T, typename C>
    UtfIterator<C> str_to_int(T& t, const UtfIterator<C>& start, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, start, flags, 10);
    }

    template <typename T, typename C>
    T str_to_int(const basic_string<C>& str, uint32_t flags = 0) {
        T t = T(0);
        UnicornDetail::convert_str_to_int<T>(t, utf_begin(str), flags, 10);
        return t;
    }

    template <typename T, typename C>
    T str_to_int(const UtfIterator<C>& start, uint32_t flags = 0) {
        T t = T(0);
        UnicornDetail::convert_str_to_int<T>(t, start, flags, 10);
        return t;
    }

    template <typename T, typename C>
    size_t hex_to_int(T& t, const basic_string<C>& str, size_t offset = 0, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, utf_iterator(str, offset), flags, 16).offset() - offset;
    }

    template <typename T, typename C>
    UtfIterator<C> hex_to_int(T& t, const UtfIterator<C>& start, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, start, flags, 16);
    }

    template <typename T, typename C>
    T hex_to_int(const basic_string<C>& str, uint32_t flags = 0) {
        T t = T(0);
        UnicornDetail::convert_str_to_int<T>(t, utf_begin(str), flags, 16);
        return t;
    }

    template <typename T, typename C>
    T hex_to_int(const UtfIterator<C>& start, uint32_t flags = 0) {
        T t = T(0);
        UnicornDetail::convert_str_to_int<T>(t, start, flags, 16);
        return t;
    }

    template <typename T, typename C>
    UtfIterator<C> str_to_float(T& t, const UtfIterator<C>& start, uint32_t flags = 0) {
        using traits = UnicornDetail::FloatConversionTraits<T>;
        static const auto float_chars = recode<C>(u8string("+-.0123456789Ee"));
        static constexpr T max_value = std::numeric_limits<T>::max();
        const auto& src(start.source());
        size_t offset = start.offset();
        if (offset >= src.size()) {
            if (flags & err_throw)
                throw std::invalid_argument("Invalid number (empty string)");
            t = T(0);
            return utf_end(src);
        }
        size_t endpos = src.find_first_not_of(float_chars, offset);
        if (endpos == offset) {
            if (flags & err_throw)
                throw std::invalid_argument("Invalid number: " + quote(to_utf8(start.str())));
            t = T(0);
            return start;
        }
        if (endpos == npos)
            endpos = src.size();
        u8string fragment(endpos - offset, 0);
        std::transform(src.begin() + offset, src.begin() + endpos, fragment.begin(),
            [] (C c) { return char(c); });
        char* endptr = nullptr;
        T value = traits::str_to_t(fragment.data(), &endptr);
        size_t len = endptr - fragment.data();
        auto stop = utf_iterator(src, offset + len);
        if (len == 0) {
            if (flags & err_throw)
                throw std::invalid_argument("Invalid number: " + quote(fragment));
            t = T(0);
        } else if (value == traits::huge_val || value == - traits::huge_val) {
            if (flags & err_throw)
                throw std::range_error("Number out of range: " + quote(fragment));
            t = value > T(0) ? max_value : - max_value;
        } else {
            t = value;
        }
        return stop;
    }

    template <typename T, typename C>
    size_t str_to_float(T& t, const basic_string<C>& str, size_t offset = 0, uint32_t flags = 0) {
        return str_to_float(t, utf_iterator(str, offset), flags).offset() - offset;
    }

    template <typename T, typename C>
    T str_to_float(const basic_string<C>& str, uint32_t flags = 0) {
        T t = T(0);
        str_to_float(t, utf_begin(str), flags);
        return t;
    }

    template <typename T, typename C>
    T str_to_float(const UtfIterator<C>& start, uint32_t flags = 0) {
        T t = T(0);
        str_to_float(t, start, flags);
        return t;
    }

}
