#include "unicorn/core.hpp"
#include "prion/unit-test.hpp"
#include <string>

using namespace std::literals;
using namespace Unicorn;
using namespace Unicorn::Literals;

TEST_MAIN;

namespace {

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

    void check_version_information() {

        auto v1 = unicorn_version(), v2 = unicode_version();
        std::cout << "... Unicorn version: " << v1.str(3) << "\n";
        std::cout << "... Unicode version: " << v2.str(3) << "\n";
        TEST_COMPARE(v1, >=, (Version{0,1,0}));
        TEST_COMPARE(v2, >=, (Version{8,0,0}));

    }

}

TEST_MODULE(unicorn, core) {

    check_character_types();
    check_version_information();

}
