#include "unicorn/string.hpp"
#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <iterator>
#include <random>
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void check_common() {

        u8string a, b = "Hello", c = "Hello world", d = "Hellfire", e = "Goodbye";
        size_t n = 0;

        TRY(n = str_common(a, b));      TEST_EQUAL(n, 0);
        TRY(n = str_common(b, c));      TEST_EQUAL(n, 5);
        TRY(n = str_common(b, d));      TEST_EQUAL(n, 4);
        TRY(n = str_common(b, e));      TEST_EQUAL(n, 0);
        TRY(n = str_common(b, d, 3));   TEST_EQUAL(n, 1);
        TRY(n = str_common(b, d, 4));   TEST_EQUAL(n, 0);
        TRY(n = str_common(b, d, 5));   TEST_EQUAL(n, 0);
        TRY(n = str_common(b, d, 42));  TEST_EQUAL(n, 0);

        // "αβγδ" = Unicode 3b1 3b1 3b1 3c4 = UTF-8 ce b1 ce b2 ce b3 ce b4
        // "αβδε" = Unicode 3b1 3b1 3c4 3c5 = UTF-8 ce b1 ce b2 ce b4 ce b5

        u8string x = u8"αβγδ", y = u8"αβδε";

        TRY(n = str_common(x, y));      TEST_EQUAL(n, 5);
        TRY(n = str_common_utf(x, y));  TEST_EQUAL(n, 4);

    }

    void check_expect() {

        u8string a, b = u8"Hello world", c = u8"€uro ∈lement";
        Utf8Iterator i;

        TRY(i = utf_begin(a));  TEST(! str_expect(i, u8""s));                TEST_EQUAL(std::distance(utf_begin(a), i), 0);
        TRY(i = utf_begin(a));  TEST(! str_expect(i, u8"Hello"s));           TEST_EQUAL(std::distance(utf_begin(a), i), 0);
        TRY(i = utf_begin(a));  TEST(! str_expect(i, u8"€uro"s));            TEST_EQUAL(std::distance(utf_begin(a), i), 0);
        TRY(i = utf_begin(b));  TEST(! str_expect(i, u8""s));                TEST_EQUAL(std::distance(utf_begin(b), i), 0);
        TRY(i = utf_begin(b));  TEST(str_expect(i, u8"Hello"s));             TEST_EQUAL(std::distance(utf_begin(b), i), 5);
        TRY(i = utf_begin(b));  TEST(str_expect(i, u8"Hello world"s));       TEST_EQUAL(std::distance(utf_begin(b), i), 11);
        TRY(i = utf_begin(b));  TEST(! str_expect(i, u8"Hello world 2"s));   TEST_EQUAL(std::distance(utf_begin(b), i), 0);
        TRY(i = utf_begin(b));  TEST(! str_expect(i, u8"world"s));           TEST_EQUAL(std::distance(utf_begin(b), i), 0);
        TRY(i = utf_begin(b));  TEST(! str_expect(i, u8"€uro"s));            TEST_EQUAL(std::distance(utf_begin(b), i), 0);
        TRY(i = utf_begin(c));  TEST(! str_expect(i, u8""s));                TEST_EQUAL(std::distance(utf_begin(c), i), 0);
        TRY(i = utf_begin(c));  TEST(! str_expect(i, u8"Hello"s));           TEST_EQUAL(std::distance(utf_begin(c), i), 0);
        TRY(i = utf_begin(c));  TEST(str_expect(i, u8"€uro"s));              TEST_EQUAL(std::distance(utf_begin(c), i), 4);
        TRY(i = utf_begin(c));  TEST(str_expect(i, u8"€uro ∈lement"s));      TEST_EQUAL(std::distance(utf_begin(c), i), 12);
        TRY(i = utf_begin(c));  TEST(! str_expect(i, u8"€uro ∈lement 2"s));  TEST_EQUAL(std::distance(utf_begin(c), i), 0);
        TRY(i = utf_begin(c));  TEST(! str_expect(i, u8"∈lement"s));         TEST_EQUAL(std::distance(utf_begin(c), i), 0);

    }

    void check_find_char() {

        u8string s;
        Utf8Iterator i;

        s = u8"€uro €uro €uro";
        TRY(i = str_find_char(s, U'€'));                  TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_find_char(s, U'o'));                  TEST_EQUAL(std::distance(utf_begin(s), i), 3);
        TRY(i = str_find_char(s, U'z'));                  TEST_EQUAL(std::distance(utf_begin(s), i), 14);
        TRY(i = str_find_char(s, U'§'));                  TEST_EQUAL(std::distance(utf_begin(s), i), 14);
        TRY(i = str_find_last_char(s, U'€'));             TEST_EQUAL(std::distance(utf_begin(s), i), 10);
        TRY(i = str_find_last_char(s, U'o'));             TEST_EQUAL(std::distance(utf_begin(s), i), 13);
        TRY(i = str_find_last_char(s, U'z'));             TEST_EQUAL(std::distance(utf_begin(s), i), 14);
        TRY(i = str_find_last_char(s, U'§'));             TEST_EQUAL(std::distance(utf_begin(s), i), 14);
        TRY(i = str_find_char(utf_range(s), U'€'));       TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_find_char(utf_range(s), U'o'));       TEST_EQUAL(std::distance(utf_begin(s), i), 3);
        TRY(i = str_find_char(utf_range(s), U'z'));       TEST_EQUAL(std::distance(utf_begin(s), i), 14);
        TRY(i = str_find_char(utf_range(s), U'§'));       TEST_EQUAL(std::distance(utf_begin(s), i), 14);
        TRY(i = str_find_last_char(utf_range(s), U'€'));  TEST_EQUAL(std::distance(utf_begin(s), i), 10);
        TRY(i = str_find_last_char(utf_range(s), U'o'));  TEST_EQUAL(std::distance(utf_begin(s), i), 13);
        TRY(i = str_find_last_char(utf_range(s), U'z'));  TEST_EQUAL(std::distance(utf_begin(s), i), 14);
        TRY(i = str_find_last_char(utf_range(s), U'§'));  TEST_EQUAL(std::distance(utf_begin(s), i), 14);

    }

    void check_find_first() {

        u8string s;
        Utf8Iterator i;

        s = u8"€uro ∈lement";
        TRY(i = str_find_first_of(s, u8"€∈"));                           TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_find_first_of(s, u8"jklmn"));                        TEST_EQUAL(std::distance(utf_begin(s), i), 6);
        TRY(i = str_find_first_of(s, u8"vwxyz"));                        TEST_EQUAL(std::distance(utf_begin(s), i), 12);
        TRY(i = str_find_first_of(utf_range(s), u8"€∈"));                TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_find_first_of(utf_range(s), u8"jklmn"));             TEST_EQUAL(std::distance(utf_begin(s), i), 6);
        TRY(i = str_find_first_of(utf_range(s), u8"vwxyz"));             TEST_EQUAL(std::distance(utf_begin(s), i), 12);
        TRY(i = str_find_first_not_of(s, u8"abcde"));                    TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_find_first_not_of(s, u8"€uro"));                     TEST_EQUAL(std::distance(utf_begin(s), i), 4);
        TRY(i = str_find_first_not_of(s, u8"€uro ∈lement"));             TEST_EQUAL(std::distance(utf_begin(s), i), 12);
        TRY(i = str_find_first_not_of(utf_range(s), u8"abcde"));         TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_find_first_not_of(utf_range(s), u8"€uro"));          TEST_EQUAL(std::distance(utf_begin(s), i), 4);
        TRY(i = str_find_first_not_of(utf_range(s), u8"€uro ∈lement"));  TEST_EQUAL(std::distance(utf_begin(s), i), 12);
        TRY(i = str_find_last_of(s, u8"€∈"));                            TEST_EQUAL(std::distance(utf_begin(s), i), 5);
        TRY(i = str_find_last_of(s, u8"jklmn"));                         TEST_EQUAL(std::distance(utf_begin(s), i), 10);
        TRY(i = str_find_last_of(s, u8"vwxyz"));                         TEST_EQUAL(std::distance(utf_begin(s), i), 12);
        TRY(i = str_find_last_of(utf_range(s), u8"€∈"));                 TEST_EQUAL(std::distance(utf_begin(s), i), 5);
        TRY(i = str_find_last_of(utf_range(s), u8"jklmn"));              TEST_EQUAL(std::distance(utf_begin(s), i), 10);
        TRY(i = str_find_last_of(utf_range(s), u8"vwxyz"));              TEST_EQUAL(std::distance(utf_begin(s), i), 12);
        TRY(i = str_find_last_not_of(s, u8"abcde"));                     TEST_EQUAL(std::distance(utf_begin(s), i), 11);
        TRY(i = str_find_last_not_of(s, u8"∈lement"));                   TEST_EQUAL(std::distance(utf_begin(s), i), 4);
        TRY(i = str_find_last_not_of(s, u8"€uro ∈lement"));              TEST_EQUAL(std::distance(utf_begin(s), i), 12);
        TRY(i = str_find_last_not_of(utf_range(s), u8"abcde"));          TEST_EQUAL(std::distance(utf_begin(s), i), 11);
        TRY(i = str_find_last_not_of(utf_range(s), u8"∈lement"));        TEST_EQUAL(std::distance(utf_begin(s), i), 4);
        TRY(i = str_find_last_not_of(utf_range(s), u8"€uro ∈lement"));   TEST_EQUAL(std::distance(utf_begin(s), i), 12);

    }

    void check_search() {

        u8string s;
        Utf8Iterator i;

        s = u8"€uro ∈lement";
        TRY(i = str_search(s, u8""));                    TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_search(s, u8"€uro"));                TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_search(s, u8"∈lement"));             TEST_EQUAL(std::distance(utf_begin(s), i), 5);
        TRY(i = str_search(s, u8"Hello"));               TEST_EQUAL(std::distance(utf_begin(s), i), 12);
        TRY(i = str_search(utf_range(s), u8""));         TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_search(utf_range(s), u8"€uro"));     TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        TRY(i = str_search(utf_range(s), u8"∈lement"));  TEST_EQUAL(std::distance(utf_begin(s), i), 5);
        TRY(i = str_search(utf_range(s), u8"Hello"));    TEST_EQUAL(std::distance(utf_begin(s), i), 12);

    }

    void check_skipws() {

        u8string s;
        Utf8Iterator i;

        s = u8""s;                   TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 0);  TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        s = u8"Hello world"s;        TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 0);  TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        s = u8" Hello "s;            TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 1);  TEST_EQUAL(std::distance(utf_begin(s), i), 1);
        s = u8" \r\n Hello \r\n "s;  TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 4);  TEST_EQUAL(std::distance(utf_begin(s), i), 4);
        s = u8"€uro ∈lement"s;       TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 0);  TEST_EQUAL(std::distance(utf_begin(s), i), 0);
        s = u8" €uro "s;             TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 1);  TEST_EQUAL(std::distance(utf_begin(s), i), 1);
        s = u8" \r\n €uro \r\n "s;   TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 4);  TEST_EQUAL(std::distance(utf_begin(s), i), 4);

    }

}

TEST_MODULE(unicorn, string_algorithm) {

    check_common();
    check_expect();
    check_find_char();
    check_find_first();
    check_search();
    check_skipws();

}
