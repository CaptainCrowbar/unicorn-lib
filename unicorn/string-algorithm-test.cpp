#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <iterator>
#include <utility>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_algorithm_common() {

    Ustring a, b = "Hello", c = "Hello world", d = "Hellfire", e = "Goodbye";
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

    Ustring x = "αβγδ", y = "αβδε";

    TRY(n = str_common(x, y));      TEST_EQUAL(n, 5);
    TRY(n = str_common_utf(x, y));  TEST_EQUAL(n, 4);

}

void test_unicorn_string_algorithm_expect() {

    Ustring a, b = "Hello world", c = "€uro ∈lement";
    Utf8Iterator i;

    TRY(i = utf_begin(a));  TEST(! str_expect(i, ""s));                TEST_EQUAL(std::distance(utf_begin(a), i), 0);
    TRY(i = utf_begin(a));  TEST(! str_expect(i, "Hello"s));           TEST_EQUAL(std::distance(utf_begin(a), i), 0);
    TRY(i = utf_begin(a));  TEST(! str_expect(i, "€uro"s));            TEST_EQUAL(std::distance(utf_begin(a), i), 0);
    TRY(i = utf_begin(b));  TEST(! str_expect(i, ""s));                TEST_EQUAL(std::distance(utf_begin(b), i), 0);
    TRY(i = utf_begin(b));  TEST(str_expect(i, "Hello"s));             TEST_EQUAL(std::distance(utf_begin(b), i), 5);
    TRY(i = utf_begin(b));  TEST(str_expect(i, "Hello world"s));       TEST_EQUAL(std::distance(utf_begin(b), i), 11);
    TRY(i = utf_begin(b));  TEST(! str_expect(i, "Hello world 2"s));   TEST_EQUAL(std::distance(utf_begin(b), i), 0);
    TRY(i = utf_begin(b));  TEST(! str_expect(i, "world"s));           TEST_EQUAL(std::distance(utf_begin(b), i), 0);
    TRY(i = utf_begin(b));  TEST(! str_expect(i, "€uro"s));            TEST_EQUAL(std::distance(utf_begin(b), i), 0);
    TRY(i = utf_begin(c));  TEST(! str_expect(i, ""s));                TEST_EQUAL(std::distance(utf_begin(c), i), 0);
    TRY(i = utf_begin(c));  TEST(! str_expect(i, "Hello"s));           TEST_EQUAL(std::distance(utf_begin(c), i), 0);
    TRY(i = utf_begin(c));  TEST(str_expect(i, "€uro"s));              TEST_EQUAL(std::distance(utf_begin(c), i), 4);
    TRY(i = utf_begin(c));  TEST(str_expect(i, "€uro ∈lement"s));      TEST_EQUAL(std::distance(utf_begin(c), i), 12);
    TRY(i = utf_begin(c));  TEST(! str_expect(i, "€uro ∈lement 2"s));  TEST_EQUAL(std::distance(utf_begin(c), i), 0);
    TRY(i = utf_begin(c));  TEST(! str_expect(i, "∈lement"s));         TEST_EQUAL(std::distance(utf_begin(c), i), 0);

}

void test_unicorn_string_algorithm_find_char() {

    Ustring s;
    Utf8Iterator i;

    s = "€uro €uro €uro";
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

void test_unicorn_string_algorithm_find_first() {

    Ustring s;
    Utf8Iterator i;

    s = "€uro ∈lement";
    TRY(i = str_find_first_of(s, "€∈"));                           TEST_EQUAL(std::distance(utf_begin(s), i), 0);
    TRY(i = str_find_first_of(s, "jklmn"));                        TEST_EQUAL(std::distance(utf_begin(s), i), 6);
    TRY(i = str_find_first_of(s, "vwxyz"));                        TEST_EQUAL(std::distance(utf_begin(s), i), 12);
    TRY(i = str_find_first_of(utf_range(s), "€∈"));                TEST_EQUAL(std::distance(utf_begin(s), i), 0);
    TRY(i = str_find_first_of(utf_range(s), "jklmn"));             TEST_EQUAL(std::distance(utf_begin(s), i), 6);
    TRY(i = str_find_first_of(utf_range(s), "vwxyz"));             TEST_EQUAL(std::distance(utf_begin(s), i), 12);
    TRY(i = str_find_first_not_of(s, "abcde"));                    TEST_EQUAL(std::distance(utf_begin(s), i), 0);
    TRY(i = str_find_first_not_of(s, "€uro"));                     TEST_EQUAL(std::distance(utf_begin(s), i), 4);
    TRY(i = str_find_first_not_of(s, "€uro ∈lement"));             TEST_EQUAL(std::distance(utf_begin(s), i), 12);
    TRY(i = str_find_first_not_of(utf_range(s), "abcde"));         TEST_EQUAL(std::distance(utf_begin(s), i), 0);
    TRY(i = str_find_first_not_of(utf_range(s), "€uro"));          TEST_EQUAL(std::distance(utf_begin(s), i), 4);
    TRY(i = str_find_first_not_of(utf_range(s), "€uro ∈lement"));  TEST_EQUAL(std::distance(utf_begin(s), i), 12);
    TRY(i = str_find_last_of(s, "€∈"));                            TEST_EQUAL(std::distance(utf_begin(s), i), 5);
    TRY(i = str_find_last_of(s, "jklmn"));                         TEST_EQUAL(std::distance(utf_begin(s), i), 10);
    TRY(i = str_find_last_of(s, "vwxyz"));                         TEST_EQUAL(std::distance(utf_begin(s), i), 12);
    TRY(i = str_find_last_of(utf_range(s), "€∈"));                 TEST_EQUAL(std::distance(utf_begin(s), i), 5);
    TRY(i = str_find_last_of(utf_range(s), "jklmn"));              TEST_EQUAL(std::distance(utf_begin(s), i), 10);
    TRY(i = str_find_last_of(utf_range(s), "vwxyz"));              TEST_EQUAL(std::distance(utf_begin(s), i), 12);
    TRY(i = str_find_last_not_of(s, "abcde"));                     TEST_EQUAL(std::distance(utf_begin(s), i), 11);
    TRY(i = str_find_last_not_of(s, "∈lement"));                   TEST_EQUAL(std::distance(utf_begin(s), i), 4);
    TRY(i = str_find_last_not_of(s, "€uro ∈lement"));              TEST_EQUAL(std::distance(utf_begin(s), i), 12);
    TRY(i = str_find_last_not_of(utf_range(s), "abcde"));          TEST_EQUAL(std::distance(utf_begin(s), i), 11);
    TRY(i = str_find_last_not_of(utf_range(s), "∈lement"));        TEST_EQUAL(std::distance(utf_begin(s), i), 4);
    TRY(i = str_find_last_not_of(utf_range(s), "€uro ∈lement"));   TEST_EQUAL(std::distance(utf_begin(s), i), 12);

}

void test_unicorn_string_algorithm_line_column() {

    Ustring s0 = "",
        s1 = "Hello world",
        s2 = "Hello world\nGoodbye\n",
        s3 = "Hello world\r\nGoodbye\r\n";
    size_t ofs;
    std::pair<size_t, size_t> lc;

    TEST_EQUAL(s1.size(), 11);
    TEST_EQUAL(s2.size(), 20);
    TEST_EQUAL(s3.size(), 22);

    ofs = 0;   TEST_EQUAL(s0[ofs], '\0');  TRY(lc = str_line_column(s0, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 1);
    ofs = 0;   TEST_EQUAL(s1[ofs], 'H');   TRY(lc = str_line_column(s1, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 1);
    ofs = 10;  TEST_EQUAL(s1[ofs], 'd');   TRY(lc = str_line_column(s1, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 11);
    ofs = 11;  TEST_EQUAL(s1[ofs], '\0');  TRY(lc = str_line_column(s1, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 12);
    ofs = 0;   TEST_EQUAL(s2[ofs], 'H');   TRY(lc = str_line_column(s2, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 1);
    ofs = 10;  TEST_EQUAL(s2[ofs], 'd');   TRY(lc = str_line_column(s2, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 11);
    ofs = 11;  TEST_EQUAL(s2[ofs], '\n');  TRY(lc = str_line_column(s2, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 12);
    ofs = 12;  TEST_EQUAL(s2[ofs], 'G');   TRY(lc = str_line_column(s2, ofs));  TEST_EQUAL(lc.first, 2);  TEST_EQUAL(lc.second, 1);
    ofs = 18;  TEST_EQUAL(s2[ofs], 'e');   TRY(lc = str_line_column(s2, ofs));  TEST_EQUAL(lc.first, 2);  TEST_EQUAL(lc.second, 7);
    ofs = 19;  TEST_EQUAL(s2[ofs], '\n');  TRY(lc = str_line_column(s2, ofs));  TEST_EQUAL(lc.first, 2);  TEST_EQUAL(lc.second, 8);
    ofs = 20;  TEST_EQUAL(s2[ofs], '\0');  TRY(lc = str_line_column(s2, ofs));  TEST_EQUAL(lc.first, 3);  TEST_EQUAL(lc.second, 1);
    ofs = 0;   TEST_EQUAL(s3[ofs], 'H');   TRY(lc = str_line_column(s3, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 1);
    ofs = 10;  TEST_EQUAL(s3[ofs], 'd');   TRY(lc = str_line_column(s3, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 11);
    ofs = 11;  TEST_EQUAL(s3[ofs], '\r');  TRY(lc = str_line_column(s3, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 12);
    ofs = 12;  TEST_EQUAL(s3[ofs], '\n');  TRY(lc = str_line_column(s3, ofs));  TEST_EQUAL(lc.first, 1);  TEST_EQUAL(lc.second, 13);
    ofs = 13;  TEST_EQUAL(s3[ofs], 'G');   TRY(lc = str_line_column(s3, ofs));  TEST_EQUAL(lc.first, 2);  TEST_EQUAL(lc.second, 1);
    ofs = 19;  TEST_EQUAL(s3[ofs], 'e');   TRY(lc = str_line_column(s3, ofs));  TEST_EQUAL(lc.first, 2);  TEST_EQUAL(lc.second, 7);
    ofs = 20;  TEST_EQUAL(s3[ofs], '\r');  TRY(lc = str_line_column(s3, ofs));  TEST_EQUAL(lc.first, 2);  TEST_EQUAL(lc.second, 8);
    ofs = 21;  TEST_EQUAL(s3[ofs], '\n');  TRY(lc = str_line_column(s3, ofs));  TEST_EQUAL(lc.first, 2);  TEST_EQUAL(lc.second, 9);
    ofs = 22;  TEST_EQUAL(s3[ofs], '\0');  TRY(lc = str_line_column(s3, ofs));  TEST_EQUAL(lc.first, 3);  TEST_EQUAL(lc.second, 1);

}

void test_unicorn_string_algorithm_search() {

    Ustring s = "€uro ∈lement";
    Irange<Utf8Iterator> r;

    TRY(r = str_search(s, ""));                    TEST_EQUAL(std::distance(utf_begin(s), r.first), 0);   TEST_EQUAL(std::distance(r.first, r.second), 0);
    TRY(r = str_search(s, "€uro"));                TEST_EQUAL(std::distance(utf_begin(s), r.first), 0);   TEST_EQUAL(std::distance(r.first, r.second), 4);
    TRY(r = str_search(s, "∈lement"));             TEST_EQUAL(std::distance(utf_begin(s), r.first), 5);   TEST_EQUAL(std::distance(r.first, r.second), 7);
    TRY(r = str_search(s, "Hello"));               TEST_EQUAL(std::distance(utf_begin(s), r.first), 12);  TEST_EQUAL(std::distance(r.first, r.second), 0);
    TRY(r = str_search(utf_range(s), ""));         TEST_EQUAL(std::distance(utf_begin(s), r.first), 0);   TEST_EQUAL(std::distance(r.first, r.second), 0);
    TRY(r = str_search(utf_range(s), "€uro"));     TEST_EQUAL(std::distance(utf_begin(s), r.first), 0);   TEST_EQUAL(std::distance(r.first, r.second), 4);
    TRY(r = str_search(utf_range(s), "∈lement"));  TEST_EQUAL(std::distance(utf_begin(s), r.first), 5);   TEST_EQUAL(std::distance(r.first, r.second), 7);
    TRY(r = str_search(utf_range(s), "Hello"));    TEST_EQUAL(std::distance(utf_begin(s), r.first), 12);  TEST_EQUAL(std::distance(r.first, r.second), 0);

}

void test_unicorn_string_algorithm_skipws() {

    Ustring s;
    Utf8Iterator i;

    s = ""s;                   TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 0);  TEST_EQUAL(std::distance(utf_begin(s), i), 0);
    s = "Hello world"s;        TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 0);  TEST_EQUAL(std::distance(utf_begin(s), i), 0);
    s = " Hello "s;            TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 1);  TEST_EQUAL(std::distance(utf_begin(s), i), 1);
    s = " \r\n Hello \r\n "s;  TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 4);  TEST_EQUAL(std::distance(utf_begin(s), i), 4);
    s = "€uro ∈lement"s;       TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 0);  TEST_EQUAL(std::distance(utf_begin(s), i), 0);
    s = " €uro "s;             TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 1);  TEST_EQUAL(std::distance(utf_begin(s), i), 1);
    s = " \r\n €uro \r\n "s;   TRY(i = utf_begin(s));  TEST_EQUAL(str_skipws(i), 4);  TEST_EQUAL(std::distance(utf_begin(s), i), 4);

}
