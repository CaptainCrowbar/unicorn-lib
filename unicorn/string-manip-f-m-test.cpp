#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <string>
#include <vector>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void check_fix_left() {

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

    }

    void check_fix_right() {

        u8string s8;
        u16string s16;
        u32string s32;

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

    check_fix_left();
    check_fix_right();
    check_join();

}
