#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <iterator>
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void check_append() {

        // UTF-32    UTF-16     UTF-8
        // 0000004d  004d       4d
        // 00000430  0430       d0 b0
        // 00004e8c  4e8c       e4 ba 8c
        // 00010302  d800 df02  f0 90 8c 82
        // 0010fffd  dbff dffd  f4 8f bf bd

        const u8string utf8_example {"\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};
        const u16string utf16_example {0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
        const u32string utf32_example {0x430,0x4e8c,0x10302,0x10fffd};

        u8string s8, t8;
        u16string s16, t16;
        u32string s32, t32;

        s8.clear();
        TRY(str_append(s8, u8"Hello "));  TEST_EQUAL(s8, u8"Hello ");
        TRY(str_append(s8, u8"€urope"));  TEST_EQUAL(s8, u8"Hello €urope");
        s8.clear();
        TRY(str_append(s8, u"Hello "));  TEST_EQUAL(s8, u8"Hello ");
        TRY(str_append(s8, u"€urope"));  TEST_EQUAL(s8, u8"Hello €urope");
        s8.clear();
        TRY(str_append(s8, U"Hello "));  TEST_EQUAL(s8, u8"Hello ");
        TRY(str_append(s8, U"€urope"));  TEST_EQUAL(s8, u8"Hello €urope");

        s16.clear();
        TRY(str_append(s16, u8"Hello "));  TEST_EQUAL(s16, u"Hello ");
        TRY(str_append(s16, u8"€urope"));  TEST_EQUAL(s16, u"Hello €urope");
        s16.clear();
        TRY(str_append(s16, u"Hello "));  TEST_EQUAL(s16, u"Hello ");
        TRY(str_append(s16, u"€urope"));  TEST_EQUAL(s16, u"Hello €urope");
        s16.clear();
        TRY(str_append(s16, U"Hello "));  TEST_EQUAL(s16, u"Hello ");
        TRY(str_append(s16, U"€urope"));  TEST_EQUAL(s16, u"Hello €urope");

        s32.clear();
        TRY(str_append(s32, u8"Hello "));  TEST_EQUAL(s32, U"Hello ");
        TRY(str_append(s32, u8"€urope"));  TEST_EQUAL(s32, U"Hello €urope");
        s32.clear();
        TRY(str_append(s32, u"Hello "));  TEST_EQUAL(s32, U"Hello ");
        TRY(str_append(s32, u"€urope"));  TEST_EQUAL(s32, U"Hello €urope");
        s32.clear();
        TRY(str_append(s32, U"Hello "));  TEST_EQUAL(s32, U"Hello ");
        TRY(str_append(s32, U"€urope"));  TEST_EQUAL(s32, U"Hello €urope");

        s8.clear();
        t8 = u8"Hello ";  TRY(str_append(s8, utf_range(t8)));  TEST_EQUAL(s8, u8"Hello ");
        t8 = u8"€urope";  TRY(str_append(s8, utf_range(t8)));  TEST_EQUAL(s8, u8"Hello €urope");
        s8.clear();
        t16 = u"Hello ";  TRY(str_append(s8, utf_range(t16)));  TEST_EQUAL(s8, u8"Hello ");
        t16 = u"€urope";  TRY(str_append(s8, utf_range(t16)));  TEST_EQUAL(s8, u8"Hello €urope");
        s8.clear();
        t32 = U"Hello ";  TRY(str_append(s8, utf_range(t32)));  TEST_EQUAL(s8, u8"Hello ");
        t32 = U"€urope";  TRY(str_append(s8, utf_range(t32)));  TEST_EQUAL(s8, u8"Hello €urope");

        s16.clear();
        t8 = u8"Hello ";  TRY(str_append(s16, utf_range(t8)));  TEST_EQUAL(s16, u"Hello ");
        t8 = u8"€urope";  TRY(str_append(s16, utf_range(t8)));  TEST_EQUAL(s16, u"Hello €urope");
        s16.clear();
        t16 = u"Hello ";  TRY(str_append(s16, utf_range(t16)));  TEST_EQUAL(s16, u"Hello ");
        t16 = u"€urope";  TRY(str_append(s16, utf_range(t16)));  TEST_EQUAL(s16, u"Hello €urope");
        s16.clear();
        t32 = U"Hello ";  TRY(str_append(s16, utf_range(t32)));  TEST_EQUAL(s16, u"Hello ");
        t32 = U"€urope";  TRY(str_append(s16, utf_range(t32)));  TEST_EQUAL(s16, u"Hello €urope");

        s32.clear();
        t8 = u8"Hello ";  TRY(str_append(s32, utf_range(t8)));  TEST_EQUAL(s32, U"Hello ");
        t8 = u8"€urope";  TRY(str_append(s32, utf_range(t8)));  TEST_EQUAL(s32, U"Hello €urope");
        s32.clear();
        t16 = u"Hello ";  TRY(str_append(s32, utf_range(t16)));  TEST_EQUAL(s32, U"Hello ");
        t16 = u"€urope";  TRY(str_append(s32, utf_range(t16)));  TEST_EQUAL(s32, U"Hello €urope");
        s32.clear();
        t32 = U"Hello ";  TRY(str_append(s32, utf_range(t32)));  TEST_EQUAL(s32, U"Hello ");
        t32 = U"€urope";  TRY(str_append(s32, utf_range(t32)));  TEST_EQUAL(s32, U"Hello €urope");

        s8.clear();
        TRY(str_append_char(s8, 'A'));  TEST_EQUAL(s8, "A");
        TRY(str_append_char(s8, 'B'));  TEST_EQUAL(s8, "AB");
        TRY(str_append_char(s8, 'C'));  TEST_EQUAL(s8, "ABC");
        s8.clear();
        TRY(str_append_char(s8, 0x430));     TEST_EQUAL(s8, "\xd0\xb0");
        TRY(str_append_char(s8, 0x4e8c));    TEST_EQUAL(s8, "\xd0\xb0\xe4\xba\x8c");
        TRY(str_append_char(s8, 0x10302));   TEST_EQUAL(s8, "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82");
        TRY(str_append_char(s8, 0x10fffd));  TEST_EQUAL(s8, utf8_example);

        s8.clear();  TRY(str_append_char(s8, 'A', 'B', 'C'));                     TEST_EQUAL(s8, "ABC");
        s8.clear();  TRY(str_append_char(s8, 0x430, 0x4e8c, 0x10302, 0x10fffd));  TEST_EQUAL(s8, utf8_example);

        s16.clear();
        TRY(str_append_char(s16, 'A'));  TEST_EQUAL(s16, u"A");
        TRY(str_append_char(s16, 'B'));  TEST_EQUAL(s16, u"AB");
        TRY(str_append_char(s16, 'C'));  TEST_EQUAL(s16, u"ABC");
        s16.clear();
        TRY(str_append_char(s16, 0x430));     TEST_EQUAL(s16, utf16_example.substr(0, 1));
        TRY(str_append_char(s16, 0x4e8c));    TEST_EQUAL(s16, utf16_example.substr(0, 2));
        TRY(str_append_char(s16, 0x10302));   TEST_EQUAL(s16, utf16_example.substr(0, 4));
        TRY(str_append_char(s16, 0x10fffd));  TEST_EQUAL(s16, utf16_example);

        s16.clear();  TRY(str_append_char(s16, 'A', 'B', 'C'));                     TEST_EQUAL(s16, u"ABC");
        s16.clear();  TRY(str_append_char(s16, 0x430, 0x4e8c, 0x10302, 0x10fffd));  TEST_EQUAL(s16, utf16_example);

        s32.clear();
        TRY(str_append_char(s32, 'A'));  TEST_EQUAL(s32, U"A");
        TRY(str_append_char(s32, 'B'));  TEST_EQUAL(s32, U"AB");
        TRY(str_append_char(s32, 'C'));  TEST_EQUAL(s32, U"ABC");
        s32.clear();
        TRY(str_append_char(s32, 0x430));     TEST_EQUAL(s32, utf32_example.substr(0, 1));
        TRY(str_append_char(s32, 0x4e8c));    TEST_EQUAL(s32, utf32_example.substr(0, 2));
        TRY(str_append_char(s32, 0x10302));   TEST_EQUAL(s32, utf32_example.substr(0, 3));
        TRY(str_append_char(s32, 0x10fffd));  TEST_EQUAL(s32, utf32_example);

        s32.clear();  TRY(str_append_char(s32, 'A', 'B', 'C'));                     TEST_EQUAL(s32, U"ABC");
        s32.clear();  TRY(str_append_char(s32, 0x430, 0x4e8c, 0x10302, 0x10fffd));  TEST_EQUAL(s32, utf32_example);

        s8.clear();
        TRY(str_append_chars(s8, U'A', 2));  TEST_EQUAL(s8, u8"AA");
        TRY(str_append_chars(s8, U'€', 4));  TEST_EQUAL(s8, u8"AA€€€€");
        s16.clear();
        TRY(str_append_chars(s16, U'A', 2));  TEST_EQUAL(s16, u"AA");
        TRY(str_append_chars(s16, U'€', 4));  TEST_EQUAL(s16, u"AA€€€€");
        s32.clear();
        TRY(str_append_chars(s32, U'A', 2));  TEST_EQUAL(s32, U"AA");
        TRY(str_append_chars(s32, U'€', 4));  TEST_EQUAL(s32, U"AA€€€€");

    }

    void check_change() {

        u8string s8, t8;
        u16string s16, t16;
        u32string s32, t32;
        Utf8Iterator i8, j8;
        Utf16Iterator i16, j16;
        Utf32Iterator i32, j32;

        s8 = u8"Hello world";
        t8 = u8"€urope";
        TRY(i8 = j8 = utf_begin(s8));
        TRY(std::advance(j8, 5));
        TEST_EQUAL(str_change(i8, j8, t8), u8"€urope world");
        TEST_EQUAL(str_change(irange(i8, j8), utf_range(t8)), u8"€urope world");
        TRY(i8 = utf_begin(s8));
        TRY(j8 = utf_end(s8));
        TRY(std::advance(i8, 6));
        TEST_EQUAL(str_change(i8, j8, t8), u8"Hello €urope");
        TEST_EQUAL(str_change(irange(i8, j8), utf_range(t8)), u8"Hello €urope");
        TEST_EQUAL(str_change(i8, i8, t8), u8"Hello €uropeworld");
        TEST_EQUAL(str_change(irange(i8, i8), utf_range(t8)), u8"Hello €uropeworld");
        TEST_EQUAL(str_change(utf_range(s8), t8), u8"€urope");
        TEST_EQUAL(str_change(utf_range(s8), utf_range(t8)), u8"€urope");
        s8 = u8"Hello world";
        TRY(i8 = j8 = utf_begin(s8));
        TRY(std::advance(j8, 5));
        TEST_EQUAL(u_str(str_change_in(s8, i8, j8, t8)), u8"€urope");
        TEST_EQUAL(s8, u8"€urope world");
        s8 = u8"Hello world";
        TRY(i8 = j8 = utf_begin(s8));
        TRY(std::advance(j8, 5));
        TEST_EQUAL(u_str(str_change_in(s8, irange(i8, j8), utf_range(t8))), u8"€urope");
        TEST_EQUAL(s8, u8"€urope world");
        s8 = u8"Hello world";
        TRY(i8 = utf_begin(s8));
        TRY(j8 = utf_end(s8));
        TRY(std::advance(i8, 6));
        TEST_EQUAL(u_str(str_change_in(s8, i8, j8, t8)), u8"€urope");
        TEST_EQUAL(s8, u8"Hello €urope");
        s8 = u8"Hello world";
        TRY(i8 = utf_begin(s8));
        TRY(j8 = utf_end(s8));
        TRY(std::advance(i8, 6));
        TEST_EQUAL(u_str(str_change_in(s8, irange(i8, j8), utf_range(t8))), u8"€urope");
        TEST_EQUAL(s8, u8"Hello €urope");
        s8 = u8"Hello world";
        TRY(i8 = utf_begin(s8));
        TRY(j8 = utf_end(s8));
        TRY(std::advance(i8, 6));
        TEST_EQUAL(u_str(str_change_in(s8, i8, i8, t8)), u8"€urope");
        TEST_EQUAL(s8, u8"Hello €uropeworld");
        s8 = u8"Hello world";
        TRY(i8 = utf_begin(s8));
        TRY(j8 = utf_end(s8));
        TRY(std::advance(i8, 6));
        TEST_EQUAL(u_str(str_change_in(s8, irange(i8, i8), utf_range(t8))), u8"€urope");
        TEST_EQUAL(s8, u8"Hello €uropeworld");
        s8 = u8"Hello world";
        TEST_EQUAL(u_str(str_change_in(s8, utf_range(s8), t8)), u8"€urope");
        TEST_EQUAL(s8, u8"€urope");
        s8 = u8"Hello world";
        TEST_EQUAL(u_str(str_change_in(s8, utf_range(s8), utf_range(t8))), u8"€urope");
        TEST_EQUAL(s8, u8"€urope");

        s16 = u"Hello world";
        t16 = u"€urope";
        TRY(i16 = j16 = utf_begin(s16));
        TRY(std::advance(j16, 5));
        TEST_EQUAL(str_change(i16, j16, t16), u"€urope world");
        TEST_EQUAL(str_change(irange(i16, j16), utf_range(t16)), u"€urope world");
        TRY(i16 = utf_begin(s16));
        TRY(j16 = utf_end(s16));
        TRY(std::advance(i16, 6));
        TEST_EQUAL(str_change(i16, j16, t16), u"Hello €urope");
        TEST_EQUAL(str_change(irange(i16, j16), utf_range(t16)), u"Hello €urope");
        TEST_EQUAL(str_change(i16, i16, t16), u"Hello €uropeworld");
        TEST_EQUAL(str_change(irange(i16, i16), utf_range(t16)), u"Hello €uropeworld");
        TEST_EQUAL(str_change(utf_range(s16), t16), u"€urope");
        TEST_EQUAL(str_change(utf_range(s16), utf_range(t16)), u"€urope");
        s16 = u"Hello world";
        TRY(i16 = j16 = utf_begin(s16));
        TRY(std::advance(j16, 5));
        TEST_EQUAL(u_str(str_change_in(s16, i16, j16, t16)), u"€urope");
        TEST_EQUAL(s16, u"€urope world");
        s16 = u"Hello world";
        TRY(i16 = j16 = utf_begin(s16));
        TRY(std::advance(j16, 5));
        TEST_EQUAL(u_str(str_change_in(s16, irange(i16, j16), utf_range(t16))), u"€urope");
        TEST_EQUAL(s16, u"€urope world");
        s16 = u"Hello world";
        TRY(i16 = utf_begin(s16));
        TRY(j16 = utf_end(s16));
        TRY(std::advance(i16, 6));
        TEST_EQUAL(u_str(str_change_in(s16, i16, j16, t16)), u"€urope");
        TEST_EQUAL(s16, u"Hello €urope");
        s16 = u"Hello world";
        TRY(i16 = utf_begin(s16));
        TRY(j16 = utf_end(s16));
        TRY(std::advance(i16, 6));
        TEST_EQUAL(u_str(str_change_in(s16, irange(i16, j16), utf_range(t16))), u"€urope");
        TEST_EQUAL(s16, u"Hello €urope");
        s16 = u"Hello world";
        TRY(i16 = utf_begin(s16));
        TRY(j16 = utf_end(s16));
        TRY(std::advance(i16, 6));
        TEST_EQUAL(u_str(str_change_in(s16, i16, i16, t16)), u"€urope");
        TEST_EQUAL(s16, u"Hello €uropeworld");
        s16 = u"Hello world";
        TRY(i16 = utf_begin(s16));
        TRY(j16 = utf_end(s16));
        TRY(std::advance(i16, 6));
        TEST_EQUAL(u_str(str_change_in(s16, irange(i16, i16), utf_range(t16))), u"€urope");
        TEST_EQUAL(s16, u"Hello €uropeworld");
        s16 = u"Hello world";
        TEST_EQUAL(u_str(str_change_in(s16, utf_range(s16), t16)), u"€urope");
        TEST_EQUAL(s16, u"€urope");
        s16 = u"Hello world";
        TEST_EQUAL(u_str(str_change_in(s16, utf_range(s16), utf_range(t16))), u"€urope");
        TEST_EQUAL(s16, u"€urope");

        s32 = U"Hello world";
        t32 = U"€urope";
        TRY(i32 = j32 = utf_begin(s32));
        TRY(std::advance(j32, 5));
        TEST_EQUAL(str_change(i32, j32, t32), U"€urope world");
        TEST_EQUAL(str_change(irange(i32, j32), utf_range(t32)), U"€urope world");
        TRY(i32 = utf_begin(s32));
        TRY(j32 = utf_end(s32));
        TRY(std::advance(i32, 6));
        TEST_EQUAL(str_change(i32, j32, t32), U"Hello €urope");
        TEST_EQUAL(str_change(irange(i32, j32), utf_range(t32)), U"Hello €urope");
        TEST_EQUAL(str_change(i32, i32, t32), U"Hello €uropeworld");
        TEST_EQUAL(str_change(irange(i32, i32), utf_range(t32)), U"Hello €uropeworld");
        TEST_EQUAL(str_change(utf_range(s32), t32), U"€urope");
        TEST_EQUAL(str_change(utf_range(s32), utf_range(t32)), U"€urope");
        s32 = U"Hello world";
        TRY(i32 = j32 = utf_begin(s32));
        TRY(std::advance(j32, 5));
        TEST_EQUAL(u_str(str_change_in(s32, i32, j32, t32)), U"€urope");
        TEST_EQUAL(s32, U"€urope world");
        s32 = U"Hello world";
        TRY(i32 = j32 = utf_begin(s32));
        TRY(std::advance(j32, 5));
        TEST_EQUAL(u_str(str_change_in(s32, irange(i32, j32), utf_range(t32))), U"€urope");
        TEST_EQUAL(s32, U"€urope world");
        s32 = U"Hello world";
        TRY(i32 = utf_begin(s32));
        TRY(j32 = utf_end(s32));
        TRY(std::advance(i32, 6));
        TEST_EQUAL(u_str(str_change_in(s32, i32, j32, t32)), U"€urope");
        TEST_EQUAL(s32, U"Hello €urope");
        s32 = U"Hello world";
        TRY(i32 = utf_begin(s32));
        TRY(j32 = utf_end(s32));
        TRY(std::advance(i32, 6));
        TEST_EQUAL(u_str(str_change_in(s32, irange(i32, j32), utf_range(t32))), U"€urope");
        TEST_EQUAL(s32, U"Hello €urope");
        s32 = U"Hello world";
        TRY(i32 = utf_begin(s32));
        TRY(j32 = utf_end(s32));
        TRY(std::advance(i32, 6));
        TEST_EQUAL(u_str(str_change_in(s32, i32, i32, t32)), U"€urope");
        TEST_EQUAL(s32, U"Hello €uropeworld");
        s32 = U"Hello world";
        TRY(i32 = utf_begin(s32));
        TRY(j32 = utf_end(s32));
        TRY(std::advance(i32, 6));
        TEST_EQUAL(u_str(str_change_in(s32, irange(i32, i32), utf_range(t32))), U"€urope");
        TEST_EQUAL(s32, U"Hello €uropeworld");
        s32 = U"Hello world";
        TEST_EQUAL(u_str(str_change_in(s32, utf_range(s32), t32)), U"€urope");
        TEST_EQUAL(s32, U"€urope");
        s32 = U"Hello world";
        TEST_EQUAL(u_str(str_change_in(s32, utf_range(s32), utf_range(t32))), U"€urope");
        TEST_EQUAL(s32, U"€urope");

    }

    void check_chars() {

        TEST_EQUAL(str_chars<char>('A'), "A");
        TEST_EQUAL(str_chars<char>(0x430), "\xd0\xb0");
        TEST_EQUAL(str_chars<char>(0x4e8c), "\xe4\xba\x8c");
        TEST_EQUAL(str_chars<char>(0x10302), "\xf0\x90\x8c\x82");
        TEST_EQUAL(str_chars<char>(0x10fffd), "\xf4\x8f\xbf\xbd");
        TEST_EQUAL(str_chars<char>('A', 3), "AAA");
        TEST_EQUAL(str_chars<char>(0x430, 3), "\xd0\xb0\xd0\xb0\xd0\xb0");
        TEST_EQUAL(str_chars<char>(0x4e8c, 3), "\xe4\xba\x8c\xe4\xba\x8c\xe4\xba\x8c");
        TEST_EQUAL(str_chars<char>(0x10302, 3), "\xf0\x90\x8c\x82\xf0\x90\x8c\x82\xf0\x90\x8c\x82");
        TEST_EQUAL(str_chars<char>(0x10fffd, 3), "\xf4\x8f\xbf\xbd\xf4\x8f\xbf\xbd\xf4\x8f\xbf\xbd");

        TEST_EQUAL(str_chars<char16_t>('A'), u"A");
        TEST_EQUAL(str_chars<char16_t>(0x430), u16string{0x0430});
        TEST_EQUAL(str_chars<char16_t>(0x4e8c), u16string{0x4e8c});
        TEST_EQUAL(str_chars<char16_t>(0x10302), (u16string{0xd800,0xdf02}));
        TEST_EQUAL(str_chars<char16_t>(0x10fffd), (u16string{0xdbff,0xdffd}));
        TEST_EQUAL(str_chars<char16_t>('A', 3), u"AAA");
        TEST_EQUAL(str_chars<char16_t>(0x430, 3), (u16string{0x0430,0x0430,0x0430}));
        TEST_EQUAL(str_chars<char16_t>(0x4e8c, 3), (u16string{0x4e8c,0x4e8c,0x4e8c}));
        TEST_EQUAL(str_chars<char16_t>(0x10302, 3), (u16string{0xd800,0xdf02,0xd800,0xdf02,0xd800,0xdf02}));
        TEST_EQUAL(str_chars<char16_t>(0x10fffd, 3), (u16string{0xdbff,0xdffd,0xdbff,0xdffd,0xdbff,0xdffd}));

        TEST_EQUAL(str_chars<char32_t>('A'), U"A");
        TEST_EQUAL(str_chars<char32_t>(0x430), u32string{0x430});
        TEST_EQUAL(str_chars<char32_t>(0x4e8c), u32string{0x4e8c});
        TEST_EQUAL(str_chars<char32_t>(0x10302), u32string{0x10302});
        TEST_EQUAL(str_chars<char32_t>(0x10fffd), u32string{0x10fffd});
        TEST_EQUAL(str_chars<char32_t>('A', 3), U"AAA");
        TEST_EQUAL(str_chars<char32_t>(0x430, 3), (u32string{0x430,0x430,0x430}));
        TEST_EQUAL(str_chars<char32_t>(0x4e8c, 3), (u32string{0x4e8c,0x4e8c,0x4e8c}));
        TEST_EQUAL(str_chars<char32_t>(0x10302, 3), (u32string{0x10302,0x10302,0x10302}));
        TEST_EQUAL(str_chars<char32_t>(0x10fffd, 3), (u32string{0x10fffd,0x10fffd,0x10fffd}));

    }

    void check_concat_1() {

        TEST_EQUAL(str_concat(""), "");
        TEST_EQUAL(str_concat(u""), u"");
        TEST_EQUAL(str_concat(U""), U"");
        TEST_EQUAL(str_concat(""s), "");
        TEST_EQUAL(str_concat(u""s), u"");
        TEST_EQUAL(str_concat(U""s), U"");
        TEST_EQUAL(str_concat("Hello", " world"), "Hello world");
        TEST_EQUAL(str_concat("Hello", u" world"), "Hello world");
        TEST_EQUAL(str_concat("Hello", U" world"), "Hello world");
        TEST_EQUAL(str_concat(u"Hello", " world"), u"Hello world");
        TEST_EQUAL(str_concat(u"Hello", u" world"), u"Hello world");
        TEST_EQUAL(str_concat(u"Hello", U" world"), u"Hello world");
        TEST_EQUAL(str_concat(U"Hello", " world"), U"Hello world");
        TEST_EQUAL(str_concat(U"Hello", u" world"), U"Hello world");
        TEST_EQUAL(str_concat(U"Hello", U" world"), U"Hello world");
        TEST_EQUAL(str_concat("Hello", " world"s), "Hello world");
        TEST_EQUAL(str_concat("Hello", u" world"s), "Hello world");
        TEST_EQUAL(str_concat("Hello", U" world"s), "Hello world");
        TEST_EQUAL(str_concat(u"Hello", " world"s), u"Hello world");
        TEST_EQUAL(str_concat(u"Hello", u" world"s), u"Hello world");
        TEST_EQUAL(str_concat(u"Hello", U" world"s), u"Hello world");
        TEST_EQUAL(str_concat(U"Hello", " world"s), U"Hello world");
        TEST_EQUAL(str_concat(U"Hello", u" world"s), U"Hello world");
        TEST_EQUAL(str_concat(U"Hello", U" world"s), U"Hello world");
        TEST_EQUAL(str_concat("Hello"s, " world"), "Hello world");
        TEST_EQUAL(str_concat("Hello"s, u" world"), "Hello world");
        TEST_EQUAL(str_concat("Hello"s, U" world"), "Hello world");
        TEST_EQUAL(str_concat(u"Hello"s, " world"), u"Hello world");
        TEST_EQUAL(str_concat(u"Hello"s, u" world"), u"Hello world");
        TEST_EQUAL(str_concat(u"Hello"s, U" world"), u"Hello world");
        TEST_EQUAL(str_concat(U"Hello"s, " world"), U"Hello world");
        TEST_EQUAL(str_concat(U"Hello"s, u" world"), U"Hello world");
        TEST_EQUAL(str_concat(U"Hello"s, U" world"), U"Hello world");
        TEST_EQUAL(str_concat("Hello"s, " world"s), "Hello world");
        TEST_EQUAL(str_concat("Hello"s, u" world"s), "Hello world");
        TEST_EQUAL(str_concat("Hello"s, U" world"s), "Hello world");
        TEST_EQUAL(str_concat(u"Hello"s, " world"s), u"Hello world");
        TEST_EQUAL(str_concat(u"Hello"s, u" world"s), u"Hello world");
        TEST_EQUAL(str_concat(u"Hello"s, U" world"s), u"Hello world");
        TEST_EQUAL(str_concat(U"Hello"s, " world"s), U"Hello world");
        TEST_EQUAL(str_concat(U"Hello"s, u" world"s), U"Hello world");
        TEST_EQUAL(str_concat(U"Hello"s, U" world"s), U"Hello world");

        TEST_EQUAL(str_concat_with("++"), "");
        TEST_EQUAL(str_concat_with("++", "Hello"), "Hello");
        TEST_EQUAL(str_concat_with("++", "Hello", "world"), "Hello++world");
        TEST_EQUAL(str_concat_with("++", "Hello", "world", "goodbye"), "Hello++world++goodbye");
        TEST_EQUAL(str_concat_with(u"++"), u"");
        TEST_EQUAL(str_concat_with(u"++", "Hello"), u"Hello");
        TEST_EQUAL(str_concat_with(u"++", "Hello", "world"), u"Hello++world");
        TEST_EQUAL(str_concat_with(u"++", "Hello", "world", "goodbye"), u"Hello++world++goodbye");
        TEST_EQUAL(str_concat_with(U"++"), U"");
        TEST_EQUAL(str_concat_with(U"++", "Hello"), U"Hello");
        TEST_EQUAL(str_concat_with(U"++", "Hello", "world"), U"Hello++world");
        TEST_EQUAL(str_concat_with(U"++", "Hello", "world", "goodbye"), U"Hello++world++goodbye");

    }

    void check_concat_2() {

        TEST_EQUAL(str_concat("Hello", " world", "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world", u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world", U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world", "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world", u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world", U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world", "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world", u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world", U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world", "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world", u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world", U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world", "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world", u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world", U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world", "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world", u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world", U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world", "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world", u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world", U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world", "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world", u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world", U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world", "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world", u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world", U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world", "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world", u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world", U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world", "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world", u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world", U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world", "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world", u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world", U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world", "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world", u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world", U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world", "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world", u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world", U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world", "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world", u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world", U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world", "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world", u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world", U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world", "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world", u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world", U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world", "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world", u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world", U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world"s, "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world"s, u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world"s, U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world"s, "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world"s, u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world"s, U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world"s, "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world"s, u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world"s, U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world"s, "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world"s, u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world"s, U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world"s, "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world"s, u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world"s, U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world"s, "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world"s, u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world"s, U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world"s, "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world"s, u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world"s, U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world"s, "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world"s, u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world"s, U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world"s, "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world"s, u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world"s, U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world"s, "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world"s, u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", " world"s, U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world"s, "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world"s, u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", u" world"s, U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world"s, "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world"s, u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello", U" world"s, U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world"s, "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world"s, u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", " world"s, U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world"s, "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world"s, u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", u" world"s, U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world"s, "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world"s, u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello", U" world"s, U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world"s, "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world"s, u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", " world"s, U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world"s, "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world"s, u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", u" world"s, U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world"s, "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world"s, u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello", U" world"s, U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world", "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world", u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world", U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world", "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world", u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world", U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world", "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world", u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world", U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world", "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world", u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world", U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world", "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world", u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world", U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world", "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world", u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world", U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world", "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world", u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world", U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world", "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world", u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world", U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world", "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world", u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world", U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world", "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world", u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world", U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world", "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world", u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world", U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world", "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world", u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world", U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world", "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world", u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world", U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world", "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world", u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world", U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world", "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world", u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world", U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world", "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world", u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world", U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world", "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world", u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world", U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world", "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world", u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world", U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world"s, "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world"s, u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world"s, U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world"s, "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world"s, u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world"s, U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world"s, "; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world"s, u"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world"s, U"; goodbye"), "Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world"s, "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world"s, u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world"s, U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world"s, "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world"s, u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world"s, U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world"s, "; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world"s, u"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world"s, U"; goodbye"), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world"s, "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world"s, u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world"s, U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world"s, "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world"s, u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world"s, U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world"s, "; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world"s, u"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world"s, U"; goodbye"), U"Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world"s, "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world"s, u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, " world"s, U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world"s, "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world"s, u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, u" world"s, U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world"s, "; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world"s, u"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat("Hello"s, U" world"s, U"; goodbye"s), "Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world"s, "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world"s, u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, " world"s, U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world"s, "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world"s, u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, u" world"s, U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world"s, "; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world"s, u"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(u"Hello"s, U" world"s, U"; goodbye"s), u"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world"s, "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world"s, u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, " world"s, U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world"s, "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world"s, u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, u" world"s, U"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world"s, "; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world"s, u"; goodbye"s), U"Hello world; goodbye");
        TEST_EQUAL(str_concat(U"Hello"s, U" world"s, U"; goodbye"s), U"Hello world; goodbye");

    }

    void check_drop() {

        u8string s8;
        u16string s16;
        u32string s32;

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

        TEST_EQUAL(str_drop_prefix(u""s, u""), u"");
        TEST_EQUAL(str_drop_prefix(u"Hello"s, u""), u"Hello");
        TEST_EQUAL(str_drop_prefix(u""s, u"Hello"), u"");
        TEST_EQUAL(str_drop_prefix(u"Hello"s, u"Hello"), u"");
        TEST_EQUAL(str_drop_prefix(u"Hello world"s, u"Hello"), u" world");
        TEST_EQUAL(str_drop_prefix(u""s, u""s), u"");
        TEST_EQUAL(str_drop_prefix(u"Hello"s, u""s), u"Hello");
        TEST_EQUAL(str_drop_prefix(u""s, u"Hello"s), u"");
        TEST_EQUAL(str_drop_prefix(u"Hello"s, u"Hello"s), u"");
        TEST_EQUAL(str_drop_prefix(u"Hello world"s, u"Hello"s), u" world");

        TEST_EQUAL(str_drop_prefix(U""s, U""), U"");
        TEST_EQUAL(str_drop_prefix(U"Hello"s, U""), U"Hello");
        TEST_EQUAL(str_drop_prefix(U""s, U"Hello"), U"");
        TEST_EQUAL(str_drop_prefix(U"Hello"s, U"Hello"), U"");
        TEST_EQUAL(str_drop_prefix(U"Hello world"s, U"Hello"), U" world");
        TEST_EQUAL(str_drop_prefix(U""s, U""s), U"");
        TEST_EQUAL(str_drop_prefix(U"Hello"s, U""s), U"Hello");
        TEST_EQUAL(str_drop_prefix(U""s, U"Hello"s), U"");
        TEST_EQUAL(str_drop_prefix(U"Hello"s, U"Hello"s), U"");
        TEST_EQUAL(str_drop_prefix(U"Hello world"s, U"Hello"s), U" world");

        s8 = "";             TRY(str_drop_prefix_in(s8, ""));        TEST_EQUAL(s8, "");
        s8 = "Hello";        TRY(str_drop_prefix_in(s8, ""));        TEST_EQUAL(s8, "Hello");
        s8 = "";             TRY(str_drop_prefix_in(s8, "Hello"));   TEST_EQUAL(s8, "");
        s8 = "Hello";        TRY(str_drop_prefix_in(s8, "Hello"));   TEST_EQUAL(s8, "");
        s8 = "Hello world";  TRY(str_drop_prefix_in(s8, "Hello"));   TEST_EQUAL(s8, " world");
        s8 = "";             TRY(str_drop_prefix_in(s8, ""s));       TEST_EQUAL(s8, "");
        s8 = "Hello";        TRY(str_drop_prefix_in(s8, ""s));       TEST_EQUAL(s8, "Hello");
        s8 = "";             TRY(str_drop_prefix_in(s8, "Hello"s));  TEST_EQUAL(s8, "");
        s8 = "Hello";        TRY(str_drop_prefix_in(s8, "Hello"s));  TEST_EQUAL(s8, "");
        s8 = "Hello world";  TRY(str_drop_prefix_in(s8, "Hello"s));  TEST_EQUAL(s8, " world");

        s16 = u"";             TRY(str_drop_prefix_in(s16, u""));        TEST_EQUAL(s16, u"");
        s16 = u"Hello";        TRY(str_drop_prefix_in(s16, u""));        TEST_EQUAL(s16, u"Hello");
        s16 = u"";             TRY(str_drop_prefix_in(s16, u"Hello"));   TEST_EQUAL(s16, u"");
        s16 = u"Hello";        TRY(str_drop_prefix_in(s16, u"Hello"));   TEST_EQUAL(s16, u"");
        s16 = u"Hello world";  TRY(str_drop_prefix_in(s16, u"Hello"));   TEST_EQUAL(s16, u" world");
        s16 = u"";             TRY(str_drop_prefix_in(s16, u""s));       TEST_EQUAL(s16, u"");
        s16 = u"Hello";        TRY(str_drop_prefix_in(s16, u""s));       TEST_EQUAL(s16, u"Hello");
        s16 = u"";             TRY(str_drop_prefix_in(s16, u"Hello"s));  TEST_EQUAL(s16, u"");
        s16 = u"Hello";        TRY(str_drop_prefix_in(s16, u"Hello"s));  TEST_EQUAL(s16, u"");
        s16 = u"Hello world";  TRY(str_drop_prefix_in(s16, u"Hello"s));  TEST_EQUAL(s16, u" world");

        s32 = U"";             TRY(str_drop_prefix_in(s32, U""));        TEST_EQUAL(s32, U"");
        s32 = U"Hello";        TRY(str_drop_prefix_in(s32, U""));        TEST_EQUAL(s32, U"Hello");
        s32 = U"";             TRY(str_drop_prefix_in(s32, U"Hello"));   TEST_EQUAL(s32, U"");
        s32 = U"Hello";        TRY(str_drop_prefix_in(s32, U"Hello"));   TEST_EQUAL(s32, U"");
        s32 = U"Hello world";  TRY(str_drop_prefix_in(s32, U"Hello"));   TEST_EQUAL(s32, U" world");
        s32 = U"";             TRY(str_drop_prefix_in(s32, U""s));       TEST_EQUAL(s32, U"");
        s32 = U"Hello";        TRY(str_drop_prefix_in(s32, U""s));       TEST_EQUAL(s32, U"Hello");
        s32 = U"";             TRY(str_drop_prefix_in(s32, U"Hello"s));  TEST_EQUAL(s32, U"");
        s32 = U"Hello";        TRY(str_drop_prefix_in(s32, U"Hello"s));  TEST_EQUAL(s32, U"");
        s32 = U"Hello world";  TRY(str_drop_prefix_in(s32, U"Hello"s));  TEST_EQUAL(s32, U" world");

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

        TEST_EQUAL(str_drop_suffix(u""s, u""), u"");
        TEST_EQUAL(str_drop_suffix(u"Hello"s, u""), u"Hello");
        TEST_EQUAL(str_drop_suffix(u""s, u"Hello"), u"");
        TEST_EQUAL(str_drop_suffix(u"Hello"s, u"Hello"), u"");
        TEST_EQUAL(str_drop_suffix(u"Hello world"s, u"world"), u"Hello ");
        TEST_EQUAL(str_drop_suffix(u""s, u""s), u"");
        TEST_EQUAL(str_drop_suffix(u"Hello"s, u""s), u"Hello");
        TEST_EQUAL(str_drop_suffix(u""s, u"Hello"s), u"");
        TEST_EQUAL(str_drop_suffix(u"Hello"s, u"Hello"s), u"");
        TEST_EQUAL(str_drop_suffix(u"Hello world"s, u"world"s), u"Hello ");

        TEST_EQUAL(str_drop_suffix(U""s, U""), U"");
        TEST_EQUAL(str_drop_suffix(U"Hello"s, U""), U"Hello");
        TEST_EQUAL(str_drop_suffix(U""s, U"Hello"), U"");
        TEST_EQUAL(str_drop_suffix(U"Hello"s, U"Hello"), U"");
        TEST_EQUAL(str_drop_suffix(U"Hello world"s, U"world"), U"Hello ");
        TEST_EQUAL(str_drop_suffix(U""s, U""s), U"");
        TEST_EQUAL(str_drop_suffix(U"Hello"s, U""s), U"Hello");
        TEST_EQUAL(str_drop_suffix(U""s, U"Hello"s), U"");
        TEST_EQUAL(str_drop_suffix(U"Hello"s, U"Hello"s), U"");
        TEST_EQUAL(str_drop_suffix(U"Hello world"s, U"world"s), U"Hello ");

        s8 = "";             TRY(str_drop_suffix_in(s8, ""));        TEST_EQUAL(s8, "");
        s8 = "Hello";        TRY(str_drop_suffix_in(s8, ""));        TEST_EQUAL(s8, "Hello");
        s8 = "";             TRY(str_drop_suffix_in(s8, "Hello"));   TEST_EQUAL(s8, "");
        s8 = "Hello";        TRY(str_drop_suffix_in(s8, "Hello"));   TEST_EQUAL(s8, "");
        s8 = "Hello world";  TRY(str_drop_suffix_in(s8, "world"));   TEST_EQUAL(s8, "Hello ");
        s8 = "";             TRY(str_drop_suffix_in(s8, ""s));       TEST_EQUAL(s8, "");
        s8 = "Hello";        TRY(str_drop_suffix_in(s8, ""s));       TEST_EQUAL(s8, "Hello");
        s8 = "Hello";        TRY(str_drop_suffix_in(s8, "Hello"s));  TEST_EQUAL(s8, "");
        s8 = "";             TRY(str_drop_suffix_in(s8, "Hello"s));  TEST_EQUAL(s8, "");
        s8 = "Hello world";  TRY(str_drop_suffix_in(s8, "world"s));  TEST_EQUAL(s8, "Hello ");

        s16 = u"";             TRY(str_drop_suffix_in(s16, u""));        TEST_EQUAL(s16, u"");
        s16 = u"Hello";        TRY(str_drop_suffix_in(s16, u""));        TEST_EQUAL(s16, u"Hello");
        s16 = u"";             TRY(str_drop_suffix_in(s16, u"Hello"));   TEST_EQUAL(s16, u"");
        s16 = u"Hello";        TRY(str_drop_suffix_in(s16, u"Hello"));   TEST_EQUAL(s16, u"");
        s16 = u"Hello world";  TRY(str_drop_suffix_in(s16, u"world"));   TEST_EQUAL(s16, u"Hello ");
        s16 = u"";             TRY(str_drop_suffix_in(s16, u""s));       TEST_EQUAL(s16, u"");
        s16 = u"Hello";        TRY(str_drop_suffix_in(s16, u""s));       TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello";        TRY(str_drop_suffix_in(s16, u"Hello"s));  TEST_EQUAL(s16, u"");
        s16 = u"";             TRY(str_drop_suffix_in(s16, u"Hello"s));  TEST_EQUAL(s16, u"");
        s16 = u"Hello world";  TRY(str_drop_suffix_in(s16, u"world"s));  TEST_EQUAL(s16, u"Hello ");

        s32 = U"";             TRY(str_drop_suffix_in(s32, U""));        TEST_EQUAL(s32, U"");
        s32 = U"Hello";        TRY(str_drop_suffix_in(s32, U""));        TEST_EQUAL(s32, U"Hello");
        s32 = U"";             TRY(str_drop_suffix_in(s32, U"Hello"));   TEST_EQUAL(s32, U"");
        s32 = U"Hello";        TRY(str_drop_suffix_in(s32, U"Hello"));   TEST_EQUAL(s32, U"");
        s32 = U"Hello world";  TRY(str_drop_suffix_in(s32, U"world"));   TEST_EQUAL(s32, U"Hello ");
        s32 = U"";             TRY(str_drop_suffix_in(s32, U""s));       TEST_EQUAL(s32, U"");
        s32 = U"Hello";        TRY(str_drop_suffix_in(s32, U""s));       TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello";        TRY(str_drop_suffix_in(s32, U"Hello"s));  TEST_EQUAL(s32, U"");
        s32 = U"";             TRY(str_drop_suffix_in(s32, U"Hello"s));  TEST_EQUAL(s32, U"");
        s32 = U"Hello world";  TRY(str_drop_suffix_in(s32, U"world"s));  TEST_EQUAL(s32, U"Hello ");

    }

    void check_erase() {

        u8string s8;
        u16string s16;
        u32string s32;

        TEST_EQUAL(str_erase_left(u8"€uro"s, 0), u8"€uro");
        TEST_EQUAL(str_erase_left(u8"€uro"s, 1), u8"uro");
        TEST_EQUAL(str_erase_left(u8"€uro"s, 2), u8"ro");
        TEST_EQUAL(str_erase_left(u8"€uro"s, 3), u8"o");
        TEST_EQUAL(str_erase_left(u8"€uro"s, 4), u8"");
        TEST_EQUAL(str_erase_left(u8"€uro"s, 5), u8"");

        TEST_EQUAL(str_erase_left(u"€uro"s, 0), u"€uro");
        TEST_EQUAL(str_erase_left(u"€uro"s, 1), u"uro");
        TEST_EQUAL(str_erase_left(u"€uro"s, 2), u"ro");
        TEST_EQUAL(str_erase_left(u"€uro"s, 3), u"o");
        TEST_EQUAL(str_erase_left(u"€uro"s, 4), u"");
        TEST_EQUAL(str_erase_left(u"€uro"s, 5), u"");

        TEST_EQUAL(str_erase_left(U"€uro"s, 0), U"€uro");
        TEST_EQUAL(str_erase_left(U"€uro"s, 1), U"uro");
        TEST_EQUAL(str_erase_left(U"€uro"s, 2), U"ro");
        TEST_EQUAL(str_erase_left(U"€uro"s, 3), U"o");
        TEST_EQUAL(str_erase_left(U"€uro"s, 4), U"");
        TEST_EQUAL(str_erase_left(U"€uro"s, 5), U"");

        s8 = u8"€uro";  TRY(str_erase_left_in(s8, 0));  TEST_EQUAL(s8, u8"€uro");
        s8 = u8"€uro";  TRY(str_erase_left_in(s8, 1));  TEST_EQUAL(s8, u8"uro");
        s8 = u8"€uro";  TRY(str_erase_left_in(s8, 2));  TEST_EQUAL(s8, u8"ro");
        s8 = u8"€uro";  TRY(str_erase_left_in(s8, 3));  TEST_EQUAL(s8, u8"o");
        s8 = u8"€uro";  TRY(str_erase_left_in(s8, 4));  TEST_EQUAL(s8, u8"");
        s8 = u8"€uro";  TRY(str_erase_left_in(s8, 5));  TEST_EQUAL(s8, u8"");

        s16 = u"€uro";  TRY(str_erase_left_in(s16, 0));  TEST_EQUAL(s16, u"€uro");
        s16 = u"€uro";  TRY(str_erase_left_in(s16, 1));  TEST_EQUAL(s16, u"uro");
        s16 = u"€uro";  TRY(str_erase_left_in(s16, 2));  TEST_EQUAL(s16, u"ro");
        s16 = u"€uro";  TRY(str_erase_left_in(s16, 3));  TEST_EQUAL(s16, u"o");
        s16 = u"€uro";  TRY(str_erase_left_in(s16, 4));  TEST_EQUAL(s16, u"");
        s16 = u"€uro";  TRY(str_erase_left_in(s16, 5));  TEST_EQUAL(s16, u"");

        s32 = U"€uro";  TRY(str_erase_left_in(s32, 0));  TEST_EQUAL(s32, U"€uro");
        s32 = U"€uro";  TRY(str_erase_left_in(s32, 1));  TEST_EQUAL(s32, U"uro");
        s32 = U"€uro";  TRY(str_erase_left_in(s32, 2));  TEST_EQUAL(s32, U"ro");
        s32 = U"€uro";  TRY(str_erase_left_in(s32, 3));  TEST_EQUAL(s32, U"o");
        s32 = U"€uro";  TRY(str_erase_left_in(s32, 4));  TEST_EQUAL(s32, U"");
        s32 = U"€uro";  TRY(str_erase_left_in(s32, 5));  TEST_EQUAL(s32, U"");

        TEST_EQUAL(str_erase_right("€uro"s, 0), "€uro");
        TEST_EQUAL(str_erase_right("€uro"s, 1), "€ur");
        TEST_EQUAL(str_erase_right("€uro"s, 2), "€u");
        TEST_EQUAL(str_erase_right("€uro"s, 3), "€");
        TEST_EQUAL(str_erase_right("€uro"s, 4), "");
        TEST_EQUAL(str_erase_right("€uro"s, 5), "");

        TEST_EQUAL(str_erase_right(u"€uro"s, 0), u"€uro");
        TEST_EQUAL(str_erase_right(u"€uro"s, 1), u"€ur");
        TEST_EQUAL(str_erase_right(u"€uro"s, 2), u"€u");
        TEST_EQUAL(str_erase_right(u"€uro"s, 3), u"€");
        TEST_EQUAL(str_erase_right(u"€uro"s, 4), u"");
        TEST_EQUAL(str_erase_right(u"€uro"s, 5), u"");

        TEST_EQUAL(str_erase_right(U"€uro"s, 0), U"€uro");
        TEST_EQUAL(str_erase_right(U"€uro"s, 1), U"€ur");
        TEST_EQUAL(str_erase_right(U"€uro"s, 2), U"€u");
        TEST_EQUAL(str_erase_right(U"€uro"s, 3), U"€");
        TEST_EQUAL(str_erase_right(U"€uro"s, 4), U"");
        TEST_EQUAL(str_erase_right(U"€uro"s, 5), U"");

        s8 = u8"€uro";  TRY(str_erase_right_in(s8, 0));  TEST_EQUAL(s8, u8"€uro");
        s8 = u8"€uro";  TRY(str_erase_right_in(s8, 1));  TEST_EQUAL(s8, u8"€ur");
        s8 = u8"€uro";  TRY(str_erase_right_in(s8, 2));  TEST_EQUAL(s8, u8"€u");
        s8 = u8"€uro";  TRY(str_erase_right_in(s8, 3));  TEST_EQUAL(s8, u8"€");
        s8 = u8"€uro";  TRY(str_erase_right_in(s8, 4));  TEST_EQUAL(s8, u8"");
        s8 = u8"€uro";  TRY(str_erase_right_in(s8, 5));  TEST_EQUAL(s8, u8"");

        s16 = u"€uro";  TRY(str_erase_right_in(s16, 0));  TEST_EQUAL(s16, u"€uro");
        s16 = u"€uro";  TRY(str_erase_right_in(s16, 1));  TEST_EQUAL(s16, u"€ur");
        s16 = u"€uro";  TRY(str_erase_right_in(s16, 2));  TEST_EQUAL(s16, u"€u");
        s16 = u"€uro";  TRY(str_erase_right_in(s16, 3));  TEST_EQUAL(s16, u"€");
        s16 = u"€uro";  TRY(str_erase_right_in(s16, 4));  TEST_EQUAL(s16, u"");
        s16 = u"€uro";  TRY(str_erase_right_in(s16, 5));  TEST_EQUAL(s16, u"");

        s32 = U"€uro";  TRY(str_erase_right_in(s32, 0));  TEST_EQUAL(s32, U"€uro");
        s32 = U"€uro";  TRY(str_erase_right_in(s32, 1));  TEST_EQUAL(s32, U"€ur");
        s32 = U"€uro";  TRY(str_erase_right_in(s32, 2));  TEST_EQUAL(s32, U"€u");
        s32 = U"€uro";  TRY(str_erase_right_in(s32, 3));  TEST_EQUAL(s32, U"€");
        s32 = U"€uro";  TRY(str_erase_right_in(s32, 4));  TEST_EQUAL(s32, U"");
        s32 = U"€uro";  TRY(str_erase_right_in(s32, 5));  TEST_EQUAL(s32, U"");

    }

    void check_expand() {

        std::vector<int> tabs {5,10,15,20};

        const u8string a8 = u8"";
        const u8string b8 = u8"Hello world\tGoodbye";
        const u8string c8 = u8"ab\tcd\tef\tgh\tij\tkl\tmn\top";
        const u8string d8 = u8"ab\tcd\nef\tgh\nij\tkl\nmn\top";
        const u8string e8 = u8"abc\t\t\txyz";
        const u8string f8 = u8"€uro\t∈lement\t∃";
        const u8string g8 = u8"\tabc\txyz\t";

        u8string s8;

        TRY(s8 = str_expand_tabs(a8));               TEST_EQUAL(s8, u8"");
        TRY(s8 = str_expand_tabs(b8));               TEST_EQUAL(s8, u8"Hello world     Goodbye");
        TRY(s8 = str_expand_tabs(c8));               TEST_EQUAL(s8, u8"ab      cd      ef      gh      ij      kl      mn      op");
        TRY(s8 = str_expand_tabs(d8));               TEST_EQUAL(s8, u8"ab      cd\nef      gh\nij      kl\nmn      op");
        TRY(s8 = str_expand_tabs(e8));               TEST_EQUAL(s8, u8"abc                     xyz");
        TRY(s8 = str_expand_tabs(f8));               TEST_EQUAL(s8, u8"€uro    ∈lement ∃");
        TRY(s8 = str_expand_tabs(g8));               TEST_EQUAL(s8, u8"        abc     xyz     ");
        TRY(s8 = str_expand_tabs(a8, tabs));         TEST_EQUAL(s8, u8"");
        TRY(s8 = str_expand_tabs(b8, tabs));         TEST_EQUAL(s8, u8"Hello world    Goodbye");
        TRY(s8 = str_expand_tabs(c8, tabs));         TEST_EQUAL(s8, u8"ab   cd   ef   gh   ij   kl   mn   op");
        TRY(s8 = str_expand_tabs(d8, tabs));         TEST_EQUAL(s8, u8"ab   cd\nef   gh\nij   kl\nmn   op");
        TRY(s8 = str_expand_tabs(e8, tabs));         TEST_EQUAL(s8, u8"abc            xyz");
        TRY(s8 = str_expand_tabs(f8, tabs));         TEST_EQUAL(s8, u8"€uro ∈lement   ∃");
        TRY(s8 = str_expand_tabs(g8, tabs));         TEST_EQUAL(s8, u8"     abc  xyz  ");
        TRY(s8 = str_expand_tabs(a8, {4,8,12,16}));  TEST_EQUAL(s8, u8"");
        TRY(s8 = str_expand_tabs(b8, {4,8,12,16}));  TEST_EQUAL(s8, u8"Hello world Goodbye");
        TRY(s8 = str_expand_tabs(c8, {4,8,12,16}));  TEST_EQUAL(s8, u8"ab  cd  ef  gh  ij  kl  mn  op");
        TRY(s8 = str_expand_tabs(d8, {4,8,12,16}));  TEST_EQUAL(s8, u8"ab  cd\nef  gh\nij  kl\nmn  op");
        TRY(s8 = str_expand_tabs(e8, {4,8,12,16}));  TEST_EQUAL(s8, u8"abc         xyz");
        TRY(s8 = str_expand_tabs(f8, {4,8,12,16}));  TEST_EQUAL(s8, u8"€uro    ∈lement ∃");
        TRY(s8 = str_expand_tabs(g8, {4,8,12,16}));  TEST_EQUAL(s8, u8"    abc xyz ");

        s8 = a8;  TRY(str_expand_tabs_in(s8));               TEST_EQUAL(s8, u8"");
        s8 = b8;  TRY(str_expand_tabs_in(s8));               TEST_EQUAL(s8, u8"Hello world     Goodbye");
        s8 = c8;  TRY(str_expand_tabs_in(s8));               TEST_EQUAL(s8, u8"ab      cd      ef      gh      ij      kl      mn      op");
        s8 = d8;  TRY(str_expand_tabs_in(s8));               TEST_EQUAL(s8, u8"ab      cd\nef      gh\nij      kl\nmn      op");
        s8 = e8;  TRY(str_expand_tabs_in(s8));               TEST_EQUAL(s8, u8"abc                     xyz");
        s8 = f8;  TRY(str_expand_tabs_in(s8));               TEST_EQUAL(s8, u8"€uro    ∈lement ∃");
        s8 = g8;  TRY(str_expand_tabs_in(s8));               TEST_EQUAL(s8, u8"        abc     xyz     ");
        s8 = a8;  TRY(str_expand_tabs_in(s8, tabs));         TEST_EQUAL(s8, u8"");
        s8 = b8;  TRY(str_expand_tabs_in(s8, tabs));         TEST_EQUAL(s8, u8"Hello world    Goodbye");
        s8 = c8;  TRY(str_expand_tabs_in(s8, tabs));         TEST_EQUAL(s8, u8"ab   cd   ef   gh   ij   kl   mn   op");
        s8 = d8;  TRY(str_expand_tabs_in(s8, tabs));         TEST_EQUAL(s8, u8"ab   cd\nef   gh\nij   kl\nmn   op");
        s8 = e8;  TRY(str_expand_tabs_in(s8, tabs));         TEST_EQUAL(s8, u8"abc            xyz");
        s8 = f8;  TRY(str_expand_tabs_in(s8, tabs));         TEST_EQUAL(s8, u8"€uro ∈lement   ∃");
        s8 = g8;  TRY(str_expand_tabs_in(s8, tabs));         TEST_EQUAL(s8, u8"     abc  xyz  ");
        s8 = a8;  TRY(str_expand_tabs_in(s8, {4,8,12,16}));  TEST_EQUAL(s8, u8"");
        s8 = b8;  TRY(str_expand_tabs_in(s8, {4,8,12,16}));  TEST_EQUAL(s8, u8"Hello world Goodbye");
        s8 = c8;  TRY(str_expand_tabs_in(s8, {4,8,12,16}));  TEST_EQUAL(s8, u8"ab  cd  ef  gh  ij  kl  mn  op");
        s8 = d8;  TRY(str_expand_tabs_in(s8, {4,8,12,16}));  TEST_EQUAL(s8, u8"ab  cd\nef  gh\nij  kl\nmn  op");
        s8 = e8;  TRY(str_expand_tabs_in(s8, {4,8,12,16}));  TEST_EQUAL(s8, u8"abc         xyz");
        s8 = f8;  TRY(str_expand_tabs_in(s8, {4,8,12,16}));  TEST_EQUAL(s8, u8"€uro    ∈lement ∃");
        s8 = g8;  TRY(str_expand_tabs_in(s8, {4,8,12,16}));  TEST_EQUAL(s8, u8"    abc xyz ");

        const u16string a16 = u"";
        const u16string b16 = u"Hello world\tGoodbye";
        const u16string c16 = u"ab\tcd\tef\tgh\tij\tkl\tmn\top";
        const u16string d16 = u"ab\tcd\nef\tgh\nij\tkl\nmn\top";
        const u16string e16 = u"abc\t\t\txyz";
        const u16string f16 = u"€uro\t∈lement\t∃";
        const u16string g16 = u"\tabc\txyz\t";

        u16string s16;

        TRY(s16 = str_expand_tabs(a16));               TEST_EQUAL(s16, u"");
        TRY(s16 = str_expand_tabs(b16));               TEST_EQUAL(s16, u"Hello world     Goodbye");
        TRY(s16 = str_expand_tabs(c16));               TEST_EQUAL(s16, u"ab      cd      ef      gh      ij      kl      mn      op");
        TRY(s16 = str_expand_tabs(d16));               TEST_EQUAL(s16, u"ab      cd\nef      gh\nij      kl\nmn      op");
        TRY(s16 = str_expand_tabs(e16));               TEST_EQUAL(s16, u"abc                     xyz");
        TRY(s16 = str_expand_tabs(f16));               TEST_EQUAL(s16, u"€uro    ∈lement ∃");
        TRY(s16 = str_expand_tabs(g16));               TEST_EQUAL(s16, u"        abc     xyz     ");
        TRY(s16 = str_expand_tabs(a16, tabs));         TEST_EQUAL(s16, u"");
        TRY(s16 = str_expand_tabs(b16, tabs));         TEST_EQUAL(s16, u"Hello world    Goodbye");
        TRY(s16 = str_expand_tabs(c16, tabs));         TEST_EQUAL(s16, u"ab   cd   ef   gh   ij   kl   mn   op");
        TRY(s16 = str_expand_tabs(d16, tabs));         TEST_EQUAL(s16, u"ab   cd\nef   gh\nij   kl\nmn   op");
        TRY(s16 = str_expand_tabs(e16, tabs));         TEST_EQUAL(s16, u"abc            xyz");
        TRY(s16 = str_expand_tabs(f16, tabs));         TEST_EQUAL(s16, u"€uro ∈lement   ∃");
        TRY(s16 = str_expand_tabs(g16, tabs));         TEST_EQUAL(s16, u"     abc  xyz  ");
        TRY(s16 = str_expand_tabs(a16, {4,8,12,16}));  TEST_EQUAL(s16, u"");
        TRY(s16 = str_expand_tabs(b16, {4,8,12,16}));  TEST_EQUAL(s16, u"Hello world Goodbye");
        TRY(s16 = str_expand_tabs(c16, {4,8,12,16}));  TEST_EQUAL(s16, u"ab  cd  ef  gh  ij  kl  mn  op");
        TRY(s16 = str_expand_tabs(d16, {4,8,12,16}));  TEST_EQUAL(s16, u"ab  cd\nef  gh\nij  kl\nmn  op");
        TRY(s16 = str_expand_tabs(e16, {4,8,12,16}));  TEST_EQUAL(s16, u"abc         xyz");
        TRY(s16 = str_expand_tabs(f16, {4,8,12,16}));  TEST_EQUAL(s16, u"€uro    ∈lement ∃");
        TRY(s16 = str_expand_tabs(g16, {4,8,12,16}));  TEST_EQUAL(s16, u"    abc xyz ");

        const u32string a32 = U"";
        const u32string b32 = U"Hello world\tGoodbye";
        const u32string c32 = U"ab\tcd\tef\tgh\tij\tkl\tmn\top";
        const u32string d32 = U"ab\tcd\nef\tgh\nij\tkl\nmn\top";
        const u32string e32 = U"abc\t\t\txyz";
        const u32string f32 = U"€uro\t∈lement\t∃";
        const u32string g32 = U"\tabc\txyz\t";

        u32string s32;

        TRY(s32 = str_expand_tabs(a32));               TEST_EQUAL(s32, U"");
        TRY(s32 = str_expand_tabs(b32));               TEST_EQUAL(s32, U"Hello world     Goodbye");
        TRY(s32 = str_expand_tabs(c32));               TEST_EQUAL(s32, U"ab      cd      ef      gh      ij      kl      mn      op");
        TRY(s32 = str_expand_tabs(d32));               TEST_EQUAL(s32, U"ab      cd\nef      gh\nij      kl\nmn      op");
        TRY(s32 = str_expand_tabs(e32));               TEST_EQUAL(s32, U"abc                     xyz");
        TRY(s32 = str_expand_tabs(f32));               TEST_EQUAL(s32, U"€uro    ∈lement ∃");
        TRY(s32 = str_expand_tabs(g32));               TEST_EQUAL(s32, U"        abc     xyz     ");
        TRY(s32 = str_expand_tabs(a32, tabs));         TEST_EQUAL(s32, U"");
        TRY(s32 = str_expand_tabs(b32, tabs));         TEST_EQUAL(s32, U"Hello world    Goodbye");
        TRY(s32 = str_expand_tabs(c32, tabs));         TEST_EQUAL(s32, U"ab   cd   ef   gh   ij   kl   mn   op");
        TRY(s32 = str_expand_tabs(d32, tabs));         TEST_EQUAL(s32, U"ab   cd\nef   gh\nij   kl\nmn   op");
        TRY(s32 = str_expand_tabs(e32, tabs));         TEST_EQUAL(s32, U"abc            xyz");
        TRY(s32 = str_expand_tabs(f32, tabs));         TEST_EQUAL(s32, U"€uro ∈lement   ∃");
        TRY(s32 = str_expand_tabs(g32, tabs));         TEST_EQUAL(s32, U"     abc  xyz  ");
        TRY(s32 = str_expand_tabs(a32, {4,8,12,16}));  TEST_EQUAL(s32, U"");
        TRY(s32 = str_expand_tabs(b32, {4,8,12,16}));  TEST_EQUAL(s32, U"Hello world Goodbye");
        TRY(s32 = str_expand_tabs(c32, {4,8,12,16}));  TEST_EQUAL(s32, U"ab  cd  ef  gh  ij  kl  mn  op");
        TRY(s32 = str_expand_tabs(d32, {4,8,12,16}));  TEST_EQUAL(s32, U"ab  cd\nef  gh\nij  kl\nmn  op");
        TRY(s32 = str_expand_tabs(e32, {4,8,12,16}));  TEST_EQUAL(s32, U"abc         xyz");
        TRY(s32 = str_expand_tabs(f32, {4,8,12,16}));  TEST_EQUAL(s32, U"€uro    ∈lement ∃");
        TRY(s32 = str_expand_tabs(g32, {4,8,12,16}));  TEST_EQUAL(s32, U"    abc xyz ");

    }

}

TEST_MODULE(unicorn, string_manipulation_a_e) {

    check_append();
    check_change();
    check_chars();
    check_concat_1();
    check_concat_2();
    check_drop();
    check_erase();
    check_expand();

}
