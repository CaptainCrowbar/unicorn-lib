// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-property.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>

namespace Unicorn {

    UNICORN_DEFINE_FLAG(string escaping, esc_ascii,   0);  // Escape all non-ASCII characters
    UNICORN_DEFINE_FLAG(string escaping, esc_nostdc,  1);  // Do not use standard C symbols such as `\n`
    UNICORN_DEFINE_FLAG(string escaping, esc_pcre,    2);  // Use `\x{...}` instead of `\u` and `\U` (implies `esc_nonascii`)
    UNICORN_DEFINE_FLAG(string escaping, esc_punct,   3);  // Escape ASCII punctuation

    namespace UnicornDetail {

        constexpr const char* uri_comp_escaped = "!\"#$%&'()*+,/:;<=>?@[\\]^`{|}";
        constexpr const char* uri_full_escaped = "\"%<>\\^`{|}";

        void encode_uri_helper(const u8string& src, u8string& dst, const char* escaped);
        void unencode_uri_helper(const u8string& src, u8string& dst);

        template <typename C>
        void append_hex_digits(uint32_t x, basic_string<C>& dst) {
            static const auto hexdigits = PRI_CSTR("0123456789abcdef", C);
            size_t pos = dst.size();
            do {
                dst += hexdigits[x % 16];
                x /= 16;
            } while (x != 0);
            std::reverse(dst.begin() + pos, dst.end());
        }

        template <typename C>
        void append_hex_digits(uint32_t x, basic_string<C>& dst, size_t digits) {
            static const auto hexdigits = PRI_CSTR("0123456789abcdef", C);
            size_t pos = dst.size();
            dst.append(digits, C('0'));
            for (auto i = ptrdiff_t(digits) - 1; i >= 0 && x != 0; --i, x /= 16)
                dst[pos + i] = hexdigits[x % 16];
        }

        template <typename C>
        void append_escape_pcre(char32_t c, basic_string<C>& dst) {
            dst += C('\\');
            dst += C('x');
            dst += C('{');
            append_hex_digits(c, dst);
            dst += C('}');
        }

        template <typename C>
        void append_escape_stdc(char32_t c, basic_string<C>& dst) {
            switch (c) {
                case 0:        dst += C('\\'); dst += C('0'); break;
                case C('\a'):  dst += C('\\'); dst += C('a'); break;
                case C('\b'):  dst += C('\\'); dst += C('b'); break;
                case C('\t'):  dst += C('\\'); dst += C('t'); break;
                case C('\n'):  dst += C('\\'); dst += C('n'); break;
                case C('\v'):  dst += C('\\'); dst += C('v'); break;
                case C('\f'):  dst += C('\\'); dst += C('f'); break;
                case C('\r'):  dst += C('\\'); dst += C('r'); break;
                default:       break;
            }
        }

        template <typename C>
        void append_escape_u4(char16_t c, basic_string<C>& dst) {
            dst += C('\\');
            dst += C('u');
            append_hex_digits(c, dst, 4);
        }

        template <typename C>
        void append_escape_u8(char32_t c, basic_string<C>& dst) {
            dst += C('\\');
            dst += C('U');
            append_hex_digits(c, dst, 8);
        }

        template <typename C>
        void append_escape_x2(char c, basic_string<C>& dst) {
            dst += C('\\');
            dst += C('x');
            append_hex_digits(uint8_t(c), dst, 2);
        }

        template <typename C>
        void escape_helper(const basic_string<C>& src, basic_string<C>& dst, uint32_t flags, char32_t quote = 0xffffffff) {
            const bool ascii = flags & (esc_ascii | esc_pcre);
            const bool nostdc = flags & esc_nostdc;
            const bool pcre = flags & esc_pcre;
            const bool punct = flags & esc_punct;
            for (char32_t c: utf_range(src)) {
                if (! nostdc && (c == 0 || (c >= U'\a' && c <= U'\r'))) {
                    append_escape_stdc(c, dst);
                } else if (c <= 0x1f || c == 0x7f) {
                    append_escape_x2(c, dst);
                } else if (c == U'\\' || c == quote || (punct && c < 0x80 && ascii_ispunct(char(c)))) {
                    dst += C('\\');
                    dst += C(c);
                } else if (c < 0x80 || ! ascii) {
                    str_append_char(dst, c);
                } else if (pcre) {
                    append_escape_pcre(c, dst);
                } else if (c <= 0xffff) {
                    append_escape_u4(c, dst);
                } else {
                    append_escape_u8(c, dst);
                }
            }
        }

        constexpr uint32_t decode_xdigit(char32_t c) {
            return c >= U'0' && c <= U'9' ? c - U'0'
                : c >= U'A' && c <= U'F' ? c - U'A' + 10
                : c >= U'a' && c <= U'f' ? c - U'a' + 10 : 0;
        }

        template <typename C>
        void read_hex_char(UtfIterator<C>& i, UtfIterator<C> end, char32_t& c, size_t maxlen, bool varlen) {
            if (i == end)
                return;
            if (varlen && *i == U'{') {
                ++i;
                c = 0;
                for (; i != end && char_is_xdigit(*i); ++i)
                    c = (c << 4) + decode_xdigit(*i);
                if (i != end && *i == U'}')
                    ++i;
            } else {
                if (! char_is_xdigit(*i))
                    return;
                c = 0;
                for (size_t j = 0; j < maxlen && i != end && char_is_xdigit(*i); ++i, ++j)
                    c = (c << 4) + decode_xdigit(*i);
            }
        }

        template <typename C>
        UtfIterator<C> unescape_helper(const UtfIterator<C>& begin, const UtfIterator<C>& end, basic_string<C>& dst, char32_t quote = 0xffffffff) {
            auto i = begin, j = begin;
            while (i != end) {
                for (j = i; j != end && *j != U'\\' && *j != quote; ++j) {}
                str_append(dst, i, j);
                if (j == end || *j == quote)
                    return j;
                i = std::next(j);
                if (i == end) {
                    str_append_char(dst, *j);
                    return end;
                }
                char32_t c = *i;
                switch (*i) {
                    case C('0'):  c = U'\0'; ++i; break;
                    case C('a'):  c = U'\a'; ++i; break;
                    case C('b'):  c = U'\b'; ++i; break;
                    case C('t'):  c = U'\t'; ++i; break;
                    case C('n'):  c = U'\n'; ++i; break;
                    case C('v'):  c = U'\v'; ++i; break;
                    case C('f'):  c = U'\f'; ++i; break;
                    case C('r'):  c = U'\r'; ++i; break;
                    case C('x'):  ++i; read_hex_char(i, end, c, 2, true); break;
                    case C('u'):  ++i; read_hex_char(i, end, c, 4, false); break;
                    case C('U'):  ++i; read_hex_char(i, end, c, 8, false); break;
                    default:      ++i; break;
                }
                str_append_char(dst, c);
            }
            return end;
        }

        template <typename C>
        void unquote_helper(const basic_string<C>& src, basic_string<C>& dst, char32_t quote) {
            auto i = utf_begin(src), j = i, end = utf_end(src);
            while (i != end) {
                j = unescape_helper(i, end, dst, quote);
                i = std::find_if(j, end, [quote] (char32_t c) { return c != quote; });
            }
        }

    }

    template <typename C>
    u8string str_encode_uri(const basic_string<C>& str) {
        u8string result;
        UnicornDetail::encode_uri_helper(to_utf8(str), result, UnicornDetail::uri_full_escaped);
        return result;
    }

    template <typename C>
    u8string str_encode_uri_component(const basic_string<C>& str) {
        u8string result;
        UnicornDetail::encode_uri_helper(to_utf8(str), result, UnicornDetail::uri_comp_escaped);
        return result;
    }

    void str_encode_uri_in(u8string& str);
    void str_encode_uri_component_in(u8string& str);
    u8string str_unencode_uri(const u8string& str);
    void str_unencode_uri_in(u8string& str);

    template <typename C>
    basic_string<C> str_escape(const basic_string<C>& str, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::escape_helper(str, result, flags);
        return result;
    }

    template <typename C>
    void str_escape_in(basic_string<C>& str, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::escape_helper(str, result, flags);
        str = std::move(result);
    }

    template <typename C>
    basic_string<C> str_unescape(const basic_string<C>& str) {
        basic_string<C> result;
        UnicornDetail::unescape_helper(utf_begin(str), utf_end(str), result);
        return result;
    }

    template <typename C>
    void str_unescape_in(basic_string<C>& str) {
        basic_string<C> result;
        UnicornDetail::unescape_helper(utf_begin(str), utf_end(str), result);
        str = std::move(result);
    }

    template <typename C>
    basic_string<C> str_quote(const basic_string<C>& str, uint32_t flags = 0, char32_t quote = U'\"') {
        auto result = str_char<C>(quote);
        UnicornDetail::escape_helper(str, result, flags, quote);
        str_append_char(result, quote);
        return result;
    }

    template <typename C>
    void str_quote_in(basic_string<C>& str, uint32_t flags = 0, char32_t quote = U'\"') {
        auto result = str_char<C>(quote);
        UnicornDetail::escape_helper(str, result, flags, quote);
        str_append_char(result, quote);
        str = std::move(result);
    }

    template <typename C>
    basic_string<C> str_unquote(const basic_string<C>& str, char32_t quote = U'\"') {
        basic_string<C> result;
        UnicornDetail::unquote_helper(str, result, quote);
        return result;
    }

    template <typename C>
    void str_unquote_in(basic_string<C>& str, char32_t quote = U'\"') {
        basic_string<C> result;
        UnicornDetail::unquote_helper(str, result, quote);
        str = std::move(result);
    }

}
