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

    // TODO

    template <typename C> basic_string<C> str_encode_uri(const basic_string<C>& str);
    template <typename C> basic_string<C> str_encode_uri_component(const basic_string<C>& str);
    template <typename C> void str_encode_uri_in(basic_string<C>& str);
    template <typename C> void str_encode_uri_in_component(basic_string<C>& str);

    template <typename C> basic_string<C> str_escape(const basic_string<C>& str, uint32_t flags = 0);
    template <typename C> void str_escape_in(basic_string<C>& str, uint32_t flags = 0);

    template <typename C> basic_string<C> str_quote(const basic_string<C>& str);
    template <typename C> basic_string<C> str_quote(const basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0);
    template <typename C> basic_string<C> str_quote(const basic_string<C>& str, const C* quotes, uint32_t flags = 0);
    template <typename C> void str_quote_in(basic_string<C>& str);
    template <typename C> void str_quote_in(basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0);
    template <typename C> void str_quote_in(basic_string<C>& str, const C* quotes, uint32_t flags = 0);

    template <typename C> basic_string<C> str_unencode_uri(const basic_string<C>& str);
    template <typename C> void str_unencode_uri_in(basic_string<C>& str);

    template <typename C> basic_string<C> str_unescape(const basic_string<C>& str, uint32_t flags = 0);
    template <typename C> void str_unescape_in(basic_string<C>& str, uint32_t flags = 0);

    template <typename C> basic_string<C> str_unquote(const basic_string<C>& str);
    template <typename C> basic_string<C> str_unquote(const basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0);
    template <typename C> basic_string<C> str_unquote(const basic_string<C>& str, const C* quotes, uint32_t flags = 0);
    template <typename C> void str_unquote_in(basic_string<C>& str);
    template <typename C> void str_unquote_in(basic_string<C>& str, const basic_string<C>& quotes, uint32_t flags = 0);
    template <typename C> void str_unquote_in(basic_string<C>& str, const C* quotes, uint32_t flags = 0);

}
