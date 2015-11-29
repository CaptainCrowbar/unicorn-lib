#include "unicorn/string-escape.hpp"
#include <cstring>

namespace Unicorn {

    namespace UnicornDetail {

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

    }

    void str_encode_uri_in(u8string& str) {
        u8string result;
        UnicornDetail::encode_uri_helper(str, result, UnicornDetail::uri_full_escaped);
        str = std::move(result);
    }

    void str_encode_uri_component_in(u8string& str) {
        u8string result;
        UnicornDetail::encode_uri_helper(str, result, UnicornDetail::uri_comp_escaped);
        str = std::move(result);
    }

    u8string str_unencode_uri(const u8string& str) {
        u8string result;
        UnicornDetail::unencode_uri_helper(str, result);
        return result;
    }

    void str_unencode_uri_in(u8string& str) {
        u8string result;
        UnicornDetail::unencode_uri_helper(str, result);
        str = std::move(result);
    }

}
