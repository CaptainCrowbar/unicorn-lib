// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/utf.hpp"
#include <string>

namespace Unicorn {

    size_t str_common(const u8string& s1, const u8string& s2, size_t start = 0) noexcept;
    size_t str_common_utf(const u8string& s1, const u8string& s2, size_t start = 0) noexcept;
    bool str_expect(Utf8Iterator& i, const Utf8Iterator& end, const u8string& prefix);
    bool str_expect(Utf8Iterator& i, const u8string& prefix);
    Utf8Iterator str_find_char(const Utf8Iterator& b, const Utf8Iterator& e, char32_t c);
    Utf8Iterator str_find_char(const Irange<Utf8Iterator>& range, char32_t c);
    Utf8Iterator str_find_char(const u8string& str, char32_t c);
    Utf8Iterator str_find_last_char(const Utf8Iterator& b, const Utf8Iterator& e, char32_t c);
    Utf8Iterator str_find_last_char(const Irange<Utf8Iterator>& range, char32_t c);
    Utf8Iterator str_find_last_char(const u8string& str, char32_t c);
    Utf8Iterator str_find_first_of(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target);
    Utf8Iterator str_find_first_of(const Irange<Utf8Iterator>& range, const u8string& target);
    Utf8Iterator str_find_first_of(const u8string& str, const u8string& target);
    Utf8Iterator str_find_first_not_of(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target);
    Utf8Iterator str_find_first_not_of(const Irange<Utf8Iterator>& range, const u8string& target);
    Utf8Iterator str_find_first_not_of(const u8string& str, const u8string& target);
    Utf8Iterator str_find_last_of(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target);
    Utf8Iterator str_find_last_of(const Irange<Utf8Iterator>& range, const u8string& target);
    Utf8Iterator str_find_last_of(const u8string& str, const u8string& target);
    Utf8Iterator str_find_last_not_of(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target);
    Utf8Iterator str_find_last_not_of(const Irange<Utf8Iterator>& range, const u8string& target);
    Utf8Iterator str_find_last_not_of(const u8string& str, const u8string& target);
    Utf8Iterator str_search(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target);
    Utf8Iterator str_search(const Irange<Utf8Iterator>& range, const u8string& target);
    Utf8Iterator str_search(const u8string& str, const u8string& target);
    size_t str_skipws(Utf8Iterator& i, const Utf8Iterator& end);
    size_t str_skipws(Utf8Iterator& i);

}
