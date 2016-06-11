// Internal to the library, do not include this directly

// This header contains the string functions that are used extensively in
// string submodules other than the one they are defined in.

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
    basic_string<C> str_char(char32_t c) {
        using namespace UnicornDetail;
        basic_string<C> str(UtfEncoding<C>::max_units, C(0));
        auto len = UtfEncoding<C>::encode(c, &str[0]);
        if (len == 0)
            len = UtfEncoding<C>::encode(replacement_char, &str[0]);
        str.resize(len);
        return str;
    }

    template <typename C>
    basic_string<C> str_chars(size_t n, char32_t c) {
        using namespace UnicornDetail;
        if (n == 0)
            return {};
        basic_string<C> str(UtfEncoding<C>::max_units, C(0));
        auto len = UtfEncoding<C>::encode(c, &str[0]);
        if (len == 0)
            len = UtfEncoding<C>::encode(replacement_char, &str[0]);
        str.resize(len);
        return str_repeat(str, n);
    }

    template <typename C, typename C2>
    void str_append(basic_string<C>& str, const basic_string<C2>& suffix) {
        std::copy(utf_begin(suffix), utf_end(suffix), utf_writer(str));
    }

    template <typename C>
    void str_append(basic_string<C>& str, const basic_string<C>& suffix) {
        str += suffix;
    }

    template <typename C, typename C2>
    void str_append(basic_string<C>& str, const UtfIterator<C2>& suffix_begin, const UtfIterator<C2>& suffix_end) {
        std::copy(suffix_begin, suffix_end, utf_writer(str));
    }

    template <typename C>
    void str_append(basic_string<C>& str, const UtfIterator<C>& suffix_begin, const UtfIterator<C>& suffix_end) {
        str.append(suffix_begin.source(), suffix_begin.offset(), suffix_end.offset() - suffix_begin.offset());
    }

    template <typename C, typename C2>
    void str_append(basic_string<C>& str, const Irange<UtfIterator<C2>>& suffix) {
        str_append(str, suffix.begin(), suffix.end());
    }

    template <typename C>
    void str_append(basic_string<C>& str, const Irange<UtfIterator<C>>& suffix) {
        str_append(str, suffix.begin(), suffix.end());
    }

    template <typename C, typename C2>
    void str_append(basic_string<C>& str, const C2* suffix) {
        str_append(str, cstr(suffix));
    }

    template <typename C>
    void str_append(basic_string<C>& str, const C* suffix) {
        if (suffix)
            str += suffix;
    }

    template <typename C, typename C2>
    void str_append(basic_string<C>& dst, const C2* ptr, size_t n) {
        if (ptr)
            for (auto out = utf_writer(dst); n > 0; --n, ++ptr)
                *out = char_to_uint(*ptr++);
    }

    template <typename C>
    void str_append(basic_string<C>& dst, const C* ptr, size_t n) {
        if (ptr)
            dst.append(ptr, n);
    }

    template <typename C, typename C2>
    void str_append_char(basic_string<C>& dst, C2 c) {
        *utf_writer(dst) = char_to_uint(c);
    }

    template <typename C>
    void str_append_char(basic_string<C>& dst, C c) {
        dst += c;
    }

    template <typename C, typename C2, typename C3, typename... Chars>
    void str_append_char(basic_string<C>& dst, C2 c2, C3 c3, Chars... chars) {
        str_append_char(dst, c2);
        str_append_char(dst, c3, chars...);
    }

    template <typename C>
    void str_append_chars(basic_string<C>& dst, size_t n, char32_t c) {
        auto s = str_char<C>(c);
        if (s.size() == 1)
            dst.append(n, s[0]);
        else
            dst += str_repeat(s, n);
    }

}
