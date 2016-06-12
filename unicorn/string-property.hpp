// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <iterator>
#include <string>

namespace Unicorn {

    template <typename C>
    char32_t str_char_at(const basic_string<C>& str, size_t index) noexcept {
        auto range = utf_range(str);
        for (char32_t c: range)
            if (! index--)
                return c;
        return 0;
    }

    template <typename C>
    char32_t str_first_char(const basic_string<C>& str) noexcept {
        return str.empty() ? 0 : *utf_begin(str);
    }

    template <typename C>
    char32_t str_last_char(const basic_string<C>& str) noexcept {
        return str.empty() ? 0 : *std::prev(utf_end(str));
    }

    bool str_is_east_asian(const u8string& str);
    bool str_starts_with(const u8string& str, const u8string& prefix) noexcept;
    bool str_ends_with(const u8string& str, const u8string& suffix) noexcept;

}
