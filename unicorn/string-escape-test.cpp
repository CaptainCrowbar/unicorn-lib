#include "unicorn/core.hpp"
#include "unicorn/string.hpp"
#include "unicorn/string-escape.hpp" // TODO
#include "prion/unit-test.hpp"
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void check_encode_uri() {

        u8string s1, s2;
        u16string s16;
        u32string s32;

        s1 = "";                         TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "");
        s1 = "hello world";              TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "hello%20world");
        s1 = "http://www.example.com/";  TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
        s1 = "\"%<>\\^`{|}";             TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s1 = "-._~";                     TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "-._~");
        s1 = u8"αβγδε";                  TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s1 = "";                         TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "");
        s1 = "hello world";              TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "hello%20world");
        s1 = "http://www.example.com/";  TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "http%3a%2f%2fwww.example.com%2f");
        s1 = "\"%<>\\^`{|}";             TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
        s1 = "-._~";                     TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "-._~");
        s1 = u8"αβγδε";                  TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s1 = "";                         TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "");
        s1 = "hello world";              TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "hello%20world");
        s1 = "http://www.example.com/";  TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "http://www.example.com/");
        s1 = "\"%<>\\^`{|}";             TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "!#$&'()*+,/:;=?@[]");
        s1 = "-._~";                     TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "-._~");
        s1 = u8"αβγδε";                  TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s1 = "";                         TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "");
        s1 = "hello world";              TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "hello%20world");
        s1 = "http://www.example.com/";  TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "http%3a%2f%2fwww.example.com%2f");
        s1 = "\"%<>\\^`{|}";             TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
        s1 = "-._~";                     TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "-._~");
        s1 = u8"αβγδε";                  TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s16 = u"";                         TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "");
        s16 = u"hello world";              TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "hello%20world");
        s16 = u"http://www.example.com/";  TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "http://www.example.com/");
        s16 = u"\"%<>\\^`{|}";             TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s16 = u"!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s16 = u"-._~";                     TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "-._~");
        s16 = u"αβγδε";                    TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s16 = u"";                         TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "");
        s16 = u"hello world";              TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "hello%20world");
        s16 = u"http://www.example.com/";  TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "http%3a%2f%2fwww.example.com%2f");
        s16 = u"\"%<>\\^`{|}";             TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s16 = u"!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
        s16 = u"-._~";                     TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "-._~");
        s16 = u"αβγδε";                    TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s32 = U"";                         TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "");
        s32 = U"hello world";              TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "hello%20world");
        s32 = U"http://www.example.com/";  TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "http://www.example.com/");
        s32 = U"\"%<>\\^`{|}";             TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s32 = U"!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s32 = U"-._~";                     TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "-._~");
        s32 = U"αβγδε";                    TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s32 = U"";                         TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "");
        s32 = U"hello world";              TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "hello%20world");
        s32 = U"http://www.example.com/";  TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "http%3a%2f%2fwww.example.com%2f");
        s32 = U"\"%<>\\^`{|}";             TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s32 = U"!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
        s32 = U"-._~";                     TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "-._~");
        s32 = U"αβγδε";                    TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s1 = "";                                                        TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "");
        s1 = "hello%20world";                                           TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "hello world");
        s1 = "http://www.example.com/";                                 TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
        s1 = "http%3a%2f%2fwww.example.com%2f";                         TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
        s1 = "%22%25%3c%3e%5c%5e%60%7b%7c%7d";                          TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "\"%<>\\^`{|}");
        s1 = "!#$&'()*+,/:;=?@[]";                                      TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s1 = "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d";  TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s1 = "-._~";                                                    TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "-._~");
        s1 = "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5";                          TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, u8"αβγδε");

        s1 = "";                                                        TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "");
        s1 = "hello%20world";                                           TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "hello world");
        s1 = "http://www.example.com/";                                 TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "http://www.example.com/");
        s1 = "http%3a%2f%2fwww.example.com%2f";                         TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "http://www.example.com/");
        s1 = "%22%25%3c%3e%5c%5e%60%7b%7c%7d";                          TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "\"%<>\\^`{|}");
        s1 = "!#$&'()*+,/:;=?@[]";                                      TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "!#$&'()*+,/:;=?@[]");
        s1 = "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d";  TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "!#$&'()*+,/:;=?@[]");
        s1 = "-._~";                                                    TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "-._~");
        s1 = "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5";                          TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, u8"αβγδε");

    }

    void check_escape() {

        u8string s1, s2;

        s1 = "";  TRY(s2 = str_escape(s1));  TEST_EQUAL(s2, "");

        // TODO

        // `esc_control`    | Escape only C0 controls and backslash (default)
        // `esc_control01`  | Escape C0 and C1 controls
        // `esc_nonascii`   | Escape all characters that are not printable ASCII
        // `esc_punct`      | Escape ASCII punctuation as well as all non-ASCII
        // `esc_keeplf`     | Do not escape line feeds
        // `esc_uhex`       | Use `\u` and `\U` instead of `\x{...}`
        // `esc_surrogate`  | Use two `\u` escapes for astral characters
        // `esc_utf8`       | Use UTF-8 hex bytes for non-ASCII characters

    }

    void check_quote() {

        // TODO

    }

}

TEST_MODULE(unicorn, string_escape) {

    check_encode_uri();
    check_escape();
    check_quote();

}
