#include "unicorn/string.hpp"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace RS::Unicorn {

    namespace {

        class CharIn {
        public:
            CharIn(const std::u32string& chars) noexcept: x(&chars) {}
            bool operator()(char32_t c) const noexcept { return x->find(c) != npos; }
        private:
            const std::u32string* x;
        };

        void check_whitespace(const Utf8Iterator& i, const Utf8Iterator& j, size_t& linebreaks, size_t& tailspaces) {
            linebreaks = tailspaces = 0;
            auto k = i;
            while (k != j) {
                auto c = *k;
                if (char_is_line_break(c))
                    ++linebreaks;
                ++k;
                if (k != j && c == U'\r' && *k == U'\n')
                    ++k;
                if (char_is_inline_space(c))
                    ++tailspaces;
                else
                    tailspaces = 0;
            }
        }

        void insert_padding(Ustring& str, size_t old_length, size_t new_length, char32_t c, uint32_t flags, char side) {
            size_t pad_chars = new_length - old_length;
            if (flags & UnicornDetail::east_asian_flags) {
                auto eaw = east_asian_width(c);
                if (eaw == East_Asian_Width::F || eaw == East_Asian_Width::W
                        || (eaw == East_Asian_Width::A && str_is_east_asian(str)))
                    pad_chars = (pad_chars + 1) / 2;
            }
            if (code_units<char>(c) == 1) {
                if (side == 'L')
                    str.insert(0, pad_chars, char(c));
                else
                    str.append(pad_chars, char(c));
            } else {
                auto padding = str_chars(pad_chars, c);
                if (side == 'L')
                    str.insert(0, padding);
                else
                    str += padding;
            }
        }

        void squeeze_helper(const Ustring& src, Ustring& dst, bool trim) {
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
                    dst += ' ';
            }
        }

        void squeeze_helper(const Ustring& src, Ustring& dst, bool trim, const Ustring& chars) {
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

    namespace UnicornDetail {

        Ustring expand_tabs(const Ustring& str, const std::vector<size_t>& tabs, uint32_t flags) {
            std::vector<size_t> xtabs = {0};
            for (auto t: tabs)
                if (t > xtabs.back())
                    xtabs.push_back(t);
            size_t delta = 8;
            if (xtabs.size() > 1)
                delta = xtabs.end()[-1] - xtabs.end()[-2];
            Ustring result;
            auto line_begin = utf_begin(str), str_end = utf_end(str);
            while (line_begin != str_end) {
                auto t = xtabs.begin(), t_end = xtabs.end();
                auto line_end = std::find_if(line_begin, str_end, char_is_line_break);
                auto u1 = line_begin;
                size_t col = 0;
                while (u1 != line_end) {
                    auto cut = std::find(str.begin() + u1.offset(), str.begin() + line_end.offset(), '\t');
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
                    result.append(tab_col - col, ' ');
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

    void str_append(Ustring& str, const Utf8Iterator& suffix_begin, const Utf8Iterator& suffix_end) {
        str.append(suffix_begin.source(), suffix_begin.offset(), suffix_end.offset() - suffix_begin.offset());
    }

    void str_append(Ustring& str, const Irange<Utf8Iterator>& suffix) {
        str_append(str, suffix.begin(), suffix.end());
    }

    void str_append(Ustring& str, const char* suffix) {
        if (suffix)
            str += suffix;
    }

    void str_append(Ustring& dst, const char* ptr, size_t n) {
        if (ptr)
            dst.append(ptr, n);
    }

    void str_append_chars(Ustring& dst, size_t n, char32_t c) {
        auto s = str_char(c);
        if (s.size() == 1)
            dst.append(n, s[0]);
        else
            dst += str_repeat(s, n);
    }

    Ustring str_char(char32_t c) {
        using namespace UnicornDetail;
        Ustring str(4, '\0');
        auto len = UtfEncoding<char>::encode(c, &str[0]);
        if (len == 0)
            len = UtfEncoding<char>::encode(replacement_char, &str[0]);
        str.resize(len);
        return str;
    }

    Ustring str_chars(size_t n, char32_t c) {
        using namespace UnicornDetail;
        if (n == 0)
            return {};
        Ustring str(4, '\0');
        auto len = UtfEncoding<char>::encode(c, &str[0]);
        if (len == 0)
            len = UtfEncoding<char>::encode(replacement_char, &str[0]);
        str.resize(len);
        return str_repeat(str, n);
    }

    Ustring str_drop_prefix(const Ustring& str, const Ustring& prefix) {
        return str_starts_with(str, prefix) ? str.substr(prefix.size()) : str;
    }

    void str_drop_prefix_in(Ustring& str, const Ustring& prefix) noexcept {
        if (str_starts_with(str, prefix))
            str.erase(0, prefix.size());
    }

    Ustring str_drop_suffix(const Ustring& str, const Ustring& suffix) {
        return str_ends_with(str, suffix) ? str.substr(0, str.size() - suffix.size()) : str;
    }

    void str_drop_suffix_in(Ustring& str, const Ustring& suffix) noexcept {
        if (str_ends_with(str, suffix))
            str.resize(str.size() - suffix.size());
    }

    Ustring str_erase_left(const Ustring& str, size_t length) {
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

    void str_erase_left_in(Ustring& str, size_t length) noexcept {
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

    Ustring str_erase_right(const Ustring& str, size_t length) {
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

    void str_erase_right_in(Ustring& str, size_t length) noexcept {
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

    Ustring str_expand_tabs(const Ustring& str) {
        return UnicornDetail::expand_tabs(str, {}, {});
    }

    void str_expand_tabs_in(Ustring& str) {
        auto result = str_expand_tabs(str);
        str.swap(result);
    }

    Ustring str_fix_left(const Ustring& str, size_t length, char32_t c, uint32_t flags) {
        size_t offset = str_find_offset(str, length, flags);
        if (offset == npos) {
            auto result = str;
            insert_padding(result, str_length(result, flags), length, c, flags, 'R');
            return result;
        } else {
            return str.substr(0, offset);
        }
    }

    void str_fix_left_in(Ustring& str, size_t length, char32_t c, uint32_t flags) {
        size_t offset = str_find_offset(str, length, flags);
        if (offset == npos)
            insert_padding(str, str_length(str, flags), length, c, flags, 'R');
        else
            str.erase(offset, npos);
    }

    Ustring str_fix_right(const Ustring& str, size_t length, char32_t c, uint32_t flags) {
        size_t old_length = str_length(str, flags);
        if (old_length < length) {
            auto result = str;
            insert_padding(result, old_length, length, c, flags, 'L');
            return result;
        } else {
            size_t offset = str_find_offset(str, old_length - length, flags);
            return str.substr(offset, npos);
        }
    }

    void str_fix_right_in(Ustring& str, size_t length, char32_t c, uint32_t flags) {
        size_t old_length = str_length(str, flags);
        if (old_length < length) {
            insert_padding(str, old_length, length, c, flags, 'L');
        } else {
            size_t offset = str_find_offset(str, old_length - length, flags);
            str.erase(0, offset);
        }
    }

    Ustring str_insert(const Utf8Iterator& dst, const Utf8Iterator& src_begin, const Utf8Iterator& src_end) {
        Ustring result(dst.source(), 0, dst.offset());
        result.append(src_begin.source(), src_begin.offset(), src_end.offset() - src_begin.offset());
        result.append(dst.source(), dst.offset(), npos);
        return result;
    }

    Ustring str_insert(const Utf8Iterator& dst, const Irange<Utf8Iterator>& src) {
        return str_insert(dst, src.begin(), src.end());
    }

    Ustring str_insert(const Utf8Iterator& dst, const Ustring& src) {
        Ustring result(dst.source(), 0, dst.offset());
        result += src;
        result.append(dst.source(), dst.offset(), npos);
        return result;
    }

    Ustring str_insert(const Utf8Iterator& dst_begin, const Utf8Iterator& dst_end,
            const Utf8Iterator& src_begin, const Utf8Iterator& src_end) {
        Ustring result(dst_begin.source(), 0, dst_begin.offset());
        result.append(src_begin.source(), src_begin.offset(), src_end.offset() - src_begin.offset());
        result.append(dst_end.source(), dst_end.offset(), npos);
        return result;
    }

    Ustring str_insert(const Irange<Utf8Iterator>& dst, const Irange<Utf8Iterator>& src) {
        return str_insert(dst.begin(), dst.end(), src.begin(), src.end());
    }

    Ustring str_insert(const Utf8Iterator& dst_begin, const Utf8Iterator& dst_end, const Ustring& src) {
        Ustring result(dst_begin.source(), 0, dst_begin.offset());
        result += src;
        result.append(dst_end.source(), dst_end.offset(), npos);
        return result;
    }

    Ustring str_insert(const Irange<Utf8Iterator>& dst, const Ustring& src) {
        return str_insert(dst.begin(), dst.end(), utf_begin(src), utf_end(src));
    }

    Irange<Utf8Iterator> str_insert_in(Ustring& dst, const Utf8Iterator& where,
            const Utf8Iterator& src_begin, const Utf8Iterator& src_end) {
        size_t ofs1 = where.offset(), ofs2 = src_begin.offset(), n = src_end.offset() - ofs2;
        dst.insert(ofs1, src_begin.source(), ofs2, n);
        return {utf_iterator(dst, ofs1), utf_iterator(dst, ofs1 + n)};
    }

    Irange<Utf8Iterator> str_insert_in(Ustring& dst, const Utf8Iterator& where, const Irange<Utf8Iterator>& src) {
        return str_insert_in(dst, where, src.begin(), src.end());
    }

    Irange<Utf8Iterator> str_insert_in(Ustring& dst, const Utf8Iterator& where, const Ustring& src) {
        size_t ofs = where.offset();
        dst.insert(ofs, src);
        return {utf_iterator(dst, ofs), utf_iterator(dst, ofs + src.size())};
    }

    Irange<Utf8Iterator> str_insert_in(Ustring& dst, const Utf8Iterator& range_begin, const Utf8Iterator& range_end,
            const Utf8Iterator& src_begin, const Utf8Iterator& src_end) {
        size_t ofs1 = range_begin.offset(), n1 = range_end.offset() - ofs1,
            ofs2 = src_begin.offset(), n2 = src_end.offset() - ofs2;
        dst.replace(ofs1, n1, src_begin.source(), ofs2, n2);
        return {utf_iterator(dst, ofs1), utf_iterator(dst, ofs1 + n2)};
    }

    Irange<Utf8Iterator> str_insert_in(Ustring& dst, const Irange<Utf8Iterator>& range, const Irange<Utf8Iterator>& src) {
        return str_insert_in(dst, range.begin(), range.end(), src.begin(), src.end());
    }

    Irange<Utf8Iterator> str_insert_in(Ustring& dst, const Utf8Iterator& range_begin, const Utf8Iterator& range_end,
            const Ustring& src) {
        size_t ofs = range_begin.offset(), n = range_end.offset() - ofs;
        dst.replace(ofs, n, src);
        return {utf_iterator(dst, ofs), utf_iterator(dst, ofs + src.size())};
    }

    Irange<Utf8Iterator> str_insert_in(Ustring& dst, const Irange<Utf8Iterator>& range, const Ustring& src) {
        return str_insert_in(dst, range.begin(), range.end(), utf_begin(src), utf_end(src));
    }

    Ustring str_pad_left(const Ustring& str, size_t length, char32_t c, uint32_t flags) {
        size_t old_length = str_length(str, flags);
        if (length > old_length) {
            auto result = str;
            insert_padding(result, old_length, length, c, flags, 'L');
            return result;
        } else {
            return str;
        }
    }

    void str_pad_left_in(Ustring& str, size_t length, char32_t c, uint32_t flags) {
        size_t old_length = str_length(str, flags);
        if (length > old_length)
            insert_padding(str, old_length, length, c, flags, 'L');
    }

    Ustring str_pad_right(const Ustring& str, size_t length, char32_t c, uint32_t flags) {
        size_t old_length = str_length(str, flags);
        if (length > old_length) {
            auto result = str;
            insert_padding(result, old_length, length, c, flags, 'R');
            return result;
        } else {
            return str;
        }
    }

    void str_pad_right_in(Ustring& str, size_t length, char32_t c, uint32_t flags) {
        size_t old_length = str_length(str, flags);
        if (length > old_length)
            insert_padding(str, old_length, length, c, flags, 'R');
    }

    bool str_partition(const Ustring& str, Ustring& prefix, Ustring& suffix) {
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

    bool str_partition_at(const Ustring& str, Ustring& prefix, Ustring& suffix, const Ustring& delim) {
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

    bool str_partition_by(const Ustring& str, Ustring& prefix, Ustring& suffix, const Ustring& delim) {
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

    Ustring str_remove(const Ustring& str, char32_t c) {
        Ustring dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [c] (char32_t x) { return x != c; });
        return dst;
    }

    Ustring str_remove(const Ustring& str, const Ustring& chars) {
        Ustring dst;
        auto uchars = to_utf32(chars);
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [&uchars] (char32_t x) { return uchars.find(x) == npos; });
        return dst;
    }

    void str_remove_in(Ustring& str, char32_t c) {
        Ustring dst;
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [c] (char32_t x) { return x != c; });
        str.swap(dst);
    }

    void str_remove_in(Ustring& str, const Ustring& chars) {
        Ustring dst;
        auto uchars = to_utf32(chars);
        std::copy_if(utf_begin(str), utf_end(str), utf_writer(dst), [&uchars] (char32_t x) { return uchars.find(x) == npos; });
        str.swap(dst);
    }

    Ustring str_repeat(const Ustring& str, size_t n) {
        if (n == 0 || str.empty())
            return {};
        if (n == 1)
            return str;
        if (str.size() == 1)
            return Ustring(n, str[0]);
        size_t size = n * str.size();
        auto dst = str;
        dst.reserve(size);
        while (dst.size() <= size / 2)
            dst += dst;
        dst += str_repeat(str, n - dst.size() / str.size());
        return dst;
    }

    void str_repeat_in(Ustring& str, size_t n) {
        auto dst = str_repeat(str, n);
        str.swap(dst);
    }

    Ustring str_replace(const Ustring& str, const Ustring& target, const Ustring& sub, size_t n) {
        if (target.empty() || n == 0)
            return str;
        Ustring dst;
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

    void str_replace_in(Ustring& str, const Ustring& target, const Ustring& sub, size_t n) {
        auto result = str_replace(str, target, sub, n);
        str.swap(result);
    }

    Strings str_splitv(const Ustring& src) {
        Strings v;
        str_split(src, append(v));
        return v;
    }

    Strings str_splitv_at(const Ustring& src, const Ustring& delim) {
        Strings v;
        str_split_at(src, append(v), delim);
        return v;
    }

    Strings str_splitv_by(const Ustring& src, const Ustring& delim) {
        Strings v;
        str_split_by(src, append(v), delim);
        return v;
    }

    Strings str_splitv_lines(const Ustring& src) {
        Strings v;
        str_split_lines(src, append(v));
        return v;
    }

    Ustring str_squeeze(const Ustring& str) {
        Ustring dst;
        squeeze_helper(str, dst, false);
        return dst;
    }

    Ustring str_squeeze(const Ustring& str, const Ustring& chars) {
        Ustring dst;
        squeeze_helper(str, dst, false, chars);
        return dst;
    }

    Ustring str_squeeze_trim(const Ustring& str) {
        Ustring dst;
        squeeze_helper(str, dst, true);
        return dst;
    }

    Ustring str_squeeze_trim(const Ustring& str, const Ustring& chars) {
        Ustring dst;
        squeeze_helper(str, dst, true, chars);
        return dst;
    }

    void str_squeeze_in(Ustring& str) {
        Ustring dst;
        squeeze_helper(str, dst, false);
        str.swap(dst);
    }

    void str_squeeze_in(Ustring& str, const Ustring& chars) {
        Ustring dst;
        squeeze_helper(str, dst, false, chars);
        str.swap(dst);
    }

    void str_squeeze_trim_in(Ustring& str) {
        Ustring dst;
        squeeze_helper(str, dst, true);
        str.swap(dst);
    }

    void str_squeeze_trim_in(Ustring& str, const Ustring& chars) {
        Ustring dst;
        squeeze_helper(str, dst, true, chars);
        str.swap(dst);
    }

    Ustring str_substring(const Ustring& str, size_t offset, size_t count) {
        if (offset < str.size())
            return str.substr(offset, count);
        else
            return {};
    }

    Ustring utf_substring(const Ustring& str, size_t index, size_t length, uint32_t flags) {
        UnicornDetail::check_length_flags(flags);
        auto b = utf_begin(str), e = utf_end(str);
        auto i = str_find_index(b, e, index, flags);
        auto j = length == npos ? e : str_find_index(i, e, length, flags);
        return u_str(i, j);
    }

    Ustring str_translate(const Ustring& str, const Ustring& target, const Ustring& sub) {
        if (target.empty() || sub.empty())
            return str;
        auto t = to_utf32(target), s = to_utf32(sub);
        if (s.size() < t.size())
            s.resize(t.size(), s.back());
        Ustring dst;
        dst.reserve(str.size());
        for (auto c: utf_range(str)) {
            size_t pos = t.find(c);
            if (pos != npos)
                c = s[pos];
            str_append_char(dst, c);
        }
        return dst;
    }

    void str_translate_in(Ustring& str, const Ustring& target, const Ustring& sub) {
        auto result = str_translate(str, target, sub);
        str.swap(result);
    }

    Ustring str_trim(const Ustring& str, const Ustring& chars) {
        return str_trim_if(str, CharIn(to_utf32(chars)));
    }

    Ustring str_trim(const Ustring& str) {
        return str_trim_if(str, char_is_white_space);
    }

    Ustring str_trim_left(const Ustring& str, const Ustring& chars) {
        return str_trim_left_if(str, CharIn(to_utf32(chars)));
    }

    Ustring str_trim_left(const Ustring& str) {
        return str_trim_left_if(str, char_is_white_space);
    }

    Ustring str_trim_right(const Ustring& str, const Ustring& chars) {
        return str_trim_right_if(str, CharIn(to_utf32(chars)));
    }

    Ustring str_trim_right(const Ustring& str) {
        return str_trim_right_if(str, char_is_white_space);
    }

    void str_trim_in(Ustring& str, const Ustring& chars) {
        str_trim_in_if(str, CharIn(to_utf32(chars)));
    }

    void str_trim_in(Ustring& str) {
        str_trim_in_if(str, char_is_white_space);
    }

    void str_trim_left_in(Ustring& str, const Ustring& chars) {
        str_trim_left_in_if(str, CharIn(to_utf32(chars)));
    }

    void str_trim_left_in(Ustring& str) {
        str_trim_left_in_if(str, char_is_white_space);
    }

    void str_trim_right_in(Ustring& str, const Ustring& chars) {
        str_trim_right_in_if(str, CharIn(to_utf32(chars)));
    }

    void str_trim_right_in(Ustring& str) {
        str_trim_right_in_if(str, char_is_white_space);
    }

    Ustring str_unify_lines(const Ustring& str, const Ustring& newline) {
        auto i = utf_begin(str), e = utf_end(str);
        Ustring result;
        bool last_break = false;
        while (i != e) {
            auto j = std::find_if(i, e, char_is_line_break);
            if (j != i) {
                result += u_str(i, j);
                last_break = false;
            }
            while (j != e && char_is_line_break(*j)) {
                result += newline;
                auto c = *j;
                ++j;
                if (j != e && c == U'\r' && *j == U'\n')
                    ++j;
                last_break = true;
            }
            i = j;
        }
        if (! last_break && ! result.empty())
            result += newline;
        return result;
    }

    Ustring str_unify_lines(const Ustring& str, char32_t newline) {
        return str_unify_lines(str, str_char(newline));
    }

    Ustring str_unify_lines(const Ustring& str) {
        return str_unify_lines(str, "\n");
    }

    void str_unify_lines_in(Ustring& str, const Ustring& newline) {
        auto result = str_unify_lines(str, newline);
        str.swap(result);
    }

    void str_unify_lines_in(Ustring& str, char32_t newline) {
        str_unify_lines_in(str, str_char(newline));
    }

    void str_unify_lines_in(Ustring& str) {
        str_unify_lines_in(str, "\n");
    }

    void Wrap::init() {
        if (width_ == npos) {
            auto columns = decnum(cstr(getenv("COLUMNS")));
            if (columns < 3)
                columns = 80;
            width_ = size_t(columns) - 2;
        }
        if (margin2_ == npos)
            margin2_ = margin_;
        if (margin_ >= width_ || margin2_ >= width_)
            throw std::length_error("Word wrap width and margins are inconsistent");
        pbreak_ = lines_ ? 1 : 2;
        spacing_ = flags_ & Length::wide ? 2 : 1;
    }

    void Wrap::do_wrap(const Ustring& src, Ustring& dst) const {
        using namespace UnicornDetail;
        auto i = utf_begin(src), end = utf_end(src);
        size_t linewidth = 0, words = 0, linebreaks = 0, spaces = margin_, tailspaces = 0;
        while (i != end) {
            auto j = std::find_if_not(i, end, char_is_white_space);
            if (j == end)
                break;
            check_whitespace(i, j, linebreaks, tailspaces);
            if (! dst.empty() && linebreaks >= pbreak_) {
                if (words > 0) {
                    dst += newpara_;
                    words = linewidth = 0;
                } else {
                    dst += newline_;
                }
                spaces = margin_;
            }
            i = j;
            if (preserve_ && linebreaks >= 1 && tailspaces >= 1) {
                if (words > 0)
                    dst += newline_;
                dst.append(tailspaces, ' ');
                j = std::find_if(i, end, char_is_line_break);
                dst += str_unify_lines(u_str(i, j), newline_);
                words = linewidth = 0;
            } else {
                j = std::find_if(i, end, char_is_white_space);
                auto word = u_str(i, j);
                auto wordlen = str_length(word, flags_ & all_length_flags);
                if (words > 0) {
                    if (linewidth + wordlen + spacing_ > size_t(width_)) {
                        dst += newline_;
                        words = linewidth = 0;
                    } else {
                        dst += ' ';
                        linewidth += spacing_;
                    }
                }
                if (words == 0) {
                    dst.append(spaces, ' ');
                    linewidth = spaces * spacing_;
                    spaces = margin2_;
                }
                dst += word;
                ++words;
                linewidth += wordlen;
                if (enforce_ && linewidth > size_t(width_))
                    throw std::length_error("Word is too long for wrapping width");
            }
            i = j;
        }
        if (words > 0)
            dst += newline_;
    }

}
