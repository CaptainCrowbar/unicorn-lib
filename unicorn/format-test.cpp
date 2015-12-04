#include "unicorn/core.hpp"
#include "unicorn/format.hpp"
#include "prion/unit-test.hpp"
#include <chrono>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std::chrono;
using namespace std::literals;
using namespace Unicorn;
using namespace Unicorn::Literals;

namespace {

    void check_boolean_formatting() {

        // b, fx_binary  = Binary number
        // t, fx_tf      = Write as true/false [default]
        // y, fx_yesno   = Write as yes/no

        TEST_EQUAL(format_as<char>(true), "true");
        TEST_EQUAL(format_as<char>(false), "false");
        TEST_EQUAL(format_as<char>(true, "b"), "1");
        TEST_EQUAL(format_as<char>(false, "b"), "0");
        TEST_EQUAL(format_as<char>(true, "t"), "true");
        TEST_EQUAL(format_as<char>(false, "t"), "false");
        TEST_EQUAL(format_as<char>(true, "y"), "yes");
        TEST_EQUAL(format_as<char>(false, "y"), "no");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(true), u"true");
            TEST_EQUAL(format_as<char16_t>(false), u"false");
            TEST_EQUAL(format_as<char16_t>(true, u"b"), u"1");
            TEST_EQUAL(format_as<char16_t>(false, u"b"), u"0");
            TEST_EQUAL(format_as<char16_t>(true, u"t"), u"true");
            TEST_EQUAL(format_as<char16_t>(false, u"t"), u"false");
            TEST_EQUAL(format_as<char16_t>(true, u"y"), u"yes");
            TEST_EQUAL(format_as<char16_t>(false, u"y"), u"no");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(true), U"true");
            TEST_EQUAL(format_as<char32_t>(false), U"false");
            TEST_EQUAL(format_as<char32_t>(true, U"b"), U"1");
            TEST_EQUAL(format_as<char32_t>(false, U"b"), U"0");
            TEST_EQUAL(format_as<char32_t>(true, U"t"), U"true");
            TEST_EQUAL(format_as<char32_t>(false, U"t"), U"false");
            TEST_EQUAL(format_as<char32_t>(true, U"y"), U"yes");
            TEST_EQUAL(format_as<char32_t>(false, U"y"), U"no");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(true), L"true");
            TEST_EQUAL(format_as<wchar_t>(false), L"false");
            TEST_EQUAL(format_as<wchar_t>(true, L"b"), L"1");
            TEST_EQUAL(format_as<wchar_t>(false, L"b"), L"0");
            TEST_EQUAL(format_as<wchar_t>(true, L"t"), L"true");
            TEST_EQUAL(format_as<wchar_t>(false, L"t"), L"false");
            TEST_EQUAL(format_as<wchar_t>(true, L"y"), L"yes");
            TEST_EQUAL(format_as<wchar_t>(false, L"y"), L"no");
        #endif

    }

    void check_integer_formatting() {

        // n, fx_decimal  = Decimal number [default]
        // b, fx_binary   = Binary number
        // x, fx_hex      = Hexadecimal number
        // r, fx_roman    = Roman numerals
        // s, fx_sign     = Always show a sign
        // S, fx_signz    = Always show a sign unless zero

        TEST_EQUAL(format_as<char>(0), "0");
        TEST_EQUAL(format_as<char>(42), "42");
        TEST_EQUAL(format_as<char>(-42), "-42");

        TEST_EQUAL(format_as<char>(0, "n"), "0");
        TEST_EQUAL(format_as<char>(42, "n"), "42");
        TEST_EQUAL(format_as<char>(-42, "n"), "-42");
        TEST_EQUAL(format_as<char>(0, "b"), "0");
        TEST_EQUAL(format_as<char>(42, "b"), "101010");
        TEST_EQUAL(format_as<char>(-42, "b"), "-101010");
        TEST_EQUAL(format_as<char>(0, "x"), "0");
        TEST_EQUAL(format_as<char>(42, "x"), "2a");
        TEST_EQUAL(format_as<char>(-42, "x"), "-2a");

        TEST_EQUAL(format_as<char>(0, "s"), "+0");
        TEST_EQUAL(format_as<char>(42, "s"), "+42");
        TEST_EQUAL(format_as<char>(-42, "s"), "-42");
        TEST_EQUAL(format_as<char>(0, "sb"), "+0");
        TEST_EQUAL(format_as<char>(42, "sb"), "+101010");
        TEST_EQUAL(format_as<char>(-42, "sb"), "-101010");
        TEST_EQUAL(format_as<char>(0, "sx"), "+0");
        TEST_EQUAL(format_as<char>(42, "sx"), "+2a");
        TEST_EQUAL(format_as<char>(-42, "sx"), "-2a");

        TEST_EQUAL(format_as<char>(0, "S"), "0");
        TEST_EQUAL(format_as<char>(42, "S"), "+42");
        TEST_EQUAL(format_as<char>(-42, "S"), "-42");
        TEST_EQUAL(format_as<char>(0, "Sb"), "0");
        TEST_EQUAL(format_as<char>(42, "Sb"), "+101010");
        TEST_EQUAL(format_as<char>(-42, "Sb"), "-101010");
        TEST_EQUAL(format_as<char>(0, "Sx"), "0");
        TEST_EQUAL(format_as<char>(42, "Sx"), "+2a");
        TEST_EQUAL(format_as<char>(-42, "Sx"), "-2a");

        TEST_EQUAL(format_as<char>(0, "8"), "00000000");
        TEST_EQUAL(format_as<char>(42, "8"), "00000042");
        TEST_EQUAL(format_as<char>(-42, "8"), "-00000042");
        TEST_EQUAL(format_as<char>(0, "b8"), "00000000");
        TEST_EQUAL(format_as<char>(42, "b8"), "00101010");
        TEST_EQUAL(format_as<char>(-42, "b8"), "-00101010");
        TEST_EQUAL(format_as<char>(0, "x8"), "00000000");
        TEST_EQUAL(format_as<char>(42, "x8"), "0000002a");
        TEST_EQUAL(format_as<char>(-42, "x8"), "-0000002a");

        TEST_EQUAL(format_as<char>(0, "r"), "0");
        TEST_EQUAL(format_as<char>(1, "r"), "I");
        TEST_EQUAL(format_as<char>(42, "r"), "XLII");
        TEST_EQUAL(format_as<char>(1111, "r"), "MCXI");
        TEST_EQUAL(format_as<char>(2222, "r"), "MMCCXXII");
        TEST_EQUAL(format_as<char>(3333, "r"), "MMMCCCXXXIII");
        TEST_EQUAL(format_as<char>(4444, "r"), "MMMMCDXLIV");
        TEST_EQUAL(format_as<char>(5555, "r"), "MMMMMDLV");
        TEST_EQUAL(format_as<char>(6666, "r"), "MMMMMMDCLXVI");
        TEST_EQUAL(format_as<char>(7777, "r"), "MMMMMMMDCCLXXVII");
        TEST_EQUAL(format_as<char>(8888, "r"), "MMMMMMMMDCCCLXXXVIII");
        TEST_EQUAL(format_as<char>(9999, "r"), "MMMMMMMMMCMXCIX");

        static constexpr auto max_int128_t = 0x7fffffffffffffffffffffffffffffff_s128;
        static constexpr auto max_uint128_t = 0xffffffffffffffffffffffffffffffff_u128;

        TEST_EQUAL(format_as<char>(0_s128), "0");
        TEST_EQUAL(format_as<char>(42_s128), "42");
        TEST_EQUAL(format_as<char>(-42_s128), "-42");
        TEST_EQUAL(format_as<char>(max_int128_t), "170141183460469231731687303715884105727");
        TEST_EQUAL(format_as<char>(max_int128_t, "n"), "170141183460469231731687303715884105727");
        TEST_EQUAL(format_as<char>(max_int128_t, "b"), u8string(127, '1'));
        TEST_EQUAL(format_as<char>(max_int128_t, "x"), "7fffffffffffffffffffffffffffffff");
        TEST_EQUAL(format_as<char>(max_int128_t, "40"), "0170141183460469231731687303715884105727");
        TEST_EQUAL(format_as<char>(max_int128_t, "x40"), "000000007fffffffffffffffffffffffffffffff");
        TEST_EQUAL(format_as<char>(0_u128), "0");
        TEST_EQUAL(format_as<char>(42_u128), "42");
        TEST_EQUAL(format_as<char>(max_uint128_t), "340282366920938463463374607431768211455");
        TEST_EQUAL(format_as<char>(max_uint128_t, "n"), "340282366920938463463374607431768211455");
        TEST_EQUAL(format_as<char>(max_uint128_t, "b"), u8string(128, '1'));
        TEST_EQUAL(format_as<char>(max_uint128_t, "x"), "ffffffffffffffffffffffffffffffff");
        TEST_EQUAL(format_as<char>(max_uint128_t, "40"), "0340282366920938463463374607431768211455");
        TEST_EQUAL(format_as<char>(max_uint128_t, "x40"), "00000000ffffffffffffffffffffffffffffffff");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(42), u"42");
            TEST_EQUAL(format_as<char16_t>(-42), u"-42");
            TEST_EQUAL(format_as<char16_t>(42, u"b"), u"101010");
            TEST_EQUAL(format_as<char16_t>(42, u"x"), u"2a");
            TEST_EQUAL(format_as<char16_t>(42, u"4"), u"0042");
            TEST_EQUAL(format_as<char16_t>(9999, u"r"), u"MMMMMMMMMCMXCIX");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(42), U"42");
            TEST_EQUAL(format_as<char32_t>(-42), U"-42");
            TEST_EQUAL(format_as<char32_t>(42, U"b"), U"101010");
            TEST_EQUAL(format_as<char32_t>(42, U"x"), U"2a");
            TEST_EQUAL(format_as<char32_t>(42, U"4"), U"0042");
            TEST_EQUAL(format_as<char32_t>(9999, U"r"), U"MMMMMMMMMCMXCIX");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(42), L"42");
            TEST_EQUAL(format_as<wchar_t>(-42), L"-42");
            TEST_EQUAL(format_as<wchar_t>(42, L"b"), L"101010");
            TEST_EQUAL(format_as<wchar_t>(42, L"x"), L"2a");
            TEST_EQUAL(format_as<wchar_t>(42, L"4"), L"0042");
            TEST_EQUAL(format_as<wchar_t>(9999, L"r"), L"MMMMMMMMMCMXCIX");
        #endif

    }

    void check_floating_point_formatting() {

        // d, fx_digits   = Fixed significant figures
        // e, fx_exp      = Scientific notation
        // f, fx_fixed    = Fixed point notation
        // g, fx_general  = Use the shorter of d or e [default]
        // p, fx_prob     = Probability formatting
        // s, fx_sign     = Always show a sign
        // i, fx_signz    = Always show a sign unless zero
        // z, fx_stripz   = Strip trailing zeros

        TEST_EQUAL(format_as<char>(0.0, "d"), "0.00000");
        TEST_EQUAL(format_as<char>(123.0, "d"), "123.000");
        TEST_EQUAL(format_as<char>(-123.0, "d"), "-123.000");
        TEST_EQUAL(format_as<char>(0.123, "d"), "0.123000");
        TEST_EQUAL(format_as<char>(0.0, "e"), "0.00000e0");
        TEST_EQUAL(format_as<char>(123.0, "e"), "1.23000e2");
        TEST_EQUAL(format_as<char>(-123.0, "e"), "-1.23000e2");
        TEST_EQUAL(format_as<char>(0.123, "e"), "1.23000e-1");
        TEST_EQUAL(format_as<char>(0.0, "f"), "0.000000");
        TEST_EQUAL(format_as<char>(123.0, "f"), "123.000000");
        TEST_EQUAL(format_as<char>(-123.0, "f"), "-123.000000");
        TEST_EQUAL(format_as<char>(0.123, "f"), "0.123000");
        TEST_EQUAL(format_as<char>(0.0, "g"), "0.00000");
        TEST_EQUAL(format_as<char>(123.0, "g"), "123.000");
        TEST_EQUAL(format_as<char>(-123.0, "g"), "-123.000");
        TEST_EQUAL(format_as<char>(0.123, "g"), "0.123000");
        TEST_EQUAL(format_as<char>(0.0, "d3"), "0.00");
        TEST_EQUAL(format_as<char>(0.0, "d2"), "0.0");
        TEST_EQUAL(format_as<char>(0.0, "d1"), "0");
        TEST_EQUAL(format_as<char>(0.0, "d0"), "0");
        TEST_EQUAL(format_as<char>(0.0, "dz3"), "0");
        TEST_EQUAL(format_as<char>(56789.0, "d6"), "56789.0");
        TEST_EQUAL(format_as<char>(56789.0, "d5"), "56789");
        TEST_EQUAL(format_as<char>(56789.0, "d4"), "56790");
        TEST_EQUAL(format_as<char>(56789.0, "d3"), "56800");
        TEST_EQUAL(format_as<char>(56789.0, "d2"), "57000");
        TEST_EQUAL(format_as<char>(56789.0, "d1"), "60000");
        TEST_EQUAL(format_as<char>(56789.0, "d0"), "60000");
        TEST_EQUAL(format_as<char>(56789.0, "dz6"), "56789");
        TEST_EQUAL(format_as<char>(56789.0, "dz5"), "56789");
        TEST_EQUAL(format_as<char>(0.00056789, "d3"), "0.000568");
        TEST_EQUAL(format_as<char>(0.0056789, "d3"), "0.00568");
        TEST_EQUAL(format_as<char>(0.056789, "d3"), "0.0568");
        TEST_EQUAL(format_as<char>(0.56789, "d3"), "0.568");
        TEST_EQUAL(format_as<char>(5.6789, "d3"), "5.68");
        TEST_EQUAL(format_as<char>(56.789, "d3"), "56.8");
        TEST_EQUAL(format_as<char>(567.89, "d3"), "568");
        TEST_EQUAL(format_as<char>(5678.9, "d3"), "5680");
        TEST_EQUAL(format_as<char>(56789.0, "d3"), "56800");
        TEST_EQUAL(format_as<char>(567890.0, "d3"), "568000");
        TEST_EQUAL(format_as<char>(-0.00056789, "d3"), "-0.000568");
        TEST_EQUAL(format_as<char>(-0.0056789, "d3"), "-0.00568");
        TEST_EQUAL(format_as<char>(-0.056789, "d3"), "-0.0568");
        TEST_EQUAL(format_as<char>(-0.56789, "d3"), "-0.568");
        TEST_EQUAL(format_as<char>(-5.6789, "d3"), "-5.68");
        TEST_EQUAL(format_as<char>(-56.789, "d3"), "-56.8");
        TEST_EQUAL(format_as<char>(-567.89, "d3"), "-568");
        TEST_EQUAL(format_as<char>(-5678.9, "d3"), "-5680");
        TEST_EQUAL(format_as<char>(-56789.0, "d3"), "-56800");
        TEST_EQUAL(format_as<char>(-567890.0, "d3"), "-568000");
        TEST_EQUAL(format_as<char>(0.0, "e3"), "0.00e0");
        TEST_EQUAL(format_as<char>(0.0, "e2"), "0.0e0");
        TEST_EQUAL(format_as<char>(0.0, "e1"), "0e0");
        TEST_EQUAL(format_as<char>(0.0, "e0"), "0e0");
        TEST_EQUAL(format_as<char>(0.0, "ez3"), "0e0");
        TEST_EQUAL(format_as<char>(56789.0, "e6"), "5.67890e4");
        TEST_EQUAL(format_as<char>(56789.0, "e5"), "5.6789e4");
        TEST_EQUAL(format_as<char>(56789.0, "e4"), "5.679e4");
        TEST_EQUAL(format_as<char>(56789.0, "e3"), "5.68e4");
        TEST_EQUAL(format_as<char>(56789.0, "e2"), "5.7e4");
        TEST_EQUAL(format_as<char>(56789.0, "e1"), "6e4");
        TEST_EQUAL(format_as<char>(56789.0, "e0"), "6e4");
        TEST_EQUAL(format_as<char>(56789.0, "ez6"), "5.6789e4");
        TEST_EQUAL(format_as<char>(56789.0, "ez5"), "5.6789e4");
        TEST_EQUAL(format_as<char>(0.00056789, "e3"), "5.68e-4");
        TEST_EQUAL(format_as<char>(0.0056789, "e3"), "5.68e-3");
        TEST_EQUAL(format_as<char>(0.056789, "e3"), "5.68e-2");
        TEST_EQUAL(format_as<char>(0.56789, "e3"), "5.68e-1");
        TEST_EQUAL(format_as<char>(5.6789, "e3"), "5.68e0");
        TEST_EQUAL(format_as<char>(56.789, "e3"), "5.68e1");
        TEST_EQUAL(format_as<char>(567.89, "e3"), "5.68e2");
        TEST_EQUAL(format_as<char>(5678.9, "e3"), "5.68e3");
        TEST_EQUAL(format_as<char>(56789.0, "e3"), "5.68e4");
        TEST_EQUAL(format_as<char>(567890.0, "e3"), "5.68e5");
        TEST_EQUAL(format_as<char>(-0.00056789, "e3"), "-5.68e-4");
        TEST_EQUAL(format_as<char>(-0.0056789, "e3"), "-5.68e-3");
        TEST_EQUAL(format_as<char>(-0.056789, "e3"), "-5.68e-2");
        TEST_EQUAL(format_as<char>(-0.56789, "e3"), "-5.68e-1");
        TEST_EQUAL(format_as<char>(-5.6789, "e3"), "-5.68e0");
        TEST_EQUAL(format_as<char>(-56.789, "e3"), "-5.68e1");
        TEST_EQUAL(format_as<char>(-567.89, "e3"), "-5.68e2");
        TEST_EQUAL(format_as<char>(-5678.9, "e3"), "-5.68e3");
        TEST_EQUAL(format_as<char>(-56789.0, "e3"), "-5.68e4");
        TEST_EQUAL(format_as<char>(-567890.0, "e3"), "-5.68e5");
        TEST_EQUAL(format_as<char>(0.0, "f2"), "0.00");
        TEST_EQUAL(format_as<char>(0.0, "f1"), "0.0");
        TEST_EQUAL(format_as<char>(0.0, "f0"), "0");
        TEST_EQUAL(format_as<char>(0.0, "fz2"), "0");
        TEST_EQUAL(format_as<char>(5.6789, "f5"), "5.67890");
        TEST_EQUAL(format_as<char>(5.6789, "f4"), "5.6789");
        TEST_EQUAL(format_as<char>(5.6789, "f3"), "5.679");
        TEST_EQUAL(format_as<char>(5.6789, "f2"), "5.68");
        TEST_EQUAL(format_as<char>(5.6789, "f1"), "5.7");
        TEST_EQUAL(format_as<char>(5.6789, "f0"), "6");
        TEST_EQUAL(format_as<char>(5.6789, "fz5"), "5.6789");
        TEST_EQUAL(format_as<char>(5.6789, "fz4"), "5.6789");
        TEST_EQUAL(format_as<char>(0.00056789, "f2"), "0.00");
        TEST_EQUAL(format_as<char>(0.0056789, "f2"), "0.01");
        TEST_EQUAL(format_as<char>(0.056789, "f2"), "0.06");
        TEST_EQUAL(format_as<char>(0.56789, "f2"), "0.57");
        TEST_EQUAL(format_as<char>(5.6789, "f2"), "5.68");
        TEST_EQUAL(format_as<char>(56.789, "f2"), "56.79");
        TEST_EQUAL(format_as<char>(567.89, "f2"), "567.89");
        TEST_EQUAL(format_as<char>(5678.9, "f2"), "5678.90");
        TEST_EQUAL(format_as<char>(56789.0, "f2"), "56789.00");
        TEST_EQUAL(format_as<char>(567890.0, "f2"), "567890.00");
        TEST_EQUAL(format_as<char>(-0.00056789, "f2"), "-0.00");
        TEST_EQUAL(format_as<char>(-0.0056789, "f2"), "-0.01");
        TEST_EQUAL(format_as<char>(-0.056789, "f2"), "-0.06");
        TEST_EQUAL(format_as<char>(-0.56789, "f2"), "-0.57");
        TEST_EQUAL(format_as<char>(-5.6789, "f2"), "-5.68");
        TEST_EQUAL(format_as<char>(-56.789, "f2"), "-56.79");
        TEST_EQUAL(format_as<char>(-567.89, "f2"), "-567.89");
        TEST_EQUAL(format_as<char>(-5678.9, "f2"), "-5678.90");
        TEST_EQUAL(format_as<char>(-56789.0, "f2"), "-56789.00");
        TEST_EQUAL(format_as<char>(-567890.0, "f2"), "-567890.00");
        TEST_EQUAL(format_as<char>(0.00056789, "fz2"), "0");
        TEST_EQUAL(format_as<char>(0.0056789, "fz2"), "0.01");
        TEST_EQUAL(format_as<char>(0.056789, "fz2"), "0.06");
        TEST_EQUAL(format_as<char>(0.56789, "fz2"), "0.57");
        TEST_EQUAL(format_as<char>(5.6789, "fz2"), "5.68");
        TEST_EQUAL(format_as<char>(56.789, "fz2"), "56.79");
        TEST_EQUAL(format_as<char>(567.89, "fz2"), "567.89");
        TEST_EQUAL(format_as<char>(5678.9, "fz2"), "5678.9");
        TEST_EQUAL(format_as<char>(56789.0, "fz2"), "56789");
        TEST_EQUAL(format_as<char>(567890.0, "fz2"), "567890");
        TEST_EQUAL(format_as<char>(0.0, "g3"), "0.00");
        TEST_EQUAL(format_as<char>(0.0, "g2"), "0.0");
        TEST_EQUAL(format_as<char>(0.0, "g1"), "0");
        TEST_EQUAL(format_as<char>(0.0, "g0"), "0");
        TEST_EQUAL(format_as<char>(0.0, "gz3"), "0");
        TEST_EQUAL(format_as<char>(0.0000056789, "g3"), "5.68e-6");
        TEST_EQUAL(format_as<char>(0.000056789, "g3"), "5.68e-5");
        TEST_EQUAL(format_as<char>(0.00056789, "g3"), "5.68e-4");
        TEST_EQUAL(format_as<char>(0.0056789, "g3"), "0.00568");
        TEST_EQUAL(format_as<char>(0.056789, "g3"), "0.0568");
        TEST_EQUAL(format_as<char>(0.56789, "g3"), "0.568");
        TEST_EQUAL(format_as<char>(5.6789, "g3"), "5.68");
        TEST_EQUAL(format_as<char>(56.789, "g3"), "56.8");
        TEST_EQUAL(format_as<char>(567.89, "g3"), "568");
        TEST_EQUAL(format_as<char>(5678.9, "g3"), "5680");
        TEST_EQUAL(format_as<char>(56789.0, "g3"), "56800");
        TEST_EQUAL(format_as<char>(567890.0, "g3"), "568000");
        TEST_EQUAL(format_as<char>(5678900.0, "g3"), "5.68e6");
        TEST_EQUAL(format_as<char>(56789000.0, "g3"), "5.68e7");
        TEST_EQUAL(format_as<char>(567890000.0, "g3"), "5.68e8");
        TEST_EQUAL(format_as<char>(-0.0000056789, "g3"), "-5.68e-6");
        TEST_EQUAL(format_as<char>(-0.000056789, "g3"), "-5.68e-5");
        TEST_EQUAL(format_as<char>(-0.00056789, "g3"), "-5.68e-4");
        TEST_EQUAL(format_as<char>(-0.0056789, "g3"), "-0.00568");
        TEST_EQUAL(format_as<char>(-0.056789, "g3"), "-0.0568");
        TEST_EQUAL(format_as<char>(-0.56789, "g3"), "-0.568");
        TEST_EQUAL(format_as<char>(-5.6789, "g3"), "-5.68");
        TEST_EQUAL(format_as<char>(-56.789, "g3"), "-56.8");
        TEST_EQUAL(format_as<char>(-567.89, "g3"), "-568");
        TEST_EQUAL(format_as<char>(-5678.9, "g3"), "-5680");
        TEST_EQUAL(format_as<char>(-56789.0, "g3"), "-56800");
        TEST_EQUAL(format_as<char>(-567890.0, "g3"), "-568000");
        TEST_EQUAL(format_as<char>(-5678900.0, "g3"), "-5.68e6");
        TEST_EQUAL(format_as<char>(-56789000.0, "g3"), "-5.68e7");
        TEST_EQUAL(format_as<char>(-567890000.0, "g3"), "-5.68e8");
        TEST_EQUAL(format_as<char>(-0.25, "p3"), "0");
        TEST_EQUAL(format_as<char>(0.0, "p3"), "0");
        TEST_EQUAL(format_as<char>(0.25, "p3"), "0.250");
        TEST_EQUAL(format_as<char>(0.5, "p3"), "0.500");
        TEST_EQUAL(format_as<char>(0.75, "p3"), "0.750");
        TEST_EQUAL(format_as<char>(1.0, "p3"), "1");
        TEST_EQUAL(format_as<char>(1.25, "p3"), "1");
        TEST_EQUAL(format_as<char>(0.12345, "p3"), "0.123");
        TEST_EQUAL(format_as<char>(0.012345, "p3"), "0.0123");
        TEST_EQUAL(format_as<char>(0.0012345, "p3"), "0.00123");
        TEST_EQUAL(format_as<char>(0.00012345, "p3"), "0.000123");
        TEST_EQUAL(format_as<char>(0.000012345, "p3"), "0.0000123");
        TEST_EQUAL(format_as<char>(0.87654, "p3"), "0.877");
        TEST_EQUAL(format_as<char>(0.987654, "p3"), "0.9877");
        TEST_EQUAL(format_as<char>(0.9987654, "p3"), "0.99877");
        TEST_EQUAL(format_as<char>(0.99987654, "p3"), "0.999877");
        TEST_EQUAL(format_as<char>(0.999987654, "p3"), "0.9999877");
        TEST_EQUAL(format_as<char>(0.0, "sd3"), "+0.00");
        TEST_EQUAL(format_as<char>(42.0, "sd3"), "+42.0");
        TEST_EQUAL(format_as<char>(-42.0, "sd3"), "-42.0");
        TEST_EQUAL(format_as<char>(0.0, "Sd3"), "0.00");
        TEST_EQUAL(format_as<char>(42.0, "Sd3"), "+42.0");
        TEST_EQUAL(format_as<char>(-42.0, "Sd3"), "-42.0");
        TEST_EQUAL(format_as<char>(0.0, "se3"), "+0.00e0");
        TEST_EQUAL(format_as<char>(42.0, "se3"), "+4.20e1");
        TEST_EQUAL(format_as<char>(-42.0, "se3"), "-4.20e1");
        TEST_EQUAL(format_as<char>(0.0, "Se3"), "0.00e0");
        TEST_EQUAL(format_as<char>(42.0, "Se3"), "+4.20e1");
        TEST_EQUAL(format_as<char>(-42.0, "Se3"), "-4.20e1");
        TEST_EQUAL(format_as<char>(0.0, "sf3"), "+0.000");
        TEST_EQUAL(format_as<char>(42.0, "sf3"), "+42.000");
        TEST_EQUAL(format_as<char>(-42.0, "sf3"), "-42.000");
        TEST_EQUAL(format_as<char>(0.0, "Sf3"), "0.000");
        TEST_EQUAL(format_as<char>(42.0, "Sf3"), "+42.000");
        TEST_EQUAL(format_as<char>(-42.0, "Sf3"), "-42.000");
        TEST_EQUAL(format_as<char>(0.0, "sg3"), "+0.00");
        TEST_EQUAL(format_as<char>(42.0, "sg3"), "+42.0");
        TEST_EQUAL(format_as<char>(-42.0, "sg3"), "-42.0");
        TEST_EQUAL(format_as<char>(0.0, "Sg3"), "0.00");
        TEST_EQUAL(format_as<char>(42.0, "Sg3"), "+42.0");
        TEST_EQUAL(format_as<char>(-42.0, "Sg3"), "-42.0");
        TEST_EQUAL(format_as<char>(0.0, "sp3"), "+0");
        TEST_EQUAL(format_as<char>(0.42, "sp3"), "+0.420");
        TEST_EQUAL(format_as<char>(-0.42, "sp3"), "+0");
        TEST_EQUAL(format_as<char>(0.0, "Sp3"), "0");
        TEST_EQUAL(format_as<char>(0.42, "Sp3"), "+0.420");
        TEST_EQUAL(format_as<char>(-0.42, "Sp3"), "0");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(0.0, u"d3"), u"0.00");
            TEST_EQUAL(format_as<char16_t>(42.0, u"d3"), u"42.0");
            TEST_EQUAL(format_as<char16_t>(-42.0, u"d3"), u"-42.0");
            TEST_EQUAL(format_as<char16_t>(0.0, u"e3"), u"0.00e0");
            TEST_EQUAL(format_as<char16_t>(42.0, u"e3"), u"4.20e1");
            TEST_EQUAL(format_as<char16_t>(-42.0, u"e3"), u"-4.20e1");
            TEST_EQUAL(format_as<char16_t>(0.0, u"f3"), u"0.000");
            TEST_EQUAL(format_as<char16_t>(42.0, u"f3"), u"42.000");
            TEST_EQUAL(format_as<char16_t>(-42.0, u"f3"), u"-42.000");
            TEST_EQUAL(format_as<char16_t>(0.0, u"g3"), u"0.00");
            TEST_EQUAL(format_as<char16_t>(42.0, u"g3"), u"42.0");
            TEST_EQUAL(format_as<char16_t>(-42.0, u"g3"), u"-42.0");
            TEST_EQUAL(format_as<char16_t>(0.0, u"p3"), u"0");
            TEST_EQUAL(format_as<char16_t>(0.42, u"p3"), u"0.420");
            TEST_EQUAL(format_as<char16_t>(-0.42, u"p3"), u"0");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(0.0, U"d3"), U"0.00");
            TEST_EQUAL(format_as<char32_t>(42.0, U"d3"), U"42.0");
            TEST_EQUAL(format_as<char32_t>(-42.0, U"d3"), U"-42.0");
            TEST_EQUAL(format_as<char32_t>(0.0, U"e3"), U"0.00e0");
            TEST_EQUAL(format_as<char32_t>(42.0, U"e3"), U"4.20e1");
            TEST_EQUAL(format_as<char32_t>(-42.0, U"e3"), U"-4.20e1");
            TEST_EQUAL(format_as<char32_t>(0.0, U"f3"), U"0.000");
            TEST_EQUAL(format_as<char32_t>(42.0, U"f3"), U"42.000");
            TEST_EQUAL(format_as<char32_t>(-42.0, U"f3"), U"-42.000");
            TEST_EQUAL(format_as<char32_t>(0.0, U"g3"), U"0.00");
            TEST_EQUAL(format_as<char32_t>(42.0, U"g3"), U"42.0");
            TEST_EQUAL(format_as<char32_t>(-42.0, U"g3"), U"-42.0");
            TEST_EQUAL(format_as<char32_t>(0.0, U"p3"), U"0");
            TEST_EQUAL(format_as<char32_t>(0.42, U"p3"), U"0.420");
            TEST_EQUAL(format_as<char32_t>(-0.42, U"p3"), U"0");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(0.0, L"d3"), L"0.00");
            TEST_EQUAL(format_as<wchar_t>(42.0, L"d3"), L"42.0");
            TEST_EQUAL(format_as<wchar_t>(-42.0, L"d3"), L"-42.0");
            TEST_EQUAL(format_as<wchar_t>(0.0, L"e3"), L"0.00e0");
            TEST_EQUAL(format_as<wchar_t>(42.0, L"e3"), L"4.20e1");
            TEST_EQUAL(format_as<wchar_t>(-42.0, L"e3"), L"-4.20e1");
            TEST_EQUAL(format_as<wchar_t>(0.0, L"f3"), L"0.000");
            TEST_EQUAL(format_as<wchar_t>(42.0, L"f3"), L"42.000");
            TEST_EQUAL(format_as<wchar_t>(-42.0, L"f3"), L"-42.000");
            TEST_EQUAL(format_as<wchar_t>(0.0, L"g3"), L"0.00");
            TEST_EQUAL(format_as<wchar_t>(42.0, L"g3"), L"42.0");
            TEST_EQUAL(format_as<wchar_t>(-42.0, L"g3"), L"-42.0");
            TEST_EQUAL(format_as<wchar_t>(0.0, L"p3"), L"0");
            TEST_EQUAL(format_as<wchar_t>(0.42, L"p3"), L"0.420");
            TEST_EQUAL(format_as<wchar_t>(-0.42, L"p3"), L"0");
        #endif

    }

    void check_character_formatting() {

        // a, fx_ascii     = Escape if not printable ASCII
        // c, fx_escape    = Escape if C0/C1 control
        // n, fx_decimal   = Decimal number
        // q, fx_quote     = Quote string, escape C0/C1
        // o, fx_ascquote  = Quote string, escape non-ASCII
        // x, fx_hex       = Hexadecimal number
        // u, fx_hex8      = Hex UTF-8 bytes
        // v, fx_hex16     = Hex UTF-16 code units

        TEST_EQUAL(format_as<char>('\t'), "\t");
        TEST_EQUAL(format_as<char>('\t', "a"), "\\t");
        TEST_EQUAL(format_as<char>('\t', "c"), "\\t");
        TEST_EQUAL(format_as<char>('\t', "q"), "\"\\t\"");
        TEST_EQUAL(format_as<char>('\t', "o"), "\"\\t\"");
        TEST_EQUAL(format_as<char>('\t', "n"), "9");
        TEST_EQUAL(format_as<char>('\t', "u"), "09");
        TEST_EQUAL(format_as<char>('\t', "v"), "0009");
        TEST_EQUAL(format_as<char>('\t', "x"), "9");

        TEST_EQUAL(format_as<char>('A'), "A");
        TEST_EQUAL(format_as<char>('A', "a"), "A");
        TEST_EQUAL(format_as<char>('A', "c"), "A");
        TEST_EQUAL(format_as<char>('A', "q"), "\"A\"");
        TEST_EQUAL(format_as<char>('A', "o"), "\"A\"");
        TEST_EQUAL(format_as<char>('A', "n"), "65");
        TEST_EQUAL(format_as<char>('A', "u"), "41");
        TEST_EQUAL(format_as<char>('A', "v"), "0041");
        TEST_EQUAL(format_as<char>('A', "x"), "41");

        TEST_EQUAL(format_as<char>(u'\t'), "\t");
        TEST_EQUAL(format_as<char>(u'\t', "a"), "\\t");
        TEST_EQUAL(format_as<char>(u'\t', "c"), "\\t");
        TEST_EQUAL(format_as<char>(u'\t', "q"), "\"\\t\"");
        TEST_EQUAL(format_as<char>(u'\t', "o"), "\"\\t\"");
        TEST_EQUAL(format_as<char>(u'\t', "n"), "9");
        TEST_EQUAL(format_as<char>(u'\t', "u"), "09");
        TEST_EQUAL(format_as<char>(u'\t', "v"), "0009");
        TEST_EQUAL(format_as<char>(u'\t', "x"), "9");

        TEST_EQUAL(format_as<char>(u'A'), "A");
        TEST_EQUAL(format_as<char>(u'A', "a"), "A");
        TEST_EQUAL(format_as<char>(u'A', "c"), "A");
        TEST_EQUAL(format_as<char>(u'A', "q"), "\"A\"");
        TEST_EQUAL(format_as<char>(u'A', "o"), "\"A\"");
        TEST_EQUAL(format_as<char>(u'A', "n"), "65");
        TEST_EQUAL(format_as<char>(u'A', "u"), "41");
        TEST_EQUAL(format_as<char>(u'A', "v"), "0041");
        TEST_EQUAL(format_as<char>(u'A', "x"), "41");

        TEST_EQUAL(format_as<char>(u'€'), u8"€");
        TEST_EQUAL(format_as<char>(u'€', "a"), "\\x{20ac}");
        TEST_EQUAL(format_as<char>(u'€', "c"), u8"€");
        TEST_EQUAL(format_as<char>(u'€', "q"), u8"\"€\"");
        TEST_EQUAL(format_as<char>(u'€', "o"), "\"\\x{20ac}\"");
        TEST_EQUAL(format_as<char>(u'€', "n"), "8364");
        TEST_EQUAL(format_as<char>(u'€', "u"), "e2 82 ac");
        TEST_EQUAL(format_as<char>(u'€', "v"), "20ac");
        TEST_EQUAL(format_as<char>(u'€', "x"), "20ac");

        TEST_EQUAL(format_as<char>(U'\t'), "\t");
        TEST_EQUAL(format_as<char>(U'\t', "a"), "\\t");
        TEST_EQUAL(format_as<char>(U'\t', "c"), "\\t");
        TEST_EQUAL(format_as<char>(U'\t', "q"), "\"\\t\"");
        TEST_EQUAL(format_as<char>(U'\t', "o"), "\"\\t\"");
        TEST_EQUAL(format_as<char>(U'\t', "n"), "9");
        TEST_EQUAL(format_as<char>(U'\t', "u"), "09");
        TEST_EQUAL(format_as<char>(U'\t', "v"), "0009");
        TEST_EQUAL(format_as<char>(U'\t', "x"), "9");

        TEST_EQUAL(format_as<char>(U'A'), "A");
        TEST_EQUAL(format_as<char>(U'A', "a"), "A");
        TEST_EQUAL(format_as<char>(U'A', "c"), "A");
        TEST_EQUAL(format_as<char>(U'A', "q"), "\"A\"");
        TEST_EQUAL(format_as<char>(U'A', "o"), "\"A\"");
        TEST_EQUAL(format_as<char>(U'A', "n"), "65");
        TEST_EQUAL(format_as<char>(U'A', "u"), "41");
        TEST_EQUAL(format_as<char>(U'A', "v"), "0041");
        TEST_EQUAL(format_as<char>(U'A', "x"), "41");

        TEST_EQUAL(format_as<char>(U'€'), u8"€");
        TEST_EQUAL(format_as<char>(U'€', "a"), "\\x{20ac}");
        TEST_EQUAL(format_as<char>(U'€', "c"), u8"€");
        TEST_EQUAL(format_as<char>(U'€', "q"), u8"\"€\"");
        TEST_EQUAL(format_as<char>(U'€', "o"), "\"\\x{20ac}\"");
        TEST_EQUAL(format_as<char>(U'€', "n"), "8364");
        TEST_EQUAL(format_as<char>(U'€', "u"), "e2 82 ac");
        TEST_EQUAL(format_as<char>(U'€', "v"), "20ac");
        TEST_EQUAL(format_as<char>(U'€', "x"), "20ac");

        TEST_EQUAL(format_as<char>(char32_t(0x10fffd)), "\xf4\x8f\xbf\xbd");
        TEST_EQUAL(format_as<char>(char32_t(0x10fffd), "a"), "\\x{10fffd}");
        TEST_EQUAL(format_as<char>(char32_t(0x10fffd), "c"), "\xf4\x8f\xbf\xbd");
        TEST_EQUAL(format_as<char>(char32_t(0x10fffd), "q"), "\"\xf4\x8f\xbf\xbd\"");
        TEST_EQUAL(format_as<char>(char32_t(0x10fffd), "o"), "\"\\x{10fffd}\"");
        TEST_EQUAL(format_as<char>(char32_t(0x10fffd), "n"), "1114109");
        TEST_EQUAL(format_as<char>(char32_t(0x10fffd), "u"), "f4 8f bf bd");
        TEST_EQUAL(format_as<char>(char32_t(0x10fffd), "v"), "dbff dffd");
        TEST_EQUAL(format_as<char>(char32_t(0x10fffd), "x"), "10fffd");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>('A'), u"A");
            TEST_EQUAL(format_as<char16_t>('A', u"a"), u"A");
            TEST_EQUAL(format_as<char16_t>('A', u"c"), u"A");
            TEST_EQUAL(format_as<char16_t>('A', u"q"), u"\"A\"");
            TEST_EQUAL(format_as<char16_t>('A', u"o"), u"\"A\"");
            TEST_EQUAL(format_as<char16_t>('A', u"n"), u"65");
            TEST_EQUAL(format_as<char16_t>('A', u"u"), u"41");
            TEST_EQUAL(format_as<char16_t>('A', u"v"), u"0041");
            TEST_EQUAL(format_as<char16_t>('A', u"x"), u"41");
            TEST_EQUAL(format_as<char16_t>(u'€'), u"€");
            TEST_EQUAL(format_as<char16_t>(u'€', u"a"), u"\\x{20ac}");
            TEST_EQUAL(format_as<char16_t>(u'€', u"c"), u"€");
            TEST_EQUAL(format_as<char16_t>(u'€', u"q"), u"\"€\"");
            TEST_EQUAL(format_as<char16_t>(u'€', u"o"), u"\"\\x{20ac}\"");
            TEST_EQUAL(format_as<char16_t>(u'€', u"n"), u"8364");
            TEST_EQUAL(format_as<char16_t>(u'€', u"u"), u"e2 82 ac");
            TEST_EQUAL(format_as<char16_t>(u'€', u"v"), u"20ac");
            TEST_EQUAL(format_as<char16_t>(u'€', u"x"), u"20ac");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>('A'), U"A");
            TEST_EQUAL(format_as<char32_t>('A', U"a"), U"A");
            TEST_EQUAL(format_as<char32_t>('A', U"c"), U"A");
            TEST_EQUAL(format_as<char32_t>('A', U"q"), U"\"A\"");
            TEST_EQUAL(format_as<char32_t>('A', U"o"), U"\"A\"");
            TEST_EQUAL(format_as<char32_t>('A', U"n"), U"65");
            TEST_EQUAL(format_as<char32_t>('A', U"u"), U"41");
            TEST_EQUAL(format_as<char32_t>('A', U"v"), U"0041");
            TEST_EQUAL(format_as<char32_t>('A', U"x"), U"41");
            TEST_EQUAL(format_as<char32_t>(u'€'), U"€");
            TEST_EQUAL(format_as<char32_t>(u'€', U"a"), U"\\x{20ac}");
            TEST_EQUAL(format_as<char32_t>(u'€', U"c"), U"€");
            TEST_EQUAL(format_as<char32_t>(u'€', U"q"), U"\"€\"");
            TEST_EQUAL(format_as<char32_t>(u'€', U"o"), U"\"\\x{20ac}\"");
            TEST_EQUAL(format_as<char32_t>(u'€', U"n"), U"8364");
            TEST_EQUAL(format_as<char32_t>(u'€', U"u"), U"e2 82 ac");
            TEST_EQUAL(format_as<char32_t>(u'€', U"v"), U"20ac");
            TEST_EQUAL(format_as<char32_t>(u'€', U"x"), U"20ac");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>('A'), L"A");
            TEST_EQUAL(format_as<wchar_t>('A', L"a"), L"A");
            TEST_EQUAL(format_as<wchar_t>('A', L"c"), L"A");
            TEST_EQUAL(format_as<wchar_t>('A', L"q"), L"\"A\"");
            TEST_EQUAL(format_as<wchar_t>('A', L"o"), L"\"A\"");
            TEST_EQUAL(format_as<wchar_t>('A', L"n"), L"65");
            TEST_EQUAL(format_as<wchar_t>('A', L"u"), L"41");
            TEST_EQUAL(format_as<wchar_t>('A', L"v"), L"0041");
            TEST_EQUAL(format_as<wchar_t>('A', L"x"), L"41");
            TEST_EQUAL(format_as<wchar_t>(u'€'), L"€");
            TEST_EQUAL(format_as<wchar_t>(u'€', L"a"), L"\\x{20ac}");
            TEST_EQUAL(format_as<wchar_t>(u'€', L"c"), L"€");
            TEST_EQUAL(format_as<wchar_t>(u'€', L"q"), L"\"€\"");
            TEST_EQUAL(format_as<wchar_t>(u'€', L"o"), L"\"\\x{20ac}\"");
            TEST_EQUAL(format_as<wchar_t>(u'€', L"n"), L"8364");
            TEST_EQUAL(format_as<wchar_t>(u'€', L"u"), L"e2 82 ac");
            TEST_EQUAL(format_as<wchar_t>(u'€', L"v"), L"20ac");
            TEST_EQUAL(format_as<wchar_t>(u'€', L"x"), L"20ac");
        #endif

    }

    void check_string_formatting() {

        // a, fx_ascii     = Escape if not printable ASCII
        // c, fx_escape    = Escape if C0/C1 control
        // d, fx_decimal   = Decimal number
        // q, fx_quote     = Quote string, escape C0/C1
        // o, fx_ascquote  = Quote string, escape non-ASCII
        // x, fx_hex       = Hexadecimal number
        // u, fx_hex8      = Hex UTF-8 bytes
        // v, fx_hex16     = Hex UTF-16 code units

        TEST_EQUAL(format_as<char>(""s), "");
        TEST_EQUAL(format_as<char>(""s, "a"), "");
        TEST_EQUAL(format_as<char>(""s, "c"), "");
        TEST_EQUAL(format_as<char>(""s, "q"), "\"\"");
        TEST_EQUAL(format_as<char>(""s, "o"), "\"\"");
        TEST_EQUAL(format_as<char>(""s, "n"), "");
        TEST_EQUAL(format_as<char>(""s, "u"), "");
        TEST_EQUAL(format_as<char>(""s, "v"), "");
        TEST_EQUAL(format_as<char>(""s, "x"), "");

        TEST_EQUAL(format_as<char>("Hello"s), "Hello");
        TEST_EQUAL(format_as<char>("Hello"s, "a"), "Hello");
        TEST_EQUAL(format_as<char>("Hello"s, "c"), "Hello");
        TEST_EQUAL(format_as<char>("Hello"s, "q"), "\"Hello\"");
        TEST_EQUAL(format_as<char>("Hello"s, "o"), "\"Hello\"");
        TEST_EQUAL(format_as<char>("Hello"s, "n"), "72 101 108 108 111");
        TEST_EQUAL(format_as<char>("Hello"s, "u"), "48 65 6c 6c 6f");
        TEST_EQUAL(format_as<char>("Hello"s, "v"), "0048 0065 006c 006c 006f");
        TEST_EQUAL(format_as<char>("Hello"s, "x"), "48 65 6c 6c 6f");

        TEST_EQUAL(format_as<char>(u8"§€ αβγ\n"s), u8"§€ αβγ\n");
        TEST_EQUAL(format_as<char>(u8"§€ αβγ\n"s, "a"), "\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
        TEST_EQUAL(format_as<char>(u8"§€ αβγ\n"s, "c"), u8"§€ αβγ\\n");
        TEST_EQUAL(format_as<char>(u8"§€ αβγ\n"s, "q"), u8"\"§€ αβγ\\n\"");
        TEST_EQUAL(format_as<char>(u8"§€ αβγ\n"s, "o"), "\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
        TEST_EQUAL(format_as<char>(u8"§€ αβγ\n"s, "n"), "167 8364 32 945 946 947 10");
        TEST_EQUAL(format_as<char>(u8"§€ αβγ\n"s, "u"), "c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
        TEST_EQUAL(format_as<char>(u8"§€ αβγ\n"s, "v"), "00a7 20ac 0020 03b1 03b2 03b3 000a");
        TEST_EQUAL(format_as<char>(u8"§€ αβγ\n"s, "x"), "a7 20ac 20 3b1 3b2 3b3 a");

        TEST_EQUAL(format_as<char>(u""s), "");
        TEST_EQUAL(format_as<char>(u""s, "a"), "");
        TEST_EQUAL(format_as<char>(u""s, "c"), "");
        TEST_EQUAL(format_as<char>(u""s, "q"), "\"\"");
        TEST_EQUAL(format_as<char>(u""s, "o"), "\"\"");
        TEST_EQUAL(format_as<char>(u""s, "n"), "");
        TEST_EQUAL(format_as<char>(u""s, "u"), "");
        TEST_EQUAL(format_as<char>(u""s, "v"), "");
        TEST_EQUAL(format_as<char>(u""s, "x"), "");

        TEST_EQUAL(format_as<char>(u"Hello"s), "Hello");
        TEST_EQUAL(format_as<char>(u"Hello"s, "a"), "Hello");
        TEST_EQUAL(format_as<char>(u"Hello"s, "c"), "Hello");
        TEST_EQUAL(format_as<char>(u"Hello"s, "q"), "\"Hello\"");
        TEST_EQUAL(format_as<char>(u"Hello"s, "o"), "\"Hello\"");
        TEST_EQUAL(format_as<char>(u"Hello"s, "n"), "72 101 108 108 111");
        TEST_EQUAL(format_as<char>(u"Hello"s, "u"), "48 65 6c 6c 6f");
        TEST_EQUAL(format_as<char>(u"Hello"s, "v"), "0048 0065 006c 006c 006f");
        TEST_EQUAL(format_as<char>(u"Hello"s, "x"), "48 65 6c 6c 6f");

        TEST_EQUAL(format_as<char>(u"§€ αβγ\n"s), u8"§€ αβγ\n");
        TEST_EQUAL(format_as<char>(u"§€ αβγ\n"s, "a"), "\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
        TEST_EQUAL(format_as<char>(u"§€ αβγ\n"s, "c"), u8"§€ αβγ\\n");
        TEST_EQUAL(format_as<char>(u"§€ αβγ\n"s, "q"), u8"\"§€ αβγ\\n\"");
        TEST_EQUAL(format_as<char>(u"§€ αβγ\n"s, "o"), "\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
        TEST_EQUAL(format_as<char>(u"§€ αβγ\n"s, "n"), "167 8364 32 945 946 947 10");
        TEST_EQUAL(format_as<char>(u"§€ αβγ\n"s, "u"), "c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
        TEST_EQUAL(format_as<char>(u"§€ αβγ\n"s, "v"), "00a7 20ac 0020 03b1 03b2 03b3 000a");
        TEST_EQUAL(format_as<char>(u"§€ αβγ\n"s, "x"), "a7 20ac 20 3b1 3b2 3b3 a");

        TEST_EQUAL(format_as<char>(U""s), "");
        TEST_EQUAL(format_as<char>(U""s, "a"), "");
        TEST_EQUAL(format_as<char>(U""s, "c"), "");
        TEST_EQUAL(format_as<char>(U""s, "q"), "\"\"");
        TEST_EQUAL(format_as<char>(U""s, "o"), "\"\"");
        TEST_EQUAL(format_as<char>(U""s, "n"), "");
        TEST_EQUAL(format_as<char>(U""s, "u"), "");
        TEST_EQUAL(format_as<char>(U""s, "v"), "");
        TEST_EQUAL(format_as<char>(U""s, "x"), "");

        TEST_EQUAL(format_as<char>(U"Hello"s), "Hello");
        TEST_EQUAL(format_as<char>(U"Hello"s, "a"), "Hello");
        TEST_EQUAL(format_as<char>(U"Hello"s, "c"), "Hello");
        TEST_EQUAL(format_as<char>(U"Hello"s, "q"), "\"Hello\"");
        TEST_EQUAL(format_as<char>(U"Hello"s, "o"), "\"Hello\"");
        TEST_EQUAL(format_as<char>(U"Hello"s, "n"), "72 101 108 108 111");
        TEST_EQUAL(format_as<char>(U"Hello"s, "u"), "48 65 6c 6c 6f");
        TEST_EQUAL(format_as<char>(U"Hello"s, "v"), "0048 0065 006c 006c 006f");
        TEST_EQUAL(format_as<char>(U"Hello"s, "x"), "48 65 6c 6c 6f");

        TEST_EQUAL(format_as<char>(U"§€ αβγ\n"s), u8"§€ αβγ\n");
        TEST_EQUAL(format_as<char>(U"§€ αβγ\n"s, "a"), "\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
        TEST_EQUAL(format_as<char>(U"§€ αβγ\n"s, "c"), u8"§€ αβγ\\n");
        TEST_EQUAL(format_as<char>(U"§€ αβγ\n"s, "q"), u8"\"§€ αβγ\\n\"");
        TEST_EQUAL(format_as<char>(U"§€ αβγ\n"s, "o"), "\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
        TEST_EQUAL(format_as<char>(U"§€ αβγ\n"s, "n"), "167 8364 32 945 946 947 10");
        TEST_EQUAL(format_as<char>(U"§€ αβγ\n"s, "u"), "c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
        TEST_EQUAL(format_as<char>(U"§€ αβγ\n"s, "v"), "00a7 20ac 0020 03b1 03b2 03b3 000a");
        TEST_EQUAL(format_as<char>(U"§€ αβγ\n"s, "x"), "a7 20ac 20 3b1 3b2 3b3 a");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(u"§€ αβγ\n"s), u"§€ αβγ\n");
            TEST_EQUAL(format_as<char16_t>(u"§€ αβγ\n"s, u"a"), u"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
            TEST_EQUAL(format_as<char16_t>(u"§€ αβγ\n"s, u"c"), u"§€ αβγ\\n");
            TEST_EQUAL(format_as<char16_t>(u"§€ αβγ\n"s, u"q"), u"\"§€ αβγ\\n\"");
            TEST_EQUAL(format_as<char16_t>(u"§€ αβγ\n"s, u"o"), u"\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
            TEST_EQUAL(format_as<char16_t>(u"§€ αβγ\n"s, u"n"), u"167 8364 32 945 946 947 10");
            TEST_EQUAL(format_as<char16_t>(u"§€ αβγ\n"s, u"u"), u"c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
            TEST_EQUAL(format_as<char16_t>(u"§€ αβγ\n"s, u"v"), u"00a7 20ac 0020 03b1 03b2 03b3 000a");
            TEST_EQUAL(format_as<char16_t>(u"§€ αβγ\n"s, u"x"), u"a7 20ac 20 3b1 3b2 3b3 a");
            TEST_EQUAL(format_as<char16_t>(U"§€ αβγ\n"s), u"§€ αβγ\n");
            TEST_EQUAL(format_as<char16_t>(U"§€ αβγ\n"s, u"a"), u"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
            TEST_EQUAL(format_as<char16_t>(U"§€ αβγ\n"s, u"c"), u"§€ αβγ\\n");
            TEST_EQUAL(format_as<char16_t>(U"§€ αβγ\n"s, u"q"), u"\"§€ αβγ\\n\"");
            TEST_EQUAL(format_as<char16_t>(U"§€ αβγ\n"s, u"o"), u"\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
            TEST_EQUAL(format_as<char16_t>(U"§€ αβγ\n"s, u"n"), u"167 8364 32 945 946 947 10");
            TEST_EQUAL(format_as<char16_t>(U"§€ αβγ\n"s, u"u"), u"c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
            TEST_EQUAL(format_as<char16_t>(U"§€ αβγ\n"s, u"v"), u"00a7 20ac 0020 03b1 03b2 03b3 000a");
            TEST_EQUAL(format_as<char16_t>(U"§€ αβγ\n"s, u"x"), u"a7 20ac 20 3b1 3b2 3b3 a");
            TEST_EQUAL(format_as<char16_t>(L"§€ αβγ\n"s), u"§€ αβγ\n");
            TEST_EQUAL(format_as<char16_t>(L"§€ αβγ\n"s, u"a"), u"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
            TEST_EQUAL(format_as<char16_t>(L"§€ αβγ\n"s, u"c"), u"§€ αβγ\\n");
            TEST_EQUAL(format_as<char16_t>(L"§€ αβγ\n"s, u"q"), u"\"§€ αβγ\\n\"");
            TEST_EQUAL(format_as<char16_t>(L"§€ αβγ\n"s, u"o"), u"\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
            TEST_EQUAL(format_as<char16_t>(L"§€ αβγ\n"s, u"n"), u"167 8364 32 945 946 947 10");
            TEST_EQUAL(format_as<char16_t>(L"§€ αβγ\n"s, u"u"), u"c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
            TEST_EQUAL(format_as<char16_t>(L"§€ αβγ\n"s, u"v"), u"00a7 20ac 0020 03b1 03b2 03b3 000a");
            TEST_EQUAL(format_as<char16_t>(L"§€ αβγ\n"s, u"x"), u"a7 20ac 20 3b1 3b2 3b3 a");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(u"§€ αβγ\n"s), U"§€ αβγ\n");
            TEST_EQUAL(format_as<char32_t>(u"§€ αβγ\n"s, U"a"), U"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
            TEST_EQUAL(format_as<char32_t>(u"§€ αβγ\n"s, U"c"), U"§€ αβγ\\n");
            TEST_EQUAL(format_as<char32_t>(u"§€ αβγ\n"s, U"q"), U"\"§€ αβγ\\n\"");
            TEST_EQUAL(format_as<char32_t>(u"§€ αβγ\n"s, U"o"), U"\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
            TEST_EQUAL(format_as<char32_t>(u"§€ αβγ\n"s, U"n"), U"167 8364 32 945 946 947 10");
            TEST_EQUAL(format_as<char32_t>(u"§€ αβγ\n"s, U"u"), U"c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
            TEST_EQUAL(format_as<char32_t>(u"§€ αβγ\n"s, U"v"), U"00a7 20ac 0020 03b1 03b2 03b3 000a");
            TEST_EQUAL(format_as<char32_t>(u"§€ αβγ\n"s, U"x"), U"a7 20ac 20 3b1 3b2 3b3 a");
            TEST_EQUAL(format_as<char32_t>(U"§€ αβγ\n"s), U"§€ αβγ\n");
            TEST_EQUAL(format_as<char32_t>(U"§€ αβγ\n"s, U"a"), U"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
            TEST_EQUAL(format_as<char32_t>(U"§€ αβγ\n"s, U"c"), U"§€ αβγ\\n");
            TEST_EQUAL(format_as<char32_t>(U"§€ αβγ\n"s, U"q"), U"\"§€ αβγ\\n\"");
            TEST_EQUAL(format_as<char32_t>(U"§€ αβγ\n"s, U"o"), U"\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
            TEST_EQUAL(format_as<char32_t>(U"§€ αβγ\n"s, U"n"), U"167 8364 32 945 946 947 10");
            TEST_EQUAL(format_as<char32_t>(U"§€ αβγ\n"s, U"u"), U"c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
            TEST_EQUAL(format_as<char32_t>(U"§€ αβγ\n"s, U"v"), U"00a7 20ac 0020 03b1 03b2 03b3 000a");
            TEST_EQUAL(format_as<char32_t>(U"§€ αβγ\n"s, U"x"), U"a7 20ac 20 3b1 3b2 3b3 a");
            TEST_EQUAL(format_as<char32_t>(L"§€ αβγ\n"s), U"§€ αβγ\n");
            TEST_EQUAL(format_as<char32_t>(L"§€ αβγ\n"s, U"a"), U"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
            TEST_EQUAL(format_as<char32_t>(L"§€ αβγ\n"s, U"c"), U"§€ αβγ\\n");
            TEST_EQUAL(format_as<char32_t>(L"§€ αβγ\n"s, U"q"), U"\"§€ αβγ\\n\"");
            TEST_EQUAL(format_as<char32_t>(L"§€ αβγ\n"s, U"o"), U"\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
            TEST_EQUAL(format_as<char32_t>(L"§€ αβγ\n"s, U"n"), U"167 8364 32 945 946 947 10");
            TEST_EQUAL(format_as<char32_t>(L"§€ αβγ\n"s, U"u"), U"c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
            TEST_EQUAL(format_as<char32_t>(L"§€ αβγ\n"s, U"v"), U"00a7 20ac 0020 03b1 03b2 03b3 000a");
            TEST_EQUAL(format_as<char32_t>(L"§€ αβγ\n"s, U"x"), U"a7 20ac 20 3b1 3b2 3b3 a");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(u"§€ αβγ\n"s), L"§€ αβγ\n");
            TEST_EQUAL(format_as<wchar_t>(u"§€ αβγ\n"s, L"a"), L"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
            TEST_EQUAL(format_as<wchar_t>(u"§€ αβγ\n"s, L"c"), L"§€ αβγ\\n");
            TEST_EQUAL(format_as<wchar_t>(u"§€ αβγ\n"s, L"q"), L"\"§€ αβγ\\n\"");
            TEST_EQUAL(format_as<wchar_t>(u"§€ αβγ\n"s, L"o"), L"\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
            TEST_EQUAL(format_as<wchar_t>(u"§€ αβγ\n"s, L"n"), L"167 8364 32 945 946 947 10");
            TEST_EQUAL(format_as<wchar_t>(u"§€ αβγ\n"s, L"u"), L"c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
            TEST_EQUAL(format_as<wchar_t>(u"§€ αβγ\n"s, L"v"), L"00a7 20ac 0020 03b1 03b2 03b3 000a");
            TEST_EQUAL(format_as<wchar_t>(u"§€ αβγ\n"s, L"x"), L"a7 20ac 20 3b1 3b2 3b3 a");
            TEST_EQUAL(format_as<wchar_t>(U"§€ αβγ\n"s), L"§€ αβγ\n");
            TEST_EQUAL(format_as<wchar_t>(U"§€ αβγ\n"s, L"a"), L"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
            TEST_EQUAL(format_as<wchar_t>(U"§€ αβγ\n"s, L"c"), L"§€ αβγ\\n");
            TEST_EQUAL(format_as<wchar_t>(U"§€ αβγ\n"s, L"q"), L"\"§€ αβγ\\n\"");
            TEST_EQUAL(format_as<wchar_t>(U"§€ αβγ\n"s, L"o"), L"\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
            TEST_EQUAL(format_as<wchar_t>(U"§€ αβγ\n"s, L"n"), L"167 8364 32 945 946 947 10");
            TEST_EQUAL(format_as<wchar_t>(U"§€ αβγ\n"s, L"u"), L"c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
            TEST_EQUAL(format_as<wchar_t>(U"§€ αβγ\n"s, L"v"), L"00a7 20ac 0020 03b1 03b2 03b3 000a");
            TEST_EQUAL(format_as<wchar_t>(U"§€ αβγ\n"s, L"x"), L"a7 20ac 20 3b1 3b2 3b3 a");
            TEST_EQUAL(format_as<wchar_t>(L"§€ αβγ\n"s), L"§€ αβγ\n");
            TEST_EQUAL(format_as<wchar_t>(L"§€ αβγ\n"s, L"a"), L"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
            TEST_EQUAL(format_as<wchar_t>(L"§€ αβγ\n"s, L"c"), L"§€ αβγ\\n");
            TEST_EQUAL(format_as<wchar_t>(L"§€ αβγ\n"s, L"q"), L"\"§€ αβγ\\n\"");
            TEST_EQUAL(format_as<wchar_t>(L"§€ αβγ\n"s, L"o"), L"\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
            TEST_EQUAL(format_as<wchar_t>(L"§€ αβγ\n"s, L"n"), L"167 8364 32 945 946 947 10");
            TEST_EQUAL(format_as<wchar_t>(L"§€ αβγ\n"s, L"u"), L"c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
            TEST_EQUAL(format_as<wchar_t>(L"§€ αβγ\n"s, L"v"), L"00a7 20ac 0020 03b1 03b2 03b3 000a");
            TEST_EQUAL(format_as<wchar_t>(L"§€ αβγ\n"s, L"x"), L"a7 20ac 20 3b1 3b2 3b3 a");
        #endif

    }

    void check_date_and_time_formatting() {

        system_clock::time_point ldate, udate;

        TRY(udate = make_date(2000, 1, 2, 3, 4, 5.678));
        TRY(ldate = make_date(2000, 1, 2, 3, 4, 5.678, local_date));

        TEST_EQUAL(format_as<char>(udate), "2000-01-02 03:04:05");
        TEST_EQUAL(format_as<char>(udate, "3"), "2000-01-02 03:04:05.678");
        TEST_EQUAL(format_as<char>(ldate, "l"), "2000-01-02 03:04:05");
        TEST_EQUAL(format_as<char>(ldate, "l3"), "2000-01-02 03:04:05.678");
        TEST_EQUAL(format_as<char>(udate, "t"), "2000-01-02T03:04:05");
        TEST_EQUAL(format_as<char>(udate, "t3"), "2000-01-02T03:04:05.678");
        TEST_EQUAL(format_as<char>(ldate, "tl"), "2000-01-02T03:04:05");
        TEST_EQUAL(format_as<char>(ldate, "tl3"), "2000-01-02T03:04:05.678");

        TEST_THROW(format_as<char>(udate, "tc"), std::invalid_argument);

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(udate), u"2000-01-02 03:04:05");
            TEST_EQUAL(format_as<char16_t>(udate, u"3"), u"2000-01-02 03:04:05.678");
            TEST_EQUAL(format_as<char16_t>(ldate, u"l3"), u"2000-01-02 03:04:05.678");
            TEST_EQUAL(format_as<char16_t>(udate, u"t3"), u"2000-01-02T03:04:05.678");
            TEST_EQUAL(format_as<char16_t>(ldate, u"tl3"), u"2000-01-02T03:04:05.678");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(udate), U"2000-01-02 03:04:05");
            TEST_EQUAL(format_as<char32_t>(udate, U"3"), U"2000-01-02 03:04:05.678");
            TEST_EQUAL(format_as<char32_t>(ldate, U"l3"), U"2000-01-02 03:04:05.678");
            TEST_EQUAL(format_as<char32_t>(udate, U"t3"), U"2000-01-02T03:04:05.678");
            TEST_EQUAL(format_as<char32_t>(ldate, U"tl3"), U"2000-01-02T03:04:05.678");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(udate), L"2000-01-02 03:04:05");
            TEST_EQUAL(format_as<wchar_t>(udate, L"3"), L"2000-01-02 03:04:05.678");
            TEST_EQUAL(format_as<wchar_t>(ldate, L"l3"), L"2000-01-02 03:04:05.678");
            TEST_EQUAL(format_as<wchar_t>(udate, L"t3"), L"2000-01-02T03:04:05.678");
            TEST_EQUAL(format_as<wchar_t>(ldate, L"tl3"), L"2000-01-02T03:04:05.678");
        #endif

        system_clock::duration t;

        TRY(from_seconds(0, t));            TEST_EQUAL(format_as<char>(t), "0s");
        TRY(from_seconds(0, t));            TEST_EQUAL(format_as<char>(t, "3"), "0.000s");
        TRY(from_seconds(0.25, t));         TEST_EQUAL(format_as<char>(t, "3"), "0.250s");
        TRY(from_seconds(1, t));            TEST_EQUAL(format_as<char>(t, "3"), "1.000s");
        TRY(from_seconds(1234, t));         TEST_EQUAL(format_as<char>(t, "3"), "20m34.000s");
        TRY(from_seconds(12345, t));        TEST_EQUAL(format_as<char>(t, "3"), "3h25m45.000s");
        TRY(from_seconds(123456, t));       TEST_EQUAL(format_as<char>(t, "3"), "1d10h17m36.000s");
        TRY(from_seconds(1234567, t));      TEST_EQUAL(format_as<char>(t, "3"), "14d06h56m07.000s");
        TRY(from_seconds(12345678, t));     TEST_EQUAL(format_as<char>(t, "3"), "142d21h21m18.000s");
        TRY(from_seconds(123456789, t));    TEST_EQUAL(format_as<char>(t, "3"), "3y333d03h33m09.000s");
        TRY(from_seconds(1234567890, t));   TEST_EQUAL(format_as<char>(t, "3"), "39y044d05h31m30.000s");
        TRY(from_seconds(-0.25, t));        TEST_EQUAL(format_as<char>(t, "3"), "-0.250s");
        TRY(from_seconds(-1, t));           TEST_EQUAL(format_as<char>(t, "3"), "-1.000s");
        TRY(from_seconds(-1234, t));        TEST_EQUAL(format_as<char>(t, "3"), "-20m34.000s");
        TRY(from_seconds(-1234567890, t));  TEST_EQUAL(format_as<char>(t, "3"), "-39y044d05h31m30.000s");

        #if defined(UNICORN_PCRE16)
            TRY(from_seconds(0, t));            TEST_EQUAL(format_as<char16_t>(t), u"0s");
            TRY(from_seconds(0.25, t));         TEST_EQUAL(format_as<char16_t>(t, u"3"), u"0.250s");
            TRY(from_seconds(1234567890, t));   TEST_EQUAL(format_as<char16_t>(t, u"3"), u"39y044d05h31m30.000s");
            TRY(from_seconds(-0.25, t));        TEST_EQUAL(format_as<char16_t>(t, u"3"), u"-0.250s");
            TRY(from_seconds(-1234567890, t));  TEST_EQUAL(format_as<char16_t>(t, u"3"), u"-39y044d05h31m30.000s");
        #endif

        #if defined(UNICORN_PCRE32)
            TRY(from_seconds(0, t));            TEST_EQUAL(format_as<char32_t>(t), U"0s");
            TRY(from_seconds(0.25, t));         TEST_EQUAL(format_as<char32_t>(t, U"3"), U"0.250s");
            TRY(from_seconds(1234567890, t));   TEST_EQUAL(format_as<char32_t>(t, U"3"), U"39y044d05h31m30.000s");
            TRY(from_seconds(-0.25, t));        TEST_EQUAL(format_as<char32_t>(t, U"3"), U"-0.250s");
            TRY(from_seconds(-1234567890, t));  TEST_EQUAL(format_as<char32_t>(t, U"3"), U"-39y044d05h31m30.000s");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TRY(from_seconds(0, t));            TEST_EQUAL(format_as<wchar_t>(t), L"0s");
            TRY(from_seconds(0.25, t));         TEST_EQUAL(format_as<wchar_t>(t, L"3"), L"0.250s");
            TRY(from_seconds(1234567890, t));   TEST_EQUAL(format_as<wchar_t>(t, L"3"), L"39y044d05h31m30.000s");
            TRY(from_seconds(-0.25, t));        TEST_EQUAL(format_as<wchar_t>(t, L"3"), L"-0.250s");
            TRY(from_seconds(-1234567890, t));  TEST_EQUAL(format_as<wchar_t>(t, L"3"), L"-39y044d05h31m30.000s");
        #endif

    }

    void check_uuid_formatting() {

        Uuid u1, u2 {0x01234567, 0x89ab, 0xcdef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

        TEST_EQUAL(format_as<char>(u1), "00000000-0000-0000-0000-000000000000");
        TEST_EQUAL(format_as<char>(u2), "01234567-89ab-cdef-0123-456789abcdef");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(u1), u"00000000-0000-0000-0000-000000000000");
            TEST_EQUAL(format_as<char16_t>(u2), u"01234567-89ab-cdef-0123-456789abcdef");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(u1), U"00000000-0000-0000-0000-000000000000");
            TEST_EQUAL(format_as<char32_t>(u2), U"01234567-89ab-cdef-0123-456789abcdef");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(u1), L"00000000-0000-0000-0000-000000000000");
            TEST_EQUAL(format_as<wchar_t>(u2), L"01234567-89ab-cdef-0123-456789abcdef");
        #endif

    }

    void check_version_formatting() {

        Version v1 {1, 0, 0}, v2 {2, 3, 4};

        TEST_EQUAL(format_as<char>(v1), "1.0");
        TEST_EQUAL(format_as<char>(v2), "2.3.4");
        TEST_EQUAL(format_as<char>(v1, "4"), "1.0.0.0");
        TEST_EQUAL(format_as<char>(v2, "4"), "2.3.4.0");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(v1), u"1.0");
            TEST_EQUAL(format_as<char16_t>(v2), u"2.3.4");
            TEST_EQUAL(format_as<char16_t>(v1, u"4"), u"1.0.0.0");
            TEST_EQUAL(format_as<char16_t>(v2, u"4"), u"2.3.4.0");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(v1), U"1.0");
            TEST_EQUAL(format_as<char32_t>(v2), U"2.3.4");
            TEST_EQUAL(format_as<char32_t>(v1, U"4"), U"1.0.0.0");
            TEST_EQUAL(format_as<char32_t>(v2, U"4"), U"2.3.4.0");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(v1), L"1.0");
            TEST_EQUAL(format_as<wchar_t>(v2), L"2.3.4");
            TEST_EQUAL(format_as<wchar_t>(v1, L"4"), L"1.0.0.0");
            TEST_EQUAL(format_as<wchar_t>(v2, L"4"), L"2.3.4.0");
        #endif

    }

    void check_range_formatting() {

        vector<int> iv0, iv1 {10, 20, 30, 40, 50};
        vector<u8string> sv0, sv1 {"hello", "world", "goodbye"};
        std::map<int, u8string> ism0, ism1 {{10, "hello"}, {20, "world"}, {30, "goodbye"}};

        TEST_EQUAL(format_as<char>(iv0), "");
        TEST_EQUAL(format_as<char>(iv1), "10,20,30,40,50");
        TEST_EQUAL(format_as<char>(iv1, "x4"), "000a,0014,001e,0028,0032");
        TEST_EQUAL(format_as<char>(sv0), "");
        TEST_EQUAL(format_as<char>(sv1), "hello,world,goodbye");
        TEST_EQUAL(format_as<char>(sv1, "U"), "HELLO,WORLD,GOODBYE");
        TEST_EQUAL(format_as<char>(ism0), "");
        TEST_EQUAL(format_as<char>(ism1), "10:hello,20:world,30:goodbye");
        TEST_EQUAL(format_as<char>(ism1, "U4"), "0010:HELLO,0020:WORLD,0030:GOODBYE");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(iv1), u"10,20,30,40,50");
            TEST_EQUAL(format_as<char16_t>(sv1), u"hello,world,goodbye");
            TEST_EQUAL(format_as<char16_t>(ism1), u"10:hello,20:world,30:goodbye");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(iv1), U"10,20,30,40,50");
            TEST_EQUAL(format_as<char32_t>(sv1), U"hello,world,goodbye");
            TEST_EQUAL(format_as<char32_t>(ism1), U"10:hello,20:world,30:goodbye");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(iv1), L"10,20,30,40,50");
            TEST_EQUAL(format_as<wchar_t>(sv1), L"hello,world,goodbye");
            TEST_EQUAL(format_as<wchar_t>(ism1), L"10:hello,20:world,30:goodbye");
        #endif

    }

    void check_alignment_and_padding() {

        // <, fx_left    = Left align
        // =, fx_centre  = Centre align
        // >, fx_right   = Right align

        TEST_EQUAL(format_as<char>(42, fx_left, 0, 1u), "42");
        TEST_EQUAL(format_as<char>(42, fx_right, 0, 1u), "42");
        TEST_EQUAL(format_as<char>(42, fx_centre, 0, 1u), "42");
        TEST_EQUAL(format_as<char>(42, fx_left, 0, 5u), "42   ");
        TEST_EQUAL(format_as<char>(42, fx_right, 0, 5u), "   42");
        TEST_EQUAL(format_as<char>(42, fx_centre, 0, 8u), "   42   ");
        TEST_EQUAL(format_as<char>(42, fx_centre, 0, 9u), "   42    ");
        TEST_EQUAL(format_as<char>(42, fx_left, 0, 5u, U'*'), "42***");
        TEST_EQUAL(format_as<char>(42, fx_right, 0, 5u, U'*'), "***42");
        TEST_EQUAL(format_as<char>(42, fx_centre, 0, 8u, U'*'), "***42***");

        TEST_EQUAL(format_as<char>(42, "<"), "42");
        TEST_EQUAL(format_as<char>(42, ">"), "42");
        TEST_EQUAL(format_as<char>(42, "="), "42");
        TEST_EQUAL(format_as<char>(42, "<5"), "42   ");
        TEST_EQUAL(format_as<char>(42, ">5"), "   42");
        TEST_EQUAL(format_as<char>(42, "=8"), "   42   ");
        TEST_EQUAL(format_as<char>(42, "<*5"), "42***");
        TEST_EQUAL(format_as<char>(42, ">*5"), "***42");
        TEST_EQUAL(format_as<char>(42, "=*8"), "***42***");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(42, u"<"), u"42");
            TEST_EQUAL(format_as<char16_t>(42, u">"), u"42");
            TEST_EQUAL(format_as<char16_t>(42, u"="), u"42");
            TEST_EQUAL(format_as<char16_t>(42, u"<5"), u"42   ");
            TEST_EQUAL(format_as<char16_t>(42, u">5"), u"   42");
            TEST_EQUAL(format_as<char16_t>(42, u"=8"), u"   42   ");
            TEST_EQUAL(format_as<char16_t>(42, u"<*5"), u"42***");
            TEST_EQUAL(format_as<char16_t>(42, u">*5"), u"***42");
            TEST_EQUAL(format_as<char16_t>(42, u"=*8"), u"***42***");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(42, U"<"), U"42");
            TEST_EQUAL(format_as<char32_t>(42, U">"), U"42");
            TEST_EQUAL(format_as<char32_t>(42, U"="), U"42");
            TEST_EQUAL(format_as<char32_t>(42, U"<5"), U"42   ");
            TEST_EQUAL(format_as<char32_t>(42, U">5"), U"   42");
            TEST_EQUAL(format_as<char32_t>(42, U"=8"), U"   42   ");
            TEST_EQUAL(format_as<char32_t>(42, U"<*5"), U"42***");
            TEST_EQUAL(format_as<char32_t>(42, U">*5"), U"***42");
            TEST_EQUAL(format_as<char32_t>(42, U"=*8"), U"***42***");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(42, L"<"), L"42");
            TEST_EQUAL(format_as<wchar_t>(42, L">"), L"42");
            TEST_EQUAL(format_as<wchar_t>(42, L"="), L"42");
            TEST_EQUAL(format_as<wchar_t>(42, L"<5"), L"42   ");
            TEST_EQUAL(format_as<wchar_t>(42, L">5"), L"   42");
            TEST_EQUAL(format_as<wchar_t>(42, L"=8"), L"   42   ");
            TEST_EQUAL(format_as<wchar_t>(42, L"<*5"), L"42***");
            TEST_EQUAL(format_as<wchar_t>(42, L">*5"), L"***42");
            TEST_EQUAL(format_as<wchar_t>(42, L"=*8"), L"***42***");
        #endif

    }

    void check_case_mapping() {

        // L, fx_lower  = Convert to lower case
        // T, fx_title  = Convert to title case
        // U, fx_upper  = Convert to upper case

        TEST_EQUAL(format_as<char>("hello WORLD"s, "L"), "hello world"s);
        TEST_EQUAL(format_as<char>("hello WORLD"s, "T"), "Hello World"s);
        TEST_EQUAL(format_as<char>("hello WORLD"s, "U"), "HELLO WORLD"s);

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format_as<char16_t>(u"hello WORLD"s, u"L"), u"hello world"s);
            TEST_EQUAL(format_as<char16_t>(u"hello WORLD"s, u"T"), u"Hello World"s);
            TEST_EQUAL(format_as<char16_t>(u"hello WORLD"s, u"U"), u"HELLO WORLD"s);
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format_as<char32_t>(U"hello WORLD"s, U"L"), U"hello world"s);
            TEST_EQUAL(format_as<char32_t>(U"hello WORLD"s, U"T"), U"Hello World"s);
            TEST_EQUAL(format_as<char32_t>(U"hello WORLD"s, U"U"), U"HELLO WORLD"s);
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format_as<wchar_t>(L"hello WORLD"s, L"L"), L"hello world"s);
            TEST_EQUAL(format_as<wchar_t>(L"hello WORLD"s, L"T"), L"Hello World"s);
            TEST_EQUAL(format_as<wchar_t>(L"hello WORLD"s, L"U"), L"HELLO WORLD"s);
        #endif

    }

    void check_formatter_class() {

        TEST_EQUAL(format("")(), "");
        TEST_EQUAL(format("Hello world")(), "Hello world");
        TEST_EQUAL(format("Hello $$world")(), "Hello $world");
        TEST_EQUAL(format("Hello $1")("world"), "Hello world");
        TEST_EQUAL(format("Hello $1")("world", 42), "Hello world");
        TEST_EQUAL(format("Hello $2")("world", 42), "Hello 42");
        TEST_EQUAL(format("Hello $3")("world", 42), "Hello ");
        TEST_EQUAL(format("Hello $1 $2")("world", 42), "Hello world 42");
        TEST_EQUAL(format("Hello $2 $1")("world", 42), "Hello 42 world");
        TEST_EQUAL(format("Hello ${1}")("world"), "Hello world");
        TEST_EQUAL(format("Hello ${1}")("world", 42), "Hello world");
        TEST_EQUAL(format("Hello ${2}")("world", 42), "Hello 42");
        TEST_EQUAL(format("Hello ${3}")("world", 42), "Hello ");
        TEST_EQUAL(format("Hello ${1} ${2}")("world", 42), "Hello world 42");
        TEST_EQUAL(format("Hello ${2} ${1}")("world", 42), "Hello 42 world");
        TEST_EQUAL(format("Hello ${1U}")("world"), "Hello WORLD");
        TEST_EQUAL(format("Hello ${1<8}")("world"), "Hello world   ");
        TEST_EQUAL(format("Hello ${1U<8}")("world"), "Hello WORLD   ");
        TEST_EQUAL(format("Hello ${1 4}")(42), "Hello 0042");
        TEST_EQUAL(format("Hello ${1x}")(42), "Hello 2a");
        TEST_EQUAL(format("Hello ${1x4}")(42), "Hello 002a");
        TEST_EQUAL(format("Hello $1x")(42), "Hello 2a");
        TEST_EQUAL(format("Hello $1x4")(42), "Hello 002a");
        TEST_EQUAL(format("Hello $1")(u"world"), "Hello world");
        TEST_EQUAL(format("Hello $1")(U"world"), "Hello world");
        TEST_EQUAL(format("Hello $1")(L"world"), "Hello world");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(format(u"")(), u"");
            TEST_EQUAL(format(u"Hello world")(), u"Hello world");
            TEST_EQUAL(format(u"Hello $$world")(), u"Hello $world");
            TEST_EQUAL(format(u"Hello $1")(u"world"), u"Hello world");
            TEST_EQUAL(format(u"Hello $1")(u"world", 42), u"Hello world");
            TEST_EQUAL(format(u"Hello $2")(u"world", 42), u"Hello 42");
            TEST_EQUAL(format(u"Hello $3")(u"world", 42), u"Hello ");
            TEST_EQUAL(format(u"Hello $1 $2")(u"world", 42), u"Hello world 42");
            TEST_EQUAL(format(u"Hello $2 $1")(u"world", 42), u"Hello 42 world");
            TEST_EQUAL(format(u"Hello ${1}")(u"world"), u"Hello world");
            TEST_EQUAL(format(u"Hello ${1}")(u"world", 42), u"Hello world");
            TEST_EQUAL(format(u"Hello ${2}")(u"world", 42), u"Hello 42");
            TEST_EQUAL(format(u"Hello ${3}")(u"world", 42), u"Hello ");
            TEST_EQUAL(format(u"Hello ${1} ${2}")(u"world", 42), u"Hello world 42");
            TEST_EQUAL(format(u"Hello ${2} ${1}")(u"world", 42), u"Hello 42 world");
            TEST_EQUAL(format(u"Hello ${1U}")(u"world"), u"Hello WORLD");
            TEST_EQUAL(format(u"Hello ${1<8}")(u"world"), u"Hello world   ");
            TEST_EQUAL(format(u"Hello ${1U<8}")(u"world"), u"Hello WORLD   ");
            TEST_EQUAL(format(u"Hello ${1 4}")(42), u"Hello 0042");
            TEST_EQUAL(format(u"Hello ${1x}")(42), u"Hello 2a");
            TEST_EQUAL(format(u"Hello ${1x4}")(42), u"Hello 002a");
            TEST_EQUAL(format(u"Hello $1x")(42), u"Hello 2a");
            TEST_EQUAL(format(u"Hello $1x4")(42), u"Hello 002a");
            TEST_EQUAL(format(u"Hello $1")("world"), u"Hello world");
            TEST_EQUAL(format(u"Hello $1")(U"world"), u"Hello world");
            TEST_EQUAL(format(u"Hello $1")(L"world"), u"Hello world");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(format(U"")(), U"");
            TEST_EQUAL(format(U"Hello world")(), U"Hello world");
            TEST_EQUAL(format(U"Hello $$world")(), U"Hello $world");
            TEST_EQUAL(format(U"Hello $1")(U"world"), U"Hello world");
            TEST_EQUAL(format(U"Hello $1")(U"world", 42), U"Hello world");
            TEST_EQUAL(format(U"Hello $2")(U"world", 42), U"Hello 42");
            TEST_EQUAL(format(U"Hello $3")(U"world", 42), U"Hello ");
            TEST_EQUAL(format(U"Hello $1 $2")(U"world", 42), U"Hello world 42");
            TEST_EQUAL(format(U"Hello $2 $1")(U"world", 42), U"Hello 42 world");
            TEST_EQUAL(format(U"Hello ${1}")(U"world"), U"Hello world");
            TEST_EQUAL(format(U"Hello ${1}")(U"world", 42), U"Hello world");
            TEST_EQUAL(format(U"Hello ${2}")(U"world", 42), U"Hello 42");
            TEST_EQUAL(format(U"Hello ${3}")(U"world", 42), U"Hello ");
            TEST_EQUAL(format(U"Hello ${1} ${2}")(U"world", 42), U"Hello world 42");
            TEST_EQUAL(format(U"Hello ${2} ${1}")(U"world", 42), U"Hello 42 world");
            TEST_EQUAL(format(U"Hello ${1U}")(U"world"), U"Hello WORLD");
            TEST_EQUAL(format(U"Hello ${1<8}")(U"world"), U"Hello world   ");
            TEST_EQUAL(format(U"Hello ${1U<8}")(U"world"), U"Hello WORLD   ");
            TEST_EQUAL(format(U"Hello ${1 4}")(42), U"Hello 0042");
            TEST_EQUAL(format(U"Hello ${1x}")(42), U"Hello 2a");
            TEST_EQUAL(format(U"Hello ${1x4}")(42), U"Hello 002a");
            TEST_EQUAL(format(U"Hello $1x")(42), U"Hello 2a");
            TEST_EQUAL(format(U"Hello $1x4")(42), U"Hello 002a");
            TEST_EQUAL(format(U"Hello $1")("world"), U"Hello world");
            TEST_EQUAL(format(U"Hello $1")(u"world"), U"Hello world");
            TEST_EQUAL(format(U"Hello $1")(L"world"), U"Hello world");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(format(L"")(), L"");
            TEST_EQUAL(format(L"Hello world")(), L"Hello world");
            TEST_EQUAL(format(L"Hello $$world")(), L"Hello $world");
            TEST_EQUAL(format(L"Hello $1")(L"world"), L"Hello world");
            TEST_EQUAL(format(L"Hello $1")(L"world", 42), L"Hello world");
            TEST_EQUAL(format(L"Hello $2")(L"world", 42), L"Hello 42");
            TEST_EQUAL(format(L"Hello $3")(L"world", 42), L"Hello ");
            TEST_EQUAL(format(L"Hello $1 $2")(L"world", 42), L"Hello world 42");
            TEST_EQUAL(format(L"Hello $2 $1")(L"world", 42), L"Hello 42 world");
            TEST_EQUAL(format(L"Hello ${1}")(L"world"), L"Hello world");
            TEST_EQUAL(format(L"Hello ${1}")(L"world", 42), L"Hello world");
            TEST_EQUAL(format(L"Hello ${2}")(L"world", 42), L"Hello 42");
            TEST_EQUAL(format(L"Hello ${3}")(L"world", 42), L"Hello ");
            TEST_EQUAL(format(L"Hello ${1} ${2}")(L"world", 42), L"Hello world 42");
            TEST_EQUAL(format(L"Hello ${2} ${1}")(L"world", 42), L"Hello 42 world");
            TEST_EQUAL(format(L"Hello ${1U}")(L"world"), L"Hello WORLD");
            TEST_EQUAL(format(L"Hello ${1<8}")(L"world"), L"Hello world   ");
            TEST_EQUAL(format(L"Hello ${1U<8}")(L"world"), L"Hello WORLD   ");
            TEST_EQUAL(format(L"Hello ${1 4}")(42), L"Hello 0042");
            TEST_EQUAL(format(L"Hello ${1x}")(42), L"Hello 2a");
            TEST_EQUAL(format(L"Hello ${1x4}")(42), L"Hello 002a");
            TEST_EQUAL(format(L"Hello $1x")(42), L"Hello 2a");
            TEST_EQUAL(format(L"Hello $1x4")(42), L"Hello 002a");
            TEST_EQUAL(format(L"Hello $1")("world"), L"Hello world");
            TEST_EQUAL(format(L"Hello $1")(u"world"), L"Hello world");
            TEST_EQUAL(format(L"Hello $1")(U"world"), L"Hello world");
        #endif

    }

    void check_formatter_literals() {

        TEST_EQUAL(""_fmt(), "");
        TEST_EQUAL("Hello world"_fmt(), "Hello world");
        TEST_EQUAL("Hello $1"_fmt("world"), "Hello world");

        #if defined(UNICORN_PCRE16)
            TEST_EQUAL(u""_fmt(), u"");
            TEST_EQUAL(u"Hello world"_fmt(), u"Hello world");
            TEST_EQUAL(u"Hello $1"_fmt(u"world"), u"Hello world");
        #endif

        #if defined(UNICORN_PCRE32)
            TEST_EQUAL(U""_fmt(), U"");
            TEST_EQUAL(U"Hello world"_fmt(), U"Hello world");
            TEST_EQUAL(U"Hello $1"_fmt(U"world"), U"Hello world");
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            TEST_EQUAL(L""_fmt(), L"");
            TEST_EQUAL(L"Hello world"_fmt(), L"Hello world");
            TEST_EQUAL(L"Hello $1"_fmt(L"world"), L"Hello world");
        #endif

    }

}

TEST_MODULE(unicorn, format) {

    check_boolean_formatting();
    check_integer_formatting();
    check_floating_point_formatting();
    check_character_formatting();
    check_string_formatting();
    check_date_and_time_formatting();
    check_uuid_formatting();
    check_version_formatting();
    check_range_formatting();
    check_alignment_and_padding();
    check_case_mapping();
    check_formatter_class();
    check_formatter_literals();

}
