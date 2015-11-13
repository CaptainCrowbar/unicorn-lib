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

    UNICORN_DEFINE_FLAG(string escaping, esc_control,    0);  // Escape only C0 controls and backslash (default)
    UNICORN_DEFINE_FLAG(string escaping, esc_latin,      1);  // Escape C0 and C1 controls
    UNICORN_DEFINE_FLAG(string escaping, esc_nonascii,   2);  // Escape all characters that are not printable ASCII
    UNICORN_DEFINE_FLAG(string escaping, esc_punct,      3);  // Escape ASCII punctuation as well as all non-ASCII
    UNICORN_DEFINE_FLAG(string escaping, esc_keeplf,     4);  // Do not escape line feeds
    UNICORN_DEFINE_FLAG(string escaping, esc_uhex,       5);  // Use `\u` and `\U` instead of `\x`
    UNICORN_DEFINE_FLAG(string escaping, esc_surrogate,  6);  // Use two `\u` escapes for astral characters
    UNICORN_DEFINE_FLAG(string escaping, esc_utf8,       7);  // Use UTF-8 hex bytes for non-ASCII characters

    namespace UnicornDetail {

        constexpr const char* uri_comp_escaped = "!\"#$%&'()*+,/:;<=>?@[\\]^`{|}";
        constexpr const char* uri_full_escaped = "\"%<>\\^`{|}";

        void encode_uri_helper(const u8string& src, u8string& dst, const char* escaped);
        void unencode_uri_helper(const u8string& src, u8string& dst);

        template <typename C>
        void escape_helper(const basic_string<C>& src, basic_string<C>& dst, uint32_t flags) {
            // TODO
            (void)src;
            (void)dst;
            (void)flags;
        }

        template <typename C>
        void unescape_helper(const basic_string<C>& src, basic_string<C>& dst, uint32_t flags) {
            // TODO
            (void)src;
            (void)dst;
            (void)flags;
        }

        template <typename C>
        void quote_helper(const basic_string<C>& src, basic_string<C>& dst, const basic_string<C>& quotes, uint32_t flags) {
            // TODO
            (void)src;
            (void)dst;
            (void)quotes;
            (void)flags;
        }

        template <typename C>
        void unquote_helper(const basic_string<C>& src, basic_string<C>& dst, const basic_string<C>& quotes, uint32_t flags) {
            // TODO
            (void)src;
            (void)dst;
            (void)quotes;
            (void)flags;
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
        basic_string<C> result;
        UnicornDetail::quote_helper(str, result, "\"\"", 0);
        return result;
    }

    template <typename C>
    basic_string<C> str_quote(const basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::quote_helper(str, result, quotes, flags);
        return result;
    }

    template <typename C>
    basic_string<C> str_quote(const basic_string<C>& str, const C* quotes, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::quote_helper(str, result, cstr(quotes), flags);
        return result;
    }

    template <typename C>
    void str_quote_in(basic_string<C>& str) {
        basic_string<C> result;
        UnicornDetail::quote_helper(str, result, "\"\"", 0);
        str = std::move(result);
    }

    template <typename C>
    void str_quote_in(basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::quote_helper(str, result, quotes, flags);
        str = std::move(result);
    }

    template <typename C>
    void str_quote_in(basic_string<C>& str, const C* quotes, uint32_t flags = 0) {
        basic_string<C> result;
        UnicornDetail::quote_helper(str, result, cstr(quotes), flags);
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
