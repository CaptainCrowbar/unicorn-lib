#include "unicorn/string.hpp"
#include "unicorn/core.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_compare_3_way() {

    int rc = 0;

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

void test_unicorn_string_compare_case_insensitive() {

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

void test_unicorn_string_compare_natural() {

    U8string s0 = "";
    U8string s1 = "abc 123";
    U8string s2 = "abc 45";
    U8string s3 = "ABC 67";
    U8string s4 = "abc 000123";
    U8string s5 = "abc 123 string_compare";
    U8string s6 = "abc 123 456";
    U8string s7 = "+abc 123";

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

void test_unicorn_string_compare_utf_compare() {

    U8string a8 = u8"\uff00\U00010000";
    U8string b8 = u8"\U00010000\uff00";
    std::u16string a16 = u"\uff00\U00010000";
    std::u16string b16 = u"\U00010000\uff00";
    std::u32string a32 = U"\uff00\U00010000";
    std::u32string b32 = U"\U00010000\uff00";

    TEST(! utf_compare(a8, a8));
    TEST(utf_compare(a8, b8));
    TEST(! utf_compare(b8, a8));
    TEST(! utf_compare(b8, b8));
    TEST(! utf_compare(a16, a16));
    TEST(utf_compare(a16, b16));
    TEST(! utf_compare(b16, a16));
    TEST(! utf_compare(b16, b16));
    TEST(! utf_compare(a32, a32));
    TEST(utf_compare(a32, b32));
    TEST(! utf_compare(b32, a32));
    TEST(! utf_compare(b32, b32));

}
