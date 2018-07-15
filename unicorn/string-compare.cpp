#include "unicorn/string.hpp"
#include <algorithm>

namespace RS::Unicorn {

    namespace {

        bool char_is_ascii_digit(char32_t c) noexcept {
            return c >= U'0' && c <= U'9';
        }

        bool char_is_significant(char32_t c) noexcept {
            char cat = char_primary_category(c);
            return cat == 'L' || cat == 'M' || cat == 'N' || cat == 'S';
        }

        struct NaturalSegment:
        Irange<Utf8Iterator> {
            bool is_number = false;
            Ustring cooked;
            void cook() {
                if (first == second)
                    return;
                if (is_number) {
                    auto i = first;
                    while (i != second && *i == U'0')
                        ++i;
                    str_append(cooked, i, second);
                } else {
                    Ustring s;
                    for (char32_t c: *this)
                        if (char_is_significant(c))
                            str_append_char(s, c);
                    cooked += str_casefold(s);
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
                        return StringCompare<Strcmp::triple>()(cooked, rhs.cooked);
                } else {
                    return StringCompare<Strcmp::triple>()(cooked, rhs.cooked);
                }
                return 0;
            }
        };

        class NaturalSegmentIterator:
        public ForwardIterator<NaturalSegmentIterator, const NaturalSegment> {
        public:
            NaturalSegmentIterator() = default;
            NaturalSegmentIterator(Utf8Iterator i, Utf8Iterator e) {
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
            NaturalSegment segment;
            Utf8Iterator end;
        };

    }

    namespace UnicornDetail {

        int do_compare_basic(const Ustring& lhs, const Ustring& rhs) {
            size_t n = std::min(lhs.size(), rhs.size());
            for (size_t i = 0; i < n; ++i)
                if (lhs[i] != rhs[i])
                    return uint8_t(lhs[i]) < uint8_t(rhs[i]) ? -1 : 1;
            return lhs.size() < rhs.size() ? -1 : lhs.size() == rhs.size() ? 0 : 1;
        }

        int do_compare_icase(const Ustring& lhs, const Ustring& rhs) {
            if (lhs.empty() || rhs.empty())
                return ! rhs.empty() ? -1 : ! lhs.empty() ? 1 : 0;
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
                    return i2 != u2.end() ? -1 : i1 != u1.end() ? 1 : 0;
                if (buf1[p1] != buf2[p2])
                    return buf1[p1] < buf2[p2] ? -1 : buf1[p1] == buf2[p2] ? 0 : 1;
                ++p1;
                ++p2;
            }
        }

        int do_compare_natural(const Ustring& lhs, const Ustring& rhs) {
            auto b1 = utf_begin(lhs), e1 = utf_end(lhs), b2 = utf_begin(rhs), e2 = utf_end(rhs);
            NaturalSegmentIterator s1(b1, e1), s1_end(e1, e1), s2(b2, e2), s2_end(e2, e2);
            for (; s1 != s1_end && s2 != s2_end; ++s1, ++s2) {
                int c = s1->compare(*s2);
                if (c != 0)
                    return c;
            }
            return s2 != s2_end ? -1 : s1 != s1_end ? 1 : 0;
        }

    }

}
