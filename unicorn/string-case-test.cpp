#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_case_conversions() {

    Ustring s;

    TEST_EQUAL(str_case(""s, Case::none), "");
    TEST_EQUAL(str_case("HELLO WORLD"s, Case::none), "HELLO WORLD");
    TEST_EQUAL(str_case("hello world"s, Case::none), "hello world");

    TEST_EQUAL(str_uppercase(""s), "");
    TEST_EQUAL(str_uppercase("HELLO WORLD"s), "HELLO WORLD");
    TEST_EQUAL(str_uppercase("hello world"s), "HELLO WORLD");
    TEST_EQUAL(str_case(""s, Case::upper), "");
    TEST_EQUAL(str_case("HELLO WORLD"s, Case::upper), "HELLO WORLD");
    TEST_EQUAL(str_case("hello world"s, Case::upper), "HELLO WORLD");

    s = "";             TRY(str_uppercase_in(s));          TEST_EQUAL(s, "");
    s = "HELLO WORLD";  TRY(str_uppercase_in(s));          TEST_EQUAL(s, "HELLO WORLD");
    s = "hello world";  TRY(str_uppercase_in(s));          TEST_EQUAL(s, "HELLO WORLD");
    s = "";             TRY(str_case_in(s, Case::upper));  TEST_EQUAL(s, "");
    s = "HELLO WORLD";  TRY(str_case_in(s, Case::upper));  TEST_EQUAL(s, "HELLO WORLD");
    s = "hello world";  TRY(str_case_in(s, Case::upper));  TEST_EQUAL(s, "HELLO WORLD");

    TEST_EQUAL(str_lowercase(""s), "");
    TEST_EQUAL(str_lowercase("HELLO WORLD"s), "hello world");
    TEST_EQUAL(str_lowercase("hello world"s), "hello world");
    TEST_EQUAL(str_case(""s, Case::lower), "");
    TEST_EQUAL(str_case("HELLO WORLD"s, Case::lower), "hello world");
    TEST_EQUAL(str_case("hello world"s, Case::lower), "hello world");

    s = "";             TRY(str_lowercase_in(s));          TEST_EQUAL(s, "");
    s = "HELLO WORLD";  TRY(str_lowercase_in(s));          TEST_EQUAL(s, "hello world");
    s = "hello world";  TRY(str_lowercase_in(s));          TEST_EQUAL(s, "hello world");
    s = "";             TRY(str_case_in(s, Case::lower));  TEST_EQUAL(s, "");
    s = "HELLO WORLD";  TRY(str_case_in(s, Case::lower));  TEST_EQUAL(s, "hello world");
    s = "hello world";  TRY(str_case_in(s, Case::lower));  TEST_EQUAL(s, "hello world");

    TEST_EQUAL(str_titlecase(""s), "");
    TEST_EQUAL(str_titlecase("HELLO WORLD"s), "Hello World");
    TEST_EQUAL(str_titlecase("hello world"s), "Hello World");
    TEST_EQUAL(str_titlecase("(hello-world)"s), "(Hello-World)");
    TEST_EQUAL(str_titlecase("@hello@world@"s), "@Hello@World@");
    TEST_EQUAL(str_titlecase("ΜΜΜ ΜΜΜ"s), "Μμμ Μμμ");
    TEST_EQUAL(str_titlecase("µµµ µµµ"s), "Μµµ Μµµ");
    TEST_EQUAL(str_titlecase("ǄǄǄ ǄǄǄ"s), "ǅǆǆ ǅǆǆ");
    TEST_EQUAL(str_titlecase("ǅǅǅ ǅǅǅ"s), "ǅǆǆ ǅǆǆ");
    TEST_EQUAL(str_titlecase("ǆǆǆ ǆǆǆ"s), "ǅǆǆ ǅǆǆ");
    TEST_EQUAL(str_case(""s, Case::title), "");
    TEST_EQUAL(str_case("HELLO WORLD"s, Case::title), "Hello World");
    TEST_EQUAL(str_case("hello world"s, Case::title), "Hello World");
    TEST_EQUAL(str_case("(hello-world)"s, Case::title), "(Hello-World)");
    TEST_EQUAL(str_case("@hello@world@"s, Case::title), "@Hello@World@");
    TEST_EQUAL(str_case("ΜΜΜ ΜΜΜ"s, Case::title), "Μμμ Μμμ");
    TEST_EQUAL(str_case("µµµ µµµ"s, Case::title), "Μµµ Μµµ");
    TEST_EQUAL(str_case("ǄǄǄ ǄǄǄ"s, Case::title), "ǅǆǆ ǅǆǆ");
    TEST_EQUAL(str_case("ǅǅǅ ǅǅǅ"s, Case::title), "ǅǆǆ ǅǆǆ");
    TEST_EQUAL(str_case("ǆǆǆ ǆǆǆ"s, Case::title), "ǅǆǆ ǅǆǆ");

    s = "";               TRY(str_titlecase_in(s));          TEST_EQUAL(s, "");
    s = "HELLO WORLD";    TRY(str_titlecase_in(s));          TEST_EQUAL(s, "Hello World");
    s = "hello world";    TRY(str_titlecase_in(s));          TEST_EQUAL(s, "Hello World");
    s = "(hello-world)";  TRY(str_titlecase_in(s));          TEST_EQUAL(s, "(Hello-World)");
    s = "@hello@world@";  TRY(str_titlecase_in(s));          TEST_EQUAL(s, "@Hello@World@");
    s = "ΜΜΜ ΜΜΜ";        TRY(str_titlecase_in(s));          TEST_EQUAL(s, "Μμμ Μμμ");
    s = "µµµ µµµ";        TRY(str_titlecase_in(s));          TEST_EQUAL(s, "Μµµ Μµµ");
    s = "ǄǄǄ ǄǄǄ";        TRY(str_titlecase_in(s));          TEST_EQUAL(s, "ǅǆǆ ǅǆǆ");
    s = "ǅǅǅ ǅǅǅ";        TRY(str_titlecase_in(s));          TEST_EQUAL(s, "ǅǆǆ ǅǆǆ");
    s = "ǆǆǆ ǆǆǆ";        TRY(str_titlecase_in(s));          TEST_EQUAL(s, "ǅǆǆ ǅǆǆ");
    s = "";               TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "");
    s = "HELLO WORLD";    TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "Hello World");
    s = "hello world";    TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "Hello World");
    s = "(hello-world)";  TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "(Hello-World)");
    s = "@hello@world@";  TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "@Hello@World@");
    s = "ΜΜΜ ΜΜΜ";        TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "Μμμ Μμμ");
    s = "µµµ µµµ";        TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "Μµµ Μµµ");
    s = "ǄǄǄ ǄǄǄ";        TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "ǅǆǆ ǅǆǆ");
    s = "ǅǅǅ ǅǅǅ";        TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "ǅǆǆ ǅǆǆ");
    s = "ǆǆǆ ǆǆǆ";        TRY(str_case_in(s, Case::title));  TEST_EQUAL(s, "ǅǆǆ ǅǆǆ");

    TEST_EQUAL(str_casefold(""s), "");
    TEST_EQUAL(str_casefold("HELLO WORLD"s), "hello world");
    TEST_EQUAL(str_casefold("hello world"s), "hello world");
    TEST_EQUAL(str_case(""s, Case::fold), "");
    TEST_EQUAL(str_case("HELLO WORLD"s, Case::fold), "hello world");
    TEST_EQUAL(str_case("hello world"s, Case::fold), "hello world");

    s = "";             TRY(str_casefold_in(s));          TEST_EQUAL(s, "");
    s = "HELLO WORLD";  TRY(str_casefold_in(s));          TEST_EQUAL(s, "hello world");
    s = "hello world";  TRY(str_casefold_in(s));          TEST_EQUAL(s, "hello world");
    s = "";             TRY(str_case_in(s, Case::fold));  TEST_EQUAL(s, "");
    s = "HELLO WORLD";  TRY(str_case_in(s, Case::fold));  TEST_EQUAL(s, "hello world");
    s = "hello world";  TRY(str_case_in(s, Case::fold));  TEST_EQUAL(s, "hello world");

    TEST_EQUAL(str_initial_titlecase(""s), "");
    TEST_EQUAL(str_initial_titlecase("HELLO WORLD"s), "HELLO WORLD");
    TEST_EQUAL(str_initial_titlecase("hello world"s), "Hello world");
    TEST_EQUAL(str_initial_titlecase("(hello-world)"s), "(hello-world)");
    TEST_EQUAL(str_initial_titlecase("ΜΜΜ ΜΜΜ"s), "ΜΜΜ ΜΜΜ");
    TEST_EQUAL(str_initial_titlecase("µµµ µµµ"s), "Μµµ µµµ");
    TEST_EQUAL(str_initial_titlecase("ǄǄǄ ǄǄǄ"s), "ǅǄǄ ǄǄǄ");
    TEST_EQUAL(str_initial_titlecase("ǅǅǅ ǅǅǅ"s), "ǅǅǅ ǅǅǅ");
    TEST_EQUAL(str_initial_titlecase("ǆǆǆ ǆǆǆ"s), "ǅǆǆ ǆǆǆ");

    s = "";               TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, "");
    s = "HELLO WORLD";    TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, "HELLO WORLD");
    s = "hello world";    TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, "Hello world");
    s = "(hello-world)";  TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, "(hello-world)");
    s = "ΜΜΜ ΜΜΜ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, "ΜΜΜ ΜΜΜ");
    s = "µµµ µµµ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, "Μµµ µµµ");
    s = "ǄǄǄ ǄǄǄ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, "ǅǄǄ ǄǄǄ");
    s = "ǅǅǅ ǅǅǅ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, "ǅǅǅ ǅǅǅ");
    s = "ǆǆǆ ǆǆǆ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, "ǅǆǆ ǆǆǆ");

}
