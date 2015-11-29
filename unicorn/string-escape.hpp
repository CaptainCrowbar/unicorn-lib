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

    UNICORN_DEFINE_FLAG(string escaping, esc_control,    0);  // Escape only C0 controls and backslash (default)
    UNICORN_DEFINE_FLAG(string escaping, esc_control01,  1);  // Escape C0 and C1 controls
    UNICORN_DEFINE_FLAG(string escaping, esc_nonascii,   2);  // Escape all characters that are not printable ASCII
    UNICORN_DEFINE_FLAG(string escaping, esc_punct,      3);  // Escape ASCII punctuation as well as all non-ASCII
    UNICORN_DEFINE_FLAG(string escaping, esc_keeplf,     4);  // Do not escape line feeds
    UNICORN_DEFINE_FLAG(string escaping, esc_uhex,       5);  // Use `\u` and `\U` instead of `\x`
    UNICORN_DEFINE_FLAG(string escaping, esc_surrogate,  6);  // Use two `\u` escapes for astral characters
    UNICORN_DEFINE_FLAG(string escaping, esc_utf8,       7);  // Use UTF-8 hex bytes for non-ASCII characters

    namespace UnicornDetail {

        constexpr auto esc_selection_mask = esc_control | esc_control01 | esc_nonascii | esc_punct;
        constexpr auto esc_unicode_mask = esc_uhex | esc_surrogate | esc_utf8;
        constexpr auto esc_mask = esc_selection_mask | esc_unicode_mask | esc_keeplf;

        template <typename C>
        u32string fix_quotes(basic_string<C>& quotes) {
            if (quotes.empty())
                return U"\"\"";
            auto i = utf_begin(quotes);
            u32string uquotes{*i++};
            if (i == utf_end(quotes))
                uquotes += uquotes[0];
            else
                uquotes += *i;
            if (uquotes[0] == U'\\' || uquotes[1] == U'\\')
                throw std::invalid_argument("Invalid quote characters: " + quote(to_utf8(quotes)));
            return uquotes;
        }

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
            dst.append(digits, PRI_CHAR('0', C));
            for (auto i = ptrdiff_t(digits) - 1; i >= 0 && x != 0; --i, x /= 16)
                dst[pos + i] = hexdigits[x % 16];
        }

        template <typename C>
        void append_escape_u4(char16_t c, basic_string<C>& dst) {
            dst += PRI_CSTR("\\u", C);
            append_hex_digits(c, dst, 4);
        }

        template <typename C>
        void append_escape_u8(char32_t c, basic_string<C>& dst) {
            dst += PRI_CSTR("\\U", C);
            append_hex_digits(c, dst, 8);
        }

        template <typename C>
        void append_escape_x2(char c, basic_string<C>& dst) {
            dst += PRI_CSTR("\\x", C);
            append_hex_digits(uint8_t(c), dst, 2);
        }

        template <typename C>
        void append_escape_xx(char32_t c, basic_string<C>& dst) {
            dst += PRI_CSTR("\\x{", C);
            append_hex_digits(c, dst);
            dst += PRI_CHAR('}', C);
        }

        template <typename C>
        void escape_helper(const basic_string<C>& src, basic_string<C>& dst, const u32string& quotes, uint32_t flags) {
            allow_flags(flags, esc_mask, "string escaping");
            exclusive_flags(flags, esc_selection_mask, "string escaping");
            exclusive_flags(flags, esc_unicode_mask, "string escaping");
            bool (*basic_escape)(char32_t c);
            switch (flags & esc_selection_mask) {
                case esc_control01:
                    basic_escape = [] (char32_t c) { return (c <= 0x1f && c != 0x0a) || (c >= 0x7f && c <= 0x9f); };
                    break;
                case esc_nonascii:
                    basic_escape = [] (char32_t c) { return (c <= 0x1f && c != 0x0a) || c >= 0x7f; };
                    break;
                case esc_punct:
                    basic_escape = [] (char32_t c) { return c >= 0x80 || (c != 0x0a && ! ascii_isalnum(char(c))); };
                    break;
                default:
                    basic_escape = [] (char32_t c) { return (c <= 0x1f && c != 0x0a) || c == 0x7f; };
                    break;
            }
            const char32_t c1 = (flags & esc_keeplf) ? U'\\' : U'\n';
            const char32_t c2 = quotes.empty() ? U'\\' : quotes[0];
            const char32_t c3 = quotes.size() < 2 ? U'\\' : quotes[1];
            auto any_escape = [=] (char32_t c) { return c == U'\\' || c == c1 || c == c2 || c == c3 || basic_escape(c); };
            auto i = utf_begin(src), end = utf_end(src);
            while (i != end) {
                auto j = std::find_if(i, end, any_escape);
                str_append(dst, i, j);
                if (j == end)
                    break;
                i = j;
                do {
                    // append escaped *i to dst
                    if (flags & esc_uhex) {
                        // esc_uhex: bmp => \uHHHH, astral => \UHHHHHHHH
                        if (char_is_bmp(*i))
                            append_escape_u4(*i, dst);
                        else
                            append_escape_u8(*i, dst);
                    } else if (flags & esc_surrogate) {
                        // esc_surrogate: bmp => \uHHHH, astral => \uHHHH\uHHHH
                        char16_t u16[2];
                        size_t n = UtfEncoding<char16_t>::encode(*i, u16);
                        for (size_t i = 0; i < n; ++i)
                            append_escape_u4(u16[i], dst);
                    } else if (flags & esc_utf8) {
                        // esc_utf8: all => \xHH[\xHH\xHH\xHH]
                        char u8[4];
                        size_t n = UtfEncoding<char>::encode(*i, u8);
                        for (size_t i = 0; i < n; ++i)
                            append_escape_x2(u8[i], dst);
                    } else {
                        // default: latin1 => \xHH, other => \x{HHH...}
                        if (*i <= 0x7f)
                            append_escape_x2(char(*i), dst);
                        else
                            append_escape_xx(*i, dst);
                    }
                    ++i;
                } while (i != end && any_escape(*i));
            }
        }

        template <typename C>
        void unescape_helper(const basic_string<C>& src, basic_string<C>& dst, uint32_t flags) {
            allow_flags(flags, esc_mask, "string escaping");
            exclusive_flags(flags, esc_selection_mask, "string escaping");
            exclusive_flags(flags, esc_unicode_mask, "string escaping");
            // TODO
            (void)src;
            (void)dst;
        }

        template <typename C>
        void unquote_helper(const basic_string<C>& src, basic_string<C>& dst, const u32string& quotes, uint32_t flags) {
            allow_flags(flags, esc_mask, "string escaping");
            exclusive_flags(flags, esc_selection_mask, "string escaping");
            exclusive_flags(flags, esc_unicode_mask, "string escaping");
            // TODO
            (void)src;
            (void)dst;
            (void)quotes;
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
        UnicornDetail::escape_helper(str, result, {}, flags);
        return result;
    }

    template <typename C>
    void str_escape_in(basic_string<C>& str, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::escape_helper(str, result, {}, flags);
        str = std::move(result);
    }

    template <typename C>
    basic_string<C> str_unescape(const basic_string<C>& str, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::unescape_helper(str, result, flags);
        return result;
    }

    template <typename C>
    void str_unescape_in(basic_string<C>& str, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::unescape_helper(str, result, flags);
        str = std::move(result);
    }

    template <typename C>
    basic_string<C> str_quote(const basic_string<C>& str) {
        static constexpr C q = PRI_CHAR('\"', C);
        basic_string<C> result{q};
        UnicornDetail::escape_helper(str, result, U"\"\"", 0);
        result += q;
        return result;
    }

    template <typename C>
    basic_string<C> str_quote(const basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0) {
        auto uquotes = fix_quotes(quotes), result = str_chars(uquotes[0]);
        UnicornDetail::escape_helper(str, result, fix_quotes(quotes), flags);
        str_append_char(result, uquotes[1]);
        return result;
    }

    template <typename C>
    basic_string<C> str_quote(const basic_string<C>& str, const C* quotes, uint32_t flags = 0) {
        auto uquotes = fix_quotes(cstr(quotes)), result = str_chars(uquotes[0]);
        UnicornDetail::escape_helper(str, result, fix_quotes(cstr(quotes)), flags);
        str_append_char(result, uquotes[1]);
        return result;
    }

    template <typename C>
    void str_quote_in(basic_string<C>& str) {
        static constexpr C q = PRI_CHAR('\"', C);
        basic_string<C> result{q};
        UnicornDetail::escape_helper(str, result, U"\"\"", 0);
        result += q;
        str = std::move(result);
    }

    template <typename C>
    void str_quote_in(basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0) {
        auto uquotes = fix_quotes(quotes), result = str_chars(uquotes[0]);
        UnicornDetail::escape_helper(str, result, fix_quotes(quotes), flags);
        str_append_char(result, uquotes[1]);
        str = std::move(result);
    }

    template <typename C>
    void str_quote_in(basic_string<C>& str, const C* quotes, uint32_t flags = 0) {
        auto uquotes = fix_quotes(cstr(quotes)), result = str_chars(uquotes[0]);
        UnicornDetail::escape_helper(str, result, fix_quotes(cstr(quotes)), flags);
        str_append_char(result, uquotes[1]);
        str = std::move(result);
    }

    template <typename C>
    basic_string<C> str_unquote(const basic_string<C>& str) {
        basic_string<C> result;
        UnicornDetail::unquote_helper(str, result, "\"\"", 0);
        return result;
    }

    template <typename C>
    basic_string<C> str_unquote(const basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::unquote_helper(str, result, quotes, flags);
        return result;
    }

    template <typename C>
    basic_string<C> str_unquote(const basic_string<C>& str, const C* quotes, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::unquote_helper(str, result, cstr(quotes), flags);
        return result;
    }

    template <typename C>
    void str_unquote_in(basic_string<C>& str) {
        basic_string<C> result;
        UnicornDetail::unquote_helper(str, result, "\"\"", 0);
        str = std::move(result);
    }

    template <typename C>
    void str_unquote_in(basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::unquote_helper(str, result, quotes, flags);
        str = std::move(result);
    }

    template <typename C>
    void str_unquote_in(basic_string<C>& str, const C* quotes, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::unquote_helper(str, result, cstr(quotes), flags);
        str = std::move(result);
    }

}
