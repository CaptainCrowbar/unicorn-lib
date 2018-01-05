#include "unicorn/string.hpp"
#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_case_conversions() {

    Ustring s;

    TEST_EQUAL(str_uppercase(u8""s), u8"");
    TEST_EQUAL(str_uppercase(u8"HELLO WORLD"s), u8"HELLO WORLD");
    TEST_EQUAL(str_uppercase(u8"hello world"s), u8"HELLO WORLD");

    s = u8"";             TRY(str_uppercase_in(s));  TEST_EQUAL(s, u8"");
    s = u8"HELLO WORLD";  TRY(str_uppercase_in(s));  TEST_EQUAL(s, u8"HELLO WORLD");
    s = u8"hello world";  TRY(str_uppercase_in(s));  TEST_EQUAL(s, u8"HELLO WORLD");

    TEST_EQUAL(str_lowercase(u8""s), u8"");
    TEST_EQUAL(str_lowercase(u8"HELLO WORLD"s), u8"hello world");
    TEST_EQUAL(str_lowercase(u8"hello world"s), u8"hello world");

    s = u8"";             TRY(str_lowercase_in(s));  TEST_EQUAL(s, u8"");
    s = u8"HELLO WORLD";  TRY(str_lowercase_in(s));  TEST_EQUAL(s, u8"hello world");
    s = u8"hello world";  TRY(str_lowercase_in(s));  TEST_EQUAL(s, u8"hello world");

    TEST_EQUAL(str_titlecase(u8""s), u8"");
    TEST_EQUAL(str_titlecase(u8"HELLO WORLD"s), u8"Hello World");
    TEST_EQUAL(str_titlecase(u8"hello world"s), u8"Hello World");
    TEST_EQUAL(str_titlecase(u8"(hello-world)"s), u8"(Hello-World)");
    TEST_EQUAL(str_titlecase(u8"@hello@world@"s), u8"@Hello@World@");
    TEST_EQUAL(str_titlecase(u8"ΜΜΜ ΜΜΜ"s), u8"Μμμ Μμμ");
    TEST_EQUAL(str_titlecase(u8"µµµ µµµ"s), u8"Μµµ Μµµ");
    TEST_EQUAL(str_titlecase(u8"ǄǄǄ ǄǄǄ"s), u8"ǅǆǆ ǅǆǆ");
    TEST_EQUAL(str_titlecase(u8"ǅǅǅ ǅǅǅ"s), u8"ǅǆǆ ǅǆǆ");
    TEST_EQUAL(str_titlecase(u8"ǆǆǆ ǆǆǆ"s), u8"ǅǆǆ ǅǆǆ");

    s = u8"";               TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"");
    s = u8"HELLO WORLD";    TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"Hello World");
    s = u8"hello world";    TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"Hello World");
    s = u8"(hello-world)";  TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"(Hello-World)");
    s = u8"@hello@world@";  TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"@Hello@World@");
    s = u8"ΜΜΜ ΜΜΜ";        TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"Μμμ Μμμ");
    s = u8"µµµ µµµ";        TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"Μµµ Μµµ");
    s = u8"ǄǄǄ ǄǄǄ";        TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"ǅǆǆ ǅǆǆ");
    s = u8"ǅǅǅ ǅǅǅ";        TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"ǅǆǆ ǅǆǆ");
    s = u8"ǆǆǆ ǆǆǆ";        TRY(str_titlecase_in(s));  TEST_EQUAL(s, u8"ǅǆǆ ǅǆǆ");

    TEST_EQUAL(str_casefold(u8""s), u8"");
    TEST_EQUAL(str_casefold(u8"HELLO WORLD"s), u8"hello world");
    TEST_EQUAL(str_casefold(u8"hello world"s), u8"hello world");

    s = u8"";             TRY(str_casefold_in(s));  TEST_EQUAL(s, u8"");
    s = u8"HELLO WORLD";  TRY(str_casefold_in(s));  TEST_EQUAL(s, u8"hello world");
    s = u8"hello world";  TRY(str_casefold_in(s));  TEST_EQUAL(s, u8"hello world");

    TEST_EQUAL(str_initial_titlecase(u8""s), u8"");
    TEST_EQUAL(str_initial_titlecase(u8"HELLO WORLD"s), u8"HELLO WORLD");
    TEST_EQUAL(str_initial_titlecase(u8"hello world"s), u8"Hello world");
    TEST_EQUAL(str_initial_titlecase(u8"(hello-world)"s), u8"(hello-world)");
    TEST_EQUAL(str_initial_titlecase(u8"ΜΜΜ ΜΜΜ"s), u8"ΜΜΜ ΜΜΜ");
    TEST_EQUAL(str_initial_titlecase(u8"µµµ µµµ"s), u8"Μµµ µµµ");
    TEST_EQUAL(str_initial_titlecase(u8"ǄǄǄ ǄǄǄ"s), u8"ǅǄǄ ǄǄǄ");
    TEST_EQUAL(str_initial_titlecase(u8"ǅǅǅ ǅǅǅ"s), u8"ǅǅǅ ǅǅǅ");
    TEST_EQUAL(str_initial_titlecase(u8"ǆǆǆ ǆǆǆ"s), u8"ǅǆǆ ǆǆǆ");

    s = u8"";               TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, u8"");
    s = u8"HELLO WORLD";    TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, u8"HELLO WORLD");
    s = u8"hello world";    TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, u8"Hello world");
    s = u8"(hello-world)";  TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, u8"(hello-world)");
    s = u8"ΜΜΜ ΜΜΜ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, u8"ΜΜΜ ΜΜΜ");
    s = u8"µµµ µµµ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, u8"Μµµ µµµ");
    s = u8"ǄǄǄ ǄǄǄ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, u8"ǅǄǄ ǄǄǄ");
    s = u8"ǅǅǅ ǅǅǅ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, u8"ǅǅǅ ǅǅǅ");
    s = u8"ǆǆǆ ǆǆǆ";        TRY(str_initial_titlecase_in(s));  TEST_EQUAL(s, u8"ǅǆǆ ǆǆǆ");

}
