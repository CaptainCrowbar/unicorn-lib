#include "unicorn/string.hpp"
#include "unicorn/unit-test.hpp"

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_escape_encode_uri() {

    Ustring s1, s2;

    s1 = "";                         TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "");
    s1 = "Hello world";              TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "Hello%20world");
    s1 = "http://www.example.com/";  TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
    s1 = "\"%<>\\^`{|}";             TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
    s1 = "!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
    s1 = "-._~";                     TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "-._~");
    s1 = "αβγδε";                    TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

    s1 = "";                         TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "");
    s1 = "Hello world";              TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "Hello%20world");
    s1 = "http://www.example.com/";  TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "http%3a%2f%2fwww.example.com%2f");
    s1 = "\"%<>\\^`{|}";             TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
    s1 = "!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
    s1 = "-._~";                     TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "-._~");
    s1 = "αβγδε";                    TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

    s1 = "";                         TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "");
    s1 = "Hello world";              TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "Hello%20world");
    s1 = "http://www.example.com/";  TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "http://www.example.com/");
    s1 = "\"%<>\\^`{|}";             TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
    s1 = "!#$&'()*+,/:;=?@[]";       TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "!#$&'()*+,/:;=?@[]");
    s1 = "-._~";                     TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "-._~");
    s1 = "αβγδε";                    TRY(str_encode_uri_in(s1));  TEST_EQUAL(s1, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

    s1 = "";                         TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "");
    s1 = "Hello world";              TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "Hello%20world");
    s1 = "http://www.example.com/";  TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "http%3a%2f%2fwww.example.com%2f");
    s1 = "\"%<>\\^`{|}";             TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
    s1 = "!#$&'()*+,/:;=?@[]";       TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
    s1 = "-._~";                     TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "-._~");
    s1 = "αβγδε";                    TRY(str_encode_uri_component_in(s1));  TEST_EQUAL(s1, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

    s1 = "";                                                        TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "");
    s1 = "Hello%20world";                                           TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "Hello world");
    s1 = "http://www.example.com/";                                 TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
    s1 = "http%3a%2f%2fwww.example.com%2f";                         TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
    s1 = "%22%25%3c%3e%5c%5e%60%7b%7c%7d";                          TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "\"%<>\\^`{|}");
    s1 = "!#$&'()*+,/:;=?@[]";                                      TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
    s1 = "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d";  TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
    s1 = "-._~";                                                    TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "-._~");
    s1 = "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5";                          TRY(s2 = str_unencode_uri(s1));  TEST_EQUAL(s2, "αβγδε");

    s1 = "";                                                        TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "");
    s1 = "Hello%20world";                                           TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "Hello world");
    s1 = "http://www.example.com/";                                 TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "http://www.example.com/");
    s1 = "http%3a%2f%2fwww.example.com%2f";                         TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "http://www.example.com/");
    s1 = "%22%25%3c%3e%5c%5e%60%7b%7c%7d";                          TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "\"%<>\\^`{|}");
    s1 = "!#$&'()*+,/:;=?@[]";                                      TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "!#$&'()*+,/:;=?@[]");
    s1 = "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d";  TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "!#$&'()*+,/:;=?@[]");
    s1 = "-._~";                                                    TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "-._~");
    s1 = "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5";                          TRY(str_unencode_uri_in(s1));  TEST_EQUAL(s1, "αβγδε");

}

void test_unicorn_string_escape_backslash() {

    Ustring s1, s2;

    s1 = "";                                           TRY(s2 = str_escape(s1));                  TEST_EQUAL(s2, "");
    s1 = "Hello world";                                TRY(s2 = str_escape(s1));                  TEST_EQUAL(s2, "Hello world");
    s1 = "Hello world";                                TRY(s2 = str_escape(s1, Escape::ascii));   TEST_EQUAL(s2, "Hello world");
    s1 = "Hello world";                                TRY(s2 = str_escape(s1, Escape::nostdc));  TEST_EQUAL(s2, "Hello world");
    s1 = "Hello world";                                TRY(s2 = str_escape(s1, Escape::pcre));    TEST_EQUAL(s2, "Hello world");
    s1 = "Hello world";                                TRY(s2 = str_escape(s1, Escape::punct));   TEST_EQUAL(s2, "Hello world");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1));                  TEST_EQUAL(s2, "(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1, Escape::ascii));   TEST_EQUAL(s2, "(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1, Escape::nostdc));  TEST_EQUAL(s2, "(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1, Escape::pcre));    TEST_EQUAL(s2, "(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_escape(s1, Escape::punct));   TEST_EQUAL(s2, "\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)");

    s1 = "";                                           TRY(str_escape_in(s1));                  TEST_EQUAL(s1, "");
    s1 = "Hello world";                                TRY(str_escape_in(s1));                  TEST_EQUAL(s1, "Hello world");
    s1 = "Hello world";                                TRY(str_escape_in(s1, Escape::ascii));   TEST_EQUAL(s1, "Hello world");
    s1 = "Hello world";                                TRY(str_escape_in(s1, Escape::nostdc));  TEST_EQUAL(s1, "Hello world");
    s1 = "Hello world";                                TRY(str_escape_in(s1, Escape::pcre));    TEST_EQUAL(s1, "Hello world");
    s1 = "Hello world";                                TRY(str_escape_in(s1, Escape::punct));   TEST_EQUAL(s1, "Hello world");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1));                  TEST_EQUAL(s1, "(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1, Escape::ascii));   TEST_EQUAL(s1, "(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1, Escape::nostdc));  TEST_EQUAL(s1, "(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1, Escape::pcre));    TEST_EQUAL(s1, "(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_escape_in(s1, Escape::punct));   TEST_EQUAL(s1, "\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)");

    s1 = "";                                                                  TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, "");
    s1 = "Hello world";                                                       TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, "Hello world");
    s1 = "(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")";                       TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")";   TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")";                     TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")";  TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)";               TRY(s2 = str_unescape(s1));  TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");

    s1 = "";                                                                  TRY(str_unescape_in(s1));  TEST_EQUAL(s1, "");
    s1 = "Hello world";                                                       TRY(str_unescape_in(s1));  TEST_EQUAL(s1, "Hello world");
    s1 = "(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")";                       TRY(str_unescape_in(s1));  TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "(\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\")";   TRY(str_unescape_in(s1));  TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\")";                     TRY(str_unescape_in(s1));  TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "(\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\")";  TRY(str_unescape_in(s1));  TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)";               TRY(str_unescape_in(s1));  TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");

}

void test_unicorn_string_escape_quote() {

    Ustring s1, s2;

    s1 = "";                                           TRY(s2 = str_quote(s1));                  TEST_EQUAL(s2, "\"\"");
    s1 = "Hello world";                                TRY(s2 = str_quote(s1));                  TEST_EQUAL(s2, "\"Hello world\"");
    s1 = "Hello world";                                TRY(s2 = str_quote(s1, Escape::ascii));   TEST_EQUAL(s2, "\"Hello world\"");
    s1 = "Hello world";                                TRY(s2 = str_quote(s1, Escape::nostdc));  TEST_EQUAL(s2, "\"Hello world\"");
    s1 = "Hello world";                                TRY(s2 = str_quote(s1, Escape::pcre));    TEST_EQUAL(s2, "\"Hello world\"");
    s1 = "Hello world";                                TRY(s2 = str_quote(s1, Escape::punct));   TEST_EQUAL(s2, "\"Hello world\"");
    s1 = "Hello world";                                TRY(s2 = str_quote(s1, 0, U'\''));        TEST_EQUAL(s2, "\'Hello world\'");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1));                  TEST_EQUAL(s2, "\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\")\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, Escape::ascii));   TEST_EQUAL(s2, "\"(\\\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\\\")\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, Escape::nostdc));  TEST_EQUAL(s2, "\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\\\")\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, Escape::pcre));    TEST_EQUAL(s2, "\"(\\\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\\\")\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, Escape::punct));   TEST_EQUAL(s2, "\"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(s2 = str_quote(s1, 0, U'\''));        TEST_EQUAL(s2, "'(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")'");

    s1 = "";                                           TRY(str_quote_in(s1));                  TEST_EQUAL(s1, "\"\"");
    s1 = "Hello world";                                TRY(str_quote_in(s1));                  TEST_EQUAL(s1, "\"Hello world\"");
    s1 = "Hello world";                                TRY(str_quote_in(s1, Escape::ascii));   TEST_EQUAL(s1, "\"Hello world\"");
    s1 = "Hello world";                                TRY(str_quote_in(s1, Escape::nostdc));  TEST_EQUAL(s1, "\"Hello world\"");
    s1 = "Hello world";                                TRY(str_quote_in(s1, Escape::pcre));    TEST_EQUAL(s1, "\"Hello world\"");
    s1 = "Hello world";                                TRY(str_quote_in(s1, Escape::punct));   TEST_EQUAL(s1, "\"Hello world\"");
    s1 = "Hello world";                                TRY(str_quote_in(s1, 0, U'\''));        TEST_EQUAL(s1, "\'Hello world\'");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1));                  TEST_EQUAL(s1, "\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\")\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, Escape::ascii));   TEST_EQUAL(s1, "\"(\\\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\\\")\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, Escape::nostdc));  TEST_EQUAL(s1, "\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\\\")\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, Escape::pcre));    TEST_EQUAL(s1, "\"(\\\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\\\")\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, Escape::punct));   TEST_EQUAL(s1, "\"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)\"");
    s1 = "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")";  TRY(str_quote_in(s1, 0, U'\''));        TEST_EQUAL(s1, "'(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")'");

    s1 = "\"\"";                                                                      TRY(s2 = str_unquote(s1));         TEST_EQUAL(s2, "");
    s1 = "\"Hello world\"";                                                           TRY(s2 = str_unquote(s1));         TEST_EQUAL(s2, "Hello world");
    s1 = "\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\")\"";                       TRY(s2 = str_unquote(s1));         TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"(\\\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\\\")\"";   TRY(s2 = str_unquote(s1));         TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\\\")\"";                     TRY(s2 = str_unquote(s1));         TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"(\\\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\\\")\"";  TRY(s2 = str_unquote(s1));         TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)\"";                   TRY(s2 = str_unquote(s1));         TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"Hello\" \"world\"";                                                       TRY(s2 = str_unquote(s1));         TEST_EQUAL(s2, "Hello world");
    s1 = "αβγδε \"ζηθικ\" λμνξο \"πρστυ\" φχψω";                                      TRY(s2 = str_unquote(s1));         TEST_EQUAL(s2, "αβγδε ζηθικ λμνξο πρστυ φχψω");
    s1 = "\'Hello world\'";                                                           TRY(s2 = str_unquote(s1, U'\''));  TEST_EQUAL(s2, "Hello world");
    s1 = "\'Hello\' \'world\'";                                                       TRY(s2 = str_unquote(s1, U'\''));  TEST_EQUAL(s2, "Hello world");
    s1 = "'(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")'";                             TRY(s2 = str_unquote(s1, U'\''));  TEST_EQUAL(s2, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "αβγδε \'ζηθικ\' λμνξο \'πρστυ\' φχψω";                                      TRY(s2 = str_unquote(s1, U'\''));  TEST_EQUAL(s2, "αβγδε ζηθικ λμνξο πρστυ φχψω");

    s1 = "\"\"";                                                                      TRY(str_unquote_in(s1));         TEST_EQUAL(s1, "");
    s1 = "\"Hello world\"";                                                           TRY(str_unquote_in(s1));         TEST_EQUAL(s1, "Hello world");
    s1 = "\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\")\"";                       TRY(str_unquote_in(s1));         TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"(\\\"\\x01\\u0080\\u00a7\\u00b6 \\\\ \\u20acuro \\U00012000\\n\\\")\"";   TRY(str_unquote_in(s1));         TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\x0a\\\")\"";                     TRY(str_unquote_in(s1));         TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"(\\\"\\x01\\x{80}\\x{a7}\\x{b6} \\\\ \\x{20ac}uro \\x{12000}\\n\\\")\"";  TRY(str_unquote_in(s1));         TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"\\(\\\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\\\"\\)\"";                   TRY(str_unquote_in(s1));         TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "\"Hello\" \"world\"";                                                       TRY(str_unquote_in(s1));         TEST_EQUAL(s1, "Hello world");
    s1 = "αβγδε \"ζηθικ\" λμνξο \"πρστυ\" φχψω";                                      TRY(str_unquote_in(s1));         TEST_EQUAL(s1, "αβγδε ζηθικ λμνξο πρστυ φχψω");
    s1 = "\'Hello world\'";                                                           TRY(str_unquote_in(s1, U'\''));  TEST_EQUAL(s1, "Hello world");
    s1 = "\'Hello\' \'world\'";                                                       TRY(str_unquote_in(s1, U'\''));  TEST_EQUAL(s1, "Hello world");
    s1 = "'(\"\\x01\u0080§¶ \\\\ €uro \U00012000\\n\")'";                             TRY(str_unquote_in(s1, U'\''));  TEST_EQUAL(s1, "(\"\u0001\u0080§¶ \\ €uro \U00012000\n\")");
    s1 = "αβγδε \'ζηθικ\' λμνξο \'πρστυ\' φχψω";                                      TRY(str_unquote_in(s1, U'\''));  TEST_EQUAL(s1, "αβγδε ζηθικ λμνξο πρστυ φχψω");

}
