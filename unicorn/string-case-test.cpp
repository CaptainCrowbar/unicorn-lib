#include "unicorn/string-case.hpp"
#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void check_case_conversions() {

        u8string s8;
        u16string s16;
        u32string s32;

        TEST_EQUAL(str_uppercase(u8""s), u8"");
        TEST_EQUAL(str_uppercase(u8"HELLO WORLD"s), u8"HELLO WORLD");
        TEST_EQUAL(str_uppercase(u8"hello world"s), u8"HELLO WORLD");

        TEST_EQUAL(str_uppercase(u""s), u"");
        TEST_EQUAL(str_uppercase(u"HELLO WORLD"s), u"HELLO WORLD");
        TEST_EQUAL(str_uppercase(u"hello world"s), u"HELLO WORLD");

        TEST_EQUAL(str_uppercase(U""s), U"");
        TEST_EQUAL(str_uppercase(U"HELLO WORLD"s), U"HELLO WORLD");
        TEST_EQUAL(str_uppercase(U"hello world"s), U"HELLO WORLD");

        s8 = u8"";             TRY(str_uppercase_in(s8));  TEST_EQUAL(s8, u8"");
        s8 = u8"HELLO WORLD";  TRY(str_uppercase_in(s8));  TEST_EQUAL(s8, u8"HELLO WORLD");
        s8 = u8"hello world";  TRY(str_uppercase_in(s8));  TEST_EQUAL(s8, u8"HELLO WORLD");

        s16 = u"";             TRY(str_uppercase_in(s16));  TEST_EQUAL(s16, u"");
        s16 = u"HELLO WORLD";  TRY(str_uppercase_in(s16));  TEST_EQUAL(s16, u"HELLO WORLD");
        s16 = u"hello world";  TRY(str_uppercase_in(s16));  TEST_EQUAL(s16, u"HELLO WORLD");

        s32 = U"";             TRY(str_uppercase_in(s32));  TEST_EQUAL(s32, U"");
        s32 = U"HELLO WORLD";  TRY(str_uppercase_in(s32));  TEST_EQUAL(s32, U"HELLO WORLD");
        s32 = U"hello world";  TRY(str_uppercase_in(s32));  TEST_EQUAL(s32, U"HELLO WORLD");

        TEST_EQUAL(str_lowercase(u8""s), u8"");
        TEST_EQUAL(str_lowercase(u8"HELLO WORLD"s), u8"hello world");
        TEST_EQUAL(str_lowercase(u8"hello world"s), u8"hello world");

        TEST_EQUAL(str_lowercase(u""s), u"");
        TEST_EQUAL(str_lowercase(u"HELLO WORLD"s), u"hello world");
        TEST_EQUAL(str_lowercase(u"hello world"s), u"hello world");

        TEST_EQUAL(str_lowercase(U""s), U"");
        TEST_EQUAL(str_lowercase(U"HELLO WORLD"s), U"hello world");
        TEST_EQUAL(str_lowercase(U"hello world"s), U"hello world");

        s8 = u8"";             TRY(str_lowercase_in(s8));  TEST_EQUAL(s8, u8"");
        s8 = u8"HELLO WORLD";  TRY(str_lowercase_in(s8));  TEST_EQUAL(s8, u8"hello world");
        s8 = u8"hello world";  TRY(str_lowercase_in(s8));  TEST_EQUAL(s8, u8"hello world");

        s16 = u"";             TRY(str_lowercase_in(s16));  TEST_EQUAL(s16, u"");
        s16 = u"HELLO WORLD";  TRY(str_lowercase_in(s16));  TEST_EQUAL(s16, u"hello world");
        s16 = u"hello world";  TRY(str_lowercase_in(s16));  TEST_EQUAL(s16, u"hello world");

        s32 = U"";             TRY(str_lowercase_in(s32));  TEST_EQUAL(s32, U"");
        s32 = U"HELLO WORLD";  TRY(str_lowercase_in(s32));  TEST_EQUAL(s32, U"hello world");
        s32 = U"hello world";  TRY(str_lowercase_in(s32));  TEST_EQUAL(s32, U"hello world");

        TEST_EQUAL(str_titlecase(u8""s), u8"");
        TEST_EQUAL(str_titlecase(u8"HELLO WORLD"s), u8"Hello World");
        TEST_EQUAL(str_titlecase(u8"hello world"s), u8"Hello World");
        TEST_EQUAL(str_titlecase(u8"hello world"s), u8"Hello World");
        TEST_EQUAL(str_titlecase(u8"(hello-world)"s), u8"(Hello-World)");
        TEST_EQUAL(str_titlecase(u8"@hello@world@"s), u8"@Hello@World@");
        TEST_EQUAL(str_titlecase(u8"ΜΜΜ ΜΜΜ"s), u8"Μμμ Μμμ");
        TEST_EQUAL(str_titlecase(u8"µµµ µµµ"s), u8"Μµµ Μµµ");
        TEST_EQUAL(str_titlecase(u8"ǄǄǄ ǄǄǄ"s), u8"ǅǆǆ ǅǆǆ");
        TEST_EQUAL(str_titlecase(u8"ǅǅǅ ǅǅǅ"s), u8"ǅǆǆ ǅǆǆ");
        TEST_EQUAL(str_titlecase(u8"ǆǆǆ ǆǆǆ"s), u8"ǅǆǆ ǅǆǆ");

        TEST_EQUAL(str_titlecase(u""s), u"");
        TEST_EQUAL(str_titlecase(u"HELLO WORLD"s), u"Hello World");
        TEST_EQUAL(str_titlecase(u"hello world"s), u"Hello World");
        TEST_EQUAL(str_titlecase(u"hello world"s), u"Hello World");
        TEST_EQUAL(str_titlecase(u"(hello-world)"s), u"(Hello-World)");
        TEST_EQUAL(str_titlecase(u"@hello@world@"s), u"@Hello@World@");
        TEST_EQUAL(str_titlecase(u"ΜΜΜ ΜΜΜ"s), u"Μμμ Μμμ");
        TEST_EQUAL(str_titlecase(u"µµµ µµµ"s), u"Μµµ Μµµ");
        TEST_EQUAL(str_titlecase(u"ǄǄǄ ǄǄǄ"s), u"ǅǆǆ ǅǆǆ");
        TEST_EQUAL(str_titlecase(u"ǅǅǅ ǅǅǅ"s), u"ǅǆǆ ǅǆǆ");
        TEST_EQUAL(str_titlecase(u"ǆǆǆ ǆǆǆ"s), u"ǅǆǆ ǅǆǆ");

        TEST_EQUAL(str_titlecase(U""s), U"");
        TEST_EQUAL(str_titlecase(U"HELLO WORLD"s), U"Hello World");
        TEST_EQUAL(str_titlecase(U"hello world"s), U"Hello World");
        TEST_EQUAL(str_titlecase(U"hello world"s), U"Hello World");
        TEST_EQUAL(str_titlecase(U"(hello-world)"s), U"(Hello-World)");
        TEST_EQUAL(str_titlecase(U"@hello@world@"s), U"@Hello@World@");
        TEST_EQUAL(str_titlecase(U"ΜΜΜ ΜΜΜ"s), U"Μμμ Μμμ");
        TEST_EQUAL(str_titlecase(U"µµµ µµµ"s), U"Μµµ Μµµ");
        TEST_EQUAL(str_titlecase(U"ǄǄǄ ǄǄǄ"s), U"ǅǆǆ ǅǆǆ");
        TEST_EQUAL(str_titlecase(U"ǅǅǅ ǅǅǅ"s), U"ǅǆǆ ǅǆǆ");
        TEST_EQUAL(str_titlecase(U"ǆǆǆ ǆǆǆ"s), U"ǅǆǆ ǅǆǆ");

        s8 = u8"";               TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"");
        s8 = u8"HELLO WORLD";    TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"Hello World");
        s8 = u8"hello world";    TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"Hello World");
        s8 = u8"hello world";    TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"Hello World");
        s8 = u8"(hello-world)";  TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"(Hello-World)");
        s8 = u8"@hello@world@";  TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"@Hello@World@");
        s8 = u8"ΜΜΜ ΜΜΜ";        TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"Μμμ Μμμ");
        s8 = u8"µµµ µµµ";        TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"Μµµ Μµµ");
        s8 = u8"ǄǄǄ ǄǄǄ";        TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"ǅǆǆ ǅǆǆ");
        s8 = u8"ǅǅǅ ǅǅǅ";        TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"ǅǆǆ ǅǆǆ");
        s8 = u8"ǆǆǆ ǆǆǆ";        TRY(str_titlecase_in(s8));  TEST_EQUAL(s8, u8"ǅǆǆ ǅǆǆ");

        s16 = u"";               TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"");
        s16 = u"HELLO WORLD";    TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"Hello World");
        s16 = u"hello world";    TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"Hello World");
        s16 = u"hello world";    TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"Hello World");
        s16 = u"(hello-world)";  TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"(Hello-World)");
        s16 = u"@hello@world@";  TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"@Hello@World@");
        s16 = u"ΜΜΜ ΜΜΜ";        TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"Μμμ Μμμ");
        s16 = u"µµµ µµµ";        TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"Μµµ Μµµ");
        s16 = u"ǄǄǄ ǄǄǄ";        TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"ǅǆǆ ǅǆǆ");
        s16 = u"ǅǅǅ ǅǅǅ";        TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"ǅǆǆ ǅǆǆ");
        s16 = u"ǆǆǆ ǆǆǆ";        TRY(str_titlecase_in(s16));  TEST_EQUAL(s16, u"ǅǆǆ ǅǆǆ");

        s32 = U"";               TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"");
        s32 = U"HELLO WORLD";    TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"Hello World");
        s32 = U"hello world";    TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"Hello World");
        s32 = U"hello world";    TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"Hello World");
        s32 = U"(hello-world)";  TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"(Hello-World)");
        s32 = U"@hello@world@";  TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"@Hello@World@");
        s32 = U"ΜΜΜ ΜΜΜ";        TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"Μμμ Μμμ");
        s32 = U"µµµ µµµ";        TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"Μµµ Μµµ");
        s32 = U"ǄǄǄ ǄǄǄ";        TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"ǅǆǆ ǅǆǆ");
        s32 = U"ǅǅǅ ǅǅǅ";        TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"ǅǆǆ ǅǆǆ");
        s32 = U"ǆǆǆ ǆǆǆ";        TRY(str_titlecase_in(s32));  TEST_EQUAL(s32, U"ǅǆǆ ǅǆǆ");

        TEST_EQUAL(str_casefold(u8""s), u8"");
        TEST_EQUAL(str_casefold(u8"HELLO WORLD"s), u8"hello world");
        TEST_EQUAL(str_casefold(u8"hello world"s), u8"hello world");

        TEST_EQUAL(str_casefold(u""s), u"");
        TEST_EQUAL(str_casefold(u"HELLO WORLD"s), u"hello world");
        TEST_EQUAL(str_casefold(u"hello world"s), u"hello world");

        TEST_EQUAL(str_casefold(U""s), U"");
        TEST_EQUAL(str_casefold(U"HELLO WORLD"s), U"hello world");
        TEST_EQUAL(str_casefold(U"hello world"s), U"hello world");

        s8 = u8"";             TRY(str_casefold_in(s8));  TEST_EQUAL(s8, u8"");
        s8 = u8"HELLO WORLD";  TRY(str_casefold_in(s8));  TEST_EQUAL(s8, u8"hello world");
        s8 = u8"hello world";  TRY(str_casefold_in(s8));  TEST_EQUAL(s8, u8"hello world");

        s16 = u"";             TRY(str_casefold_in(s16));  TEST_EQUAL(s16, u"");
        s16 = u"HELLO WORLD";  TRY(str_casefold_in(s16));  TEST_EQUAL(s16, u"hello world");
        s16 = u"hello world";  TRY(str_casefold_in(s16));  TEST_EQUAL(s16, u"hello world");

        s32 = U"";             TRY(str_casefold_in(s32));  TEST_EQUAL(s32, U"");
        s32 = U"HELLO WORLD";  TRY(str_casefold_in(s32));  TEST_EQUAL(s32, U"hello world");
        s32 = U"hello world";  TRY(str_casefold_in(s32));  TEST_EQUAL(s32, U"hello world");

    }

}

TEST_MODULE(unicorn, string_case) {

    check_case_conversions();

}
