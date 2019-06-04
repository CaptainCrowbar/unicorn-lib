#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <string>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

namespace {

    // UTF-32    UTF-16     UTF-8
    // 0000004d  004d       4d
    // 00000430  0430       d0 b0
    // 00004e8c  4e8c       e4 ba 8c
    // 00010302  d800 df02  f0 90 8c 82
    // 0010fffd  dbff dffd  f4 8f bf bd

    const Ustring utf8_example {"\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};
    const Ustring utf8_reverse {"\xf4\x8f\xbf\xbd\xf0\x90\x8c\x82\xe4\xba\x8c\xd0\xb0"};
    const std::u16string utf16_example {0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
    const std::u16string utf16_reverse {0xdbff,0xdffd,0xd800,0xdf02,0x4e8c,0x430};
    const std::u32string utf32_example {0x430,0x4e8c,0x10302,0x10fffd};
    const std::u32string utf32_reverse {0x10fffd,0x10302,0x4e8c,0x430};

}

void test_unicorn_string_property_char_at() {

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

void test_unicorn_string_property_first_and_last() {

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

void test_unicorn_string_property_east_asian() {

    TEST(! str_is_east_asian("ÀÀÀ"s));                  // neutral
    TEST(! str_is_east_asian("ààà"s));                  // ambiguous
    TEST(str_is_east_asian("\u3000\u3000\u3000"s));     // fullwidth
    TEST(str_is_east_asian("\u20a9\u20a9\u20a9"s));     // halfwidth
    TEST(! str_is_east_asian("AAA"s));                  // narrow
    TEST(str_is_east_asian("\u3001\u3001\u3001"s));     // wide
    TEST(! str_is_east_asian("àààÀÀÀ"s));               // ambiguous + neutral
    TEST(str_is_east_asian("ààà\u3000\u3000\u3000"s));  // ambiguous + fullwidth
    TEST(str_is_east_asian("ààà\u20a9\u20a9\u20a9"s));  // ambiguous + halfwidth
    TEST(! str_is_east_asian("àààAAA"s));               // ambiguous + narrow
    TEST(str_is_east_asian("ààà\u3001\u3001\u3001"s));  // ambiguous + wide

}

void test_unicorn_string_property_starts_and_ends() {

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

}
