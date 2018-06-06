#include "unicorn/string.hpp"
#include <algorithm>
#include <utility>

namespace RS::Unicorn {

    namespace {

        constexpr const char* hexdigits = "0123456789abcdef";
        constexpr const char* uri_comp_escaped = "!\"#$%&'()*+,/:;<=>?@[\\]^`{|}";
        constexpr const char* uri_full_escaped = "\"%<>\\^`{|}";

        void append_hex_digits(uint32_t x, Ustring& dst) {
            size_t pos = dst.size();
            do {
                dst += hexdigits[x % 16];
                x /= 16;
            } while (x != 0);
            std::reverse(dst.begin() + pos, dst.end());
        }

        void append_hex_digits(uint32_t x, Ustring& dst, size_t digits) {
            size_t pos = dst.size();
            dst.append(digits, '0');
            for (auto i = ptrdiff_t(digits) - 1; i >= 0 && x != 0; --i, x /= 16)
                dst[pos + i] = hexdigits[x % 16];
        }

        void append_escape_pcre(char32_t c, Ustring& dst) {
            dst += "\\x{";
            append_hex_digits(c, dst);
            dst += '}';
        }

        void append_escape_stdc(char32_t c, Ustring& dst) {
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

        void append_escape_u4(char16_t c, Ustring& dst) {
            dst += "\\u";
            append_hex_digits(c, dst, 4);
        }

        void append_escape_u8(char32_t c, Ustring& dst) {
            dst += "\\U";
            append_hex_digits(c, dst, 8);
        }

        void append_escape_x2(char c, Ustring& dst) {
            dst += "\\x";
            append_hex_digits(uint8_t(c), dst, 2);
        }

        void encode_uri_helper(const Ustring& src, Ustring& dst, const char* escaped) {
            for (auto in = utf_begin(src), end = utf_end(src); in != end; ++in) {
                if (*in >= 33 && *in <= 126 && ! strchr(escaped, char(*in))) {
                    dst += char(*in);
                } else {
                    for (char c: in.range())
                        dst += '%' + hex(uint8_t(c));
                }
            }
        }

        void unencode_uri_helper(const Ustring& src, Ustring& dst) {
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

        void escape_helper(const Ustring& src, Ustring& dst, uint32_t flags, char32_t quote = 0xffffffff) {
            const bool ascii = flags & (Escape::ascii | Escape::pcre);
            const bool nostdc = flags & Escape::nostdc;
            const bool pcre = flags & Escape::pcre;
            const bool punct = flags & Escape::punct;
            for (char32_t c: utf_range(src)) {
                if (! nostdc && (c == 0 || (c >= U'\a' && c <= U'\r'))) {
                    append_escape_stdc(c, dst);
                } else if (c <= 0x1f || c == 0x7f) {
                    append_escape_x2(char(c), dst);
                } else if (c == U'\\' || c == quote || (punct && c < 0x80 && ascii_ispunct(char(c)))) {
                    dst += '\\';
                    dst += char(c);
                } else if (c < 0x80 || ! ascii) {
                    str_append_char(dst, c);
                } else if (pcre) {
                    append_escape_pcre(c, dst);
                } else if (c <= 0xffff) {
                    append_escape_u4(char16_t(c), dst);
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

        Utf8Iterator unescape_helper(const Utf8Iterator& begin, const Utf8Iterator& end, Ustring& dst, char32_t quote = 0xffffffff) {
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

        void unquote_helper(const Ustring& src, Ustring& dst, char32_t quote) {
            auto i = utf_begin(src), j = i, end = utf_end(src);
            while (i != end) {
                j = unescape_helper(i, end, dst, quote);
                i = std::find_if(j, end, [quote] (char32_t c) { return c != quote; });
            }
        }

    }

    Ustring str_encode_uri(const Ustring& str) {
        Ustring result;
        encode_uri_helper(str, result, uri_full_escaped);
        return result;
    }

    Ustring str_encode_uri_component(const Ustring& str) {
        Ustring result;
        encode_uri_helper(str, result, uri_comp_escaped);
        return result;
    }

    void str_encode_uri_in(Ustring& str) {
        Ustring result;
        encode_uri_helper(str, result, uri_full_escaped);
        str = move(result);
    }

    void str_encode_uri_component_in(Ustring& str) {
        Ustring result;
        encode_uri_helper(str, result, uri_comp_escaped);
        str = move(result);
    }

    Ustring str_unencode_uri(const Ustring& str) {
        Ustring result;
        unencode_uri_helper(str, result);
        return result;
    }

    void str_unencode_uri_in(Ustring& str) {
        Ustring result;
        unencode_uri_helper(str, result);
        str = move(result);
    }

    Ustring str_escape(const Ustring& str, uint32_t flags) {
        Ustring result;
        escape_helper(str, result, flags);
        return result;
    }

    void str_escape_in(Ustring& str, uint32_t flags) {
        Ustring result;
        escape_helper(str, result, flags);
        str = move(result);
    }

    Ustring str_unescape(const Ustring& str) {
        Ustring result;
        unescape_helper(utf_begin(str), utf_end(str), result);
        return result;
    }

    void str_unescape_in(Ustring& str) {
        Ustring result;
        unescape_helper(utf_begin(str), utf_end(str), result);
        str = move(result);
    }

    Ustring str_quote(const Ustring& str, uint32_t flags, char32_t quote) {
        Ustring result;
        str_append_char(result, quote);
        escape_helper(str, result, flags, quote);
        str_append_char(result, quote);
        return result;
    }

    void str_quote_in(Ustring& str, uint32_t flags, char32_t quote) {
        Ustring result;
        str_append_char(result, quote);
        escape_helper(str, result, flags, quote);
        str_append_char(result, quote);
        str = move(result);
    }

    Ustring str_unquote(const Ustring& str, char32_t quote) {
        Ustring result;
        unquote_helper(str, result, quote);
        return result;
    }

    void str_unquote_in(Ustring& str, char32_t quote) {
        Ustring result;
        unquote_helper(str, result, quote);
        str = move(result);
    }

}
