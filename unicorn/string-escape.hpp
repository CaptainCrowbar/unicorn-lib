// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <string>

namespace Unicorn {

    constexpr uint32_t esc_ascii   = 1ul << 0;  // Escape all non-ASCII characters
    constexpr uint32_t esc_nostdc  = 1ul << 1;  // Do not use standard C symbols such as `\n`
    constexpr uint32_t esc_pcre    = 1ul << 2;  // Use `\x{...}` instead of `\u` and `\U` (implies `esc_nonascii`)
    constexpr uint32_t esc_punct   = 1ul << 3;  // Escape ASCII punctuation

    u8string str_encode_uri(const u8string& str);
    u8string str_encode_uri_component(const u8string& str);
    void str_encode_uri_in(u8string& str);
    void str_encode_uri_component_in(u8string& str);
    u8string str_unencode_uri(const u8string& str);
    void str_unencode_uri_in(u8string& str);
    u8string str_escape(const u8string& str, uint32_t flags = 0);
    void str_escape_in(u8string& str, uint32_t flags = 0);
    u8string str_unescape(const u8string& str);
    void str_unescape_in(u8string& str);
    u8string str_quote(const u8string& str, uint32_t flags = 0, char32_t quote = U'\"');
    void str_quote_in(u8string& str, uint32_t flags = 0, char32_t quote = U'\"');
    u8string str_unquote(const u8string& str, char32_t quote = U'\"');
    void str_unquote_in(u8string& str, char32_t quote = U'\"');

}
