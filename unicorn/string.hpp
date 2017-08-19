#pragma once

#include "unicorn/character.hpp"
#include "unicorn/core.hpp"
#include "unicorn/segment.hpp"
#include "unicorn/utf.hpp"
#include "rs-core/kwargs.hpp"
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS::Unicorn {

    // Early declarations for functions used internally

    inline void str_append(U8string& str, const U8string& suffix) { str += suffix; }
    void str_append(U8string& str, const Utf8Iterator& suffix_begin, const Utf8Iterator& suffix_end);
    void str_append(U8string& str, const Irange<Utf8Iterator>& suffix);
    void str_append(U8string& str, const char* suffix);
    void str_append(U8string& dst, const char* ptr, size_t n);
    inline void str_append_char(U8string& dst, char c) { dst += c; }
    void str_append_chars(U8string& dst, size_t n, char32_t c);
    U8string str_char(char32_t c);
    U8string str_chars(size_t n, char32_t c);
    U8string str_repeat(const U8string& str, size_t n);
    void str_repeat_in(U8string& str, size_t n);

    template <typename C>
    void str_append(U8string& str, const std::basic_string<C>& suffix) {
        std::copy(utf_begin(suffix), utf_end(suffix), utf_writer(str));
    }

    template <typename C>
    void str_append(U8string& str, const UtfIterator<C>& suffix_begin, const UtfIterator<C>& suffix_end) {
        std::copy(suffix_begin, suffix_end, utf_writer(str));
    }

    template <typename C>
    void str_append(U8string& str, const Irange<UtfIterator<C>>& suffix) {
        str_append(str, suffix.begin(), suffix.end());
    }

    template <typename C>
    void str_append(U8string& str, const C* suffix) {
        str_append(str, cstr(suffix));
    }

    template <typename C>
    void str_append(U8string& dst, const C* ptr, size_t n) {
        if (ptr)
            for (auto out = utf_writer(dst); n > 0; --n, ++ptr)
                *out = char_to_uint(*ptr++);
    }

    template <typename C>
    void str_append_char(U8string& dst, C c) {
        *utf_writer(dst) = char_to_uint(c);
    }

    template <typename C1, typename C2, typename... Chars>
    void str_append_char(U8string& dst, C1 c1, C2 c2, Chars... chars) {
        str_append_char(dst, c1);
        str_append_char(dst, c2, chars...);
    }

    // String size functions

    // Remember that any other set of flags that might be combined with these
    // needs to skip the bits that are already spoken for. The string length
    // flags are linked to upper case letters for use with unicorn/format, so
    // they have bit positions in the 0-25 range.

    struct Length {
        static constexpr uint32_t characters  = letter_to_mask('C');  // Measure string in characters (default)
        static constexpr uint32_t graphemes   = letter_to_mask('G');  // Measure string in grapheme clusters
        static constexpr uint32_t narrow      = letter_to_mask('N');  // East Asian width, defaulting to narrow
        static constexpr uint32_t wide        = letter_to_mask('W');  // East Asian width, defaulting to wide
        uint32_t flags = 0;
        Length() = default;
        explicit Length(uint32_t length_flags);
        template <typename C> size_t operator()(const std::basic_string<C>& str) const;
        template <typename C> size_t operator()(const Irange<UtfIterator<C>>& range) const;
        template <typename C> size_t operator()(const UtfIterator<C>& b, const UtfIterator<C>& e) const;
    };

    namespace UnicornDetail {

        constexpr auto east_asian_flags = Length::narrow | Length::wide;
        constexpr auto all_length_flags = Length::characters | Length::graphemes | east_asian_flags;

        inline bool char_is_advancing(char32_t c) {
            return char_general_category(c) != GC::Mn;
        }

        template <typename C>
        inline bool grapheme_is_advancing(Irange<UtfIterator<C>> g) {
            return ! g.empty() && char_is_advancing(*g.begin());
        }

        inline void check_length_flags(uint32_t& flags) {
            if (ibits(flags & (Length::characters | Length::graphemes)) > 1
                    || ibits(flags & (Length::characters | east_asian_flags)) > 1)
                throw std::invalid_argument("Inconsistent string length flags");
            if (ibits(flags & all_length_flags) == 0)
                flags |= Length::characters;
        }

        class EastAsianCount {
        public:
            explicit EastAsianCount(uint32_t flags) noexcept: count(), fset(flags) { memset(count, 0, sizeof(count)); }
            void add(char32_t c) noexcept {
                if (char_is_advancing(c))
                    ++count[unsigned(east_asian_width(c))];
            }
            size_t get() const noexcept {
                size_t default_width = fset & Length::wide ? 2 : 1;
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
            if (pos == 0)
                return {range.begin(), true};
            if (flags & Length::characters) {
                auto i = range.begin();
                size_t len = 0;
                while (i != range.end() && len < pos) {
                    ++i;
                    ++len;
                }
                return {i, len == pos};
            } else if (flags & east_asian_flags) {
                EastAsianCount eac(flags);
                if (flags & Length::graphemes) {
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

    inline Length::Length(uint32_t length_flags):
    flags(length_flags) {
        UnicornDetail::check_length_flags(flags);
    }

    template <typename C>
    size_t Length::operator()(const std::basic_string<C>& str) const {
        return (*this)(utf_range(str));
    }

    template <typename C>
    size_t Length::operator()(const Irange<UtfIterator<C>>& range) const {
        using namespace UnicornDetail;
        if (flags & Length::characters) {
            return range_count(range);
        } else if (flags & east_asian_flags) {
            EastAsianCount eac(flags);
            if (flags & Length::graphemes) {
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
    size_t Length::operator()(const UtfIterator<C>& b, const UtfIterator<C>& e) const {
        return (*this)(irange(b, e));
    }

    template <typename C>
    size_t str_length(const std::basic_string<C>& str, uint32_t flags = 0) {
        return Length(flags)(str);
    }

    template <typename C>
    size_t str_length(const Irange<UtfIterator<C>>& range, uint32_t flags = 0) {
        return Length(flags)(range);
    }

    template <typename C>
    size_t str_length(const UtfIterator<C>& b, const UtfIterator<C>& e, uint32_t flags = 0) {
        return Length(flags)(b, e);
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
    UtfIterator<C> str_find_index(const std::basic_string<C>& str, size_t pos, uint32_t flags = 0) {
        return str_find_index(utf_range(str), pos, flags);
    }

    template <typename C>
    size_t str_find_offset(const std::basic_string<C>& str, size_t pos, uint32_t flags = 0) {
        auto rc = UnicornDetail::find_position(utf_range(str), pos, flags);
        return rc.second ? rc.first.offset() : npos;
    }

    // Other string properties
    // Defined in string-property.cpp

    template <typename C>
    char32_t str_char_at(const std::basic_string<C>& str, size_t index) noexcept {
        auto range = utf_range(str);
        for (char32_t c: range)
            if (! index--)
                return c;
        return 0;
    }

    template <typename C>
    char32_t str_first_char(const std::basic_string<C>& str) noexcept {
        return str.empty() ? 0 : *utf_begin(str);
    }

    template <typename C>
    char32_t str_last_char(const std::basic_string<C>& str) noexcept {
        return str.empty() ? 0 : *std::prev(utf_end(str));
    }

    bool str_is_east_asian(const U8string& str);
    bool str_starts_with(const U8string& str, const U8string& prefix) noexcept;
    bool str_ends_with(const U8string& str, const U8string& suffix) noexcept;

    // String comparison
    // Defined in string-compare.cpp

    int str_compare_3way(const U8string& lhs, const U8string& rhs);
    bool str_icase_compare(const U8string& lhs, const U8string& rhs) noexcept;
    bool str_icase_equal(const U8string& lhs, const U8string& rhs) noexcept;
    bool str_natural_compare(const U8string& lhs, const U8string& rhs) noexcept;

    template <typename C>
    bool utf_compare(const std::basic_string<C>& lhs, const std::basic_string<C>& rhs) noexcept {
        if (sizeof(C) == 2)
            return std::lexicographical_compare(utf_begin(lhs), utf_end(lhs), utf_begin(rhs), utf_end(rhs));
        else
            return lhs < rhs;
    }

    // Other string algorithms
    // Defined in string-algorithm.cpp

    size_t str_common(const U8string& s1, const U8string& s2, size_t start = 0) noexcept;
    size_t str_common_utf(const U8string& s1, const U8string& s2, size_t start = 0) noexcept;
    bool str_expect(Utf8Iterator& i, const Utf8Iterator& end, const U8string& prefix);
    bool str_expect(Utf8Iterator& i, const U8string& prefix);
    Utf8Iterator str_find_char(const Utf8Iterator& b, const Utf8Iterator& e, char32_t c);
    Utf8Iterator str_find_char(const Irange<Utf8Iterator>& range, char32_t c);
    Utf8Iterator str_find_char(const U8string& str, char32_t c);
    Utf8Iterator str_find_last_char(const Utf8Iterator& b, const Utf8Iterator& e, char32_t c);
    Utf8Iterator str_find_last_char(const Irange<Utf8Iterator>& range, char32_t c);
    Utf8Iterator str_find_last_char(const U8string& str, char32_t c);
    Utf8Iterator str_find_first_of(const Utf8Iterator& b, const Utf8Iterator& e, const U8string& target);
    Utf8Iterator str_find_first_of(const Irange<Utf8Iterator>& range, const U8string& target);
    Utf8Iterator str_find_first_of(const U8string& str, const U8string& target);
    Utf8Iterator str_find_first_not_of(const Utf8Iterator& b, const Utf8Iterator& e, const U8string& target);
    Utf8Iterator str_find_first_not_of(const Irange<Utf8Iterator>& range, const U8string& target);
    Utf8Iterator str_find_first_not_of(const U8string& str, const U8string& target);
    Utf8Iterator str_find_last_of(const Utf8Iterator& b, const Utf8Iterator& e, const U8string& target);
    Utf8Iterator str_find_last_of(const Irange<Utf8Iterator>& range, const U8string& target);
    Utf8Iterator str_find_last_of(const U8string& str, const U8string& target);
    Utf8Iterator str_find_last_not_of(const Utf8Iterator& b, const Utf8Iterator& e, const U8string& target);
    Utf8Iterator str_find_last_not_of(const Irange<Utf8Iterator>& range, const U8string& target);
    Utf8Iterator str_find_last_not_of(const U8string& str, const U8string& target);
    std::pair<size_t, size_t> str_line_column(const U8string& str, size_t offset, size_t flags = 0);
    Irange<Utf8Iterator> str_search(const Utf8Iterator& b, const Utf8Iterator& e, const U8string& target);
    Irange<Utf8Iterator> str_search(const Irange<Utf8Iterator>& range, const U8string& target);
    Irange<Utf8Iterator> str_search(const U8string& str, const U8string& target);
    size_t str_skipws(Utf8Iterator& i, const Utf8Iterator& end);
    size_t str_skipws(Utf8Iterator& i);

    // String manipulation functions
    // Defined in string-manip.cpp

    namespace UnicornDetail {

        enum { trimleft = 1, trimright = 2 };

        U8string expand_tabs(const U8string& str, const std::vector<size_t>& tabs, uint32_t flags);

        template <typename DS>
        void concat_helper(U8string&, const DS&) {}

        template <typename DS, typename S2, typename... Strings>
        void concat_helper(U8string& s1, const DS& delim, const S2& s2, const Strings&... ss) {
            str_append(s1, delim);
            str_append(s1, s2);
            concat_helper(s1, delim, ss...);
        }

        template <typename Pred>
        U8string trim_helper(const U8string& src, int mode, Pred p) {
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
        void trim_in_helper(U8string& src, int mode, Pred p) {
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

    U8string str_drop_prefix(const U8string& str, const U8string& prefix);
    void str_drop_prefix_in(U8string& str, const U8string& prefix) noexcept;
    U8string str_drop_suffix(const U8string& str, const U8string& suffix);
    void str_drop_suffix_in(U8string& str, const U8string& suffix) noexcept;
    U8string str_erase_left(const U8string& str, size_t length);
    void str_erase_left_in(U8string& str, size_t length) noexcept;
    U8string str_erase_right(const U8string& str, size_t length);
    void str_erase_right_in(U8string& str, size_t length) noexcept;
    U8string str_expand_tabs(const U8string& str);
    void str_expand_tabs_in(U8string& str);
    U8string str_fix_left(const U8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    void str_fix_left_in(U8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    U8string str_fix_right(const U8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    void str_fix_right_in(U8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    U8string str_insert(const Utf8Iterator& dst, const Utf8Iterator& src_begin, const Utf8Iterator& src_end);
    U8string str_insert(const Utf8Iterator& dst, const Irange<Utf8Iterator>& src);
    U8string str_insert(const Utf8Iterator& dst, const U8string& src);
    U8string str_insert(const Utf8Iterator& dst_begin, const Utf8Iterator& dst_end, const Utf8Iterator& src_begin, const Utf8Iterator& src_end);
    U8string str_insert(const Irange<Utf8Iterator>& dst, const Irange<Utf8Iterator>& src);
    U8string str_insert(const Utf8Iterator& dst_begin, const Utf8Iterator& dst_end, const U8string& src);
    U8string str_insert(const Irange<Utf8Iterator>& dst, const U8string& src);
    Irange<Utf8Iterator> str_insert_in(U8string& dst, const Utf8Iterator& where, const Utf8Iterator& src_begin, const Utf8Iterator& src_end);
    Irange<Utf8Iterator> str_insert_in(U8string& dst, const Utf8Iterator& where, const Irange<Utf8Iterator>& src);
    Irange<Utf8Iterator> str_insert_in(U8string& dst, const Utf8Iterator& where, const U8string& src);
    Irange<Utf8Iterator> str_insert_in(U8string& dst, const Utf8Iterator& range_begin, const Utf8Iterator& range_end, const Utf8Iterator& src_begin, const Utf8Iterator& src_end);
    Irange<Utf8Iterator> str_insert_in(U8string& dst, const Irange<Utf8Iterator>& range, const Irange<Utf8Iterator>& src);
    Irange<Utf8Iterator> str_insert_in(U8string& dst, const Utf8Iterator& range_begin, const Utf8Iterator& range_end, const U8string& src);
    Irange<Utf8Iterator> str_insert_in(U8string& dst, const Irange<Utf8Iterator>& range, const U8string& src);
    U8string str_pad_left(const U8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    void str_pad_left_in(U8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    U8string str_pad_right(const U8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    void str_pad_right_in(U8string& str, size_t length, char32_t c = U' ', uint32_t flags = 0);
    bool str_partition(const U8string& str, U8string& prefix, U8string& suffix);
    bool str_partition_at(const U8string& str, U8string& prefix, U8string& suffix, const U8string& delim);
    bool str_partition_by(const U8string& str, U8string& prefix, U8string& suffix, const U8string& delim);
    U8string str_remove(const U8string& str, char32_t c);
    U8string str_remove(const U8string& str, const U8string& chars);
    void str_remove_in(U8string& str, char32_t c);
    void str_remove_in(U8string& str, const U8string& chars);
    U8string str_replace(const U8string& str, const U8string& target, const U8string& sub, size_t n = npos);
    void str_replace_in(U8string& str, const U8string& target, const U8string& sub, size_t n = npos);
    Strings str_splitv(const U8string& src);
    Strings str_splitv_at(const U8string& src, const U8string& delim);
    Strings str_splitv_by(const U8string& src, const U8string& delim);
    U8string str_squeeze(const U8string& str);
    U8string str_squeeze(const U8string& str, const U8string& chars);
    U8string str_squeeze_trim(const U8string& str);
    U8string str_squeeze_trim(const U8string& str, const U8string& chars);
    void str_squeeze_in(U8string& str);
    void str_squeeze_in(U8string& str, const U8string& chars);
    void str_squeeze_trim_in(U8string& str);
    void str_squeeze_trim_in(U8string& str, const U8string& chars);
    U8string str_substring(const U8string& str, size_t offset, size_t count = npos);
    U8string utf_substring(const U8string& str, size_t index, size_t length = npos, uint32_t flags = 0);
    U8string str_translate(const U8string& str, const U8string& target, const U8string& sub);
    void str_translate_in(U8string& str, const U8string& target, const U8string& sub);
    U8string str_trim(const U8string& str, const U8string& chars);
    U8string str_trim(const U8string& str);
    U8string str_trim_left(const U8string& str, const U8string& chars);
    U8string str_trim_left(const U8string& str);
    U8string str_trim_right(const U8string& str, const U8string& chars);
    U8string str_trim_right(const U8string& str);
    void str_trim_in(U8string& str, const U8string& chars);
    void str_trim_in(U8string& str);
    void str_trim_left_in(U8string& str, const U8string& chars);
    void str_trim_left_in(U8string& str);
    void str_trim_right_in(U8string& str, const U8string& chars);
    void str_trim_right_in(U8string& str);
    U8string str_unify_lines(const U8string& str, const U8string& newline);
    U8string str_unify_lines(const U8string& str, char32_t newline);
    U8string str_unify_lines(const U8string& str);
    void str_unify_lines_in(U8string& str, const U8string& newline);
    void str_unify_lines_in(U8string& str, char32_t newline);
    void str_unify_lines_in(U8string& str);

    struct Wrap {

        static constexpr Kwarg<bool> enforce = {};      // Enforce right margin strictly (default false)
        static constexpr Kwarg<bool> preserve = {};     // Preserve layout on already indented lines (default false)
        static constexpr Kwarg<uint32_t> flags = {};    // Flags for string length (default 0)
        static constexpr Kwarg<size_t> margin = {};     // Margin for first line (default 0)
        static constexpr Kwarg<size_t> margin2 = {};    // Margin for subsequent lines (default same as margin)
        static constexpr Kwarg<size_t> width = {};      // Wrap width (default COLUMNS-2)
        static constexpr Kwarg<U8string> newline = {};  // Line break (default "\n")

    };

    namespace UnicornDetail {

        void str_wrap_helper(const U8string& src, U8string& dst, bool enforce, bool preserve, uint32_t flags, size_t margin, size_t margin2, size_t width, const U8string& newline);

    }

    template <typename... Args> U8string str_wrap(const U8string& str, const Args&... args) {
        bool enforce = false;
        bool preserve = false;
        uint32_t flags = 0;
        size_t margin = 0;
        size_t margin2 = npos;
        size_t width = npos;
        U8string newline = "\n";
        kwget(Wrap::enforce, enforce, args...);
        kwget(Wrap::preserve, preserve, args...);
        kwget(Wrap::flags, flags, args...);
        kwget(Wrap::margin, margin, args...);
        kwget(Wrap::margin2, margin2, args...);
        kwget(Wrap::width, width, args...);
        kwget(Wrap::newline, newline, args...);
        U8string result;
        UnicornDetail::str_wrap_helper(str, result, enforce, preserve, flags, margin, margin2, width, newline);
        return result;
    }

    template <typename... Args> void str_wrap_in(U8string& str, const Args&... args) {
        bool enforce = false;
        bool preserve = false;
        uint32_t flags = 0;
        size_t margin = 0;
        size_t margin2 = npos;
        size_t width = npos;
        U8string newline = "\n";
        kwget(Wrap::enforce, enforce, args...);
        kwget(Wrap::preserve, preserve, args...);
        kwget(Wrap::flags, flags, args...);
        kwget(Wrap::margin, margin, args...);
        kwget(Wrap::margin2, margin2, args...);
        kwget(Wrap::width, width, args...);
        kwget(Wrap::newline, newline, args...);
        U8string result;
        UnicornDetail::str_wrap_helper(str, result, enforce, preserve, flags, margin, margin2, width, newline);
        str = std::move(result);
    }

    template <typename C, typename... Strings>
    U8string str_concat(const std::basic_string<C>& s, const Strings&... ss) {
        U8string result;
        UnicornDetail::concat_helper(result, std::basic_string<C>(), s, ss...);
        return result;
    }

    template <typename C, typename... Strings>
    U8string str_concat(const C* s, const Strings&... ss) {
        U8string result;
        UnicornDetail::concat_helper(result, std::basic_string<C>(), cstr(s), ss...);
        return result;
    }

    template <typename C>
    U8string str_concat_with(const std::basic_string<C>& /*delim*/) {
        return {};
    }

    template <typename C>
    U8string str_concat_with(const C* /*delim*/) {
        return {};
    }

    template <typename C1, typename C2, typename... Strings>
    U8string str_concat_with(const std::basic_string<C1>& delim, const std::basic_string<C2>& s, const Strings&... ss) {
        U8string result = to_utf8(s);
        UnicornDetail::concat_helper(result, delim, ss...);
        return result;
    }

    template <typename C1, typename C2, typename... Strings>
    U8string str_concat_with(const std::basic_string<C1>& delim, const C2* s, const Strings&... ss) {
        U8string result = to_utf8(cstr(s));
        UnicornDetail::concat_helper(result, delim, ss...);
        return result;
    }

    template <typename C1, typename C2, typename... Strings>
    U8string str_concat_with(const C1* delim, const std::basic_string<C2>& s, const Strings&... ss) {
        U8string result = to_utf8(s);
        UnicornDetail::concat_helper(result, cstr(delim), ss...);
        return result;
    }

    template <typename C1, typename C2, typename... Strings>
    U8string str_concat_with(const C1* delim, const C2* s, const Strings&... ss) {
        U8string result = to_utf8(cstr(s));
        UnicornDetail::concat_helper(result, cstr(delim), ss...);
        return result;
    }

    template <typename IntList>
    U8string str_expand_tabs(const U8string& str, const IntList& tabs, uint32_t flags = 0) {
        std::vector<size_t> tv(tabs.begin(), tabs.end());
        return UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename IntType>
    U8string str_expand_tabs(const U8string& str, std::initializer_list<IntType> tabs, uint32_t flags = 0) {
        std::vector<size_t> tv(tabs.begin(), tabs.end());
        return UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename IntList>
    void str_expand_tabs_in(U8string& str, const IntList& tabs, uint32_t flags = 0) {
        std::vector<size_t> tv(tabs.begin(), tabs.end());
        str = UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename IntType>
    void str_expand_tabs_in(U8string& str, std::initializer_list<IntType> tabs, uint32_t flags = 0) {
        std::vector<size_t> tv(tabs.begin(), tabs.end());
        str = UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename FwdRange>
    U8string str_join(const FwdRange& r, const U8string& delim, bool term = false) {
        using std::begin;
        using std::end;
        U8string dst;
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
        return str_join(r, U8string());
    }

    template <typename Pred>
    U8string str_remove_if(const U8string& str, Pred p) {
        U8string dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [p] (char32_t x) { return ! p(x); });
        return dst;
    }

    template <typename Pred>
    U8string str_remove_if_not(const U8string& str, Pred p) {
        U8string dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), p);
        return dst;
    }

    template <typename Pred>
    void str_remove_in_if(U8string& str, Pred p) {
        U8string dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [p] (char32_t x) { return ! p(x); });
        str.swap(dst);
    }

    template <typename Pred>
    void str_remove_in_if_not(U8string& str, Pred p) {
        U8string dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), p);
        str.swap(dst);
    }

    template <typename OutIter>
    void str_split(const U8string& src, OutIter dst) {
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
    void str_split_at(const U8string& src, OutIter dst, const U8string& delim) {
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
    void str_split_by(const U8string& src, OutIter dst, const U8string& delim) {
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
    void str_split_lines(const U8string& src, OutIter dst) {
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
    U8string str_trim_if(const U8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft | trimright, p);
    }

    template <typename Pred>
    U8string str_trim_left_if(const U8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft, p);
    }

    template <typename Pred>
    U8string str_trim_right_if(const U8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimright, p);
    }

    template <typename Pred>
    U8string str_trim_if_not(const U8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft | trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    U8string str_trim_left_if_not(const U8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimleft, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    U8string str_trim_right_if_not(const U8string& str, Pred p) {
        using namespace UnicornDetail;
        return trim_helper(str, trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    void str_trim_in_if(U8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft | trimright, p);
    }

    template <typename Pred>
    void str_trim_left_in_if(U8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft, p);
    }

    template <typename Pred>
    void str_trim_right_in_if(U8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimright, p);
    }

    template <typename Pred>
    void str_trim_in_if_not(U8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft | trimright, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    void str_trim_left_in_if_not(U8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimleft, [p] (char32_t c) { return ! p(c); });
    }

    template <typename Pred>
    void str_trim_right_in_if_not(U8string& str, Pred p) {
        using namespace UnicornDetail;
        trim_in_helper(str, trimright, [p] (char32_t c) { return ! p(c); });
    }

    // Case mapping functions
    // Defined in string-case.cpp

    U8string str_uppercase(const U8string& str);
    U8string str_lowercase(const U8string& str);
    U8string str_titlecase(const U8string& str);
    U8string str_casefold(const U8string& str);
    void str_uppercase_in(U8string& str);
    void str_lowercase_in(U8string& str);
    void str_titlecase_in(U8string& str);
    void str_casefold_in(U8string& str);

    // Escaping and quoting functions
    // Defined in string-escape.cpp

    struct Escape {

        static constexpr uint32_t ascii   = 1ul << 0;  // Escape all non-ASCII characters
        static constexpr uint32_t nostdc  = 1ul << 1;  // Do not use standard C symbols such as `\n`
        static constexpr uint32_t pcre    = 1ul << 2;  // Use `\x{...}` instead of `\u` and `\U` (implies `nonascii`)
        static constexpr uint32_t punct   = 1ul << 3;  // Escape ASCII punctuation

    };

    U8string str_encode_uri(const U8string& str);
    U8string str_encode_uri_component(const U8string& str);
    void str_encode_uri_in(U8string& str);
    void str_encode_uri_component_in(U8string& str);
    U8string str_unencode_uri(const U8string& str);
    void str_unencode_uri_in(U8string& str);
    U8string str_escape(const U8string& str, uint32_t flags = 0);
    void str_escape_in(U8string& str, uint32_t flags = 0);
    U8string str_unescape(const U8string& str);
    void str_unescape_in(U8string& str);
    U8string str_quote(const U8string& str, uint32_t flags = 0, char32_t quote = U'\"');
    void str_quote_in(U8string& str, uint32_t flags = 0, char32_t quote = U'\"');
    U8string str_unquote(const U8string& str, char32_t quote = U'\"');
    void str_unquote_in(U8string& str, char32_t quote = U'\"');

    // Type conversion functions

    namespace UnicornDetail {

        template <typename T>
        Utf8Iterator convert_str_to_int(T& t, const Utf8Iterator& start, uint32_t flags, int base) {
            static const U8string dec_chars = "+-0123456789";
            static const U8string hex_chars = "+-0123456789ABCDEFabcdef";
            const U8string& src(start.source());
            size_t offset = start.offset();
            if (offset >= src.size()) {
                if (flags & Utf::throws)
                    throw std::invalid_argument("Invalid integer (empty string)");
                t = T(0);
                return utf_end(src);
            }
            size_t endpos = src.find_first_not_of(base == 16 ? hex_chars : dec_chars, offset);
            if (endpos == offset) {
                if (flags & Utf::throws)
                    throw std::invalid_argument("Invalid integer: " + quote(to_utf8(start.str())));
                t = T(0);
                return start;
            }
            if (endpos == npos)
                endpos = src.size();
            U8string fragment(src, offset, endpos - offset);
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
                if ((flags & Utf::throws) && stop != utf_end(src))
                    throw std::invalid_argument("Invalid integer: " + quote(fragment));
                if (len == 0) {
                    if (flags & Utf::throws)
                        throw std::invalid_argument("Invalid integer: " + quote(u_str(start, utf_end(src))));
                    t = T(0);
                } else if (err == ERANGE || value < min_value || value > max_value) {
                    if (flags & Utf::throws)
                        throw std::range_error("Integer out of range: " + quote(fragment));
                    t = T(value > 0 ? max_value : min_value);
                } else {
                    t = T(value);
                }
            } else {
                static constexpr auto max_value = static_cast<unsigned long long>(std::numeric_limits<T>::max());
                char* endptr = nullptr;
                errno = 0;
                unsigned long long value = strtoull(fragment.data(), &endptr, base);
                int err = errno;
                size_t len = endptr - fragment.data();
                stop = utf_iterator(src, offset + len);
                if ((flags & Utf::throws) && stop != utf_end(src))
                        throw std::invalid_argument("Invalid integer: " + quote(u_str(start, utf_end(src))));
                if (len == 0) {
                    if (flags & Utf::throws)
                        throw std::invalid_argument("Invalid integer: " + quote(fragment));
                    t = T(0);
                } else if (err == ERANGE || value > max_value) {
                    if (flags & Utf::throws)
                        throw std::range_error("Integer out of range: " + quote(fragment));
                    t = T(max_value);
                } else {
                    t = T(value);
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
    size_t str_to_int(T& t, const U8string& str, size_t offset = 0, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, utf_iterator(str, offset), flags, 10).offset() - offset;
    }

    template <typename T>
    Utf8Iterator str_to_int(T& t, const Utf8Iterator& start, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, start, flags, 10);
    }

    template <typename T>
    T str_to_int(const U8string& str, uint32_t flags = 0) {
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
    size_t hex_to_int(T& t, const U8string& str, size_t offset = 0, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, utf_iterator(str, offset), flags, 16).offset() - offset;
    }

    template <typename T>
    Utf8Iterator hex_to_int(T& t, const Utf8Iterator& start, uint32_t flags = 0) {
        return UnicornDetail::convert_str_to_int<T>(t, start, flags, 16);
    }

    template <typename T>
    T hex_to_int(const U8string& str, uint32_t flags = 0) {
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
        const U8string& src(start.source());
        size_t offset = start.offset();
        if (offset >= src.size()) {
            if (flags & Utf::throws)
                throw std::invalid_argument("Invalid number (empty string)");
            t = T(0);
            return utf_end(src);
        }
        size_t endpos = src.find_first_not_of("+-.0123456789Ee", offset);
        if (endpos == offset) {
            if (flags & Utf::throws)
                throw std::invalid_argument("Invalid number: " + quote(start.str()));
            t = T(0);
            return start;
        }
        if (endpos == npos)
            endpos = src.size();
        U8string fragment(src, offset, endpos - offset);
        char* endptr = nullptr;
        T value = traits::str_to_t(fragment.data(), &endptr);
        size_t len = endptr - fragment.data();
        auto stop = utf_iterator(src, offset + len);
        if ((flags & Utf::throws) && stop != utf_end(src))
            throw std::invalid_argument("Invalid number: " + quote(u_str(start, utf_end(src))));
        if (len == 0) {
            if (flags & Utf::throws)
                throw std::invalid_argument("Invalid number: " + quote(fragment));
            t = T(0);
        } else if (value == traits::huge_val || value == - traits::huge_val) {
            if (flags & Utf::throws)
                throw std::range_error("Number out of range: " + quote(fragment));
            t = value > T(0) ? max_value : - max_value;
        } else {
            t = value;
        }
        return stop;
    }

    template <typename T>
    size_t str_to_float(T& t, const U8string& str, size_t offset = 0, uint32_t flags = 0) {
        return str_to_float(t, utf_iterator(str, offset), flags).offset() - offset;
    }

    template <typename T>
    T str_to_float(const U8string& str, uint32_t flags = 0) {
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
