#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/segment.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace Unicorn {

    // Constants

    // Remember that any other set of flags that might be combined with these
    // needs to skip the bits that are already spoken for. The string length
    // flags are linked to upper case letters for use with unicorn/format, so
    // they have bit positions in the 0-25 range.

    UNICORN_DEFINE_FLAG_LETTER(string length, character_units, 'C');  // Measure string in characters (default)
    UNICORN_DEFINE_FLAG_LETTER(string length, grapheme_units, 'G');   // Measure string in grapheme clusters
    UNICORN_DEFINE_FLAG_LETTER(string length, narrow_context, 'N');   // East Asian width, defaulting to narrow
    UNICORN_DEFINE_FLAG_LETTER(string length, wide_context, 'W');     // East Asian width, defaulting to wide
    UNICORN_DEFINE_FLAG(word wrapping, wrap_crlf, 26);                // Use CR+LF for line breaks (default LF)
    UNICORN_DEFINE_FLAG(word wrapping, wrap_enforce, 27);             // Enforce right margin strictly
    UNICORN_DEFINE_FLAG(word wrapping, wrap_preserve, 28);            // Preserve layout on already indented lines

    // String size functions

    namespace UnicornDetail {

        constexpr auto east_asian_flags = narrow_context | wide_context;
        constexpr auto all_length_flags = character_units | grapheme_units | east_asian_flags;

        inline void check_length_flags(uint32_t& flags) {
            allow_flags(flags, all_length_flags, "string length");
            exclusive_flags(flags, character_units | grapheme_units, "string length");
            exclusive_flags(flags, character_units | east_asian_flags, "string length");
            default_flags(flags, all_length_flags, character_units);
        }

        class EastAsianCount {
        public:
            explicit EastAsianCount(uint32_t flags) noexcept: count(), fset(flags) { memset(count, 0, sizeof(count)); }
            void add(char32_t c) noexcept { ++count[unsigned(east_asian_width(c))]; }
            size_t get() const noexcept {
                size_t default_width = fset & wide_context ? 2 : 1;
                return count[neut] + count[half] + count[narr] + 2 * count[full] + 2 * count[wide]
                    + default_width * count[ambi];
            }
        private:
            static constexpr auto neut = int(East_Asian_Width::N);
            static constexpr auto ambi = int(East_Asian_Width::A);
            static constexpr auto full = int(East_Asian_Width::F);
            static constexpr auto half = int(East_Asian_Width::H);
            static constexpr auto narr = int(East_Asian_Width::Na);
            static constexpr auto wide = int(East_Asian_Width::W);
            size_t count[6];
            uint32_t fset;
        };

        template <typename C>
        std::pair<UtfIterator<C>, bool> find_position(const Irange<UtfIterator<C>>& range, size_t pos, uint32_t flags = 0) {
            check_length_flags(flags);
            if (flags & character_units) {
                auto i = std::begin(range);
                size_t len = 0;
                while (i != std::end(range) && len < pos) {
                    ++i;
                    ++len;
                }
                return {i, len == pos};
            } else if (flags & east_asian_flags) {
                EastAsianCount eac(flags);
                if (flags & grapheme_units) {
                    auto graph = grapheme_range(range);
                    auto g = std::begin(graph);
                    while (g != std::end(graph) && eac.get() < pos) {
                        eac.add(*std::begin(*g));
                        ++g;
                    }
                    return {std::begin(*g), eac.get() >= pos};
                } else {
                    auto i = std::begin(range);
                    while (i != std::end(range) && eac.get() < pos) {
                        eac.add(*i);
                        ++i;
                    }
                    return {i, eac.get() >= pos};
                }
            } else {
                auto graph = grapheme_range(range);
                auto g = std::begin(graph);
                size_t len = 0;
                while (g != std::end(graph) && len < pos) {
                    ++g;
                    ++len;
                }
                return {std::begin(*g), len == pos};
            }
        }

    }

    template <typename C>
    size_t str_length(const Irange<UtfIterator<C>>& range, uint32_t flags = 0) {
        using namespace UnicornDetail;
        check_length_flags(flags);
        if (flags & character_units) {
            return range_count(range);
        } else if (flags & east_asian_flags) {
            EastAsianCount eac(flags);
            if (flags & grapheme_units) {
                for (auto g: grapheme_range(range))
                    eac.add(*std::begin(g));
            } else {
                for (auto c: range)
                    eac.add(c);
            }
            return eac.get();
        } else {
            return range_count(grapheme_range(range));
        }
    }

    template <typename C>
    size_t str_length(const UtfIterator<C>& b, const UtfIterator<C>& e, uint32_t flags = 0) {
        return str_length(irange(b, e), flags);
    }

    template <typename C>
    size_t str_length(const basic_string<C>& str, uint32_t flags = 0) {
        return str_length(utf_range(str), flags);
    }

    template <typename C>
    UtfIterator<C> str_find_index(const Irange<UtfIterator<C>>& range, size_t pos, uint32_t flags = 0) {
        return UnicornDetail::find_position(range, pos, flags).first;
    }

    template <typename C>
    UtfIterator<C> str_find_index(const UtfIterator<C>& b, const UtfIterator<C>& e, size_t pos, uint32_t flags = 0) {
        return str_find_index(irange(b, e), pos, flags);
    }

    template <typename C>
    UtfIterator<C> str_find_index(const basic_string<C>& str, size_t pos, uint32_t flags = 0) {
        return str_find_index(utf_range(str), pos, flags);
    }

    template <typename C>
    size_t str_find_offset(const basic_string<C>& str, size_t pos, uint32_t flags = 0) {
        auto rc = UnicornDetail::find_position(utf_range(str), pos, flags);
        return rc.second ? rc.first.offset() : npos;
    }

    // Functions needed early

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

    // Other string properties

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

    // String algorithms

    template <typename C>
    size_t str_common(const basic_string<C>& s1, const basic_string<C>& s2, size_t start = 0) noexcept {
        if (start >= s1.size() || start >= s2.size())
            return 0;
        auto p = s1.data() + start;
        for (auto q = s2.data() + start, endp = p + std::min(s1.size(), s2.size()); p != endp && *p == *q; ++p, ++q) {}
        return p - s1.data() - start;
    }

    template <typename C>
    size_t str_common_utf(const basic_string<C>& s1, const basic_string<C>& s2, size_t start = 0) noexcept {
        size_t n = str_common(s1, s2, start);
        if (n == 0 || sizeof(C) == 4)
            return n;
        size_t pos = start + n;
        if (! is_following_unit(s1[pos]) && ! is_following_unit(s2[pos]))
            return n;
        --pos;
        while (pos > start && is_following_unit(s1[pos]))
            --pos;
        return pos - start;
    }

    struct StringCompare {
        template <typename C>
        bool operator()(const basic_string<C>& lhs, const basic_string<C>& rhs) const noexcept {
            if (sizeof(C) == 2) {
                auto ur1 = utf_range(lhs), ur2 = utf_range(rhs);
                return std::lexicographical_compare(PRI_BOUNDS(ur1), PRI_BOUNDS(ur2));
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

    template <typename C>
    bool str_expect(UtfIterator<C>& i, const UtfIterator<C>& end, const basic_string<C>& prefix) {
        size_t psize = prefix.size();
        if (psize == 0 || end.offset() - i.offset() < psize
                || memcmp(i.source().data() + i.offset(), prefix.data(), psize) != 0)
            return false;
        i = utf_iterator(i.source(), i.offset() + psize);
        return true;
    }

    template <typename C>
    bool str_expect(UtfIterator<C>& i, const basic_string<C>& prefix) {
        return str_expect(i, utf_end(i.source()), prefix);
    }

    template <typename C>
    UtfIterator<C> str_find_char(const UtfIterator<C>& b, const UtfIterator<C>& e, char32_t c) {
        return std::find(b, e, c);
    }

    template <typename C>
    UtfIterator<C> str_find_char(const Irange<UtfIterator<C>>& range, char32_t c) {
        return std::find(PRI_BOUNDS(range), c);
    }

    template <typename C>
    UtfIterator<C> str_find_char(const basic_string<C>& str, char32_t c) {
        return utf_iterator(str, str.find(str_chars<C>(c)));
    }

    template <typename C>
    UtfIterator<C> str_find_last_char(const UtfIterator<C>& b, const UtfIterator<C>& e, char32_t c) {
        auto i = e;
        while (i != b) {
            --i;
            if (*i == c)
                return i;
        }
        return e;
    }

    template <typename C>
    UtfIterator<C> str_find_last_char(const Irange<UtfIterator<C>>& range, char32_t c) {
        return str_find_last_char(PRI_BOUNDS(range), c);
    }

    template <typename C>
    UtfIterator<C> str_find_last_char(const basic_string<C>& str, char32_t c) {
        return utf_iterator(str, str.rfind(str_chars<C>(c)));
    }

    template <typename C>
    UtfIterator<C> str_find_first_of(const UtfIterator<C>& b, const UtfIterator<C>& e,
            const basic_string<C>& target) {
        auto u_target = to_utf32(target);
        return std::find_if(b, e,
            [&] (char32_t c) { return u_target.find(c) != npos; });
    }

    template <typename C>
    UtfIterator<C> str_find_first_of(const Irange<UtfIterator<C>>& range, const basic_string<C>& target) {
        return str_find_first_of(PRI_BOUNDS(range), target);
    }

    template <typename C>
    UtfIterator<C> str_find_first_of(const basic_string<C>& str, const basic_string<C>& target) {
        return str_find_first_of(utf_begin(str), utf_end(str), target);
    }

    template <typename C>
    UtfIterator<C> str_find_first_of(const UtfIterator<C>& b, const UtfIterator<C>& e, const C* target) {
        return str_find_first_of(b, e, cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_find_first_of(const Irange<UtfIterator<C>>& range, const C* target) {
        return str_find_first_of(PRI_BOUNDS(range), target);
    }

    template <typename C>
    UtfIterator<C> str_find_first_of(const basic_string<C>& str, const C* target) {
        return str_find_first_of(utf_begin(str), utf_end(str), cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_find_first_not_of(const UtfIterator<C>& b, const UtfIterator<C>& e,
            const basic_string<C>& target) {
        auto u_target = to_utf32(target);
        return std::find_if(b, e,
            [&] (char32_t c) { return u_target.find(c) == npos; });
    }

    template <typename C>
    UtfIterator<C> str_find_first_not_of(const Irange<UtfIterator<C>>& range,
            const basic_string<C>& target) {
        return str_find_first_not_of(PRI_BOUNDS(range), target);
    }

    template <typename C>
    UtfIterator<C> str_find_first_not_of(const basic_string<C>& str,
            const basic_string<C>& target) {
        return str_find_first_not_of(utf_begin(str), utf_end(str), target);
    }

    template <typename C>
    UtfIterator<C> str_find_first_not_of(const UtfIterator<C>& b, const UtfIterator<C>& e,
            const C* target) {
        return str_find_first_not_of(b, e, cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_find_first_not_of(const Irange<UtfIterator<C>>& range, const C* target) {
        return str_find_first_not_of(PRI_BOUNDS(range), target);
    }

    template <typename C>
    UtfIterator<C> str_find_first_not_of(const basic_string<C>& str, const C* target) {
        return str_find_first_not_of(utf_begin(str), utf_end(str), cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_find_last_of(const UtfIterator<C>& b, const UtfIterator<C>& e,
            const basic_string<C>& target) {
        auto u_target = to_utf32(target);
        auto i = e;
        while (i != b) {
            --i;
            if (u_target.find(*i) != npos)
                return i;
        }
        return e;
    }

    template <typename C>
    UtfIterator<C> str_find_last_of(const Irange<UtfIterator<C>>& range, const basic_string<C>& target) {
        return str_find_last_of(PRI_BOUNDS(range), target);
    }

    template <typename C>
    UtfIterator<C> str_find_last_of(const basic_string<C>& str, const basic_string<C>& target) {
        return str_find_last_of(utf_begin(str), utf_end(str), target);
    }

    template <typename C>
    UtfIterator<C> str_find_last_of(const UtfIterator<C>& b, const UtfIterator<C>& e, const C* target) {
        return str_find_last_of(b, e, cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_find_last_of(const Irange<UtfIterator<C>>& range, const C* target) {
        return str_find_last_of(PRI_BOUNDS(range), target);
    }

    template <typename C>
    UtfIterator<C> str_find_last_of(const basic_string<C>& str, const C* target) {
        return str_find_last_of(utf_begin(str), utf_end(str), cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_find_last_not_of(const UtfIterator<C>& b, const UtfIterator<C>& e,
            const basic_string<C>& target) {
        auto u_target = to_utf32(target);
        auto i = e;
        while (i != b) {
            --i;
            if (u_target.find(*i) == npos)
                return i;
        }
        return e;
    }

    template <typename C>
    UtfIterator<C> str_find_last_not_of(const Irange<UtfIterator<C>>& range,
            const basic_string<C>& target) {
        return str_find_last_not_of(PRI_BOUNDS(range), target);
    }

    template <typename C>
    UtfIterator<C> str_find_last_not_of(const basic_string<C>& str,
            const basic_string<C>& target) {
        return str_find_last_not_of(utf_begin(str), utf_end(str), target);
    }

    template <typename C>
    UtfIterator<C> str_find_last_not_of(const UtfIterator<C>& b, const UtfIterator<C>& e,
            const C* target) {
        return str_find_last_not_of(b, e, cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_find_last_not_of(const Irange<UtfIterator<C>>& range, const C* target) {
        return str_find_last_not_of(PRI_BOUNDS(range), target);
    }

    template <typename C>
    UtfIterator<C> str_find_last_not_of(const basic_string<C>& str, const C* target) {
        return str_find_last_not_of(utf_begin(str), utf_end(str), cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_search(const UtfIterator<C>& b, const UtfIterator<C>& e,
            const basic_string<C>& target) {
        auto u_target = to_utf32(target);
        return std::search(b, e, PRI_BOUNDS(u_target));
    }

    template <typename C>
    UtfIterator<C> str_search(const Irange<UtfIterator<C>>& range, const basic_string<C>& target) {
        return str_search(PRI_BOUNDS(range), target);
    }

    template <typename C>
    UtfIterator<C> str_search(const basic_string<C>& str, const basic_string<C>& target) {
        return str_search(utf_begin(str), utf_end(str), target);
    }

    template <typename C>
    UtfIterator<C> str_search(const UtfIterator<C>& b, const UtfIterator<C>& e, const C* target) {
        return str_search(b, e, cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_search(const Irange<UtfIterator<C>>& range, const C* target) {
        return str_search(PRI_BOUNDS(range), cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_search(const basic_string<C>& str, const C* target) {
        return str_search(utf_begin(str), utf_end(str), cstr(target));
    }

    template <typename C>
    size_t str_skipws(UtfIterator<C>& i, const UtfIterator<C>& end) {
        size_t n = 0;
        for (; i != end && char_is_white_space(*i); ++i, ++n) {}
        return n;
    }

    template <typename C>
    size_t str_skipws(UtfIterator<C>& i) {
        return str_skipws(i, utf_end(i.source()));
    }

    // String manipulation functions

    template <typename C, typename C2>
    void str_append(basic_string<C>& str, const basic_string<C2>& suffix) {
        std::copy(utf_begin(suffix), utf_end(suffix), utf_writer(str));
    }

    template <typename C, typename C2>
    void str_append(basic_string<C>& str, const Irange<UtfIterator<C2>>& suffix) {
        std::copy(std::begin(suffix), std::end(suffix), utf_writer(str));
    }

    template <typename C, typename C2>
    void str_append(basic_string<C>& str, const UtfIterator<C2>& suffix_begin, const UtfIterator<C2>& suffix_end) {
        std::copy(suffix_begin, suffix_end, utf_writer(str));
    }

    template <typename C, typename C2>
    void str_append(basic_string<C>& str, const C2* suffix) {
        str_append(str, cstr(suffix));
    }

    template <typename C, typename C2>
    void str_append(basic_string<C>& dst, const C2* ptr, size_t n) {
        auto out = utf_writer(dst);
        if (ptr)
            for (; n; --n)
                *out = char_to_uint(*ptr++);
    }

    template <typename C, typename C2>
    void str_append_char(basic_string<C>& dst, C2 c2) {
        *utf_writer(dst) = char_to_uint(c2);
    }

    template <typename C, typename C2, typename C3, typename... Chars>
    void str_append_char(basic_string<C>& dst, C2 c2, C3 c3, Chars... chars) {
        str_append_char(dst, c2);
        str_append_char(dst, c3, chars...);
    }

    template <typename C>
    void str_append_chars(basic_string<C>& dst, char32_t c, size_t n) {
        auto s = str_chars<C>(c);
        if (s.size() == 1)
            dst.append(n, s[0]);
        else
            dst += str_repeat(s, n);
    }

    namespace UnicornDetail {

        template <typename S1, typename DS>
        void concat_helper(S1&, const DS&) {}

        template <typename S1, typename DS, typename S2, typename... Strings>
        void concat_helper(S1& s1, const DS& delim, const S2& s2, const Strings&... ss) {
            str_append(s1, delim);
            str_append(s1, s2);
            concat_helper(s1, delim, ss...);
        }

    }

    template <typename C, typename... Strings>
    basic_string<C> str_concat(const basic_string<C>& s, const Strings&... ss) {
        auto result = s;
        UnicornDetail::concat_helper(result, basic_string<C>(), ss...);
        return result;
    }

    template <typename C, typename... Strings>
    basic_string<C> str_concat(const C* s, const Strings&... ss) {
        auto result = cstr(s);
        UnicornDetail::concat_helper(result, basic_string<C>(), ss...);
        return result;
    }

    template <typename C>
    basic_string<C> str_concat_with(const basic_string<C>& /*delim*/) {
        return {};
    }

    template <typename C>
    basic_string<C> str_concat_with(const C* /*delim*/) {
        return {};
    }

    template <typename C1, typename C2, typename... Strings>
    basic_string<C1> str_concat_with(const basic_string<C1>& delim,
            const basic_string<C2>& s, const Strings&... ss) {
        basic_string<C1> result;
        recode(s, result);
        UnicornDetail::concat_helper(result, delim, ss...);
        return result;
    }

    template <typename C1, typename C2, typename... Strings>
    basic_string<C1> str_concat_with(const basic_string<C1>& delim, const C2* s, const Strings&... ss) {
        basic_string<C1> result;
        recode(s, result);
        UnicornDetail::concat_helper(result, delim, ss...);
        return result;
    }

    template <typename C1, typename C2, typename... Strings>
    basic_string<C1> str_concat_with(const C1* delim, const basic_string<C2>& s, const Strings&... ss) {
        basic_string<C1> result;
        recode(s, result);
        UnicornDetail::concat_helper(result, cstr(delim), ss...);
        return result;
    }

    template <typename C1, typename C2, typename... Strings>
    basic_string<C1> str_concat_with(const C1* delim, const C2* s, const Strings&... ss) {
        basic_string<C1> result;
        recode(cstr(s), result);
        UnicornDetail::concat_helper(result, cstr(delim), ss...);
        return result;
    }

    template <typename C>
    basic_string<C> str_drop_prefix(const basic_string<C>& str, const basic_string<C>& prefix) {
        return str_starts_with(str, prefix) ? str.substr(prefix.size()) : str;
    }

    template <typename C>
    basic_string<C> str_drop_prefix(const basic_string<C>& str, const C* prefix) {
        return str_starts_with(str, prefix) ? str.substr(cstr_size(prefix)) : str;
    }

    template <typename C>
    void str_drop_prefix_in(basic_string<C>& str, const basic_string<C>& prefix) noexcept {
        if (str_starts_with(str, prefix))
            str.erase(0, prefix.size());
    }

    template <typename C>
    void str_drop_prefix_in(basic_string<C>& str, const C* prefix) noexcept {
        if (str_starts_with(str, prefix))
            str.erase(0, cstr_size(prefix));
    }

    template <typename C>
    basic_string<C> str_drop_suffix(const basic_string<C>& str, const basic_string<C>& suffix) {
        return str_ends_with(str, suffix) ? str.substr(0, str.size() - suffix.size()) : str;
    }

    template <typename C>
    basic_string<C> str_drop_suffix(const basic_string<C>& str, const C* suffix) {
        return str_ends_with(str, suffix) ? str.substr(0, str.size() - cstr_size(suffix)) : str;
    }

    template <typename C>
    void str_drop_suffix_in(basic_string<C>& str, const basic_string<C>& suffix) noexcept {
        if (str_ends_with(str, suffix))
            str.resize(str.size() - suffix.size());
    }

    template <typename C>
    void str_drop_suffix_in(basic_string<C>& str, const C* suffix) noexcept {
        if (str_ends_with(str, suffix))
            str.resize(str.size() - cstr_size(suffix));
    }

    template <typename C>
    basic_string<C> str_erase_left(const basic_string<C>& str, size_t length) {
        if (length == 0)
            return str;
        auto range = utf_range(str);
        auto i = std::begin(range);
        for (size_t j = 0; j < length && i != std::end(range); ++i, ++j) {}
        if (i == std::end(range))
            return {};
        else
            return str.substr(i.offset(), npos);
    }

    template <typename C>
    void str_erase_left_in(basic_string<C>& str, size_t length) noexcept {
        if (length == 0)
            return;
        auto range = utf_range(str);
        auto i = std::begin(range);
        for (size_t j = 0; j < length && i != std::end(range); ++i, ++j) {}
        if (i == std::end(range))
            str.clear();
        else
            str.erase(0, i.offset());
    }

    template <typename C>
    basic_string<C> str_erase_right(const basic_string<C>& str, size_t length) {
        if (length == 0)
            return str;
        auto range = utf_range(str);
        auto i = std::end(range);
        for (size_t j = 0; j < length && i != std::begin(range); --i, ++j) {}
        if (i == std::begin(range))
            return {};
        else
            return str.substr(0, i.offset());
    }

    template <typename C>
    void str_erase_right_in(basic_string<C>& str, size_t length) noexcept {
        if (length == 0)
            return;
        auto range = utf_range(str);
        auto i = std::end(range);
        for (size_t j = 0; j < length && i != std::begin(range); --i, ++j) {}
        if (i == std::begin(range))
            str.clear();
        else
            str.erase(i.offset(), npos);
    }

    namespace UnicornDetail {

        template <typename C>
        basic_string<C> expand_tabs(const basic_string<C>& str, const vector<size_t>& tabs, uint32_t flags) {
            vector<size_t> xtabs = {0};
            for (auto t: tabs)
                if (t > xtabs.back())
                    xtabs.push_back(t);
            size_t delta = 8;
            if (xtabs.size() > 1)
                delta = xtabs.end()[-1] - xtabs.end()[-2];
            basic_string<C> result;
            auto line_begin = utf_begin(str), str_end = utf_end(str);
            while (line_begin != str_end) {
                auto t = xtabs.begin(), t_end = xtabs.end();
                auto line_end = std::find_if(line_begin, str_end, char_is_line_break);
                auto u1 = line_begin;
                size_t col = 0;
                while (u1 != line_end) {
                    auto cut = std::find(str.begin() + u1.offset(), str.begin() + line_end.offset(), PRI_CHAR('\t', C));
                    auto u2 = utf_iterator(str, cut - str.begin());
                    str_append(result, u1, u2);
                    col += str_length(u1, u2, flags);
                    if (u2 == line_end)
                        break;
                    while (t != t_end && *t <= col)
                        ++t;
                    size_t tab_col;
                    if (t == t_end)
                        tab_col = xtabs.back() + delta * ((col - xtabs.back()) / delta + 1);
                    else
                        tab_col = *t;
                    result.append(tab_col - col, PRI_CHAR(' ', C));
                    col = tab_col;
                    u1 = std::next(u2);
                }
                if (line_end == str_end)
                    break;
                str_append_char(result, *line_end);
                line_begin = std::next(line_end);
            }
            return result;
        }

    }

    template <typename C, typename IntList>
    basic_string<C> str_expand_tabs(const basic_string<C>& str, const IntList& tabs, uint32_t flags = 0) {
        vector<size_t> tv(PRI_BOUNDS(tabs));
        return UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename C, typename IntType>
    basic_string<C> str_expand_tabs(const basic_string<C>& str, std::initializer_list<IntType> tabs, uint32_t flags = 0) {
        vector<size_t> tv(PRI_BOUNDS(tabs));
        return UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename C>
    basic_string<C> str_expand_tabs(const basic_string<C>& str) {
        return UnicornDetail::expand_tabs(str, {}, {});
    }

    template <typename C, typename IntList>
    void str_expand_tabs_in(basic_string<C>& str, const IntList& tabs, uint32_t flags = 0) {
        auto result = str_expand_tabs(str, tabs, flags);
        str.swap(result);
    }

    template <typename C, typename IntType>
    void str_expand_tabs_in(basic_string<C>& str, std::initializer_list<IntType> tabs, uint32_t flags = 0) {
        auto result = str_expand_tabs(str, tabs, flags);
        str.swap(result);
    }

    template <typename C>
    void str_expand_tabs_in(basic_string<C>& str) {
        auto result = str_expand_tabs(str);
        str.swap(result);
    }

    namespace UnicornDetail {

        template <typename C>
        void insert_padding(basic_string<C>& str, size_t old_length, size_t new_length, char32_t c, uint32_t flags, char side) {
            size_t pad_chars = new_length - old_length;
            if (flags & east_asian_flags) {
                auto eaw = east_asian_width(c);
                if (eaw == East_Asian_Width::F || eaw == East_Asian_Width::W
                        || (eaw == East_Asian_Width::A && str_is_east_asian(str)))
                    pad_chars = (pad_chars + 1) / 2;
            }
            if (code_units<C>(c) == 1) {
                if (side == 'L')
                    str.insert(0, pad_chars, static_cast<C>(c));
                else
                    str.append(pad_chars, static_cast<C>(c));
            } else {
                auto padding = str_chars<C>(c, pad_chars);
                if (side == 'L')
                    str.insert(0, padding);
                else
                    str += padding;
            }
        }

    }

    template <typename C>
    basic_string<C> str_fix_left(const basic_string<C>& str, size_t length, char32_t c = U' ', uint32_t flags = 0) {
        size_t offset = str_find_offset(str, length, flags);
        if (offset == npos) {
            auto result = str;
            UnicornDetail::insert_padding(result, str_length(result, flags), length, c, flags, 'R');
            return result;
        } else {
            return str.substr(0, offset);
        }
    }

    template <typename C>
    void str_fix_left_in(basic_string<C>& str, size_t length, char32_t c = U' ', uint32_t flags = 0) {
        size_t offset = str_find_offset(str, length, flags);
        if (offset == npos)
            UnicornDetail::insert_padding(str, str_length(str, flags), length, c, flags, 'R');
        else
            str.erase(offset, npos);
    }

    template <typename C>
    basic_string<C> str_fix_right(const basic_string<C>& str, size_t length, char32_t c = U' ', uint32_t flags = 0) {
        size_t old_length = str_length(str, flags);
        if (old_length < length) {
            auto result = str;
            UnicornDetail::insert_padding(result, old_length, length, c, flags, 'L');
            return result;
        } else {
            size_t offset = str_find_offset(str, old_length - length, flags);
            return str.substr(offset, npos);
        }
    }

    template <typename C>
    void str_fix_right_in(basic_string<C>& str, size_t length, char32_t c = U' ', uint32_t flags = 0) {
        size_t old_length = str_length(str, flags);
        if (old_length < length) {
            UnicornDetail::insert_padding(str, old_length, length, c, flags, 'L');
        } else {
            size_t offset = str_find_offset(str, old_length - length, flags);
            str.erase(0, offset);
        }
    }

    template <typename C>
    basic_string<C> str_insert(const UtfIterator<C>& dst, const UtfIterator<C>& src_begin, const UtfIterator<C>& src_end) {
        basic_string<C> result(dst.source(), 0, dst.offset());
        result.append(src_begin.source(), src_begin.offset(), src_end.offset() - src_begin.offset());
        result.append(dst.source(), dst.offset(), npos);
        return result;
    }

    template <typename C>
    basic_string<C> str_insert(const UtfIterator<C>& dst, const Irange<UtfIterator<C>>& src) {
        return str_insert(dst, PRI_BOUNDS(src));
    }

    template <typename C>
    basic_string<C> str_insert(const UtfIterator<C>& dst, const basic_string<C>& src) {
        basic_string<C> result(dst.source(), 0, dst.offset());
        result += src;
        result.append(dst.source(), dst.offset(), npos);
        return result;
    }

    template <typename C>
    basic_string<C> str_insert(const UtfIterator<C>& dst_begin, const UtfIterator<C>& dst_end,
            const UtfIterator<C>& src_begin, const UtfIterator<C>& src_end) {
        basic_string<C> result(dst_begin.source(), 0, dst_begin.offset());
        result.append(src_begin.source(), src_begin.offset(), src_end.offset() - src_begin.offset());
        result.append(dst_end.source(), dst_end.offset(), npos);
        return result;
    }

    template <typename C>
    basic_string<C> str_insert(const Irange<UtfIterator<C>>& dst, const Irange<UtfIterator<C>>& src) {
        return str_insert(PRI_BOUNDS(dst), PRI_BOUNDS(src));
    }

    template <typename C>
    basic_string<C> str_insert(const UtfIterator<C>& dst_begin, const UtfIterator<C>& dst_end,
            const basic_string<C>& src) {
        basic_string<C> result(dst_begin.source(), 0, dst_begin.offset());
        result += src;
        result.append(dst_end.source(), dst_end.offset(), npos);
        return result;
    }

    template <typename C>
    basic_string<C> str_insert(const Irange<UtfIterator<C>>& dst, const basic_string<C>& src) {
        return str_insert(PRI_BOUNDS(dst), utf_begin(src), utf_end(src));
    }

    template <typename C>
    Irange<UtfIterator<C>> str_insert_in(basic_string<C>& dst, const UtfIterator<C>& where,
            const UtfIterator<C>& src_begin, const UtfIterator<C>& src_end) {
        size_t ofs1 = where.offset(), ofs2 = src_begin.offset(), n = src_end.offset() - ofs2;
        dst.insert(ofs1, src_begin.source(), ofs2, n);
        return {utf_iterator(dst, ofs1), utf_iterator(dst, ofs1 + n)};
    }

    template <typename C>
    Irange<UtfIterator<C>> str_insert_in(basic_string<C>& dst, const UtfIterator<C>& where, const Irange<UtfIterator<C>>& src) {
        return str_insert_in(dst, where, PRI_BOUNDS(src));
    }

    template <typename C>
    Irange<UtfIterator<C>> str_insert_in(basic_string<C>& dst, const UtfIterator<C>& where, const basic_string<C>& src) {
        size_t ofs = where.offset();
        dst.insert(ofs, src);
        return {utf_iterator(dst, ofs), utf_iterator(dst, ofs + src.size())};
    }

    template <typename C>
    Irange<UtfIterator<C>> str_insert_in(basic_string<C>& dst,
            const UtfIterator<C>& range_begin, const UtfIterator<C>& range_end,
            const UtfIterator<C>& src_begin, const UtfIterator<C>& src_end) {
        size_t ofs1 = range_begin.offset(), n1 = range_end.offset() - ofs1,
            ofs2 = src_begin.offset(), n2 = src_end.offset() - ofs2;
        dst.replace(ofs1, n1, src_begin.source(), ofs2, n2);
        return {utf_iterator(dst, ofs1), utf_iterator(dst, ofs1 + n2)};
    }

    template <typename C>
    Irange<UtfIterator<C>> str_insert_in(basic_string<C>& dst, const Irange<UtfIterator<C>>& range,
            const Irange<UtfIterator<C>>& src) {
        return str_insert_in(dst, PRI_BOUNDS(range), PRI_BOUNDS(src));
    }

    template <typename C>
    Irange<UtfIterator<C>> str_insert_in(basic_string<C>& dst,
            const UtfIterator<C>& range_begin, const UtfIterator<C>& range_end,
            const basic_string<C>& src) {
        size_t ofs = range_begin.offset(), n = range_end.offset() - ofs;
        dst.replace(ofs, n, src);
        return {utf_iterator(dst, ofs), utf_iterator(dst, ofs + src.size())};
    }

    template <typename C>
    Irange<UtfIterator<C>> str_insert_in(basic_string<C>& dst, const Irange<UtfIterator<C>>& range,
            const basic_string<C>& src) {
        return str_insert_in(dst, PRI_BOUNDS(range), utf_begin(src), utf_end(src));
    }

    template <typename FwdRange, typename C>
    basic_string<C> str_join(const FwdRange& r, const basic_string<C>& delim) {
        using string_type = basic_string<C>;
        using range_char = std::decay_t<decltype((*std::begin(r))[0])>;
        PRI_STATIC_ASSERT((std::is_same<C, range_char>::value));
        string_type s;
        auto i = std::begin(r), e = std::end(r);
        if (i != e) {
            s = *i;
            ++i;
        }
        while (i != e) {
            s += delim;
            s += *i;
            ++i;
        }
        return s;
    }

    template <typename FwdRange, typename C>
    basic_string<C> str_join(const FwdRange& r, const C* delim) {
        return str_join(r, cstr(delim));
    }

    template <typename FwdRange>
    auto str_join(const FwdRange& r) -> basic_string<std::decay_t<decltype((*std::begin(r))[0])>> {
        using range_char = std::decay_t<decltype((*std::begin(r))[0])>;
        using string_type = basic_string<range_char>;
        return str_join(r, string_type());
    }

    template <typename C>
    basic_string<C> str_pad_left(const basic_string<C>& str, size_t length, char32_t c = U' ', uint32_t flags = 0) {
        size_t old_length = str_length(str, flags);
        if (length > old_length) {
            auto result = str;
            UnicornDetail::insert_padding(result, old_length, length, c, flags, 'L');
            return result;
        } else {
            return str;
        }
    }

    template <typename C>
    void str_pad_left_in(basic_string<C>& str, size_t length, char32_t c = U' ', uint32_t flags = 0) {
        size_t old_length = str_length(str, flags);
        if (length > old_length)
            UnicornDetail::insert_padding(str, old_length, length, c, flags, 'L');
    }

    template <typename C>
    basic_string<C> str_pad_right(const basic_string<C>& str, size_t length, char32_t c = U' ', uint32_t flags = 0) {
        size_t old_length = str_length(str, flags);
        if (length > old_length) {
            auto result = str;
            UnicornDetail::insert_padding(result, old_length, length, c, flags, 'R');
            return result;
        } else {
            return str;
        }
    }

    template <typename C>
    void str_pad_right_in(basic_string<C>& str, size_t length, char32_t c = U' ', uint32_t flags = 0) {
        size_t old_length = str_length(str, flags);
        if (length > old_length)
            UnicornDetail::insert_padding(str, old_length, length, c, flags, 'R');
    }

    template <typename C>
    bool str_partition(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix) {
        if (str.empty()) {
            prefix.clear();
            suffix.clear();
            return false;
        }
        auto range = utf_range(str);
        auto i = std::find_if(PRI_BOUNDS(range), char_is_white_space);
        if (i == std::end(range)) {
            prefix = str;
            suffix.clear();
            return false;
        }
        auto j = std::find_if_not(i, std::end(range), char_is_white_space);
        auto temp = u_str(std::begin(range), i);
        suffix = u_str(j, std::end(range));
        prefix.swap(temp);
        return true;
    }

    template <typename C>
    bool str_partition_at(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix, const basic_string<C>& delim) {
        size_t pos = delim.empty() ? npos : str.find(delim);
        if (pos == npos) {
            prefix = str;
            suffix.clear();
            return false;
        } else {
            auto temp = str.substr(0, pos);
            suffix = str.substr(pos + delim.size(), npos);
            prefix.swap(temp);
            return true;
        }
    }

    template <typename C>
    bool str_partition_at(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix, const C* delim) {
        using string_type = basic_string<C>;
        return str_partition_at(str, prefix, suffix, delim ? string_type(delim) : string_type());
    }

    template <typename C>
    bool str_partition_by(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix, const basic_string<C>& delim) {
        if (str.empty() || delim.empty()) {
            prefix = str;
            suffix.clear();
            return false;
        }
        auto u_delim = to_utf32(delim);
        size_t i = 0, j = 0;
        if (delim.size() == u_delim.size()) {
            i = str.find_first_of(delim);
            if (i != npos)
                j = str.find_first_not_of(delim, i);
        } else {
            auto match = [&] (char32_t c) { return u_delim.find(c) != npos; };
            auto range = utf_range(str);
            auto p = std::find_if(PRI_BOUNDS(range), match);
            i = p.offset();
            if (p != std::end(range)) {
                auto q = std::find_if_not(p, std::end(range), match);
                j = q.offset();
            }
        }
        if (i == npos) {
            prefix = str;
            suffix.clear();
        } else if (j == npos) {
            prefix = str.substr(0, i);
            suffix.clear();
        } else {
            auto temp = str.substr(0, i);
            suffix = str.substr(j, npos);
            prefix.swap(temp);
        }
        return i != npos;
    }

    template <typename C>
    bool str_partition_by(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix, const C* delim) {
        using string_type = basic_string<C>;
        return str_partition_by(str, prefix, suffix, delim ? string_type(delim) : string_type());
    }

    template <typename C>
    basic_string<C> str_remove(const basic_string<C>& str, char32_t c) {
        basic_string<C> dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [c] (char32_t x) { return x != c; });
        return dst;
    }

    template <typename C>
    basic_string<C> str_remove(const basic_string<C>& str, const basic_string<C>& chars) {
        basic_string<C> dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [&chars] (char32_t x) { return chars.find(x) == npos; });
        return dst;
    }

    template <typename C>
    basic_string<C> str_remove(const basic_string<C>& str, const C* chars) {
        return str_remove(str, cstr(chars));
    }

    template <typename C, typename Pred>
    basic_string<C> str_remove_if(const basic_string<C>& str, Pred p) {
        basic_string<C> dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [p] (char32_t x) { return ! p(x); });
        return dst;
    }

    template <typename C, typename Pred>
    basic_string<C> str_remove_if_not(const basic_string<C>& str, Pred p) {
        basic_string<C> dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), p);
        return dst;
    }

    template <typename C>
    void str_remove_in(basic_string<C>& str, char32_t c) {
        basic_string<C> dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [c] (char32_t x) { return x != c; });
        str.swap(dst);
    }

    template <typename C>
    void str_remove_in(basic_string<C>& str, const basic_string<C>& chars) {
        basic_string<C> dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [&chars] (char32_t x) { return chars.find(x) == npos; });
        str.swap(dst);
    }

    template <typename C>
    void str_remove_in(basic_string<C>& str, const C* chars) {
        str_remove_in(str, cstr(chars));
    }

    template <typename C, typename Pred>
    void str_remove_in_if(basic_string<C>& str, Pred p) {
        basic_string<C> dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [p] (char32_t x) { return ! p(x); });
        str.swap(dst);
    }

    template <typename C, typename Pred>
    void str_remove_in_if_not(basic_string<C>& str, Pred p) {
        basic_string<C> dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), p);
        str.swap(dst);
    }

    template <typename C>
    void str_repeat_in(basic_string<C>& str, size_t n) {
        auto dst = str_repeat(str, n);
        str.swap(dst);
    }

    template <typename C>
    basic_string<C> str_replace(const basic_string<C>& str, const basic_string<C>& target, const basic_string<C>& sub, size_t n = npos) {
        using string_type = basic_string<C>;
        if (target.empty() || n == 0)
            return str;
        string_type dst;
        size_t i = 0, size = str.size(), tsize = target.size();
        for (size_t k = 0; k < n && i < size; ++k) {
            auto j = str.find(target, i);
            if (j == npos) {
                dst.append(str, i, npos);
                i = npos;
                break;
            }
            dst.append(str, i, j - i);
            dst += sub;
            i = j + tsize;
        }
        if (i < size)
            dst.append(str, i, npos);
        return dst;
    }

    template <typename C>
    basic_string<C> str_replace(const basic_string<C>& str, const C* target, const basic_string<C>& sub, size_t n = npos) {
        return str_replace(str, cstr(target), sub, n);
    }

    template <typename C>
    basic_string<C> str_replace(const basic_string<C>& str, const basic_string<C>& target, const C* sub, size_t n = npos) {
        return str_replace(str, target, cstr(sub), n);
    }

    template <typename C>
    basic_string<C> str_replace(const basic_string<C>& str, const C* target, const C* sub, size_t n = npos) {
        return str_replace(str, cstr(target), cstr(sub), n);
    }

    template <typename C>
    void str_replace_in(basic_string<C>& str, const basic_string<C>& target, const basic_string<C>& sub, size_t n = npos) {
        auto result = str_replace(str, target, sub, n);
        str.swap(result);
    }

    template <typename C>
    void str_replace_in(basic_string<C>& str, const C* target, const basic_string<C>& sub, size_t n = npos) {
        str_replace_in(str, cstr(target), sub, n);
    }

    template <typename C>
    void str_replace_in(basic_string<C>& str, const basic_string<C>& target, const C* sub, size_t n = npos) {
        str_replace_in(str, target, cstr(sub), n);
    }

    template <typename C>
    void str_replace_in(basic_string<C>& str, const C* target, const C* sub, size_t n = npos) {
        str_replace_in(str, cstr(target), cstr(sub), n);
    }

    template <typename C, typename OutIter>
    void str_split(const basic_string<C>& src, OutIter dst) {
        auto range = utf_range(src);
        auto i = std::begin(range), j = i;
        while (i != std::end(range)) {
            j = std::find_if_not(i, std::end(range), char_is_white_space);
            if (j == std::end(range))
                break;
            i = std::find_if(j, std::end(range), char_is_white_space);
            *dst++ = u_str(j, i);
        }
    }

    template <typename C, typename OutIter>
    void str_split_at(const basic_string<C>& src, OutIter dst, const basic_string<C>& delim) {
        if (delim.empty()) {
            *dst++ = src;
            return;
        }
        size_t i = 0, dsize = delim.size();
        for (;;) {
            auto j = src.find(delim, i);
            if (j == npos) {
                *dst++ = src.substr(i);
                break;
            }
            *dst++ = src.substr(i, j - i);
            i = j + dsize;
        }
    }

    template <typename C, typename OutIter>
    void str_split_at(const basic_string<C>& src, OutIter dst, const C* delim) {
        str_split_at(src, dst, cstr(delim));
    }

    template <typename C, typename OutIter>
    void str_split_by(const basic_string<C>& src, OutIter dst, const basic_string<C>& delim) {
        if (delim.empty()) {
            *dst++ = src;
            return;
        }
        auto u_delim = to_utf32(delim);
        if (delim.size() == u_delim.size()) {
            size_t i = 0, n = src.size();
            while (i < n) {
                auto j = src.find_first_not_of(delim, i);
                if (j == npos)
                    break;
                i = src.find_first_of(delim, j);
                if (i == npos)
                    i = n;
                *dst++ = src.substr(j, i - j);
            }
        } else {
            auto match = [&] (char32_t c) { return u_delim.find(c) != npos; };
            auto range = utf_range(src);
            auto i = std::begin(range), j = i;
            while (i != std::end(range)) {
                j = std::find_if_not(i, std::end(range), match);
                if (j == std::end(range))
                    break;
                i = std::find_if(j, std::end(range), match);
                *dst++ = u_str(j, i);
            }
        }
    }

    template <typename C, typename OutIter>
    void str_split_by(const basic_string<C>& src, OutIter dst, const C* delim) {
        str_split_by(src, dst, cstr(delim));
    }

    namespace UnicornDetail {

        template <typename C>
        void str_squeeze_helper(const basic_string<C>& src, basic_string<C>& dst, bool trim) {
            auto i = utf_begin(src), end = utf_end(src);
            if (trim)
                i = std::find_if_not(i, end, char_is_white_space);
            while (i != end) {
                auto j = std::find_if(i, end, char_is_white_space);
                str_append(dst, i, j);
                if (j == end)
                    break;
                i = std::find_if_not(j, end, char_is_white_space);
                if (! trim || i != end)
                    str_append_char(dst, PRI_CHAR(' ', C));
            }
        }

        template <typename C>
        void str_squeeze_helper(const basic_string<C>& src, basic_string<C>& dst, bool trim, const basic_string<C>& chars) {
            if (chars.empty()) {
                dst = src;
                return;
            }
            auto sub = str_first_char(chars);
            auto i = utf_begin(src), end = utf_end(src);
            if (trim)
                i = str_find_first_not_of(i, end, chars);
            while (i != end) {
                auto j = str_find_first_of(i, end, chars);
                str_append(dst, i, j);
                if (j == end)
                    break;
                i = str_find_first_not_of(j, end, chars);
                if (! trim || i != end)
                    str_append_char(dst, sub);
            }
        }

    }

    template <typename C>
    basic_string<C> str_squeeze(const basic_string<C>& str) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, false);
        return dst;
    }

    template <typename C>
    basic_string<C> str_squeeze(const basic_string<C>& str, const basic_string<C>& chars) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, false, chars);
        return dst;
    }

    template <typename C>
    basic_string<C> str_squeeze(const basic_string<C>& str, const C* chars) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, false, cstr(chars));
        return dst;
    }

    template <typename C>
    basic_string<C> str_squeeze_trim(const basic_string<C>& str) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, true);
        return dst;
    }

    template <typename C>
    basic_string<C> str_squeeze_trim(const basic_string<C>& str, const basic_string<C>& chars) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, true, chars);
        return dst;
    }

    template <typename C>
    basic_string<C> str_squeeze_trim(const basic_string<C>& str, const C* chars) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, true, cstr(chars));
        return dst;
    }

    template <typename C>
    void str_squeeze_in(basic_string<C>& str) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, false);
        str.swap(dst);
    }

    template <typename C>
    void str_squeeze_in(basic_string<C>& str, const basic_string<C>& chars) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, false, chars);
        str.swap(dst);
    }

    template <typename C>
    void str_squeeze_in(basic_string<C>& str, const C* chars) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, false, cstr(chars));
        str.swap(dst);
    }

    template <typename C>
    void str_squeeze_trim_in(basic_string<C>& str) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, true);
        str.swap(dst);
    }

    template <typename C>
    void str_squeeze_trim_in(basic_string<C>& str, const basic_string<C>& chars) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, true, chars);
        str.swap(dst);
    }

    template <typename C>
    void str_squeeze_trim_in(basic_string<C>& str, const C* chars) {
        basic_string<C> dst;
        UnicornDetail::str_squeeze_helper(str, dst, true, cstr(chars));
        str.swap(dst);
    }

    template <typename C>
    basic_string<C> str_substring(const basic_string<C>& str, size_t offset, size_t count = npos) {
        if (offset < str.size())
            return str.substr(offset, count);
        else
            return {};
    }

    template <typename C>
    basic_string<C> utf_substring(const basic_string<C>& str, size_t index, size_t length = npos, uint32_t flags = 0) {
        UnicornDetail::check_length_flags(flags);
        auto b = utf_begin(str), e = utf_end(str);
        auto i = str_find_index(b, e, index, flags);
        auto j = length == npos ? e : str_find_index(i, e, length, flags);
        return u_str(i, j);
    }

    template <typename C>
    basic_string<C> str_translate(const basic_string<C>& str, const basic_string<C>& target, const basic_string<C>& sub) {
        using string_type = basic_string<C>;
        if (target.empty() || sub.empty())
            return str;
        auto t = to_utf32(target), s = to_utf32(sub);
        if (s.size() < t.size())
            s.resize(t.size(), s.back());
        string_type dst;
        dst.reserve(str.size());
        for (auto c: utf_range(str)) {
            size_t pos = t.find(c);
            if (pos != npos)
                c = s[pos];
            str_append_char(dst, c);
        }
        return dst;
    }

    template <typename C>
    basic_string<C> str_translate(const basic_string<C>& str, const C* target, const basic_string<C>& sub) {
        return str_translate(str, cstr(target), sub);
    }

    template <typename C>
    basic_string<C> str_translate(const basic_string<C>& str, const basic_string<C>& target, const C* sub) {
        return str_translate(str, target, cstr(sub));
    }

    template <typename C>
    basic_string<C> str_translate(const basic_string<C>& str, const C* target, const C* sub) {
        return str_translate(str, cstr(target), cstr(sub));
    }

    template <typename C>
    void str_translate_in(basic_string<C>& str, const basic_string<C>& target, const basic_string<C>& sub) {
        auto result = str_translate(str, target, sub);
        str.swap(result);
    }

    template <typename C>
    void str_translate_in(basic_string<C>& str, const C* target, const basic_string<C>& sub) {
        str_translate_in(str, cstr(target), sub);
    }

    template <typename C>
    void str_translate_in(basic_string<C>& str, const basic_string<C>& target, const C* sub) {
        str_translate_in(str, target, cstr(sub));
    }

    template <typename C>
    void str_translate_in(basic_string<C>& str, const C* target, const C* sub) {
        str_translate_in(str, cstr(target), cstr(sub));
    }

    namespace UnicornDetail {

        enum { trimleft = 1, trimright = 2 };

        class CharIn {
        public:
            CharIn(const u32string& chars) noexcept: x(&chars) {}
            bool operator()(char32_t c) const noexcept { return x->find(c) != npos; }
        private:
            const u32string* x;
        };

        template <typename String, typename Pred>
        String trim_helper(const String& src, int mode, Pred p) {
            auto range = utf_range(src);
            auto i = std::begin(range), j = std::end(range);
            if (mode & trimleft)
                while (i != j && p(*i))
                    ++i;
            if ((mode & trimright) && i != j) {
                for (;;) {
                    --j;
                    if (! p(*j)) {
                        ++j;
                        break;
                    }
                    if (i == j)
                        break;
                }
            }
            return u_str(i, j);
        }

        template <typename String, typename Pred>
        void trim_in_helper(String& src, int mode, Pred p) {
            auto range = utf_range(src);
            auto i = std::begin(range), j = std::end(range);
            if (mode & trimleft)
                while (i != j && p(*i))
                    ++i;
            if ((mode & trimright) && i != j) {
                for (;;) {
                    --j;
                    if (! p(*j)) {
                        ++j;
                        break;
                    }
                    if (i == j)
                        break;
                }
            }
            src.erase(j.offset(), npos);
            src.erase(0, i.offset());
        }

    }

    template <typename C, typename Pred>
    basic_string<C> str_trim_if(const basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft | trimright, p);
    }

    template <typename C, typename Pred>
    basic_string<C> str_trim_left_if(const basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft, p);
    }

    template <typename C, typename Pred>
    basic_string<C> str_trim_right_if(const basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimright, p);
    }

    template <typename C, typename Pred>
    basic_string<C> str_trim_if_not(const basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft | trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename C, typename Pred>
    basic_string<C> str_trim_left_if_not(const basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft, [p] (char32_t c) { return ! p(c); });
    }

    template <typename C, typename Pred>
    basic_string<C> str_trim_right_if_not(const basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename C>
    basic_string<C> str_trim(const basic_string<C>& str, const basic_string<C>& chars) {
        return str_trim_if(str, UnicornDetail::CharIn(to_utf32(chars)));
    }

    template <typename C>
    basic_string<C> str_trim(const basic_string<C>& str, const C* chars) {
        return str_trim_if(str, UnicornDetail::CharIn(to_utf32(cstr(chars))));
    }

    template <typename C>
    basic_string<C> str_trim(const basic_string<C>& str) {
        return str_trim_if(str, char_is_white_space);
    }

    template <typename C>
    basic_string<C> str_trim_left(const basic_string<C>& str, const basic_string<C>& chars) {
        return str_trim_left_if(str, UnicornDetail::CharIn(to_utf32(chars)));
    }

    template <typename C>
    basic_string<C> str_trim_left(const basic_string<C>& str, const C* chars) {
        return str_trim_left_if(str, UnicornDetail::CharIn(to_utf32(cstr(chars))));
    }

    template <typename C>
    basic_string<C> str_trim_left(const basic_string<C>& str) {
        return str_trim_left_if(str, char_is_white_space);
    }

    template <typename C>
    basic_string<C> str_trim_right(const basic_string<C>& str, const basic_string<C>& chars) {
        return str_trim_right_if(str, UnicornDetail::CharIn(to_utf32(chars)));
    }

    template <typename C>
    basic_string<C> str_trim_right(const basic_string<C>& str, const C* chars) {
        return str_trim_right_if(str, UnicornDetail::CharIn(to_utf32(cstr(chars))));
    }

    template <typename C>
    basic_string<C> str_trim_right(const basic_string<C>& str) {
        return str_trim_right_if(str, char_is_white_space);
    }

    template <typename C, typename Pred>
    void str_trim_in_if(basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft | trimright, p);
    }

    template <typename C, typename Pred>
    void str_trim_left_in_if(basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft, p);
    }

    template <typename C, typename Pred>
    void str_trim_right_in_if(basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimright, p);
    }

    template <typename C, typename Pred>
    void str_trim_in_if_not(basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft | trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename C, typename Pred>
    void str_trim_left_in_if_not(basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft, [p] (char32_t c) { return ! p(c); });
    }

    template <typename C, typename Pred>
    void str_trim_right_in_if_not(basic_string<C>& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename C>
    void str_trim_in(basic_string<C>& str, const basic_string<C>& chars) {
        str_trim_in_if(str, UnicornDetail::CharIn(to_utf32(chars)));
    }

    template <typename C>
    void str_trim_in(basic_string<C>& str, const C* chars) {
        str_trim_in_if(str, UnicornDetail::CharIn(to_utf32(cstr(chars))));
    }

    template <typename C>
    void str_trim_in(basic_string<C>& str) {
        str_trim_in_if(str, char_is_white_space);
    }

    template <typename C>
    void str_trim_left_in(basic_string<C>& str, const basic_string<C>& chars) {
        str_trim_left_in_if(str, UnicornDetail::CharIn(to_utf32(chars)));
    }

    template <typename C>
    void str_trim_left_in(basic_string<C>& str, const C* chars) {
        str_trim_left_in_if(str, UnicornDetail::CharIn(to_utf32(cstr(chars))));
    }

    template <typename C>
    void str_trim_left_in(basic_string<C>& str) {
        str_trim_left_in_if(str, char_is_white_space);
    }

    template <typename C>
    void str_trim_right_in(basic_string<C>& str, const basic_string<C>& chars) {
        str_trim_right_in_if(str, UnicornDetail::CharIn(to_utf32(chars)));
    }

    template <typename C>
    void str_trim_right_in(basic_string<C>& str, const C* chars) {
        str_trim_right_in_if(str, UnicornDetail::CharIn(to_utf32(cstr(chars))));
    }

    template <typename C>
    void str_trim_right_in(basic_string<C>& str) {
        str_trim_right_in_if(str, char_is_white_space);
    }

    template <typename C>
    basic_string<C> str_unify_lines(const basic_string<C>& str, const basic_string<C>& newline) {
        using string_type = basic_string<C>;
        auto i = utf_begin(str), e = utf_end(str);
        string_type result;
        while (i != e) {
            auto j = std::find_if(i, e, char_is_line_break);
            result += u_str(i, j);
            while (j != e && char_is_line_break(*j)) {
                result += newline;
                auto c = *j;
                ++j;
                if (j != e && c == PRI_CHAR('\r', C) && *j == PRI_CHAR('\n', C))
                    ++j;
            }
            i = j;
        }
        return result;
    }

    template <typename C>
    basic_string<C> str_unify_lines(const basic_string<C>& str, const C* newline) {
        return str_unify_lines(str, cstr(newline));
    }

    template <typename C>
    basic_string<C> str_unify_lines(const basic_string<C>& str, char32_t newline) {
        return str_unify_lines(str, str_chars<C>(newline));
    }

    template <typename C>
    basic_string<C> str_unify_lines(const basic_string<C>& str) {
        return str_unify_lines(str, basic_string<C>(1, PRI_CHAR('\n', C)));
    }

    template <typename C>
    void str_unify_lines_in(basic_string<C>& str, const basic_string<C>& newline) {
        auto result = str_unify_lines(str, newline);
        str.swap(result);
    }

    template <typename C>
    void str_unify_lines_in(basic_string<C>& str, const C* newline) {
        str_unify_lines_in(str, cstr(newline));
    }

    template <typename C>
    void str_unify_lines_in(basic_string<C>& str, char32_t newline) {
        str_unify_lines_in(str, str_chars<C>(newline));
    }

    template <typename C>
    void str_unify_lines_in(basic_string<C>& str) {
        str_unify_lines_in(str, basic_string<C>(1, PRI_CHAR('\n', C)));
    }

    namespace UnicornDetail {

        template <typename C>
        void check_whitespace(const UtfIterator<C>& i, const UtfIterator<C>& j, size_t& linebreaks, size_t& tailspaces) {
            linebreaks = tailspaces = 0;
            auto k = i;
            while (k != j) {
                auto c = *k;
                if (char_is_line_break(c))
                    ++linebreaks;
                ++k;
                if (k != j && c == PRI_CHAR('\r', C) && *k == PRI_CHAR('\n', C))
                    ++k;
                if (char_is_inline_space(c))
                    ++tailspaces;
                else
                    tailspaces = 0;
            }
        }

    }

    template <typename C>
    basic_string<C> str_wrap(const basic_string<C>& str, uint32_t flags = 0, size_t width = 0, size_t margin1 = 0, size_t margin2 = npos) {
        using namespace UnicornDetail;
        using string_type = basic_string<C>;
        UnicornDetail::allow_flags(flags, wrap_crlf | wrap_enforce | wrap_preserve | all_length_flags, "word wrapping");
        if (width == 0 || width == npos) {
            auto columns = decnum(cstr(getenv("COLUMNS")));
            if (columns < 3)
                columns = 80;
            width = size_t(columns) - 2;
        }
        if (margin2 == npos)
            margin2 = margin1;
        if (margin1 >= width || margin2 >= width)
            throw std::length_error("Word wrap width and margins are inconsistent");
        size_t spacing = flags & wide_context ? 2 : 1;
        string_type newline, result;
        if (flags & wrap_crlf)
            newline = {PRI_CHAR('\r', C), PRI_CHAR('\n', C)};
        else
            newline = {PRI_CHAR('\n', C)};
        auto range = utf_range(str);
        auto i = std::begin(range), e = std::end(range);
        size_t linewidth = 0, words = 0, linebreaks = 0, spaces = margin1, tailspaces = 0;
        while (i != e) {
            auto j = std::find_if_not(i, e, char_is_white_space);
            if (j == e)
                break;
            check_whitespace(i, j, linebreaks, tailspaces);
            if (! result.empty() && linebreaks >= 2) {
                if (words > 0) {
                    result += newline;
                    words = linewidth = 0;
                }
                result += newline;
                spaces = margin1;
            }
            i = j;
            if ((flags & wrap_preserve) && linebreaks >= 1 && tailspaces >= 1) {
                if (words > 0)
                    result += newline;
                result.append(tailspaces, PRI_CHAR(' ', C));
                j = std::find_if(i, e, char_is_line_break);
                result += str_unify_lines(u_str(i, j), newline);
                result += newline;
                words = linewidth = 0;
            } else {
                j = std::find_if(i, e, char_is_white_space);
                auto word = u_str(i, j);
                auto wordlen = str_length(word, flags & all_length_flags);
                if (words > 0) {
                    if (linewidth + wordlen + spacing > size_t(width)) {
                        result += newline;
                        words = linewidth = 0;
                    } else {
                        result += PRI_CHAR(' ', C);
                        linewidth += spacing;
                    }
                }
                if (words == 0) {
                    result.append(spaces, PRI_CHAR(' ', C));
                    linewidth = spaces * spacing;
                    spaces = margin2;
                }
                result += word;
                ++words;
                linewidth += wordlen;
                if ((flags & wrap_enforce) && linewidth > size_t(width))
                    throw std::length_error("Word is too long for wrapping width");
            }
            i = j;
        }
        if (words > 0)
            result += newline;
        return result;
    }

    template <typename C>
    void str_wrap_in(basic_string<C>& str, uint32_t flags = 0, size_t width = 0, size_t margin1 = 0, size_t margin2 = npos) {
        auto result = str_wrap(str, flags, width, margin1, margin2);
        str.swap(result);
    }

    // Case mapping functions

    namespace UnicornDetail {

        template <typename C, typename F>
        const basic_string<C> casemap_helper(const basic_string<C>& src, F f) {
            basic_string<C> dst;
            char32_t buf[max_case_decomposition];
            auto out = utf_writer(dst);
            for (auto c: utf_range(src)) {
                auto n = f(c, buf);
                std::copy_n(buf, n, out);
            }
            return dst;
        }

        template <typename FwdIter>
        bool next_cased(FwdIter i, FwdIter e) {
            if (i == e)
                return false;
            for (++i; i != e; ++i)
                if (! char_is_case_ignorable(*i))
                    return char_is_cased(*i);
            return false;
        }

        struct LowerChar {
            static constexpr char32_t sigma = 0x3c3;
            static constexpr char32_t final_sigma = 0x3c2;
            bool last_cased = false;
            char32_t buf[max_case_decomposition];
            template <typename FwdIter, typename OutIter>
            void convert(FwdIter i, FwdIter e, OutIter to) {
                auto n = char_to_full_lowercase(*i, buf);
                if (buf[0] == sigma && last_cased && ! next_cased(i, e))
                    buf[0] = final_sigma;
                std::copy_n(buf, n, to);
                if (! char_is_case_ignorable(*i))
                    last_cased = char_is_cased(*i);
            }
        };

    }

    template <typename C>
    basic_string<C> str_uppercase(const basic_string<C>& str) {
        return UnicornDetail::casemap_helper(str, char_to_full_uppercase);
    }

    template <typename C>
    basic_string<C> str_lowercase(const basic_string<C>& str) {
        basic_string<C> dst;
        UnicornDetail::LowerChar lc;
        auto range = utf_range(str);
        auto out = utf_writer(dst);
        for (auto i = std::begin(range), e = std::end(range); i != e; ++i)
            lc.convert(i, e, out);
        return dst;
    }

    template <typename C>
    basic_string<C> str_titlecase(const basic_string<C>& str) {
        basic_string<C> dst;
        UnicornDetail::LowerChar lc;
        auto e = utf_end(str);
        auto out = utf_writer(dst);
        for (auto& w: word_range(str)) {
            bool initial = true;
            for (auto i = std::begin(w); i != std::end(w); ++i) {
                if (initial && char_is_cased(*i)) {
                    auto n = char_to_full_titlecase(*i, lc.buf);
                    std::copy_n(lc.buf, n, out);
                    lc.last_cased = true;
                    initial = false;
                } else {
                    lc.convert(i, e, out);
                }
            }
        }
        return dst;
    }

    template <typename C>
    basic_string<C> str_casefold(const basic_string<C>& str) {
        return UnicornDetail::casemap_helper(str, char_to_full_casefold);
    }

    template <typename C>
    void str_uppercase_in(basic_string<C>& str) {
        auto result = str_uppercase(str);
        str.swap(result);
    }

    template <typename C>
    void str_lowercase_in(basic_string<C>& str) {
        auto result = str_lowercase(str);
        str.swap(result);
    }

    template <typename C>
    void str_titlecase_in(basic_string<C>& str) {
        auto result = str_titlecase(str);
        str.swap(result);
    }

    template <typename C>
    void str_casefold_in(basic_string<C>& str) {
        auto result = str_casefold(str);
        str.swap(result);
    }

    struct IcaseCompare {
        template <typename C>
        bool operator()(const basic_string<C>& lhs, const basic_string<C>& rhs) const noexcept {
            if (lhs.empty() || rhs.empty())
                return ! rhs.empty();
            auto u1 = utf_range(lhs), u2 = utf_range(rhs);
            auto i1 = std::begin(u1), i2 = std::begin(u2);
            char32_t buf1[max_case_decomposition], buf2[max_case_decomposition];
            size_t p1 = 0, p2 = 0;
            size_t n1 = char_to_full_casefold(*i1, buf1);
            size_t n2 = char_to_full_casefold(*i2, buf2);
            for (;;) {
                if (p1 == n1) {
                    p1 = 0;
                    if (++i1 != std::end(u1))
                        n1 = char_to_full_casefold(*i1, buf1);
                }
                if (p2 == n2) {
                    p2 = 0;
                    if (++i2 != std::end(u2))
                        n2 = char_to_full_casefold(*i2, buf2);
                }
                if ((p1 == 0 && i1 == std::end(u1)) || (p2 == 0 && i2 == std::end(u2)))
                    return i2 != std::end(u2);
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
            auto i1 = std::begin(u1), i2 = std::begin(u2);
            char32_t buf1[max_case_decomposition], buf2[max_case_decomposition];
            size_t p1 = 0, p2 = 0;
            size_t n1 = char_to_full_casefold(*i1, buf1);
            size_t n2 = char_to_full_casefold(*i2, buf2);
            for (;;) {
                if (p1 == n1) {
                    p1 = 0;
                    if (++i1 != std::end(u1))
                        n1 = char_to_full_casefold(*i1, buf1);
                }
                if (p2 == n2) {
                    p2 = 0;
                    if (++i2 != std::end(u2))
                        n2 = char_to_full_casefold(*i2, buf2);
                }
                if ((p1 == 0 && i1 == std::end(u1)) || (p2 == 0 && i2 == std::end(u2)))
                    return i1 == std::end(u1) && i2 == std::end(u2);
                if (buf1[p1] != buf2[p2])
                    return false;
                ++p1;
                ++p2;
            }
        }
    };

    constexpr IcaseCompare str_icase_compare {};
    constexpr IcaseEqual str_icase_equal {};

    // Escaping and quoting functions

    UNICORN_DEFINE_FLAG(string escaping, esc_control,    0);  // Escape only C0 controls and backslash (default)
    UNICORN_DEFINE_FLAG(string escaping, esc_latin,      1);  // Escape C0 and C1 controls
    UNICORN_DEFINE_FLAG(string escaping, esc_nonascii,   2);  // Escape all characters that are not printable ASCII
    UNICORN_DEFINE_FLAG(string escaping, esc_punct,      3);  // Escape ASCII punctuation as well as all non-ASCII
    UNICORN_DEFINE_FLAG(string escaping, esc_keeplf,     4);  // Do not escape line feeds
    UNICORN_DEFINE_FLAG(string escaping, esc_uhex,       5);  // Use `\u` and `\U` instead of `\x`
    UNICORN_DEFINE_FLAG(string escaping, esc_surrogate,  6);  // Use two `\u` escapes for astral characters
    UNICORN_DEFINE_FLAG(string escaping, esc_utf8,       7);  // Use UTF-8 hex bytes for non-ASCII characters

    // TODO

    template <typename C> basic_string<C> str_encode_uri(const basic_string<C>& str);
    template <typename C> basic_string<C> str_encode_uri_component(const basic_string<C>& str);
    template <typename C> void str_encode_uri_in(basic_string<C>& str);
    template <typename C> void str_encode_uri_in_component(basic_string<C>& str);

    template <typename C> basic_string<C> str_escape(const basic_string<C>& str, uint32_t flags = 0);
    template <typename C> void str_escape_in(basic_string<C>& str, uint32_t flags = 0);

    template <typename C> basic_string<C> str_quote(const basic_string<C>& str);
    template <typename C> basic_string<C> str_quote(const basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0);
    template <typename C> basic_string<C> str_quote(const basic_string<C>& str, const C* quotes, uint32_t flags = 0);
    template <typename C> void str_quote_in(basic_string<C>& str);
    template <typename C> void str_quote_in(basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0);
    template <typename C> void str_quote_in(basic_string<C>& str, const C* quotes, uint32_t flags = 0);

    template <typename C> basic_string<C> str_unencode_uri(const basic_string<C>& str);
    template <typename C> void str_unencode_uri_in(basic_string<C>& str);

    template <typename C> basic_string<C> str_unescape(const basic_string<C>& str, uint32_t flags = 0);
    template <typename C> void str_unescape_in(basic_string<C>& str, uint32_t flags = 0);

    template <typename C> basic_string<C> str_unquote(const basic_string<C>& str);
    template <typename C> basic_string<C> str_unquote(const basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0);
    template <typename C> basic_string<C> str_unquote(const basic_string<C>& str, const C* quotes, uint32_t flags = 0);
    template <typename C> void str_unquote_in(basic_string<C>& str);
    template <typename C> void str_unquote_in(basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0);
    template <typename C> void str_unquote_in(basic_string<C>& str, const C* quotes, uint32_t flags = 0);

    // Type conversion functions

    namespace UnicornDetail {

        template <typename T, typename C>
        size_t string_to_integer(T& t, const basic_string<C>& str, size_t offset, int base) {
            PRI_STATIC_ASSERT(std::is_integral<T>::value);
            static const auto dec_chars = recode<C>(u8string("+-0123456789"));
            static const auto hex_chars = recode<C>(u8string("+-0123456789ABCDEFabcdef"));
            t = static_cast<T>(0);
            if (offset > str.size())
                return 0;
            size_t endpos = str.find_first_not_of(base == 16 ? hex_chars : dec_chars, offset);
            if (endpos == offset)
                return 0;
            if (endpos == npos)
                endpos = str.size();
            u8string fragment(endpos - offset, 0);
            std::transform(std::begin(str) + offset, std::begin(str) + endpos, std::begin(fragment),
                [] (C c) { return char(c); });
            char* endptr = nullptr;
            if (std::is_signed<T>::value) {
                auto value = strtoll(fragment.data(), &endptr, base);
                value = clamp(value, std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
                t = static_cast<T>(value);
            } else {
                auto value = strtoull(fragment.data(), &endptr, base);
                value = clamp(value, 0, std::numeric_limits<T>::max());
                t = static_cast<T>(value);
            }
            return endptr - fragment.data();
        }

    }

    template <typename T, typename C>
    size_t str_to_integer(T& t, const basic_string<C>& str, size_t offset = 0) {
        return UnicornDetail::string_to_integer<T>(t, str, offset, 10);
    }

    template <typename T, typename C>
    T str_to_integer(const basic_string<C>& str, size_t offset = 0) {
        auto t = static_cast<T>(0);
        UnicornDetail::string_to_integer<T>(t, str, offset, 10);
        return t;
    }

    template <typename T, typename C>
    size_t hex_to_integer(T& t, const basic_string<C>& str, size_t offset = 0) {
        return UnicornDetail::string_to_integer<T>(t, str, offset, 16);
    }

    template <typename T, typename C>
    T hex_to_integer(const basic_string<C>& str, size_t offset = 0) {
        auto t = static_cast<T>(0);
        UnicornDetail::string_to_integer<T>(t, str, offset, 16);
        return t;
    }

    template <typename T, typename C>
    size_t str_to_real(T& t, const basic_string<C>& str, size_t offset = 0) {
        PRI_STATIC_ASSERT(std::is_floating_point<T>::value);
        static const auto float_chars = recode<C>(u8string("+-.0123456789Ee"));
        t = static_cast<T>(0);
        if (offset > str.size())
            return 0;
        size_t endpos = str.find_first_not_of(float_chars, offset);
        if (endpos == offset)
            return 0;
        if (endpos == npos)
            endpos = str.size();
        u8string fragment(endpos - offset, 0);
        std::transform(std::begin(str) + offset, std::begin(str) + endpos, std::begin(fragment),
            [] (C c) { return char(c); });
        char* endptr = nullptr;
        auto value = strtold(fragment.data(), &endptr);
        value = clamp(value, - std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
        t = static_cast<T>(value);
        return endptr - fragment.data();
    }

    template <typename T, typename C>
    T str_to_real(const basic_string<C>& str, size_t offset = 0) {
        auto t = static_cast<T>(0);
        str_to_real(t, str, offset);
        return t;
    }

}
