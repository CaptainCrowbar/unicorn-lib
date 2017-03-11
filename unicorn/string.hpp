#pragma once

#include "unicorn/character.hpp"
#include "unicorn/core.hpp"
#include "unicorn/segment.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace Unicorn {

    // Early declarations for functions used internally

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

    // String size functions

    // Remember that any other set of flags that might be combined with these
    // needs to skip the bits that are already spoken for. The string length
    // flags are linked to upper case letters for use with unicorn/format, so
    // they have bit positions in the 0-25 range.

    constexpr uint32_t character_units  = letter_to_mask('C');  // Measure string in characters (default)
    constexpr uint32_t grapheme_units   = letter_to_mask('G');  // Measure string in grapheme clusters
    constexpr uint32_t narrow_context   = letter_to_mask('N');  // East Asian width, defaulting to narrow
    constexpr uint32_t wide_context     = letter_to_mask('W');  // East Asian width, defaulting to wide

    namespace UnicornDetail {

        constexpr auto east_asian_flags = narrow_context | wide_context;
        constexpr auto all_length_flags = character_units | grapheme_units | east_asian_flags;

        inline bool char_is_advancing(char32_t c) {
            return char_general_category(c) != GC::Mn;
        }

        template <typename C>
        inline bool grapheme_is_advancing(Irange<UtfIterator<C>> g) {
            return ! g.empty() && char_is_advancing(*g.begin());
        }

        inline void check_length_flags(uint32_t& flags) {
            if (bits_set(flags & (character_units | grapheme_units)) > 1
                    || bits_set(flags & (character_units | east_asian_flags)) > 1)
                throw std::invalid_argument("Inconsistent string length flags");
            if (bits_set(flags & all_length_flags) == 0)
                flags |= character_units;
        }

        class EastAsianCount {
        public:
            explicit EastAsianCount(uint32_t flags) noexcept: count(), fset(flags) { memset(count, 0, sizeof(count)); }
            void add(char32_t c) noexcept {
                if (char_is_advancing(c))
                    ++count[unsigned(east_asian_width(c))];
            }
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
        pair<UtfIterator<C>, bool> find_position(const Irange<UtfIterator<C>>& range, size_t pos, uint32_t flags = 0) {
            check_length_flags(flags);
            if (pos == 0)
                return {range.begin(), true};
            if (flags & character_units) {
                auto i = range.begin();
                size_t len = 0;
                while (i != range.end() && len < pos) {
                    ++i;
                    ++len;
                }
                return {i, len == pos};
            } else if (flags & east_asian_flags) {
                EastAsianCount eac(flags);
                if (flags & grapheme_units) {
                    auto graph = grapheme_range(range);
                    auto g = graph.begin();
                    while (g != graph.end() && eac.get() < pos) {
                        eac.add(*g->begin());
                        ++g;
                    }
                    return {g->begin(), eac.get() >= pos};
                } else {
                    auto i = range.begin();
                    while (i != range.end() && eac.get() < pos) {
                        eac.add(*i);
                        ++i;
                    }
                    return {i, eac.get() >= pos};
                }
            } else {
                auto graph = grapheme_range(range);
                size_t len = 0;
                for (auto& g: graph) {
                    if (grapheme_is_advancing(g)) {
                        ++len;
                        if (len == pos)
                            return {g.end(), true};
                    }
                }
                return {range.end(), len == pos};
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
                    eac.add(*g.begin());
            } else {
                for (auto c: range)
                    eac.add(c);
            }
            return eac.get();
        } else {
            auto gr = grapheme_range(range);
            return std::count_if(gr.begin(), gr.end(), grapheme_is_advancing<C>);
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

    // Other string properties
    // Defined in string-property.cpp

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

    // String comparison
    // Defined in string-compare.cpp

    int str_compare_3way(const u8string& lhs, const u8string& rhs);
    bool str_icase_compare(const u8string& lhs, const u8string& rhs) noexcept;
    bool str_icase_equal(const u8string& lhs, const u8string& rhs) noexcept;
    bool str_natural_compare(const u8string& lhs, const u8string& rhs) noexcept;

    // Other string algorithms
    // Defined in string-algorithm.cpp

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
    void str_line_column(const u8string& str, size_t offset, size_t& line, size_t& column, size_t flags = 0);
    Utf8Iterator str_search(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target);
    Utf8Iterator str_search(const Irange<Utf8Iterator>& range, const u8string& target);
    Utf8Iterator str_search(const u8string& str, const u8string& target);
    size_t str_skipws(Utf8Iterator& i, const Utf8Iterator& end);
    size_t str_skipws(Utf8Iterator& i);

    // String manipulation functions
    // Defined in string-manip.cpp

    constexpr uint32_t wrap_crlf      = 1ul << 26;  // Use CR+LF for line breaks (default LF)
    constexpr uint32_t wrap_enforce   = 1ul << 27;  // Enforce right margin strictly
    constexpr uint32_t wrap_preserve  = 1ul << 28;  // Preserve layout on already indented lines

    namespace UnicornDetail {

        enum { trimleft = 1, trimright = 2 };

        u8string expand_tabs(const u8string& str, const vector<size_t>& tabs, uint32_t flags);

        template <typename DS>
        void concat_helper(u8string&, const DS&) {}

        template <typename DS, typename S2, typename... Strings>
        void concat_helper(u8string& s1, const DS& delim, const S2& s2, const Strings&... ss) {
            str_append(s1, delim);
            str_append(s1, s2);
            concat_helper(s1, delim, ss...);
        }

        template <typename Pred>
        u8string trim_helper(const u8string& src, int mode, Pred p) {
            auto range = utf_range(src);
            auto i = range.begin(), j = range.end();
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

        template <typename Pred>
        void trim_in_helper(u8string& src, int mode, Pred p) {
            auto range = utf_range(src);
            auto i = range.begin(), j = range.end();
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

    u8string str_drop_prefix(const u8string& str, const u8string& prefix);
    void str_drop_prefix_in(u8string& str, const u8string& prefix) noexcept;
    u8string str_drop_suffix(const u8string& str, const u8string& suffix);
    void str_drop_suffix_in(u8string& str, const u8string& suffix) noexcept;
    u8string str_erase_left(const u8string& str, size_t length);
    void str_erase_left_in(u8string& str, size_t length) noexcept;
    u8string str_erase_right(const u8string& str, size_t length);
    void str_erase_right_in(u8string& str, size_t length) noexcept;
    u8string str_expand_tabs(const u8string& str);
    void str_expand_tabs_in(u8string& str);
    u8string str_fix_left(const u8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    void str_fix_left_in(u8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    u8string str_fix_right(const u8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    void str_fix_right_in(u8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    u8string str_insert(const Utf8Iterator& dst, const Utf8Iterator& src_begin, const Utf8Iterator& src_end);
    u8string str_insert(const Utf8Iterator& dst, const Irange<Utf8Iterator>& src);
    u8string str_insert(const Utf8Iterator& dst, const u8string& src);
    u8string str_insert(const Utf8Iterator& dst_begin, const Utf8Iterator& dst_end,
            const Utf8Iterator& src_begin, const Utf8Iterator& src_end);
    u8string str_insert(const Irange<Utf8Iterator>& dst, const Irange<Utf8Iterator>& src);
    u8string str_insert(const Utf8Iterator& dst_begin, const Utf8Iterator& dst_end, const u8string& src);
    u8string str_insert(const Irange<Utf8Iterator>& dst, const u8string& src);
    Irange<Utf8Iterator> str_insert_in(u8string& dst, const Utf8Iterator& where,
            const Utf8Iterator& src_begin, const Utf8Iterator& src_end);
    Irange<Utf8Iterator> str_insert_in(u8string& dst, const Utf8Iterator& where, const Irange<Utf8Iterator>& src);
    Irange<Utf8Iterator> str_insert_in(u8string& dst, const Utf8Iterator& where, const u8string& src);
    Irange<Utf8Iterator> str_insert_in(u8string& dst, const Utf8Iterator& range_begin, const Utf8Iterator& range_end,
            const Utf8Iterator& src_begin, const Utf8Iterator& src_end);
    Irange<Utf8Iterator> str_insert_in(u8string& dst, const Irange<Utf8Iterator>& range, const Irange<Utf8Iterator>& src);
    Irange<Utf8Iterator> str_insert_in(u8string& dst, const Utf8Iterator& range_begin, const Utf8Iterator& range_end,
            const u8string& src);
    Irange<Utf8Iterator> str_insert_in(u8string& dst, const Irange<Utf8Iterator>& range, const u8string& src);
    u8string str_pad_left(const u8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    void str_pad_left_in(u8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    u8string str_pad_right(const u8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    void str_pad_right_in(u8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    bool str_partition(const u8string& str, u8string& prefix, u8string& suffix);
    bool str_partition_at(const u8string& str, u8string& prefix, u8string& suffix, const u8string& delim);
    bool str_partition_by(const u8string& str, u8string& prefix, u8string& suffix, const u8string& delim);
    u8string str_remove(const u8string& str, char32_t c);
    u8string str_remove(const u8string& str, const u8string& chars);
    void str_remove_in(u8string& str, char32_t c);
    void str_remove_in(u8string& str, const u8string& chars);
    u8string str_replace(const u8string& str, const u8string& target, const u8string& sub, size_t n = npos);
    void str_replace_in(u8string& str, const u8string& target, const u8string& sub, size_t n = npos);
    vector<u8string> str_splitv(const u8string& src);
    vector<u8string> str_splitv_at(const u8string& src, const u8string& delim);
    vector<u8string> str_splitv_by(const u8string& src, const u8string& delim);
    u8string str_squeeze(const u8string& str);
    u8string str_squeeze(const u8string& str, const u8string& chars);
    u8string str_squeeze_trim(const u8string& str);
    u8string str_squeeze_trim(const u8string& str, const u8string& chars);
    void str_squeeze_in(u8string& str);
    void str_squeeze_in(u8string& str, const u8string& chars);
    void str_squeeze_trim_in(u8string& str);
    void str_squeeze_trim_in(u8string& str, const u8string& chars);
    u8string str_substring(const u8string& str, size_t offset, size_t count = npos);
    u8string utf_substring(const u8string& str, size_t index, size_t length = npos, uint32_t flags = 0);
    u8string str_translate(const u8string& str, const u8string& target, const u8string& sub);
    void str_translate_in(u8string& str, const u8string& target, const u8string& sub);
    u8string str_trim(const u8string& str, const u8string& chars);
    u8string str_trim(const u8string& str);
    u8string str_trim_left(const u8string& str, const u8string& chars);
    u8string str_trim_left(const u8string& str);
    u8string str_trim_right(const u8string& str, const u8string& chars);
    u8string str_trim_right(const u8string& str);
    void str_trim_in(u8string& str, const u8string& chars);
    void str_trim_in(u8string& str);
    void str_trim_left_in(u8string& str, const u8string& chars);
    void str_trim_left_in(u8string& str);
    void str_trim_right_in(u8string& str, const u8string& chars);
    void str_trim_right_in(u8string& str);
    u8string str_unify_lines(const u8string& str, const u8string& newline);
    u8string str_unify_lines(const u8string& str, char32_t newline);
    u8string str_unify_lines(const u8string& str);
    void str_unify_lines_in(u8string& str, const u8string& newline);
    void str_unify_lines_in(u8string& str, char32_t newline);
    void str_unify_lines_in(u8string& str);
    u8string str_wrap(const u8string& str, uint32_t flags = 0, size_t width = 0, size_t margin1 = 0, size_t margin2 = npos);
    void str_wrap_in(u8string& str, uint32_t flags = 0, size_t width = 0, size_t margin1 = 0, size_t margin2 = npos);

    template <typename C, typename... Strings>
    u8string str_concat(const basic_string<C>& s, const Strings&... ss) {
        u8string result;
        UnicornDetail::concat_helper(result, basic_string<C>(), s, ss...);
        return result;
    }

    template <typename C, typename... Strings>
    u8string str_concat(const C* s, const Strings&... ss) {
        u8string result;
        UnicornDetail::concat_helper(result, basic_string<C>(), cstr(s), ss...);
        return result;
    }

    template <typename C>
    u8string str_concat_with(const basic_string<C>& /*delim*/) {
        return {};
    }

    template <typename C>
    u8string str_concat_with(const C* /*delim*/) {
        return {};
    }

    template <typename C1, typename C2, typename... Strings>
    u8string str_concat_with(const basic_string<C1>& delim, const basic_string<C2>& s, const Strings&... ss) {
        u8string result = to_utf8(s);
        UnicornDetail::concat_helper(result, delim, ss...);
        return result;
    }

    template <typename C1, typename C2, typename... Strings>
    u8string str_concat_with(const basic_string<C1>& delim, const C2* s, const Strings&... ss) {
        u8string result = to_utf8(cstr(s));
        UnicornDetail::concat_helper(result, delim, ss...);
        return result;
    }

    template <typename C1, typename C2, typename... Strings>
    u8string str_concat_with(const C1* delim, const basic_string<C2>& s, const Strings&... ss) {
        u8string result = to_utf8(s);
        UnicornDetail::concat_helper(result, cstr(delim), ss...);
        return result;
    }

    template <typename C1, typename C2, typename... Strings>
    u8string str_concat_with(const C1* delim, const C2* s, const Strings&... ss) {
        u8string result = to_utf8(cstr(s));
        UnicornDetail::concat_helper(result, cstr(delim), ss...);
        return result;
    }

    template <typename IntList>
    u8string str_expand_tabs(const u8string& str, const IntList& tabs, uint32_t flags = 0) {
        vector<size_t> tv(tabs.begin(), tabs.end());
        return UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename IntType>
    u8string str_expand_tabs(const u8string& str, std::initializer_list<IntType> tabs, uint32_t flags = 0) {
        vector<size_t> tv(tabs.begin(), tabs.end());
        return UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename IntList>
    void str_expand_tabs_in(u8string& str, const IntList& tabs, uint32_t flags = 0) {
        vector<size_t> tv(tabs.begin(), tabs.end());
        str = UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename IntType>
    void str_expand_tabs_in(u8string& str, std::initializer_list<IntType> tabs, uint32_t flags = 0) {
        vector<size_t> tv(tabs.begin(), tabs.end());
        str = UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename FwdRange>
    u8string str_join(const FwdRange& r, const u8string& delim, bool term = false) {
        using std::begin;
        using std::end;
        u8string dst;
        for (auto& s: r) {
            dst += s;
            dst += delim;
        }
        if (! term && ! dst.empty() && ! delim.empty())
            dst.resize(dst.size() - delim.size());
        return dst;
    }

    template <typename FwdRange>
    auto str_join(const FwdRange& r) {
        return str_join(r, u8string());
    }

    template <typename Pred>
    u8string str_remove_if(const u8string& str, Pred p) {
        u8string dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [p] (char32_t x) { return ! p(x); });
        return dst;
    }

    template <typename Pred>
    u8string str_remove_if_not(const u8string& str, Pred p) {
        u8string dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), p);
        return dst;
    }

    template <typename Pred>
    void str_remove_in_if(u8string& str, Pred p) {
        u8string dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [p] (char32_t x) { return ! p(x); });
        str.swap(dst);
    }

    template <typename Pred>
    void str_remove_in_if_not(u8string& str, Pred p) {
        u8string dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), p);
        str.swap(dst);
    }

    template <typename OutIter>
    void str_split(const u8string& src, OutIter dst) {
        auto range = utf_range(src);
        auto i = range.begin(), j = i;
        while (i != range.end()) {
            j = std::find_if_not(i, range.end(), char_is_white_space);
            if (j == range.end())
                break;
            i = std::find_if(j, range.end(), char_is_white_space);
            *dst++ = u_str(j, i);
        }
    }

    template <typename OutIter>
    void str_split_at(const u8string& src, OutIter dst, const u8string& delim) {
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

    template <typename OutIter>
    void str_split_by(const u8string& src, OutIter dst, const u8string& delim) {
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
            auto i = range.begin(), j = i;
            while (i != range.end()) {
                j = std::find_if_not(i, range.end(), match);
                if (j == range.end())
                    break;
                i = std::find_if(j, range.end(), match);
                *dst++ = u_str(j, i);
            }
        }
    }

    template <typename OutIter>
    void str_split_lines(const u8string& src, OutIter dst) {
        auto i = utf_begin(src), j = i, e = utf_end(src);
        while (i != e) {
            j = std::find_if(i, e, char_is_line_break);
            *dst++ = u_str(i, j);
            if (j == e)
                break;
            char32_t c = *j;
            if (++j == e)
                break;
            if (c == U'\r' && *j == U'\n')
                ++j;
            i = j;
        }
    }

    template <typename Pred>
    u8string str_trim_if(const u8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft | trimright, p);
    }

    template <typename Pred>
    u8string str_trim_left_if(const u8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft, p);
    }

    template <typename Pred>
    u8string str_trim_right_if(const u8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimright, p);
    }

    template <typename Pred>
    u8string str_trim_if_not(const u8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft | trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    u8string str_trim_left_if_not(const u8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    u8string str_trim_right_if_not(const u8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    void str_trim_in_if(u8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft | trimright, p);
    }

    template <typename Pred>
    void str_trim_left_in_if(u8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft, p);
    }

    template <typename Pred>
    void str_trim_right_in_if(u8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimright, p);
    }

    template <typename Pred>
    void str_trim_in_if_not(u8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft | trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    void str_trim_left_in_if_not(u8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    void str_trim_right_in_if_not(u8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimright, [p] (char32_t c) { return ! p(c); });
    }

    // Case mapping functions
    // Defined in string-case.cpp

    u8string str_uppercase(const u8string& str);
    u8string str_lowercase(const u8string& str);
    u8string str_titlecase(const u8string& str);
    u8string str_casefold(const u8string& str);
    void str_uppercase_in(u8string& str);
    void str_lowercase_in(u8string& str);
    void str_titlecase_in(u8string& str);
    void str_casefold_in(u8string& str);

    // Escaping and quoting functions
    // Defined in string-escape.cpp

    constexpr uint32_t esc_ascii   = 1ul << 0;  // Escape all non-ASCII characters
    constexpr uint32_t esc_nostdc  = 1ul << 1;  // Do not use standard C symbols such as `\n`
    constexpr uint32_t esc_pcre    = 1ul << 2;  // Use `\x{...}` instead of `\u` and `\U` (implies `esc_nonascii`)
    constexpr uint32_t esc_punct   = 1ul << 3;  // Escape ASCII punctuation

    u8string str_encode_uri(const u8string& str);
    u8string str_encode_uri_component(const u8string& str);
    void str_encode_uri_in(u8string& str);
    void str_encode_uri_component_in(u8string& str);
    u8string str_unencode_uri(const u8string& str);
    void str_unencode_uri_in(u8string& str);
    u8string str_escape(const u8string& str, uint32_t flags = 0);
    void str_escape_in(u8string& str, uint32_t flags = 0);
    u8string str_unescape(const u8string& str);
    void str_unescape_in(u8string& str);
    u8string str_quote(const u8string& str, uint32_t flags = 0, char32_t quote = U'\"');
    void str_quote_in(u8string& str, uint32_t flags = 0, char32_t quote = U'\"');
    u8string str_unquote(const u8string& str, char32_t quote = U'\"');
    void str_unquote_in(u8string& str, char32_t quote = U'\"');

    // Type conversion functions

    namespace UnicornDetail {

        template <typename T>
        Utf8Iterator convert_str_to_int(T& t, const Utf8Iterator& start, uint32_t flags, int base) {
            static const u8string dec_chars = "+-0123456789";
            static const u8string hex_chars = "+-0123456789ABCDEFabcdef";
            const u8string& src(start.source());
            size_t offset = start.offset();
            if (offset >= src.size()) {
                if (flags & err_throw)
                    throw std::invalid_argument("Invalid integer (empty string)");
                t = T(0);
                return utf_end(src);
            }
            size_t endpos = src.find_first_not_of(base == 16 ? hex_chars : dec_chars, offset);
            if (endpos == offset) {
                if (flags & err_throw)
                    throw std::invalid_argument("Invalid integer: " + uquote(to_utf8(start.str())));
                t = T(0);
                return start;
            }
            if (endpos == npos)
                endpos = src.size();
            u8string fragment(src, offset, endpos - offset);
            Utf8Iterator stop;
            if (std::is_signed<T>::value) {
                static constexpr auto min_value = static_cast<long long>(std::numeric_limits<T>::min());
                static constexpr auto max_value = static_cast<long long>(std::numeric_limits<T>::max());
                char* endptr = nullptr;
                errno = 0;
                long long value = strtoll(fragment.data(), &endptr, base);
                int err = errno;
                size_t len = endptr - fragment.data();
                stop = utf_iterator(src, offset + len);
                if ((flags & err_throw) && stop != utf_end(src))
                    throw std::invalid_argument("Invalid integer: " + uquote(fragment));
                if (len == 0) {
                    if (flags & err_throw)
                        throw std::invalid_argument("Invalid integer: " + uquote(u_str(start, utf_end(src))));
                    t = T(0);
                } else if (err == ERANGE || value < min_value || value > max_value) {
                    if (flags & err_throw)
                        throw std::range_error("Integer out of range: " + uquote(fragment));
                    t = value > T() ? max_value : min_value;
                } else {
                    t = value;
                }
            } else {
                static constexpr auto max_value = static_cast<unsigned long long>(std::numeric_limits<T>::max());
                char* endptr = nullptr;
                errno = 0;
                unsigned long long value = strtoull(fragment.data(), &endptr, base);
                int err = errno;
                size_t len = endptr - fragment.data();
                stop = utf_iterator(src, offset + len);
                if ((flags & err_throw) && stop != utf_end(src))
                        throw std::invalid_argument("Invalid integer: " + uquote(u_str(start, utf_end(src))));
                if (len == 0) {
                    if (flags & err_throw)
                        throw std::invalid_argument("Invalid integer: " + uquote(fragment));
                    t = T(0);
                } else if (err == ERANGE || value > max_value) {
                    if (flags & err_throw)
                        throw std::range_error("Integer out of range: " + uquote(fragment));
                    t = max_value;
                } else {
                    t = value;
                }
            }
            return stop;
        }

        template <typename T>
        struct FloatConversionTraits {
            static constexpr long double huge_val = HUGE_VALL;
            static long double str_to_t(const char* p, char** ep) noexcept { return strtold(p, ep); }
        };

        template <>
        struct FloatConversionTraits<float> {
            static constexpr float huge_val = HUGE_VALF;
            static float str_to_t(const char* p, char** ep) noexcept { return strtof(p, ep); }
        };

        template <>
        struct FloatConversionTraits<double> {
            static constexpr double huge_val = HUGE_VAL;
            static double str_to_t(const char* p, char** ep) noexcept { return strtod(p, ep); }
        };

    }

    template <typename T>
    size_t str_to_int(T& t, const u8string& str, size_t offset = 0, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, utf_iterator(str, offset), flags, 10).offset() - offset;
    }

    template <typename T>
    Utf8Iterator str_to_int(T& t, const Utf8Iterator& start, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, start, flags, 10);
    }

    template <typename T>
    T str_to_int(const u8string& str, uint32_t flags = 0) {
        T t = T(0);
        UnicornDetail::convert_str_to_int<T>(t, utf_begin(str), flags, 10);
        return t;
    }

    template <typename T>
    T str_to_int(const Utf8Iterator& start, uint32_t flags = 0) {
        T t = T(0);
        UnicornDetail::convert_str_to_int<T>(t, start, flags, 10);
        return t;
    }

    template <typename T>
    size_t hex_to_int(T& t, const u8string& str, size_t offset = 0, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, utf_iterator(str, offset), flags, 16).offset() - offset;
    }

    template <typename T>
    Utf8Iterator hex_to_int(T& t, const Utf8Iterator& start, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, start, flags, 16);
    }

    template <typename T>
    T hex_to_int(const u8string& str, uint32_t flags = 0) {
        T t = T(0);
        UnicornDetail::convert_str_to_int<T>(t, utf_begin(str), flags, 16);
        return t;
    }

    template <typename T>
    T hex_to_int(const Utf8Iterator& start, uint32_t flags = 0) {
        T t = T(0);
        UnicornDetail::convert_str_to_int<T>(t, start, flags, 16);
        return t;
    }

    template <typename T>
    Utf8Iterator str_to_float(T& t, const Utf8Iterator& start, uint32_t flags = 0) {
        using traits = UnicornDetail::FloatConversionTraits<T>;
        static constexpr T max_value = std::numeric_limits<T>::max();
        const u8string& src(start.source());
        size_t offset = start.offset();
        if (offset >= src.size()) {
            if (flags & err_throw)
                throw std::invalid_argument("Invalid number (empty string)");
            t = T(0);
            return utf_end(src);
        }
        size_t endpos = src.find_first_not_of("+-.0123456789Ee", offset);
        if (endpos == offset) {
            if (flags & err_throw)
                throw std::invalid_argument("Invalid number: " + uquote(start.str()));
            t = T(0);
            return start;
        }
        if (endpos == npos)
            endpos = src.size();
        u8string fragment(src, offset, endpos - offset);
        char* endptr = nullptr;
        T value = traits::str_to_t(fragment.data(), &endptr);
        size_t len = endptr - fragment.data();
        auto stop = utf_iterator(src, offset + len);
        if ((flags & err_throw) && stop != utf_end(src))
            throw std::invalid_argument("Invalid number: " + uquote(u_str(start, utf_end(src))));
        if (len == 0) {
            if (flags & err_throw)
                throw std::invalid_argument("Invalid number: " + uquote(fragment));
            t = T(0);
        } else if (value == traits::huge_val || value == - traits::huge_val) {
            if (flags & err_throw)
                throw std::range_error("Number out of range: " + uquote(fragment));
            t = value > T(0) ? max_value : - max_value;
        } else {
            t = value;
        }
        return stop;
    }

    template <typename T>
    size_t str_to_float(T& t, const u8string& str, size_t offset = 0, uint32_t flags = 0) {
        return str_to_float(t, utf_iterator(str, offset), flags).offset() - offset;
    }

    template <typename T>
    T str_to_float(const u8string& str, uint32_t flags = 0) {
        T t = T(0);
        str_to_float(t, utf_begin(str), flags);
        return t;
    }

    template <typename T>
    T str_to_float(const Utf8Iterator& start, uint32_t flags = 0) {
        T t = T(0);
        str_to_float(t, start, flags);
        return t;
    }

}
