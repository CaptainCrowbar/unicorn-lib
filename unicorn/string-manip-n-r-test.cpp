#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_manip_pad_left() {

    Ustring s;

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

    s = "";       TRY(str_pad_left_in(s, 0));         TEST_EQUAL(s, "");
    s = "";       TRY(str_pad_left_in(s, 1));         TEST_EQUAL(s, " ");
    s = "";       TRY(str_pad_left_in(s, 2));         TEST_EQUAL(s, "  ");
    s = "";       TRY(str_pad_left_in(s, 0, U'*'));   TEST_EQUAL(s, "");
    s = "";       TRY(str_pad_left_in(s, 1, U'*'));   TEST_EQUAL(s, "*");
    s = "";       TRY(str_pad_left_in(s, 2, U'*'));   TEST_EQUAL(s, "**");
    s = "";       TRY(str_pad_left_in(s, 0, U'√'));   TEST_EQUAL(s, "");
    s = "";       TRY(str_pad_left_in(s, 1, U'√'));   TEST_EQUAL(s, "√");
    s = "";       TRY(str_pad_left_in(s, 2, U'√'));   TEST_EQUAL(s, "√√");
    s = "Hello";  TRY(str_pad_left_in(s, 0));         TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_left_in(s, 5));         TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_left_in(s, 10));        TEST_EQUAL(s, "     Hello");
    s = "Hello";  TRY(str_pad_left_in(s, 0, U'*'));   TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_left_in(s, 5, U'*'));   TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_left_in(s, 10, U'*'));  TEST_EQUAL(s, "*****Hello");
    s = "Hello";  TRY(str_pad_left_in(s, 0, U'√'));   TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_left_in(s, 5, U'√'));   TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_left_in(s, 10, U'√'));  TEST_EQUAL(s, "√√√√√Hello");
    s = "∀∃∀∃∀";  TRY(str_pad_left_in(s, 0));         TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_left_in(s, 5));         TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_left_in(s, 10));        TEST_EQUAL(s, "     ∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_left_in(s, 0, U'*'));   TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_left_in(s, 5, U'*'));   TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_left_in(s, 10, U'*'));  TEST_EQUAL(s, "*****∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_left_in(s, 0, U'√'));   TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_left_in(s, 5, U'√'));   TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_left_in(s, 10, U'√'));  TEST_EQUAL(s, "√√√√√∀∃∀∃∀");

}

void test_unicorn_string_manip_pad_right() {

    Ustring s;

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

    s = "";       TRY(str_pad_right_in(s, 0));         TEST_EQUAL(s, "");
    s = "";       TRY(str_pad_right_in(s, 1));         TEST_EQUAL(s, " ");
    s = "";       TRY(str_pad_right_in(s, 2));         TEST_EQUAL(s, "  ");
    s = "";       TRY(str_pad_right_in(s, 0, U'*'));   TEST_EQUAL(s, "");
    s = "";       TRY(str_pad_right_in(s, 1, U'*'));   TEST_EQUAL(s, "*");
    s = "";       TRY(str_pad_right_in(s, 2, U'*'));   TEST_EQUAL(s, "**");
    s = "";       TRY(str_pad_right_in(s, 0, U'√'));   TEST_EQUAL(s, "");
    s = "";       TRY(str_pad_right_in(s, 1, U'√'));   TEST_EQUAL(s, "√");
    s = "";       TRY(str_pad_right_in(s, 2, U'√'));   TEST_EQUAL(s, "√√");
    s = "Hello";  TRY(str_pad_right_in(s, 0));         TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_right_in(s, 5));         TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_right_in(s, 10));        TEST_EQUAL(s, "Hello     ");
    s = "Hello";  TRY(str_pad_right_in(s, 0, U'*'));   TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_right_in(s, 5, U'*'));   TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_right_in(s, 10, U'*'));  TEST_EQUAL(s, "Hello*****");
    s = "Hello";  TRY(str_pad_right_in(s, 0, U'√'));   TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_right_in(s, 5, U'√'));   TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(str_pad_right_in(s, 10, U'√'));  TEST_EQUAL(s, "Hello√√√√√");
    s = "∀∃∀∃∀";  TRY(str_pad_right_in(s, 0));         TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_right_in(s, 5));         TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_right_in(s, 10));        TEST_EQUAL(s, "∀∃∀∃∀     ");
    s = "∀∃∀∃∀";  TRY(str_pad_right_in(s, 0, U'*'));   TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_right_in(s, 5, U'*'));   TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_right_in(s, 10, U'*'));  TEST_EQUAL(s, "∀∃∀∃∀*****");
    s = "∀∃∀∃∀";  TRY(str_pad_right_in(s, 0, U'√'));   TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_right_in(s, 5, U'√'));   TEST_EQUAL(s, "∀∃∀∃∀");
    s = "∀∃∀∃∀";  TRY(str_pad_right_in(s, 10, U'√'));  TEST_EQUAL(s, "∀∃∀∃∀√√√√√");

}

void test_unicorn_string_manip_partition() {

    Ustring s, t;

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

void test_unicorn_string_manip_remove() {

    Ustring s, t;

    s = "";             TRY(t = str_remove(s, U'o'));                                         TEST_EQUAL(t, "");
    s = "";             TRY(t = str_remove(s, "aeiou"));                                      TEST_EQUAL(t, "");
    s = "Hello world";  TRY(t = str_remove(s, U'o'));                                         TEST_EQUAL(t, "Hell wrld");
    s = "Hello world";  TRY(t = str_remove(s, "aeiou"));                                      TEST_EQUAL(t, "Hll wrld");
    s = "";             TRY(t = str_remove_if(s, [] (char32_t c) { return c < U'a'; }));      TEST_EQUAL(t, "");
    s = "Hello world";  TRY(t = str_remove_if(s, [] (char32_t c) { return c < U'a'; }));      TEST_EQUAL(t, "elloworld");
    s = "";             TRY(t = str_remove_if_not(s, [] (char32_t c) { return c < U'a'; }));  TEST_EQUAL(t, "");
    s = "Hello world";  TRY(t = str_remove_if_not(s, [] (char32_t c) { return c < U'a'; }));  TEST_EQUAL(t, "H ");

    s = "";             TRY(str_remove_in(s, U'o'));                                         TEST_EQUAL(s, "");
    s = "";             TRY(str_remove_in(s, "aeiou"));                                      TEST_EQUAL(s, "");
    s = "Hello world";  TRY(str_remove_in(s, U'o'));                                         TEST_EQUAL(s, "Hell wrld");
    s = "Hello world";  TRY(str_remove_in(s, "aeiou"));                                      TEST_EQUAL(s, "Hll wrld");
    s = "";             TRY(str_remove_in_if(s, [] (char32_t c) { return c < U'a'; }));      TEST_EQUAL(s, "");
    s = "Hello world";  TRY(str_remove_in_if(s, [] (char32_t c) { return c < U'a'; }));      TEST_EQUAL(s, "elloworld");
    s = "";             TRY(str_remove_in_if_not(s, [] (char32_t c) { return c < U'a'; }));  TEST_EQUAL(s, "");
    s = "Hello world";  TRY(str_remove_in_if_not(s, [] (char32_t c) { return c < U'a'; }));  TEST_EQUAL(s, "H ");

}

void test_unicorn_string_manip_repeat() {

    Ustring s;

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

void test_unicorn_string_manip_replace() {

    Ustring s;

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

    s = "";                 TRY(str_replace_in(s, "", ""));           TEST_EQUAL(s, "");
    s = "(abc)";            TRY(str_replace_in(s, "", ""));           TEST_EQUAL(s, "(abc)");
    s = "(abc)";            TRY(str_replace_in(s, "abc", ""));        TEST_EQUAL(s, "()");
    s = "(abc)";            TRY(str_replace_in(s, "abc", "xyz"));     TEST_EQUAL(s, "(xyz)");
    s = "(abc)";            TRY(str_replace_in(s, "xyz", "abc"));     TEST_EQUAL(s, "(abc)");
    s = "(abc)(abc)";       TRY(str_replace_in(s, "abc", ""));        TEST_EQUAL(s, "()()");
    s = "(abc)(abc)";       TRY(str_replace_in(s, "abc", "xyz"));     TEST_EQUAL(s, "(xyz)(xyz)");
    s = "(abc)(abc)";       TRY(str_replace_in(s, "xyz", "abc"));     TEST_EQUAL(s, "(abc)(abc)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", ""));        TEST_EQUAL(s, "()()()");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "xyz"));     TEST_EQUAL(s, "(xyz)(xyz)(xyz)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "xyz", "abc"));     TEST_EQUAL(s, "(abc)(abc)(abc)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "xyz", 0));  TEST_EQUAL(s, "(abc)(abc)(abc)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "xyz", 1));  TEST_EQUAL(s, "(xyz)(abc)(abc)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "xyz", 2));  TEST_EQUAL(s, "(xyz)(xyz)(abc)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "xyz", 3));  TEST_EQUAL(s, "(xyz)(xyz)(xyz)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "xyz", 4));  TEST_EQUAL(s, "(xyz)(xyz)(xyz)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "∀∃∇", 0));  TEST_EQUAL(s, "(abc)(abc)(abc)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "∀∃∇", 1));  TEST_EQUAL(s, "(∀∃∇)(abc)(abc)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "∀∃∇", 2));  TEST_EQUAL(s, "(∀∃∇)(∀∃∇)(abc)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "∀∃∇", 3));  TEST_EQUAL(s, "(∀∃∇)(∀∃∇)(∀∃∇)");
    s = "(abc)(abc)(abc)";  TRY(str_replace_in(s, "abc", "∀∃∇", 4));  TEST_EQUAL(s, "(∀∃∇)(∀∃∇)(∀∃∇)");
    s = "(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, "∀∃∇", "xyz", 0));  TEST_EQUAL(s, "(∀∃∇)(∀∃∇)(∀∃∇)");
    s = "(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, "∀∃∇", "xyz", 1));  TEST_EQUAL(s, "(xyz)(∀∃∇)(∀∃∇)");
    s = "(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, "∀∃∇", "xyz", 2));  TEST_EQUAL(s, "(xyz)(xyz)(∀∃∇)");
    s = "(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, "∀∃∇", "xyz", 3));  TEST_EQUAL(s, "(xyz)(xyz)(xyz)");
    s = "(∀∃∇)(∀∃∇)(∀∃∇)";  TRY(str_replace_in(s, "∀∃∇", "xyz", 4));  TEST_EQUAL(s, "(xyz)(xyz)(xyz)");

}
