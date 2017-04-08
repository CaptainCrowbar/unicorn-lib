#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <algorithm>
#include <iterator>
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

    const U8string a8 {};
    const U8string b8 {"Hello"};
    const U8string c8 {"\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};
    const std::string x8 {"Hello\xffworld"};
    const U8string y8 {"Hello\xef\xbf\xbdworld"};
    const U8string z8 {"\0\0\0"s};
    const std::u16string a16 {};
    const std::u16string b16 {u"Hello"};
    const std::u16string c16 {0x4d,0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
    const std::u16string x16 {'H','e','l','l','o',0xd800,'w','o','r','l','d'};
    const std::u16string y16 {'H','e','l','l','o',0xfffd,'w','o','r','l','d'};
    const std::u16string z16 {0, 0, 0};
    const std::u32string a32 {};
    const std::u32string b32 {U"Hello"};
    const std::u32string c32 {0x4d,0x430,0x4e8c,0x10302,0x10fffd};
    const std::u32string x32 {'H','e','l','l','o',0xd800,'w','o','r','l','d'};
    const std::u32string y32 {'H','e','l','l','o',0xfffd,'w','o','r','l','d'};
    const std::u32string z32 {0, 0, 0};
    const std::wstring aw {};
    const std::wstring bw {L"Hello"};
    const std::wstring zw {0, 0, 0};

    #if defined(UNICORN_WCHAR_UTF16)
        const std::wstring cw(c16.begin(), c16.end());
        const std::wstring xw(x16.begin(), x16.end());
        const std::wstring yw(y16.begin(), y16.end());
    #else
        const std::wstring cw(c32.begin(), c32.end());
        const std::wstring xw(x32.begin(), x32.end());
        const std::wstring yw(y32.begin(), y32.end());
    #endif

    void check_basic_conversions() {

        // UTF-32    UTF-16     UTF-8
        // 0000004d  004d       4d
        // 00000430  0430       d0 b0
        // 00004e8c  4e8c       e4 ba 8c
        // 00010302  d800 df02  f0 90 8c 82
        // 0010fffd  dbff dffd  f4 8f bf bd

        using UnicornDetail::UtfEncoding;

        char buf8[10];
        char16_t buf16[10];
        char32_t buf32[10], u;
        wchar_t wbuf[10];

        { char a[] = "\0\0\0\0";          TEST_EQUAL(UtfEncoding<char>::decode(a, 5, u), 1);  TEST_EQUAL(u, 0); }
        { char a[] = "\x4d\0\0\0";        TEST_EQUAL(UtfEncoding<char>::decode(a, 5, u), 1);  TEST_EQUAL(u, 0x4d); }
        { char a[] = "\xd0\xb0\0\0";      TEST_EQUAL(UtfEncoding<char>::decode(a, 5, u), 2);  TEST_EQUAL(u, 0x430); }
        { char a[] = "\xe4\xba\x8c\0";    TEST_EQUAL(UtfEncoding<char>::decode(a, 5, u), 3);  TEST_EQUAL(u, 0x4e8c); }
        { char a[] = "\xf0\x90\x8c\x82";  TEST_EQUAL(UtfEncoding<char>::decode(a, 5, u), 4);  TEST_EQUAL(u, 0x10302); }
        { char a[] = "\xf4\x8f\xbf\xbd";  TEST_EQUAL(UtfEncoding<char>::decode(a, 5, u), 4);  TEST_EQUAL(u, 0x10fffd); }

        TEST_EQUAL(UtfEncoding<char>::encode(0, buf8), 1);         TEST_EQUAL(U8string(buf8, 1), "\0"s);
        TEST_EQUAL(UtfEncoding<char>::encode(0x4d, buf8), 1);      TEST_EQUAL(U8string(buf8, 1), "\x4d");
        TEST_EQUAL(UtfEncoding<char>::encode(0x430, buf8), 2);     TEST_EQUAL(U8string(buf8, 2), "\xd0\xb0");
        TEST_EQUAL(UtfEncoding<char>::encode(0x4e8c, buf8), 3);    TEST_EQUAL(U8string(buf8, 3), "\xe4\xba\x8c");
        TEST_EQUAL(UtfEncoding<char>::encode(0x10302, buf8), 4);   TEST_EQUAL(U8string(buf8, 4), "\xf0\x90\x8c\x82");
        TEST_EQUAL(UtfEncoding<char>::encode(0x10fffd, buf8), 4);  TEST_EQUAL(U8string(buf8, 4), "\xf4\x8f\xbf\xbd");

        { char16_t a[] {0x0000, 0};       TEST_EQUAL(UtfEncoding<char16_t>::decode(a, 3, u), 1);  TEST_EQUAL(u, 0); }
        { char16_t a[] {0x004d, 0};       TEST_EQUAL(UtfEncoding<char16_t>::decode(a, 3, u), 1);  TEST_EQUAL(u, 0x4d); }
        { char16_t a[] {0x0430, 0};       TEST_EQUAL(UtfEncoding<char16_t>::decode(a, 3, u), 1);  TEST_EQUAL(u, 0x430); }
        { char16_t a[] {0x4e8c, 0};       TEST_EQUAL(UtfEncoding<char16_t>::decode(a, 3, u), 1);  TEST_EQUAL(u, 0x4e8c); }
        { char16_t a[] {0xd800, 0xdf02};  TEST_EQUAL(UtfEncoding<char16_t>::decode(a, 3, u), 2);  TEST_EQUAL(u, 0x10302); }
        { char16_t a[] {0xdbff, 0xdffd};  TEST_EQUAL(UtfEncoding<char16_t>::decode(a, 3, u), 2);  TEST_EQUAL(u, 0x10fffd); }

        TEST_EQUAL(UtfEncoding<char16_t>::encode(0, buf16), 1);         TEST_EQUAL(buf16[0], 0);
        TEST_EQUAL(UtfEncoding<char16_t>::encode(0x4d, buf16), 1);      TEST_EQUAL(buf16[0], 0x4d);
        TEST_EQUAL(UtfEncoding<char16_t>::encode(0x430, buf16), 1);     TEST_EQUAL(buf16[0], 0x430);
        TEST_EQUAL(UtfEncoding<char16_t>::encode(0x4e8c, buf16), 1);    TEST_EQUAL(buf16[0], 0x4e8c);
        TEST_EQUAL(UtfEncoding<char16_t>::encode(0x10302, buf16), 2);   TEST_EQUAL(buf16[0], 0xd800);  TEST_EQUAL(buf16[1], 0xdf02);
        TEST_EQUAL(UtfEncoding<char16_t>::encode(0x10fffd, buf16), 2);  TEST_EQUAL(buf16[0], 0xdbff);  TEST_EQUAL(buf16[1], 0xdffd);

        { char32_t a[] {0, 0};         TEST_EQUAL(UtfEncoding<char32_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0); }
        { char32_t a[] {0x4d, 0};      TEST_EQUAL(UtfEncoding<char32_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x4d); }
        { char32_t a[] {0x430, 0};     TEST_EQUAL(UtfEncoding<char32_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x430); }
        { char32_t a[] {0x4e8c, 0};    TEST_EQUAL(UtfEncoding<char32_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x4e8c); }
        { char32_t a[] {0x10302, 0};   TEST_EQUAL(UtfEncoding<char32_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x10302); }
        { char32_t a[] {0x10fffd, 0};  TEST_EQUAL(UtfEncoding<char32_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x10fffd); }

        TEST_EQUAL(UtfEncoding<char32_t>::encode(0, buf32), 1);         TEST_EQUAL(buf32[0], 0);
        TEST_EQUAL(UtfEncoding<char32_t>::encode(0x4d, buf32), 1);      TEST_EQUAL(buf32[0], 0x4d);
        TEST_EQUAL(UtfEncoding<char32_t>::encode(0x430, buf32), 1);     TEST_EQUAL(buf32[0], 0x430);
        TEST_EQUAL(UtfEncoding<char32_t>::encode(0x4e8c, buf32), 1);    TEST_EQUAL(buf32[0], 0x4e8c);
        TEST_EQUAL(UtfEncoding<char32_t>::encode(0x10302, buf32), 1);   TEST_EQUAL(buf32[0], 0x10302);
        TEST_EQUAL(UtfEncoding<char32_t>::encode(0x10fffd, buf32), 1);  TEST_EQUAL(buf32[0], 0x10fffd);

        #if defined(UNICORN_WCHAR_UTF16)

            { wchar_t a[] {0x004d, 0};       TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 3, u), 1); }
            { wchar_t a[] {0x0430, 0};       TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 3, u), 1); }
            { wchar_t a[] {0x4e8c, 0};       TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 3, u), 1); }
            { wchar_t a[] {0xd800, 0xdf02};  TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 3, u), 2); }
            { wchar_t a[] {0xdbff, 0xdffd};  TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 3, u), 2); }

            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x4d, wbuf), 1);      TEST_EQUAL(wbuf[0], 0x004d);
            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x430, wbuf), 1);     TEST_EQUAL(wbuf[0], 0x0430);
            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x4e8c, wbuf), 1);    TEST_EQUAL(wbuf[0], 0x4e8c);
            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x10302, wbuf), 2);   TEST_EQUAL(wbuf[0], 0xd800); TEST_EQUAL(wbuf[1], 0xdf02);
            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x10fffd, wbuf), 2);  TEST_EQUAL(wbuf[0], 0xdbff); TEST_EQUAL(wbuf[1], 0xdffd);

        #else

            { wchar_t a[] {0x4d, 0};      TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x4d); }
            { wchar_t a[] {0x430, 0};     TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x430); }
            { wchar_t a[] {0x4e8c, 0};    TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x4e8c); }
            { wchar_t a[] {0x10302, 0};   TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x10302); }
            { wchar_t a[] {0x10fffd, 0};  TEST_EQUAL(UtfEncoding<wchar_t>::decode(a, 2, u), 1);  TEST_EQUAL(u, 0x10fffd); }

            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x4d, wbuf), 1);      TEST_EQUAL(wbuf[0], 0x4d);
            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x430, wbuf), 1);     TEST_EQUAL(wbuf[0], 0x430);
            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x4e8c, wbuf), 1);    TEST_EQUAL(wbuf[0], 0x4e8c);
            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x10302, wbuf), 1);   TEST_EQUAL(wbuf[0], 0x10302);
            TEST_EQUAL(UtfEncoding<wchar_t>::encode(0x10fffd, wbuf), 1);  TEST_EQUAL(wbuf[0], 0x10fffd);

        #endif

        { char a[] = "\0\0\0\0";          u = 0;  TEST_EQUAL(char_from_utf8(a, u), 1);  TEST_EQUAL(u, 0); }
        { char a[] = "\x4d\0\0\0";        u = 0;  TEST_EQUAL(char_from_utf8(a, u), 1);  TEST_EQUAL(u, 0x4d); }
        { char a[] = "\xd0\xb0\0\0";      u = 0;  TEST_EQUAL(char_from_utf8(a, u), 2);  TEST_EQUAL(u, 0x430); }
        { char a[] = "\xe4\xba\x8c\0";    u = 0;  TEST_EQUAL(char_from_utf8(a, u), 3);  TEST_EQUAL(u, 0x4e8c); }
        { char a[] = "\xf0\x90\x8c\x82";  u = 0;  TEST_EQUAL(char_from_utf8(a, u), 4);  TEST_EQUAL(u, 0x10302); }
        { char a[] = "\xf4\x8f\xbf\xbd";  u = 0;  TEST_EQUAL(char_from_utf8(a, u), 4);  TEST_EQUAL(u, 0x10fffd); }
        { char a[] = "\xff\xff\xff\xff";  u = 0;  TEST_EQUAL(char_from_utf8(a, u), 0);  TEST_EQUAL(u, 0); }

        TEST_EQUAL(char_to_utf8(0, buf8), 1);         TEST_EQUAL(U8string(buf8, 1), "\0"s);
        TEST_EQUAL(char_to_utf8(0x4d, buf8), 1);      TEST_EQUAL(U8string(buf8, 1), "\x4d");
        TEST_EQUAL(char_to_utf8(0x430, buf8), 2);     TEST_EQUAL(U8string(buf8, 2), "\xd0\xb0");
        TEST_EQUAL(char_to_utf8(0x4e8c, buf8), 3);    TEST_EQUAL(U8string(buf8, 3), "\xe4\xba\x8c");
        TEST_EQUAL(char_to_utf8(0x10302, buf8), 4);   TEST_EQUAL(U8string(buf8, 4), "\xf0\x90\x8c\x82");
        TEST_EQUAL(char_to_utf8(0x10fffd, buf8), 4);  TEST_EQUAL(U8string(buf8, 4), "\xf4\x8f\xbf\xbd");
        TEST_EQUAL(char_to_utf8(0xd800, buf8), 0);
        TEST_EQUAL(char_to_utf8(0x110000, buf8), 0);

        { char16_t a[] {0x0000, 0};       u = 0;  TEST_EQUAL(char_from_utf16(a, u), 1);  TEST_EQUAL(u, 0); }
        { char16_t a[] {0x004d, 0};       u = 0;  TEST_EQUAL(char_from_utf16(a, u), 1);  TEST_EQUAL(u, 0x4d); }
        { char16_t a[] {0x0430, 0};       u = 0;  TEST_EQUAL(char_from_utf16(a, u), 1);  TEST_EQUAL(u, 0x430); }
        { char16_t a[] {0x4e8c, 0};       u = 0;  TEST_EQUAL(char_from_utf16(a, u), 1);  TEST_EQUAL(u, 0x4e8c); }
        { char16_t a[] {0xd800, 0xdf02};  u = 0;  TEST_EQUAL(char_from_utf16(a, u), 2);  TEST_EQUAL(u, 0x10302); }
        { char16_t a[] {0xdbff, 0xdffd};  u = 0;  TEST_EQUAL(char_from_utf16(a, u), 2);  TEST_EQUAL(u, 0x10fffd); }
        { char16_t a[] {0xdffd, 0xdbff};  u = 0;  TEST_EQUAL(char_from_utf16(a, u), 0);  TEST_EQUAL(u, 0); }

        TEST_EQUAL(char_to_utf16(0, buf16), 1);         TEST_EQUAL(buf16[0], 0);
        TEST_EQUAL(char_to_utf16(0x4d, buf16), 1);      TEST_EQUAL(buf16[0], 0x4d);
        TEST_EQUAL(char_to_utf16(0x430, buf16), 1);     TEST_EQUAL(buf16[0], 0x430);
        TEST_EQUAL(char_to_utf16(0x4e8c, buf16), 1);    TEST_EQUAL(buf16[0], 0x4e8c);
        TEST_EQUAL(char_to_utf16(0x10302, buf16), 2);   TEST_EQUAL(buf16[0], 0xd800);  TEST_EQUAL(buf16[1], 0xdf02);
        TEST_EQUAL(char_to_utf16(0x10fffd, buf16), 2);  TEST_EQUAL(buf16[0], 0xdbff);  TEST_EQUAL(buf16[1], 0xdffd);
        TEST_EQUAL(char_to_utf16(0xd800, buf16), 0);
        TEST_EQUAL(char_to_utf16(0x110000, buf16), 0);

    }

    void check_basic_utilities() {

        using UnicornDetail::UtfEncoding;

        char buf8[10], c;
        char16_t buf16[10], t;
        char32_t buf32[10], u;
        wchar_t wbuf[10], w;

        for (char32_t c = 0; c <= last_unicode_char; ++c) {
            if (char_is_unicode(c)) {
                TEST_EQUAL(code_units<char>(c), UtfEncoding<char>::encode(c, buf8));
                TEST_EQUAL(code_units<char16_t>(c), UtfEncoding<char16_t>::encode(c, buf16));
                TEST_EQUAL(code_units<char32_t>(c), UtfEncoding<char32_t>::encode(c, buf32));
                TEST_EQUAL(code_units<wchar_t>(c), UtfEncoding<wchar_t>::encode(c, wbuf));
            }
        }

        c = '\x00';  TEST(is_single_unit(c));    TEST(! is_start_unit(c));  TEST(! is_nonstart_unit(c));  TEST(! is_invalid_unit(c));
        c = '\x7f';  TEST(is_single_unit(c));    TEST(! is_start_unit(c));  TEST(! is_nonstart_unit(c));  TEST(! is_invalid_unit(c));
        c = '\x80';  TEST(! is_single_unit(c));  TEST(! is_start_unit(c));  TEST(is_nonstart_unit(c));    TEST(! is_invalid_unit(c));
        c = '\xbf';  TEST(! is_single_unit(c));  TEST(! is_start_unit(c));  TEST(is_nonstart_unit(c));    TEST(! is_invalid_unit(c));
        c = '\xc0';  TEST(! is_single_unit(c));  TEST(! is_start_unit(c));  TEST(! is_nonstart_unit(c));  TEST(is_invalid_unit(c));
        c = '\xc1';  TEST(! is_single_unit(c));  TEST(! is_start_unit(c));  TEST(! is_nonstart_unit(c));  TEST(is_invalid_unit(c));
        c = '\xc2';  TEST(! is_single_unit(c));  TEST(is_start_unit(c));    TEST(! is_nonstart_unit(c));  TEST(! is_invalid_unit(c));
        c = '\xf4';  TEST(! is_single_unit(c));  TEST(is_start_unit(c));    TEST(! is_nonstart_unit(c));  TEST(! is_invalid_unit(c));
        c = '\xf5';  TEST(! is_single_unit(c));  TEST(! is_start_unit(c));  TEST(! is_nonstart_unit(c));  TEST(is_invalid_unit(c));
        c = '\xff';  TEST(! is_single_unit(c));  TEST(! is_start_unit(c));  TEST(! is_nonstart_unit(c));  TEST(is_invalid_unit(c));

        t = 0;       TEST(is_single_unit(t));    TEST(! is_start_unit(t));  TEST(! is_nonstart_unit(t));  TEST(! is_invalid_unit(t));
        t = 0xd7ff;  TEST(is_single_unit(t));    TEST(! is_start_unit(t));  TEST(! is_nonstart_unit(t));  TEST(! is_invalid_unit(t));
        t = 0xd800;  TEST(! is_single_unit(t));  TEST(is_start_unit(t));    TEST(! is_nonstart_unit(t));  TEST(! is_invalid_unit(t));
        t = 0xdbff;  TEST(! is_single_unit(t));  TEST(is_start_unit(t));    TEST(! is_nonstart_unit(t));  TEST(! is_invalid_unit(t));
        t = 0xdc00;  TEST(! is_single_unit(t));  TEST(! is_start_unit(t));  TEST(is_nonstart_unit(t));    TEST(! is_invalid_unit(t));
        t = 0xdfff;  TEST(! is_single_unit(t));  TEST(! is_start_unit(t));  TEST(is_nonstart_unit(t));    TEST(! is_invalid_unit(t));
        t = 0xe000;  TEST(is_single_unit(t));    TEST(! is_start_unit(t));  TEST(! is_nonstart_unit(t));  TEST(! is_invalid_unit(t));
        t = 0xffff;  TEST(is_single_unit(t));    TEST(! is_start_unit(t));  TEST(! is_nonstart_unit(t));  TEST(! is_invalid_unit(t));

        u = 0;       TEST(is_single_unit(u));    TEST(! is_start_unit(u));  TEST(! is_nonstart_unit(u));  TEST(! is_invalid_unit(u));
        u = 0xd7ff;  TEST(is_single_unit(u));    TEST(! is_start_unit(u));  TEST(! is_nonstart_unit(u));  TEST(! is_invalid_unit(u));
        u = 0xd800;  TEST(! is_single_unit(u));  TEST(! is_start_unit(u));  TEST(! is_nonstart_unit(u));  TEST(is_invalid_unit(u));
        u = 0xdfff;  TEST(! is_single_unit(u));  TEST(! is_start_unit(u));  TEST(! is_nonstart_unit(u));  TEST(is_invalid_unit(u));
        u = 0xe000;  TEST(is_single_unit(u));    TEST(! is_start_unit(u));  TEST(! is_nonstart_unit(u));  TEST(! is_invalid_unit(u));
        u = 0xffff;  TEST(is_single_unit(u));    TEST(! is_start_unit(u));  TEST(! is_nonstart_unit(u));  TEST(! is_invalid_unit(u));

        w = 0;       TEST(is_single_unit(w));    TEST(! is_start_unit(w));  TEST(! is_nonstart_unit(w));  TEST(! is_invalid_unit(w));
        w = 0xd7ff;  TEST(is_single_unit(w));    TEST(! is_start_unit(w));  TEST(! is_nonstart_unit(w));  TEST(! is_invalid_unit(w));

        if (sizeof(wchar_t) == 2) {

            w = 0xd800;  TEST(! is_single_unit(w));  TEST(is_start_unit(w));    TEST(! is_nonstart_unit(w));  TEST(! is_invalid_unit(w));
            w = 0xdbff;  TEST(! is_single_unit(w));  TEST(is_start_unit(w));    TEST(! is_nonstart_unit(w));  TEST(! is_invalid_unit(w));
            w = 0xdc00;  TEST(! is_single_unit(w));  TEST(! is_start_unit(w));  TEST(is_nonstart_unit(w));    TEST(! is_invalid_unit(w));
            w = 0xdfff;  TEST(! is_single_unit(w));  TEST(! is_start_unit(w));  TEST(is_nonstart_unit(w));    TEST(! is_invalid_unit(w));

        } else {

            w = 0xd800;  TEST(! is_single_unit(w));  TEST(! is_start_unit(w));  TEST(! is_nonstart_unit(w));  TEST(is_invalid_unit(w));
            w = 0xdfff;  TEST(! is_single_unit(w));  TEST(! is_start_unit(w));  TEST(! is_nonstart_unit(w));  TEST(is_invalid_unit(w));

        }

        w = 0xe000;  TEST(is_single_unit(w));    TEST(! is_start_unit(w));  TEST(! is_nonstart_unit(w));  TEST(! is_invalid_unit(w));
        w = 0xffff;  TEST(is_single_unit(w));    TEST(! is_start_unit(w));  TEST(! is_nonstart_unit(w));  TEST(! is_invalid_unit(w));

    }

    void check_decoding_iterators() {

        Utf8Iterator i8;
        Utf16Iterator i16;
        Utf32Iterator i32;
        WcharIterator iw;
        Utf8Range r8;
        Utf16Range r16;
        Utf32Range r32;
        WcharRange rw;

        TEST_EQUAL(i8.str(), "");
        TRY(r8 = utf_range(c8));
        TRY(i8 = r8.first);  TEST_EQUAL(*i8, 0x4d);      TEST_EQUAL(i8.str(), "\x4d");              TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\x4d");
        TRY(++i8);           TEST_EQUAL(*i8, 0x430);     TEST_EQUAL(i8.str(), "\xd0\xb0");          TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\xd0\xb0");
        TRY(++i8);           TEST_EQUAL(*i8, 0x4e8c);    TEST_EQUAL(i8.str(), "\xe4\xba\x8c");      TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\xe4\xba\x8c");
        TRY(++i8);           TEST_EQUAL(*i8, 0x10302);   TEST_EQUAL(i8.str(), "\xf0\x90\x8c\x82");  TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\xf0\x90\x8c\x82");
        TRY(++i8);           TEST_EQUAL(*i8, 0x10fffd);  TEST_EQUAL(i8.str(), "\xf4\x8f\xbf\xbd");  TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\xf4\x8f\xbf\xbd");
        TRY(++i8);           TEST(i8 == r8.second);      TEST_EQUAL(i8.str(), "");                  TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "");
        TRY(--i8);           TEST_EQUAL(*i8, 0x10fffd);  TEST_EQUAL(i8.str(), "\xf4\x8f\xbf\xbd");  TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\xf4\x8f\xbf\xbd");
        TRY(--i8);           TEST_EQUAL(*i8, 0x10302);   TEST_EQUAL(i8.str(), "\xf0\x90\x8c\x82");  TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\xf0\x90\x8c\x82");
        TRY(--i8);           TEST_EQUAL(*i8, 0x4e8c);    TEST_EQUAL(i8.str(), "\xe4\xba\x8c");      TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\xe4\xba\x8c");
        TRY(--i8);           TEST_EQUAL(*i8, 0x430);     TEST_EQUAL(i8.str(), "\xd0\xb0");          TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\xd0\xb0");
        TRY(--i8);           TEST_EQUAL(*i8, 0x4d);      TEST_EQUAL(i8.str(), "\x4d");              TEST_EQUAL(std::string(i8.range().begin(), i8.range().end()), "\x4d");
        TEST(i8 == r8.first);

        TEST_EQUAL(i16.str(), u"");
        TRY(r16 = utf_range(c16));
        TRY(i16 = r16.first);  TEST_EQUAL(*i16, 0x4d);      TEST_EQUAL(i16.str(), (std::u16string{0x004d}));          TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0x004d}));
        TRY(++i16);            TEST_EQUAL(*i16, 0x430);     TEST_EQUAL(i16.str(), (std::u16string{0x0430}));          TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0x0430}));
        TRY(++i16);            TEST_EQUAL(*i16, 0x4e8c);    TEST_EQUAL(i16.str(), (std::u16string{0x4e8c}));          TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0x4e8c}));
        TRY(++i16);            TEST_EQUAL(*i16, 0x10302);   TEST_EQUAL(i16.str(), (std::u16string{0xd800, 0xdf02}));  TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0xd800, 0xdf02}));
        TRY(++i16);            TEST_EQUAL(*i16, 0x10fffd);  TEST_EQUAL(i16.str(), (std::u16string{0xdbff, 0xdffd}));  TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0xdbff, 0xdffd}));
        TRY(++i16);            TEST(i16 == r16.second);     TEST_EQUAL(i16.str(), u"");                          TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), u"");
        TRY(--i16);            TEST_EQUAL(*i16, 0x10fffd);  TEST_EQUAL(i16.str(), (std::u16string{0xdbff, 0xdffd}));  TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0xdbff, 0xdffd}));
        TRY(--i16);            TEST_EQUAL(*i16, 0x10302);   TEST_EQUAL(i16.str(), (std::u16string{0xd800, 0xdf02}));  TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0xd800, 0xdf02}));
        TRY(--i16);            TEST_EQUAL(*i16, 0x4e8c);    TEST_EQUAL(i16.str(), (std::u16string{0x4e8c}));          TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0x4e8c}));
        TRY(--i16);            TEST_EQUAL(*i16, 0x430);     TEST_EQUAL(i16.str(), (std::u16string{0x0430}));          TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0x0430}));
        TRY(--i16);            TEST_EQUAL(*i16, 0x4d);      TEST_EQUAL(i16.str(), (std::u16string{0x004d}));          TEST_EQUAL(std::u16string(i16.range().begin(), i16.range().end()), (std::u16string{0x004d}));
        TEST(i16 == r16.first);

        TEST_EQUAL(i32.str(), U"");
        TRY(r32 = utf_range(c32));
        TRY(i32 = r32.first);  TEST_EQUAL(*i32, 0x4d);      TEST_EQUAL(i32.str(), std::u32string{0x0000004d});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x0000004d});
        TRY(++i32);            TEST_EQUAL(*i32, 0x430);     TEST_EQUAL(i32.str(), std::u32string{0x00000430});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x00000430});
        TRY(++i32);            TEST_EQUAL(*i32, 0x4e8c);    TEST_EQUAL(i32.str(), std::u32string{0x00004e8c});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x00004e8c});
        TRY(++i32);            TEST_EQUAL(*i32, 0x10302);   TEST_EQUAL(i32.str(), std::u32string{0x00010302});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x00010302});
        TRY(++i32);            TEST_EQUAL(*i32, 0x10fffd);  TEST_EQUAL(i32.str(), std::u32string{0x0010fffd});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x0010fffd});
        TRY(++i32);            TEST(i32 == r32.second);     TEST_EQUAL(i32.str(), U"");                    TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), U"");
        TRY(--i32);            TEST_EQUAL(*i32, 0x10fffd);  TEST_EQUAL(i32.str(), std::u32string{0x0010fffd});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x0010fffd});
        TRY(--i32);            TEST_EQUAL(*i32, 0x10302);   TEST_EQUAL(i32.str(), std::u32string{0x00010302});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x00010302});
        TRY(--i32);            TEST_EQUAL(*i32, 0x4e8c);    TEST_EQUAL(i32.str(), std::u32string{0x00004e8c});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x00004e8c});
        TRY(--i32);            TEST_EQUAL(*i32, 0x430);     TEST_EQUAL(i32.str(), std::u32string{0x00000430});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x00000430});
        TRY(--i32);            TEST_EQUAL(*i32, 0x4d);      TEST_EQUAL(i32.str(), std::u32string{0x0000004d});  TEST_EQUAL(std::u32string(i32.range().begin(), i32.range().end()), std::u32string{0x0000004d});
        TEST(i32 == r32.first);

        TRY(rw = utf_range(cw));
        TRY(iw = rw.first);  TEST_EQUAL(*iw, 0x4d);
        TRY(++iw);           TEST_EQUAL(*iw, 0x430);
        TRY(++iw);           TEST_EQUAL(*iw, 0x4e8c);
        TRY(++iw);           TEST_EQUAL(*iw, 0x10302);
        TRY(++iw);           TEST_EQUAL(*iw, 0x10fffd);
        TRY(++iw);           TEST(iw == rw.second);
        TRY(--iw);           TEST_EQUAL(*iw, 0x10fffd);
        TRY(--iw);           TEST_EQUAL(*iw, 0x10302);
        TRY(--iw);           TEST_EQUAL(*iw, 0x4e8c);
        TRY(--iw);           TEST_EQUAL(*iw, 0x430);
        TRY(--iw);           TEST_EQUAL(*iw, 0x4d);
        TEST(iw == rw.first);

    }

    void check_decoding_ranges() {

        U8string s8;
        std::u16string s16;
        std::u32string s32;
        std::wstring sw;
        Utf8Range r8;
        Utf16Range r16;
        Utf32Range r32;
        WcharRange rw;

        TRY(r8 = utf_range(a8));    TRY(std::copy(r8.begin(), r8.end(), overwrite(s32)));   TEST_EQUAL(s32, U"");
        TRY(r8 = utf_range(b8));    TRY(std::copy(r8.begin(), r8.end(), overwrite(s32)));   TEST_EQUAL(s32, U"Hello");
        TRY(r8 = utf_range(c8));    TRY(std::copy(r8.begin(), r8.end(), overwrite(s32)));   TEST_EQUAL(s32, c32);
        TRY(r8 = utf_range(z8));    TRY(std::copy(r8.begin(), r8.end(), overwrite(s32)));   TEST_EQUAL(s32, z32);
        TRY(r16 = utf_range(a16));  TRY(std::copy(r16.begin(), r16.end(), overwrite(s32)));  TEST_EQUAL(s32, U"");
        TRY(r16 = utf_range(b16));  TRY(std::copy(r16.begin(), r16.end(), overwrite(s32)));  TEST_EQUAL(s32, U"Hello");
        TRY(r16 = utf_range(c16));  TRY(std::copy(r16.begin(), r16.end(), overwrite(s32)));  TEST_EQUAL(s32, c32);
        TRY(r16 = utf_range(z16));  TRY(std::copy(r16.begin(), r16.end(), overwrite(s32)));  TEST_EQUAL(s32, z32);
        TRY(r32 = utf_range(a32));  TRY(std::copy(r32.begin(), r32.end(), overwrite(s32)));  TEST_EQUAL(s32, U"");
        TRY(r32 = utf_range(b32));  TRY(std::copy(r32.begin(), r32.end(), overwrite(s32)));  TEST_EQUAL(s32, U"Hello");
        TRY(r32 = utf_range(c32));  TRY(std::copy(r32.begin(), r32.end(), overwrite(s32)));  TEST_EQUAL(s32, c32);
        TRY(r32 = utf_range(z32));  TRY(std::copy(r32.begin(), r32.end(), overwrite(s32)));  TEST_EQUAL(s32, z32);
        TRY(rw = utf_range(aw));    TRY(std::copy(rw.begin(), rw.end(), overwrite(s32)));   TEST_EQUAL(s32, U"");
        TRY(rw = utf_range(bw));    TRY(std::copy(rw.begin(), rw.end(), overwrite(s32)));   TEST_EQUAL(s32, U"Hello");
        TRY(rw = utf_range(cw));    TRY(std::copy(rw.begin(), rw.end(), overwrite(s32)));   TEST_EQUAL(s32, c32);

        TRY(std::copy(utf_begin(a8), utf_end(a8), overwrite(s32)));    TEST_EQUAL(s32, U"");
        TRY(std::copy(utf_begin(b8), utf_end(b8), overwrite(s32)));    TEST_EQUAL(s32, U"Hello");
        TRY(std::copy(utf_begin(c8), utf_end(c8), overwrite(s32)));    TEST_EQUAL(s32, c32);
        TRY(std::copy(utf_begin(z8), utf_end(z8), overwrite(s32)));    TEST_EQUAL(s32, z32);
        TRY(std::copy(utf_begin(a16), utf_end(a16), overwrite(s32)));  TEST_EQUAL(s32, U"");
        TRY(std::copy(utf_begin(b16), utf_end(b16), overwrite(s32)));  TEST_EQUAL(s32, U"Hello");
        TRY(std::copy(utf_begin(c16), utf_end(c16), overwrite(s32)));  TEST_EQUAL(s32, c32);
        TRY(std::copy(utf_begin(z16), utf_end(z16), overwrite(s32)));  TEST_EQUAL(s32, z32);
        TRY(std::copy(utf_begin(a32), utf_end(a32), overwrite(s32)));  TEST_EQUAL(s32, U"");
        TRY(std::copy(utf_begin(b32), utf_end(b32), overwrite(s32)));  TEST_EQUAL(s32, U"Hello");
        TRY(std::copy(utf_begin(c32), utf_end(c32), overwrite(s32)));  TEST_EQUAL(s32, c32);
        TRY(std::copy(utf_begin(z32), utf_end(z32), overwrite(s32)));  TEST_EQUAL(s32, z32);
        TRY(std::copy(utf_begin(aw), utf_end(aw), overwrite(s32)));    TEST_EQUAL(s32, U"");
        TRY(std::copy(utf_begin(bw), utf_end(bw), overwrite(s32)));    TEST_EQUAL(s32, U"Hello");
        TRY(std::copy(utf_begin(cw), utf_end(cw), overwrite(s32)));    TEST_EQUAL(s32, c32);

        TRY(std::copy(utf_iterator(c8, 0), utf_iterator(c8, 14), overwrite(s32)));   TEST_EQUAL(s32, (std::u32string{0x4d,0x430,0x4e8c,0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c8, 0), utf_iterator(c8, 1), overwrite(s32)));    TEST_EQUAL(s32, (std::u32string{0x4d}));
        TRY(std::copy(utf_iterator(c8, 0), utf_iterator(c8, 3), overwrite(s32)));    TEST_EQUAL(s32, (std::u32string{0x4d,0x430}));
        TRY(std::copy(utf_iterator(c8, 0), utf_iterator(c8, 6), overwrite(s32)));    TEST_EQUAL(s32, (std::u32string{0x4d,0x430,0x4e8c}));
        TRY(std::copy(utf_iterator(c8, 0), utf_iterator(c8, 10), overwrite(s32)));   TEST_EQUAL(s32, (std::u32string{0x4d,0x430,0x4e8c,0x10302}));
        TRY(std::copy(utf_iterator(c8, 1), utf_iterator(c8, 14), overwrite(s32)));   TEST_EQUAL(s32, (std::u32string{0x430,0x4e8c,0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c8, 3), utf_iterator(c8, 14), overwrite(s32)));   TEST_EQUAL(s32, (std::u32string{0x4e8c,0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c8, 6), utf_iterator(c8, 14), overwrite(s32)));   TEST_EQUAL(s32, (std::u32string{0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c8, 10), utf_iterator(c8, 14), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x10fffd}));
        TRY(std::copy(utf_iterator(c16, 0), utf_iterator(c16, 7), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d,0x430,0x4e8c,0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c16, 0), utf_iterator(c16, 1), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d}));
        TRY(std::copy(utf_iterator(c16, 0), utf_iterator(c16, 2), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d,0x430}));
        TRY(std::copy(utf_iterator(c16, 0), utf_iterator(c16, 3), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d,0x430,0x4e8c}));
        TRY(std::copy(utf_iterator(c16, 0), utf_iterator(c16, 5), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d,0x430,0x4e8c,0x10302}));
        TRY(std::copy(utf_iterator(c16, 1), utf_iterator(c16, 7), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x430,0x4e8c,0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c16, 2), utf_iterator(c16, 7), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4e8c,0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c16, 3), utf_iterator(c16, 7), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c16, 5), utf_iterator(c16, 7), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x10fffd}));
        TRY(std::copy(utf_iterator(c32, 0), utf_iterator(c32, 5), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d,0x430,0x4e8c,0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c32, 0), utf_iterator(c32, 1), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d}));
        TRY(std::copy(utf_iterator(c32, 0), utf_iterator(c32, 2), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d,0x430}));
        TRY(std::copy(utf_iterator(c32, 0), utf_iterator(c32, 3), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d,0x430,0x4e8c}));
        TRY(std::copy(utf_iterator(c32, 0), utf_iterator(c32, 4), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4d,0x430,0x4e8c,0x10302}));
        TRY(std::copy(utf_iterator(c32, 1), utf_iterator(c32, 5), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x430,0x4e8c,0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c32, 2), utf_iterator(c32, 5), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x4e8c,0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c32, 3), utf_iterator(c32, 5), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x10302,0x10fffd}));
        TRY(std::copy(utf_iterator(c32, 4), utf_iterator(c32, 5), overwrite(s32)));  TEST_EQUAL(s32, (std::u32string{0x10fffd}));

        s8.clear();   TRY(std::copy(a32.begin(), a32.end(), utf_writer(s8)));   TEST_EQUAL(s8, "");
        s8.clear();   TRY(std::copy(b32.begin(), b32.end(), utf_writer(s8)));   TEST_EQUAL(s8, "Hello");
        s8.clear();   TRY(std::copy(c32.begin(), c32.end(), utf_writer(s8)));   TEST_EQUAL(s8, c8);
        s8.clear();   TRY(std::copy(z32.begin(), z32.end(), utf_writer(s8)));   TEST_EQUAL(s8, z8);
        s16.clear();  TRY(std::copy(a32.begin(), a32.end(), utf_writer(s16)));  TEST_EQUAL(s16, u"");
        s16.clear();  TRY(std::copy(b32.begin(), b32.end(), utf_writer(s16)));  TEST_EQUAL(s16, u"Hello");
        s16.clear();  TRY(std::copy(c32.begin(), c32.end(), utf_writer(s16)));  TEST_EQUAL(s16, c16);
        s16.clear();  TRY(std::copy(z32.begin(), z32.end(), utf_writer(s16)));  TEST_EQUAL(s16, z16);
        s32.clear();  TRY(std::copy(a32.begin(), a32.end(), utf_writer(s32)));  TEST_EQUAL(s32, U"");
        s32.clear();  TRY(std::copy(b32.begin(), b32.end(), utf_writer(s32)));  TEST_EQUAL(s32, U"Hello");
        s32.clear();  TRY(std::copy(c32.begin(), c32.end(), utf_writer(s32)));  TEST_EQUAL(s32, c32);
        s32.clear();  TRY(std::copy(z32.begin(), z32.end(), utf_writer(s32)));  TEST_EQUAL(s32, z32);
        sw.clear();   TRY(std::copy(a32.begin(), a32.end(), utf_writer(sw)));   TEST_EQUAL(sw, L"");
        sw.clear();   TRY(std::copy(b32.begin(), b32.end(), utf_writer(sw)));   TEST_EQUAL(sw, L"Hello");
        sw.clear();   TRY(std::copy(c32.begin(), c32.end(), utf_writer(sw)));   TEST_EQUAL(sw, cw);

    }

    void check_implicit_recoding() {

        U8string s8;
        std::u16string s16;
        std::u32string s32;
        std::wstring sw;

        TRY(recode(a8, s8));    TEST_EQUAL(s8, a8);
        TRY(recode(b8, s8));    TEST_EQUAL(s8, b8);
        TRY(recode(c8, s8));    TEST_EQUAL(s8, c8);
        TRY(recode(z8, s8));    TEST_EQUAL(s8, z8);
        TRY(recode(a8, s16));   TEST_EQUAL(s16, a16);
        TRY(recode(b8, s16));   TEST_EQUAL(s16, b16);
        TRY(recode(c8, s16));   TEST_EQUAL(s16, c16);
        TRY(recode(z8, s16));   TEST_EQUAL(s16, z16);
        TRY(recode(a8, s32));   TEST_EQUAL(s32, a32);
        TRY(recode(b8, s32));   TEST_EQUAL(s32, b32);
        TRY(recode(c8, s32));   TEST_EQUAL(s32, c32);
        TRY(recode(z8, s32));   TEST_EQUAL(s32, z32);
        TRY(recode(a8, sw));    TEST_EQUAL(sw, aw);
        TRY(recode(b8, sw));    TEST_EQUAL(sw, bw);
        TRY(recode(c8, sw));    TEST_EQUAL(sw, cw);
        TRY(recode(z8, sw));    TEST_EQUAL(sw, zw);
        TRY(recode(a16, s8));   TEST_EQUAL(s8, a8);
        TRY(recode(b16, s8));   TEST_EQUAL(s8, b8);
        TRY(recode(c16, s8));   TEST_EQUAL(s8, c8);
        TRY(recode(z16, s8));   TEST_EQUAL(s8, z8);
        TRY(recode(a16, s16));  TEST_EQUAL(s16, a16);
        TRY(recode(b16, s16));  TEST_EQUAL(s16, b16);
        TRY(recode(c16, s16));  TEST_EQUAL(s16, c16);
        TRY(recode(z16, s16));  TEST_EQUAL(s16, z16);
        TRY(recode(a16, s32));  TEST_EQUAL(s32, a32);
        TRY(recode(b16, s32));  TEST_EQUAL(s32, b32);
        TRY(recode(c16, s32));  TEST_EQUAL(s32, c32);
        TRY(recode(z16, s32));  TEST_EQUAL(s32, z32);
        TRY(recode(a16, sw));   TEST_EQUAL(sw, aw);
        TRY(recode(b16, sw));   TEST_EQUAL(sw, bw);
        TRY(recode(c16, sw));   TEST_EQUAL(sw, cw);
        TRY(recode(z16, sw));   TEST_EQUAL(sw, zw);
        TRY(recode(a32, s8));   TEST_EQUAL(s8, a8);
        TRY(recode(b32, s8));   TEST_EQUAL(s8, b8);
        TRY(recode(c32, s8));   TEST_EQUAL(s8, c8);
        TRY(recode(z32, s8));   TEST_EQUAL(s8, z8);
        TRY(recode(a32, s16));  TEST_EQUAL(s16, a16);
        TRY(recode(b32, s16));  TEST_EQUAL(s16, b16);
        TRY(recode(c32, s16));  TEST_EQUAL(s16, c16);
        TRY(recode(z32, s16));  TEST_EQUAL(s16, z16);
        TRY(recode(a32, s32));  TEST_EQUAL(s32, a32);
        TRY(recode(b32, s32));  TEST_EQUAL(s32, b32);
        TRY(recode(c32, s32));  TEST_EQUAL(s32, c32);
        TRY(recode(z32, s32));  TEST_EQUAL(s32, z32);
        TRY(recode(a32, sw));   TEST_EQUAL(sw, aw);
        TRY(recode(b32, sw));   TEST_EQUAL(sw, bw);
        TRY(recode(c32, sw));   TEST_EQUAL(sw, cw);
        TRY(recode(z32, sw));   TEST_EQUAL(sw, zw);
        TRY(recode(aw, s8));    TEST_EQUAL(s8, a8);
        TRY(recode(bw, s8));    TEST_EQUAL(s8, b8);
        TRY(recode(cw, s8));    TEST_EQUAL(s8, c8);
        TRY(recode(zw, s8));    TEST_EQUAL(s8, z8);
        TRY(recode(aw, s16));   TEST_EQUAL(s16, a16);
        TRY(recode(bw, s16));   TEST_EQUAL(s16, b16);
        TRY(recode(cw, s16));   TEST_EQUAL(s16, c16);
        TRY(recode(zw, s16));   TEST_EQUAL(s16, z16);
        TRY(recode(aw, s32));   TEST_EQUAL(s32, a32);
        TRY(recode(bw, s32));   TEST_EQUAL(s32, b32);
        TRY(recode(cw, s32));   TEST_EQUAL(s32, c32);
        TRY(recode(zw, s32));   TEST_EQUAL(s32, z32);
        TRY(recode(aw, sw));    TEST_EQUAL(sw, aw);
        TRY(recode(bw, sw));    TEST_EQUAL(sw, bw);
        TRY(recode(cw, sw));    TEST_EQUAL(sw, cw);
        TRY(recode(zw, sw));    TEST_EQUAL(sw, zw);

        TRY(recode(a8.data(), a8.size(), s8));     TEST_EQUAL(s8, a8);
        TRY(recode(b8.data(), b8.size(), s8));     TEST_EQUAL(s8, b8);
        TRY(recode(c8.data(), c8.size(), s8));     TEST_EQUAL(s8, c8);
        TRY(recode(a8.data(), a8.size(), s16));    TEST_EQUAL(s16, a16);
        TRY(recode(b8.data(), b8.size(), s16));    TEST_EQUAL(s16, b16);
        TRY(recode(c8.data(), c8.size(), s16));    TEST_EQUAL(s16, c16);
        TRY(recode(a8.data(), a8.size(), s32));    TEST_EQUAL(s32, a32);
        TRY(recode(b8.data(), b8.size(), s32));    TEST_EQUAL(s32, b32);
        TRY(recode(c8.data(), c8.size(), s32));    TEST_EQUAL(s32, c32);
        TRY(recode(a16.data(), a16.size(), s8));   TEST_EQUAL(s8, a8);
        TRY(recode(b16.data(), b16.size(), s8));   TEST_EQUAL(s8, b8);
        TRY(recode(c16.data(), c16.size(), s8));   TEST_EQUAL(s8, c8);
        TRY(recode(a16.data(), a16.size(), s16));  TEST_EQUAL(s16, a16);
        TRY(recode(b16.data(), b16.size(), s16));  TEST_EQUAL(s16, b16);
        TRY(recode(c16.data(), c16.size(), s16));  TEST_EQUAL(s16, c16);
        TRY(recode(a16.data(), a16.size(), s32));  TEST_EQUAL(s32, a32);
        TRY(recode(b16.data(), b16.size(), s32));  TEST_EQUAL(s32, b32);
        TRY(recode(c16.data(), c16.size(), s32));  TEST_EQUAL(s32, c32);
        TRY(recode(a32.data(), a32.size(), s8));   TEST_EQUAL(s8, a8);
        TRY(recode(b32.data(), b32.size(), s8));   TEST_EQUAL(s8, b8);
        TRY(recode(c32.data(), c32.size(), s8));   TEST_EQUAL(s8, c8);
        TRY(recode(a32.data(), a32.size(), s16));  TEST_EQUAL(s16, a16);
        TRY(recode(b32.data(), b32.size(), s16));  TEST_EQUAL(s16, b16);
        TRY(recode(c32.data(), c32.size(), s16));  TEST_EQUAL(s16, c16);
        TRY(recode(a32.data(), a32.size(), s32));  TEST_EQUAL(s32, a32);
        TRY(recode(b32.data(), b32.size(), s32));  TEST_EQUAL(s32, b32);
        TRY(recode(c32.data(), c32.size(), s32));  TEST_EQUAL(s32, c32);

    }

    void check_explicit_recoding() {

        TEST_EQUAL(recode<char>(a8), a8);
        TEST_EQUAL(recode<char>(b8), b8);
        TEST_EQUAL(recode<char>(c8), c8);
        TEST_EQUAL(recode<char>(z8), z8);
        TEST_EQUAL(recode<char16_t>(a8), a16);
        TEST_EQUAL(recode<char16_t>(b8), b16);
        TEST_EQUAL(recode<char16_t>(c8), c16);
        TEST_EQUAL(recode<char16_t>(z8), z16);
        TEST_EQUAL(recode<char32_t>(a8), a32);
        TEST_EQUAL(recode<char32_t>(b8), b32);
        TEST_EQUAL(recode<char32_t>(c8), c32);
        TEST_EQUAL(recode<char32_t>(z8), z32);
        TEST_EQUAL(recode<wchar_t>(a8), aw);
        TEST_EQUAL(recode<wchar_t>(b8), bw);
        TEST_EQUAL(recode<wchar_t>(c8), cw);
        TEST_EQUAL(recode<wchar_t>(z8), zw);
        TEST_EQUAL(recode<char>(a16), a8);
        TEST_EQUAL(recode<char>(b16), b8);
        TEST_EQUAL(recode<char>(c16), c8);
        TEST_EQUAL(recode<char>(z16), z8);
        TEST_EQUAL(recode<char16_t>(a16), a16);
        TEST_EQUAL(recode<char16_t>(b16), b16);
        TEST_EQUAL(recode<char16_t>(c16), c16);
        TEST_EQUAL(recode<char16_t>(z16), z16);
        TEST_EQUAL(recode<char32_t>(a16), a32);
        TEST_EQUAL(recode<char32_t>(b16), b32);
        TEST_EQUAL(recode<char32_t>(c16), c32);
        TEST_EQUAL(recode<char32_t>(z16), z32);
        TEST_EQUAL(recode<wchar_t>(a16), aw);
        TEST_EQUAL(recode<wchar_t>(b16), bw);
        TEST_EQUAL(recode<wchar_t>(c16), cw);
        TEST_EQUAL(recode<wchar_t>(z16), zw);
        TEST_EQUAL(recode<char>(a32), a8);
        TEST_EQUAL(recode<char>(b32), b8);
        TEST_EQUAL(recode<char>(c32), c8);
        TEST_EQUAL(recode<char>(z32), z8);
        TEST_EQUAL(recode<char16_t>(a32), a16);
        TEST_EQUAL(recode<char16_t>(b32), b16);
        TEST_EQUAL(recode<char16_t>(c32), c16);
        TEST_EQUAL(recode<char16_t>(z32), z16);
        TEST_EQUAL(recode<char32_t>(a32), a32);
        TEST_EQUAL(recode<char32_t>(b32), b32);
        TEST_EQUAL(recode<char32_t>(c32), c32);
        TEST_EQUAL(recode<char32_t>(z32), z32);
        TEST_EQUAL(recode<wchar_t>(a32), aw);
        TEST_EQUAL(recode<wchar_t>(b32), bw);
        TEST_EQUAL(recode<wchar_t>(c32), cw);
        TEST_EQUAL(recode<wchar_t>(z32), zw);
        TEST_EQUAL(recode<char>(aw), a8);
        TEST_EQUAL(recode<char>(bw), b8);
        TEST_EQUAL(recode<char>(cw), c8);
        TEST_EQUAL(recode<char>(zw), z8);
        TEST_EQUAL(recode<char16_t>(aw), a16);
        TEST_EQUAL(recode<char16_t>(bw), b16);
        TEST_EQUAL(recode<char16_t>(cw), c16);
        TEST_EQUAL(recode<char16_t>(zw), z16);
        TEST_EQUAL(recode<char32_t>(aw), a32);
        TEST_EQUAL(recode<char32_t>(bw), b32);
        TEST_EQUAL(recode<char32_t>(cw), c32);
        TEST_EQUAL(recode<char32_t>(zw), z32);
        TEST_EQUAL(recode<wchar_t>(aw), aw);
        TEST_EQUAL(recode<wchar_t>(bw), bw);
        TEST_EQUAL(recode<wchar_t>(cw), cw);
        TEST_EQUAL(recode<wchar_t>(zw), zw);

        TEST_EQUAL(to_utf8(a8), a8);
        TEST_EQUAL(to_utf8(b8), b8);
        TEST_EQUAL(to_utf8(c8), c8);
        TEST_EQUAL(to_utf8(a16), a8);
        TEST_EQUAL(to_utf8(b16), b8);
        TEST_EQUAL(to_utf8(c16), c8);
        TEST_EQUAL(to_utf8(a32), a8);
        TEST_EQUAL(to_utf8(b32), b8);
        TEST_EQUAL(to_utf8(c32), c8);
        TEST_EQUAL(to_utf8(aw), a8);
        TEST_EQUAL(to_utf8(bw), b8);
        TEST_EQUAL(to_utf8(cw), c8);
        TEST_EQUAL(to_utf16(a8), a16);
        TEST_EQUAL(to_utf16(b8), b16);
        TEST_EQUAL(to_utf16(c8), c16);
        TEST_EQUAL(to_utf16(a16), a16);
        TEST_EQUAL(to_utf16(b16), b16);
        TEST_EQUAL(to_utf16(c16), c16);
        TEST_EQUAL(to_utf16(a32), a16);
        TEST_EQUAL(to_utf16(b32), b16);
        TEST_EQUAL(to_utf16(c32), c16);
        TEST_EQUAL(to_utf16(aw), a16);
        TEST_EQUAL(to_utf16(bw), b16);
        TEST_EQUAL(to_utf16(cw), c16);
        TEST_EQUAL(to_utf32(a8), a32);
        TEST_EQUAL(to_utf32(b8), b32);
        TEST_EQUAL(to_utf32(c8), c32);
        TEST_EQUAL(to_utf32(a16), a32);
        TEST_EQUAL(to_utf32(b16), b32);
        TEST_EQUAL(to_utf32(c16), c32);
        TEST_EQUAL(to_utf32(a32), a32);
        TEST_EQUAL(to_utf32(b32), b32);
        TEST_EQUAL(to_utf32(c32), c32);
        TEST_EQUAL(to_utf32(aw), a32);
        TEST_EQUAL(to_utf32(bw), b32);
        TEST_EQUAL(to_utf32(cw), c32);
        TEST_EQUAL(to_wstring(a8), aw);
        TEST_EQUAL(to_wstring(b8), bw);
        TEST_EQUAL(to_wstring(c8), cw);
        TEST_EQUAL(to_wstring(a16), aw);
        TEST_EQUAL(to_wstring(b16), bw);
        TEST_EQUAL(to_wstring(c16), cw);
        TEST_EQUAL(to_wstring(a32), aw);
        TEST_EQUAL(to_wstring(b32), bw);
        TEST_EQUAL(to_wstring(c32), cw);
        TEST_EQUAL(to_wstring(aw), aw);
        TEST_EQUAL(to_wstring(bw), bw);
        TEST_EQUAL(to_wstring(cw), cw);

    }

    void check_string_validation() {

        U8string s8;
        std::u16string s16;
        std::u32string s32;
        std::wstring sw;

        TRY(check_string(c8));
        TEST_THROW(check_string(x8), EncodingError);
        TRY(check_string(c16));
        TEST_THROW(check_string(x16), EncodingError);
        TRY(check_string(c32));
        TEST_THROW(check_string(x32), EncodingError);

        TEST(valid_string(a8));
        TEST(valid_string(b8));
        TEST(valid_string(c8));
        TEST(! valid_string(x8));
        TEST(valid_string(a16));
        TEST(valid_string(b16));
        TEST(valid_string(c16));
        TEST(! valid_string(x16));
        TEST(valid_string(a32));
        TEST(valid_string(b32));
        TEST(valid_string(c32));
        TEST(! valid_string(x32));

        TRY(s8 = sanitize(a8));    TEST_EQUAL(s8, a8);
        TRY(s8 = sanitize(b8));    TEST_EQUAL(s8, b8);
        TRY(s8 = sanitize(c8));    TEST_EQUAL(s8, c8);
        TRY(s8 = sanitize(x8));    TEST_EQUAL(s8, y8);
        TRY(s16 = sanitize(a16));  TEST_EQUAL(s16, a16);
        TRY(s16 = sanitize(b16));  TEST_EQUAL(s16, b16);
        TRY(s16 = sanitize(c16));  TEST_EQUAL(s16, c16);
        TRY(s16 = sanitize(x16));  TEST_EQUAL(s16, y16);
        TRY(s32 = sanitize(a32));  TEST_EQUAL(s32, a32);
        TRY(s32 = sanitize(b32));  TEST_EQUAL(s32, b32);
        TRY(s32 = sanitize(c32));  TEST_EQUAL(s32, c32);
        TRY(s32 = sanitize(x32));  TEST_EQUAL(s32, y32);
        TRY(sw = sanitize(aw));    TEST_EQUAL(sw, aw);
        TRY(sw = sanitize(bw));    TEST_EQUAL(sw, bw);
        TRY(sw = sanitize(cw));    TEST_EQUAL(sw, cw);
        TRY(sw = sanitize(xw));    TEST_EQUAL(sw, yw);

        s8 = a8;    TRY(sanitize_in(s8));   TEST_EQUAL(s8, a8);
        s8 = b8;    TRY(sanitize_in(s8));   TEST_EQUAL(s8, b8);
        s8 = c8;    TRY(sanitize_in(s8));   TEST_EQUAL(s8, c8);
        s8 = x8;    TRY(sanitize_in(s8));   TEST_EQUAL(s8, y8);
        s16 = a16;  TRY(sanitize_in(s16));  TEST_EQUAL(s16, a16);
        s16 = b16;  TRY(sanitize_in(s16));  TEST_EQUAL(s16, b16);
        s16 = c16;  TRY(sanitize_in(s16));  TEST_EQUAL(s16, c16);
        s16 = x16;  TRY(sanitize_in(s16));  TEST_EQUAL(s16, y16);
        s32 = a32;  TRY(sanitize_in(s32));  TEST_EQUAL(s32, a32);
        s32 = b32;  TRY(sanitize_in(s32));  TEST_EQUAL(s32, b32);
        s32 = c32;  TRY(sanitize_in(s32));  TEST_EQUAL(s32, c32);
        s32 = x32;  TRY(sanitize_in(s32));  TEST_EQUAL(s32, y32);
        sw = aw;    TRY(sanitize_in(sw));   TEST_EQUAL(sw, aw);
        sw = bw;    TRY(sanitize_in(sw));   TEST_EQUAL(sw, bw);
        sw = cw;    TRY(sanitize_in(sw));   TEST_EQUAL(sw, cw);
        sw = xw;    TRY(sanitize_in(sw));   TEST_EQUAL(sw, yw);

        TEST_EQUAL(valid_count(a8), npos);
        TEST_EQUAL(valid_count(b8), npos);
        TEST_EQUAL(valid_count(c8), npos);
        TEST_EQUAL(valid_count(x8), 5);
        TEST_EQUAL(valid_count(a16), npos);
        TEST_EQUAL(valid_count(b16), npos);
        TEST_EQUAL(valid_count(c16), npos);
        TEST_EQUAL(valid_count(x16), 5);
        TEST_EQUAL(valid_count(a32), npos);
        TEST_EQUAL(valid_count(b32), npos);
        TEST_EQUAL(valid_count(c32), npos);
        TEST_EQUAL(valid_count(x32), 5);

    }

    void check_error_handling() {

        U8string s8;
        std::u16string s16;
        std::u32string s32;
        std::wstring sw;
        Utf8Iterator i8;
        Utf16Iterator i16;
        Utf32Iterator i32;
        WcharIterator iw;
        Utf8Range r8;
        Utf16Range r16;
        Utf32Range r32;
        WcharRange rw;

        TRY(r8 = utf_range(x8, err_replace));    TRY(std::copy(r8.begin(), r8.end(), overwrite(s32)));   TEST_EQUAL(s32, y32);
        TRY(r16 = utf_range(x16, err_replace));  TRY(std::copy(r16.begin(), r16.end(), overwrite(s32)));  TEST_EQUAL(s32, y32);
        TRY(r32 = utf_range(x32, err_replace));  TRY(std::copy(r32.begin(), r32.end(), overwrite(s32)));  TEST_EQUAL(s32, y32);
        TRY(rw = utf_range(xw, err_replace));    TRY(std::copy(rw.begin(), rw.end(), overwrite(s32)));   TEST_EQUAL(s32, y32);

        s8.clear();   TRY(std::copy(x32.begin(), x32.end(), utf_writer(s8, err_replace)));   TEST_EQUAL(s8, y8);
        s16.clear();  TRY(std::copy(x32.begin(), x32.end(), utf_writer(s16, err_replace)));  TEST_EQUAL(s16, y16);
        s32.clear();  TRY(std::copy(x32.begin(), x32.end(), utf_writer(s32, err_replace)));  TEST_EQUAL(s32, y32);
        sw.clear();   TRY(std::copy(x32.begin(), x32.end(), utf_writer(sw, err_replace)));   TEST_EQUAL(sw, yw);

        TRY(i8 = utf_begin(x8, err_throw));
        TRY(std::advance(i8, 4));
        TEST_THROW_EQUAL(++i8, EncodingError, "Encoding error (UTF-8); offset 5; hex ff");
        TRY(i8 = utf_iterator(x8, 0, err_throw));
        TEST_THROW_EQUAL(i8 = utf_iterator(x8, 5, err_throw), EncodingError, "Encoding error (UTF-8); offset 5; hex ff");
        TRY(i16 = utf_begin(x16, err_throw));
        TRY(std::advance(i16, 4));
        TEST_THROW_EQUAL(++i16, EncodingError, "Encoding error (UTF-16); offset 5; hex d800");
        TRY(i16 = utf_iterator(x16, 0, err_throw));
        TEST_THROW_EQUAL(i16 = utf_iterator(x16, 5, err_throw), EncodingError, "Encoding error (UTF-16); offset 5; hex d800");
        TRY(i32 = utf_begin(x32, err_throw));
        TRY(std::advance(i32, 4));
        TEST_THROW_EQUAL(++i32, EncodingError, "Encoding error (UTF-32); offset 5; hex 0000d800");
        TRY(i32 = utf_iterator(x32, 0, err_throw));
        TEST_THROW_EQUAL(i32 = utf_iterator(x32, 5, err_throw), EncodingError, "Encoding error (UTF-32); offset 5; hex 0000d800");

        s8.clear();
        TEST_THROW_EQUAL(std::copy(x32.begin(), x32.end(), utf_writer(s8, err_throw)),
            EncodingError, "Encoding error (UTF-8); offset 5; hex 0000d800");
        s16.clear();
        TEST_THROW_EQUAL(std::copy(x32.begin(), x32.end(), utf_writer(s16, err_throw)),
            EncodingError, "Encoding error (UTF-16); offset 5; hex 0000d800");
        s32.clear();
        TEST_THROW_EQUAL(std::copy(x32.begin(), x32.end(), utf_writer(s32, err_throw)),
            EncodingError, "Encoding error (UTF-32); offset 5; hex 0000d800");

        // Check that error handling for UTF-8 matches the Unicode recommendation
        // (Unicode Standard 7.0, section 3.9, page 128)
        const std::string bad_utf8 {"\x61\xf1\x80\x80\xe1\x80\xc2\x62\x80\x63\x80\xbf\x64"};
        const std::u32string expected {0x61,0xfffd,0xfffd,0xfffd,0x62,0xfffd,0x63,0xfffd,0xfffd,0x64};
        TRY(recode(bad_utf8, s32, err_replace));
        TEST_EQUAL(s32, expected);

    }

}

TEST_MODULE(unicorn, utf) {

    check_basic_conversions();
    check_basic_utilities();
    check_decoding_iterators();
    check_decoding_ranges();
    check_implicit_recoding();
    check_explicit_recoding();
    check_string_validation();
    check_error_handling();

}
