#include "crow/core.hpp"
#include "crow/unit-test.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include <string>
#include <vector>

using namespace std::literals;
using namespace Crow;
using namespace Unicorn;

namespace {

    void check_split() {

        std::vector<u8string> v8;
        std::vector<u16string> v16;
        std::vector<u32string> v32;

        TRY(str_split(""s, overwrite(v8)));                      TEST_EQUAL(v8.size(), 0);  TEST_EQUAL(str_join(v8, "/"), "");
        TRY(str_split("Hello"s, overwrite(v8)));                 TEST_EQUAL(v8.size(), 1);  TEST_EQUAL(str_join(v8, "/"), "Hello");
        TRY(str_split("Hello world"s, overwrite(v8)));           TEST_EQUAL(v8.size(), 2);  TEST_EQUAL(str_join(v8, "/"), "Hello/world");
        TRY(str_split("\t Hello \t world \t"s, overwrite(v8)));  TEST_EQUAL(v8.size(), 2);  TEST_EQUAL(str_join(v8, "/"), "Hello/world");

        TRY(str_split(u""s, overwrite(v16)));                      TEST_EQUAL(v16.size(), 0);  TEST_EQUAL(str_join(v16, u"/"), u"");
        TRY(str_split(u"Hello"s, overwrite(v16)));                 TEST_EQUAL(v16.size(), 1);  TEST_EQUAL(str_join(v16, u"/"), u"Hello");
        TRY(str_split(u"Hello world"s, overwrite(v16)));           TEST_EQUAL(v16.size(), 2);  TEST_EQUAL(str_join(v16, u"/"), u"Hello/world");
        TRY(str_split(u"\t Hello \t world \t"s, overwrite(v16)));  TEST_EQUAL(v16.size(), 2);  TEST_EQUAL(str_join(v16, u"/"), u"Hello/world");

        TRY(str_split(U""s, overwrite(v32)));                      TEST_EQUAL(v32.size(), 0);  TEST_EQUAL(str_join(v32, U"/"), U"");
        TRY(str_split(U"Hello"s, overwrite(v32)));                 TEST_EQUAL(v32.size(), 1);  TEST_EQUAL(str_join(v32, U"/"), U"Hello");
        TRY(str_split(U"Hello world"s, overwrite(v32)));           TEST_EQUAL(v32.size(), 2);  TEST_EQUAL(str_join(v32, U"/"), U"Hello/world");
        TRY(str_split(U"\t Hello \t world \t"s, overwrite(v32)));  TEST_EQUAL(v32.size(), 2);  TEST_EQUAL(str_join(v32, U"/"), U"Hello/world");

        TRY(str_split_at(""s, overwrite(v8), "<>"));                        TEST_EQUAL(v8.size(), 1);  TEST_EQUAL(str_join(v8, "/"), "");
        TRY(str_split_at("<>"s, overwrite(v8), "<>"));                      TEST_EQUAL(v8.size(), 2);  TEST_EQUAL(str_join(v8, "/"), "/");
        TRY(str_split_at("<><>"s, overwrite(v8), "<>"));                    TEST_EQUAL(v8.size(), 3);  TEST_EQUAL(str_join(v8, "/"), "//");
        TRY(str_split_at("Hello"s, overwrite(v8), "<>"));                   TEST_EQUAL(v8.size(), 1);  TEST_EQUAL(str_join(v8, "/"), "Hello");
        TRY(str_split_at("<>Hello<>"s, overwrite(v8), "<>"));               TEST_EQUAL(v8.size(), 3);  TEST_EQUAL(str_join(v8, "/"), "/Hello/");
        TRY(str_split_at("<><>Hello<><>"s, overwrite(v8), "<>"));           TEST_EQUAL(v8.size(), 5);  TEST_EQUAL(str_join(v8, "/"), "//Hello//");
        TRY(str_split_at("Hello<>world"s, overwrite(v8), "<>"));            TEST_EQUAL(v8.size(), 2);  TEST_EQUAL(str_join(v8, "/"), "Hello/world");
        TRY(str_split_at("<>Hello<>world<>"s, overwrite(v8), "<>"));        TEST_EQUAL(v8.size(), 4);  TEST_EQUAL(str_join(v8, "/"), "/Hello/world/");
        TRY(str_split_at("<><>Hello<><>world<><>"s, overwrite(v8), "<>"));  TEST_EQUAL(v8.size(), 7);  TEST_EQUAL(str_join(v8, "/"), "//Hello//world//");

        TRY(str_split_at(u""s, overwrite(v16), u"<>"));                        TEST_EQUAL(v16.size(), 1);  TEST_EQUAL(str_join(v16, u"/"), u"");
        TRY(str_split_at(u"<>"s, overwrite(v16), u"<>"));                      TEST_EQUAL(v16.size(), 2);  TEST_EQUAL(str_join(v16, u"/"), u"/");
        TRY(str_split_at(u"<><>"s, overwrite(v16), u"<>"));                    TEST_EQUAL(v16.size(), 3);  TEST_EQUAL(str_join(v16, u"/"), u"//");
        TRY(str_split_at(u"Hello"s, overwrite(v16), u"<>"));                   TEST_EQUAL(v16.size(), 1);  TEST_EQUAL(str_join(v16, u"/"), u"Hello");
        TRY(str_split_at(u"<>Hello<>"s, overwrite(v16), u"<>"));               TEST_EQUAL(v16.size(), 3);  TEST_EQUAL(str_join(v16, u"/"), u"/Hello/");
        TRY(str_split_at(u"<><>Hello<><>"s, overwrite(v16), u"<>"));           TEST_EQUAL(v16.size(), 5);  TEST_EQUAL(str_join(v16, u"/"), u"//Hello//");
        TRY(str_split_at(u"Hello<>world"s, overwrite(v16), u"<>"));            TEST_EQUAL(v16.size(), 2);  TEST_EQUAL(str_join(v16, u"/"), u"Hello/world");
        TRY(str_split_at(u"<>Hello<>world<>"s, overwrite(v16), u"<>"));        TEST_EQUAL(v16.size(), 4);  TEST_EQUAL(str_join(v16, u"/"), u"/Hello/world/");
        TRY(str_split_at(u"<><>Hello<><>world<><>"s, overwrite(v16), u"<>"));  TEST_EQUAL(v16.size(), 7);  TEST_EQUAL(str_join(v16, u"/"), u"//Hello//world//");

        TRY(str_split_at(U""s, overwrite(v32), U"<>"));                        TEST_EQUAL(v32.size(), 1);  TEST_EQUAL(str_join(v32, U"/"), U"");
        TRY(str_split_at(U"<>"s, overwrite(v32), U"<>"));                      TEST_EQUAL(v32.size(), 2);  TEST_EQUAL(str_join(v32, U"/"), U"/");
        TRY(str_split_at(U"<><>"s, overwrite(v32), U"<>"));                    TEST_EQUAL(v32.size(), 3);  TEST_EQUAL(str_join(v32, U"/"), U"//");
        TRY(str_split_at(U"Hello"s, overwrite(v32), U"<>"));                   TEST_EQUAL(v32.size(), 1);  TEST_EQUAL(str_join(v32, U"/"), U"Hello");
        TRY(str_split_at(U"<>Hello<>"s, overwrite(v32), U"<>"));               TEST_EQUAL(v32.size(), 3);  TEST_EQUAL(str_join(v32, U"/"), U"/Hello/");
        TRY(str_split_at(U"<><>Hello<><>"s, overwrite(v32), U"<>"));           TEST_EQUAL(v32.size(), 5);  TEST_EQUAL(str_join(v32, U"/"), U"//Hello//");
        TRY(str_split_at(U"Hello<>world"s, overwrite(v32), U"<>"));            TEST_EQUAL(v32.size(), 2);  TEST_EQUAL(str_join(v32, U"/"), U"Hello/world");
        TRY(str_split_at(U"<>Hello<>world<>"s, overwrite(v32), U"<>"));        TEST_EQUAL(v32.size(), 4);  TEST_EQUAL(str_join(v32, U"/"), U"/Hello/world/");
        TRY(str_split_at(U"<><>Hello<><>world<><>"s, overwrite(v32), U"<>"));  TEST_EQUAL(v32.size(), 7);  TEST_EQUAL(str_join(v32, U"/"), U"//Hello//world//");

        TRY(str_split_by("**Hello**world**"s, overwrite(v8), "*"));        TEST_EQUAL(v8.size(), 2);  TEST_EQUAL(str_join(v8, "/"), "Hello/world");
        TRY(str_split_by("**Hello**world**"s, overwrite(v8), "*"s));       TEST_EQUAL(v8.size(), 2);  TEST_EQUAL(str_join(v8, "/"), "Hello/world");
        TRY(str_split_by("*****"s, overwrite(v8), "@*"));                  TEST_EQUAL(v8.size(), 0);  TEST_EQUAL(str_join(v8, "/"), "");
        TRY(str_split_by("*****"s, overwrite(v8), "@*"s));                 TEST_EQUAL(v8.size(), 0);  TEST_EQUAL(str_join(v8, "/"), "");
        TRY(str_split_by("“”,“€uro”,“∈lement”"s, overwrite(v8), "“”,"));   TEST_EQUAL(v8.size(), 2);  TEST_EQUAL(str_join(v8, "/"), "€uro/∈lement");
        TRY(str_split_by("“”,“€uro”,“∈lement”"s, overwrite(v8), "“”,"s));  TEST_EQUAL(v8.size(), 2);  TEST_EQUAL(str_join(v8, "/"), "€uro/∈lement");

        TRY(str_split_by(u"**Hello**world**"s, overwrite(v16), u"*"));        TEST_EQUAL(v16.size(), 2);  TEST_EQUAL(str_join(v16, u"/"), u"Hello/world");
        TRY(str_split_by(u"**Hello**world**"s, overwrite(v16), u"*"s));       TEST_EQUAL(v16.size(), 2);  TEST_EQUAL(str_join(v16, u"/"), u"Hello/world");
        TRY(str_split_by(u"*****"s, overwrite(v16), u"@*"));                  TEST_EQUAL(v16.size(), 0);  TEST_EQUAL(str_join(v16, u"/"), u"");
        TRY(str_split_by(u"*****"s, overwrite(v16), u"@*"s));                 TEST_EQUAL(v16.size(), 0);  TEST_EQUAL(str_join(v16, u"/"), u"");
        TRY(str_split_by(u"“”,“€uro”,“∈lement”"s, overwrite(v16), u"“”,"));   TEST_EQUAL(v16.size(), 2);  TEST_EQUAL(str_join(v16, u"/"), u"€uro/∈lement");
        TRY(str_split_by(u"“”,“€uro”,“∈lement”"s, overwrite(v16), u"“”,"s));  TEST_EQUAL(v16.size(), 2);  TEST_EQUAL(str_join(v16, u"/"), u"€uro/∈lement");

        TRY(str_split_by(U"**Hello**world**"s, overwrite(v32), U"*"));        TEST_EQUAL(v32.size(), 2);  TEST_EQUAL(str_join(v32, U"/"), U"Hello/world");
        TRY(str_split_by(U"**Hello**world**"s, overwrite(v32), U"*"s));       TEST_EQUAL(v32.size(), 2);  TEST_EQUAL(str_join(v32, U"/"), U"Hello/world");
        TRY(str_split_by(U"*****"s, overwrite(v32), U"@*"));                  TEST_EQUAL(v32.size(), 0);  TEST_EQUAL(str_join(v32, U"/"), U"");
        TRY(str_split_by(U"*****"s, overwrite(v32), U"@*"s));                 TEST_EQUAL(v32.size(), 0);  TEST_EQUAL(str_join(v32, U"/"), U"");
        TRY(str_split_by(U"“”,“€uro”,“∈lement”"s, overwrite(v32), U"“”,"));   TEST_EQUAL(v32.size(), 2);  TEST_EQUAL(str_join(v32, U"/"), U"€uro/∈lement");
        TRY(str_split_by(U"“”,“€uro”,“∈lement”"s, overwrite(v32), U"“”,"s));  TEST_EQUAL(v32.size(), 2);  TEST_EQUAL(str_join(v32, U"/"), U"€uro/∈lement");

    }

    void check_substring() {

        // UTF-32    UTF-16     UTF-8
        // 0000004d  004d       4d
        // 00000430  0430       d0 b0
        // 00004e8c  4e8c       e4 ba 8c
        // 00010302  d800 df02  f0 90 8c 82
        // 0010fffd  dbff dffd  f4 8f bf bd

        const u8string utf8_example {"\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};
        const u16string utf16_example {0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
        const u32string utf32_example {0x430,0x4e8c,0x10302,0x10fffd};

        TEST_EQUAL(str_substring(""s, 0), "");
        TEST_EQUAL(str_substring(""s, 5), "");
        TEST_EQUAL(str_substring(""s, 0, 0), "");
        TEST_EQUAL(str_substring(""s, 0, 5), "");
        TEST_EQUAL(str_substring(""s, 5, 0), "");
        TEST_EQUAL(str_substring(""s, 5, 5), "");
        TEST_EQUAL(str_substring("Hello world"s, 0), "Hello world");
        TEST_EQUAL(str_substring("Hello world"s, 6), "world");
        TEST_EQUAL(str_substring("Hello world"s, 20), "");
        TEST_EQUAL(str_substring("Hello world"s, 0, 0), "");
        TEST_EQUAL(str_substring("Hello world"s, 0, 5), "Hello");
        TEST_EQUAL(str_substring("Hello world"s, 0, 20), "Hello world");
        TEST_EQUAL(str_substring("Hello world"s, 6, 0), "");
        TEST_EQUAL(str_substring("Hello world"s, 6, 5), "world");
        TEST_EQUAL(str_substring("Hello world"s, 6, 20), "world");
        TEST_EQUAL(str_substring("Hello world"s, 20, 0), "");
        TEST_EQUAL(str_substring("Hello world"s, 20, 20), "");

        TEST_EQUAL(utf_substring(""s, 0), "");
        TEST_EQUAL(utf_substring(""s, 5), "");
        TEST_EQUAL(utf_substring(""s, 0, 0), "");
        TEST_EQUAL(utf_substring(""s, 0, 5), "");
        TEST_EQUAL(utf_substring(""s, 5, 0), "");
        TEST_EQUAL(utf_substring(""s, 5, 5), "");
        TEST_EQUAL(utf_substring("Hello world"s, 0), "Hello world");
        TEST_EQUAL(utf_substring("Hello world"s, 6), "world");
        TEST_EQUAL(utf_substring("Hello world"s, 20), "");
        TEST_EQUAL(utf_substring("Hello world"s, 0, 0), "");
        TEST_EQUAL(utf_substring("Hello world"s, 0, 5), "Hello");
        TEST_EQUAL(utf_substring("Hello world"s, 0, 20), "Hello world");
        TEST_EQUAL(utf_substring("Hello world"s, 6, 0), "");
        TEST_EQUAL(utf_substring("Hello world"s, 6, 5), "world");
        TEST_EQUAL(utf_substring("Hello world"s, 6, 20), "world");
        TEST_EQUAL(utf_substring("Hello world"s, 20, 0), "");
        TEST_EQUAL(utf_substring("Hello world"s, 20, 20), "");
        TEST_EQUAL(utf_substring(utf8_example, 0), "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 1), "\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 2), "\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 3), "\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 4), "");
        TEST_EQUAL(utf_substring(utf8_example, 5), "");
        TEST_EQUAL(utf_substring(utf8_example, 0, 0), "");
        TEST_EQUAL(utf_substring(utf8_example, 0, 1), "\xd0\xb0");
        TEST_EQUAL(utf_substring(utf8_example, 0, 2), "\xd0\xb0\xe4\xba\x8c");
        TEST_EQUAL(utf_substring(utf8_example, 0, 3), "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82");
        TEST_EQUAL(utf_substring(utf8_example, 0, 4), "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 0, 5), "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 1, 0), "");
        TEST_EQUAL(utf_substring(utf8_example, 1, 1), "\xe4\xba\x8c");
        TEST_EQUAL(utf_substring(utf8_example, 1, 2), "\xe4\xba\x8c\xf0\x90\x8c\x82");
        TEST_EQUAL(utf_substring(utf8_example, 1, 3), "\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 1, 4), "\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 2, 0), "");
        TEST_EQUAL(utf_substring(utf8_example, 2, 1), "\xf0\x90\x8c\x82");
        TEST_EQUAL(utf_substring(utf8_example, 2, 2), "\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 2, 3), "\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 3, 0), "");
        TEST_EQUAL(utf_substring(utf8_example, 3, 1), "\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 3, 2), "\xf4\x8f\xbf\xbd");
        TEST_EQUAL(utf_substring(utf8_example, 4, 0), "");
        TEST_EQUAL(utf_substring(utf8_example, 4, 1), "");
        TEST_EQUAL(utf_substring(utf8_example, 5, 0), "");
        TEST_EQUAL(utf_substring(utf8_example, 5, 1), "");

        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 0, grapheme_units), u8"");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 1, grapheme_units), u8"é");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 2, grapheme_units), u8"éí");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 3, grapheme_units), u8"éíó");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 4, grapheme_units), u8"éíóú");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 5, grapheme_units), u8"éíóú");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, npos, grapheme_units), u8"éíóú");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 5, npos, grapheme_units), u8"");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 0, grapheme_units), u8"");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 1, grapheme_units), u8"e\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 2, grapheme_units), u8"e\u0301i\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 3, grapheme_units), u8"e\u0301i\u0301o\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 4, grapheme_units), u8"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 5, grapheme_units), u8"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, npos, grapheme_units), u8"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, npos, grapheme_units), u8"");

        TEST_EQUAL(utf_substring(u"áéíóú"s, 1, 0, grapheme_units), u"");
        TEST_EQUAL(utf_substring(u"áéíóú"s, 1, 1, grapheme_units), u"é");
        TEST_EQUAL(utf_substring(u"áéíóú"s, 1, 2, grapheme_units), u"éí");
        TEST_EQUAL(utf_substring(u"áéíóú"s, 1, 3, grapheme_units), u"éíó");
        TEST_EQUAL(utf_substring(u"áéíóú"s, 1, 4, grapheme_units), u"éíóú");
        TEST_EQUAL(utf_substring(u"áéíóú"s, 1, 5, grapheme_units), u"éíóú");
        TEST_EQUAL(utf_substring(u"áéíóú"s, 1, npos, grapheme_units), u"éíóú");
        TEST_EQUAL(utf_substring(u"áéíóú"s, 5, npos, grapheme_units), u"");
        TEST_EQUAL(utf_substring(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 0, grapheme_units), u"");
        TEST_EQUAL(utf_substring(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 1, grapheme_units), u"e\u0301");
        TEST_EQUAL(utf_substring(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 2, grapheme_units), u"e\u0301i\u0301");
        TEST_EQUAL(utf_substring(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 3, grapheme_units), u"e\u0301i\u0301o\u0301");
        TEST_EQUAL(utf_substring(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 4, grapheme_units), u"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 5, grapheme_units), u"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, npos, grapheme_units), u"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, npos, grapheme_units), u"");

        TEST_EQUAL(utf_substring(U"áéíóú"s, 1, 0, grapheme_units), U"");
        TEST_EQUAL(utf_substring(U"áéíóú"s, 1, 1, grapheme_units), U"é");
        TEST_EQUAL(utf_substring(U"áéíóú"s, 1, 2, grapheme_units), U"éí");
        TEST_EQUAL(utf_substring(U"áéíóú"s, 1, 3, grapheme_units), U"éíó");
        TEST_EQUAL(utf_substring(U"áéíóú"s, 1, 4, grapheme_units), U"éíóú");
        TEST_EQUAL(utf_substring(U"áéíóú"s, 1, 5, grapheme_units), U"éíóú");
        TEST_EQUAL(utf_substring(U"áéíóú"s, 1, npos, grapheme_units), U"éíóú");
        TEST_EQUAL(utf_substring(U"áéíóú"s, 5, npos, grapheme_units), U"");
        TEST_EQUAL(utf_substring(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 0, grapheme_units), U"");
        TEST_EQUAL(utf_substring(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 1, grapheme_units), U"e\u0301");
        TEST_EQUAL(utf_substring(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 2, grapheme_units), U"e\u0301i\u0301");
        TEST_EQUAL(utf_substring(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 3, grapheme_units), U"e\u0301i\u0301o\u0301");
        TEST_EQUAL(utf_substring(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 4, grapheme_units), U"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 5, grapheme_units), U"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, npos, grapheme_units), U"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, npos, grapheme_units), U"");

    }

    void check_translate() {

        u8string s8;
        u16string s16;
        u32string s32;

        TEST_EQUAL(str_translate(""s, "", ""), "");
        TEST_EQUAL(str_translate("Hello world"s, "", ""), "Hello world");
        TEST_EQUAL(str_translate("Hello world"s, "", "abcde"), "Hello world");
        TEST_EQUAL(str_translate("Hello world"s, "abcde", ""), "Hello world");
        TEST_EQUAL(str_translate("Hello world"s, "abcde", "12345"), "H5llo worl4");
        TEST_EQUAL(str_translate("Hello world"s, "abcde", "*"), "H*llo worl*");
        TEST_EQUAL(str_translate("€uro ∈lement"s, "€∈", "∇√"), "∇uro √lement");
        TEST_EQUAL(str_translate("€uro ∈lement"s, "€∈", "*"), "*uro *lement");
        TEST_EQUAL(str_translate("€uro ∈lement"s, "e", "∇√"), "€uro ∈l∇m∇nt");

        TEST_EQUAL(str_translate(u""s, u"", u""), u"");
        TEST_EQUAL(str_translate(u"Hello world"s, u"", u""), u"Hello world");
        TEST_EQUAL(str_translate(u"Hello world"s, u"", u"abcde"), u"Hello world");
        TEST_EQUAL(str_translate(u"Hello world"s, u"abcde", u""), u"Hello world");
        TEST_EQUAL(str_translate(u"Hello world"s, u"abcde", u"12345"), u"H5llo worl4");
        TEST_EQUAL(str_translate(u"Hello world"s, u"abcde", u"*"), u"H*llo worl*");
        TEST_EQUAL(str_translate(u"€uro ∈lement"s, u"€∈", u"∇√"), u"∇uro √lement");
        TEST_EQUAL(str_translate(u"€uro ∈lement"s, u"€∈", u"*"), u"*uro *lement");
        TEST_EQUAL(str_translate(u"€uro ∈lement"s, u"e", u"∇√"), u"€uro ∈l∇m∇nt");

        TEST_EQUAL(str_translate(U""s, U"", U""), U"");
        TEST_EQUAL(str_translate(U"Hello world"s, U"", U""), U"Hello world");
        TEST_EQUAL(str_translate(U"Hello world"s, U"", U"abcde"), U"Hello world");
        TEST_EQUAL(str_translate(U"Hello world"s, U"abcde", U""), U"Hello world");
        TEST_EQUAL(str_translate(U"Hello world"s, U"abcde", U"12345"), U"H5llo worl4");
        TEST_EQUAL(str_translate(U"Hello world"s, U"abcde", U"*"), U"H*llo worl*");
        TEST_EQUAL(str_translate(U"€uro ∈lement"s, U"€∈", U"∇√"), U"∇uro √lement");
        TEST_EQUAL(str_translate(U"€uro ∈lement"s, U"€∈", U"*"), U"*uro *lement");
        TEST_EQUAL(str_translate(U"€uro ∈lement"s, U"e", U"∇√"), U"€uro ∈l∇m∇nt");

        s8 = u8"";              TRY(str_translate_in(s8, u8"", u8""));            TEST_EQUAL(s8, u8"");
        s8 = u8"Hello world";   TRY(str_translate_in(s8, u8"", u8""));            TEST_EQUAL(s8, u8"Hello world");
        s8 = u8"Hello world";   TRY(str_translate_in(s8, u8"", u8"abcde"));       TEST_EQUAL(s8, u8"Hello world");
        s8 = u8"Hello world";   TRY(str_translate_in(s8, u8"abcde", u8""));       TEST_EQUAL(s8, u8"Hello world");
        s8 = u8"Hello world";   TRY(str_translate_in(s8, u8"abcde", u8"12345"));  TEST_EQUAL(s8, u8"H5llo worl4");
        s8 = u8"Hello world";   TRY(str_translate_in(s8, u8"abcde", u8"*"));      TEST_EQUAL(s8, u8"H*llo worl*");
        s8 = u8"€uro ∈lement";  TRY(str_translate_in(s8, u8"€∈", u8"∇√"));        TEST_EQUAL(s8, u8"∇uro √lement");
        s8 = u8"€uro ∈lement";  TRY(str_translate_in(s8, u8"€∈", u8"*"));         TEST_EQUAL(s8, u8"*uro *lement");
        s8 = u8"€uro ∈lement";  TRY(str_translate_in(s8, u8"e", u8"∇√"));         TEST_EQUAL(s8, u8"€uro ∈l∇m∇nt");

        s16 = u"";              TRY(str_translate_in(s16, u"", u""));            TEST_EQUAL(s16, u"");
        s16 = u"Hello world";   TRY(str_translate_in(s16, u"", u""));            TEST_EQUAL(s16, u"Hello world");
        s16 = u"Hello world";   TRY(str_translate_in(s16, u"", u"abcde"));       TEST_EQUAL(s16, u"Hello world");
        s16 = u"Hello world";   TRY(str_translate_in(s16, u"abcde", u""));       TEST_EQUAL(s16, u"Hello world");
        s16 = u"Hello world";   TRY(str_translate_in(s16, u"abcde", u"12345"));  TEST_EQUAL(s16, u"H5llo worl4");
        s16 = u"Hello world";   TRY(str_translate_in(s16, u"abcde", u"*"));      TEST_EQUAL(s16, u"H*llo worl*");
        s16 = u"€uro ∈lement";  TRY(str_translate_in(s16, u"€∈", u"∇√"));        TEST_EQUAL(s16, u"∇uro √lement");
        s16 = u"€uro ∈lement";  TRY(str_translate_in(s16, u"€∈", u"*"));         TEST_EQUAL(s16, u"*uro *lement");
        s16 = u"€uro ∈lement";  TRY(str_translate_in(s16, u"e", u"∇√"));         TEST_EQUAL(s16, u"€uro ∈l∇m∇nt");

        s32 = U"";              TRY(str_translate_in(s32, U"", U""));            TEST_EQUAL(s32, U"");
        s32 = U"Hello world";   TRY(str_translate_in(s32, U"", U""));            TEST_EQUAL(s32, U"Hello world");
        s32 = U"Hello world";   TRY(str_translate_in(s32, U"", U"abcde"));       TEST_EQUAL(s32, U"Hello world");
        s32 = U"Hello world";   TRY(str_translate_in(s32, U"abcde", U""));       TEST_EQUAL(s32, U"Hello world");
        s32 = U"Hello world";   TRY(str_translate_in(s32, U"abcde", U"12345"));  TEST_EQUAL(s32, U"H5llo worl4");
        s32 = U"Hello world";   TRY(str_translate_in(s32, U"abcde", U"*"));      TEST_EQUAL(s32, U"H*llo worl*");
        s32 = U"€uro ∈lement";  TRY(str_translate_in(s32, U"€∈", U"∇√"));        TEST_EQUAL(s32, U"∇uro √lement");
        s32 = U"€uro ∈lement";  TRY(str_translate_in(s32, U"€∈", U"*"));         TEST_EQUAL(s32, U"*uro *lement");
        s32 = U"€uro ∈lement";  TRY(str_translate_in(s32, U"e", U"∇√"));         TEST_EQUAL(s32, U"€uro ∈l∇m∇nt");

    }

    void check_trim() {

        u8string s8;
        u16string s16;
        u32string s32;

        TEST_EQUAL(str_trim(u8""s), u8"");
        TEST_EQUAL(str_trim(u8"Hello"s), u8"Hello");
        TEST_EQUAL(str_trim(u8"Hello world"s), u8"Hello world");
        TEST_EQUAL(str_trim(u8""s, u8""), u8"");
        TEST_EQUAL(str_trim(u8"Hello"s, u8""), u8"Hello");
        TEST_EQUAL(str_trim(u8"<<<>>>"s, u8"<>"), u8"");
        TEST_EQUAL(str_trim(u8"<<<Hello>>>"s, u8"<>"), u8"Hello");
        TEST_EQUAL(str_trim(u8"<<<Hello>>> <<<world>>>"s, u8"<>"), u8"Hello>>> <<<world");
        TEST_EQUAL(str_trim(u8"≤≤≤≥≥≥"s, u8"≤≥"), u8"");
        TEST_EQUAL(str_trim(u8"≤≤≤€uro≥≥≥"s, u8"≤≥"), u8"€uro");
        TEST_EQUAL(str_trim(u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, u8"≤≥"), u8"€uro≥≥≥ ≤≤≤∈lement");

        TEST_EQUAL(str_trim(u""s), u"");
        TEST_EQUAL(str_trim(u"Hello"s), u"Hello");
        TEST_EQUAL(str_trim(u"Hello world"s), u"Hello world");
        TEST_EQUAL(str_trim(u""s, u""), u"");
        TEST_EQUAL(str_trim(u"Hello"s, u""), u"Hello");
        TEST_EQUAL(str_trim(u"<<<>>>"s, u"<>"), u"");
        TEST_EQUAL(str_trim(u"<<<Hello>>>"s, u"<>"), u"Hello");
        TEST_EQUAL(str_trim(u"<<<Hello>>> <<<world>>>"s, u"<>"), u"Hello>>> <<<world");
        TEST_EQUAL(str_trim(u"≤≤≤≥≥≥"s, u"≤≥"), u"");
        TEST_EQUAL(str_trim(u"≤≤≤€uro≥≥≥"s, u"≤≥"), u"€uro");
        TEST_EQUAL(str_trim(u"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, u"≤≥"), u"€uro≥≥≥ ≤≤≤∈lement");

        TEST_EQUAL(str_trim(U""s), U"");
        TEST_EQUAL(str_trim(U"Hello"s), U"Hello");
        TEST_EQUAL(str_trim(U"Hello world"s), U"Hello world");
        TEST_EQUAL(str_trim(U""s, U""), U"");
        TEST_EQUAL(str_trim(U"Hello"s, U""), U"Hello");
        TEST_EQUAL(str_trim(U"<<<>>>"s, U"<>"), U"");
        TEST_EQUAL(str_trim(U"<<<Hello>>>"s, U"<>"), U"Hello");
        TEST_EQUAL(str_trim(U"<<<Hello>>> <<<world>>>"s, U"<>"), U"Hello>>> <<<world");
        TEST_EQUAL(str_trim(U"≤≤≤≥≥≥"s, U"≤≥"), U"");
        TEST_EQUAL(str_trim(U"≤≤≤€uro≥≥≥"s, U"≤≥"), U"€uro");
        TEST_EQUAL(str_trim(U"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, U"≤≥"), U"€uro≥≥≥ ≤≤≤∈lement");

        s8 = u8"";                          TRY(str_trim_in(s8));          TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";                     TRY(str_trim_in(s8));          TEST_EQUAL(s8, u8"Hello");
        s8 = u8"Hello world";               TRY(str_trim_in(s8));          TEST_EQUAL(s8, u8"Hello world");
        s8 = u8"";                          TRY(str_trim_in(s8, u8""));    TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";                     TRY(str_trim_in(s8, u8""));    TEST_EQUAL(s8, u8"Hello");
        s8 = u8"<<<>>>";                    TRY(str_trim_in(s8, u8"<>"));  TEST_EQUAL(s8, u8"");
        s8 = u8"<<<Hello>>>";               TRY(str_trim_in(s8, u8"<>"));  TEST_EQUAL(s8, u8"Hello");
        s8 = u8"<<<Hello>>> <<<world>>>";   TRY(str_trim_in(s8, u8"<>"));  TEST_EQUAL(s8, u8"Hello>>> <<<world");
        s8 = u8"≤≤≤≥≥≥";                    TRY(str_trim_in(s8, u8"≤≥"));  TEST_EQUAL(s8, u8"");
        s8 = u8"≤≤≤€uro≥≥≥";                TRY(str_trim_in(s8, u8"≤≥"));  TEST_EQUAL(s8, u8"€uro");
        s8 = u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_in(s8, u8"≤≥"));  TEST_EQUAL(s8, u8"€uro≥≥≥ ≤≤≤∈lement");

        s16 = u"";                          TRY(str_trim_in(s16));         TEST_EQUAL(s16, u"");
        s16 = u"Hello";                     TRY(str_trim_in(s16));         TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello world";               TRY(str_trim_in(s16));         TEST_EQUAL(s16, u"Hello world");
        s16 = u"";                          TRY(str_trim_in(s16, u""));    TEST_EQUAL(s16, u"");
        s16 = u"Hello";                     TRY(str_trim_in(s16, u""));    TEST_EQUAL(s16, u"Hello");
        s16 = u"<<<>>>";                    TRY(str_trim_in(s16, u"<>"));  TEST_EQUAL(s16, u"");
        s16 = u"<<<Hello>>>";               TRY(str_trim_in(s16, u"<>"));  TEST_EQUAL(s16, u"Hello");
        s16 = u"<<<Hello>>> <<<world>>>";   TRY(str_trim_in(s16, u"<>"));  TEST_EQUAL(s16, u"Hello>>> <<<world");
        s16 = u"≤≤≤≥≥≥";                    TRY(str_trim_in(s16, u"≤≥"));  TEST_EQUAL(s16, u"");
        s16 = u"≤≤≤€uro≥≥≥";                TRY(str_trim_in(s16, u"≤≥"));  TEST_EQUAL(s16, u"€uro");
        s16 = u"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_in(s16, u"≤≥"));  TEST_EQUAL(s16, u"€uro≥≥≥ ≤≤≤∈lement");

        s32 = U"";                          TRY(str_trim_in(s32));         TEST_EQUAL(s32, U"");
        s32 = U"Hello";                     TRY(str_trim_in(s32));         TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello world";               TRY(str_trim_in(s32));         TEST_EQUAL(s32, U"Hello world");
        s32 = U"";                          TRY(str_trim_in(s32, U""));    TEST_EQUAL(s32, U"");
        s32 = U"Hello";                     TRY(str_trim_in(s32, U""));    TEST_EQUAL(s32, U"Hello");
        s32 = U"<<<>>>";                    TRY(str_trim_in(s32, U"<>"));  TEST_EQUAL(s32, U"");
        s32 = U"<<<Hello>>>";               TRY(str_trim_in(s32, U"<>"));  TEST_EQUAL(s32, U"Hello");
        s32 = U"<<<Hello>>> <<<world>>>";   TRY(str_trim_in(s32, U"<>"));  TEST_EQUAL(s32, U"Hello>>> <<<world");
        s32 = U"≤≤≤≥≥≥";                    TRY(str_trim_in(s32, U"≤≥"));  TEST_EQUAL(s32, U"");
        s32 = U"≤≤≤€uro≥≥≥";                TRY(str_trim_in(s32, U"≤≥"));  TEST_EQUAL(s32, U"€uro");
        s32 = U"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_in(s32, U"≤≥"));  TEST_EQUAL(s32, U"€uro≥≥≥ ≤≤≤∈lement");

        TEST_EQUAL(str_trim_left(u8""s), u8"");
        TEST_EQUAL(str_trim_left(u8"Hello"s), u8"Hello");
        TEST_EQUAL(str_trim_left(u8"Hello world"s), u8"Hello world");
        TEST_EQUAL(str_trim_left(u8""s, u8""), u8"");
        TEST_EQUAL(str_trim_left(u8"Hello"s, u8""), u8"Hello");
        TEST_EQUAL(str_trim_left(u8"<<<>>>"s, u8"<>"), u8"");
        TEST_EQUAL(str_trim_left(u8"<<<Hello>>>"s, u8"<>"), u8"Hello>>>");
        TEST_EQUAL(str_trim_left(u8"<<<Hello>>> <<<world>>>"s, u8"<>"), u8"Hello>>> <<<world>>>");
        TEST_EQUAL(str_trim_left(u8"≤≤≤≥≥≥"s, u8"≤≥"), u8"");
        TEST_EQUAL(str_trim_left(u8"≤≤≤€uro≥≥≥"s, u8"≤≥"), u8"€uro≥≥≥");
        TEST_EQUAL(str_trim_left(u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, u8"≤≥"), u8"€uro≥≥≥ ≤≤≤∈lement≥≥≥");

        TEST_EQUAL(str_trim_left(u""s), u"");
        TEST_EQUAL(str_trim_left(u"Hello"s), u"Hello");
        TEST_EQUAL(str_trim_left(u"Hello world"s), u"Hello world");
        TEST_EQUAL(str_trim_left(u""s, u""), u"");
        TEST_EQUAL(str_trim_left(u"Hello"s, u""), u"Hello");
        TEST_EQUAL(str_trim_left(u"<<<>>>"s, u"<>"), u"");
        TEST_EQUAL(str_trim_left(u"<<<Hello>>>"s, u"<>"), u"Hello>>>");
        TEST_EQUAL(str_trim_left(u"<<<Hello>>> <<<world>>>"s, u"<>"), u"Hello>>> <<<world>>>");
        TEST_EQUAL(str_trim_left(u"≤≤≤≥≥≥"s, u"≤≥"), u"");
        TEST_EQUAL(str_trim_left(u"≤≤≤€uro≥≥≥"s, u"≤≥"), u"€uro≥≥≥");
        TEST_EQUAL(str_trim_left(u"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, u"≤≥"), u"€uro≥≥≥ ≤≤≤∈lement≥≥≥");

        TEST_EQUAL(str_trim_left(U""s), U"");
        TEST_EQUAL(str_trim_left(U"Hello"s), U"Hello");
        TEST_EQUAL(str_trim_left(U"Hello world"s), U"Hello world");
        TEST_EQUAL(str_trim_left(U""s, U""), U"");
        TEST_EQUAL(str_trim_left(U"Hello"s, U""), U"Hello");
        TEST_EQUAL(str_trim_left(U"<<<>>>"s, U"<>"), U"");
        TEST_EQUAL(str_trim_left(U"<<<Hello>>>"s, U"<>"), U"Hello>>>");
        TEST_EQUAL(str_trim_left(U"<<<Hello>>> <<<world>>>"s, U"<>"), U"Hello>>> <<<world>>>");
        TEST_EQUAL(str_trim_left(U"≤≤≤≥≥≥"s, U"≤≥"), U"");
        TEST_EQUAL(str_trim_left(U"≤≤≤€uro≥≥≥"s, U"≤≥"), U"€uro≥≥≥");
        TEST_EQUAL(str_trim_left(U"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, U"≤≥"), U"€uro≥≥≥ ≤≤≤∈lement≥≥≥");

        s8 = u8"";                          TRY(str_trim_left_in(s8));          TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";                     TRY(str_trim_left_in(s8));          TEST_EQUAL(s8, u8"Hello");
        s8 = u8"Hello world";               TRY(str_trim_left_in(s8));          TEST_EQUAL(s8, u8"Hello world");
        s8 = u8"";                          TRY(str_trim_left_in(s8, u8""));    TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";                     TRY(str_trim_left_in(s8, u8""));    TEST_EQUAL(s8, u8"Hello");
        s8 = u8"<<<>>>";                    TRY(str_trim_left_in(s8, u8"<>"));  TEST_EQUAL(s8, u8"");
        s8 = u8"<<<Hello>>>";               TRY(str_trim_left_in(s8, u8"<>"));  TEST_EQUAL(s8, u8"Hello>>>");
        s8 = u8"<<<Hello>>> <<<world>>>";   TRY(str_trim_left_in(s8, u8"<>"));  TEST_EQUAL(s8, u8"Hello>>> <<<world>>>");
        s8 = u8"≤≤≤≥≥≥";                    TRY(str_trim_left_in(s8, u8"≤≥"));  TEST_EQUAL(s8, u8"");
        s8 = u8"≤≤≤€uro≥≥≥";                TRY(str_trim_left_in(s8, u8"≤≥"));  TEST_EQUAL(s8, u8"€uro≥≥≥");
        s8 = u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_left_in(s8, u8"≤≥"));  TEST_EQUAL(s8, u8"€uro≥≥≥ ≤≤≤∈lement≥≥≥");

        s16 = u"";                          TRY(str_trim_left_in(s16));         TEST_EQUAL(s16, u"");
        s16 = u"Hello";                     TRY(str_trim_left_in(s16));         TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello world";               TRY(str_trim_left_in(s16));         TEST_EQUAL(s16, u"Hello world");
        s16 = u"";                          TRY(str_trim_left_in(s16, u""));    TEST_EQUAL(s16, u"");
        s16 = u"Hello";                     TRY(str_trim_left_in(s16, u""));    TEST_EQUAL(s16, u"Hello");
        s16 = u"<<<>>>";                    TRY(str_trim_left_in(s16, u"<>"));  TEST_EQUAL(s16, u"");
        s16 = u"<<<Hello>>>";               TRY(str_trim_left_in(s16, u"<>"));  TEST_EQUAL(s16, u"Hello>>>");
        s16 = u"<<<Hello>>> <<<world>>>";   TRY(str_trim_left_in(s16, u"<>"));  TEST_EQUAL(s16, u"Hello>>> <<<world>>>");
        s16 = u"≤≤≤≥≥≥";                    TRY(str_trim_left_in(s16, u"≤≥"));  TEST_EQUAL(s16, u"");
        s16 = u"≤≤≤€uro≥≥≥";                TRY(str_trim_left_in(s16, u"≤≥"));  TEST_EQUAL(s16, u"€uro≥≥≥");
        s16 = u"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_left_in(s16, u"≤≥"));  TEST_EQUAL(s16, u"€uro≥≥≥ ≤≤≤∈lement≥≥≥");

        s32 = U"";                          TRY(str_trim_left_in(s32));         TEST_EQUAL(s32, U"");
        s32 = U"Hello";                     TRY(str_trim_left_in(s32));         TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello world";               TRY(str_trim_left_in(s32));         TEST_EQUAL(s32, U"Hello world");
        s32 = U"";                          TRY(str_trim_left_in(s32, U""));    TEST_EQUAL(s32, U"");
        s32 = U"Hello";                     TRY(str_trim_left_in(s32, U""));    TEST_EQUAL(s32, U"Hello");
        s32 = U"<<<>>>";                    TRY(str_trim_left_in(s32, U"<>"));  TEST_EQUAL(s32, U"");
        s32 = U"<<<Hello>>>";               TRY(str_trim_left_in(s32, U"<>"));  TEST_EQUAL(s32, U"Hello>>>");
        s32 = U"<<<Hello>>> <<<world>>>";   TRY(str_trim_left_in(s32, U"<>"));  TEST_EQUAL(s32, U"Hello>>> <<<world>>>");
        s32 = U"≤≤≤≥≥≥";                    TRY(str_trim_left_in(s32, U"≤≥"));  TEST_EQUAL(s32, U"");
        s32 = U"≤≤≤€uro≥≥≥";                TRY(str_trim_left_in(s32, U"≤≥"));  TEST_EQUAL(s32, U"€uro≥≥≥");
        s32 = U"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_left_in(s32, U"≤≥"));  TEST_EQUAL(s32, U"€uro≥≥≥ ≤≤≤∈lement≥≥≥");

        TEST_EQUAL(str_trim_right(u8""s), u8"");
        TEST_EQUAL(str_trim_right(u8"Hello"s), u8"Hello");
        TEST_EQUAL(str_trim_right(u8"Hello world"s), u8"Hello world");
        TEST_EQUAL(str_trim_right(u8""s, u8""), u8"");
        TEST_EQUAL(str_trim_right(u8"Hello"s, u8""), u8"Hello");
        TEST_EQUAL(str_trim_right(u8"<<<>>>"s, u8"<>"), u8"");
        TEST_EQUAL(str_trim_right(u8"<<<Hello>>>"s, u8"<>"), u8"<<<Hello");
        TEST_EQUAL(str_trim_right(u8"<<<Hello>>> <<<world>>>"s, u8"<>"), u8"<<<Hello>>> <<<world");
        TEST_EQUAL(str_trim_right(u8"≤≤≤≥≥≥"s, u8"≤≥"), u8"");
        TEST_EQUAL(str_trim_right(u8"≤≤≤€uro≥≥≥"s, u8"≤≥"), u8"≤≤≤€uro");
        TEST_EQUAL(str_trim_right(u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, u8"≤≥"), u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement");

        TEST_EQUAL(str_trim_right(u""s), u"");
        TEST_EQUAL(str_trim_right(u"Hello"s), u"Hello");
        TEST_EQUAL(str_trim_right(u"Hello world"s), u"Hello world");
        TEST_EQUAL(str_trim_right(u""s, u""), u"");
        TEST_EQUAL(str_trim_right(u"Hello"s, u""), u"Hello");
        TEST_EQUAL(str_trim_right(u"<<<>>>"s, u"<>"), u"");
        TEST_EQUAL(str_trim_right(u"<<<Hello>>>"s, u"<>"), u"<<<Hello");
        TEST_EQUAL(str_trim_right(u"<<<Hello>>> <<<world>>>"s, u"<>"), u"<<<Hello>>> <<<world");
        TEST_EQUAL(str_trim_right(u"≤≤≤≥≥≥"s, u"≤≥"), u"");
        TEST_EQUAL(str_trim_right(u"≤≤≤€uro≥≥≥"s, u"≤≥"), u"≤≤≤€uro");
        TEST_EQUAL(str_trim_right(u"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, u"≤≥"), u"≤≤≤€uro≥≥≥ ≤≤≤∈lement");

        TEST_EQUAL(str_trim_right(U""s), U"");
        TEST_EQUAL(str_trim_right(U"Hello"s), U"Hello");
        TEST_EQUAL(str_trim_right(U"Hello world"s), U"Hello world");
        TEST_EQUAL(str_trim_right(U""s, U""), U"");
        TEST_EQUAL(str_trim_right(U"Hello"s, U""), U"Hello");
        TEST_EQUAL(str_trim_right(U"<<<>>>"s, U"<>"), U"");
        TEST_EQUAL(str_trim_right(U"<<<Hello>>>"s, U"<>"), U"<<<Hello");
        TEST_EQUAL(str_trim_right(U"<<<Hello>>> <<<world>>>"s, U"<>"), U"<<<Hello>>> <<<world");
        TEST_EQUAL(str_trim_right(U"≤≤≤≥≥≥"s, U"≤≥"), U"");
        TEST_EQUAL(str_trim_right(U"≤≤≤€uro≥≥≥"s, U"≤≥"), U"≤≤≤€uro");
        TEST_EQUAL(str_trim_right(U"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, U"≤≥"), U"≤≤≤€uro≥≥≥ ≤≤≤∈lement");

        s8 = u8"";                          TRY(str_trim_right_in(s8));          TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";                     TRY(str_trim_right_in(s8));          TEST_EQUAL(s8, u8"Hello");
        s8 = u8"Hello world";               TRY(str_trim_right_in(s8));          TEST_EQUAL(s8, u8"Hello world");
        s8 = u8"";                          TRY(str_trim_right_in(s8, u8""));    TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";                     TRY(str_trim_right_in(s8, u8""));    TEST_EQUAL(s8, u8"Hello");
        s8 = u8"<<<>>>";                    TRY(str_trim_right_in(s8, u8"<>"));  TEST_EQUAL(s8, u8"");
        s8 = u8"<<<Hello>>>";               TRY(str_trim_right_in(s8, u8"<>"));  TEST_EQUAL(s8, u8"<<<Hello");
        s8 = u8"<<<Hello>>> <<<world>>>";   TRY(str_trim_right_in(s8, u8"<>"));  TEST_EQUAL(s8, u8"<<<Hello>>> <<<world");
        s8 = u8"≤≤≤≥≥≥";                    TRY(str_trim_right_in(s8, u8"≤≥"));  TEST_EQUAL(s8, u8"");
        s8 = u8"≤≤≤€uro≥≥≥";                TRY(str_trim_right_in(s8, u8"≤≥"));  TEST_EQUAL(s8, u8"≤≤≤€uro");
        s8 = u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_right_in(s8, u8"≤≥"));  TEST_EQUAL(s8, u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement");

        s16 = u"";                          TRY(str_trim_right_in(s16));         TEST_EQUAL(s16, u"");
        s16 = u"Hello";                     TRY(str_trim_right_in(s16));         TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello world";               TRY(str_trim_right_in(s16));         TEST_EQUAL(s16, u"Hello world");
        s16 = u"";                          TRY(str_trim_right_in(s16, u""));    TEST_EQUAL(s16, u"");
        s16 = u"Hello";                     TRY(str_trim_right_in(s16, u""));    TEST_EQUAL(s16, u"Hello");
        s16 = u"<<<>>>";                    TRY(str_trim_right_in(s16, u"<>"));  TEST_EQUAL(s16, u"");
        s16 = u"<<<Hello>>>";               TRY(str_trim_right_in(s16, u"<>"));  TEST_EQUAL(s16, u"<<<Hello");
        s16 = u"<<<Hello>>> <<<world>>>";   TRY(str_trim_right_in(s16, u"<>"));  TEST_EQUAL(s16, u"<<<Hello>>> <<<world");
        s16 = u"≤≤≤≥≥≥";                    TRY(str_trim_right_in(s16, u"≤≥"));  TEST_EQUAL(s16, u"");
        s16 = u"≤≤≤€uro≥≥≥";                TRY(str_trim_right_in(s16, u"≤≥"));  TEST_EQUAL(s16, u"≤≤≤€uro");
        s16 = u"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_right_in(s16, u"≤≥"));  TEST_EQUAL(s16, u"≤≤≤€uro≥≥≥ ≤≤≤∈lement");

        s32 = U"";                          TRY(str_trim_right_in(s32));         TEST_EQUAL(s32, U"");
        s32 = U"Hello";                     TRY(str_trim_right_in(s32));         TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello world";               TRY(str_trim_right_in(s32));         TEST_EQUAL(s32, U"Hello world");
        s32 = U"";                          TRY(str_trim_right_in(s32, U""));    TEST_EQUAL(s32, U"");
        s32 = U"Hello";                     TRY(str_trim_right_in(s32, U""));    TEST_EQUAL(s32, U"Hello");
        s32 = U"<<<>>>";                    TRY(str_trim_right_in(s32, U"<>"));  TEST_EQUAL(s32, U"");
        s32 = U"<<<Hello>>>";               TRY(str_trim_right_in(s32, U"<>"));  TEST_EQUAL(s32, U"<<<Hello");
        s32 = U"<<<Hello>>> <<<world>>>";   TRY(str_trim_right_in(s32, U"<>"));  TEST_EQUAL(s32, U"<<<Hello>>> <<<world");
        s32 = U"≤≤≤≥≥≥";                    TRY(str_trim_right_in(s32, U"≤≥"));  TEST_EQUAL(s32, U"");
        s32 = U"≤≤≤€uro≥≥≥";                TRY(str_trim_right_in(s32, U"≤≥"));  TEST_EQUAL(s32, U"≤≤≤€uro");
        s32 = U"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_right_in(s32, U"≤≥"));  TEST_EQUAL(s32, U"≤≤≤€uro≥≥≥ ≤≤≤∈lement");

    }

    void check_trim_if() {

        u8string s8;
        u16string s16;
        u32string s32;

        TEST_EQUAL(str_trim_if(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_if(u8"Hello"s, char_is_line_break), u8"Hello");
        TEST_EQUAL(str_trim_if(u8"\r\nHello\r\n"s, char_is_line_break), u8"Hello");
        TEST_EQUAL(str_trim_left_if(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_left_if(u8"Hello"s, char_is_line_break), u8"Hello");
        TEST_EQUAL(str_trim_left_if(u8"\r\nHello\r\n"s, char_is_line_break), u8"Hello\r\n");
        TEST_EQUAL(str_trim_right_if(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_right_if(u8"Hello"s, char_is_line_break), u8"Hello");
        TEST_EQUAL(str_trim_right_if(u8"\r\nHello\r\n"s, char_is_line_break), u8"\r\nHello");
        TEST_EQUAL(str_trim_if_not(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_if_not(u8"Hello\r\nworld"s, char_is_line_break), u8"\r\n");
        TEST_EQUAL(str_trim_if_not(u8"\r\nHello world\r\n"s, char_is_line_break), u8"\r\nHello world\r\n");
        TEST_EQUAL(str_trim_left_if_not(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_left_if_not(u8"Hello\r\nworld"s, char_is_line_break), u8"\r\nworld");
        TEST_EQUAL(str_trim_left_if_not(u8"\r\nHello world\r\n"s, char_is_line_break), u8"\r\nHello world\r\n");
        TEST_EQUAL(str_trim_right_if_not(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_right_if_not(u8"Hello\r\nworld"s, char_is_line_break), u8"Hello\r\n");
        TEST_EQUAL(str_trim_right_if_not(u8"\r\nHello world\r\n"s, char_is_line_break), u8"\r\nHello world\r\n");

        TEST_EQUAL(str_trim_if(u""s, char_is_line_break), u"");
        TEST_EQUAL(str_trim_if(u"Hello"s, char_is_line_break), u"Hello");
        TEST_EQUAL(str_trim_if(u"\r\nHello\r\n"s, char_is_line_break), u"Hello");
        TEST_EQUAL(str_trim_left_if(u""s, char_is_line_break), u"");
        TEST_EQUAL(str_trim_left_if(u"Hello"s, char_is_line_break), u"Hello");
        TEST_EQUAL(str_trim_left_if(u"\r\nHello\r\n"s, char_is_line_break), u"Hello\r\n");
        TEST_EQUAL(str_trim_right_if(u""s, char_is_line_break), u"");
        TEST_EQUAL(str_trim_right_if(u"Hello"s, char_is_line_break), u"Hello");
        TEST_EQUAL(str_trim_right_if(u"\r\nHello\r\n"s, char_is_line_break), u"\r\nHello");
        TEST_EQUAL(str_trim_if_not(u""s, char_is_line_break), u"");
        TEST_EQUAL(str_trim_if_not(u"Hello\r\nworld"s, char_is_line_break), u"\r\n");
        TEST_EQUAL(str_trim_if_not(u"\r\nHello world\r\n"s, char_is_line_break), u"\r\nHello world\r\n");
        TEST_EQUAL(str_trim_left_if_not(u""s, char_is_line_break), u"");
        TEST_EQUAL(str_trim_left_if_not(u"Hello\r\nworld"s, char_is_line_break), u"\r\nworld");
        TEST_EQUAL(str_trim_left_if_not(u"\r\nHello world\r\n"s, char_is_line_break), u"\r\nHello world\r\n");
        TEST_EQUAL(str_trim_right_if_not(u""s, char_is_line_break), u"");
        TEST_EQUAL(str_trim_right_if_not(u"Hello\r\nworld"s, char_is_line_break), u"Hello\r\n");
        TEST_EQUAL(str_trim_right_if_not(u"\r\nHello world\r\n"s, char_is_line_break), u"\r\nHello world\r\n");

        TEST_EQUAL(str_trim_if(U""s, char_is_line_break), U"");
        TEST_EQUAL(str_trim_if(U"Hello"s, char_is_line_break), U"Hello");
        TEST_EQUAL(str_trim_if(U"\r\nHello\r\n"s, char_is_line_break), U"Hello");
        TEST_EQUAL(str_trim_left_if(U""s, char_is_line_break), U"");
        TEST_EQUAL(str_trim_left_if(U"Hello"s, char_is_line_break), U"Hello");
        TEST_EQUAL(str_trim_left_if(U"\r\nHello\r\n"s, char_is_line_break), U"Hello\r\n");
        TEST_EQUAL(str_trim_right_if(U""s, char_is_line_break), U"");
        TEST_EQUAL(str_trim_right_if(U"Hello"s, char_is_line_break), U"Hello");
        TEST_EQUAL(str_trim_right_if(U"\r\nHello\r\n"s, char_is_line_break), U"\r\nHello");
        TEST_EQUAL(str_trim_if_not(U""s, char_is_line_break), U"");
        TEST_EQUAL(str_trim_if_not(U"Hello\r\nworld"s, char_is_line_break), U"\r\n");
        TEST_EQUAL(str_trim_if_not(U"\r\nHello world\r\n"s, char_is_line_break), U"\r\nHello world\r\n");
        TEST_EQUAL(str_trim_left_if_not(U""s, char_is_line_break), U"");
        TEST_EQUAL(str_trim_left_if_not(U"Hello\r\nworld"s, char_is_line_break), U"\r\nworld");
        TEST_EQUAL(str_trim_left_if_not(U"\r\nHello world\r\n"s, char_is_line_break), U"\r\nHello world\r\n");
        TEST_EQUAL(str_trim_right_if_not(U""s, char_is_line_break), U"");
        TEST_EQUAL(str_trim_right_if_not(U"Hello\r\nworld"s, char_is_line_break), U"Hello\r\n");
        TEST_EQUAL(str_trim_right_if_not(U"\r\nHello world\r\n"s, char_is_line_break), U"\r\nHello world\r\n");

        s8 = u8"";                     TRY(str_trim_in_if(s8, char_is_line_break));            TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";                TRY(str_trim_in_if(s8, char_is_line_break));            TEST_EQUAL(s8, u8"Hello");
        s8 = u8"\r\nHello\r\n";        TRY(str_trim_in_if(s8, char_is_line_break));            TEST_EQUAL(s8, u8"Hello");
        s8 = u8"";                     TRY(str_trim_left_in_if(s8, char_is_line_break));       TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";                TRY(str_trim_left_in_if(s8, char_is_line_break));       TEST_EQUAL(s8, u8"Hello");
        s8 = u8"\r\nHello\r\n";        TRY(str_trim_left_in_if(s8, char_is_line_break));       TEST_EQUAL(s8, u8"Hello\r\n");
        s8 = u8"";                     TRY(str_trim_right_in_if(s8, char_is_line_break));      TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";                TRY(str_trim_right_in_if(s8, char_is_line_break));      TEST_EQUAL(s8, u8"Hello");
        s8 = u8"\r\nHello\r\n";        TRY(str_trim_right_in_if(s8, char_is_line_break));      TEST_EQUAL(s8, u8"\r\nHello");
        s8 = u8"";                     TRY(str_trim_in_if_not(s8, char_is_line_break));        TEST_EQUAL(s8, u8"");
        s8 = u8"Hello\r\nworld";       TRY(str_trim_in_if_not(s8, char_is_line_break));        TEST_EQUAL(s8, u8"\r\n");
        s8 = u8"\r\nHello world\r\n";  TRY(str_trim_in_if_not(s8, char_is_line_break));        TEST_EQUAL(s8, u8"\r\nHello world\r\n");
        s8 = u8"";                     TRY(str_trim_left_in_if_not(s8, char_is_line_break));   TEST_EQUAL(s8, u8"");
        s8 = u8"Hello\r\nworld";       TRY(str_trim_left_in_if_not(s8, char_is_line_break));   TEST_EQUAL(s8, u8"\r\nworld");
        s8 = u8"\r\nHello world\r\n";  TRY(str_trim_left_in_if_not(s8, char_is_line_break));   TEST_EQUAL(s8, u8"\r\nHello world\r\n");
        s8 = u8"";                     TRY(str_trim_right_in_if_not(s8, char_is_line_break));  TEST_EQUAL(s8, u8"");
        s8 = u8"Hello\r\nworld";       TRY(str_trim_right_in_if_not(s8, char_is_line_break));  TEST_EQUAL(s8, u8"Hello\r\n");
        s8 = u8"\r\nHello world\r\n";  TRY(str_trim_right_in_if_not(s8, char_is_line_break));  TEST_EQUAL(s8, u8"\r\nHello world\r\n");

        s16 = u"";                     TRY(str_trim_in_if(s16, char_is_line_break));            TEST_EQUAL(s16, u"");
        s16 = u"Hello";                TRY(str_trim_in_if(s16, char_is_line_break));            TEST_EQUAL(s16, u"Hello");
        s16 = u"\r\nHello\r\n";        TRY(str_trim_in_if(s16, char_is_line_break));            TEST_EQUAL(s16, u"Hello");
        s16 = u"";                     TRY(str_trim_left_in_if(s16, char_is_line_break));       TEST_EQUAL(s16, u"");
        s16 = u"Hello";                TRY(str_trim_left_in_if(s16, char_is_line_break));       TEST_EQUAL(s16, u"Hello");
        s16 = u"\r\nHello\r\n";        TRY(str_trim_left_in_if(s16, char_is_line_break));       TEST_EQUAL(s16, u"Hello\r\n");
        s16 = u"";                     TRY(str_trim_right_in_if(s16, char_is_line_break));      TEST_EQUAL(s16, u"");
        s16 = u"Hello";                TRY(str_trim_right_in_if(s16, char_is_line_break));      TEST_EQUAL(s16, u"Hello");
        s16 = u"\r\nHello\r\n";        TRY(str_trim_right_in_if(s16, char_is_line_break));      TEST_EQUAL(s16, u"\r\nHello");
        s16 = u"";                     TRY(str_trim_in_if_not(s16, char_is_line_break));        TEST_EQUAL(s16, u"");
        s16 = u"Hello\r\nworld";       TRY(str_trim_in_if_not(s16, char_is_line_break));        TEST_EQUAL(s16, u"\r\n");
        s16 = u"\r\nHello world\r\n";  TRY(str_trim_in_if_not(s16, char_is_line_break));        TEST_EQUAL(s16, u"\r\nHello world\r\n");
        s16 = u"";                     TRY(str_trim_left_in_if_not(s16, char_is_line_break));   TEST_EQUAL(s16, u"");
        s16 = u"Hello\r\nworld";       TRY(str_trim_left_in_if_not(s16, char_is_line_break));   TEST_EQUAL(s16, u"\r\nworld");
        s16 = u"\r\nHello world\r\n";  TRY(str_trim_left_in_if_not(s16, char_is_line_break));   TEST_EQUAL(s16, u"\r\nHello world\r\n");
        s16 = u"";                     TRY(str_trim_right_in_if_not(s16, char_is_line_break));  TEST_EQUAL(s16, u"");
        s16 = u"Hello\r\nworld";       TRY(str_trim_right_in_if_not(s16, char_is_line_break));  TEST_EQUAL(s16, u"Hello\r\n");
        s16 = u"\r\nHello world\r\n";  TRY(str_trim_right_in_if_not(s16, char_is_line_break));  TEST_EQUAL(s16, u"\r\nHello world\r\n");

        s32 = U"";                     TRY(str_trim_in_if(s32, char_is_line_break));            TEST_EQUAL(s32, U"");
        s32 = U"Hello";                TRY(str_trim_in_if(s32, char_is_line_break));            TEST_EQUAL(s32, U"Hello");
        s32 = U"\r\nHello\r\n";        TRY(str_trim_in_if(s32, char_is_line_break));            TEST_EQUAL(s32, U"Hello");
        s32 = U"";                     TRY(str_trim_left_in_if(s32, char_is_line_break));       TEST_EQUAL(s32, U"");
        s32 = U"Hello";                TRY(str_trim_left_in_if(s32, char_is_line_break));       TEST_EQUAL(s32, U"Hello");
        s32 = U"\r\nHello\r\n";        TRY(str_trim_left_in_if(s32, char_is_line_break));       TEST_EQUAL(s32, U"Hello\r\n");
        s32 = U"";                     TRY(str_trim_right_in_if(s32, char_is_line_break));      TEST_EQUAL(s32, U"");
        s32 = U"Hello";                TRY(str_trim_right_in_if(s32, char_is_line_break));      TEST_EQUAL(s32, U"Hello");
        s32 = U"\r\nHello\r\n";        TRY(str_trim_right_in_if(s32, char_is_line_break));      TEST_EQUAL(s32, U"\r\nHello");
        s32 = U"";                     TRY(str_trim_in_if_not(s32, char_is_line_break));        TEST_EQUAL(s32, U"");
        s32 = U"Hello\r\nworld";       TRY(str_trim_in_if_not(s32, char_is_line_break));        TEST_EQUAL(s32, U"\r\n");
        s32 = U"\r\nHello world\r\n";  TRY(str_trim_in_if_not(s32, char_is_line_break));        TEST_EQUAL(s32, U"\r\nHello world\r\n");
        s32 = U"";                     TRY(str_trim_left_in_if_not(s32, char_is_line_break));   TEST_EQUAL(s32, U"");
        s32 = U"Hello\r\nworld";       TRY(str_trim_left_in_if_not(s32, char_is_line_break));   TEST_EQUAL(s32, U"\r\nworld");
        s32 = U"\r\nHello world\r\n";  TRY(str_trim_left_in_if_not(s32, char_is_line_break));   TEST_EQUAL(s32, U"\r\nHello world\r\n");
        s32 = U"";                     TRY(str_trim_right_in_if_not(s32, char_is_line_break));  TEST_EQUAL(s32, U"");
        s32 = U"Hello\r\nworld";       TRY(str_trim_right_in_if_not(s32, char_is_line_break));  TEST_EQUAL(s32, U"Hello\r\n");
        s32 = U"\r\nHello world\r\n";  TRY(str_trim_right_in_if_not(s32, char_is_line_break));  TEST_EQUAL(s32, U"\r\nHello world\r\n");

    }

    void check_unify() {

        u8string s8;
        u16string s16;
        u32string s32;

        TEST_EQUAL(str_unify_lines(u8""s), u8"");
        TEST_EQUAL(str_unify_lines(u8"Hello world\nGoodbye\n"s), u8"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\nGoodbye\n"s, u8"\r\n"), u8"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\nGoodbye\n"s, U'*'), u8"Hello world*Goodbye*");
        TEST_EQUAL(str_unify_lines(u8"Hello world\r\nGoodbye\r\n"s), u8"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\r\nGoodbye\r\n"s, u8"\r\n"), u8"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\r\nGoodbye\r\n"s, U'*'), u8"Hello world*Goodbye*");
        TEST_EQUAL(str_unify_lines(u8"Hello world\u2028Goodbye\u2029"s), u8"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\u2028Goodbye\u2029"s, u8"\r\n"), u8"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\u2028Goodbye\u2029"s, U'*'), u8"Hello world*Goodbye*");

        TEST_EQUAL(str_unify_lines(u""s), u"");
        TEST_EQUAL(str_unify_lines(u"Hello world\nGoodbye\n"s), u"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u"Hello world\nGoodbye\n"s, u"\r\n"), u"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(u"Hello world\nGoodbye\n"s, U'*'), u"Hello world*Goodbye*");
        TEST_EQUAL(str_unify_lines(u"Hello world\r\nGoodbye\r\n"s), u"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u"Hello world\r\nGoodbye\r\n"s, u"\r\n"), u"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(u"Hello world\r\nGoodbye\r\n"s, U'*'), u"Hello world*Goodbye*");
        TEST_EQUAL(str_unify_lines(u"Hello world\u2028Goodbye\u2029"s), u"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u"Hello world\u2028Goodbye\u2029"s, u"\r\n"), u"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(u"Hello world\u2028Goodbye\u2029"s, U'*'), u"Hello world*Goodbye*");

        TEST_EQUAL(str_unify_lines(U""s), U"");
        TEST_EQUAL(str_unify_lines(U"Hello world\nGoodbye\n"s), U"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(U"Hello world\nGoodbye\n"s, U"\r\n"), U"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(U"Hello world\nGoodbye\n"s, U'*'), U"Hello world*Goodbye*");
        TEST_EQUAL(str_unify_lines(U"Hello world\r\nGoodbye\r\n"s), U"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(U"Hello world\r\nGoodbye\r\n"s, U"\r\n"), U"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(U"Hello world\r\nGoodbye\r\n"s, U'*'), U"Hello world*Goodbye*");
        TEST_EQUAL(str_unify_lines(U"Hello world\u2028Goodbye\u2029"s), U"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(U"Hello world\u2028Goodbye\u2029"s, U"\r\n"), U"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(U"Hello world\u2028Goodbye\u2029"s, U'*'), U"Hello world*Goodbye*");

        s8 = u8"";                                TRY(str_unify_lines_in(s8));            TEST_EQUAL(s8, u8"");
        s8 = u8"Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s8));            TEST_EQUAL(s8, u8"Hello world\nGoodbye\n");
        s8 = u8"Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s8, u8"\r\n"));  TEST_EQUAL(s8, u8"Hello world\r\nGoodbye\r\n");
        s8 = u8"Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s8));            TEST_EQUAL(s8, u8"Hello world\nGoodbye\n");
        s8 = u8"Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s8, u8"\r\n"));  TEST_EQUAL(s8, u8"Hello world\r\nGoodbye\r\n");
        s8 = u8"Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s8));            TEST_EQUAL(s8, u8"Hello world\nGoodbye\n");
        s8 = u8"Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s8, u8"\r\n"));  TEST_EQUAL(s8, u8"Hello world\r\nGoodbye\r\n");

        s16 = u"";                                TRY(str_unify_lines_in(s16));           TEST_EQUAL(s16, u"");
        s16 = u"Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s16));           TEST_EQUAL(s16, u"Hello world\nGoodbye\n");
        s16 = u"Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s16, u"\r\n"));  TEST_EQUAL(s16, u"Hello world\r\nGoodbye\r\n");
        s16 = u"Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s16));           TEST_EQUAL(s16, u"Hello world\nGoodbye\n");
        s16 = u"Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s16, u"\r\n"));  TEST_EQUAL(s16, u"Hello world\r\nGoodbye\r\n");
        s16 = u"Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s16));           TEST_EQUAL(s16, u"Hello world\nGoodbye\n");
        s16 = u"Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s16, u"\r\n"));  TEST_EQUAL(s16, u"Hello world\r\nGoodbye\r\n");

        s32 = U"";                                TRY(str_unify_lines_in(s32));           TEST_EQUAL(s32, U"");
        s32 = U"Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s32));           TEST_EQUAL(s32, U"Hello world\nGoodbye\n");
        s32 = U"Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s32, U"\r\n"));  TEST_EQUAL(s32, U"Hello world\r\nGoodbye\r\n");
        s32 = U"Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s32));           TEST_EQUAL(s32, U"Hello world\nGoodbye\n");
        s32 = U"Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s32, U"\r\n"));  TEST_EQUAL(s32, U"Hello world\r\nGoodbye\r\n");
        s32 = U"Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s32));           TEST_EQUAL(s32, U"Hello world\nGoodbye\n");
        s32 = U"Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s32, U"\r\n"));  TEST_EQUAL(s32, U"Hello world\r\nGoodbye\r\n");

    }

    void check_wrap() {

        u8string s8, t8;
        u16string s16, t16;
        u32string s32, t32;

        TEST_EQUAL(str_wrap(""s), ""s);
        TEST_EQUAL(str_wrap("\r\n"s), ""s);
        s8 =
            "Lorem ipsum dolor sit amet,\r\n"
            "consectetur adipisicing elit,\r\n"
            "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            "Ut enim ad minim veniam,\r\n"
            "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n";
        t8 =
            //...:....1....:....2....:....3....:....4....:....5
            "Lorem ipsum dolor sit amet, consectetur\n"
            "adipisicing elit, sed do eiusmod tempor\n"
            "incididunt ut labore et dolore magna\n"
            "aliqua. Ut enim ad minim veniam, quis\n"
            "nostrud exercitation ullamco laboris\n"
            "nisi ut aliquip ex ea commodo consequat.\n";
        TEST_EQUAL(str_wrap(s8, 0, 40), t8);
        TRY(str_wrap_in(s8, 0, 40));
        TEST_EQUAL(s8, t8);
        s8 =
            "\r\n"
            "\r\n"
            "Lorem ipsum dolor sit amet,\r\n"
            "consectetur adipisicing elit,\r\n"
            "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            "\r\n"
            "Lorem ipsum dolor sit amet,\r\n"
            "consectetur adipisicing elit,\r\n"
            "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            "Ut enim ad minim veniam,\r\n"
            "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n"
            "\r\n"
            "Lorem ipsum dolor sit amet, consectetur adipisicing elit.\r\n"
            "\r\n"
            "\r\n";
        t8 =
            //...:....1....:....2....:....3....:....4....:....5
            "Lorem ipsum dolor sit amet, consectetur\n"
            "adipisicing elit, sed do eiusmod tempor\n"
            "incididunt ut labore et dolore magna\n"
            "aliqua.\n"
            "\n"
            "Lorem ipsum dolor sit amet, consectetur\n"
            "adipisicing elit, sed do eiusmod tempor\n"
            "incididunt ut labore et dolore magna\n"
            "aliqua. Ut enim ad minim veniam, quis\n"
            "nostrud exercitation ullamco laboris\n"
            "nisi ut aliquip ex ea commodo consequat.\n"
            "\n"
            "Lorem ipsum dolor sit amet, consectetur\n"
            "adipisicing elit.\n";
        TEST_EQUAL(str_wrap(s8, 0, 40), t8);
        TRY(str_wrap_in(s8, 0, 40));
        TEST_EQUAL(s8, t8);
        t8 =
            //...:....1....:....2....:....3....:....4....:....5
            "                    Lorem ipsum dolor sit amet,\n"
            "          consectetur adipisicing elit, sed do\n"
            "          eiusmod tempor incididunt ut labore et\n"
            "          dolore magna aliqua.\n"
            "\n"
            "                    Lorem ipsum dolor sit amet,\n"
            "          consectetur adipisicing elit, sed do\n"
            "          eiusmod tempor incididunt ut labore et\n"
            "          dolore magna aliqua. Ut enim ad minim\n"
            "          veniam, quis nostrud exercitation\n"
            "          ullamco laboris nisi ut aliquip ex ea\n"
            "          commodo consequat.\n"
            "\n"
            "                    Lorem ipsum dolor sit amet,\n"
            "          consectetur adipisicing elit.\n";
        TEST_EQUAL(str_wrap(s8, 0, 50, 20, 10), t8);
        TRY(str_wrap_in(s8, 0, 50, 20, 10));
        TEST_EQUAL(s8, t8);
        s8 =
            "Lorem ipsum dolor sit amet,\r\n"
            "consectetur adipisicing elit,\r\n"
            "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            "Ut enim ad minim veniam,\r\n"
            "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n";
        t8 =
            //...:....1....:....2....:....3....:....4....:....5
            "Lorem ipsum dolor sit amet, consectetur\r\n"
            "adipisicing elit, sed do eiusmod tempor\r\n"
            "incididunt ut labore et dolore magna\r\n"
            "aliqua. Ut enim ad minim veniam, quis\r\n"
            "nostrud exercitation ullamco laboris\r\n"
            "nisi ut aliquip ex ea commodo consequat.\r\n";
        TEST_EQUAL(str_wrap(s8, wrap_crlf, 40), t8);
        TRY(str_wrap_in(s8, wrap_crlf, 40));
        TEST_EQUAL(s8, t8);
        s8 =
            "Lorem ipsum dolor sit amet,\r\n"
            "consectetur adipisicing elit,\r\n"
            "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            "\r\n"
            "\r\n"
            "    Lorem ipsum dolor sit amet, consectetur adipisicing elit,\r\n"
            "    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            "\r\n"
            "\r\n"
            "Lorem ipsum dolor sit amet, consectetur adipisicing elit.\r\n";
        t8 =
            //...:....1....:....2....:....3....:....4....:....5
            "Lorem ipsum dolor sit amet, consectetur\n"
            "adipisicing elit, sed do eiusmod tempor\n"
            "incididunt ut labore et dolore magna\n"
            "aliqua.\n"
            "\n"
            "    Lorem ipsum dolor sit amet, consectetur adipisicing elit,\n"
            "    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
            "\n"
            "Lorem ipsum dolor sit amet, consectetur\n"
            "adipisicing elit.\n";
        TEST_EQUAL(str_wrap(s8, wrap_preserve, 40), t8);
        TRY(str_wrap_in(s8, wrap_preserve, 40));
        TEST_EQUAL(s8, t8);

        TEST_EQUAL(str_wrap(U""s), U""s);
        TEST_EQUAL(str_wrap(U"\r\n"s), U""s);
        s32 =
            U"Lorem ipsum dolor sit amet,\r\n"
            U"consectetur adipisicing elit,\r\n"
            U"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            U"Ut enim ad minim veniam,\r\n"
            U"quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n";
        t32 =
            //....:....1....:....2....:....3....:....4....:....5
            U"Lorem ipsum dolor sit amet, consectetur\n"
            U"adipisicing elit, sed do eiusmod tempor\n"
            U"incididunt ut labore et dolore magna\n"
            U"aliqua. Ut enim ad minim veniam, quis\n"
            U"nostrud exercitation ullamco laboris\n"
            U"nisi ut aliquip ex ea commodo consequat.\n";
        TEST_EQUAL(str_wrap(s32, 0, 40), t32);
        TRY(str_wrap_in(s32, 0, 40));
        TEST_EQUAL(s32, t32);
        s32 =
            U"\r\n"
            U"\r\n"
            U"Lorem ipsum dolor sit amet,\r\n"
            U"consectetur adipisicing elit,\r\n"
            U"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            U"\r\n"
            U"Lorem ipsum dolor sit amet,\r\n"
            U"consectetur adipisicing elit,\r\n"
            U"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            U"Ut enim ad minim veniam,\r\n"
            U"quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n"
            U"\r\n"
            U"Lorem ipsum dolor sit amet, consectetur adipisicing elit.\r\n"
            U"\r\n"
            U"\r\n";
        t32 =
            //....:....1....:....2....:....3....:....4....:....5
            U"Lorem ipsum dolor sit amet, consectetur\n"
            U"adipisicing elit, sed do eiusmod tempor\n"
            U"incididunt ut labore et dolore magna\n"
            U"aliqua.\n"
            U"\n"
            U"Lorem ipsum dolor sit amet, consectetur\n"
            U"adipisicing elit, sed do eiusmod tempor\n"
            U"incididunt ut labore et dolore magna\n"
            U"aliqua. Ut enim ad minim veniam, quis\n"
            U"nostrud exercitation ullamco laboris\n"
            U"nisi ut aliquip ex ea commodo consequat.\n"
            U"\n"
            U"Lorem ipsum dolor sit amet, consectetur\n"
            U"adipisicing elit.\n";
        TEST_EQUAL(str_wrap(s32, 0, 40), t32);
        TRY(str_wrap_in(s32, 0, 40));
        TEST_EQUAL(s32, t32);
        t32 =
            //....:....1....:....2....:....3....:....4....:....5
            U"                    Lorem ipsum dolor sit amet,\n"
            U"          consectetur adipisicing elit, sed do\n"
            U"          eiusmod tempor incididunt ut labore et\n"
            U"          dolore magna aliqua.\n"
            U"\n"
            U"                    Lorem ipsum dolor sit amet,\n"
            U"          consectetur adipisicing elit, sed do\n"
            U"          eiusmod tempor incididunt ut labore et\n"
            U"          dolore magna aliqua. Ut enim ad minim\n"
            U"          veniam, quis nostrud exercitation\n"
            U"          ullamco laboris nisi ut aliquip ex ea\n"
            U"          commodo consequat.\n"
            U"\n"
            U"                    Lorem ipsum dolor sit amet,\n"
            U"          consectetur adipisicing elit.\n";
        TEST_EQUAL(str_wrap(s32, 0, 50, 20, 10), t32);
        TRY(str_wrap_in(s32, 0, 50, 20, 10));
        TEST_EQUAL(s32, t32);
        s32 =
            U"Lorem ipsum dolor sit amet,\r\n"
            U"consectetur adipisicing elit,\r\n"
            U"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            U"Ut enim ad minim veniam,\r\n"
            U"quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n";
        t32 =
            //....:....1....:....2....:....3....:....4....:....5
            U"Lorem ipsum dolor sit amet, consectetur\r\n"
            U"adipisicing elit, sed do eiusmod tempor\r\n"
            U"incididunt ut labore et dolore magna\r\n"
            U"aliqua. Ut enim ad minim veniam, quis\r\n"
            U"nostrud exercitation ullamco laboris\r\n"
            U"nisi ut aliquip ex ea commodo consequat.\r\n";
        TEST_EQUAL(str_wrap(s32, wrap_crlf, 40), t32);
        TRY(str_wrap_in(s32, wrap_crlf, 40));
        TEST_EQUAL(s32, t32);
        s32 =
            U"Lorem ipsum dolor sit amet,\r\n"
            U"consectetur adipisicing elit,\r\n"
            U"sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            U"\r\n"
            U"\r\n"
            U"    Lorem ipsum dolor sit amet, consectetur adipisicing elit,\r\n"
            U"    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
            U"\r\n"
            U"\r\n"
            U"Lorem ipsum dolor sit amet, consectetur adipisicing elit.\r\n";
        t32 =
            //....:....1....:....2....:....3....:....4....:....5
            U"Lorem ipsum dolor sit amet, consectetur\n"
            U"adipisicing elit, sed do eiusmod tempor\n"
            U"incididunt ut labore et dolore magna\n"
            U"aliqua.\n"
            U"\n"
            U"    Lorem ipsum dolor sit amet, consectetur adipisicing elit,\n"
            U"    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
            U"\n"
            U"Lorem ipsum dolor sit amet, consectetur\n"
            U"adipisicing elit.\n";
        TEST_EQUAL(str_wrap(s32, wrap_preserve, 40), t32);
        TRY(str_wrap_in(s32, wrap_preserve, 40));
        TEST_EQUAL(s32, t32);

    }

}

TEST_MODULE(unicorn, string_manipulation_s_z) {

    check_split();
    check_substring();
    check_translate();
    check_trim();
    check_trim_if();
    check_unify();
    check_wrap();

}
