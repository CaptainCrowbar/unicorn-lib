// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-property.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <string>

namespace Unicorn {

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
        return std::find(range.begin(), range.end(), c);
    }

    template <typename C>
    UtfIterator<C> str_find_char(const basic_string<C>& str, char32_t c) {
        return utf_iterator(str, str.find(str_char<C>(c)));
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
        return str_find_last_char(range.begin(), range.end(), c);
    }

    template <typename C>
    UtfIterator<C> str_find_last_char(const basic_string<C>& str, char32_t c) {
        return utf_iterator(str, str.rfind(str_char<C>(c)));
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
        return str_find_first_of(range.begin(), range.end(), target);
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
        return str_find_first_of(range.begin(), range.end(), target);
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
        return str_find_first_not_of(range.begin(), range.end(), target);
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
        return str_find_first_not_of(range.begin(), range.end(), target);
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
        return str_find_last_of(range.begin(), range.end(), target);
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
        return str_find_last_of(range.begin(), range.end(), target);
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
        return str_find_last_not_of(range.begin(), range.end(), target);
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
        return str_find_last_not_of(range.begin(), range.end(), target);
    }

    template <typename C>
    UtfIterator<C> str_find_last_not_of(const basic_string<C>& str, const C* target) {
        return str_find_last_not_of(utf_begin(str), utf_end(str), cstr(target));
    }

    template <typename C>
    UtfIterator<C> str_search(const UtfIterator<C>& b, const UtfIterator<C>& e,
            const basic_string<C>& target) {
        auto u_target = to_utf32(target);
        return std::search(b, e, u_target.begin(), u_target.end());
    }

    template <typename C>
    UtfIterator<C> str_search(const Irange<UtfIterator<C>>& range, const basic_string<C>& target) {
        return str_search(range.begin(), range.end(), target);
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
        return str_search(range.begin(), range.end(), cstr(target));
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

}
