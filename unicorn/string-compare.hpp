// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-algorithm.hpp"
#include "unicorn/string-case.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <string>

namespace Unicorn {

    struct StringCompare {
        template <typename C>
        bool operator()(const basic_string<C>& lhs, const basic_string<C>& rhs) const noexcept {
            if (sizeof(C) == 2) {
                auto ur1 = utf_range(lhs), ur2 = utf_range(rhs);
                return std::lexicographical_compare(ur1.begin(), ur1.end(), ur2.begin(), ur2.end());
            } else {
                return lhs < rhs;
            }
        }
    };

    constexpr StringCompare str_compare {};

    template <typename C>
    int str_compare_3way(const basic_string<C>& lhs, const basic_string<C>& rhs) {
        size_t common = str_common_utf(lhs, rhs);
        if (common == lhs.size() && common == rhs.size())
            return 0;
        else if (common == rhs.size())
            return 1;
        else if (common == lhs.size())
            return -1;
        char32_t u1 = *utf_iterator(lhs, common), u2 = *utf_iterator(rhs, common);
        return u1 > u2 ? 1 : -1;
    }

    struct IcaseCompare {
        template <typename C>
        bool operator()(const basic_string<C>& lhs, const basic_string<C>& rhs) const noexcept {
            if (lhs.empty() || rhs.empty())
                return ! rhs.empty();
            auto u1 = utf_range(lhs), u2 = utf_range(rhs);
            auto i1 = u1.begin(), i2 = u2.begin();
            char32_t buf1[max_case_decomposition], buf2[max_case_decomposition];
            size_t p1 = 0, p2 = 0;
            size_t n1 = char_to_full_casefold(*i1, buf1);
            size_t n2 = char_to_full_casefold(*i2, buf2);
            for (;;) {
                if (p1 == n1) {
                    p1 = 0;
                    if (++i1 != u1.end())
                        n1 = char_to_full_casefold(*i1, buf1);
                }
                if (p2 == n2) {
                    p2 = 0;
                    if (++i2 != u2.end())
                        n2 = char_to_full_casefold(*i2, buf2);
                }
                if ((p1 == 0 && i1 == u1.end()) || (p2 == 0 && i2 == u2.end()))
                    return i2 != u2.end();
                if (buf1[p1] != buf2[p2])
                    return buf1[p1] < buf2[p2];
                ++p1;
                ++p2;
            }
        }
    };

    struct IcaseEqual {
        template <typename C>
        bool operator()(const basic_string<C>& lhs, const basic_string<C>& rhs) const noexcept {
            if (lhs.empty() && rhs.empty())
                return true;
            if (lhs.size() != rhs.size())
                return false;
            auto u1 = utf_range(lhs), u2 = utf_range(rhs);
            auto i1 = u1.begin(), i2 = u2.begin();
            char32_t buf1[max_case_decomposition], buf2[max_case_decomposition];
            size_t p1 = 0, p2 = 0;
            size_t n1 = char_to_full_casefold(*i1, buf1);
            size_t n2 = char_to_full_casefold(*i2, buf2);
            for (;;) {
                if (p1 == n1) {
                    p1 = 0;
                    if (++i1 != u1.end())
                        n1 = char_to_full_casefold(*i1, buf1);
                }
                if (p2 == n2) {
                    p2 = 0;
                    if (++i2 != u2.end())
                        n2 = char_to_full_casefold(*i2, buf2);
                }
                if ((p1 == 0 && i1 == u1.end()) || (p2 == 0 && i2 == u2.end()))
                    return i1 == u1.end() && i2 == u2.end();
                if (buf1[p1] != buf2[p2])
                    return false;
                ++p1;
                ++p2;
            }
        }
    };

    constexpr IcaseCompare str_icase_compare {};
    constexpr IcaseEqual str_icase_equal {};

}
