#include "unicorn/string-algorithm.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-property.hpp"
#include "unicorn/string-size.hpp"
#include <algorithm>

namespace Unicorn {

    size_t str_common(const u8string& s1, const u8string& s2, size_t start) noexcept {
        if (start >= s1.size() || start >= s2.size())
            return 0;
        auto p = s1.data() + start;
        for (auto q = s2.data() + start, endp = p + std::min(s1.size(), s2.size()); p != endp && *p == *q; ++p, ++q) {}
        return p - s1.data() - start;
    }

    size_t str_common_utf(const u8string& s1, const u8string& s2, size_t start) noexcept {
        size_t n = str_common(s1, s2, start);
        if (n == 0)
            return 0;
        size_t pos = start + n;
        if (! is_following_unit(s1[pos]) && ! is_following_unit(s2[pos]))
            return n;
        --pos;
        while (pos > start && is_following_unit(s1[pos]))
            --pos;
        return pos - start;
    }

    bool str_expect(Utf8Iterator& i, const Utf8Iterator& end, const u8string& prefix) {
        size_t psize = prefix.size();
        if (psize == 0 || end.offset() - i.offset() < psize
                || memcmp(i.source().data() + i.offset(), prefix.data(), psize) != 0)
            return false;
        i = utf_iterator(i.source(), i.offset() + psize);
        return true;
    }

    bool str_expect(Utf8Iterator& i, const u8string& prefix) {
        return str_expect(i, utf_end(i.source()), prefix);
    }

    Utf8Iterator str_find_char(const Utf8Iterator& b, const Utf8Iterator& e, char32_t c) {
        return std::find(b, e, c);
    }

    Utf8Iterator str_find_char(const Irange<Utf8Iterator>& range, char32_t c) {
        return std::find(range.begin(), range.end(), c);
    }

    Utf8Iterator str_find_char(const u8string& str, char32_t c) {
        return utf_iterator(str, str.find(str_char<char>(c)));
    }

    Utf8Iterator str_find_last_char(const Utf8Iterator& b, const Utf8Iterator& e, char32_t c) {
        auto i = e;
        while (i != b) {
            --i;
            if (*i == c)
                return i;
        }
        return e;
    }

    Utf8Iterator str_find_last_char(const Irange<Utf8Iterator>& range, char32_t c) {
        return str_find_last_char(range.begin(), range.end(), c);
    }

    Utf8Iterator str_find_last_char(const u8string& str, char32_t c) {
        return utf_iterator(str, str.rfind(str_char<char>(c)));
    }

    Utf8Iterator str_find_first_of(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target) {
        auto u_target = to_utf32(target);
        return std::find_if(b, e,
            [&] (char32_t c) { return u_target.find(c) != npos; });
    }

    Utf8Iterator str_find_first_of(const Irange<Utf8Iterator>& range, const u8string& target) {
        return str_find_first_of(range.begin(), range.end(), target);
    }

    Utf8Iterator str_find_first_of(const u8string& str, const u8string& target) {
        return str_find_first_of(utf_begin(str), utf_end(str), target);
    }

    Utf8Iterator str_find_first_not_of(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target) {
        auto u_target = to_utf32(target);
        return std::find_if(b, e,
            [&] (char32_t c) { return u_target.find(c) == npos; });
    }

    Utf8Iterator str_find_first_not_of(const Irange<Utf8Iterator>& range, const u8string& target) {
        return str_find_first_not_of(range.begin(), range.end(), target);
    }

    Utf8Iterator str_find_first_not_of(const u8string& str, const u8string& target) {
        return str_find_first_not_of(utf_begin(str), utf_end(str), target);
    }

    Utf8Iterator str_find_last_of(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target) {
        auto u_target = to_utf32(target);
        auto i = e;
        while (i != b) {
            --i;
            if (u_target.find(*i) != npos)
                return i;
        }
        return e;
    }

    Utf8Iterator str_find_last_of(const Irange<Utf8Iterator>& range, const u8string& target) {
        return str_find_last_of(range.begin(), range.end(), target);
    }

    Utf8Iterator str_find_last_of(const u8string& str, const u8string& target) {
        return str_find_last_of(utf_begin(str), utf_end(str), target);
    }

    Utf8Iterator str_find_last_not_of(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target) {
        auto u_target = to_utf32(target);
        auto i = e;
        while (i != b) {
            --i;
            if (u_target.find(*i) == npos)
                return i;
        }
        return e;
    }

    Utf8Iterator str_find_last_not_of(const Irange<Utf8Iterator>& range, const u8string& target) {
        return str_find_last_not_of(range.begin(), range.end(), target);
    }

    Utf8Iterator str_find_last_not_of(const u8string& str, const u8string& target) {
        return str_find_last_not_of(utf_begin(str), utf_end(str), target);
    }

    Utf8Iterator str_search(const Utf8Iterator& b, const Utf8Iterator& e, const u8string& target) {
        auto u_target = to_utf32(target);
        return std::search(b, e, u_target.begin(), u_target.end());
    }

    Utf8Iterator str_search(const Irange<Utf8Iterator>& range, const u8string& target) {
        return str_search(range.begin(), range.end(), target);
    }

    Utf8Iterator str_search(const u8string& str, const u8string& target) {
        return str_search(utf_begin(str), utf_end(str), target);
    }

    size_t str_skipws(Utf8Iterator& i, const Utf8Iterator& end) {
        size_t n = 0;
        for (; i != end && char_is_white_space(*i); ++i, ++n) {}
        return n;
    }

    size_t str_skipws(Utf8Iterator& i) {
        return str_skipws(i, utf_end(i.source()));
    }

}
