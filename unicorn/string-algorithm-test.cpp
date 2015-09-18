#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <iterator>
#include <random>
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void check_common() {

        u8string a8, b8 = "Hello", c8 = "Hello world", d8 = "Hellfire", e8 = "Goodbye";
        size_t n = 0;

        TRY(n = str_common(a8, b8));      TEST_EQUAL(n, 0);
        TRY(n = str_common(b8, c8));      TEST_EQUAL(n, 5);
        TRY(n = str_common(b8, d8));      TEST_EQUAL(n, 4);
        TRY(n = str_common(b8, e8));      TEST_EQUAL(n, 0);
        TRY(n = str_common(b8, d8, 3));   TEST_EQUAL(n, 1);
        TRY(n = str_common(b8, d8, 4));   TEST_EQUAL(n, 0);
        TRY(n = str_common(b8, d8, 5));   TEST_EQUAL(n, 0);
        TRY(n = str_common(b8, d8, 42));  TEST_EQUAL(n, 0);

        u16string a16, b16 = u"Hello", c16 = u"Hello world", d16 = u"Hellfire", e16 = u"Goodbye";

        TRY(n = str_common(a16, b16));      TEST_EQUAL(n, 0);
        TRY(n = str_common(b16, c16));      TEST_EQUAL(n, 5);
        TRY(n = str_common(b16, d16));      TEST_EQUAL(n, 4);
        TRY(n = str_common(b16, e16));      TEST_EQUAL(n, 0);
        TRY(n = str_common(b16, d16, 3));   TEST_EQUAL(n, 1);
        TRY(n = str_common(b16, d16, 4));   TEST_EQUAL(n, 0);
        TRY(n = str_common(b16, d16, 5));   TEST_EQUAL(n, 0);
        TRY(n = str_common(b16, d16, 42));  TEST_EQUAL(n, 0);

        u32string a32, b32 = U"Hello", c32 = U"Hello world", d32 = U"Hellfire", e32 = U"Goodbye";

        TRY(n = str_common(a32, b32));      TEST_EQUAL(n, 0);
        TRY(n = str_common(b32, c32));      TEST_EQUAL(n, 5);
        TRY(n = str_common(b32, d32));      TEST_EQUAL(n, 4);
        TRY(n = str_common(b32, e32));      TEST_EQUAL(n, 0);
        TRY(n = str_common(b32, d32, 3));   TEST_EQUAL(n, 1);
        TRY(n = str_common(b32, d32, 4));   TEST_EQUAL(n, 0);
        TRY(n = str_common(b32, d32, 5));   TEST_EQUAL(n, 0);
        TRY(n = str_common(b32, d32, 42));  TEST_EQUAL(n, 0);

        // "αβγδ" = Unicode 3b1 3b1 3b1 3c4 = UTF-8 ce b1 ce b2 ce b3 ce b4
        // "αβδε" = Unicode 3b1 3b1 3c4 3c5 = UTF-8 ce b1 ce b2 ce b4 ce b5

        u8string x8 = u8"αβγδ", y8 = u8"αβδε";

        TRY(n = str_common(x8, y8));      TEST_EQUAL(n, 5);
        TRY(n = str_common_utf(x8, y8));  TEST_EQUAL(n, 4);

    }

    void check_compare() {

        TEST(! str_compare(u""s, u""s));
        TEST(str_compare(u""s, u"Hello"s));
        TEST(! str_compare(u"Hello"s, u""s));
        TEST(! str_compare(u"Hello"s, u"Hello"s));
        TEST(str_compare(u"Hello"s, u"world"s));
        TEST(! str_compare(u"world"s, u"Hello"s));
        TEST(! str_compare(u"Hello world"s, u"Hello"s));
        TEST(str_compare(u"Hello"s, u"Hello world"s));

        std::mt19937 mt(42);
        std::uniform_int_distribution<size_t> lengths(1, 10);
        std::uniform_int_distribution<char32_t> chars(0, 0x10ffff);

        auto make_string = [&] {
            auto n = lengths(mt);
            u32string s;
            while (s.size() < n) {
                auto c = chars(mt);
                if (char_is_unicode(c))
                    s += c;
            }
            return s;
        };

        for (int i = 0; i < 1000; ++i) {
            auto u32a = make_string(), u32b = make_string();
            auto u16a = to_utf16(u32a), u16b = to_utf16(u32b);
            auto u8a = to_utf8(u32a), u8b = to_utf8(u32b);
            TEST_EQUAL(str_compare(u16a, u16b), str_compare(u32a, u32b));
            TEST_EQUAL(str_compare(u8a, u8b), str_compare(u32a, u32b));
        }

        int rc;

        TRY(rc = str_compare_3way(""s, ""s));                TEST_EQUAL(rc, 0);
        TRY(rc = str_compare_3way("Hello"s, ""s));           TEST_EQUAL(rc, 1);
        TRY(rc = str_compare_3way(""s, "Hello"s));           TEST_EQUAL(rc, -1);
        TRY(rc = str_compare_3way("Hello"s, "Hello"s));      TEST_EQUAL(rc, 0);
        TRY(rc = str_compare_3way("Hello"s, "Hellfire"s));   TEST_EQUAL(rc, 1);
        TRY(rc = str_compare_3way("Hellfire"s, "Hello"s));   TEST_EQUAL(rc, -1);
        TRY(rc = str_compare_3way(u8"αβγδε"s, u8"αβγδε"s));  TEST_EQUAL(rc, 0);
        TRY(rc = str_compare_3way(u8"αβδε"s, u8"αβγδ"s));    TEST_EQUAL(rc, 1);
        TRY(rc = str_compare_3way(u8"αβγδ"s, u8"αβδε"s));    TEST_EQUAL(rc, -1);

        TRY(rc = str_compare_3way(u""s, u""s));               TEST_EQUAL(rc, 0);
        TRY(rc = str_compare_3way(u"Hello"s, u""s));          TEST_EQUAL(rc, 1);
        TRY(rc = str_compare_3way(u""s, u"Hello"s));          TEST_EQUAL(rc, -1);
        TRY(rc = str_compare_3way(u"Hello"s, u"Hello"s));     TEST_EQUAL(rc, 0);
        TRY(rc = str_compare_3way(u"Hello"s, u"Hellfire"s));  TEST_EQUAL(rc, 1);
        TRY(rc = str_compare_3way(u"Hellfire"s, u"Hello"s));  TEST_EQUAL(rc, -1);
        TRY(rc = str_compare_3way(u"αβγδε"s, u"αβγδε"s));     TEST_EQUAL(rc, 0);
        TRY(rc = str_compare_3way(u"αβδε"s, u"αβγδ"s));       TEST_EQUAL(rc, 1);
        TRY(rc = str_compare_3way(u"αβγδ"s, u"αβδε"s));       TEST_EQUAL(rc, -1);

        TRY(rc = str_compare_3way(U""s, U""s));               TEST_EQUAL(rc, 0);
        TRY(rc = str_compare_3way(U"Hello"s, U""s));          TEST_EQUAL(rc, 1);
        TRY(rc = str_compare_3way(U""s, U"Hello"s));          TEST_EQUAL(rc, -1);
        TRY(rc = str_compare_3way(U"Hello"s, U"Hello"s));     TEST_EQUAL(rc, 0);
        TRY(rc = str_compare_3way(U"Hello"s, U"Hellfire"s));  TEST_EQUAL(rc, 1);
        TRY(rc = str_compare_3way(U"Hellfire"s, U"Hello"s));  TEST_EQUAL(rc, -1);
        TRY(rc = str_compare_3way(U"αβγδε"s, U"αβγδε"s));     TEST_EQUAL(rc, 0);
        TRY(rc = str_compare_3way(U"αβδε"s, U"αβγδ"s));       TEST_EQUAL(rc, 1);
        TRY(rc = str_compare_3way(U"αβγδ"s, U"αβδε"s));       TEST_EQUAL(rc, -1);

    }

    void check_expect() {

        u8string a8, b8 = u8"Hello world", c8 = u8"€uro ∈lement";
        u16string a16, b16 = u"Hello world", c16 = u"€uro ∈lement";
        u32string a32, b32 = U"Hello world", c32 = U"€uro ∈lement";
        UtfIterator<char> i8;
        UtfIterator<char16_t> i16;
        UtfIterator<char32_t> i32;

        TRY(i8 = utf_begin(a8));  TEST(! str_expect(i8, u8""s));                TEST_EQUAL(std::distance(utf_begin(a8), i8), 0);
        TRY(i8 = utf_begin(a8));  TEST(! str_expect(i8, u8"Hello"s));           TEST_EQUAL(std::distance(utf_begin(a8), i8), 0);
        TRY(i8 = utf_begin(a8));  TEST(! str_expect(i8, u8"€uro"s));            TEST_EQUAL(std::distance(utf_begin(a8), i8), 0);
        TRY(i8 = utf_begin(b8));  TEST(! str_expect(i8, u8""s));                TEST_EQUAL(std::distance(utf_begin(b8), i8), 0);
        TRY(i8 = utf_begin(b8));  TEST(str_expect(i8, u8"Hello"s));             TEST_EQUAL(std::distance(utf_begin(b8), i8), 5);
        TRY(i8 = utf_begin(b8));  TEST(str_expect(i8, u8"Hello world"s));       TEST_EQUAL(std::distance(utf_begin(b8), i8), 11);
        TRY(i8 = utf_begin(b8));  TEST(! str_expect(i8, u8"Hello world 2"s));   TEST_EQUAL(std::distance(utf_begin(b8), i8), 0);
        TRY(i8 = utf_begin(b8));  TEST(! str_expect(i8, u8"world"s));           TEST_EQUAL(std::distance(utf_begin(b8), i8), 0);
        TRY(i8 = utf_begin(b8));  TEST(! str_expect(i8, u8"€uro"s));            TEST_EQUAL(std::distance(utf_begin(b8), i8), 0);
        TRY(i8 = utf_begin(c8));  TEST(! str_expect(i8, u8""s));                TEST_EQUAL(std::distance(utf_begin(c8), i8), 0);
        TRY(i8 = utf_begin(c8));  TEST(! str_expect(i8, u8"Hello"s));           TEST_EQUAL(std::distance(utf_begin(c8), i8), 0);
        TRY(i8 = utf_begin(c8));  TEST(str_expect(i8, u8"€uro"s));              TEST_EQUAL(std::distance(utf_begin(c8), i8), 4);
        TRY(i8 = utf_begin(c8));  TEST(str_expect(i8, u8"€uro ∈lement"s));      TEST_EQUAL(std::distance(utf_begin(c8), i8), 12);
        TRY(i8 = utf_begin(c8));  TEST(! str_expect(i8, u8"€uro ∈lement 2"s));  TEST_EQUAL(std::distance(utf_begin(c8), i8), 0);
        TRY(i8 = utf_begin(c8));  TEST(! str_expect(i8, u8"∈lement"s));         TEST_EQUAL(std::distance(utf_begin(c8), i8), 0);

        TRY(i16 = utf_begin(a16));  TEST(! str_expect(i16, u""s));                TEST_EQUAL(std::distance(utf_begin(a16), i16), 0);
        TRY(i16 = utf_begin(a16));  TEST(! str_expect(i16, u"Hello"s));           TEST_EQUAL(std::distance(utf_begin(a16), i16), 0);
        TRY(i16 = utf_begin(a16));  TEST(! str_expect(i16, u"€uro"s));            TEST_EQUAL(std::distance(utf_begin(a16), i16), 0);
        TRY(i16 = utf_begin(b16));  TEST(! str_expect(i16, u""s));                TEST_EQUAL(std::distance(utf_begin(b16), i16), 0);
        TRY(i16 = utf_begin(b16));  TEST(str_expect(i16, u"Hello"s));             TEST_EQUAL(std::distance(utf_begin(b16), i16), 5);
        TRY(i16 = utf_begin(b16));  TEST(str_expect(i16, u"Hello world"s));       TEST_EQUAL(std::distance(utf_begin(b16), i16), 11);
        TRY(i16 = utf_begin(b16));  TEST(! str_expect(i16, u"Hello world 2"s));   TEST_EQUAL(std::distance(utf_begin(b16), i16), 0);
        TRY(i16 = utf_begin(b16));  TEST(! str_expect(i16, u"world"s));           TEST_EQUAL(std::distance(utf_begin(b16), i16), 0);
        TRY(i16 = utf_begin(b16));  TEST(! str_expect(i16, u"€uro"s));            TEST_EQUAL(std::distance(utf_begin(b16), i16), 0);
        TRY(i16 = utf_begin(c16));  TEST(! str_expect(i16, u""s));                TEST_EQUAL(std::distance(utf_begin(c16), i16), 0);
        TRY(i16 = utf_begin(c16));  TEST(! str_expect(i16, u"Hello"s));           TEST_EQUAL(std::distance(utf_begin(c16), i16), 0);
        TRY(i16 = utf_begin(c16));  TEST(str_expect(i16, u"€uro"s));              TEST_EQUAL(std::distance(utf_begin(c16), i16), 4);
        TRY(i16 = utf_begin(c16));  TEST(str_expect(i16, u"€uro ∈lement"s));      TEST_EQUAL(std::distance(utf_begin(c16), i16), 12);
        TRY(i16 = utf_begin(c16));  TEST(! str_expect(i16, u"€uro ∈lement 2"s));  TEST_EQUAL(std::distance(utf_begin(c16), i16), 0);
        TRY(i16 = utf_begin(c16));  TEST(! str_expect(i16, u"∈lement"s));         TEST_EQUAL(std::distance(utf_begin(c16), i16), 0);

        TRY(i32 = utf_begin(a32));  TEST(! str_expect(i32, U""s));                TEST_EQUAL(std::distance(utf_begin(a32), i32), 0);
        TRY(i32 = utf_begin(a32));  TEST(! str_expect(i32, U"Hello"s));           TEST_EQUAL(std::distance(utf_begin(a32), i32), 0);
        TRY(i32 = utf_begin(a32));  TEST(! str_expect(i32, U"€uro"s));            TEST_EQUAL(std::distance(utf_begin(a32), i32), 0);
        TRY(i32 = utf_begin(b32));  TEST(! str_expect(i32, U""s));                TEST_EQUAL(std::distance(utf_begin(b32), i32), 0);
        TRY(i32 = utf_begin(b32));  TEST(str_expect(i32, U"Hello"s));             TEST_EQUAL(std::distance(utf_begin(b32), i32), 5);
        TRY(i32 = utf_begin(b32));  TEST(str_expect(i32, U"Hello world"s));       TEST_EQUAL(std::distance(utf_begin(b32), i32), 11);
        TRY(i32 = utf_begin(b32));  TEST(! str_expect(i32, U"Hello world 2"s));   TEST_EQUAL(std::distance(utf_begin(b32), i32), 0);
        TRY(i32 = utf_begin(b32));  TEST(! str_expect(i32, U"world"s));           TEST_EQUAL(std::distance(utf_begin(b32), i32), 0);
        TRY(i32 = utf_begin(b32));  TEST(! str_expect(i32, U"€uro"s));            TEST_EQUAL(std::distance(utf_begin(b32), i32), 0);
        TRY(i32 = utf_begin(c32));  TEST(! str_expect(i32, U""s));                TEST_EQUAL(std::distance(utf_begin(c32), i32), 0);
        TRY(i32 = utf_begin(c32));  TEST(! str_expect(i32, U"Hello"s));           TEST_EQUAL(std::distance(utf_begin(c32), i32), 0);
        TRY(i32 = utf_begin(c32));  TEST(str_expect(i32, U"€uro"s));              TEST_EQUAL(std::distance(utf_begin(c32), i32), 4);
        TRY(i32 = utf_begin(c32));  TEST(str_expect(i32, U"€uro ∈lement"s));      TEST_EQUAL(std::distance(utf_begin(c32), i32), 12);
        TRY(i32 = utf_begin(c32));  TEST(! str_expect(i32, U"€uro ∈lement 2"s));  TEST_EQUAL(std::distance(utf_begin(c32), i32), 0);
        TRY(i32 = utf_begin(c32));  TEST(! str_expect(i32, U"∈lement"s));         TEST_EQUAL(std::distance(utf_begin(c32), i32), 0);

    }

    void check_find_char() {

        u8string s8;
        u16string s16;
        u32string s32;
        UtfIterator<char> i8;
        UtfIterator<char16_t> i16;
        UtfIterator<char32_t> i32;

        s8 = u8"€uro €uro €uro";
        TRY(i8 = str_find_char(s8, U'€'));                  TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_find_char(s8, U'o'));                  TEST_EQUAL(std::distance(utf_begin(s8), i8), 3);
        TRY(i8 = str_find_char(s8, U'z'));                  TEST_EQUAL(std::distance(utf_begin(s8), i8), 14);
        TRY(i8 = str_find_char(s8, U'§'));                  TEST_EQUAL(std::distance(utf_begin(s8), i8), 14);
        TRY(i8 = str_find_last_char(s8, U'€'));             TEST_EQUAL(std::distance(utf_begin(s8), i8), 10);
        TRY(i8 = str_find_last_char(s8, U'o'));             TEST_EQUAL(std::distance(utf_begin(s8), i8), 13);
        TRY(i8 = str_find_last_char(s8, U'z'));             TEST_EQUAL(std::distance(utf_begin(s8), i8), 14);
        TRY(i8 = str_find_last_char(s8, U'§'));             TEST_EQUAL(std::distance(utf_begin(s8), i8), 14);
        TRY(i8 = str_find_char(utf_range(s8), U'€'));       TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_find_char(utf_range(s8), U'o'));       TEST_EQUAL(std::distance(utf_begin(s8), i8), 3);
        TRY(i8 = str_find_char(utf_range(s8), U'z'));       TEST_EQUAL(std::distance(utf_begin(s8), i8), 14);
        TRY(i8 = str_find_char(utf_range(s8), U'§'));       TEST_EQUAL(std::distance(utf_begin(s8), i8), 14);
        TRY(i8 = str_find_last_char(utf_range(s8), U'€'));  TEST_EQUAL(std::distance(utf_begin(s8), i8), 10);
        TRY(i8 = str_find_last_char(utf_range(s8), U'o'));  TEST_EQUAL(std::distance(utf_begin(s8), i8), 13);
        TRY(i8 = str_find_last_char(utf_range(s8), U'z'));  TEST_EQUAL(std::distance(utf_begin(s8), i8), 14);
        TRY(i8 = str_find_last_char(utf_range(s8), U'§'));  TEST_EQUAL(std::distance(utf_begin(s8), i8), 14);

        s16 = u"€uro €uro €uro";
        TRY(i16 = str_find_char(s16, U'€'));                  TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_find_char(s16, U'o'));                  TEST_EQUAL(std::distance(utf_begin(s16), i16), 3);
        TRY(i16 = str_find_char(s16, U'z'));                  TEST_EQUAL(std::distance(utf_begin(s16), i16), 14);
        TRY(i16 = str_find_char(s16, U'§'));                  TEST_EQUAL(std::distance(utf_begin(s16), i16), 14);
        TRY(i16 = str_find_last_char(s16, U'€'));             TEST_EQUAL(std::distance(utf_begin(s16), i16), 10);
        TRY(i16 = str_find_last_char(s16, U'o'));             TEST_EQUAL(std::distance(utf_begin(s16), i16), 13);
        TRY(i16 = str_find_last_char(s16, U'z'));             TEST_EQUAL(std::distance(utf_begin(s16), i16), 14);
        TRY(i16 = str_find_last_char(s16, U'§'));             TEST_EQUAL(std::distance(utf_begin(s16), i16), 14);
        TRY(i16 = str_find_char(utf_range(s16), U'€'));       TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_find_char(utf_range(s16), U'o'));       TEST_EQUAL(std::distance(utf_begin(s16), i16), 3);
        TRY(i16 = str_find_char(utf_range(s16), U'z'));       TEST_EQUAL(std::distance(utf_begin(s16), i16), 14);
        TRY(i16 = str_find_char(utf_range(s16), U'§'));       TEST_EQUAL(std::distance(utf_begin(s16), i16), 14);
        TRY(i16 = str_find_last_char(utf_range(s16), U'€'));  TEST_EQUAL(std::distance(utf_begin(s16), i16), 10);
        TRY(i16 = str_find_last_char(utf_range(s16), U'o'));  TEST_EQUAL(std::distance(utf_begin(s16), i16), 13);
        TRY(i16 = str_find_last_char(utf_range(s16), U'z'));  TEST_EQUAL(std::distance(utf_begin(s16), i16), 14);
        TRY(i16 = str_find_last_char(utf_range(s16), U'§'));  TEST_EQUAL(std::distance(utf_begin(s16), i16), 14);

        s32 = U"€uro €uro €uro";
        TRY(i32 = str_find_char(s32, U'€'));                  TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_find_char(s32, U'o'));                  TEST_EQUAL(std::distance(utf_begin(s32), i32), 3);
        TRY(i32 = str_find_char(s32, U'z'));                  TEST_EQUAL(std::distance(utf_begin(s32), i32), 14);
        TRY(i32 = str_find_char(s32, U'§'));                  TEST_EQUAL(std::distance(utf_begin(s32), i32), 14);
        TRY(i32 = str_find_last_char(s32, U'€'));             TEST_EQUAL(std::distance(utf_begin(s32), i32), 10);
        TRY(i32 = str_find_last_char(s32, U'o'));             TEST_EQUAL(std::distance(utf_begin(s32), i32), 13);
        TRY(i32 = str_find_last_char(s32, U'z'));             TEST_EQUAL(std::distance(utf_begin(s32), i32), 14);
        TRY(i32 = str_find_last_char(s32, U'§'));             TEST_EQUAL(std::distance(utf_begin(s32), i32), 14);
        TRY(i32 = str_find_char(utf_range(s32), U'€'));       TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_find_char(utf_range(s32), U'o'));       TEST_EQUAL(std::distance(utf_begin(s32), i32), 3);
        TRY(i32 = str_find_char(utf_range(s32), U'z'));       TEST_EQUAL(std::distance(utf_begin(s32), i32), 14);
        TRY(i32 = str_find_char(utf_range(s32), U'§'));       TEST_EQUAL(std::distance(utf_begin(s32), i32), 14);
        TRY(i32 = str_find_last_char(utf_range(s32), U'€'));  TEST_EQUAL(std::distance(utf_begin(s32), i32), 10);
        TRY(i32 = str_find_last_char(utf_range(s32), U'o'));  TEST_EQUAL(std::distance(utf_begin(s32), i32), 13);
        TRY(i32 = str_find_last_char(utf_range(s32), U'z'));  TEST_EQUAL(std::distance(utf_begin(s32), i32), 14);
        TRY(i32 = str_find_last_char(utf_range(s32), U'§'));  TEST_EQUAL(std::distance(utf_begin(s32), i32), 14);

    }

    void check_find_first() {

        u8string s8;
        u16string s16;
        u32string s32;
        UtfIterator<char> i8;
        UtfIterator<char16_t> i16;
        UtfIterator<char32_t> i32;

        s8 = u8"€uro ∈lement";
        TRY(i8 = str_find_first_of(s8, u8"€∈"));                           TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_find_first_of(s8, u8"jklmn"));                        TEST_EQUAL(std::distance(utf_begin(s8), i8), 6);
        TRY(i8 = str_find_first_of(s8, u8"vwxyz"));                        TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);
        TRY(i8 = str_find_first_of(utf_range(s8), u8"€∈"));                TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_find_first_of(utf_range(s8), u8"jklmn"));             TEST_EQUAL(std::distance(utf_begin(s8), i8), 6);
        TRY(i8 = str_find_first_of(utf_range(s8), u8"vwxyz"));             TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);
        TRY(i8 = str_find_first_not_of(s8, u8"abcde"));                    TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_find_first_not_of(s8, u8"€uro"));                     TEST_EQUAL(std::distance(utf_begin(s8), i8), 4);
        TRY(i8 = str_find_first_not_of(s8, u8"€uro ∈lement"));             TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);
        TRY(i8 = str_find_first_not_of(utf_range(s8), u8"abcde"));         TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_find_first_not_of(utf_range(s8), u8"€uro"));          TEST_EQUAL(std::distance(utf_begin(s8), i8), 4);
        TRY(i8 = str_find_first_not_of(utf_range(s8), u8"€uro ∈lement"));  TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);
        TRY(i8 = str_find_last_of(s8, u8"€∈"));                            TEST_EQUAL(std::distance(utf_begin(s8), i8), 5);
        TRY(i8 = str_find_last_of(s8, u8"jklmn"));                         TEST_EQUAL(std::distance(utf_begin(s8), i8), 10);
        TRY(i8 = str_find_last_of(s8, u8"vwxyz"));                         TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);
        TRY(i8 = str_find_last_of(utf_range(s8), u8"€∈"));                 TEST_EQUAL(std::distance(utf_begin(s8), i8), 5);
        TRY(i8 = str_find_last_of(utf_range(s8), u8"jklmn"));              TEST_EQUAL(std::distance(utf_begin(s8), i8), 10);
        TRY(i8 = str_find_last_of(utf_range(s8), u8"vwxyz"));              TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);
        TRY(i8 = str_find_last_not_of(s8, u8"abcde"));                     TEST_EQUAL(std::distance(utf_begin(s8), i8), 11);
        TRY(i8 = str_find_last_not_of(s8, u8"∈lement"));                   TEST_EQUAL(std::distance(utf_begin(s8), i8), 4);
        TRY(i8 = str_find_last_not_of(s8, u8"€uro ∈lement"));              TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);
        TRY(i8 = str_find_last_not_of(utf_range(s8), u8"abcde"));          TEST_EQUAL(std::distance(utf_begin(s8), i8), 11);
        TRY(i8 = str_find_last_not_of(utf_range(s8), u8"∈lement"));        TEST_EQUAL(std::distance(utf_begin(s8), i8), 4);
        TRY(i8 = str_find_last_not_of(utf_range(s8), u8"€uro ∈lement"));   TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);

        s16 = u"€uro ∈lement";
        TRY(i16 = str_find_first_of(s16, u"€∈"));                           TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_find_first_of(s16, u"jklmn"));                        TEST_EQUAL(std::distance(utf_begin(s16), i16), 6);
        TRY(i16 = str_find_first_of(s16, u"vwxyz"));                        TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);
        TRY(i16 = str_find_first_of(utf_range(s16), u"€∈"));                TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_find_first_of(utf_range(s16), u"jklmn"));             TEST_EQUAL(std::distance(utf_begin(s16), i16), 6);
        TRY(i16 = str_find_first_of(utf_range(s16), u"vwxyz"));             TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);
        TRY(i16 = str_find_first_not_of(s16, u"abcde"));                    TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_find_first_not_of(s16, u"€uro"));                     TEST_EQUAL(std::distance(utf_begin(s16), i16), 4);
        TRY(i16 = str_find_first_not_of(s16, u"€uro ∈lement"));             TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);
        TRY(i16 = str_find_first_not_of(utf_range(s16), u"abcde"));         TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_find_first_not_of(utf_range(s16), u"€uro"));          TEST_EQUAL(std::distance(utf_begin(s16), i16), 4);
        TRY(i16 = str_find_first_not_of(utf_range(s16), u"€uro ∈lement"));  TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);
        TRY(i16 = str_find_last_of(s16, u"€∈"));                            TEST_EQUAL(std::distance(utf_begin(s16), i16), 5);
        TRY(i16 = str_find_last_of(s16, u"jklmn"));                         TEST_EQUAL(std::distance(utf_begin(s16), i16), 10);
        TRY(i16 = str_find_last_of(s16, u"vwxyz"));                         TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);
        TRY(i16 = str_find_last_of(utf_range(s16), u"€∈"));                 TEST_EQUAL(std::distance(utf_begin(s16), i16), 5);
        TRY(i16 = str_find_last_of(utf_range(s16), u"jklmn"));              TEST_EQUAL(std::distance(utf_begin(s16), i16), 10);
        TRY(i16 = str_find_last_of(utf_range(s16), u"vwxyz"));              TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);
        TRY(i16 = str_find_last_not_of(s16, u"abcde"));                     TEST_EQUAL(std::distance(utf_begin(s16), i16), 11);
        TRY(i16 = str_find_last_not_of(s16, u"∈lement"));                   TEST_EQUAL(std::distance(utf_begin(s16), i16), 4);
        TRY(i16 = str_find_last_not_of(s16, u"€uro ∈lement"));              TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);
        TRY(i16 = str_find_last_not_of(utf_range(s16), u"abcde"));          TEST_EQUAL(std::distance(utf_begin(s16), i16), 11);
        TRY(i16 = str_find_last_not_of(utf_range(s16), u"∈lement"));        TEST_EQUAL(std::distance(utf_begin(s16), i16), 4);
        TRY(i16 = str_find_last_not_of(utf_range(s16), u"€uro ∈lement"));   TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);

        s32 = U"€uro ∈lement";
        TRY(i32 = str_find_first_of(s32, U"€∈"));                           TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_find_first_of(s32, U"jklmn"));                        TEST_EQUAL(std::distance(utf_begin(s32), i32), 6);
        TRY(i32 = str_find_first_of(s32, U"vwxyz"));                        TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);
        TRY(i32 = str_find_first_of(utf_range(s32), U"€∈"));                TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_find_first_of(utf_range(s32), U"jklmn"));             TEST_EQUAL(std::distance(utf_begin(s32), i32), 6);
        TRY(i32 = str_find_first_of(utf_range(s32), U"vwxyz"));             TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);
        TRY(i32 = str_find_first_not_of(s32, U"abcde"));                    TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_find_first_not_of(s32, U"€uro"));                     TEST_EQUAL(std::distance(utf_begin(s32), i32), 4);
        TRY(i32 = str_find_first_not_of(s32, U"€uro ∈lement"));             TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);
        TRY(i32 = str_find_first_not_of(utf_range(s32), U"abcde"));         TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_find_first_not_of(utf_range(s32), U"€uro"));          TEST_EQUAL(std::distance(utf_begin(s32), i32), 4);
        TRY(i32 = str_find_first_not_of(utf_range(s32), U"€uro ∈lement"));  TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);
        TRY(i32 = str_find_last_of(s32, U"€∈"));                            TEST_EQUAL(std::distance(utf_begin(s32), i32), 5);
        TRY(i32 = str_find_last_of(s32, U"jklmn"));                         TEST_EQUAL(std::distance(utf_begin(s32), i32), 10);
        TRY(i32 = str_find_last_of(s32, U"vwxyz"));                         TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);
        TRY(i32 = str_find_last_of(utf_range(s32), U"€∈"));                 TEST_EQUAL(std::distance(utf_begin(s32), i32), 5);
        TRY(i32 = str_find_last_of(utf_range(s32), U"jklmn"));              TEST_EQUAL(std::distance(utf_begin(s32), i32), 10);
        TRY(i32 = str_find_last_of(utf_range(s32), U"vwxyz"));              TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);
        TRY(i32 = str_find_last_not_of(s32, U"abcde"));                     TEST_EQUAL(std::distance(utf_begin(s32), i32), 11);
        TRY(i32 = str_find_last_not_of(s32, U"∈lement"));                   TEST_EQUAL(std::distance(utf_begin(s32), i32), 4);
        TRY(i32 = str_find_last_not_of(s32, U"€uro ∈lement"));              TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);
        TRY(i32 = str_find_last_not_of(utf_range(s32), U"abcde"));          TEST_EQUAL(std::distance(utf_begin(s32), i32), 11);
        TRY(i32 = str_find_last_not_of(utf_range(s32), U"∈lement"));        TEST_EQUAL(std::distance(utf_begin(s32), i32), 4);
        TRY(i32 = str_find_last_not_of(utf_range(s32), U"€uro ∈lement"));   TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);

    }

    void check_search() {

        u8string s8;
        u16string s16;
        u32string s32;
        UtfIterator<char> i8;
        UtfIterator<char16_t> i16;
        UtfIterator<char32_t> i32;

        s8 = u8"€uro ∈lement";
        TRY(i8 = str_search(s8, u8""));                    TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_search(s8, u8"€uro"));                TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_search(s8, u8"∈lement"));             TEST_EQUAL(std::distance(utf_begin(s8), i8), 5);
        TRY(i8 = str_search(s8, u8"Hello"));               TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);
        TRY(i8 = str_search(utf_range(s8), u8""));         TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_search(utf_range(s8), u8"€uro"));     TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        TRY(i8 = str_search(utf_range(s8), u8"∈lement"));  TEST_EQUAL(std::distance(utf_begin(s8), i8), 5);
        TRY(i8 = str_search(utf_range(s8), u8"Hello"));    TEST_EQUAL(std::distance(utf_begin(s8), i8), 12);

        s16 = u"€uro ∈lement";
        TRY(i16 = str_search(s16, u""));                    TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_search(s16, u"€uro"));                TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_search(s16, u"∈lement"));             TEST_EQUAL(std::distance(utf_begin(s16), i16), 5);
        TRY(i16 = str_search(s16, u"Hello"));               TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);
        TRY(i16 = str_search(utf_range(s16), u""));         TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_search(utf_range(s16), u"€uro"));     TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        TRY(i16 = str_search(utf_range(s16), u"∈lement"));  TEST_EQUAL(std::distance(utf_begin(s16), i16), 5);
        TRY(i16 = str_search(utf_range(s16), u"Hello"));    TEST_EQUAL(std::distance(utf_begin(s16), i16), 12);

        s32 = U"€uro ∈lement";
        TRY(i32 = str_search(s32, U""));                    TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_search(s32, U"€uro"));                TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_search(s32, U"∈lement"));             TEST_EQUAL(std::distance(utf_begin(s32), i32), 5);
        TRY(i32 = str_search(s32, U"Hello"));               TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);
        TRY(i32 = str_search(utf_range(s32), U""));         TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_search(utf_range(s32), U"€uro"));     TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        TRY(i32 = str_search(utf_range(s32), U"∈lement"));  TEST_EQUAL(std::distance(utf_begin(s32), i32), 5);
        TRY(i32 = str_search(utf_range(s32), U"Hello"));    TEST_EQUAL(std::distance(utf_begin(s32), i32), 12);

    }

    void check_skipws() {

        u8string s8;
        u16string s16;
        u32string s32;
        UtfIterator<char> i8;
        UtfIterator<char16_t> i16;
        UtfIterator<char32_t> i32;

        s8 = u8""s;                   TRY(i8 = utf_begin(s8));  TEST_EQUAL(str_skipws(i8), 0);  TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        s8 = u8"Hello world"s;        TRY(i8 = utf_begin(s8));  TEST_EQUAL(str_skipws(i8), 0);  TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        s8 = u8" Hello "s;            TRY(i8 = utf_begin(s8));  TEST_EQUAL(str_skipws(i8), 1);  TEST_EQUAL(std::distance(utf_begin(s8), i8), 1);
        s8 = u8" \r\n Hello \r\n "s;  TRY(i8 = utf_begin(s8));  TEST_EQUAL(str_skipws(i8), 4);  TEST_EQUAL(std::distance(utf_begin(s8), i8), 4);
        s8 = u8"€uro ∈lement"s;       TRY(i8 = utf_begin(s8));  TEST_EQUAL(str_skipws(i8), 0);  TEST_EQUAL(std::distance(utf_begin(s8), i8), 0);
        s8 = u8" €uro "s;             TRY(i8 = utf_begin(s8));  TEST_EQUAL(str_skipws(i8), 1);  TEST_EQUAL(std::distance(utf_begin(s8), i8), 1);
        s8 = u8" \r\n €uro \r\n "s;   TRY(i8 = utf_begin(s8));  TEST_EQUAL(str_skipws(i8), 4);  TEST_EQUAL(std::distance(utf_begin(s8), i8), 4);

        s16 = u""s;                   TRY(i16 = utf_begin(s16));  TEST_EQUAL(str_skipws(i16), 0);  TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        s16 = u"Hello world"s;        TRY(i16 = utf_begin(s16));  TEST_EQUAL(str_skipws(i16), 0);  TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        s16 = u" Hello "s;            TRY(i16 = utf_begin(s16));  TEST_EQUAL(str_skipws(i16), 1);  TEST_EQUAL(std::distance(utf_begin(s16), i16), 1);
        s16 = u" \r\n Hello \r\n "s;  TRY(i16 = utf_begin(s16));  TEST_EQUAL(str_skipws(i16), 4);  TEST_EQUAL(std::distance(utf_begin(s16), i16), 4);
        s16 = u"€uro ∈lement"s;       TRY(i16 = utf_begin(s16));  TEST_EQUAL(str_skipws(i16), 0);  TEST_EQUAL(std::distance(utf_begin(s16), i16), 0);
        s16 = u" €uro "s;             TRY(i16 = utf_begin(s16));  TEST_EQUAL(str_skipws(i16), 1);  TEST_EQUAL(std::distance(utf_begin(s16), i16), 1);
        s16 = u" \r\n €uro \r\n "s;   TRY(i16 = utf_begin(s16));  TEST_EQUAL(str_skipws(i16), 4);  TEST_EQUAL(std::distance(utf_begin(s16), i16), 4);

        s32 = U""s;                   TRY(i32 = utf_begin(s32));  TEST_EQUAL(str_skipws(i32), 0);  TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        s32 = U"Hello world"s;        TRY(i32 = utf_begin(s32));  TEST_EQUAL(str_skipws(i32), 0);  TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        s32 = U" Hello "s;            TRY(i32 = utf_begin(s32));  TEST_EQUAL(str_skipws(i32), 1);  TEST_EQUAL(std::distance(utf_begin(s32), i32), 1);
        s32 = U" \r\n Hello \r\n "s;  TRY(i32 = utf_begin(s32));  TEST_EQUAL(str_skipws(i32), 4);  TEST_EQUAL(std::distance(utf_begin(s32), i32), 4);
        s32 = U"€uro ∈lement"s;       TRY(i32 = utf_begin(s32));  TEST_EQUAL(str_skipws(i32), 0);  TEST_EQUAL(std::distance(utf_begin(s32), i32), 0);
        s32 = U" €uro "s;             TRY(i32 = utf_begin(s32));  TEST_EQUAL(str_skipws(i32), 1);  TEST_EQUAL(std::distance(utf_begin(s32), i32), 1);
        s32 = U" \r\n €uro \r\n "s;   TRY(i32 = utf_begin(s32));  TEST_EQUAL(str_skipws(i32), 4);  TEST_EQUAL(std::distance(utf_begin(s32), i32), 4);

    }

}

TEST_MODULE(unicorn, string_algorithm) {

    check_common();
    check_compare();
    check_expect();
    check_find_char();
    check_find_first();
    check_search();
    check_skipws();

}
