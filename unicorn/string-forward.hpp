// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <string>

namespace Unicorn {

    template <typename C>
    basic_string<C> str_repeat(const basic_string<C>& str, size_t n) {
        using string_type = basic_string<C>;
        if (n == 0 || str.empty())
            return {};
        if (n == 1)
            return str;
        if (str.size() == 1)
            return string_type(n, str[0]);
        size_t size = n * str.size();
        auto dst = str;
        dst.reserve(size);
        while (dst.size() <= size / 2)
            dst += dst;
        dst += str_repeat(str, n - dst.size() / str.size());
        return dst;
    }

    template <typename C>
    basic_string<C> str_repeat(const C* str, size_t n) {
        return str_repeat(cstr(str), n);
    }

    template <typename C>
    basic_string<C> str_chars(char32_t c, size_t n = 1) {
        using namespace UnicornDetail;
        if (n == 0)
            return {};
        basic_string<C> str(UtfEncoding<C>::max_units, static_cast<C>(0));
        auto len = UtfEncoding<C>::encode(c, &str[0]);
        if (len == 0)
            len = UtfEncoding<C>::encode(replacement_char, &str[0]);
        str.resize(len);
        return n == 1 ? str : str_repeat(str, n);
    }

}
