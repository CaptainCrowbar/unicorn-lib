// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <iterator>
#include <string>

namespace Unicorn {

    template <typename C>
    char32_t str_char_at(const basic_string<C>& str, size_t index) noexcept {
        for (auto&& c: utf_range(str))
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

    template <typename C>
    bool str_is_east_asian(const basic_string<C>& str) {
        for (auto c: utf_range(str)) {
            auto w = east_asian_width(c);
            if (w == East_Asian_Width::F || w == East_Asian_Width::H || w == East_Asian_Width::W)
                return true;
        }
        return false;
    }

    template <typename C>
    bool str_starts_with(const basic_string<C>& str, const basic_string<C>& prefix) noexcept {
        return str.size() >= prefix.size()
            && memcmp(str.data(), prefix.data(), sizeof(C) * prefix.size()) == 0;
    }

    template <typename C>
    bool str_starts_with(const basic_string<C>& str, const C* prefix) noexcept {
        if (! prefix)
            return true;
        auto count = cstr_size(prefix);
        return str.size() >= count && memcmp(str.data(), prefix, sizeof(C) * count) == 0;
    }

    template <typename C>
    bool str_ends_with(const basic_string<C>& str, const basic_string<C>& suffix) noexcept {
        return str.size() >= suffix.size()
            && memcmp(str.data() + str.size() - suffix.size(), suffix.data(), sizeof(C) * suffix.size()) == 0;
    }

    template <typename C>
    bool str_ends_with(const basic_string<C>& str, const C* suffix) noexcept {
        if (! suffix)
            return true;
        auto count = cstr_size(suffix);
        return str.size() >= count && memcmp(str.data() + str.size() - count, suffix, sizeof(C) * count) == 0;
    }

}
