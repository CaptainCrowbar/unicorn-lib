#include "unicorn/string-forward.hpp"

namespace Unicorn {

    void str_append(u8string& str, const Utf8Iterator& suffix_begin, const Utf8Iterator& suffix_end) {
        str.append(suffix_begin.source(), suffix_begin.offset(), suffix_end.offset() - suffix_begin.offset());
    }

    void str_append(u8string& str, const Irange<Utf8Iterator>& suffix) {
        str_append(str, suffix.begin(), suffix.end());
    }

    void str_append(u8string& str, const char* suffix) {
        if (suffix)
            str += suffix;
    }

    void str_append(u8string& dst, const char* ptr, size_t n) {
        if (ptr)
            dst.append(ptr, n);
    }

    void str_append_chars(u8string& dst, size_t n, char32_t c) {
        auto s = str_char(c);
        if (s.size() == 1)
            dst.append(n, s[0]);
        else
            dst += str_repeat(s, n);
    }

    u8string str_char(char32_t c) {
        using namespace UnicornDetail;
        u8string str(4, '\0');
        auto len = UtfEncoding<char>::encode(c, &str[0]);
        if (len == 0)
            len = UtfEncoding<char>::encode(replacement_char, &str[0]);
        str.resize(len);
        return str;
    }

    u8string str_chars(size_t n, char32_t c) {
        using namespace UnicornDetail;
        if (n == 0)
            return {};
        u8string str(4, '\0');
        auto len = UtfEncoding<char>::encode(c, &str[0]);
        if (len == 0)
            len = UtfEncoding<char>::encode(replacement_char, &str[0]);
        str.resize(len);
        return str_repeat(str, n);
    }

    u8string str_repeat(const u8string& str, size_t n) {
        if (n == 0 || str.empty())
            return {};
        if (n == 1)
            return str;
        if (str.size() == 1)
            return u8string(n, str[0]);
        size_t size = n * str.size();
        auto dst = str;
        dst.reserve(size);
        while (dst.size() <= size / 2)
            dst += dst;
        dst += str_repeat(str, n - dst.size() / str.size());
        return dst;
    }

    void str_repeat_in(u8string& str, size_t n) {
        auto dst = str_repeat(str, n);
        str.swap(dst);
    }

}
