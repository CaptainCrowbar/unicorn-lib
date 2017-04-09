#include "unicorn/string.hpp"
#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS::Unicorn;
using namespace std::literals;

namespace {

    void check_pad_left() {

        U8string s;

        TEST_EQUAL(str_pad_left(""s, 0), "");
        TEST_EQUAL(str_pad_left(""s, 1), " ");
        TEST_EQUAL(str_pad_left(""s, 2), "  ");
        TEST_EQUAL(str_pad_left(""s, 0, U'*'), "");
        TEST_EQUAL(str_pad_left(""s, 1, U'*'), "*");
        TEST_EQUAL(str_pad_left(""s, 2, U'*'), "**");
        TEST_EQUAL(str_pad_left(""s, 0, U'√'), "");
        TEST_EQUAL(str_pad_left(""s, 1, U'√'), "√");
        TEST_EQUAL(str_pad_left(""s, 2, U'√'), "√√");
        TEST_EQUAL(str_pad_left("Hello"s, 0), "Hello");
        TEST_EQUAL(str_pad_left("Hello"s, 5), "Hello");
        TEST_EQUAL(str_pad_left("Hello"s, 10), "     Hello");
        TEST_EQUAL(str_pad_left("Hello"s, 0, U'*'), "Hello");
        TEST_EQUAL(str_pad_left("Hello"s, 5, U'*'), "Hello");
        TEST_EQUAL(str_pad_left("Hello"s, 10, U'*'), "*****Hello");
        TEST_EQUAL(str_pad_left("Hello"s, 0, U'√'), "Hello");
        TEST_EQUAL(str_pad_left("Hello"s, 5, U'√'), "Hello");
        TEST_EQUAL(str_pad_left("Hello"s, 10, U'√'), "√√√√√Hello");
        TEST_EQUAL(str_pad_left("∀∃∀∃∀"s, 0), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_left("∀∃∀∃∀"s, 5), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_left("∀∃∀∃∀"s, 10), "     ∀∃∀∃∀");
        TEST_EQUAL(str_pad_left("∀∃∀∃∀"s, 0, U'*'), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_left("∀∃∀∃∀"s, 5, U'*'), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_left("∀∃∀∃∀"s, 10, U'*'), "*****∀∃∀∃∀");
        TEST_EQUAL(str_pad_left("∀∃∀∃∀"s, 0, U'√'), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_left("∀∃∀∃∀"s, 5, U'√'), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_left("∀∃∀∃∀"s, 10, U'√'), "√√√√√∀∃∀∃∀");

        s = u8"";       TRY(str_pad_left_in(s, 0));         TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_pad_left_in(s, 1));         TEST_EQUAL(s, u8" ");
        s = u8"";       TRY(str_pad_left_in(s, 2));         TEST_EQUAL(s, u8"  ");
        s = u8"";       TRY(str_pad_left_in(s, 0, U'*'));   TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_pad_left_in(s, 1, U'*'));   TEST_EQUAL(s, u8"*");
        s = u8"";       TRY(str_pad_left_in(s, 2, U'*'));   TEST_EQUAL(s, u8"**");
        s = u8"";       TRY(str_pad_left_in(s, 0, U'√'));   TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_pad_left_in(s, 1, U'√'));   TEST_EQUAL(s, u8"√");
        s = u8"";       TRY(str_pad_left_in(s, 2, U'√'));   TEST_EQUAL(s, u8"√√");
        s = u8"Hello";  TRY(str_pad_left_in(s, 0));         TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_left_in(s, 5));         TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_left_in(s, 10));        TEST_EQUAL(s, u8"     Hello");
        s = u8"Hello";  TRY(str_pad_left_in(s, 0, U'*'));   TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_left_in(s, 5, U'*'));   TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_left_in(s, 10, U'*'));  TEST_EQUAL(s, u8"*****Hello");
        s = u8"Hello";  TRY(str_pad_left_in(s, 0, U'√'));   TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_left_in(s, 5, U'√'));   TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_left_in(s, 10, U'√'));  TEST_EQUAL(s, u8"√√√√√Hello");
        s = u8"∀∃∀∃∀";  TRY(str_pad_left_in(s, 0));         TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_left_in(s, 5));         TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_left_in(s, 10));        TEST_EQUAL(s, u8"     ∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_left_in(s, 0, U'*'));   TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_left_in(s, 5, U'*'));   TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_left_in(s, 10, U'*'));  TEST_EQUAL(s, u8"*****∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_left_in(s, 0, U'√'));   TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_left_in(s, 5, U'√'));   TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_left_in(s, 10, U'√'));  TEST_EQUAL(s, u8"√√√√√∀∃∀∃∀");

    }

    void check_pad_right() {

        U8string s;

        TEST_EQUAL(str_pad_right(""s, 0), "");
        TEST_EQUAL(str_pad_right(""s, 1), " ");
        TEST_EQUAL(str_pad_right(""s, 2), "  ");
        TEST_EQUAL(str_pad_right(""s, 0, U'*'), "");
        TEST_EQUAL(str_pad_right(""s, 1, U'*'), "*");
        TEST_EQUAL(str_pad_right(""s, 2, U'*'), "**");
        TEST_EQUAL(str_pad_right(""s, 0, U'√'), "");
        TEST_EQUAL(str_pad_right(""s, 1, U'√'), "√");
        TEST_EQUAL(str_pad_right(""s, 2, U'√'), "√√");
        TEST_EQUAL(str_pad_right("Hello"s, 0), "Hello");
        TEST_EQUAL(str_pad_right("Hello"s, 5), "Hello");
        TEST_EQUAL(str_pad_right("Hello"s, 10), "Hello     ");
        TEST_EQUAL(str_pad_right("Hello"s, 0, U'*'), "Hello");
        TEST_EQUAL(str_pad_right("Hello"s, 5, U'*'), "Hello");
        TEST_EQUAL(str_pad_right("Hello"s, 10, U'*'), "Hello*****");
        TEST_EQUAL(str_pad_right("Hello"s, 0, U'√'), "Hello");
        TEST_EQUAL(str_pad_right("Hello"s, 5, U'√'), "Hello");
        TEST_EQUAL(str_pad_right("Hello"s, 10, U'√'), "Hello√√√√√");
        TEST_EQUAL(str_pad_right("∀∃∀∃∀"s, 0), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_right("∀∃∀∃∀"s, 5), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_right("∀∃∀∃∀"s, 10), "∀∃∀∃∀     ");
        TEST_EQUAL(str_pad_right("∀∃∀∃∀"s, 0, U'*'), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_right("∀∃∀∃∀"s, 5, U'*'), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_right("∀∃∀∃∀"s, 10, U'*'), "∀∃∀∃∀*****");
        TEST_EQUAL(str_pad_right("∀∃∀∃∀"s, 0, U'√'), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_right("∀∃∀∃∀"s, 5, U'√'), "∀∃∀∃∀");
        TEST_EQUAL(str_pad_right("∀∃∀∃∀"s, 10, U'√'), "∀∃∀∃∀√√√√√");

        s = u8"";       TRY(str_pad_right_in(s, 0));         TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_pad_right_in(s, 1));         TEST_EQUAL(s, u8" ");
        s = u8"";       TRY(str_pad_right_in(s, 2));         TEST_EQUAL(s, u8"  ");
        s = u8"";       TRY(str_pad_right_in(s, 0, U'*'));   TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_pad_right_in(s, 1, U'*'));   TEST_EQUAL(s, u8"*");
        s = u8"";       TRY(str_pad_right_in(s, 2, U'*'));   TEST_EQUAL(s, u8"**");
        s = u8"";       TRY(str_pad_right_in(s, 0, U'√'));   TEST_EQUAL(s, u8"");
        s = u8"";       TRY(str_pad_right_in(s, 1, U'√'));   TEST_EQUAL(s, u8"√");
        s = u8"";       TRY(str_pad_right_in(s, 2, U'√'));   TEST_EQUAL(s, u8"√√");
        s = u8"Hello";  TRY(str_pad_right_in(s, 0));         TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_right_in(s, 5));         TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_right_in(s, 10));        TEST_EQUAL(s, u8"Hello     ");
        s = u8"Hello";  TRY(str_pad_right_in(s, 0, U'*'));   TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_right_in(s, 5, U'*'));   TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_right_in(s, 10, U'*'));  TEST_EQUAL(s, u8"Hello*****");
        s = u8"Hello";  TRY(str_pad_right_in(s, 0, U'√'));   TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_right_in(s, 5, U'√'));   TEST_EQUAL(s, u8"Hello");
        s = u8"Hello";  TRY(str_pad_right_in(s, 10, U'√'));  TEST_EQUAL(s, u8"Hello√√√√√");
        s = u8"∀∃∀∃∀";  TRY(str_pad_right_in(s, 0));         TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_right_in(s, 5));         TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_right_in(s, 10));        TEST_EQUAL(s, u8"∀∃∀∃∀     ");
        s = u8"∀∃∀∃∀";  TRY(str_pad_right_in(s, 0, U'*'));   TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_right_in(s, 5, U'*'));   TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_right_in(s, 10, U'*'));  TEST_EQUAL(s, u8"∀∃∀∃∀*****");
        s = u8"∀∃∀∃∀";  TRY(str_pad_right_in(s, 0, U'√'));   TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_right_in(s, 5, U'√'));   TEST_EQUAL(s, u8"∀∃∀∃∀");
        s = u8"∀∃∀∃∀";  TRY(str_pad_right_in(s, 10, U'√'));  TEST_EQUAL(s, u8"∀∃∀∃∀√√√√√");

    }

    void check_partition() {

        U8string s, t;

        TEST(! str_partition(""s, s, t));                     TEST_EQUAL(s, "");       TEST_EQUAL(t, "");
        TEST(! str_partition("Hello"s, s, t));                TEST_EQUAL(s, "Hello");  TEST_EQUAL(t, "");
        TEST(str_partition("Hello world"s, s, t));            TEST_EQUAL(s, "Hello");  TEST_EQUAL(t, "world");
        TEST(str_partition("Hello brave new world"s, s, t));  TEST_EQUAL(s, "Hello");  TEST_EQUAL(t, "brave new world");
        TEST(str_partition("€uro ∈lement"s, s, t));           TEST_EQUAL(s, "€uro");   TEST_EQUAL(t, "∈lement");

        TEST(! str_partition_at(""s, s, t, " "));                             TEST_EQUAL(s, "");              TEST_EQUAL(t, "");
        TEST(! str_partition_at("Hello"s, s, t, " "));                        TEST_EQUAL(s, "Hello");         TEST_EQUAL(t, "");
        TEST(str_partition_at("Hello world"s, s, t, " "));                    TEST_EQUAL(s, "Hello");         TEST_EQUAL(t, "world");
        TEST(str_partition_at("Hello brave new world"s, s, t, " "));          TEST_EQUAL(s, "Hello");         TEST_EQUAL(t, "brave new world");
        TEST(! str_partition_at(""s, s, t, "<>"));                            TEST_EQUAL(s, "");              TEST_EQUAL(t, "");
        TEST(! str_partition_at("Hello"s, s, t, "<>"));                       TEST_EQUAL(s, "Hello");         TEST_EQUAL(t, "");
        TEST(! str_partition_at("Hello world"s, s, t, "<>"));                 TEST_EQUAL(s, "Hello world");   TEST_EQUAL(t, "");
        TEST(! str_partition_at("Hello"s, s, t, "<>"));                       TEST_EQUAL(s, "Hello");         TEST_EQUAL(t, "");
        TEST(str_partition_at("Hello<>world"s, s, t, "<>"));                  TEST_EQUAL(s, "Hello");         TEST_EQUAL(t, "world");
        TEST(str_partition_at("Hello<>brave<>new<>world"s, s, t, "<>"));      TEST_EQUAL(s, "Hello");         TEST_EQUAL(t, "brave<>new<>world");
        TEST(str_partition_at("<>Hello<>world<>"s, s, t, "<>"));              TEST_EQUAL(s, "");              TEST_EQUAL(t, "Hello<>world<>");
        TEST(str_partition_at("<>Hello<>brave<>new<>world<>"s, s, t, "<>"));  TEST_EQUAL(s, "");              TEST_EQUAL(t, "Hello<>brave<>new<>world<>");
        TEST(! str_partition_at("€uro ∈lement"s, s, t, "∇√"));                TEST_EQUAL(s, "€uro ∈lement");  TEST_EQUAL(t, "");
        TEST(str_partition_at("€uro∇√∈lement"s, s, t, "∇√"));                 TEST_EQUAL(s, "€uro");          TEST_EQUAL(t, "∈lement");
        TEST(str_partition_at("∇√€uro∇√∈lement∇√"s, s, t, "∇√"));             TEST_EQUAL(s, "");              TEST_EQUAL(t, "€uro∇√∈lement∇√");

        TEST(! str_partition_by(""s, s, t, "+-_"));                     TEST_EQUAL(s, "");       TEST_EQUAL(t, "");
        TEST(! str_partition_by("Hello"s, s, t, "+-_"));                TEST_EQUAL(s, "Hello");  TEST_EQUAL(t, "");
        TEST(str_partition_by("Hello_world"s, s, t, "+-_"));            TEST_EQUAL(s, "Hello");  TEST_EQUAL(t, "world");
        TEST(str_partition_by("Hello_brave_new_world"s, s, t, "+-_"));  TEST_EQUAL(s, "Hello");  TEST_EQUAL(t, "brave_new_world");
        TEST(str_partition_by("€uro∇√∈lement∇√"s, s, t, "√∫∂∇"));       TEST_EQUAL(s, "€uro");   TEST_EQUAL(t, "∈lement∇√");

    }

    void check_remove() {

        U8string s, t;

        s = u8"";             TRY(t = str_remove(s, U'o'));                                         TEST_EQUAL(t, u8"");
        s = u8"";             TRY(t = str_remove(s, u8"aeiou"));                                    TEST_EQUAL(t, u8"");
        s = u8"Hello world";  TRY(t = str_remove(s, U'o'));                                         TEST_EQUAL(t, u8"Hell wrld");
        s = u8"Hello world";  TRY(t = str_remove(s, u8"aeiou"));                                    TEST_EQUAL(t, u8"Hll wrld");
        s = u8"";             TRY(t = str_remove_if(s, [] (char32_t c) { return c < U'a'; }));      TEST_EQUAL(t, u8"");
        s = u8"Hello world";  TRY(t = str_remove_if(s, [] (char32_t c) { return c < U'a'; }));      TEST_EQUAL(t, u8"elloworld");
        s = u8"";             TRY(t = str_remove_if_not(s, [] (char32_t c) { return c < U'a'; }));  TEST_EQUAL(t, u8"");
        s = u8"Hello world";  TRY(t = str_remove_if_not(s, [] (char32_t c) { return c < U'a'; }));  TEST_EQUAL(t, u8"H ");

        s = u8"";             TRY(str_remove_in(s, U'o'));                                         TEST_EQUAL(s, u8"");
        s = u8"";             TRY(str_remove_in(s, u8"aeiou"));                                    TEST_EQUAL(s, u8"");
        s = u8"Hello world";  TRY(str_remove_in(s, U'o'));                                         TEST_EQUAL(s, u8"Hell wrld");
        s = u8"Hello world";  TRY(str_remove_in(s, u8"aeiou"));                                    TEST_EQUAL(s, u8"Hll wrld");
        s = u8"";             TRY(str_remove_in_if(s, [] (char32_t c) { return c < U'a'; }));      TEST_EQUAL(s, u8"");
        s = u8"Hello world";  TRY(str_remove_in_if(s, [] (char32_t c) { return c < U'a'; }));      TEST_EQUAL(s, u8"elloworld");
        s = u8"";             TRY(str_remove_in_if_not(s, [] (char32_t c) { return c < U'a'; }));  TEST_EQUAL(s, u8"");
        s = u8"Hello world";  TRY(str_remove_in_if_not(s, [] (char32_t c) { return c < U'a'; }));  TEST_EQUAL(s, u8"H ");

    }

    void check_repeat() {

        U8string s;

        TEST_EQUAL(str_repeat("", 0), "");
        TEST_EQUAL(str_repeat("", 1), "");
        TEST_EQUAL(str_repeat("", 2), "");
        TEST_EQUAL(str_repeat("@", 0), "");
        TEST_EQUAL(str_repeat("@", 1), "@");
        TEST_EQUAL(str_repeat("@", 2), "@@");
        TEST_EQUAL(str_repeat("@", 3), "@@@");
        TEST_EQUAL(str_repeat("Hello", 0), "");
        TEST_EQUAL(str_repeat("Hello", 1), "Hello");
        TEST_EQUAL(str_repeat("Hello", 2), "HelloHello");
        TEST_EQUAL(str_repeat("Hello", 3), "HelloHelloHello");
        TEST_EQUAL(str_repeat("Hello", 4), "HelloHelloHelloHello");
        TEST_EQUAL(str_repeat("Hello", 5), "HelloHelloHelloHelloHello");

        s = "";       TRY(str_repeat_in(s, 0));  TEST_EQUAL(s, "");
        s = "";       TRY(str_repeat_in(s, 1));  TEST_EQUAL(s, "");
        s = "";       TRY(str_repeat_in(s, 2));  TEST_EQUAL(s, "");
        s = "@";      TRY(str_repeat_in(s, 0));  TEST_EQUAL(s, "");
        s = "@";      TRY(str_repeat_in(s, 1));  TEST_EQUAL(s, "@");
        s = "@";      TRY(str_repeat_in(s, 2));  TEST_EQUAL(s, "@@");
        s = "@";      TRY(str_repeat_in(s, 3));  TEST_EQUAL(s, "@@@");
        s = "Hello";  TRY(str_repeat_in(s, 0));  TEST_EQUAL(s, "");
        s = "Hello";  TRY(str_repeat_in(s, 1));  TEST_EQUAL(s, "Hello");
        s = "Hello";  TRY(str_repeat_in(s, 2));  TEST_EQUAL(s, "HelloHello");
        s = "Hello";  TRY(str_repeat_in(s, 3));  TEST_EQUAL(s, "HelloHelloHello");
        s = "Hello";  TRY(str_repeat_in(s, 4));  TEST_EQUAL(s, "HelloHelloHelloHello");
        s = "Hello";  TRY(str_repeat_in(s, 5));  TEST_EQUAL(s, "HelloHelloHelloHelloHello");

    }

    void check_replace() {

        U8string s;

        TEST_EQUAL(str_replace(""s, "", ""), "");
        TEST_EQUAL(str_replace("(abc)"s, "", ""), "(abc)");
        TEST_EQUAL(str_replace("(abc)"s, "abc", ""), "()");
        TEST_EQUAL(str_replace("(abc)"s, "abc", "xyz"), "(xyz)");
        TEST_EQUAL(str_replace("(abc)"s, "xyz", "abc"), "(abc)");
        TEST_EQUAL(str_replace("(abc)(abc)"s, "abc", ""), "()()");
        TEST_EQUAL(str_replace("(abc)(abc)"s, "abc", "xyz"), "(xyz)(xyz)");
        TEST_EQUAL(str_replace("(abc)(abc)"s, "xyz", "abc"), "(abc)(abc)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", ""), "()()()");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "xyz"), "(xyz)(xyz)(xyz)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "xyz", "abc"), "(abc)(abc)(abc)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "xyz", 0), "(abc)(abc)(abc)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "xyz", 1), "(xyz)(abc)(abc)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "xyz", 2), "(xyz)(xyz)(abc)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "xyz", 3), "(xyz)(xyz)(xyz)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "xyz", 4), "(xyz)(xyz)(xyz)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "∀∃∇", 0), "(abc)(abc)(abc)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "∀∃∇", 1), "(∀∃∇)(abc)(abc)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "∀∃∇", 2), "(∀∃∇)(∀∃∇)(abc)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "∀∃∇", 3), "(∀∃∇)(∀∃∇)(∀∃∇)");
        TEST_EQUAL(str_replace("(abc)(abc)(abc)"s, "abc", "∀∃∇", 4), "(∀∃∇)(∀∃∇)(∀∃∇)");
        TEST_EQUAL(str_replace("(∀∃∇)(∀∃∇)(∀∃∇)"s, "∀∃∇", "xyz", 0), "(∀∃∇)(∀∃∇)(∀∃∇)");
        TEST_EQUAL(str_replace("(∀∃∇)(∀∃∇)(∀∃∇)"s, "∀∃∇", "xyz", 1), "(xyz)(∀∃∇)(∀∃∇)");
        TEST_EQUAL(str_replace("(∀∃∇)(∀∃∇)(∀∃∇)"s, "∀∃∇", "xyz", 2), "(xyz)(xyz)(∀∃∇)");
        TEST_EQUAL(str_replace("(∀∃∇)(∀∃∇)(∀∃∇)"s, "∀∃∇", "xyz", 3), "(xyz)(xyz)(xyz)");
        TEST_EQUAL(str_replace("(∀∃∇)(∀∃∇)(∀∃∇)"s, "∀∃∇", "xyz", 4), "(xyz)(xyz)(xyz)");

        s = u8"";                 TRY(str_replace_in(s, u8"", u8""));           TEST_EQUAL(s, u8"");
        s = u8"(abc)";            TRY(str_replace_in(s, u8"", u8""));           TEST_EQUAL(s, u8"(abc)");
        s = u8"(abc)";            TRY(str_replace_in(s, u8"abc", u8""));        TEST_EQUAL(s, u8"()");
        s = u8"(abc)";            TRY(str_replace_in(s, u8"abc", u8"xyz"));     TEST_EQUAL(s, u8"(xyz)");
        s = u8"(abc)";            TRY(str_replace_in(s, u8"xyz", u8"abc"));     TEST_EQUAL(s, u8"(abc)");
        s = u8"(abc)(abc)";       TRY(str_replace_in(s, u8"abc", u8""));        TEST_EQUAL(s, u8"()()");
        s = u8"(abc)(abc)";       TRY(str_replace_in(s, u8"abc", u8"xyz"));     TEST_EQUAL(s, u8"(xyz)(xyz)");
        s = u8"(abc)(abc)";       TRY(str_replace_in(s, u8"xyz", u8"abc"));     TEST_EQUAL(s, u8"(abc)(abc)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8""));        TEST_EQUAL(s, u8"()()()");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"xyz"));     TEST_EQUAL(s, u8"(xyz)(xyz)(xyz)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"xyz", u8"abc"));     TEST_EQUAL(s, u8"(abc)(abc)(abc)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"xyz", 0));  TEST_EQUAL(s, u8"(abc)(abc)(abc)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"xyz", 1));  TEST_EQUAL(s, u8"(xyz)(abc)(abc)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"xyz", 2));  TEST_EQUAL(s, u8"(xyz)(xyz)(abc)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"xyz", 3));  TEST_EQUAL(s, u8"(xyz)(xyz)(xyz)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"xyz", 4));  TEST_EQUAL(s, u8"(xyz)(xyz)(xyz)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"∀∃∇", 0));  TEST_EQUAL(s, u8"(abc)(abc)(abc)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"∀∃∇", 1));  TEST_EQUAL(s, u8"(∀∃∇)(abc)(abc)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"∀∃∇", 2));  TEST_EQUAL(s, u8"(∀∃∇)(∀∃∇)(abc)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"∀∃∇", 3));  TEST_EQUAL(s, u8"(∀∃∇)(∀∃∇)(∀∃∇)");
        s = u8"(abc)(abc)(abc)";  TRY(str_replace_in(s, u8"abc", u8"∀∃∇", 4));  TEST_EQUAL(s, u8"(∀∃∇)(∀∃∇)(∀∃∇)");
        s = u8"(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, u8"∀∃∇", u8"xyz", 0));  TEST_EQUAL(s, u8"(∀∃∇)(∀∃∇)(∀∃∇)");
        s = u8"(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, u8"∀∃∇", u8"xyz", 1));  TEST_EQUAL(s, u8"(xyz)(∀∃∇)(∀∃∇)");
        s = u8"(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, u8"∀∃∇", u8"xyz", 2));  TEST_EQUAL(s, u8"(xyz)(xyz)(∀∃∇)");
        s = u8"(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, u8"∀∃∇", u8"xyz", 3));  TEST_EQUAL(s, u8"(xyz)(xyz)(xyz)");
        s = u8"(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, u8"∀∃∇", u8"xyz", 4));  TEST_EQUAL(s, u8"(xyz)(xyz)(xyz)");

    }

}

TEST_MODULE(unicorn, string_manipulation_n_r) {

    check_pad_left();
    check_pad_right();
    check_partition();
    check_remove();
    check_replace();
    check_repeat();

}
