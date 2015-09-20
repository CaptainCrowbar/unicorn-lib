#include "unicorn/core.hpp"
#include "prion/unit-test.hpp"
#include <string>

using namespace std::literals;
using namespace Unicorn;
using namespace Unicorn::Literals;

TEST_MAIN;

namespace {

    UNICORN_DEFINE_FLAG(core, cf_alpha, 0);
    UNICORN_DEFINE_FLAG(core, cf_bravo, 1);
    UNICORN_DEFINE_FLAG(core, cf_charlie, 2);

    UNICORN_DEFINE_FLAG_LETTER(letter, uc_alpha, 'A');
    UNICORN_DEFINE_FLAG_LETTER(letter, uc_zulu, 'Z');
    UNICORN_DEFINE_FLAG_LETTER(letter, lc_alpha, 'a');
    UNICORN_DEFINE_FLAG_LETTER(letter, lc_zulu, 'z');

    void check_flag_operations() {

        using namespace UnicornDetail;

        uint32_t f;

        TEST_EQUAL(flag_name("core", cf_alpha), "cf_alpha");
        TEST_EQUAL(flag_name("core", cf_bravo), "cf_bravo");
        TEST_EQUAL(flag_name("core", cf_charlie), "cf_charlie");

        TEST_EQUAL(letter_to_flag('A'), 1ull << 0);
        TEST_EQUAL(letter_to_flag('Z'), 1ull << 25);
        TEST_EQUAL(letter_to_flag('a'), 1ull << 26);
        TEST_EQUAL(letter_to_flag('z'), 1ull << 51);

        TEST_EQUAL(uc_alpha, 1ull << 0);
        TEST_EQUAL(uc_zulu, 1ull << 25);
        TEST_EQUAL(lc_alpha, 1ull << 26);
        TEST_EQUAL(lc_zulu, 1ull << 51);

        f = 0;  TRY(allow_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"));
        f = 1;  TRY(allow_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"));
        f = 7;  TRY(allow_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"));
        f = 8;  TEST_THROW_EQUAL(allow_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"), std::invalid_argument, "Invalid core flags: 0x8");

        f = 0;  TRY(exclusive_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"));
        f = 1;  TRY(exclusive_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"));
        f = 2;  TRY(exclusive_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"));
        f = 4;  TRY(exclusive_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"));
        f = 3;  TEST_THROW_EQUAL(exclusive_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"), std::invalid_argument, "Inconsistent core flags: cf_alpha, cf_bravo");
        f = 7;  TEST_THROW_EQUAL(exclusive_flags(f, cf_alpha | cf_bravo | cf_charlie, "core"), std::invalid_argument, "Inconsistent core flags: cf_alpha, cf_bravo, cf_charlie");

        f = 0;  TRY(default_flags(f, cf_alpha | cf_bravo, cf_alpha));  TEST_EQUAL(f, 1);
        f = 1;  TRY(default_flags(f, cf_alpha | cf_bravo, cf_alpha));  TEST_EQUAL(f, 1);
        f = 2;  TRY(default_flags(f, cf_alpha | cf_bravo, cf_alpha));  TEST_EQUAL(f, 2);
        f = 3;  TRY(default_flags(f, cf_alpha | cf_bravo, cf_alpha));  TEST_EQUAL(f, 3);
        f = 4;  TRY(default_flags(f, cf_alpha | cf_bravo, cf_alpha));  TEST_EQUAL(f, 5);

    }

    void check_character_types() {

        TEST(IsCharacterType<char>::value);
        TEST(IsCharacterType<char16_t>::value);
        TEST(IsCharacterType<char32_t>::value);
        TEST(IsCharacterType<wchar_t>::value);
        TEST(! IsCharacterType<signed char>::value);
        TEST(! IsCharacterType<unsigned char>::value);
        TEST(! IsCharacterType<void>::value);
        TEST(! IsCharacterType<int>::value);
        TEST(! IsCharacterType<string>::value);

    }

    void check_native_string() {

        #if defined(PRI_TARGET_UNIX)

            string s = "Hello world"_nat;
            TEST_EQUAL(s, "Hello world"s);

        #else

            wstring s = "Hello world"_nat;
            TEST_EQUAL(s, L"Hello world"s);

        #endif

    }

    void check_version_information() {

        auto v1 = unicorn_version(), v2 = unicode_version();
        std::cout << "... Unicorn version: " << v1 << "\n";
        std::cout << "... Unicode version: " << v2 << "\n";
        TEST_COMPARE(v1, >=, (Version{0,1,0}));
        TEST_COMPARE(v2, >=, (Version{8,0,0}));

    }

}

TEST_MODULE(unicorn, core) {

    check_flag_operations();
    check_character_types();
    check_native_string();
    check_version_information();

}
