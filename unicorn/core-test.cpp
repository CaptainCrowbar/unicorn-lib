#include "unicorn/core.hpp"
#include "rs-core/unit-test.hpp"
#include <iostream>
#include <string>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_core_character_types() {

    TEST(is_character_type<char>);
    TEST(is_character_type<char16_t>);
    TEST(is_character_type<char32_t>);
    TEST(is_character_type<wchar_t>);
    TEST(! is_character_type<signed char>);
    TEST(! is_character_type<unsigned char>);
    TEST(! is_character_type<void>);
    TEST(! is_character_type<int>);
    TEST(! is_character_type<std::string>);

}

void test_unicorn_core_version_information() {

    auto v1 = unicorn_version(), v2 = unicode_version();
    std::cout << "... Unicorn version: " << v1.str(3) << "\n";
    std::cout << "... Unicode version: " << v2.str(3) << "\n";
    TEST_COMPARE(v1, >=, (Version{0,1,0}));
    TEST_COMPARE(v2, >=, (Version{8,0,0}));

}
