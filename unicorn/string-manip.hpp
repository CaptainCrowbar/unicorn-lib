// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <string>
#include <type_traits>

namespace Unicorn {

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
    void str_repeat_in(u8string& str, size_t n);
    u8string str_replace(const u8string& str, const u8string& target, const u8string& sub, size_t n = npos);
    void str_replace_in(u8string& str, const u8string& target, const u8string& sub, size_t n = npos);
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
    u8string str_join(const FwdRange& r, const u8string& delim) {
        using std::begin;
        using std::end;
        u8string s;
        auto i = begin(r), e = end(r);
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

}
