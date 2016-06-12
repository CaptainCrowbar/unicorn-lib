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

    inline void str_append(u8string& str, const u8string& suffix) { str += suffix; }
    void str_append(u8string& str, const Utf8Iterator& suffix_begin, const Utf8Iterator& suffix_end);
    void str_append(u8string& str, const Irange<Utf8Iterator>& suffix);
    void str_append(u8string& str, const char* suffix);
    void str_append(u8string& dst, const char* ptr, size_t n);
    inline void str_append_char(u8string& dst, char c) { dst += c; }
    void str_append_chars(u8string& dst, size_t n, char32_t c);
    u8string str_char(char32_t c);
    u8string str_chars(size_t n, char32_t c);
    u8string str_repeat(const u8string& str, size_t n);
    void str_repeat_in(u8string& str, size_t n);

    template <typename C>
    void str_append(u8string& str, const basic_string<C>& suffix) {
        std::copy(utf_begin(suffix), utf_end(suffix), utf_writer(str));
    }

    template <typename C>
    void str_append(u8string& str, const UtfIterator<C>& suffix_begin, const UtfIterator<C>& suffix_end) {
        std::copy(suffix_begin, suffix_end, utf_writer(str));
    }

    template <typename C>
    void str_append(u8string& str, const Irange<UtfIterator<C>>& suffix) {
        str_append(str, suffix.begin(), suffix.end());
    }

    template <typename C>
    void str_append(u8string& str, const C* suffix) {
        str_append(str, cstr(suffix));
    }

    template <typename C>
    void str_append(u8string& dst, const C* ptr, size_t n) {
        if (ptr)
            for (auto out = utf_writer(dst); n > 0; --n, ++ptr)
                *out = char_to_uint(*ptr++);
    }

    template <typename C>
    void str_append_char(u8string& dst, C c) {
        *utf_writer(dst) = char_to_uint(c);
    }

    template <typename C1, typename C2, typename... Chars>
    void str_append_char(u8string& dst, C1 c1, C2 c2, Chars... chars) {
        str_append_char(dst, c1);
        str_append_char(dst, c2, chars...);
    }

}
