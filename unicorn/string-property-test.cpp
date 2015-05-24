#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

    // UTF-32    UTF-16     UTF-8
    // 0000004d  004d       4d
    // 00000430  0430       d0 b0
    // 00004e8c  4e8c       e4 ba 8c
    // 00010302  d800 df02  f0 90 8c 82
    // 0010fffd  dbff dffd  f4 8f bf bd

    const u8string utf8_example {"\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};
    const u8string utf8_reverse {"\xf4\x8f\xbf\xbd\xf0\x90\x8c\x82\xe4\xba\x8c\xd0\xb0"};
    const std::u16string utf16_example {0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
    const std::u16string utf16_reverse {0xdbff,0xdffd,0xd800,0xdf02,0x4e8c,0x430};
    const std::u32string utf32_example {0x430,0x4e8c,0x10302,0x10fffd};
    const std::u32string utf32_reverse {0x10fffd,0x10302,0x4e8c,0x430};

    void check_char_at() {

        TEST_EQUAL(str_char_at(""s, 0), 0);
        TEST_EQUAL(str_char_at(""s, 1), 0);
        TEST_EQUAL(str_char_at("Hello"s, 0), 'H');
        TEST_EQUAL(str_char_at("Hello"s, 1), 'e');
        TEST_EQUAL(str_char_at("Hello"s, 2), 'l');
        TEST_EQUAL(str_char_at("Hello"s, 3), 'l');
        TEST_EQUAL(str_char_at("Hello"s, 4), 'o');
        TEST_EQUAL(str_char_at("Hello"s, 5), 0);
        TEST_EQUAL(str_char_at("Hello"s, 6), 0);
        TEST_EQUAL(str_char_at(utf8_example, 0), 0x430);
        TEST_EQUAL(str_char_at(utf8_example, 1), 0x4e8c);
        TEST_EQUAL(str_char_at(utf8_example, 2), 0x10302);
        TEST_EQUAL(str_char_at(utf8_example, 3), 0x10fffd);
        TEST_EQUAL(str_char_at(utf8_example, 4), 0);
        TEST_EQUAL(str_char_at(utf8_example, 5), 0);

        TEST_EQUAL(str_char_at(u""s, 0), 0);
        TEST_EQUAL(str_char_at(u""s, 1), 0);
        TEST_EQUAL(str_char_at(u"Hello"s, 0), 'H');
        TEST_EQUAL(str_char_at(u"Hello"s, 1), 'e');
        TEST_EQUAL(str_char_at(u"Hello"s, 2), 'l');
        TEST_EQUAL(str_char_at(u"Hello"s, 3), 'l');
        TEST_EQUAL(str_char_at(u"Hello"s, 4), 'o');
        TEST_EQUAL(str_char_at(u"Hello"s, 5), 0);
        TEST_EQUAL(str_char_at(u"Hello"s, 6), 0);
        TEST_EQUAL(str_char_at(utf16_example, 0), 0x430);
        TEST_EQUAL(str_char_at(utf16_example, 1), 0x4e8c);
        TEST_EQUAL(str_char_at(utf16_example, 2), 0x10302);
        TEST_EQUAL(str_char_at(utf16_example, 3), 0x10fffd);
        TEST_EQUAL(str_char_at(utf16_example, 4), 0);
        TEST_EQUAL(str_char_at(utf16_example, 5), 0);

        TEST_EQUAL(str_char_at(U""s, 0), 0);
        TEST_EQUAL(str_char_at(U""s, 1), 0);
        TEST_EQUAL(str_char_at(U"Hello"s, 0), 'H');
        TEST_EQUAL(str_char_at(U"Hello"s, 1), 'e');
        TEST_EQUAL(str_char_at(U"Hello"s, 2), 'l');
        TEST_EQUAL(str_char_at(U"Hello"s, 3), 'l');
        TEST_EQUAL(str_char_at(U"Hello"s, 4), 'o');
        TEST_EQUAL(str_char_at(U"Hello"s, 5), 0);
        TEST_EQUAL(str_char_at(U"Hello"s, 6), 0);
        TEST_EQUAL(str_char_at(utf32_example, 0), 0x430);
        TEST_EQUAL(str_char_at(utf32_example, 1), 0x4e8c);
        TEST_EQUAL(str_char_at(utf32_example, 2), 0x10302);
        TEST_EQUAL(str_char_at(utf32_example, 3), 0x10fffd);
        TEST_EQUAL(str_char_at(utf32_example, 4), 0);
        TEST_EQUAL(str_char_at(utf32_example, 5), 0);

    }

    void check_first_and_last() {

        TEST_EQUAL(str_first_char(""s), 0);
        TEST_EQUAL(str_first_char("Hello"s), 'H');
        TEST_EQUAL(str_first_char(utf8_example), 0x430);
        TEST_EQUAL(str_first_char(utf8_reverse), 0x10fffd);

        TEST_EQUAL(str_first_char(u""s), 0);
        TEST_EQUAL(str_first_char(u"Hello"s), 'H');
        TEST_EQUAL(str_first_char(utf16_example), 0x430);
        TEST_EQUAL(str_first_char(utf16_reverse), 0x10fffd);

        TEST_EQUAL(str_first_char(U""s), 0);
        TEST_EQUAL(str_first_char(U"Hello"s), 'H');
        TEST_EQUAL(str_first_char(utf32_example), 0x430);
        TEST_EQUAL(str_first_char(utf32_reverse), 0x10fffd);

        TEST_EQUAL(str_last_char(""s), 0);
        TEST_EQUAL(str_last_char("Hello"s), 'o');
        TEST_EQUAL(str_last_char(utf8_example), 0x10fffd);
        TEST_EQUAL(str_last_char(utf8_reverse), 0x430);

        TEST_EQUAL(str_last_char(u""s), 0);
        TEST_EQUAL(str_last_char(u"Hello"s), 'o');
        TEST_EQUAL(str_last_char(utf16_example), 0x10fffd);
        TEST_EQUAL(str_last_char(utf16_reverse), 0x430);

        TEST_EQUAL(str_last_char(U""s), 0);
        TEST_EQUAL(str_last_char(U"Hello"s), 'o');
        TEST_EQUAL(str_last_char(utf32_example), 0x10fffd);
        TEST_EQUAL(str_last_char(utf32_reverse), 0x430);

    }

    void check_east_asian() {

        TEST(! str_is_east_asian(u8"ÀÀÀ"s)); // neutral
        TEST(! str_is_east_asian(u8"ààà"s)); // ambiguous
        TEST(str_is_east_asian(u8"\u3000\u3000\u3000"s)); // fullwidth
        TEST(str_is_east_asian(u8"\u20a9\u20a9\u20a9"s)); // halfwidth
        TEST(! str_is_east_asian(u8"AAA"s)); // narrow
        TEST(str_is_east_asian(u8"\u3001\u3001\u3001"s)); // wide
        TEST(! str_is_east_asian(u8"àààÀÀÀ"s)); // ambiguous + neutral
        TEST(str_is_east_asian(u8"ààà\u3000\u3000\u3000"s)); // ambiguous + fullwidth
        TEST(str_is_east_asian(u8"ààà\u20a9\u20a9\u20a9"s)); // ambiguous + halfwidth
        TEST(! str_is_east_asian(u8"àààAAA"s)); // ambiguous + narrow
        TEST(str_is_east_asian(u8"ààà\u3001\u3001\u3001"s)); // ambiguous + wide

        TEST(! str_is_east_asian(u"ÀÀÀ"s)); // neutral
        TEST(! str_is_east_asian(u"ààà"s)); // ambiguous
        TEST(str_is_east_asian(u"\u3000\u3000\u3000"s)); // fullwidth
        TEST(str_is_east_asian(u"\u20a9\u20a9\u20a9"s)); // halfwidth
        TEST(! str_is_east_asian(u"AAA"s)); // narrow
        TEST(str_is_east_asian(u"\u3001\u3001\u3001"s)); // wide
        TEST(! str_is_east_asian(u"àààÀÀÀ"s)); // ambiguous + neutral
        TEST(str_is_east_asian(u"ààà\u3000\u3000\u3000"s)); // ambiguous + fullwidth
        TEST(str_is_east_asian(u"ààà\u20a9\u20a9\u20a9"s)); // ambiguous + halfwidth
        TEST(! str_is_east_asian(u"àààAAA"s)); // ambiguous + narrow
        TEST(str_is_east_asian(u"ààà\u3001\u3001\u3001"s)); // ambiguous + wide

        TEST(! str_is_east_asian(U"ÀÀÀ"s)); // neutral
        TEST(! str_is_east_asian(U"ààà"s)); // ambiguous
        TEST(str_is_east_asian(U"\u3000\u3000\u3000"s)); // fullwidth
        TEST(str_is_east_asian(U"\u20a9\u20a9\u20a9"s)); // halfwidth
        TEST(! str_is_east_asian(U"AAA"s)); // narrow
        TEST(str_is_east_asian(U"\u3001\u3001\u3001"s)); // wide
        TEST(! str_is_east_asian(U"àààÀÀÀ"s)); // ambiguous + neutral
        TEST(str_is_east_asian(U"ààà\u3000\u3000\u3000"s)); // ambiguous + fullwidth
        TEST(str_is_east_asian(U"ààà\u20a9\u20a9\u20a9"s)); // ambiguous + halfwidth
        TEST(! str_is_east_asian(U"àààAAA"s)); // ambiguous + narrow
        TEST(str_is_east_asian(U"ààà\u3001\u3001\u3001"s)); // ambiguous + wide

    }

    void check_starts_and_ends() {

        TEST(str_starts_with(""s, ""s));
        TEST(str_starts_with("Hello world"s, ""s));
        TEST(str_starts_with("Hello world"s, "Hello"s));
        TEST(! str_starts_with("Hello world"s, "hello"s));
        TEST(! str_starts_with("Hello world"s, "world"s));
        TEST(! str_starts_with("Hello"s, "Hello world"s));
        TEST(str_starts_with(""s, ""));
        TEST(str_starts_with("Hello world"s, ""));
        TEST(str_starts_with("Hello world"s, "Hello"));
        TEST(! str_starts_with("Hello world"s, "hello"));
        TEST(! str_starts_with("Hello world"s, "world"));
        TEST(! str_starts_with("Hello"s, "Hello world"));

        TEST(str_starts_with(u""s, u""s));
        TEST(str_starts_with(u"Hello world"s, u""s));
        TEST(str_starts_with(u"Hello world"s, u"Hello"s));
        TEST(! str_starts_with(u"Hello world"s, u"hello"s));
        TEST(! str_starts_with(u"Hello world"s, u"world"s));
        TEST(! str_starts_with(u"Hello"s, u"Hello world"s));
        TEST(str_starts_with(u""s, u""));
        TEST(str_starts_with(u"Hello world"s, u""));
        TEST(str_starts_with(u"Hello world"s, u"Hello"));
        TEST(! str_starts_with(u"Hello world"s, u"hello"));
        TEST(! str_starts_with(u"Hello world"s, u"world"));
        TEST(! str_starts_with(u"Hello"s, u"Hello world"));

        TEST(str_starts_with(U""s, U""s));
        TEST(str_starts_with(U"Hello world"s, U""s));
        TEST(str_starts_with(U"Hello world"s, U"Hello"s));
        TEST(! str_starts_with(U"Hello world"s, U"hello"s));
        TEST(! str_starts_with(U"Hello world"s, U"world"s));
        TEST(! str_starts_with(U"Hello"s, U"Hello world"s));
        TEST(str_starts_with(U""s, U""));
        TEST(str_starts_with(U"Hello world"s, U""));
        TEST(str_starts_with(U"Hello world"s, U"Hello"));
        TEST(! str_starts_with(U"Hello world"s, U"hello"));
        TEST(! str_starts_with(U"Hello world"s, U"world"));
        TEST(! str_starts_with(U"Hello"s, U"Hello world"));

        TEST(str_ends_with(""s, ""s));
        TEST(str_ends_with("Hello world"s, ""s));
        TEST(str_ends_with("Hello world"s, "world"s));
        TEST(! str_ends_with("Hello world"s, "World"s));
        TEST(! str_ends_with("Hello world"s, "Hello"s));
        TEST(! str_ends_with("world"s, "Hello world"s));
        TEST(str_ends_with(""s, ""));
        TEST(str_ends_with("Hello world"s, ""));
        TEST(str_ends_with("Hello world"s, "world"));
        TEST(! str_ends_with("Hello world"s, "World"));
        TEST(! str_ends_with("Hello world"s, "Hello"));
        TEST(! str_ends_with("world"s, "Hello world"));

        TEST(str_ends_with(u""s, u""s));
        TEST(str_ends_with(u"Hello world"s, u""s));
        TEST(str_ends_with(u"Hello world"s, u"world"s));
        TEST(! str_ends_with(u"Hello world"s, u"World"s));
        TEST(! str_ends_with(u"Hello world"s, u"Hello"s));
        TEST(! str_ends_with(u"world"s, u"Hello world"s));
        TEST(str_ends_with(u""s, u""));
        TEST(str_ends_with(u"Hello world"s, u""));
        TEST(str_ends_with(u"Hello world"s, u"world"));
        TEST(! str_ends_with(u"Hello world"s, u"World"));
        TEST(! str_ends_with(u"Hello world"s, u"Hello"));
        TEST(! str_ends_with(u"world"s, u"Hello world"));

        TEST(str_ends_with(U""s, U""s));
        TEST(str_ends_with(U"Hello world"s, U""s));
        TEST(str_ends_with(U"Hello world"s, U"world"s));
        TEST(! str_ends_with(U"Hello world"s, U"World"s));
        TEST(! str_ends_with(U"Hello world"s, U"Hello"s));
        TEST(! str_ends_with(U"world"s, U"Hello world"s));
        TEST(str_ends_with(U""s, U""));
        TEST(str_ends_with(U"Hello world"s, U""));
        TEST(str_ends_with(U"Hello world"s, U"world"));
        TEST(! str_ends_with(U"Hello world"s, U"World"));
        TEST(! str_ends_with(U"Hello world"s, U"Hello"));
        TEST(! str_ends_with(U"world"s, U"Hello world"));

    }

}

TEST_MODULE(unicorn, string_property) {

    check_char_at();
    check_first_and_last();
    check_east_asian();
    check_starts_and_ends();

}
