#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <iterator>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_manip_fix_left() {

    Ustring s;

    TEST_EQUAL(str_fix_left(""s, 0), "");
    TEST_EQUAL(str_fix_left(""s, 1), " ");
    TEST_EQUAL(str_fix_left(""s, 2), "  ");
    TEST_EQUAL(str_fix_left(""s, 0, U'*'), "");
    TEST_EQUAL(str_fix_left(""s, 1, U'*'), "*");
    TEST_EQUAL(str_fix_left(""s, 2, U'*'), "**");
    TEST_EQUAL(str_fix_left(""s, 0, U'§'), "");
    TEST_EQUAL(str_fix_left(""s, 1, U'§'), "§");
    TEST_EQUAL(str_fix_left(""s, 2, U'§'), "§§");
    TEST_EQUAL(str_fix_left("Hello"s, 0), "");
    TEST_EQUAL(str_fix_left("Hello"s, 4), "Hell");
    TEST_EQUAL(str_fix_left("Hello"s, 5), "Hello");
    TEST_EQUAL(str_fix_left("Hello"s, 6), "Hello ");
    TEST_EQUAL(str_fix_left("Hello"s, 7), "Hello  ");
    TEST_EQUAL(str_fix_left("Hello"s, 0, U'*'), "");
    TEST_EQUAL(str_fix_left("Hello"s, 4, U'*'), "Hell");
    TEST_EQUAL(str_fix_left("Hello"s, 5, U'*'), "Hello");
    TEST_EQUAL(str_fix_left("Hello"s, 6, U'*'), "Hello*");
    TEST_EQUAL(str_fix_left("Hello"s, 7, U'*'), "Hello**");
    TEST_EQUAL(str_fix_left("Hello"s, 0, U'§'), "");
    TEST_EQUAL(str_fix_left("Hello"s, 4, U'§'), "Hell");
    TEST_EQUAL(str_fix_left("Hello"s, 5, U'§'), "Hello");
    TEST_EQUAL(str_fix_left("Hello"s, 6, U'§'), "Hello§");
    TEST_EQUAL(str_fix_left("Hello"s, 7, U'§'), "Hello§§");

    s = "";       TRY(str_fix_left_in(s, 0));        TEST_EQUAL(s, "");
    s = "";       TRY(str_fix_left_in(s, 1));        TEST_EQUAL(s, " ");
    s = "";       TRY(str_fix_left_in(s, 2));        TEST_EQUAL(s, "  ");
    s = "";       TRY(str_fix_left_in(s, 0, U'*'));  TEST_EQUAL(s, "");
    s = "";       TRY(str_fix_left_in(s, 1, U'*'));  TEST_EQUAL(s, "*");
    s = "";       TRY(str_fix_left_in(s, 2, U'*'));  TEST_EQUAL(s, "**");
    s = "";       TRY(str_fix_left_in(s, 0, U'§'));  TEST_EQUAL(s, "");
    s = "";       TRY(str_fix_left_in(s, 1, U'§'));  TEST_EQUAL(s, "§");
    s = "";       TRY(str_fix_left_in(s, 2, U'§'));  TEST_EQUAL(s, "§§");
    s = "Hello";  TRY(str_fix_left_in(s, 0));        TEST_EQUAL(s, "");
    s = "Hello";  TRY(str_fix_left_in(s, 4));        TEST_EQUAL(s, "Hell");
    s = "Hello";  TRY(str_fix_left_in(s, 5));        TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_fix_left_in(s, 6));        TEST_EQUAL(s, "Hello ");
    s = "Hello";  TRY(str_fix_left_in(s, 7));        TEST_EQUAL(s, "Hello  ");
    s = "Hello";  TRY(str_fix_left_in(s, 0, U'*'));  TEST_EQUAL(s, "");
    s = "Hello";  TRY(str_fix_left_in(s, 4, U'*'));  TEST_EQUAL(s, "Hell");
    s = "Hello";  TRY(str_fix_left_in(s, 5, U'*'));  TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_fix_left_in(s, 6, U'*'));  TEST_EQUAL(s, "Hello*");
    s = "Hello";  TRY(str_fix_left_in(s, 7, U'*'));  TEST_EQUAL(s, "Hello**");
    s = "Hello";  TRY(str_fix_left_in(s, 0, U'§'));  TEST_EQUAL(s, "");
    s = "Hello";  TRY(str_fix_left_in(s, 4, U'§'));  TEST_EQUAL(s, "Hell");
    s = "Hello";  TRY(str_fix_left_in(s, 5, U'§'));  TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_fix_left_in(s, 6, U'§'));  TEST_EQUAL(s, "Hello§");
    s = "Hello";  TRY(str_fix_left_in(s, 7, U'§'));  TEST_EQUAL(s, "Hello§§");

}

void test_unicorn_string_manip_fix_right() {

    Ustring s;

    TEST_EQUAL(str_fix_right(""s, 0), "");
    TEST_EQUAL(str_fix_right(""s, 1), " ");
    TEST_EQUAL(str_fix_right(""s, 2), "  ");
    TEST_EQUAL(str_fix_right(""s, 0, U'*'), "");
    TEST_EQUAL(str_fix_right(""s, 1, U'*'), "*");
    TEST_EQUAL(str_fix_right(""s, 2, U'*'), "**");
    TEST_EQUAL(str_fix_right(""s, 0, U'§'), "");
    TEST_EQUAL(str_fix_right(""s, 1, U'§'), "§");
    TEST_EQUAL(str_fix_right(""s, 2, U'§'), "§§");
    TEST_EQUAL(str_fix_right("Hello"s, 0), "");
    TEST_EQUAL(str_fix_right("Hello"s, 4), "ello");
    TEST_EQUAL(str_fix_right("Hello"s, 5), "Hello");
    TEST_EQUAL(str_fix_right("Hello"s, 6), " Hello");
    TEST_EQUAL(str_fix_right("Hello"s, 7), "  Hello");
    TEST_EQUAL(str_fix_right("Hello"s, 0, U'*'), "");
    TEST_EQUAL(str_fix_right("Hello"s, 4, U'*'), "ello");
    TEST_EQUAL(str_fix_right("Hello"s, 5, U'*'), "Hello");
    TEST_EQUAL(str_fix_right("Hello"s, 6, U'*'), "*Hello");
    TEST_EQUAL(str_fix_right("Hello"s, 7, U'*'), "**Hello");
    TEST_EQUAL(str_fix_right("Hello"s, 0, U'§'), "");
    TEST_EQUAL(str_fix_right("Hello"s, 4, U'§'), "ello");
    TEST_EQUAL(str_fix_right("Hello"s, 5, U'§'), "Hello");
    TEST_EQUAL(str_fix_right("Hello"s, 6, U'§'), "§Hello");
    TEST_EQUAL(str_fix_right("Hello"s, 7, U'§'), "§§Hello");

    s = "";       TRY(str_fix_right_in(s, 0));        TEST_EQUAL(s, "");
    s = "";       TRY(str_fix_right_in(s, 1));        TEST_EQUAL(s, " ");
    s = "";       TRY(str_fix_right_in(s, 2));        TEST_EQUAL(s, "  ");
    s = "";       TRY(str_fix_right_in(s, 0, U'*'));  TEST_EQUAL(s, "");
    s = "";       TRY(str_fix_right_in(s, 1, U'*'));  TEST_EQUAL(s, "*");
    s = "";       TRY(str_fix_right_in(s, 2, U'*'));  TEST_EQUAL(s, "**");
    s = "";       TRY(str_fix_right_in(s, 0, U'§'));  TEST_EQUAL(s, "");
    s = "";       TRY(str_fix_right_in(s, 1, U'§'));  TEST_EQUAL(s, "§");
    s = "";       TRY(str_fix_right_in(s, 2, U'§'));  TEST_EQUAL(s, "§§");
    s = "Hello";  TRY(str_fix_right_in(s, 0));        TEST_EQUAL(s, "");
    s = "Hello";  TRY(str_fix_right_in(s, 4));        TEST_EQUAL(s, "ello");
    s = "Hello";  TRY(str_fix_right_in(s, 5));        TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_fix_right_in(s, 6));        TEST_EQUAL(s, " Hello");
    s = "Hello";  TRY(str_fix_right_in(s, 7));        TEST_EQUAL(s, "  Hello");
    s = "Hello";  TRY(str_fix_right_in(s, 0, U'*'));  TEST_EQUAL(s, "");
    s = "Hello";  TRY(str_fix_right_in(s, 4, U'*'));  TEST_EQUAL(s, "ello");
    s = "Hello";  TRY(str_fix_right_in(s, 5, U'*'));  TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_fix_right_in(s, 6, U'*'));  TEST_EQUAL(s, "*Hello");
    s = "Hello";  TRY(str_fix_right_in(s, 7, U'*'));  TEST_EQUAL(s, "**Hello");
    s = "Hello";  TRY(str_fix_right_in(s, 0, U'§'));  TEST_EQUAL(s, "");
    s = "Hello";  TRY(str_fix_right_in(s, 4, U'§'));  TEST_EQUAL(s, "ello");
    s = "Hello";  TRY(str_fix_right_in(s, 5, U'§'));  TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_fix_right_in(s, 6, U'§'));  TEST_EQUAL(s, "§Hello");
    s = "Hello";  TRY(str_fix_right_in(s, 7, U'§'));  TEST_EQUAL(s, "§§Hello");

}

void test_unicorn_string_manip_insert() {

    Ustring s, t;
    Utf8Iterator i, j;

    s = "Hello world";
    t = "€urope";
    TRY(i = j = utf_begin(s));
    TRY(std::advance(j, 5));
    TEST_EQUAL(str_insert(i, j, t), "€urope world");
    TEST_EQUAL(str_insert(irange(i, j), utf_range(t)), "€urope world");
    TRY(i = utf_begin(s));
    TRY(j = utf_end(s));
    TRY(std::advance(i, 6));
    TEST_EQUAL(str_insert(i, j, t), "Hello €urope");
    TEST_EQUAL(str_insert(irange(i, j), utf_range(t)), "Hello €urope");
    TEST_EQUAL(str_insert(i, i, t), "Hello €uropeworld");
    TEST_EQUAL(str_insert(irange(i, i), utf_range(t)), "Hello €uropeworld");
    TEST_EQUAL(str_insert(utf_range(s), t), "€urope");
    TEST_EQUAL(str_insert(utf_range(s), utf_range(t)), "€urope");
    s = "Hello world";
    TRY(i = j = utf_begin(s));
    TRY(std::advance(j, 5));
    TEST_EQUAL(u_str(str_insert_in(s, i, j, t)), "€urope");
    TEST_EQUAL(s, "€urope world");
    s = "Hello world";
    TRY(i = j = utf_begin(s));
    TRY(std::advance(j, 5));
    TEST_EQUAL(u_str(str_insert_in(s, irange(i, j), utf_range(t))), "€urope");
    TEST_EQUAL(s, "€urope world");
    s = "Hello world";
    TRY(i = utf_begin(s));
    TRY(j = utf_end(s));
    TRY(std::advance(i, 6));
    TEST_EQUAL(u_str(str_insert_in(s, i, j, t)), "€urope");
    TEST_EQUAL(s, "Hello €urope");
    s = "Hello world";
    TRY(i = utf_begin(s));
    TRY(j = utf_end(s));
    TRY(std::advance(i, 6));
    TEST_EQUAL(u_str(str_insert_in(s, irange(i, j), utf_range(t))), "€urope");
    TEST_EQUAL(s, "Hello €urope");
    s = "Hello world";
    TRY(i = utf_begin(s));
    TRY(j = utf_end(s));
    TRY(std::advance(i, 6));
    TEST_EQUAL(u_str(str_insert_in(s, i, t)), "€urope");
    TEST_EQUAL(s, "Hello €uropeworld");
    s = "Hello world";
    TRY(i = utf_begin(s));
    TRY(j = utf_end(s));
    TRY(std::advance(i, 6));
    TEST_EQUAL(u_str(str_insert_in(s, i, i, t)), "€urope");
    TEST_EQUAL(s, "Hello €uropeworld");
    s = "Hello world";
    TRY(i = utf_begin(s));
    TRY(j = utf_end(s));
    TRY(std::advance(i, 6));
    TEST_EQUAL(u_str(str_insert_in(s, irange(i, i), utf_range(t))), "€urope");
    TEST_EQUAL(s, "Hello €uropeworld");
    s = "Hello world";
    TEST_EQUAL(u_str(str_insert_in(s, utf_range(s), t)), "€urope");
    TEST_EQUAL(s, "€urope");
    s = "Hello world";
    TEST_EQUAL(u_str(str_insert_in(s, utf_range(s), utf_range(t))), "€urope");
    TEST_EQUAL(s, "€urope");

}

void test_unicorn_string_manip_join() {

    Strings v;

    v.clear();                      TEST_EQUAL(str_join(v), "");
    v = {"Hello"};                  TEST_EQUAL(str_join(v), "Hello");
    v = {"Hello","world"};          TEST_EQUAL(str_join(v), "Helloworld");
    v = {"Hello","world","again"};  TEST_EQUAL(str_join(v), "Helloworldagain");
    v.clear();                      TEST_EQUAL(str_join(v, " "), "");
    v = {"Hello"};                  TEST_EQUAL(str_join(v, " "), "Hello");
    v = {"Hello","world"};          TEST_EQUAL(str_join(v, " "), "Hello world");
    v = {"Hello","world","again"};  TEST_EQUAL(str_join(v, " "), "Hello world again");
    v.clear();                      TEST_EQUAL(str_join(v, "<*>"s), "");
    v = {"Hello"};                  TEST_EQUAL(str_join(v, "<*>"s), "Hello");
    v = {"Hello","world"};          TEST_EQUAL(str_join(v, "<*>"s), "Hello<*>world");
    v = {"Hello","world","again"};  TEST_EQUAL(str_join(v, "<*>"s), "Hello<*>world<*>again");
    v.clear();                      TEST_EQUAL(str_join(v, "\n", true), "");
    v = {"Hello"};                  TEST_EQUAL(str_join(v, "\n", true), "Hello\n");
    v = {"Hello","world"};          TEST_EQUAL(str_join(v, "\n", true), "Hello\nworld\n");
    v = {"Hello","world","again"};  TEST_EQUAL(str_join(v, "\n", true), "Hello\nworld\nagain\n");

}
