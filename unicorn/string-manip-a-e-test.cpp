#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <string>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_manip_append() {

    // UTF-32    UTF-16     UTF-8
    // 0000004d  004d       4d
    // 00000430  0430       d0 b0
    // 00004e8c  4e8c       e4 ba 8c
    // 00010302  d800 df02  f0 90 8c 82
    // 0010fffd  dbff dffd  f4 8f bf bd

    const Ustring utf8_example {"\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};
    const std::u16string utf16_example {0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
    const std::u32string utf32_example {0x430,0x4e8c,0x10302,0x10fffd};

    Ustring s, t;
    std::u16string t16;
    std::u32string t32;

    s.clear();
    TRY(str_append(s, "Hello "));  TEST_EQUAL(s, "Hello ");
    TRY(str_append(s, "§¶ "));     TEST_EQUAL(s, "Hello §¶ ");
    TRY(str_append(s, "€urope"));  TEST_EQUAL(s, "Hello §¶ €urope");
    s.clear();
    TRY(str_append(s, u"Hello "));   TEST_EQUAL(s, "Hello ");
    TRY(str_append(s, u"§¶ "));      TEST_EQUAL(s, "Hello §¶ ");
    TRY(str_append(s, u"€urope"));   TEST_EQUAL(s, "Hello §¶ €urope");
    s.clear();
    TRY(str_append(s, U"Hello "));   TEST_EQUAL(s, "Hello ");
    TRY(str_append(s, U"§¶ "));      TEST_EQUAL(s, "Hello §¶ ");
    TRY(str_append(s, U"€urope"));   TEST_EQUAL(s, "Hello §¶ €urope");

    s.clear();
    t = "Hello ";  TRY(str_append(s, utf_range(t)));   TEST_EQUAL(s, "Hello ");
    t = "§¶ ";     TRY(str_append(s, utf_range(t)));   TEST_EQUAL(s, "Hello §¶ ");
    t = "€urope";  TRY(str_append(s, utf_range(t)));   TEST_EQUAL(s, "Hello §¶ €urope");
    s.clear();
    t16 = u"Hello ";  TRY(str_append(s, utf_range(t16)));  TEST_EQUAL(s, "Hello ");
    t16 = u"§¶ ";     TRY(str_append(s, utf_range(t16)));  TEST_EQUAL(s, "Hello §¶ ");
    t16 = u"€urope";  TRY(str_append(s, utf_range(t16)));  TEST_EQUAL(s, "Hello §¶ €urope");
    s.clear();
    t32 = U"Hello ";  TRY(str_append(s, utf_range(t32)));  TEST_EQUAL(s, "Hello ");
    t32 = U"§¶ ";     TRY(str_append(s, utf_range(t32)));  TEST_EQUAL(s, "Hello §¶ ");
    t32 = U"€urope";  TRY(str_append(s, utf_range(t32)));  TEST_EQUAL(s, "Hello §¶ €urope");

    s.clear();
    TRY(str_append_char(s, 'A'));  TEST_EQUAL(s, "A");
    TRY(str_append_char(s, 'B'));  TEST_EQUAL(s, "AB");
    TRY(str_append_char(s, 'C'));  TEST_EQUAL(s, "ABC");
    s.clear();
    TRY(str_append_char(s, 0x430));     TEST_EQUAL(s, "\xd0\xb0");
    TRY(str_append_char(s, 0x4e8c));    TEST_EQUAL(s, "\xd0\xb0\xe4\xba\x8c");
    TRY(str_append_char(s, 0x10302));   TEST_EQUAL(s, "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82");
    TRY(str_append_char(s, 0x10fffd));  TEST_EQUAL(s, utf8_example);

    s.clear();  TRY(str_append_char(s, 'A', 'B', 'C'));                     TEST_EQUAL(s, "ABC");
    s.clear();  TRY(str_append_char(s, 0x430, 0x4e8c, 0x10302, 0x10fffd));  TEST_EQUAL(s, utf8_example);

    s.clear();
    TRY(str_append_chars(s, 2, U'A'));  TEST_EQUAL(s, "AA");
    TRY(str_append_chars(s, 4, U'€'));  TEST_EQUAL(s, "AA€€€€");

}

void test_unicorn_string_manip_chars() {

    TEST_EQUAL(str_char(U'A'), "A");
    TEST_EQUAL(str_char(0x430), "\xd0\xb0");
    TEST_EQUAL(str_char(0x4e8c), "\xe4\xba\x8c");
    TEST_EQUAL(str_char(0x10302), "\xf0\x90\x8c\x82");
    TEST_EQUAL(str_char(0x10fffd), "\xf4\x8f\xbf\xbd");

    TEST_EQUAL(str_chars(3, U'A'), "AAA");
    TEST_EQUAL(str_chars(3, 0x430), "\xd0\xb0\xd0\xb0\xd0\xb0");
    TEST_EQUAL(str_chars(3, 0x4e8c), "\xe4\xba\x8c\xe4\xba\x8c\xe4\xba\x8c");
    TEST_EQUAL(str_chars(3, 0x10302), "\xf0\x90\x8c\x82\xf0\x90\x8c\x82\xf0\x90\x8c\x82");
    TEST_EQUAL(str_chars(3, 0x10fffd), "\xf4\x8f\xbf\xbd\xf4\x8f\xbf\xbd\xf4\x8f\xbf\xbd");

}

void test_unicorn_string_manip_concat() {

    TEST_EQUAL(str_concat(""), "");
    TEST_EQUAL(str_concat(u""), "");
    TEST_EQUAL(str_concat(U""), "");
    TEST_EQUAL(str_concat(""s), "");
    TEST_EQUAL(str_concat(u""s), "");
    TEST_EQUAL(str_concat(U""s), "");
    TEST_EQUAL(str_concat("Hello", " world"), "Hello world");
    TEST_EQUAL(str_concat("Hello", u" world"), "Hello world");
    TEST_EQUAL(str_concat("Hello", U" world"), "Hello world");
    TEST_EQUAL(str_concat(u"Hello", " world"), "Hello world");
    TEST_EQUAL(str_concat(u"Hello", u" world"), "Hello world");
    TEST_EQUAL(str_concat(u"Hello", U" world"), "Hello world");
    TEST_EQUAL(str_concat(U"Hello", " world"), "Hello world");
    TEST_EQUAL(str_concat(U"Hello", u" world"), "Hello world");
    TEST_EQUAL(str_concat(U"Hello", U" world"), "Hello world");
    TEST_EQUAL(str_concat("Hello", " world"s), "Hello world");
    TEST_EQUAL(str_concat("Hello", u" world"s), "Hello world");
    TEST_EQUAL(str_concat("Hello", U" world"s), "Hello world");
    TEST_EQUAL(str_concat(u"Hello", " world"s), "Hello world");
    TEST_EQUAL(str_concat(u"Hello", u" world"s), "Hello world");
    TEST_EQUAL(str_concat(u"Hello", U" world"s), "Hello world");
    TEST_EQUAL(str_concat(U"Hello", " world"s), "Hello world");
    TEST_EQUAL(str_concat(U"Hello", u" world"s), "Hello world");
    TEST_EQUAL(str_concat(U"Hello", U" world"s), "Hello world");
    TEST_EQUAL(str_concat("Hello"s, " world"), "Hello world");
    TEST_EQUAL(str_concat("Hello"s, u" world"), "Hello world");
    TEST_EQUAL(str_concat("Hello"s, U" world"), "Hello world");
    TEST_EQUAL(str_concat(u"Hello"s, " world"), "Hello world");
    TEST_EQUAL(str_concat(u"Hello"s, u" world"), "Hello world");
    TEST_EQUAL(str_concat(u"Hello"s, U" world"), "Hello world");
    TEST_EQUAL(str_concat(U"Hello"s, " world"), "Hello world");
    TEST_EQUAL(str_concat(U"Hello"s, u" world"), "Hello world");
    TEST_EQUAL(str_concat(U"Hello"s, U" world"), "Hello world");
    TEST_EQUAL(str_concat("Hello"s, " world"s), "Hello world");
    TEST_EQUAL(str_concat("Hello"s, u" world"s), "Hello world");
    TEST_EQUAL(str_concat("Hello"s, U" world"s), "Hello world");
    TEST_EQUAL(str_concat(u"Hello"s, " world"s), "Hello world");
    TEST_EQUAL(str_concat(u"Hello"s, u" world"s), "Hello world");
    TEST_EQUAL(str_concat(u"Hello"s, U" world"s), "Hello world");
    TEST_EQUAL(str_concat(U"Hello"s, " world"s), "Hello world");
    TEST_EQUAL(str_concat(U"Hello"s, u" world"s), "Hello world");
    TEST_EQUAL(str_concat(U"Hello"s, U" world"s), "Hello world");
    TEST_EQUAL(str_concat("Hello", " world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", " world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", u" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello", U" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", " world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", u" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello", U" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", " world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", u" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello", U" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world", "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world", u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world", U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world", "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world", u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world", U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world"s, "; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world"s, u"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world"s, U"; goodbye"), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, " world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, u" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat("Hello"s, U" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, " world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, u" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(u"Hello"s, U" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, " world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, u" world"s, U"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world"s, "; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world"s, u"; goodbye"s), "Hello world; goodbye");
    TEST_EQUAL(str_concat(U"Hello"s, U" world"s, U"; goodbye"s), "Hello world; goodbye");

    TEST_EQUAL(str_concat_with("++"), "");
    TEST_EQUAL(str_concat_with("++", "Hello"), "Hello");
    TEST_EQUAL(str_concat_with("++", "Hello", "world"), "Hello++world");
    TEST_EQUAL(str_concat_with("++", "Hello", "world", "goodbye"), "Hello++world++goodbye");

}

void test_unicorn_string_manip_drop() {

    Ustring s;

    TEST_EQUAL(str_drop_prefix(""s, ""), "");
    TEST_EQUAL(str_drop_prefix("Hello"s, ""), "Hello");
    TEST_EQUAL(str_drop_prefix(""s, "Hello"), "");
    TEST_EQUAL(str_drop_prefix("Hello"s, "Hello"), "");
    TEST_EQUAL(str_drop_prefix("Hello world"s, "Hello"), " world");
    TEST_EQUAL(str_drop_prefix(""s, ""s), "");
    TEST_EQUAL(str_drop_prefix("Hello"s, ""s), "Hello");
    TEST_EQUAL(str_drop_prefix(""s, "Hello"s), "");
    TEST_EQUAL(str_drop_prefix("Hello"s, "Hello"s), "");
    TEST_EQUAL(str_drop_prefix("Hello world"s, "Hello"s), " world");

    s = "";             TRY(str_drop_prefix_in(s, ""));        TEST_EQUAL(s, "");
    s = "Hello";        TRY(str_drop_prefix_in(s, ""));        TEST_EQUAL(s, "Hello");
    s = "";             TRY(str_drop_prefix_in(s, "Hello"));   TEST_EQUAL(s, "");
    s = "Hello";        TRY(str_drop_prefix_in(s, "Hello"));   TEST_EQUAL(s, "");
    s = "Hello world";  TRY(str_drop_prefix_in(s, "Hello"));   TEST_EQUAL(s, " world");
    s = "";             TRY(str_drop_prefix_in(s, ""s));       TEST_EQUAL(s, "");
    s = "Hello";        TRY(str_drop_prefix_in(s, ""s));       TEST_EQUAL(s, "Hello");
    s = "";             TRY(str_drop_prefix_in(s, "Hello"s));  TEST_EQUAL(s, "");
    s = "Hello";        TRY(str_drop_prefix_in(s, "Hello"s));  TEST_EQUAL(s, "");
    s = "Hello world";  TRY(str_drop_prefix_in(s, "Hello"s));  TEST_EQUAL(s, " world");

    TEST_EQUAL(str_drop_suffix(""s, ""), "");
    TEST_EQUAL(str_drop_suffix("Hello"s, ""), "Hello");
    TEST_EQUAL(str_drop_suffix(""s, "Hello"), "");
    TEST_EQUAL(str_drop_suffix("Hello"s, "Hello"), "");
    TEST_EQUAL(str_drop_suffix("Hello world"s, "world"), "Hello ");
    TEST_EQUAL(str_drop_suffix(""s, ""s), "");
    TEST_EQUAL(str_drop_suffix("Hello"s, ""s), "Hello");
    TEST_EQUAL(str_drop_suffix(""s, "Hello"s), "");
    TEST_EQUAL(str_drop_suffix("Hello"s, "Hello"s), "");
    TEST_EQUAL(str_drop_suffix("Hello world"s, "world"s), "Hello ");

    s = "";             TRY(str_drop_suffix_in(s, ""));        TEST_EQUAL(s, "");
    s = "Hello";        TRY(str_drop_suffix_in(s, ""));        TEST_EQUAL(s, "Hello");
    s = "";             TRY(str_drop_suffix_in(s, "Hello"));   TEST_EQUAL(s, "");
    s = "Hello";        TRY(str_drop_suffix_in(s, "Hello"));   TEST_EQUAL(s, "");
    s = "Hello world";  TRY(str_drop_suffix_in(s, "world"));   TEST_EQUAL(s, "Hello ");
    s = "";             TRY(str_drop_suffix_in(s, ""s));       TEST_EQUAL(s, "");
    s = "Hello";        TRY(str_drop_suffix_in(s, ""s));       TEST_EQUAL(s, "Hello");
    s = "Hello";        TRY(str_drop_suffix_in(s, "Hello"s));  TEST_EQUAL(s, "");
    s = "";             TRY(str_drop_suffix_in(s, "Hello"s));  TEST_EQUAL(s, "");
    s = "Hello world";  TRY(str_drop_suffix_in(s, "world"s));  TEST_EQUAL(s, "Hello ");

}

void test_unicorn_string_manip_erase() {

    Ustring s;
    std::u16string s16;
    std::u32string s32;

    TEST_EQUAL(str_erase_left("€uro"s, 0), "€uro");
    TEST_EQUAL(str_erase_left("€uro"s, 1), "uro");
    TEST_EQUAL(str_erase_left("€uro"s, 2), "ro");
    TEST_EQUAL(str_erase_left("€uro"s, 3), "o");
    TEST_EQUAL(str_erase_left("€uro"s, 4), "");
    TEST_EQUAL(str_erase_left("€uro"s, 5), "");

    s = "€uro";  TRY(str_erase_left_in(s, 0));  TEST_EQUAL(s, "€uro");
    s = "€uro";  TRY(str_erase_left_in(s, 1));  TEST_EQUAL(s, "uro");
    s = "€uro";  TRY(str_erase_left_in(s, 2));  TEST_EQUAL(s, "ro");
    s = "€uro";  TRY(str_erase_left_in(s, 3));  TEST_EQUAL(s, "o");
    s = "€uro";  TRY(str_erase_left_in(s, 4));  TEST_EQUAL(s, "");
    s = "€uro";  TRY(str_erase_left_in(s, 5));  TEST_EQUAL(s, "");

    TEST_EQUAL(str_erase_right("€uro"s, 0), "€uro");
    TEST_EQUAL(str_erase_right("€uro"s, 1), "€ur");
    TEST_EQUAL(str_erase_right("€uro"s, 2), "€u");
    TEST_EQUAL(str_erase_right("€uro"s, 3), "€");
    TEST_EQUAL(str_erase_right("€uro"s, 4), "");
    TEST_EQUAL(str_erase_right("€uro"s, 5), "");

    s = "€uro";  TRY(str_erase_right_in(s, 0));  TEST_EQUAL(s, "€uro");
    s = "€uro";  TRY(str_erase_right_in(s, 1));  TEST_EQUAL(s, "€ur");
    s = "€uro";  TRY(str_erase_right_in(s, 2));  TEST_EQUAL(s, "€u");
    s = "€uro";  TRY(str_erase_right_in(s, 3));  TEST_EQUAL(s, "€");
    s = "€uro";  TRY(str_erase_right_in(s, 4));  TEST_EQUAL(s, "");
    s = "€uro";  TRY(str_erase_right_in(s, 5));  TEST_EQUAL(s, "");

}

void test_unicorn_string_manip_expand() {

    std::vector<int> tabs {5,10,15,20};

    const Ustring a = "";
    const Ustring b = "Hello world\tGoodbye";
    const Ustring c = "ab\tcd\tef\tgh\tij\tkl\tmn\top";
    const Ustring d = "ab\tcd\nef\tgh\nij\tkl\nmn\top";
    const Ustring e = "abc\t\t\txyz";
    const Ustring f = "€uro\t∈lement\t∃";
    const Ustring g = "\tabc\txyz\t";

    Ustring s;

    TRY(s = str_expand_tabs(a));               TEST_EQUAL(s, "");
    TRY(s = str_expand_tabs(b));               TEST_EQUAL(s, "Hello world     Goodbye");
    TRY(s = str_expand_tabs(c));               TEST_EQUAL(s, "ab      cd      ef      gh      ij      kl      mn      op");
    TRY(s = str_expand_tabs(d));               TEST_EQUAL(s, "ab      cd\nef      gh\nij      kl\nmn      op");
    TRY(s = str_expand_tabs(e));               TEST_EQUAL(s, "abc                     xyz");
    TRY(s = str_expand_tabs(f));               TEST_EQUAL(s, "€uro    ∈lement ∃");
    TRY(s = str_expand_tabs(g));               TEST_EQUAL(s, "        abc     xyz     ");
    TRY(s = str_expand_tabs(a, tabs));         TEST_EQUAL(s, "");
    TRY(s = str_expand_tabs(b, tabs));         TEST_EQUAL(s, "Hello world    Goodbye");
    TRY(s = str_expand_tabs(c, tabs));         TEST_EQUAL(s, "ab   cd   ef   gh   ij   kl   mn   op");
    TRY(s = str_expand_tabs(d, tabs));         TEST_EQUAL(s, "ab   cd\nef   gh\nij   kl\nmn   op");
    TRY(s = str_expand_tabs(e, tabs));         TEST_EQUAL(s, "abc            xyz");
    TRY(s = str_expand_tabs(f, tabs));         TEST_EQUAL(s, "€uro ∈lement   ∃");
    TRY(s = str_expand_tabs(g, tabs));         TEST_EQUAL(s, "     abc  xyz  ");
    TRY(s = str_expand_tabs(a, {4,8,12,16}));  TEST_EQUAL(s, "");
    TRY(s = str_expand_tabs(b, {4,8,12,16}));  TEST_EQUAL(s, "Hello world Goodbye");
    TRY(s = str_expand_tabs(c, {4,8,12,16}));  TEST_EQUAL(s, "ab  cd  ef  gh  ij  kl  mn  op");
    TRY(s = str_expand_tabs(d, {4,8,12,16}));  TEST_EQUAL(s, "ab  cd\nef  gh\nij  kl\nmn  op");
    TRY(s = str_expand_tabs(e, {4,8,12,16}));  TEST_EQUAL(s, "abc         xyz");
    TRY(s = str_expand_tabs(f, {4,8,12,16}));  TEST_EQUAL(s, "€uro    ∈lement ∃");
    TRY(s = str_expand_tabs(g, {4,8,12,16}));  TEST_EQUAL(s, "    abc xyz ");

    s = a;  TRY(str_expand_tabs_in(s));               TEST_EQUAL(s, "");
    s = b;  TRY(str_expand_tabs_in(s));               TEST_EQUAL(s, "Hello world     Goodbye");
    s = c;  TRY(str_expand_tabs_in(s));               TEST_EQUAL(s, "ab      cd      ef      gh      ij      kl      mn      op");
    s = d;  TRY(str_expand_tabs_in(s));               TEST_EQUAL(s, "ab      cd\nef      gh\nij      kl\nmn      op");
    s = e;  TRY(str_expand_tabs_in(s));               TEST_EQUAL(s, "abc                     xyz");
    s = f;  TRY(str_expand_tabs_in(s));               TEST_EQUAL(s, "€uro    ∈lement ∃");
    s = g;  TRY(str_expand_tabs_in(s));               TEST_EQUAL(s, "        abc     xyz     ");
    s = a;  TRY(str_expand_tabs_in(s, tabs));         TEST_EQUAL(s, "");
    s = b;  TRY(str_expand_tabs_in(s, tabs));         TEST_EQUAL(s, "Hello world    Goodbye");
    s = c;  TRY(str_expand_tabs_in(s, tabs));         TEST_EQUAL(s, "ab   cd   ef   gh   ij   kl   mn   op");
    s = d;  TRY(str_expand_tabs_in(s, tabs));         TEST_EQUAL(s, "ab   cd\nef   gh\nij   kl\nmn   op");
    s = e;  TRY(str_expand_tabs_in(s, tabs));         TEST_EQUAL(s, "abc            xyz");
    s = f;  TRY(str_expand_tabs_in(s, tabs));         TEST_EQUAL(s, "€uro ∈lement   ∃");
    s = g;  TRY(str_expand_tabs_in(s, tabs));         TEST_EQUAL(s, "     abc  xyz  ");
    s = a;  TRY(str_expand_tabs_in(s, {4,8,12,16}));  TEST_EQUAL(s, "");
    s = b;  TRY(str_expand_tabs_in(s, {4,8,12,16}));  TEST_EQUAL(s, "Hello world Goodbye");
    s = c;  TRY(str_expand_tabs_in(s, {4,8,12,16}));  TEST_EQUAL(s, "ab  cd  ef  gh  ij  kl  mn  op");
    s = d;  TRY(str_expand_tabs_in(s, {4,8,12,16}));  TEST_EQUAL(s, "ab  cd\nef  gh\nij  kl\nmn  op");
    s = e;  TRY(str_expand_tabs_in(s, {4,8,12,16}));  TEST_EQUAL(s, "abc         xyz");
    s = f;  TRY(str_expand_tabs_in(s, {4,8,12,16}));  TEST_EQUAL(s, "€uro    ∈lement ∃");
    s = g;  TRY(str_expand_tabs_in(s, {4,8,12,16}));  TEST_EQUAL(s, "    abc xyz ");

}
