// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-algorithm.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-property.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace Unicorn {

    constexpr uint32_t wrap_crlf      = 1ul << 26;  // Use CR+LF for line breaks (default LF)
    constexpr uint32_t wrap_enforce   = 1ul << 27;  // Enforce right margin strictly
    constexpr uint32_t wrap_preserve  = 1ul << 28;  // Preserve layout on already indented lines

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
        auto i = range.begin();
        for (size_t j = 0; j < length && i != range.end(); ++i, ++j) {}
        if (i == range.end())
            return {};
        else
            return str.substr(i.offset(), npos);
    }

    template <typename C>
    void str_erase_left_in(basic_string<C>& str, size_t length) noexcept {
        if (length == 0)
            return;
        auto range = utf_range(str);
        auto i = range.begin();
        for (size_t j = 0; j < length && i != range.end(); ++i, ++j) {}
        if (i == range.end())
            str.clear();
        else
            str.erase(0, i.offset());
    }

    template <typename C>
    basic_string<C> str_erase_right(const basic_string<C>& str, size_t length) {
        if (length == 0)
            return str;
        auto range = utf_range(str);
        auto i = range.end();
        for (size_t j = 0; j < length && i != range.begin(); --i, ++j) {}
        if (i == range.begin())
            return {};
        else
            return str.substr(0, i.offset());
    }

    template <typename C>
    void str_erase_right_in(basic_string<C>& str, size_t length) noexcept {
        if (length == 0)
            return;
        auto range = utf_range(str);
        auto i = range.end();
        for (size_t j = 0; j < length && i != range.begin(); --i, ++j) {}
        if (i == range.begin())
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
        vector<size_t> tv(tabs.begin(), tabs.end());
        return UnicornDetail::expand_tabs(str, tv, flags);
    }

    template <typename C, typename IntType>
    basic_string<C> str_expand_tabs(const basic_string<C>& str, std::initializer_list<IntType> tabs, uint32_t flags = 0) {
        vector<size_t> tv(tabs.begin(), tabs.end());
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
                auto padding = str_chars<C>(pad_chars, c);
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
        return str_insert(dst, src.begin(), src.end());
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
        return str_insert(dst.begin(), dst.end(), src.begin(), src.end());
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
        return str_insert(dst.begin(), dst.end(), utf_begin(src), utf_end(src));
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
        return str_insert_in(dst, where, src.begin(), src.end());
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
        return str_insert_in(dst, range.begin(), range.end(), src.begin(), src.end());
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
        return str_insert_in(dst, range.begin(), range.end(), utf_begin(src), utf_end(src));
    }

    template <typename FwdRange, typename C>
    basic_string<C> str_join(const FwdRange& r, const basic_string<C>& delim) {
        using std::begin;
        using std::end;
        using string_type = basic_string<C>;
        using range_char = std::decay_t<decltype((*begin(r))[0])>;
        PRI_STATIC_ASSERT((std::is_same<C, range_char>::value));
        string_type s;
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

    template <typename FwdRange, typename C>
    basic_string<C> str_join(const FwdRange& r, const C* delim) {
        return str_join(r, cstr(delim));
    }

    template <typename FwdRange>
    auto str_join(const FwdRange& r) {
        using std::begin;
        using range_char = std::decay_t<decltype((*begin(r))[0])>;
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
        auto i = std::find_if(range.begin(), range.end(), char_is_white_space);
        if (i == range.end()) {
            prefix = str;
            suffix.clear();
            return false;
        }
        auto j = std::find_if_not(i, range.end(), char_is_white_space);
        auto temp = u_str(range.begin(), i);
        suffix = u_str(j, range.end());
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
            auto p = std::find_if(range.begin(), range.end(), match);
            i = p.offset();
            if (p != range.end()) {
                auto q = std::find_if_not(p, range.end(), match);
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
        auto i = range.begin(), j = i;
        while (i != range.end()) {
            j = std::find_if_not(i, range.end(), char_is_white_space);
            if (j == range.end())
                break;
            i = std::find_if(j, range.end(), char_is_white_space);
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

        template <typename String, typename Pred>
        void trim_in_helper(String& src, int mode, Pred p) {
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
        return str_unify_lines(str, str_char<C>(newline));
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
        str_unify_lines_in(str, str_char<C>(newline));
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
        auto i = range.begin(), e = range.end();
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

}
