#include "unicorn/string-escape.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-property.hpp"
#include <algorithm>
#include <cstring>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace Unicorn {

    namespace {

        constexpr const char* hexdigits = "0123456789abcdef";
        constexpr const char* uri_comp_escaped = "!\"#$%&'()*+,/:;<=>?@[\\]^`{|}";
        constexpr const char* uri_full_escaped = "\"%<>\\^`{|}";

        void append_hex_digits(uint32_t x, u8string& dst) {
            size_t pos = dst.size();
            do {
                dst += hexdigits[x % 16];
                x /= 16;
            } while (x != 0);
            std::reverse(dst.begin() + pos, dst.end());
        }

        void append_hex_digits(uint32_t x, u8string& dst, size_t digits) {
            size_t pos = dst.size();
            dst.append(digits, '0');
            for (auto i = ptrdiff_t(digits) - 1; i >= 0 && x != 0; --i, x /= 16)
                dst[pos + i] = hexdigits[x % 16];
        }

        void append_escape_pcre(char32_t c, u8string& dst) {
            dst += "\\x{";
            append_hex_digits(c, dst);
            dst += '}';
        }

        void append_escape_stdc(char32_t c, u8string& dst) {
            switch (c) {
                case 0:     dst += "\\0"; break;
                case '\a':  dst += "\\a"; break;
                case '\b':  dst += "\\b"; break;
                case '\t':  dst += "\\t"; break;
                case '\n':  dst += "\\n"; break;
                case '\v':  dst += "\\v"; break;
                case '\f':  dst += "\\f"; break;
                case '\r':  dst += "\\r"; break;
                default:    break;
            }
        }

        void append_escape_u4(char16_t c, u8string& dst) {
            dst += "\\u";
            append_hex_digits(c, dst, 4);
        }

        void append_escape_u8(char32_t c, u8string& dst) {
            dst += "\\U";
            append_hex_digits(c, dst, 8);
        }

        void append_escape_x2(char c, u8string& dst) {
            dst += "\\x";
            append_hex_digits(uint8_t(c), dst, 2);
        }

        void encode_uri_helper(const u8string& src, u8string& dst, const char* escaped) {
            for (auto in = utf_begin(src), end = utf_end(src); in != end; ++in) {
                if (*in >= 33 && *in <= 126 && ! strchr(escaped, char(*in))) {
                    dst += char(*in);
                } else {
                    for (char c: in.range()) {
                        dst += '%';
                        PrionDetail::append_hex_byte(uint8_t(c), dst);
                    }
                }
            }
        }

        void unencode_uri_helper(const u8string& src, u8string& dst) {
            size_t i = 0, size = src.size();
            while (i < size) {
                if (src[i] == '%' && size - i >= 3 && ascii_isxdigit(src[i + 1]) && ascii_isxdigit(src[i + 2])) {
                    dst += char(hexnum(src.substr(i + 1, 2)));
                    i += 3;
                } else {
                    dst += src[i++];
                }
            }
        }

        void escape_helper(const u8string& src, u8string& dst, uint32_t flags, char32_t quote = 0xffffffff) {
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
                    dst += '\\';
                    dst += char(c);
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

        void read_hex_char(Utf8Iterator& i, Utf8Iterator end, char32_t& c, size_t maxlen, bool varlen) {
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

        Utf8Iterator unescape_helper(const Utf8Iterator& begin, const Utf8Iterator& end, u8string& dst, char32_t quote = 0xffffffff) {
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
                switch (c) {
                    case U'0':  c = U'\0'; ++i; break;
                    case U'a':  c = U'\a'; ++i; break;
                    case U'b':  c = U'\b'; ++i; break;
                    case U't':  c = U'\t'; ++i; break;
                    case U'n':  c = U'\n'; ++i; break;
                    case U'v':  c = U'\v'; ++i; break;
                    case U'f':  c = U'\f'; ++i; break;
                    case U'r':  c = U'\r'; ++i; break;
                    case U'x':  ++i; read_hex_char(i, end, c, 2, true); break;
                    case U'u':  ++i; read_hex_char(i, end, c, 4, false); break;
                    case U'U':  ++i; read_hex_char(i, end, c, 8, false); break;
                    default:    ++i; break;
                }
                str_append_char(dst, c);
            }
            return end;
        }

        void unquote_helper(const u8string& src, u8string& dst, char32_t quote) {
            auto i = utf_begin(src), j = i, end = utf_end(src);
            while (i != end) {
                j = unescape_helper(i, end, dst, quote);
                i = std::find_if(j, end, [quote] (char32_t c) { return c != quote; });
            }
        }

    }

    u8string str_encode_uri(const u8string& str) {
        u8string result;
        encode_uri_helper(str, result, uri_full_escaped);
        return result;
    }

    u8string str_encode_uri_component(const u8string& str) {
        u8string result;
        encode_uri_helper(str, result, uri_comp_escaped);
        return result;
    }

    void str_encode_uri_in(u8string& str) {
        u8string result;
        encode_uri_helper(str, result, uri_full_escaped);
        str = move(result);
    }

    void str_encode_uri_component_in(u8string& str) {
        u8string result;
        encode_uri_helper(str, result, uri_comp_escaped);
        str = move(result);
    }

    u8string str_unencode_uri(const u8string& str) {
        u8string result;
        unencode_uri_helper(str, result);
        return result;
    }

    void str_unencode_uri_in(u8string& str) {
        u8string result;
        unencode_uri_helper(str, result);
        str = move(result);
    }

    u8string str_escape(const u8string& str, uint32_t flags) {
        u8string result;
        escape_helper(str, result, flags);
        return result;
    }

    void str_escape_in(u8string& str, uint32_t flags) {
        u8string result;
        escape_helper(str, result, flags);
        str = move(result);
    }

    u8string str_unescape(const u8string& str) {
        u8string result;
        unescape_helper(utf_begin(str), utf_end(str), result);
        return result;
    }

    void str_unescape_in(u8string& str) {
        u8string result;
        unescape_helper(utf_begin(str), utf_end(str), result);
        str = move(result);
    }

    u8string str_quote(const u8string& str, uint32_t flags, char32_t quote) {
        u8string result;
        str_append_char(result, quote);
        escape_helper(str, result, flags, quote);
        str_append_char(result, quote);
        return result;
    }

    void str_quote_in(u8string& str, uint32_t flags, char32_t quote) {
        u8string result;
        str_append_char(result, quote);
        escape_helper(str, result, flags, quote);
        str_append_char(result, quote);
        str = move(result);
    }

    u8string str_unquote(const u8string& str, char32_t quote) {
        u8string result;
        unquote_helper(str, result, quote);
        return result;
    }

    void str_unquote_in(u8string& str, char32_t quote) {
        u8string result;
        unquote_helper(str, result, quote);
        str = move(result);
    }

}
