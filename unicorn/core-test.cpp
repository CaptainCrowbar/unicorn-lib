#include "unicorn/core.hpp"
#include "prion/unit-test.hpp"
#include <string>

using namespace std::literals;
using namespace Unicorn;
using namespace Unicorn::Literals;

TEST_MAIN;

namespace {

    void check_character_types() {

        TEST(is_character_type<char>);
        TEST(is_character_type<char16_t>);
        TEST(is_character_type<char32_t>);
        TEST(is_character_type<wchar_t>);
        TEST(! is_character_type<signed char>);
        TEST(! is_character_type<unsigned char>);
        TEST(! is_character_type<void>);
        TEST(! is_character_type<int>);
        TEST(! is_character_type<string>);

    }

    void check_version_information() {

        auto v1 = unicorn_version(), v2 = unicode_version();
        cout << "... Unicorn version: " << v1.str(3) << "\n";
        cout << "... Unicode version: " << v2.str(3) << "\n";
        TEST_COMPARE(v1, >=, (Version{0,1,0}));
        TEST_COMPARE(v2, >=, (Version{8,0,0}));

    }

}

TEST_MODULE(unicorn, core) {

    check_character_types();
    check_version_information();

}
