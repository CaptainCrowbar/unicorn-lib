#include "unicorn/string.hpp"
#include "unicorn/core.hpp"
#include "prion/unit-test.hpp"
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

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

    check_compare_3way();
    check_icase_compare();
    check_natural_compare();

}
