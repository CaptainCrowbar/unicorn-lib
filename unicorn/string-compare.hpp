// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-algorithm.hpp"
#include "unicorn/string-case.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <iterator>
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

    namespace UnicornDetail {

        inline bool char_is_ascii_digit(char32_t c) noexcept { return c >= U'0' && c <= U'9'; }
        inline bool char_is_significant(char32_t c) noexcept {
            char cat = char_primary_category(c);
            return cat == 'L' || cat == 'M' || cat == 'N' || cat == 'S';
        }

        template <typename C>
        struct NaturalSegment:
        Irange<UtfIterator<C>> {
            bool is_number = false;
            u32string cooked;
            void cook() {
                cooked.clear();
                if (this->first == this->second)
                    return;
                if (is_number) {
                    auto i = this->first;
                    while (i != this->second && *i == U'0')
                        ++i;
                    std::copy(i, this->second, append(cooked));
                } else {
                    std::copy_if(this->first, this->second, append(cooked), char_is_significant);
                    str_casefold_in(cooked);
                }
            }
            int compare(const NaturalSegment& rhs) const {
                if (is_number != rhs.is_number)
                    return is_number ? -1 : 1;
                if (is_number) {
                    if (cooked.size() < rhs.cooked.size())
                        return -1;
                    else if (cooked.size() > rhs.cooked.size())
                        return 1;
                    else
                        return str_compare_3way(cooked, rhs.cooked);
                } else {
                    return str_compare_3way(cooked, rhs.cooked);
                }
                return 0;
            }
        };

        template <typename C>
        class NaturalSegmentIterator:
        public ForwardIterator<NaturalSegmentIterator<C>, const NaturalSegment<C>> {
        public:
            NaturalSegmentIterator() = default;
            NaturalSegmentIterator(UtfIterator<C> i, UtfIterator<C> e) {
                segment.first = segment.second = i;
                end = e;
                ++*this;
            }
            const auto& operator*() const noexcept { return segment; }
            NaturalSegmentIterator& operator++() {
                segment.first = segment.second;
                segment.is_number = false;
                if (segment.first == end)
                    return *this;
                segment.is_number = char_is_ascii_digit(*segment.first);
                while (segment.second != end && char_is_ascii_digit(*segment.second) == segment.is_number)
                    ++segment.second;
                segment.cook();
                return *this;
            }
            bool operator==(const NaturalSegmentIterator& rhs) const noexcept { return segment.first == rhs.segment.first; }
        private:
            NaturalSegment<C> segment;
            UtfIterator<C> end;
        };

    }

    struct NaturalCompare {
        template <typename C>
        bool operator()(const basic_string<C>& lhs, const basic_string<C>& rhs) const noexcept {
            using namespace UnicornDetail;
            auto b1 = utf_begin(lhs), e1 = utf_end(lhs), b2 = utf_begin(rhs), e2 = utf_end(rhs);
            NaturalSegmentIterator<C> s1(b1, e1), s1_end(e1, e1), s2(b2, e2), s2_end(e2, e2);
            for (; s1 != s1_end && s2 != s2_end; ++s1, ++s2) {
                int c = s1->compare(*s2);
                if (c != 0)
                    return c == -1;
            }
            if (s1 != s1_end)
                return false;
            else if (s2 != s2_end)
                return true;
            else
                return str_compare(lhs, rhs);
        }
    };

    constexpr NaturalCompare str_natural_compare {};

}
