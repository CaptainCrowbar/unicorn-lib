#include "unicorn/string.hpp"
#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <iterator>
#include <vector>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void check_fix_left() {

        U8string s;

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

        s = u8"";       TRY(str_fix_left_in(s, 0));        TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_fix_left_in(s, 1));        TEST_EQUAL(s, u8" ");
        s = u8"";       TRY(str_fix_left_in(s, 2));        TEST_EQUAL(s, u8"  ");
        s = u8"";       TRY(str_fix_left_in(s, 0, U'*'));  TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_fix_left_in(s, 1, U'*'));  TEST_EQUAL(s, u8"*");
        s = u8"";       TRY(str_fix_left_in(s, 2, U'*'));  TEST_EQUAL(s, u8"**");
        s = u8"";       TRY(str_fix_left_in(s, 0, U'§'));  TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_fix_left_in(s, 1, U'§'));  TEST_EQUAL(s, u8"§");
        s = u8"";       TRY(str_fix_left_in(s, 2, U'§'));  TEST_EQUAL(s, u8"§§");
        s = u8"Hello";  TRY(str_fix_left_in(s, 0));        TEST_EQUAL(s, u8"");
        s = u8"Hello";  TRY(str_fix_left_in(s, 4));        TEST_EQUAL(s, u8"Hell");
        s = u8"Hello";  TRY(str_fix_left_in(s, 5));        TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_fix_left_in(s, 6));        TEST_EQUAL(s, u8"Hello ");
        s = u8"Hello";  TRY(str_fix_left_in(s, 7));        TEST_EQUAL(s, u8"Hello  ");
        s = u8"Hello";  TRY(str_fix_left_in(s, 0, U'*'));  TEST_EQUAL(s, u8"");
        s = u8"Hello";  TRY(str_fix_left_in(s, 4, U'*'));  TEST_EQUAL(s, u8"Hell");
        s = u8"Hello";  TRY(str_fix_left_in(s, 5, U'*'));  TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_fix_left_in(s, 6, U'*'));  TEST_EQUAL(s, u8"Hello*");
        s = u8"Hello";  TRY(str_fix_left_in(s, 7, U'*'));  TEST_EQUAL(s, u8"Hello**");
        s = u8"Hello";  TRY(str_fix_left_in(s, 0, U'§'));  TEST_EQUAL(s, u8"");
        s = u8"Hello";  TRY(str_fix_left_in(s, 4, U'§'));  TEST_EQUAL(s, u8"Hell");
        s = u8"Hello";  TRY(str_fix_left_in(s, 5, U'§'));  TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_fix_left_in(s, 6, U'§'));  TEST_EQUAL(s, u8"Hello§");
        s = u8"Hello";  TRY(str_fix_left_in(s, 7, U'§'));  TEST_EQUAL(s, u8"Hello§§");

    }

    void check_fix_right() {

        U8string s;

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

        s = u8"";       TRY(str_fix_right_in(s, 0));        TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_fix_right_in(s, 1));        TEST_EQUAL(s, u8" ");
        s = u8"";       TRY(str_fix_right_in(s, 2));        TEST_EQUAL(s, u8"  ");
        s = u8"";       TRY(str_fix_right_in(s, 0, U'*'));  TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_fix_right_in(s, 1, U'*'));  TEST_EQUAL(s, u8"*");
        s = u8"";       TRY(str_fix_right_in(s, 2, U'*'));  TEST_EQUAL(s, u8"**");
        s = u8"";       TRY(str_fix_right_in(s, 0, U'§'));  TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_fix_right_in(s, 1, U'§'));  TEST_EQUAL(s, u8"§");
        s = u8"";       TRY(str_fix_right_in(s, 2, U'§'));  TEST_EQUAL(s, u8"§§");
        s = u8"Hello";  TRY(str_fix_right_in(s, 0));        TEST_EQUAL(s, u8"");
        s = u8"Hello";  TRY(str_fix_right_in(s, 4));        TEST_EQUAL(s, u8"ello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 5));        TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 6));        TEST_EQUAL(s, u8" Hello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 7));        TEST_EQUAL(s, u8"  Hello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 0, U'*'));  TEST_EQUAL(s, u8"");
        s = u8"Hello";  TRY(str_fix_right_in(s, 4, U'*'));  TEST_EQUAL(s, u8"ello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 5, U'*'));  TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 6, U'*'));  TEST_EQUAL(s, u8"*Hello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 7, U'*'));  TEST_EQUAL(s, u8"**Hello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 0, U'§'));  TEST_EQUAL(s, u8"");
        s = u8"Hello";  TRY(str_fix_right_in(s, 4, U'§'));  TEST_EQUAL(s, u8"ello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 5, U'§'));  TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 6, U'§'));  TEST_EQUAL(s, u8"§Hello");
        s = u8"Hello";  TRY(str_fix_right_in(s, 7, U'§'));  TEST_EQUAL(s, u8"§§Hello");

    }

    void check_insert() {

        U8string s, t;
        Utf8Iterator i, j;

        s = u8"Hello world";
        t = u8"€urope";
        TRY(i = j = utf_begin(s));
        TRY(std::advance(j, 5));
        TEST_EQUAL(str_insert(i, j, t), u8"€urope world");
        TEST_EQUAL(str_insert(irange(i, j), utf_range(t)), u8"€urope world");
        TRY(i = utf_begin(s));
        TRY(j = utf_end(s));
        TRY(std::advance(i, 6));
        TEST_EQUAL(str_insert(i, j, t), u8"Hello €urope");
        TEST_EQUAL(str_insert(irange(i, j), utf_range(t)), u8"Hello €urope");
        TEST_EQUAL(str_insert(i, i, t), u8"Hello €uropeworld");
        TEST_EQUAL(str_insert(irange(i, i), utf_range(t)), u8"Hello €uropeworld");
        TEST_EQUAL(str_insert(utf_range(s), t), u8"€urope");
        TEST_EQUAL(str_insert(utf_range(s), utf_range(t)), u8"€urope");
        s = u8"Hello world";
        TRY(i = j = utf_begin(s));
        TRY(std::advance(j, 5));
        TEST_EQUAL(u_str(str_insert_in(s, i, j, t)), u8"€urope");
        TEST_EQUAL(s, u8"€urope world");
        s = u8"Hello world";
        TRY(i = j = utf_begin(s));
        TRY(std::advance(j, 5));
        TEST_EQUAL(u_str(str_insert_in(s, irange(i, j), utf_range(t))), u8"€urope");
        TEST_EQUAL(s, u8"€urope world");
        s = u8"Hello world";
        TRY(i = utf_begin(s));
        TRY(j = utf_end(s));
        TRY(std::advance(i, 6));
        TEST_EQUAL(u_str(str_insert_in(s, i, j, t)), u8"€urope");
        TEST_EQUAL(s, u8"Hello €urope");
        s = u8"Hello world";
        TRY(i = utf_begin(s));
        TRY(j = utf_end(s));
        TRY(std::advance(i, 6));
        TEST_EQUAL(u_str(str_insert_in(s, irange(i, j), utf_range(t))), u8"€urope");
        TEST_EQUAL(s, u8"Hello €urope");
        s = u8"Hello world";
        TRY(i = utf_begin(s));
        TRY(j = utf_end(s));
        TRY(std::advance(i, 6));
        TEST_EQUAL(u_str(str_insert_in(s, i, t)), u8"€urope");
        TEST_EQUAL(s, u8"Hello €uropeworld");
        s = u8"Hello world";
        TRY(i = utf_begin(s));
        TRY(j = utf_end(s));
        TRY(std::advance(i, 6));
        TEST_EQUAL(u_str(str_insert_in(s, i, i, t)), u8"€urope");
        TEST_EQUAL(s, u8"Hello €uropeworld");
        s = u8"Hello world";
        TRY(i = utf_begin(s));
        TRY(j = utf_end(s));
        TRY(std::advance(i, 6));
        TEST_EQUAL(u_str(str_insert_in(s, irange(i, i), utf_range(t))), u8"€urope");
        TEST_EQUAL(s, u8"Hello €uropeworld");
        s = u8"Hello world";
        TEST_EQUAL(u_str(str_insert_in(s, utf_range(s), t)), u8"€urope");
        TEST_EQUAL(s, u8"€urope");
        s = u8"Hello world";
        TEST_EQUAL(u_str(str_insert_in(s, utf_range(s), utf_range(t))), u8"€urope");
        TEST_EQUAL(s, u8"€urope");

    }

    void check_join() {

        std::vector<U8string> v;

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

}

TEST_MODULE(unicorn, string_manipulation_f_m) {

    check_fix_left();
    check_fix_right();
    check_insert();
    check_join();

}
