#include "unicorn/string.hpp"

namespace Unicorn {

    int str_compare_3way(const u8string& lhs, const u8string& rhs) {
        size_t size1 = lhs.size(), size2 = rhs.size();
        size_t common = std::min(lhs.size(), rhs.size());
        int rc = memcmp(lhs.data(), rhs.data(), common);
        if (rc != 0)
            return rc > 0 ? 1 : -1;
        else
            return size1 < size2 ? -1 : size1 == size2 ? 0 : 1;
    }

    bool str_icase_compare(const u8string& lhs, const u8string& rhs) noexcept {
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

    bool str_icase_equal(const u8string& lhs, const u8string& rhs) noexcept {
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
            u8string cooked;
            void cook() {
                if (first == second)
                    return;
                if (is_number) {
                    auto i = first;
                    while (i != second && *i == U'0')
                        ++i;
                    str_append(cooked, i, second);
                } else {
                    u8string s;
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
                        return str_compare_3way(cooked, rhs.cooked);
                } else {
                    return str_compare_3way(cooked, rhs.cooked);
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

    bool str_natural_compare(const u8string& lhs, const u8string& rhs) noexcept {
        auto b1 = utf_begin(lhs), e1 = utf_end(lhs), b2 = utf_begin(rhs), e2 = utf_end(rhs);
        NaturalSegmentIterator s1(b1, e1), s1_end(e1, e1), s2(b2, e2), s2_end(e2, e2);
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
            return lhs < rhs;
    }

}
