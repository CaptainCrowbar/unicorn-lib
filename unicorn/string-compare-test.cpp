#include "unicorn/string-compare.hpp"
#include "unicorn/core.hpp"
#include "prion/unit-test.hpp"
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

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

    }

    void check_compare_3way() {

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

    void check_icase_compare() {

        TEST(! str_icase_compare(u8""s, u8""s));
        TEST(! str_icase_compare(u8"hello"s, u8"hello"s));
        TEST(! str_icase_compare(u8"Hello"s, u8"hello"s));
        TEST(! str_icase_compare(u8"hello"s, u8"Hello"s));
        TEST(! str_icase_compare(u8"HELLO"s, u8"hello"s));
        TEST(! str_icase_compare(u8"hello"s, u8"HELLO"s));
        TEST(str_icase_compare(u8""s, u8"hello"s));
        TEST(! str_icase_compare(u8"hello"s, u8""s));
        TEST(str_icase_compare(u8"hello"s, u8"WORLD"s));
        TEST(str_icase_compare(u8"HELLO"s, u8"world"s));
        TEST(! str_icase_compare(u8"world"s, u8"HELLO"s));
        TEST(! str_icase_compare(u8"WORLD"s, u8"hello"s));
        TEST(str_icase_compare(u8"hello"s, u8"HELLO WORLD"s));
        TEST(str_icase_compare(u8"HELLO"s, u8"hello world"s));
        TEST(! str_icase_compare(u8"hello world"s, u8"HELLO"s));
        TEST(! str_icase_compare(u8"HELLO WORLD"s, u8"hello"s));
        TEST(str_icase_compare(u8"hello world"s, u8"HELLO!WORLD"s));
        TEST(str_icase_compare(u8"HELLO WORLD"s, u8"hello!world"s));

        TEST(! str_icase_compare(u""s, u""s));
        TEST(! str_icase_compare(u"hello"s, u"hello"s));
        TEST(! str_icase_compare(u"Hello"s, u"hello"s));
        TEST(! str_icase_compare(u"hello"s, u"Hello"s));
        TEST(! str_icase_compare(u"HELLO"s, u"hello"s));
        TEST(! str_icase_compare(u"hello"s, u"HELLO"s));
        TEST(str_icase_compare(u""s, u"hello"s));
        TEST(! str_icase_compare(u"hello"s, u""s));
        TEST(str_icase_compare(u"hello"s, u"WORLD"s));
        TEST(str_icase_compare(u"HELLO"s, u"world"s));
        TEST(! str_icase_compare(u"world"s, u"HELLO"s));
        TEST(! str_icase_compare(u"WORLD"s, u"hello"s));
        TEST(str_icase_compare(u"hello"s, u"HELLO WORLD"s));
        TEST(str_icase_compare(u"HELLO"s, u"hello world"s));
        TEST(! str_icase_compare(u"hello world"s, u"HELLO"s));
        TEST(! str_icase_compare(u"HELLO WORLD"s, u"hello"s));
        TEST(str_icase_compare(u"hello world"s, u"HELLO!WORLD"s));
        TEST(str_icase_compare(u"HELLO WORLD"s, u"hello!world"s));

        TEST(! str_icase_compare(U""s, U""s));
        TEST(! str_icase_compare(U"hello"s, U"hello"s));
        TEST(! str_icase_compare(U"Hello"s, U"hello"s));
        TEST(! str_icase_compare(U"hello"s, U"Hello"s));
        TEST(! str_icase_compare(U"HELLO"s, U"hello"s));
        TEST(! str_icase_compare(U"hello"s, U"HELLO"s));
        TEST(str_icase_compare(U""s, U"hello"s));
        TEST(! str_icase_compare(U"hello"s, U""s));
        TEST(str_icase_compare(U"hello"s, U"WORLD"s));
        TEST(str_icase_compare(U"HELLO"s, U"world"s));
        TEST(! str_icase_compare(U"world"s, U"HELLO"s));
        TEST(! str_icase_compare(U"WORLD"s, U"hello"s));
        TEST(str_icase_compare(U"hello"s, U"HELLO WORLD"s));
        TEST(str_icase_compare(U"HELLO"s, U"hello world"s));
        TEST(! str_icase_compare(U"hello world"s, U"HELLO"s));
        TEST(! str_icase_compare(U"HELLO WORLD"s, U"hello"s));
        TEST(str_icase_compare(U"hello world"s, U"HELLO!WORLD"s));
        TEST(str_icase_compare(U"HELLO WORLD"s, U"hello!world"s));

        TEST(str_icase_equal(u8""s, u8""s));
        TEST(str_icase_equal(u8"hello"s, u8"hello"s));
        TEST(str_icase_equal(u8"Hello"s, u8"hello"s));
        TEST(str_icase_equal(u8"hello"s, u8"Hello"s));
        TEST(str_icase_equal(u8"HELLO"s, u8"hello"s));
        TEST(str_icase_equal(u8"hello"s, u8"HELLO"s));
        TEST(! str_icase_equal(u8""s, u8"hello"s));
        TEST(! str_icase_equal(u8"hello"s, u8""s));
        TEST(! str_icase_equal(u8"hello"s, u8"WORLD"s));
        TEST(! str_icase_equal(u8"HELLO"s, u8"world"s));
        TEST(! str_icase_equal(u8"world"s, u8"HELLO"s));
        TEST(! str_icase_equal(u8"WORLD"s, u8"hello"s));
        TEST(! str_icase_equal(u8"hello"s, u8"HELLO WORLD"s));
        TEST(! str_icase_equal(u8"HELLO"s, u8"hello world"s));
        TEST(! str_icase_equal(u8"hello world"s, u8"HELLO"s));
        TEST(! str_icase_equal(u8"HELLO WORLD"s, u8"hello"s));
        TEST(! str_icase_equal(u8"hello world"s, u8"HELLO!WORLD"s));
        TEST(! str_icase_equal(u8"HELLO WORLD"s, u8"hello!world"s));

        TEST(str_icase_equal(u""s, u""s));
        TEST(str_icase_equal(u"hello"s, u"hello"s));
        TEST(str_icase_equal(u"Hello"s, u"hello"s));
        TEST(str_icase_equal(u"hello"s, u"Hello"s));
        TEST(str_icase_equal(u"HELLO"s, u"hello"s));
        TEST(str_icase_equal(u"hello"s, u"HELLO"s));
        TEST(! str_icase_equal(u""s, u"hello"s));
        TEST(! str_icase_equal(u"hello"s, u""s));
        TEST(! str_icase_equal(u"hello"s, u"WORLD"s));
        TEST(! str_icase_equal(u"HELLO"s, u"world"s));
        TEST(! str_icase_equal(u"world"s, u"HELLO"s));
        TEST(! str_icase_equal(u"WORLD"s, u"hello"s));
        TEST(! str_icase_equal(u"hello"s, u"HELLO WORLD"s));
        TEST(! str_icase_equal(u"HELLO"s, u"hello world"s));
        TEST(! str_icase_equal(u"hello world"s, u"HELLO"s));
        TEST(! str_icase_equal(u"HELLO WORLD"s, u"hello"s));
        TEST(! str_icase_equal(u"hello world"s, u"HELLO!WORLD"s));
        TEST(! str_icase_equal(u"HELLO WORLD"s, u"hello!world"s));

        TEST(str_icase_equal(U""s, U""s));
        TEST(str_icase_equal(U"hello"s, U"hello"s));
        TEST(str_icase_equal(U"Hello"s, U"hello"s));
        TEST(str_icase_equal(U"hello"s, U"Hello"s));
        TEST(str_icase_equal(U"HELLO"s, U"hello"s));
        TEST(str_icase_equal(U"hello"s, U"HELLO"s));
        TEST(! str_icase_equal(U""s, U"hello"s));
        TEST(! str_icase_equal(U"hello"s, U""s));
        TEST(! str_icase_equal(U"hello"s, U"WORLD"s));
        TEST(! str_icase_equal(U"HELLO"s, U"world"s));
        TEST(! str_icase_equal(U"world"s, U"HELLO"s));
        TEST(! str_icase_equal(U"WORLD"s, U"hello"s));
        TEST(! str_icase_equal(U"hello"s, U"HELLO WORLD"s));
        TEST(! str_icase_equal(U"HELLO"s, U"hello world"s));
        TEST(! str_icase_equal(U"hello world"s, U"HELLO"s));
        TEST(! str_icase_equal(U"HELLO WORLD"s, U"hello"s));
        TEST(! str_icase_equal(U"hello world"s, U"HELLO!WORLD"s));
        TEST(! str_icase_equal(U"HELLO WORLD"s, U"hello!world"s));

    }

    void check_natural_compare() {

        u8string s0 = "";
        u8string s1 = "abc 123";
        u8string s2 = "abc 45";
        u8string s3 = "ABC 67";
        u8string s4 = "abc 000123";
        u8string s5 = "abc 123 xyz";
        u8string s6 = "abc 123 456";
        u8string s7 = "+abc 123";

        TEST(! str_natural_compare(s0, s0));
        TEST(! str_natural_compare(s1, s1));
        TEST(str_natural_compare(s0, s1));
        TEST(! str_natural_compare(s1, s0));
        TEST(! str_natural_compare(s1, s2));
        TEST(str_natural_compare(s2, s1));
        TEST(str_natural_compare(s2, s3));
        TEST(! str_natural_compare(s3, s2));
        TEST(str_natural_compare(s2, s4));
        TEST(! str_natural_compare(s4, s2));
        TEST(str_natural_compare(s1, s5));
        TEST(! str_natural_compare(s5, s1));
        TEST(str_natural_compare(s1, s6));
        TEST(! str_natural_compare(s6, s1));
        TEST(! str_natural_compare(s1, s7));
        TEST(str_natural_compare(s7, s1));

    }

}

TEST_MODULE(unicorn, string_compare) {

    check_compare();
    check_compare_3way();
    check_icase_compare();
    check_natural_compare();

}
