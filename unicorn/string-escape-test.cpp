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
        s1 = "Hello world";              TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "Hello%20world");
        s1 = "http://www.example.com/";  TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
        s1 = "\"%<>\\^`{|}";             TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s1 = "-._~";                     TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "-._~");
        s1 = u8"αβγδε";                  TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s1 = "";                         TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "");
        s1 = "Hello world";              TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "Hello%20world");
        s1 = "http://www.example.com/";  TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "http%3a%2f%2fwww.example.com%2f");
        s1 = "\"%<>\\^`{|}";             TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
        s1 = "-._~";                     TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "-._~");
        s1 = u8"αβγδε";                  TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s1 = "";                         TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "");
        s1 = "Hello world";              TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "Hello%20world");
        s1 = "http://www.example.com/";  TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "http://www.example.com/");
        s1 = "\"%<>\\^`{|}";             TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "!#$&'()*+,/:;=?@[]");
        s1 = "-._~";                     TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "-._~");
        s1 = u8"αβγδε";                  TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s1 = "";                         TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "");
        s1 = "Hello world";              TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "Hello%20world");
        s1 = "http://www.example.com/";  TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "http%3a%2f%2fwww.example.com%2f");
        s1 = "\"%<>\\^`{|}";             TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
        s1 = "-._~";                     TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "-._~");
        s1 = u8"αβγδε";                  TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s16 = u"";                         TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "");
        s16 = u"Hello world";              TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "Hello%20world");
        s16 = u"http://www.example.com/";  TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "http://www.example.com/");
        s16 = u"\"%<>\\^`{|}";             TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s16 = u"!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s16 = u"-._~";                     TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "-._~");
        s16 = u"αβγδε";                    TRY(s2 = str_encode_uri(s16));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s16 = u"";                         TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "");
        s16 = u"Hello world";              TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "Hello%20world");
        s16 = u"http://www.example.com/";  TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "http%3a%2f%2fwww.example.com%2f");
        s16 = u"\"%<>\\^`{|}";             TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s16 = u"!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
        s16 = u"-._~";                     TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "-._~");
        s16 = u"αβγδε";                    TRY(s2 = str_encode_uri_component(s16));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s32 = U"";                         TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "");
        s32 = U"Hello world";              TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "Hello%20world");
        s32 = U"http://www.example.com/";  TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "http://www.example.com/");
        s32 = U"\"%<>\\^`{|}";             TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s32 = U"!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s32 = U"-._~";                     TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "-._~");
        s32 = U"αβγδε";                    TRY(s2 = str_encode_uri(s32));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s32 = U"";                         TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "");
        s32 = U"Hello world";              TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "Hello%20world");
        s32 = U"http://www.example.com/";  TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "http%3a%2f%2fwww.example.com%2f");
        s32 = U"\"%<>\\^`{|}";             TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s32 = U"!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
        s32 = U"-._~";                     TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "-._~");
        s32 = U"αβγδε";                    TRY(s2 = str_encode_uri_component(s32));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s1 = "";                                                        TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "");
        s1 = "Hello%20world";                                           TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "Hello world");
        s1 = "http://www.example.com/";                                 TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
        s1 = "http%3a%2f%2fwww.example.com%2f";                         TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
        s1 = "%22%25%3c%3e%5c%5e%60%7b%7c%7d";                          TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "\"%<>\\^`{|}");
        s1 = "!#$&'()*+,/:;=?@[]";                                      TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s1 = "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d";  TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s1 = "-._~";                                                    TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "-._~");
        s1 = "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5";                          TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, u8"αβγδε");

        s1 = "";                                                        TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "");
        s1 = "Hello%20world";                                           TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "Hello world");
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
        u16string s16;
        u32string s32;

        s1 = "";                                             TRY(s2 = str_escape(s1));              TEST_EQUAL(s2, "");
        s1 = "Hello world";                                  TRY(s2 = str_escape(s1));              TEST_EQUAL(s2, "Hello world");
        s1 = "Hello world";                                  TRY(s2 = str_escape(s1, esc_ascii));   TEST_EQUAL(s2, "Hello world");
        s1 = "Hello world";                                  TRY(s2 = str_escape(s1, esc_nostdc));  TEST_EQUAL(s2, "Hello world");
        s1 = "Hello world";                                  TRY(s2 = str_escape(s1, esc_pcre));    TEST_EQUAL(s2, "Hello world");
        s1 = "Hello world";                                  TRY(s2 = str_escape(s1, esc_punct));   TEST_EQUAL(s2, "Hello world");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1));              TEST_EQUAL(s2, u8"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1, esc_ascii));   TEST_EQUAL(s2, u8"(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1, esc_nostdc));  TEST_EQUAL(s2, u8"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1, esc_pcre));    TEST_EQUAL(s2, u8"(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1, esc_punct));   TEST_EQUAL(s2, u8"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)");

        s1 = "";                                             TRY(str_escape_in(s1));              TEST_EQUAL(s1, "");
        s1 = "Hello world";                                  TRY(str_escape_in(s1));              TEST_EQUAL(s1, "Hello world");
        s1 = "Hello world";                                  TRY(str_escape_in(s1, esc_ascii));   TEST_EQUAL(s1, "Hello world");
        s1 = "Hello world";                                  TRY(str_escape_in(s1, esc_nostdc));  TEST_EQUAL(s1, "Hello world");
        s1 = "Hello world";                                  TRY(str_escape_in(s1, esc_pcre));    TEST_EQUAL(s1, "Hello world");
        s1 = "Hello world";                                  TRY(str_escape_in(s1, esc_punct));   TEST_EQUAL(s1, "Hello world");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1));              TEST_EQUAL(s1, u8"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1, esc_ascii));   TEST_EQUAL(s1, u8"(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1, esc_nostdc));  TEST_EQUAL(s1, u8"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1, esc_pcre));    TEST_EQUAL(s1, u8"(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1, esc_punct));   TEST_EQUAL(s1, u8"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)");

        s16 = u"";                                           TRY(str_escape_in(s16));              TEST_EQUAL(s16, u"");
        s16 = u"Hello world";                                TRY(str_escape_in(s16));              TEST_EQUAL(s16, u"Hello world");
        s16 = u"Hello world";                                TRY(str_escape_in(s16, esc_ascii));   TEST_EQUAL(s16, u"Hello world");
        s16 = u"Hello world";                                TRY(str_escape_in(s16, esc_nostdc));  TEST_EQUAL(s16, u"Hello world");
        s16 = u"Hello world";                                TRY(str_escape_in(s16, esc_pcre));    TEST_EQUAL(s16, u"Hello world");
        s16 = u"Hello world";                                TRY(str_escape_in(s16, esc_punct));   TEST_EQUAL(s16, u"Hello world");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s16));              TEST_EQUAL(s16, u"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s16, esc_ascii));   TEST_EQUAL(s16, u"(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s16, esc_nostdc));  TEST_EQUAL(s16, u"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s16, esc_pcre));    TEST_EQUAL(s16, u"(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s16, esc_punct));   TEST_EQUAL(s16, u"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)");

        s32 = U"";                                           TRY(str_escape_in(s32));              TEST_EQUAL(s32, U"");
        s32 = U"Hello world";                                TRY(str_escape_in(s32));              TEST_EQUAL(s32, U"Hello world");
        s32 = U"Hello world";                                TRY(str_escape_in(s32, esc_ascii));   TEST_EQUAL(s32, U"Hello world");
        s32 = U"Hello world";                                TRY(str_escape_in(s32, esc_nostdc));  TEST_EQUAL(s32, U"Hello world");
        s32 = U"Hello world";                                TRY(str_escape_in(s32, esc_pcre));    TEST_EQUAL(s32, U"Hello world");
        s32 = U"Hello world";                                TRY(str_escape_in(s32, esc_punct));   TEST_EQUAL(s32, U"Hello world");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s32));              TEST_EQUAL(s32, U"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s32, esc_ascii));   TEST_EQUAL(s32, U"(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s32, esc_nostdc));  TEST_EQUAL(s32, U"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s32, esc_pcre));    TEST_EQUAL(s32, U"(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s32, esc_punct));   TEST_EQUAL(s32, U"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)");

        s1 = "";                                                                    TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, "");
        s1 = "Hello world";                                                         TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, "Hello world");
        s1 = u8"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")";                       TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s1 = u8"(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")";   TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s1 = u8"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")";                     TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s1 = u8"(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")";  TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s1 = u8"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)";               TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s1 = "";                                                                    TRY(str_unescape_in(s1));    TEST_EQUAL(s1, "");
        s1 = "Hello world";                                                         TRY(str_unescape_in(s1));    TEST_EQUAL(s1, "Hello world");
        s1 = u8"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")";                       TRY(str_unescape_in(s1));    TEST_EQUAL(s1, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s1 = u8"(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")";   TRY(str_unescape_in(s1));    TEST_EQUAL(s1, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s1 = u8"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")";                     TRY(str_unescape_in(s1));    TEST_EQUAL(s1, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s1 = u8"(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")";  TRY(str_unescape_in(s1));    TEST_EQUAL(s1, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s1 = u8"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)";               TRY(str_unescape_in(s1));    TEST_EQUAL(s1, u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s16 = u"";                                                                  TRY(str_unescape_in(s16));   TEST_EQUAL(s16, u"");
        s16 = u"Hello world";                                                       TRY(str_unescape_in(s16));   TEST_EQUAL(s16, u"Hello world");
        s16 = u"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")";                       TRY(str_unescape_in(s16));   TEST_EQUAL(s16, u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s16 = u"(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")";   TRY(str_unescape_in(s16));   TEST_EQUAL(s16, u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s16 = u"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")";                     TRY(str_unescape_in(s16));   TEST_EQUAL(s16, u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s16 = u"(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")";  TRY(str_unescape_in(s16));   TEST_EQUAL(s16, u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s16 = u"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)";               TRY(str_unescape_in(s16));   TEST_EQUAL(s16, u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s32 = U"";                                                                  TRY(str_unescape_in(s32));   TEST_EQUAL(s32, U"");
        s32 = U"Hello world";                                                       TRY(str_unescape_in(s32));   TEST_EQUAL(s32, U"Hello world");
        s32 = U"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")";                       TRY(str_unescape_in(s32));   TEST_EQUAL(s32, U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s32 = U"(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")";   TRY(str_unescape_in(s32));   TEST_EQUAL(s32, U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s32 = U"(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")";                     TRY(str_unescape_in(s32));   TEST_EQUAL(s32, U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s32 = U"(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")";  TRY(str_unescape_in(s32));   TEST_EQUAL(s32, U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
        s32 = U"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)";               TRY(str_unescape_in(s32));   TEST_EQUAL(s32, U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");

    }

    void check_quote() {

        u8string s1, s2;
        u16string s16;
        u32string s32;

        s1 = "";                                             TRY(s2 = str_quote(s1));              TEST_EQUAL(s2, "\"\"");
        s1 = "Hello world";                                  TRY(s2 = str_quote(s1));              TEST_EQUAL(s2, "\"Hello world\"");
        s1 = "Hello world";                                  TRY(s2 = str_quote(s1, esc_ascii));   TEST_EQUAL(s2, "\"Hello world\"");
        s1 = "Hello world";                                  TRY(s2 = str_quote(s1, esc_nostdc));  TEST_EQUAL(s2, "\"Hello world\"");
        s1 = "Hello world";                                  TRY(s2 = str_quote(s1, esc_pcre));    TEST_EQUAL(s2, "\"Hello world\"");
        s1 = "Hello world";                                  TRY(s2 = str_quote(s1, esc_punct));   TEST_EQUAL(s2, "\"Hello world\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1));              TEST_EQUAL(s2, u8"\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\")\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, esc_apos));    TEST_EQUAL(s2, u8"'(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")'");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, esc_ascii));   TEST_EQUAL(s2, u8"\"(\\\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\\\")\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, esc_nostdc));  TEST_EQUAL(s2, u8"\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\\\")\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, esc_pcre));    TEST_EQUAL(s2, u8"\"(\\\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\\\")\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, esc_punct));   TEST_EQUAL(s2, u8"\"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)\"");

        s1 = "";                                             TRY(str_quote_in(s1));              TEST_EQUAL(s1, "\"\"");
        s1 = "Hello world";                                  TRY(str_quote_in(s1));              TEST_EQUAL(s1, "\"Hello world\"");
        s1 = "Hello world";                                  TRY(str_quote_in(s1, esc_ascii));   TEST_EQUAL(s1, "\"Hello world\"");
        s1 = "Hello world";                                  TRY(str_quote_in(s1, esc_nostdc));  TEST_EQUAL(s1, "\"Hello world\"");
        s1 = "Hello world";                                  TRY(str_quote_in(s1, esc_pcre));    TEST_EQUAL(s1, "\"Hello world\"");
        s1 = "Hello world";                                  TRY(str_quote_in(s1, esc_punct));   TEST_EQUAL(s1, "\"Hello world\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1));              TEST_EQUAL(s1, u8"\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\")\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, esc_apos));    TEST_EQUAL(s1, u8"'(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")'");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, esc_ascii));   TEST_EQUAL(s1, u8"\"(\\\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\\\")\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, esc_nostdc));  TEST_EQUAL(s1, u8"\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\\\")\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, esc_pcre));    TEST_EQUAL(s1, u8"\"(\\\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\\\")\"");
        s1 = u8"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, esc_punct));   TEST_EQUAL(s1, u8"\"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)\"");

        s16 = u"";                                           TRY(str_quote_in(s16));              TEST_EQUAL(s16, u"\"\"");
        s16 = u"Hello world";                                TRY(str_quote_in(s16));              TEST_EQUAL(s16, u"\"Hello world\"");
        s16 = u"Hello world";                                TRY(str_quote_in(s16, esc_ascii));   TEST_EQUAL(s16, u"\"Hello world\"");
        s16 = u"Hello world";                                TRY(str_quote_in(s16, esc_nostdc));  TEST_EQUAL(s16, u"\"Hello world\"");
        s16 = u"Hello world";                                TRY(str_quote_in(s16, esc_pcre));    TEST_EQUAL(s16, u"\"Hello world\"");
        s16 = u"Hello world";                                TRY(str_quote_in(s16, esc_punct));   TEST_EQUAL(s16, u"\"Hello world\"");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s16));              TEST_EQUAL(s16, u"\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\")\"");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s16, esc_apos));    TEST_EQUAL(s16, u"'(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")'");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s16, esc_ascii));   TEST_EQUAL(s16, u"\"(\\\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\\\")\"");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s16, esc_nostdc));  TEST_EQUAL(s16, u"\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\\\")\"");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s16, esc_pcre));    TEST_EQUAL(s16, u"\"(\\\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\\\")\"");
        s16 = u"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s16, esc_punct));   TEST_EQUAL(s16, u"\"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)\"");

        s32 = U"";                                           TRY(str_quote_in(s32));              TEST_EQUAL(s32, U"\"\"");
        s32 = U"Hello world";                                TRY(str_quote_in(s32));              TEST_EQUAL(s32, U"\"Hello world\"");
        s32 = U"Hello world";                                TRY(str_quote_in(s32, esc_ascii));   TEST_EQUAL(s32, U"\"Hello world\"");
        s32 = U"Hello world";                                TRY(str_quote_in(s32, esc_nostdc));  TEST_EQUAL(s32, U"\"Hello world\"");
        s32 = U"Hello world";                                TRY(str_quote_in(s32, esc_pcre));    TEST_EQUAL(s32, U"\"Hello world\"");
        s32 = U"Hello world";                                TRY(str_quote_in(s32, esc_punct));   TEST_EQUAL(s32, U"\"Hello world\"");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s32));              TEST_EQUAL(s32, U"\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\")\"");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s32, esc_apos));    TEST_EQUAL(s32, U"'(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")'");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s32, esc_ascii));   TEST_EQUAL(s32, U"\"(\\\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\\\")\"");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s32, esc_nostdc));  TEST_EQUAL(s32, U"\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\\\")\"");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s32, esc_pcre));    TEST_EQUAL(s32, U"\"(\\\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\\\")\"");
        s32 = U"(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s32, esc_punct));   TEST_EQUAL(s32, U"\"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)\"");

        // TODO

    }

}

TEST_MODULE(unicorn, string_escape) {

    check_encode_uri();
    check_escape();
    check_quote();

}
