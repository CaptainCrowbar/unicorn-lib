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

    namespace {

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

    }

    namespace UnicornDetail {

        constexpr const char* uri_comp_escaped = "!\"#$%&'()*+,/:;<=>?@[\\]^`{|}";
        constexpr const char* uri_full_escaped = "\"%<>\\^`{|}";

        void encode_uri_helper(const u8string& src, u8string& dst, const char* escaped);
        void unencode_uri_helper(const u8string& src, u8string& dst);

        template <typename C>
        void escape_helper(const basic_string<C>& src, basic_string<C>& dst, const u32string& quotes, uint32_t flags) {
            allow_flags(flags, esc_mask, "string escaping");
            exclusive_flags(flags, esc_selection_mask, "string escaping");
            exclusive_flags(flags, esc_unicode_mask, "string escaping");
            bool (*basic_escape)(char32_t c);
            switch (flags & esc_selection_mask) {
                case esc_control01:
                    basic_escape = [] (char32_t c) { return c <= 0x1f || (c >= 0x7f && c <= 0x9f); };
                    break;
                case esc_nonascii:
                    basic_escape = [] (char32_t c) { return c <= 0x1f || c >= 0x7f; };
                    break;
                case esc_punct:
                    basic_escape = [] (char32_t c) { return c >= 0x80 || ! ascii_isalnum(char(c)); };
                    break;
                default:
                    basic_escape = [] (char32_t c) { return c <= 0x1f || c == 0x7f; };
                    break;
            }
            char32_t q1 = '\\', q2 = '\\';
            if (! quotes.empty()) {
                q1 = quotes[0];
                q2 = quotes[1];
            }
            auto any_escape = [=] (char32_t c) { return c == U'\\' || c == q1 || c == q2 || basic_escape(c); };
            auto i = utf_begin(src), end = utf_end(src);
            while (i != end) {
                auto j = std::find_if(i, end, any_escape);
                str_append(dst, i, j);
                if (j == end)
                    break;
                i = j;
                do {
                    // TODO
                    // append escaped *i to dst
                    // default: latin1 => \xHH, other => \x{HHH...}
                    // esc_uhex: bmp => \uHHHH, astral => \UHHHHHHHH
                    // esc_surrogate: bmp => \uHHHH, astral => \uHHHH\uHHHH
                    // esc_utf8: all => \xHH[\xHH\xHH\xHH]
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
    void str_encode_uri_in_component(u8string& str);
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
        UnicornDetail::unescape_helper(str, result);
        return result;
    }

    template <typename C>
    void str_unescape_in(basic_string<C>& str, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::unescape_helper(str, result);
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
