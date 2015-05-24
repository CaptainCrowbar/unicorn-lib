#include "crow/core.hpp"
#include "crow/unit-test.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include <string>
#include <vector>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void check_fix() {

        u8string s8;
        u16string s16;
        u32string s32;

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

        TEST_EQUAL(str_fix_left(u""s, 0), u"");
        TEST_EQUAL(str_fix_left(u""s, 1), u" ");
        TEST_EQUAL(str_fix_left(u""s, 2), u"  ");
        TEST_EQUAL(str_fix_left(u""s, 0, U'*'), u"");
        TEST_EQUAL(str_fix_left(u""s, 1, U'*'), u"*");
        TEST_EQUAL(str_fix_left(u""s, 2, U'*'), u"**");
        TEST_EQUAL(str_fix_left(u""s, 0, U'§'), u"");
        TEST_EQUAL(str_fix_left(u""s, 1, U'§'), u"§");
        TEST_EQUAL(str_fix_left(u""s, 2, U'§'), u"§§");
        TEST_EQUAL(str_fix_left(u"Hello"s, 0), u"");
        TEST_EQUAL(str_fix_left(u"Hello"s, 4), u"Hell");
        TEST_EQUAL(str_fix_left(u"Hello"s, 5), u"Hello");
        TEST_EQUAL(str_fix_left(u"Hello"s, 6), u"Hello ");
        TEST_EQUAL(str_fix_left(u"Hello"s, 7), u"Hello  ");
        TEST_EQUAL(str_fix_left(u"Hello"s, 0, U'*'), u"");
        TEST_EQUAL(str_fix_left(u"Hello"s, 4, U'*'), u"Hell");
        TEST_EQUAL(str_fix_left(u"Hello"s, 5, U'*'), u"Hello");
        TEST_EQUAL(str_fix_left(u"Hello"s, 6, U'*'), u"Hello*");
        TEST_EQUAL(str_fix_left(u"Hello"s, 7, U'*'), u"Hello**");
        TEST_EQUAL(str_fix_left(u"Hello"s, 0, U'§'), u"");
        TEST_EQUAL(str_fix_left(u"Hello"s, 4, U'§'), u"Hell");
        TEST_EQUAL(str_fix_left(u"Hello"s, 5, U'§'), u"Hello");
        TEST_EQUAL(str_fix_left(u"Hello"s, 6, U'§'), u"Hello§");
        TEST_EQUAL(str_fix_left(u"Hello"s, 7, U'§'), u"Hello§§");

        TEST_EQUAL(str_fix_left(U""s, 0), U"");
        TEST_EQUAL(str_fix_left(U""s, 1), U" ");
        TEST_EQUAL(str_fix_left(U""s, 2), U"  ");
        TEST_EQUAL(str_fix_left(U""s, 0, U'*'), U"");
        TEST_EQUAL(str_fix_left(U""s, 1, U'*'), U"*");
        TEST_EQUAL(str_fix_left(U""s, 2, U'*'), U"**");
        TEST_EQUAL(str_fix_left(U""s, 0, U'§'), U"");
        TEST_EQUAL(str_fix_left(U""s, 1, U'§'), U"§");
        TEST_EQUAL(str_fix_left(U""s, 2, U'§'), U"§§");
        TEST_EQUAL(str_fix_left(U"Hello"s, 0), U"");
        TEST_EQUAL(str_fix_left(U"Hello"s, 4), U"Hell");
        TEST_EQUAL(str_fix_left(U"Hello"s, 5), U"Hello");
        TEST_EQUAL(str_fix_left(U"Hello"s, 6), U"Hello ");
        TEST_EQUAL(str_fix_left(U"Hello"s, 7), U"Hello  ");
        TEST_EQUAL(str_fix_left(U"Hello"s, 0, U'*'), U"");
        TEST_EQUAL(str_fix_left(U"Hello"s, 4, U'*'), U"Hell");
        TEST_EQUAL(str_fix_left(U"Hello"s, 5, U'*'), U"Hello");
        TEST_EQUAL(str_fix_left(U"Hello"s, 6, U'*'), U"Hello*");
        TEST_EQUAL(str_fix_left(U"Hello"s, 7, U'*'), U"Hello**");
        TEST_EQUAL(str_fix_left(U"Hello"s, 0, U'§'), U"");
        TEST_EQUAL(str_fix_left(U"Hello"s, 4, U'§'), U"Hell");
        TEST_EQUAL(str_fix_left(U"Hello"s, 5, U'§'), U"Hello");
        TEST_EQUAL(str_fix_left(U"Hello"s, 6, U'§'), U"Hello§");
        TEST_EQUAL(str_fix_left(U"Hello"s, 7, U'§'), U"Hello§§");

        s8 = u8"";       TRY(str_fix_left_in(s8, 0));        TEST_EQUAL(s8, u8"");
        s8 = u8"";       TRY(str_fix_left_in(s8, 1));        TEST_EQUAL(s8, u8" ");
        s8 = u8"";       TRY(str_fix_left_in(s8, 2));        TEST_EQUAL(s8, u8"  ");
        s8 = u8"";       TRY(str_fix_left_in(s8, 0, U'*'));  TEST_EQUAL(s8, u8"");
        s8 = u8"";       TRY(str_fix_left_in(s8, 1, U'*'));  TEST_EQUAL(s8, u8"*");
        s8 = u8"";       TRY(str_fix_left_in(s8, 2, U'*'));  TEST_EQUAL(s8, u8"**");
        s8 = u8"";       TRY(str_fix_left_in(s8, 0, U'§'));  TEST_EQUAL(s8, u8"");
        s8 = u8"";       TRY(str_fix_left_in(s8, 1, U'§'));  TEST_EQUAL(s8, u8"§");
        s8 = u8"";       TRY(str_fix_left_in(s8, 2, U'§'));  TEST_EQUAL(s8, u8"§§");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 0));        TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 4));        TEST_EQUAL(s8, u8"Hell");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 5));        TEST_EQUAL(s8, u8"Hello");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 6));        TEST_EQUAL(s8, u8"Hello ");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 7));        TEST_EQUAL(s8, u8"Hello  ");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 0, U'*'));  TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 4, U'*'));  TEST_EQUAL(s8, u8"Hell");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 5, U'*'));  TEST_EQUAL(s8, u8"Hello");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 6, U'*'));  TEST_EQUAL(s8, u8"Hello*");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 7, U'*'));  TEST_EQUAL(s8, u8"Hello**");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 0, U'§'));  TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 4, U'§'));  TEST_EQUAL(s8, u8"Hell");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 5, U'§'));  TEST_EQUAL(s8, u8"Hello");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 6, U'§'));  TEST_EQUAL(s8, u8"Hello§");
        s8 = u8"Hello";  TRY(str_fix_left_in(s8, 7, U'§'));  TEST_EQUAL(s8, u8"Hello§§");

        s16 = u"";       TRY(str_fix_left_in(s16, 0));        TEST_EQUAL(s16, u"");
        s16 = u"";       TRY(str_fix_left_in(s16, 1));        TEST_EQUAL(s16, u" ");
        s16 = u"";       TRY(str_fix_left_in(s16, 2));        TEST_EQUAL(s16, u"  ");
        s16 = u"";       TRY(str_fix_left_in(s16, 0, U'*'));  TEST_EQUAL(s16, u"");
        s16 = u"";       TRY(str_fix_left_in(s16, 1, U'*'));  TEST_EQUAL(s16, u"*");
        s16 = u"";       TRY(str_fix_left_in(s16, 2, U'*'));  TEST_EQUAL(s16, u"**");
        s16 = u"";       TRY(str_fix_left_in(s16, 0, U'§'));  TEST_EQUAL(s16, u"");
        s16 = u"";       TRY(str_fix_left_in(s16, 1, U'§'));  TEST_EQUAL(s16, u"§");
        s16 = u"";       TRY(str_fix_left_in(s16, 2, U'§'));  TEST_EQUAL(s16, u"§§");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 0));        TEST_EQUAL(s16, u"");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 4));        TEST_EQUAL(s16, u"Hell");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 5));        TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 6));        TEST_EQUAL(s16, u"Hello ");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 7));        TEST_EQUAL(s16, u"Hello  ");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 0, U'*'));  TEST_EQUAL(s16, u"");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 4, U'*'));  TEST_EQUAL(s16, u"Hell");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 5, U'*'));  TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 6, U'*'));  TEST_EQUAL(s16, u"Hello*");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 7, U'*'));  TEST_EQUAL(s16, u"Hello**");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 0, U'§'));  TEST_EQUAL(s16, u"");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 4, U'§'));  TEST_EQUAL(s16, u"Hell");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 5, U'§'));  TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 6, U'§'));  TEST_EQUAL(s16, u"Hello§");
        s16 = u"Hello";  TRY(str_fix_left_in(s16, 7, U'§'));  TEST_EQUAL(s16, u"Hello§§");

        s32 = U"";       TRY(str_fix_left_in(s32, 0));        TEST_EQUAL(s32, U"");
        s32 = U"";       TRY(str_fix_left_in(s32, 1));        TEST_EQUAL(s32, U" ");
        s32 = U"";       TRY(str_fix_left_in(s32, 2));        TEST_EQUAL(s32, U"  ");
        s32 = U"";       TRY(str_fix_left_in(s32, 0, U'*'));  TEST_EQUAL(s32, U"");
        s32 = U"";       TRY(str_fix_left_in(s32, 1, U'*'));  TEST_EQUAL(s32, U"*");
        s32 = U"";       TRY(str_fix_left_in(s32, 2, U'*'));  TEST_EQUAL(s32, U"**");
        s32 = U"";       TRY(str_fix_left_in(s32, 0, U'§'));  TEST_EQUAL(s32, U"");
        s32 = U"";       TRY(str_fix_left_in(s32, 1, U'§'));  TEST_EQUAL(s32, U"§");
        s32 = U"";       TRY(str_fix_left_in(s32, 2, U'§'));  TEST_EQUAL(s32, U"§§");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 0));        TEST_EQUAL(s32, U"");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 4));        TEST_EQUAL(s32, U"Hell");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 5));        TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 6));        TEST_EQUAL(s32, U"Hello ");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 7));        TEST_EQUAL(s32, U"Hello  ");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 0, U'*'));  TEST_EQUAL(s32, U"");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 4, U'*'));  TEST_EQUAL(s32, U"Hell");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 5, U'*'));  TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 6, U'*'));  TEST_EQUAL(s32, U"Hello*");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 7, U'*'));  TEST_EQUAL(s32, U"Hello**");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 0, U'§'));  TEST_EQUAL(s32, U"");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 4, U'§'));  TEST_EQUAL(s32, U"Hell");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 5, U'§'));  TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 6, U'§'));  TEST_EQUAL(s32, U"Hello§");
        s32 = U"Hello";  TRY(str_fix_left_in(s32, 7, U'§'));  TEST_EQUAL(s32, U"Hello§§");

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

        TEST_EQUAL(str_fix_right(u""s, 0), u"");
        TEST_EQUAL(str_fix_right(u""s, 1), u" ");
        TEST_EQUAL(str_fix_right(u""s, 2), u"  ");
        TEST_EQUAL(str_fix_right(u""s, 0, U'*'), u"");
        TEST_EQUAL(str_fix_right(u""s, 1, U'*'), u"*");
        TEST_EQUAL(str_fix_right(u""s, 2, U'*'), u"**");
        TEST_EQUAL(str_fix_right(u""s, 0, U'§'), u"");
        TEST_EQUAL(str_fix_right(u""s, 1, U'§'), u"§");
        TEST_EQUAL(str_fix_right(u""s, 2, U'§'), u"§§");
        TEST_EQUAL(str_fix_right(u"Hello"s, 0), u"");
        TEST_EQUAL(str_fix_right(u"Hello"s, 4), u"ello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 5), u"Hello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 6), u" Hello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 7), u"  Hello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 0, U'*'), u"");
        TEST_EQUAL(str_fix_right(u"Hello"s, 4, U'*'), u"ello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 5, U'*'), u"Hello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 6, U'*'), u"*Hello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 7, U'*'), u"**Hello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 0, U'§'), u"");
        TEST_EQUAL(str_fix_right(u"Hello"s, 4, U'§'), u"ello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 5, U'§'), u"Hello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 6, U'§'), u"§Hello");
        TEST_EQUAL(str_fix_right(u"Hello"s, 7, U'§'), u"§§Hello");

        TEST_EQUAL(str_fix_right(U""s, 0), U"");
        TEST_EQUAL(str_fix_right(U""s, 1), U" ");
        TEST_EQUAL(str_fix_right(U""s, 2), U"  ");
        TEST_EQUAL(str_fix_right(U""s, 0, U'*'), U"");
        TEST_EQUAL(str_fix_right(U""s, 1, U'*'), U"*");
        TEST_EQUAL(str_fix_right(U""s, 2, U'*'), U"**");
        TEST_EQUAL(str_fix_right(U""s, 0, U'§'), U"");
        TEST_EQUAL(str_fix_right(U""s, 1, U'§'), U"§");
        TEST_EQUAL(str_fix_right(U""s, 2, U'§'), U"§§");
        TEST_EQUAL(str_fix_right(U"Hello"s, 0), U"");
        TEST_EQUAL(str_fix_right(U"Hello"s, 4), U"ello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 5), U"Hello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 6), U" Hello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 7), U"  Hello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 0, U'*'), U"");
        TEST_EQUAL(str_fix_right(U"Hello"s, 4, U'*'), U"ello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 5, U'*'), U"Hello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 6, U'*'), U"*Hello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 7, U'*'), U"**Hello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 0, U'§'), U"");
        TEST_EQUAL(str_fix_right(U"Hello"s, 4, U'§'), U"ello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 5, U'§'), U"Hello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 6, U'§'), U"§Hello");
        TEST_EQUAL(str_fix_right(U"Hello"s, 7, U'§'), U"§§Hello");

        s8 = u8"";       TRY(str_fix_right_in(s8, 0));        TEST_EQUAL(s8, u8"");
        s8 = u8"";       TRY(str_fix_right_in(s8, 1));        TEST_EQUAL(s8, u8" ");
        s8 = u8"";       TRY(str_fix_right_in(s8, 2));        TEST_EQUAL(s8, u8"  ");
        s8 = u8"";       TRY(str_fix_right_in(s8, 0, U'*'));  TEST_EQUAL(s8, u8"");
        s8 = u8"";       TRY(str_fix_right_in(s8, 1, U'*'));  TEST_EQUAL(s8, u8"*");
        s8 = u8"";       TRY(str_fix_right_in(s8, 2, U'*'));  TEST_EQUAL(s8, u8"**");
        s8 = u8"";       TRY(str_fix_right_in(s8, 0, U'§'));  TEST_EQUAL(s8, u8"");
        s8 = u8"";       TRY(str_fix_right_in(s8, 1, U'§'));  TEST_EQUAL(s8, u8"§");
        s8 = u8"";       TRY(str_fix_right_in(s8, 2, U'§'));  TEST_EQUAL(s8, u8"§§");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 0));        TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 4));        TEST_EQUAL(s8, u8"ello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 5));        TEST_EQUAL(s8, u8"Hello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 6));        TEST_EQUAL(s8, u8" Hello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 7));        TEST_EQUAL(s8, u8"  Hello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 0, U'*'));  TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 4, U'*'));  TEST_EQUAL(s8, u8"ello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 5, U'*'));  TEST_EQUAL(s8, u8"Hello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 6, U'*'));  TEST_EQUAL(s8, u8"*Hello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 7, U'*'));  TEST_EQUAL(s8, u8"**Hello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 0, U'§'));  TEST_EQUAL(s8, u8"");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 4, U'§'));  TEST_EQUAL(s8, u8"ello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 5, U'§'));  TEST_EQUAL(s8, u8"Hello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 6, U'§'));  TEST_EQUAL(s8, u8"§Hello");
        s8 = u8"Hello";  TRY(str_fix_right_in(s8, 7, U'§'));  TEST_EQUAL(s8, u8"§§Hello");

        s16 = u"";       TRY(str_fix_right_in(s16, 0));        TEST_EQUAL(s16, u"");
        s16 = u"";       TRY(str_fix_right_in(s16, 1));        TEST_EQUAL(s16, u" ");
        s16 = u"";       TRY(str_fix_right_in(s16, 2));        TEST_EQUAL(s16, u"  ");
        s16 = u"";       TRY(str_fix_right_in(s16, 0, U'*'));  TEST_EQUAL(s16, u"");
        s16 = u"";       TRY(str_fix_right_in(s16, 1, U'*'));  TEST_EQUAL(s16, u"*");
        s16 = u"";       TRY(str_fix_right_in(s16, 2, U'*'));  TEST_EQUAL(s16, u"**");
        s16 = u"";       TRY(str_fix_right_in(s16, 0, U'§'));  TEST_EQUAL(s16, u"");
        s16 = u"";       TRY(str_fix_right_in(s16, 1, U'§'));  TEST_EQUAL(s16, u"§");
        s16 = u"";       TRY(str_fix_right_in(s16, 2, U'§'));  TEST_EQUAL(s16, u"§§");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 0));        TEST_EQUAL(s16, u"");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 4));        TEST_EQUAL(s16, u"ello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 5));        TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 6));        TEST_EQUAL(s16, u" Hello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 7));        TEST_EQUAL(s16, u"  Hello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 0, U'*'));  TEST_EQUAL(s16, u"");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 4, U'*'));  TEST_EQUAL(s16, u"ello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 5, U'*'));  TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 6, U'*'));  TEST_EQUAL(s16, u"*Hello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 7, U'*'));  TEST_EQUAL(s16, u"**Hello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 0, U'§'));  TEST_EQUAL(s16, u"");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 4, U'§'));  TEST_EQUAL(s16, u"ello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 5, U'§'));  TEST_EQUAL(s16, u"Hello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 6, U'§'));  TEST_EQUAL(s16, u"§Hello");
        s16 = u"Hello";  TRY(str_fix_right_in(s16, 7, U'§'));  TEST_EQUAL(s16, u"§§Hello");

        s32 = U"";       TRY(str_fix_right_in(s32, 0));        TEST_EQUAL(s32, U"");
        s32 = U"";       TRY(str_fix_right_in(s32, 1));        TEST_EQUAL(s32, U" ");
        s32 = U"";       TRY(str_fix_right_in(s32, 2));        TEST_EQUAL(s32, U"  ");
        s32 = U"";       TRY(str_fix_right_in(s32, 0, U'*'));  TEST_EQUAL(s32, U"");
        s32 = U"";       TRY(str_fix_right_in(s32, 1, U'*'));  TEST_EQUAL(s32, U"*");
        s32 = U"";       TRY(str_fix_right_in(s32, 2, U'*'));  TEST_EQUAL(s32, U"**");
        s32 = U"";       TRY(str_fix_right_in(s32, 0, U'§'));  TEST_EQUAL(s32, U"");
        s32 = U"";       TRY(str_fix_right_in(s32, 1, U'§'));  TEST_EQUAL(s32, U"§");
        s32 = U"";       TRY(str_fix_right_in(s32, 2, U'§'));  TEST_EQUAL(s32, U"§§");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 0));        TEST_EQUAL(s32, U"");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 4));        TEST_EQUAL(s32, U"ello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 5));        TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 6));        TEST_EQUAL(s32, U" Hello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 7));        TEST_EQUAL(s32, U"  Hello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 0, U'*'));  TEST_EQUAL(s32, U"");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 4, U'*'));  TEST_EQUAL(s32, U"ello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 5, U'*'));  TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 6, U'*'));  TEST_EQUAL(s32, U"*Hello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 7, U'*'));  TEST_EQUAL(s32, U"**Hello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 0, U'§'));  TEST_EQUAL(s32, U"");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 4, U'§'));  TEST_EQUAL(s32, U"ello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 5, U'§'));  TEST_EQUAL(s32, U"Hello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 6, U'§'));  TEST_EQUAL(s32, U"§Hello");
        s32 = U"Hello";  TRY(str_fix_right_in(s32, 7, U'§'));  TEST_EQUAL(s32, U"§§Hello");

    }

    void check_fold() {

        u8string s8;
        u16string s16;
        u32string s32;

        TEST_EQUAL(str_fold_whitespace(""s), "");
        TEST_EQUAL(str_fold_whitespace("\t\t"s), " ");
        TEST_EQUAL(str_fold_whitespace("\t\t"s, fold_leading), "");
        TEST_EQUAL(str_fold_whitespace("\t\t"s, fold_trailing), "");
        TEST_EQUAL(str_fold_whitespace("Hello world"s), "Hello world");
        TEST_EQUAL(str_fold_whitespace("\t\tHello\t\tworld\t\t"s), " Hello world ");
        TEST_EQUAL(str_fold_whitespace("\t\tHello\t\tworld\t\t"s, fold_leading), "Hello world ");
        TEST_EQUAL(str_fold_whitespace("\t\tHello\t\tworld\t\t"s, fold_trailing), " Hello world");
        TEST_EQUAL(str_fold_whitespace("\t\tHello\t\tworld\t\t"s, fold_leading | fold_trailing), "Hello world");
        TEST_EQUAL(str_fold_whitespace("Hello\nworld\nagain"s), "Hello\nworld\nagain");
        TEST_EQUAL(str_fold_whitespace("Hello\nworld\nagain"s, fold_leading | fold_trailing), "Hello\nworld\nagain");
        TEST_EQUAL(str_fold_whitespace("Hello\nworld\nagain"s, fold_lines), "Hello world again");
        TEST_EQUAL(str_fold_whitespace("Hello\nworld\nagain"s, fold_leading | fold_trailing | fold_lines), "Hello world again");
        TEST_EQUAL(str_fold_whitespace("\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s), " Hello \n world \n again ");
        TEST_EQUAL(str_fold_whitespace("\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s, fold_leading | fold_trailing), "Hello\nworld\nagain");
        TEST_EQUAL(str_fold_whitespace("\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s, fold_lines), " Hello world again ");
        TEST_EQUAL(str_fold_whitespace("\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s, fold_leading | fold_trailing | fold_lines), "Hello world again");
        TEST_EQUAL(str_fold_whitespace("Hello \x01\x02\x03 world"s), "Hello \x01\x02\x03 world");
        TEST_EQUAL(str_fold_whitespace("Hello \x01\x02\x03 world"s, fold_control), "Hello world");

        TEST_EQUAL(str_fold_whitespace(u""s), u"");
        TEST_EQUAL(str_fold_whitespace(u"\t\t"s), u" ");
        TEST_EQUAL(str_fold_whitespace(u"\t\t"s, fold_leading), u"");
        TEST_EQUAL(str_fold_whitespace(u"\t\t"s, fold_trailing), u"");
        TEST_EQUAL(str_fold_whitespace(u"Hello world"s), u"Hello world");
        TEST_EQUAL(str_fold_whitespace(u"\t\tHello\t\tworld\t\t"s), u" Hello world ");
        TEST_EQUAL(str_fold_whitespace(u"\t\tHello\t\tworld\t\t"s, fold_leading), u"Hello world ");
        TEST_EQUAL(str_fold_whitespace(u"\t\tHello\t\tworld\t\t"s, fold_trailing), u" Hello world");
        TEST_EQUAL(str_fold_whitespace(u"\t\tHello\t\tworld\t\t"s, fold_leading | fold_trailing), u"Hello world");
        TEST_EQUAL(str_fold_whitespace(u"Hello\nworld\nagain"s), u"Hello\nworld\nagain");
        TEST_EQUAL(str_fold_whitespace(u"Hello\nworld\nagain"s, fold_leading | fold_trailing), u"Hello\nworld\nagain");
        TEST_EQUAL(str_fold_whitespace(u"Hello\nworld\nagain"s, fold_lines), u"Hello world again");
        TEST_EQUAL(str_fold_whitespace(u"Hello\nworld\nagain"s, fold_leading | fold_trailing | fold_lines), u"Hello world again");
        TEST_EQUAL(str_fold_whitespace(u"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s), u" Hello \n world \n again ");
        TEST_EQUAL(str_fold_whitespace(u"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s, fold_leading | fold_trailing), u"Hello\nworld\nagain");
        TEST_EQUAL(str_fold_whitespace(u"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s, fold_lines), u" Hello world again ");
        TEST_EQUAL(str_fold_whitespace(u"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s, fold_leading | fold_trailing | fold_lines), u"Hello world again");
        TEST_EQUAL(str_fold_whitespace(u"Hello \x01\x02\x03 world"s), u"Hello \x01\x02\x03 world");
        TEST_EQUAL(str_fold_whitespace(u"Hello \x01\x02\x03 world"s, fold_control), u"Hello world");

        TEST_EQUAL(str_fold_whitespace(U""s), U"");
        TEST_EQUAL(str_fold_whitespace(U"\t\t"s), U" ");
        TEST_EQUAL(str_fold_whitespace(U"\t\t"s, fold_leading), U"");
        TEST_EQUAL(str_fold_whitespace(U"\t\t"s, fold_trailing), U"");
        TEST_EQUAL(str_fold_whitespace(U"Hello world"s), U"Hello world");
        TEST_EQUAL(str_fold_whitespace(U"\t\tHello\t\tworld\t\t"s), U" Hello world ");
        TEST_EQUAL(str_fold_whitespace(U"\t\tHello\t\tworld\t\t"s, fold_leading), U"Hello world ");
        TEST_EQUAL(str_fold_whitespace(U"\t\tHello\t\tworld\t\t"s, fold_trailing), U" Hello world");
        TEST_EQUAL(str_fold_whitespace(U"\t\tHello\t\tworld\t\t"s, fold_leading | fold_trailing), U"Hello world");
        TEST_EQUAL(str_fold_whitespace(U"Hello\nworld\nagain"s), U"Hello\nworld\nagain");
        TEST_EQUAL(str_fold_whitespace(U"Hello\nworld\nagain"s, fold_leading | fold_trailing), U"Hello\nworld\nagain");
        TEST_EQUAL(str_fold_whitespace(U"Hello\nworld\nagain"s, fold_lines), U"Hello world again");
        TEST_EQUAL(str_fold_whitespace(U"Hello\nworld\nagain"s, fold_leading | fold_trailing | fold_lines), U"Hello world again");
        TEST_EQUAL(str_fold_whitespace(U"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s), U" Hello \n world \n again ");
        TEST_EQUAL(str_fold_whitespace(U"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s, fold_leading | fold_trailing), U"Hello\nworld\nagain");
        TEST_EQUAL(str_fold_whitespace(U"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s, fold_lines), U" Hello world again ");
        TEST_EQUAL(str_fold_whitespace(U"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t"s, fold_leading | fold_trailing | fold_lines), U"Hello world again");
        TEST_EQUAL(str_fold_whitespace(U"Hello \x01\x02\x03 world"s), U"Hello \x01\x02\x03 world");
        TEST_EQUAL(str_fold_whitespace(U"Hello \x01\x02\x03 world"s, fold_control), U"Hello world");

        s8 = "";                                             TRY(str_fold_whitespace_in(s8));                                             TEST_EQUAL(s8, "");
        s8 = "\t\t";                                         TRY(str_fold_whitespace_in(s8));                                             TEST_EQUAL(s8, " ");
        s8 = "\t\t";                                         TRY(str_fold_whitespace_in(s8, fold_leading));                               TEST_EQUAL(s8, "");
        s8 = "\t\t";                                         TRY(str_fold_whitespace_in(s8, fold_trailing));                              TEST_EQUAL(s8, "");
        s8 = "Hello world";                                  TRY(str_fold_whitespace_in(s8));                                             TEST_EQUAL(s8, "Hello world");
        s8 = "\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s8));                                             TEST_EQUAL(s8, " Hello world ");
        s8 = "\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s8, fold_leading));                               TEST_EQUAL(s8, "Hello world ");
        s8 = "\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s8, fold_trailing));                              TEST_EQUAL(s8, " Hello world");
        s8 = "\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s8, fold_leading | fold_trailing));               TEST_EQUAL(s8, "Hello world");
        s8 = "Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s8));                                             TEST_EQUAL(s8, "Hello\nworld\nagain");
        s8 = "Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s8, fold_leading | fold_trailing));               TEST_EQUAL(s8, "Hello\nworld\nagain");
        s8 = "Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s8, fold_lines));                                 TEST_EQUAL(s8, "Hello world again");
        s8 = "Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s8, fold_leading | fold_trailing | fold_lines));  TEST_EQUAL(s8, "Hello world again");
        s8 = "\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s8));                                             TEST_EQUAL(s8, " Hello \n world \n again ");
        s8 = "\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s8, fold_leading | fold_trailing));               TEST_EQUAL(s8, "Hello\nworld\nagain");
        s8 = "\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s8, fold_lines));                                 TEST_EQUAL(s8, " Hello world again ");
        s8 = "\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s8, fold_leading | fold_trailing | fold_lines));  TEST_EQUAL(s8, "Hello world again");
        s8 = "Hello \x01\x02\x03 world";                     TRY(str_fold_whitespace_in(s8));                                             TEST_EQUAL(s8, "Hello \x01\x02\x03 world");
        s8 = "Hello \x01\x02\x03 world";                     TRY(str_fold_whitespace_in(s8, fold_control));                               TEST_EQUAL(s8, "Hello world");

        s16 = u"";                                             TRY(str_fold_whitespace_in(s16));                                             TEST_EQUAL(s16, u"");
        s16 = u"\t\t";                                         TRY(str_fold_whitespace_in(s16));                                             TEST_EQUAL(s16, u" ");
        s16 = u"\t\t";                                         TRY(str_fold_whitespace_in(s16, fold_leading));                               TEST_EQUAL(s16, u"");
        s16 = u"\t\t";                                         TRY(str_fold_whitespace_in(s16, fold_trailing));                              TEST_EQUAL(s16, u"");
        s16 = u"Hello world";                                  TRY(str_fold_whitespace_in(s16));                                             TEST_EQUAL(s16, u"Hello world");
        s16 = u"\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s16));                                             TEST_EQUAL(s16, u" Hello world ");
        s16 = u"\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s16, fold_leading));                               TEST_EQUAL(s16, u"Hello world ");
        s16 = u"\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s16, fold_trailing));                              TEST_EQUAL(s16, u" Hello world");
        s16 = u"\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s16, fold_leading | fold_trailing));               TEST_EQUAL(s16, u"Hello world");
        s16 = u"Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s16));                                             TEST_EQUAL(s16, u"Hello\nworld\nagain");
        s16 = u"Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s16, fold_leading | fold_trailing));               TEST_EQUAL(s16, u"Hello\nworld\nagain");
        s16 = u"Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s16, fold_lines));                                 TEST_EQUAL(s16, u"Hello world again");
        s16 = u"Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s16, fold_leading | fold_trailing | fold_lines));  TEST_EQUAL(s16, u"Hello world again");
        s16 = u"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s16));                                             TEST_EQUAL(s16, u" Hello \n world \n again ");
        s16 = u"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s16, fold_leading | fold_trailing));               TEST_EQUAL(s16, u"Hello\nworld\nagain");
        s16 = u"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s16, fold_lines));                                 TEST_EQUAL(s16, u" Hello world again ");
        s16 = u"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s16, fold_leading | fold_trailing | fold_lines));  TEST_EQUAL(s16, u"Hello world again");
        s16 = u"Hello \x01\x02\x03 world";                     TRY(str_fold_whitespace_in(s16));                                             TEST_EQUAL(s16, u"Hello \x01\x02\x03 world");
        s16 = u"Hello \x01\x02\x03 world";                     TRY(str_fold_whitespace_in(s16, fold_control));                               TEST_EQUAL(s16, u"Hello world");

        s32 = U"";                                             TRY(str_fold_whitespace_in(s32));                                             TEST_EQUAL(s32, U"");
        s32 = U"\t\t";                                         TRY(str_fold_whitespace_in(s32));                                             TEST_EQUAL(s32, U" ");
        s32 = U"\t\t";                                         TRY(str_fold_whitespace_in(s32, fold_leading));                               TEST_EQUAL(s32, U"");
        s32 = U"\t\t";                                         TRY(str_fold_whitespace_in(s32, fold_trailing));                              TEST_EQUAL(s32, U"");
        s32 = U"Hello world";                                  TRY(str_fold_whitespace_in(s32));                                             TEST_EQUAL(s32, U"Hello world");
        s32 = U"\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s32));                                             TEST_EQUAL(s32, U" Hello world ");
        s32 = U"\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s32, fold_leading));                               TEST_EQUAL(s32, U"Hello world ");
        s32 = U"\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s32, fold_trailing));                              TEST_EQUAL(s32, U" Hello world");
        s32 = U"\t\tHello\t\tworld\t\t";                       TRY(str_fold_whitespace_in(s32, fold_leading | fold_trailing));               TEST_EQUAL(s32, U"Hello world");
        s32 = U"Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s32));                                             TEST_EQUAL(s32, U"Hello\nworld\nagain");
        s32 = U"Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s32, fold_leading | fold_trailing));               TEST_EQUAL(s32, U"Hello\nworld\nagain");
        s32 = U"Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s32, fold_lines));                                 TEST_EQUAL(s32, U"Hello world again");
        s32 = U"Hello\nworld\nagain";                          TRY(str_fold_whitespace_in(s32, fold_leading | fold_trailing | fold_lines));  TEST_EQUAL(s32, U"Hello world again");
        s32 = U"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s32));                                             TEST_EQUAL(s32, U" Hello \n world \n again ");
        s32 = U"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s32, fold_leading | fold_trailing));               TEST_EQUAL(s32, U"Hello\nworld\nagain");
        s32 = U"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s32, fold_lines));                                 TEST_EQUAL(s32, U" Hello world again ");
        s32 = U"\t\tHello\t\t\n\t\tworld\t\t\n\t\tagain\t\t";  TRY(str_fold_whitespace_in(s32, fold_leading | fold_trailing | fold_lines));  TEST_EQUAL(s32, U"Hello world again");
        s32 = U"Hello \x01\x02\x03 world";                     TRY(str_fold_whitespace_in(s32));                                             TEST_EQUAL(s32, U"Hello \x01\x02\x03 world");
        s32 = U"Hello \x01\x02\x03 world";                     TRY(str_fold_whitespace_in(s32, fold_control));                               TEST_EQUAL(s32, U"Hello world");

    }

    void check_join() {

        std::vector<u8string> v8;
        std::vector<u16string> v16;
        std::vector<u32string> v32;

        v8.clear();                      TEST_EQUAL(str_join(v8), "");
        v8 = {"Hello"};                  TEST_EQUAL(str_join(v8), "Hello");
        v8 = {"Hello","world"};          TEST_EQUAL(str_join(v8), "Helloworld");
        v8 = {"Hello","world","again"};  TEST_EQUAL(str_join(v8), "Helloworldagain");
        v8.clear();                      TEST_EQUAL(str_join(v8, " "), "");
        v8 = {"Hello"};                  TEST_EQUAL(str_join(v8, " "), "Hello");
        v8 = {"Hello","world"};          TEST_EQUAL(str_join(v8, " "), "Hello world");
        v8 = {"Hello","world","again"};  TEST_EQUAL(str_join(v8, " "), "Hello world again");
        v8.clear();                      TEST_EQUAL(str_join(v8, "<*>"s), "");
        v8 = {"Hello"};                  TEST_EQUAL(str_join(v8, "<*>"s), "Hello");
        v8 = {"Hello","world"};          TEST_EQUAL(str_join(v8, "<*>"s), "Hello<*>world");
        v8 = {"Hello","world","again"};  TEST_EQUAL(str_join(v8, "<*>"s), "Hello<*>world<*>again");

        v16.clear();                         TEST_EQUAL(str_join(v16), u"");
        v16 = {u"Hello"};                    TEST_EQUAL(str_join(v16), u"Hello");
        v16 = {u"Hello",u"world"};           TEST_EQUAL(str_join(v16), u"Helloworld");
        v16 = {u"Hello",u"world",u"again"};  TEST_EQUAL(str_join(v16), u"Helloworldagain");
        v16.clear();                         TEST_EQUAL(str_join(v16, u" "), u"");
        v16 = {u"Hello"};                    TEST_EQUAL(str_join(v16, u" "), u"Hello");
        v16 = {u"Hello",u"world"};           TEST_EQUAL(str_join(v16, u" "), u"Hello world");
        v16 = {u"Hello",u"world",u"again"};  TEST_EQUAL(str_join(v16, u" "), u"Hello world again");
        v16.clear();                         TEST_EQUAL(str_join(v16, u"<*>"s), u"");
        v16 = {u"Hello"};                    TEST_EQUAL(str_join(v16, u"<*>"s), u"Hello");
        v16 = {u"Hello",u"world"};           TEST_EQUAL(str_join(v16, u"<*>"s), u"Hello<*>world");
        v16 = {u"Hello",u"world",u"again"};  TEST_EQUAL(str_join(v16, u"<*>"s), u"Hello<*>world<*>again");

        v32.clear();                         TEST_EQUAL(str_join(v32), U"");
        v32 = {U"Hello"};                    TEST_EQUAL(str_join(v32), U"Hello");
        v32 = {U"Hello",U"world"};           TEST_EQUAL(str_join(v32), U"Helloworld");
        v32 = {U"Hello",U"world",U"again"};  TEST_EQUAL(str_join(v32), U"Helloworldagain");
        v32.clear();                         TEST_EQUAL(str_join(v32, U" "), U"");
        v32 = {U"Hello"};                    TEST_EQUAL(str_join(v32, U" "), U"Hello");
        v32 = {U"Hello",U"world"};           TEST_EQUAL(str_join(v32, U" "), U"Hello world");
        v32 = {U"Hello",U"world",U"again"};  TEST_EQUAL(str_join(v32, U" "), U"Hello world again");
        v32.clear();                         TEST_EQUAL(str_join(v32, U"<*>"s), U"");
        v32 = {U"Hello"};                    TEST_EQUAL(str_join(v32, U"<*>"s), U"Hello");
        v32 = {U"Hello",U"world"};           TEST_EQUAL(str_join(v32, U"<*>"s), U"Hello<*>world");
        v32 = {U"Hello",U"world",U"again"};  TEST_EQUAL(str_join(v32, U"<*>"s), U"Hello<*>world<*>again");

    }

}

TEST_MODULE(unicorn, string_manipulation_f_m) {

    check_fix();
    check_fold();
    check_join();

}
