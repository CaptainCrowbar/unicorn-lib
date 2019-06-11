#include "unicorn/format.hpp"
#include "unicorn/unit-test.hpp"
#include <chrono>
#include <map>
#include <stdexcept>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace RS::Unicorn::Literals;
using namespace std::chrono;
using namespace std::literals;

void test_unicorn_format_booleans() {

    // b, binary  = Binary number
    // t, tf      = Write as true/false [default]
    // y, yesno   = Write as yes/no

    TEST_EQUAL(format_str(true), "true");
    TEST_EQUAL(format_str(false), "false");
    TEST_EQUAL(format_str(true, "b"), "1");
    TEST_EQUAL(format_str(false, "b"), "0");
    TEST_EQUAL(format_str(true, "t"), "true");
    TEST_EQUAL(format_str(false, "t"), "false");
    TEST_EQUAL(format_str(true, "y"), "yes");
    TEST_EQUAL(format_str(false, "y"), "no");

}

void test_unicorn_format_integers() {

    // n, decimal  = Decimal number [default]
    // b, binary   = Binary number
    // x, hex      = Hexadecimal number
    // r, roman    = Roman numerals
    // s, sign     = Always show a sign
    // S, signz    = Always show a sign unless zero

    TEST_EQUAL(format_str(0), "0");
    TEST_EQUAL(format_str(42), "42");
    TEST_EQUAL(format_str(-42), "-42");

    TEST_EQUAL(format_str(0, "n"), "0");
    TEST_EQUAL(format_str(42, "n"), "42");
    TEST_EQUAL(format_str(-42, "n"), "-42");
    TEST_EQUAL(format_str(0, "b"), "0");
    TEST_EQUAL(format_str(42, "b"), "101010");
    TEST_EQUAL(format_str(-42, "b"), "-101010");
    TEST_EQUAL(format_str(0, "x"), "0");
    TEST_EQUAL(format_str(42, "x"), "2a");
    TEST_EQUAL(format_str(-42, "x"), "-2a");

    TEST_EQUAL(format_str(0, "s"), "+0");
    TEST_EQUAL(format_str(42, "s"), "+42");
    TEST_EQUAL(format_str(-42, "s"), "-42");
    TEST_EQUAL(format_str(0, "sb"), "+0");
    TEST_EQUAL(format_str(42, "sb"), "+101010");
    TEST_EQUAL(format_str(-42, "sb"), "-101010");
    TEST_EQUAL(format_str(0, "sx"), "+0");
    TEST_EQUAL(format_str(42, "sx"), "+2a");
    TEST_EQUAL(format_str(-42, "sx"), "-2a");

    TEST_EQUAL(format_str(0, "S"), "0");
    TEST_EQUAL(format_str(42, "S"), "+42");
    TEST_EQUAL(format_str(-42, "S"), "-42");
    TEST_EQUAL(format_str(0, "Sb"), "0");
    TEST_EQUAL(format_str(42, "Sb"), "+101010");
    TEST_EQUAL(format_str(-42, "Sb"), "-101010");
    TEST_EQUAL(format_str(0, "Sx"), "0");
    TEST_EQUAL(format_str(42, "Sx"), "+2a");
    TEST_EQUAL(format_str(-42, "Sx"), "-2a");

    TEST_EQUAL(format_str(0, "8"), "00000000");
    TEST_EQUAL(format_str(42, "8"), "00000042");
    TEST_EQUAL(format_str(-42, "8"), "-00000042");
    TEST_EQUAL(format_str(0, "b8"), "00000000");
    TEST_EQUAL(format_str(42, "b8"), "00101010");
    TEST_EQUAL(format_str(-42, "b8"), "-00101010");
    TEST_EQUAL(format_str(0, "x8"), "00000000");
    TEST_EQUAL(format_str(42, "x8"), "0000002a");
    TEST_EQUAL(format_str(-42, "x8"), "-0000002a");

    TEST_EQUAL(format_str(0, "r"), "");
    TEST_EQUAL(format_str(1, "r"), "I");
    TEST_EQUAL(format_str(42, "r"), "XLII");
    TEST_EQUAL(format_str(1111, "r"), "MCXI");
    TEST_EQUAL(format_str(2222, "r"), "MMCCXXII");
    TEST_EQUAL(format_str(3333, "r"), "MMMCCCXXXIII");
    TEST_EQUAL(format_str(4444, "r"), "MMMMCDXLIV");
    TEST_EQUAL(format_str(5555, "r"), "MMMMMDLV");
    TEST_EQUAL(format_str(6666, "r"), "MMMMMMDCLXVI");
    TEST_EQUAL(format_str(7777, "r"), "MMMMMMMDCCLXXVII");
    TEST_EQUAL(format_str(8888, "r"), "MMMMMMMMDCCCLXXXVIII");
    TEST_EQUAL(format_str(9999, "r"), "MMMMMMMMMCMXCIX");

}

void test_unicorn_format_floating_point() {

    // d, digits   = Fixed significant figures
    // e, exp      = Scientific notation
    // f, fixed    = Fixed point notation
    // g, general  = Use the shorter of d or e [default]
    // s, sign     = Always show a sign
    // i, signz    = Always show a sign unless zero
    // z, stripz   = Strip trailing zeros

    TEST_EQUAL(format_str(0.0, "d"), "0.00000");
    TEST_EQUAL(format_str(123.0, "d"), "123.000");
    TEST_EQUAL(format_str(-123.0, "d"), "-123.000");
    TEST_EQUAL(format_str(0.123, "d"), "0.123000");
    TEST_EQUAL(format_str(0.0, "e"), "0.00000e0");
    TEST_EQUAL(format_str(123.0, "e"), "1.23000e2");
    TEST_EQUAL(format_str(-123.0, "e"), "-1.23000e2");
    TEST_EQUAL(format_str(0.123, "e"), "1.23000e-1");
    TEST_EQUAL(format_str(0.0, "f"), "0.000000");
    TEST_EQUAL(format_str(123.0, "f"), "123.000000");
    TEST_EQUAL(format_str(-123.0, "f"), "-123.000000");
    TEST_EQUAL(format_str(0.123, "f"), "0.123000");
    TEST_EQUAL(format_str(0.0, "g"), "0.00000");
    TEST_EQUAL(format_str(123.0, "g"), "123.000");
    TEST_EQUAL(format_str(-123.0, "g"), "-123.000");
    TEST_EQUAL(format_str(0.123, "g"), "0.123000");
    TEST_EQUAL(format_str(0.0, "d3"), "0.00");
    TEST_EQUAL(format_str(0.0, "d2"), "0.0");
    TEST_EQUAL(format_str(0.0, "d1"), "0");
    TEST_EQUAL(format_str(0.0, "d0"), "0");
    TEST_EQUAL(format_str(0.0, "dz3"), "0");
    TEST_EQUAL(format_str(56789.0, "d6"), "56789.0");
    TEST_EQUAL(format_str(56789.0, "d5"), "56789");
    TEST_EQUAL(format_str(56789.0, "d4"), "56790");
    TEST_EQUAL(format_str(56789.0, "d3"), "56800");
    TEST_EQUAL(format_str(56789.0, "d2"), "57000");
    TEST_EQUAL(format_str(56789.0, "d1"), "60000");
    TEST_EQUAL(format_str(56789.0, "d0"), "60000");
    TEST_EQUAL(format_str(56789.0, "dz6"), "56789");
    TEST_EQUAL(format_str(56789.0, "dz5"), "56789");
    TEST_EQUAL(format_str(0.00056789, "d3"), "0.000568");
    TEST_EQUAL(format_str(0.0056789, "d3"), "0.00568");
    TEST_EQUAL(format_str(0.056789, "d3"), "0.0568");
    TEST_EQUAL(format_str(0.56789, "d3"), "0.568");
    TEST_EQUAL(format_str(5.6789, "d3"), "5.68");
    TEST_EQUAL(format_str(56.789, "d3"), "56.8");
    TEST_EQUAL(format_str(567.89, "d3"), "568");
    TEST_EQUAL(format_str(5678.9, "d3"), "5680");
    TEST_EQUAL(format_str(56789.0, "d3"), "56800");
    TEST_EQUAL(format_str(567890.0, "d3"), "568000");
    TEST_EQUAL(format_str(-0.00056789, "d3"), "-0.000568");
    TEST_EQUAL(format_str(-0.0056789, "d3"), "-0.00568");
    TEST_EQUAL(format_str(-0.056789, "d3"), "-0.0568");
    TEST_EQUAL(format_str(-0.56789, "d3"), "-0.568");
    TEST_EQUAL(format_str(-5.6789, "d3"), "-5.68");
    TEST_EQUAL(format_str(-56.789, "d3"), "-56.8");
    TEST_EQUAL(format_str(-567.89, "d3"), "-568");
    TEST_EQUAL(format_str(-5678.9, "d3"), "-5680");
    TEST_EQUAL(format_str(-56789.0, "d3"), "-56800");
    TEST_EQUAL(format_str(-567890.0, "d3"), "-568000");
    TEST_EQUAL(format_str(0.0, "e3"), "0.00e0");
    TEST_EQUAL(format_str(0.0, "e2"), "0.0e0");
    TEST_EQUAL(format_str(0.0, "e1"), "0e0");
    TEST_EQUAL(format_str(0.0, "e0"), "0e0");
    TEST_EQUAL(format_str(0.0, "ez3"), "0e0");
    TEST_EQUAL(format_str(56789.0, "e6"), "5.67890e4");
    TEST_EQUAL(format_str(56789.0, "e5"), "5.6789e4");
    TEST_EQUAL(format_str(56789.0, "e4"), "5.679e4");
    TEST_EQUAL(format_str(56789.0, "e3"), "5.68e4");
    TEST_EQUAL(format_str(56789.0, "e2"), "5.7e4");
    TEST_EQUAL(format_str(56789.0, "e1"), "6e4");
    TEST_EQUAL(format_str(56789.0, "e0"), "6e4");
    TEST_EQUAL(format_str(56789.0, "ez6"), "5.6789e4");
    TEST_EQUAL(format_str(56789.0, "ez5"), "5.6789e4");
    TEST_EQUAL(format_str(0.00056789, "e3"), "5.68e-4");
    TEST_EQUAL(format_str(0.0056789, "e3"), "5.68e-3");
    TEST_EQUAL(format_str(0.056789, "e3"), "5.68e-2");
    TEST_EQUAL(format_str(0.56789, "e3"), "5.68e-1");
    TEST_EQUAL(format_str(5.6789, "e3"), "5.68e0");
    TEST_EQUAL(format_str(56.789, "e3"), "5.68e1");
    TEST_EQUAL(format_str(567.89, "e3"), "5.68e2");
    TEST_EQUAL(format_str(5678.9, "e3"), "5.68e3");
    TEST_EQUAL(format_str(56789.0, "e3"), "5.68e4");
    TEST_EQUAL(format_str(567890.0, "e3"), "5.68e5");
    TEST_EQUAL(format_str(-0.00056789, "e3"), "-5.68e-4");
    TEST_EQUAL(format_str(-0.0056789, "e3"), "-5.68e-3");
    TEST_EQUAL(format_str(-0.056789, "e3"), "-5.68e-2");
    TEST_EQUAL(format_str(-0.56789, "e3"), "-5.68e-1");
    TEST_EQUAL(format_str(-5.6789, "e3"), "-5.68e0");
    TEST_EQUAL(format_str(-56.789, "e3"), "-5.68e1");
    TEST_EQUAL(format_str(-567.89, "e3"), "-5.68e2");
    TEST_EQUAL(format_str(-5678.9, "e3"), "-5.68e3");
    TEST_EQUAL(format_str(-56789.0, "e3"), "-5.68e4");
    TEST_EQUAL(format_str(-567890.0, "e3"), "-5.68e5");
    TEST_EQUAL(format_str(0.0, "f2"), "0.00");
    TEST_EQUAL(format_str(0.0, "f1"), "0.0");
    TEST_EQUAL(format_str(0.0, "f0"), "0");
    TEST_EQUAL(format_str(0.0, "fz2"), "0");
    TEST_EQUAL(format_str(5.6789, "f5"), "5.67890");
    TEST_EQUAL(format_str(5.6789, "f4"), "5.6789");
    TEST_EQUAL(format_str(5.6789, "f3"), "5.679");
    TEST_EQUAL(format_str(5.6789, "f2"), "5.68");
    TEST_EQUAL(format_str(5.6789, "f1"), "5.7");
    TEST_EQUAL(format_str(5.6789, "f0"), "6");
    TEST_EQUAL(format_str(5.6789, "fz5"), "5.6789");
    TEST_EQUAL(format_str(5.6789, "fz4"), "5.6789");
    TEST_EQUAL(format_str(0.00056789, "f2"), "0.00");
    TEST_EQUAL(format_str(0.0056789, "f2"), "0.01");
    TEST_EQUAL(format_str(0.056789, "f2"), "0.06");
    TEST_EQUAL(format_str(0.56789, "f2"), "0.57");
    TEST_EQUAL(format_str(5.6789, "f2"), "5.68");
    TEST_EQUAL(format_str(56.789, "f2"), "56.79");
    TEST_EQUAL(format_str(567.89, "f2"), "567.89");
    TEST_EQUAL(format_str(5678.9, "f2"), "5678.90");
    TEST_EQUAL(format_str(56789.0, "f2"), "56789.00");
    TEST_EQUAL(format_str(567890.0, "f2"), "567890.00");
    TEST_EQUAL(format_str(-0.00056789, "f2"), "-0.00");
    TEST_EQUAL(format_str(-0.0056789, "f2"), "-0.01");
    TEST_EQUAL(format_str(-0.056789, "f2"), "-0.06");
    TEST_EQUAL(format_str(-0.56789, "f2"), "-0.57");
    TEST_EQUAL(format_str(-5.6789, "f2"), "-5.68");
    TEST_EQUAL(format_str(-56.789, "f2"), "-56.79");
    TEST_EQUAL(format_str(-567.89, "f2"), "-567.89");
    TEST_EQUAL(format_str(-5678.9, "f2"), "-5678.90");
    TEST_EQUAL(format_str(-56789.0, "f2"), "-56789.00");
    TEST_EQUAL(format_str(-567890.0, "f2"), "-567890.00");
    TEST_EQUAL(format_str(0.00056789, "fz2"), "0");
    TEST_EQUAL(format_str(0.0056789, "fz2"), "0.01");
    TEST_EQUAL(format_str(0.056789, "fz2"), "0.06");
    TEST_EQUAL(format_str(0.56789, "fz2"), "0.57");
    TEST_EQUAL(format_str(5.6789, "fz2"), "5.68");
    TEST_EQUAL(format_str(56.789, "fz2"), "56.79");
    TEST_EQUAL(format_str(567.89, "fz2"), "567.89");
    TEST_EQUAL(format_str(5678.9, "fz2"), "5678.9");
    TEST_EQUAL(format_str(56789.0, "fz2"), "56789");
    TEST_EQUAL(format_str(567890.0, "fz2"), "567890");
    TEST_EQUAL(format_str(0.0, "g3"), "0.00");
    TEST_EQUAL(format_str(0.0, "g2"), "0.0");
    TEST_EQUAL(format_str(0.0, "g1"), "0");
    TEST_EQUAL(format_str(0.0, "g0"), "0");
    TEST_EQUAL(format_str(0.0, "gz3"), "0");
    TEST_EQUAL(format_str(0.0000056789, "g3"), "5.68e-6");
    TEST_EQUAL(format_str(0.000056789, "g3"), "5.68e-5");
    TEST_EQUAL(format_str(0.00056789, "g3"), "5.68e-4");
    TEST_EQUAL(format_str(0.0056789, "g3"), "0.00568");
    TEST_EQUAL(format_str(0.056789, "g3"), "0.0568");
    TEST_EQUAL(format_str(0.56789, "g3"), "0.568");
    TEST_EQUAL(format_str(5.6789, "g3"), "5.68");
    TEST_EQUAL(format_str(56.789, "g3"), "56.8");
    TEST_EQUAL(format_str(567.89, "g3"), "568");
    TEST_EQUAL(format_str(5678.9, "g3"), "5680");
    TEST_EQUAL(format_str(56789.0, "g3"), "56800");
    TEST_EQUAL(format_str(567890.0, "g3"), "568000");
    TEST_EQUAL(format_str(5678900.0, "g3"), "5.68e6");
    TEST_EQUAL(format_str(56789000.0, "g3"), "5.68e7");
    TEST_EQUAL(format_str(567890000.0, "g3"), "5.68e8");
    TEST_EQUAL(format_str(-0.0000056789, "g3"), "-5.68e-6");
    TEST_EQUAL(format_str(-0.000056789, "g3"), "-5.68e-5");
    TEST_EQUAL(format_str(-0.00056789, "g3"), "-5.68e-4");
    TEST_EQUAL(format_str(-0.0056789, "g3"), "-0.00568");
    TEST_EQUAL(format_str(-0.056789, "g3"), "-0.0568");
    TEST_EQUAL(format_str(-0.56789, "g3"), "-0.568");
    TEST_EQUAL(format_str(-5.6789, "g3"), "-5.68");
    TEST_EQUAL(format_str(-56.789, "g3"), "-56.8");
    TEST_EQUAL(format_str(-567.89, "g3"), "-568");
    TEST_EQUAL(format_str(-5678.9, "g3"), "-5680");
    TEST_EQUAL(format_str(-56789.0, "g3"), "-56800");
    TEST_EQUAL(format_str(-567890.0, "g3"), "-568000");
    TEST_EQUAL(format_str(-5678900.0, "g3"), "-5.68e6");
    TEST_EQUAL(format_str(-56789000.0, "g3"), "-5.68e7");
    TEST_EQUAL(format_str(-567890000.0, "g3"), "-5.68e8");
    TEST_EQUAL(format_str(0.0, "sd3"), "+0.00");
    TEST_EQUAL(format_str(42.0, "sd3"), "+42.0");
    TEST_EQUAL(format_str(-42.0, "sd3"), "-42.0");
    TEST_EQUAL(format_str(0.0, "Sd3"), "0.00");
    TEST_EQUAL(format_str(42.0, "Sd3"), "+42.0");
    TEST_EQUAL(format_str(-42.0, "Sd3"), "-42.0");
    TEST_EQUAL(format_str(0.0, "se3"), "+0.00e0");
    TEST_EQUAL(format_str(42.0, "se3"), "+4.20e1");
    TEST_EQUAL(format_str(-42.0, "se3"), "-4.20e1");
    TEST_EQUAL(format_str(0.0, "Se3"), "0.00e0");
    TEST_EQUAL(format_str(42.0, "Se3"), "+4.20e1");
    TEST_EQUAL(format_str(-42.0, "Se3"), "-4.20e1");
    TEST_EQUAL(format_str(0.0, "sf3"), "+0.000");
    TEST_EQUAL(format_str(42.0, "sf3"), "+42.000");
    TEST_EQUAL(format_str(-42.0, "sf3"), "-42.000");
    TEST_EQUAL(format_str(0.0, "Sf3"), "0.000");
    TEST_EQUAL(format_str(42.0, "Sf3"), "+42.000");
    TEST_EQUAL(format_str(-42.0, "Sf3"), "-42.000");
    TEST_EQUAL(format_str(0.0, "sg3"), "+0.00");
    TEST_EQUAL(format_str(42.0, "sg3"), "+42.0");
    TEST_EQUAL(format_str(-42.0, "sg3"), "-42.0");
    TEST_EQUAL(format_str(0.0, "Sg3"), "0.00");
    TEST_EQUAL(format_str(42.0, "Sg3"), "+42.0");
    TEST_EQUAL(format_str(-42.0, "Sg3"), "-42.0");

}

void test_unicorn_format_characters() {

    // e, escape    = Escape if C0/C1 control
    // a, ascii     = Escape if not printable ASCII
    // q, quote     = Quote string, escape C0/C1
    // Q, ascquote  = Quote string, escape non-ASCII
    // n, decimal   = Decimal number
    // x, hex       = Hexadecimal number
    // u, hex8      = Hex UTF-8 bytes
    // v, hex16     = Hex UTF-16 code units

    TEST_EQUAL(format_str('\t'), "\t");
    TEST_EQUAL(format_str('\t', "e"), "\\t");
    TEST_EQUAL(format_str('\t', "a"), "\\t");
    TEST_EQUAL(format_str('\t', "q"), "\"\\t\"");
    TEST_EQUAL(format_str('\t', "Q"), "\"\\t\"");
    TEST_EQUAL(format_str('\t', "n"), "9");
    TEST_EQUAL(format_str('\t', "u"), "09");
    TEST_EQUAL(format_str('\t', "v"), "0009");
    TEST_EQUAL(format_str('\t', "x"), "9");

    TEST_EQUAL(format_str('A'), "A");
    TEST_EQUAL(format_str('A', "e"), "A");
    TEST_EQUAL(format_str('A', "a"), "A");
    TEST_EQUAL(format_str('A', "q"), "\"A\"");
    TEST_EQUAL(format_str('A', "Q"), "\"A\"");
    TEST_EQUAL(format_str('A', "n"), "65");
    TEST_EQUAL(format_str('A', "u"), "41");
    TEST_EQUAL(format_str('A', "v"), "0041");
    TEST_EQUAL(format_str('A', "x"), "41");

    TEST_EQUAL(format_str(u'\t'), "\t");
    TEST_EQUAL(format_str(u'\t', "e"), "\\t");
    TEST_EQUAL(format_str(u'\t', "a"), "\\t");
    TEST_EQUAL(format_str(u'\t', "q"), "\"\\t\"");
    TEST_EQUAL(format_str(u'\t', "Q"), "\"\\t\"");
    TEST_EQUAL(format_str(u'\t', "n"), "9");
    TEST_EQUAL(format_str(u'\t', "u"), "09");
    TEST_EQUAL(format_str(u'\t', "v"), "0009");
    TEST_EQUAL(format_str(u'\t', "x"), "9");

    TEST_EQUAL(format_str(u'A'), "A");
    TEST_EQUAL(format_str(u'A', "e"), "A");
    TEST_EQUAL(format_str(u'A', "a"), "A");
    TEST_EQUAL(format_str(u'A', "q"), "\"A\"");
    TEST_EQUAL(format_str(u'A', "Q"), "\"A\"");
    TEST_EQUAL(format_str(u'A', "n"), "65");
    TEST_EQUAL(format_str(u'A', "u"), "41");
    TEST_EQUAL(format_str(u'A', "v"), "0041");
    TEST_EQUAL(format_str(u'A', "x"), "41");

    TEST_EQUAL(format_str(u'€'), "€");
    TEST_EQUAL(format_str(u'€', "e"), "€");
    TEST_EQUAL(format_str(u'€', "a"), "\\x{20ac}");
    TEST_EQUAL(format_str(u'€', "q"), "\"€\"");
    TEST_EQUAL(format_str(u'€', "Q"), "\"\\x{20ac}\"");
    TEST_EQUAL(format_str(u'€', "n"), "8364");
    TEST_EQUAL(format_str(u'€', "u"), "e2 82 ac");
    TEST_EQUAL(format_str(u'€', "v"), "20ac");
    TEST_EQUAL(format_str(u'€', "x"), "20ac");

    TEST_EQUAL(format_str(U'\t'), "\t");
    TEST_EQUAL(format_str(U'\t', "e"), "\\t");
    TEST_EQUAL(format_str(U'\t', "a"), "\\t");
    TEST_EQUAL(format_str(U'\t', "q"), "\"\\t\"");
    TEST_EQUAL(format_str(U'\t', "Q"), "\"\\t\"");
    TEST_EQUAL(format_str(U'\t', "n"), "9");
    TEST_EQUAL(format_str(U'\t', "u"), "09");
    TEST_EQUAL(format_str(U'\t', "v"), "0009");
    TEST_EQUAL(format_str(U'\t', "x"), "9");

    TEST_EQUAL(format_str(U'A'), "A");
    TEST_EQUAL(format_str(U'A', "e"), "A");
    TEST_EQUAL(format_str(U'A', "a"), "A");
    TEST_EQUAL(format_str(U'A', "q"), "\"A\"");
    TEST_EQUAL(format_str(U'A', "Q"), "\"A\"");
    TEST_EQUAL(format_str(U'A', "n"), "65");
    TEST_EQUAL(format_str(U'A', "u"), "41");
    TEST_EQUAL(format_str(U'A', "v"), "0041");
    TEST_EQUAL(format_str(U'A', "x"), "41");

    TEST_EQUAL(format_str(U'€'), "€");
    TEST_EQUAL(format_str(U'€', "e"), "€");
    TEST_EQUAL(format_str(U'€', "a"), "\\x{20ac}");
    TEST_EQUAL(format_str(U'€', "q"), "\"€\"");
    TEST_EQUAL(format_str(U'€', "Q"), "\"\\x{20ac}\"");
    TEST_EQUAL(format_str(U'€', "n"), "8364");
    TEST_EQUAL(format_str(U'€', "u"), "e2 82 ac");
    TEST_EQUAL(format_str(U'€', "v"), "20ac");
    TEST_EQUAL(format_str(U'€', "x"), "20ac");

    TEST_EQUAL(format_str(char32_t(0x10fffd)), "\xf4\x8f\xbf\xbd");
    TEST_EQUAL(format_str(char32_t(0x10fffd), "e"), "\xf4\x8f\xbf\xbd");
    TEST_EQUAL(format_str(char32_t(0x10fffd), "a"), "\\x{10fffd}");
    TEST_EQUAL(format_str(char32_t(0x10fffd), "q"), "\"\xf4\x8f\xbf\xbd\"");
    TEST_EQUAL(format_str(char32_t(0x10fffd), "Q"), "\"\\x{10fffd}\"");
    TEST_EQUAL(format_str(char32_t(0x10fffd), "n"), "1114109");
    TEST_EQUAL(format_str(char32_t(0x10fffd), "u"), "f4 8f bf bd");
    TEST_EQUAL(format_str(char32_t(0x10fffd), "v"), "dbff dffd");
    TEST_EQUAL(format_str(char32_t(0x10fffd), "x"), "10fffd");

}

void test_unicorn_format_strings() {

    // e, escape    = Escape if C0/C1 control
    // a, ascii     = Escape if not printable ASCII
    // q, quote     = Quote string, escape C0/C1
    // Q, ascquote  = Quote string, escape non-ASCII
    // d, decimal   = Decimal number
    // x, hex       = Hexadecimal number
    // u, hex8      = Hex UTF-8 bytes
    // v, hex16     = Hex UTF-16 code units

    TEST_EQUAL(format_str(""s), "");
    TEST_EQUAL(format_str(""s, "e"), "");
    TEST_EQUAL(format_str(""s, "a"), "");
    TEST_EQUAL(format_str(""s, "q"), "\"\"");
    TEST_EQUAL(format_str(""s, "Q"), "\"\"");
    TEST_EQUAL(format_str(""s, "n"), "");
    TEST_EQUAL(format_str(""s, "u"), "");
    TEST_EQUAL(format_str(""s, "v"), "");
    TEST_EQUAL(format_str(""s, "x"), "");

    TEST_EQUAL(format_str("Hello"s), "Hello");
    TEST_EQUAL(format_str("Hello"s, "e"), "Hello");
    TEST_EQUAL(format_str("Hello"s, "a"), "Hello");
    TEST_EQUAL(format_str("Hello"s, "q"), "\"Hello\"");
    TEST_EQUAL(format_str("Hello"s, "Q"), "\"Hello\"");
    TEST_EQUAL(format_str("Hello"s, "n"), "72 101 108 108 111");
    TEST_EQUAL(format_str("Hello"s, "u"), "48 65 6c 6c 6f");
    TEST_EQUAL(format_str("Hello"s, "v"), "0048 0065 006c 006c 006f");
    TEST_EQUAL(format_str("Hello"s, "x"), "48 65 6c 6c 6f");

    TEST_EQUAL(format_str("§€ αβγ\n"s), "§€ αβγ\n");
    TEST_EQUAL(format_str("§€ αβγ\n"s, "e"), "§€ αβγ\\n");
    TEST_EQUAL(format_str("§€ αβγ\n"s, "a"), "\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
    TEST_EQUAL(format_str("§€ αβγ\n"s, "q"), "\"§€ αβγ\\n\"");
    TEST_EQUAL(format_str("§€ αβγ\n"s, "Q"), "\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
    TEST_EQUAL(format_str("§€ αβγ\n"s, "n"), "167 8364 32 945 946 947 10");
    TEST_EQUAL(format_str("§€ αβγ\n"s, "u"), "c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
    TEST_EQUAL(format_str("§€ αβγ\n"s, "v"), "00a7 20ac 0020 03b1 03b2 03b3 000a");
    TEST_EQUAL(format_str("§€ αβγ\n"s, "x"), "a7 20ac 20 3b1 3b2 3b3 a");

    TEST_EQUAL(format_str(u""s), "");
    TEST_EQUAL(format_str(u""s, "e"), "");
    TEST_EQUAL(format_str(u""s, "a"), "");
    TEST_EQUAL(format_str(u""s, "q"), "\"\"");
    TEST_EQUAL(format_str(u""s, "Q"), "\"\"");
    TEST_EQUAL(format_str(u""s, "n"), "");
    TEST_EQUAL(format_str(u""s, "u"), "");
    TEST_EQUAL(format_str(u""s, "v"), "");
    TEST_EQUAL(format_str(u""s, "x"), "");

    TEST_EQUAL(format_str(u"Hello"s), "Hello");
    TEST_EQUAL(format_str(u"Hello"s, "e"), "Hello");
    TEST_EQUAL(format_str(u"Hello"s, "a"), "Hello");
    TEST_EQUAL(format_str(u"Hello"s, "q"), "\"Hello\"");
    TEST_EQUAL(format_str(u"Hello"s, "Q"), "\"Hello\"");
    TEST_EQUAL(format_str(u"Hello"s, "n"), "72 101 108 108 111");
    TEST_EQUAL(format_str(u"Hello"s, "u"), "48 65 6c 6c 6f");
    TEST_EQUAL(format_str(u"Hello"s, "v"), "0048 0065 006c 006c 006f");
    TEST_EQUAL(format_str(u"Hello"s, "x"), "48 65 6c 6c 6f");

    TEST_EQUAL(format_str(u"§€ αβγ\n"s), "§€ αβγ\n");
    TEST_EQUAL(format_str(u"§€ αβγ\n"s, "e"), "§€ αβγ\\n");
    TEST_EQUAL(format_str(u"§€ αβγ\n"s, "a"), "\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
    TEST_EQUAL(format_str(u"§€ αβγ\n"s, "q"), "\"§€ αβγ\\n\"");
    TEST_EQUAL(format_str(u"§€ αβγ\n"s, "Q"), "\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
    TEST_EQUAL(format_str(u"§€ αβγ\n"s, "n"), "167 8364 32 945 946 947 10");
    TEST_EQUAL(format_str(u"§€ αβγ\n"s, "u"), "c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
    TEST_EQUAL(format_str(u"§€ αβγ\n"s, "v"), "00a7 20ac 0020 03b1 03b2 03b3 000a");
    TEST_EQUAL(format_str(u"§€ αβγ\n"s, "x"), "a7 20ac 20 3b1 3b2 3b3 a");

    TEST_EQUAL(format_str(U""s), "");
    TEST_EQUAL(format_str(U""s, "e"), "");
    TEST_EQUAL(format_str(U""s, "a"), "");
    TEST_EQUAL(format_str(U""s, "q"), "\"\"");
    TEST_EQUAL(format_str(U""s, "Q"), "\"\"");
    TEST_EQUAL(format_str(U""s, "n"), "");
    TEST_EQUAL(format_str(U""s, "u"), "");
    TEST_EQUAL(format_str(U""s, "v"), "");
    TEST_EQUAL(format_str(U""s, "x"), "");

    TEST_EQUAL(format_str(U"Hello"s), "Hello");
    TEST_EQUAL(format_str(U"Hello"s, "e"), "Hello");
    TEST_EQUAL(format_str(U"Hello"s, "a"), "Hello");
    TEST_EQUAL(format_str(U"Hello"s, "q"), "\"Hello\"");
    TEST_EQUAL(format_str(U"Hello"s, "Q"), "\"Hello\"");
    TEST_EQUAL(format_str(U"Hello"s, "n"), "72 101 108 108 111");
    TEST_EQUAL(format_str(U"Hello"s, "u"), "48 65 6c 6c 6f");
    TEST_EQUAL(format_str(U"Hello"s, "v"), "0048 0065 006c 006c 006f");
    TEST_EQUAL(format_str(U"Hello"s, "x"), "48 65 6c 6c 6f");

    TEST_EQUAL(format_str(U"§€ αβγ\n"s), "§€ αβγ\n");
    TEST_EQUAL(format_str(U"§€ αβγ\n"s, "e"), "§€ αβγ\\n");
    TEST_EQUAL(format_str(U"§€ αβγ\n"s, "a"), "\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n");
    TEST_EQUAL(format_str(U"§€ αβγ\n"s, "q"), "\"§€ αβγ\\n\"");
    TEST_EQUAL(format_str(U"§€ αβγ\n"s, "Q"), "\"\\xa7\\x{20ac} \\x{3b1}\\x{3b2}\\x{3b3}\\n\"");
    TEST_EQUAL(format_str(U"§€ αβγ\n"s, "n"), "167 8364 32 945 946 947 10");
    TEST_EQUAL(format_str(U"§€ αβγ\n"s, "u"), "c2 a7 e2 82 ac 20 ce b1 ce b2 ce b3 0a");
    TEST_EQUAL(format_str(U"§€ αβγ\n"s, "v"), "00a7 20ac 0020 03b1 03b2 03b3 000a");
    TEST_EQUAL(format_str(U"§€ αβγ\n"s, "x"), "a7 20ac 20 3b1 3b2 3b3 a");

    TEST_EQUAL(format_str("Hello"sv), "Hello");
    TEST_EQUAL(format_str("αβγδε"sv), "αβγδε");
    TEST_EQUAL(format_str(u"Hello"sv), "Hello");
    TEST_EQUAL(format_str(u"αβγδε"sv), "αβγδε");
    TEST_EQUAL(format_str(U"Hello"sv), "Hello");
    TEST_EQUAL(format_str(U"αβγδε"sv), "αβγδε");
    TEST_EQUAL(format_str(L"Hello"sv), "Hello");
    TEST_EQUAL(format_str(L"αβγδε"sv), "αβγδε");

}

void test_unicorn_format_date_and_time() {

    system_clock::time_point ldate, udate;

    TRY(udate = make_date(2000, 1, 2, 3, 4, 5.678));
    TRY(ldate = make_date(2000, 1, 2, 3, 4, 5.678, local_zone));

    TEST_EQUAL(format_str(udate), "2000-01-02 03:04:05");
    TEST_EQUAL(format_str(udate, "3"), "2000-01-02 03:04:05.678");
    TEST_EQUAL(format_str(ldate, "l"), "2000-01-02 03:04:05");
    TEST_EQUAL(format_str(ldate, "l3"), "2000-01-02 03:04:05.678");
    TEST_EQUAL(format_str(udate, "t"), "2000-01-02T03:04:05");
    TEST_EQUAL(format_str(udate, "t3"), "2000-01-02T03:04:05.678");
    TEST_EQUAL(format_str(ldate, "tl"), "2000-01-02T03:04:05");
    TEST_EQUAL(format_str(ldate, "tl3"), "2000-01-02T03:04:05.678");
    TEST_THROW(format_str(udate, "tc"), std::invalid_argument);

    system_clock::duration t;

    TRY(from_seconds(0, t));           TEST_EQUAL(format_str(t), "0s");
    TRY(from_seconds(0, t));           TEST_EQUAL(format_str(t, "3"), "0.000s");
    TRY(from_seconds(0.25, t));        TEST_EQUAL(format_str(t, "3"), "0.250s");
    TRY(from_seconds(1, t));           TEST_EQUAL(format_str(t, "3"), "1.000s");
    TRY(from_seconds(1234, t));        TEST_EQUAL(format_str(t, "3"), "20m34.000s");
    TRY(from_seconds(12345, t));       TEST_EQUAL(format_str(t, "3"), "3h25m45.000s");
    TRY(from_seconds(123456, t));      TEST_EQUAL(format_str(t, "3"), "1d10h17m36.000s");
    TRY(from_seconds(1234567, t));     TEST_EQUAL(format_str(t, "3"), "14d06h56m07.000s");
    TRY(from_seconds(12345678, t));    TEST_EQUAL(format_str(t, "3"), "142d21h21m18.000s");
    TRY(from_seconds(123456789, t));   TEST_EQUAL(format_str(t, "3"), "1428d21h33m09.000s");
    TRY(from_seconds(-0.25, t));       TEST_EQUAL(format_str(t, "3"), "-0.250s");
    TRY(from_seconds(-1, t));          TEST_EQUAL(format_str(t, "3"), "-1.000s");
    TRY(from_seconds(-1234, t));       TEST_EQUAL(format_str(t, "3"), "-20m34.000s");
    TRY(from_seconds(-12345, t));      TEST_EQUAL(format_str(t, "3"), "-3h25m45.000s");
    TRY(from_seconds(-123456, t));     TEST_EQUAL(format_str(t, "3"), "-1d10h17m36.000s");
    TRY(from_seconds(-1234567, t));    TEST_EQUAL(format_str(t, "3"), "-14d06h56m07.000s");
    TRY(from_seconds(-12345678, t));   TEST_EQUAL(format_str(t, "3"), "-142d21h21m18.000s");
    TRY(from_seconds(-123456789, t));  TEST_EQUAL(format_str(t, "3"), "-1428d21h33m09.000s");

}

void test_unicorn_format_ranges() {

    std::vector<int> iv0, iv1 {10, 20, 30, 40, 50};
    Strings sv0, sv1 {"hello", "world", "goodbye"};
    std::map<int, Ustring> ism0, ism1 {{10, "hello"}, {20, "world"}, {30, "goodbye"}};

    TEST_EQUAL(format_str(iv0), "");
    TEST_EQUAL(format_str(iv1), "10,20,30,40,50");
    TEST_EQUAL(format_str(iv1, "x4"), "000a,0014,001e,0028,0032");
    TEST_EQUAL(format_str(sv0), "");
    TEST_EQUAL(format_str(sv1), "hello,world,goodbye");
    TEST_EQUAL(format_str(sv1, "U"), "HELLO,WORLD,GOODBYE");
    TEST_EQUAL(format_str(ism0), "");
    TEST_EQUAL(format_str(ism1), "10:hello,20:world,30:goodbye");
    TEST_EQUAL(format_str(ism1, "U4"), "0010:HELLO,0020:WORLD,0030:GOODBYE");

}

void test_unicorn_format_alignment_and_padding() {

    // <, left    = Left align
    // =, centre  = Centre align
    // >, right   = Right align

    TEST_EQUAL(format_str(42, Format::left, 0, 1u), "42");
    TEST_EQUAL(format_str(42, Format::right, 0, 1u), "42");
    TEST_EQUAL(format_str(42, Format::centre, 0, 1u), "42");
    TEST_EQUAL(format_str(42, Format::left, 0, 5u), "42   ");
    TEST_EQUAL(format_str(42, Format::right, 0, 5u), "   42");
    TEST_EQUAL(format_str(42, Format::centre, 0, 8u), "   42   ");
    TEST_EQUAL(format_str(42, Format::centre, 0, 9u), "   42    ");
    TEST_EQUAL(format_str(42, Format::left, 0, 5u, U'*'), "42***");
    TEST_EQUAL(format_str(42, Format::right, 0, 5u, U'*'), "***42");
    TEST_EQUAL(format_str(42, Format::centre, 0, 8u, U'*'), "***42***");

    TEST_EQUAL(format_str(42, "<"), "42");
    TEST_EQUAL(format_str(42, ">"), "42");
    TEST_EQUAL(format_str(42, "="), "42");
    TEST_EQUAL(format_str(42, "<5"), "42   ");
    TEST_EQUAL(format_str(42, ">5"), "   42");
    TEST_EQUAL(format_str(42, "=8"), "   42   ");
    TEST_EQUAL(format_str(42, "<*5"), "42***");
    TEST_EQUAL(format_str(42, ">*5"), "***42");
    TEST_EQUAL(format_str(42, "=*8"), "***42***");

}

void test_unicorn_format_case_mapping() {

    // L, lower  = Convert to lower case
    // T, title  = Convert to title case
    // U, upper  = Convert to upper case

    TEST_EQUAL(format_str("hello WORLD"s, "L"), "hello world"s);
    TEST_EQUAL(format_str("hello WORLD"s, "T"), "Hello World"s);
    TEST_EQUAL(format_str("hello WORLD"s, "U"), "HELLO WORLD"s);

}

void test_unicorn_format_class() {

    TEST_EQUAL(Format("")(), "");
    TEST_EQUAL(Format("Hello world")(), "Hello world");
    TEST_EQUAL(Format("Hello $$world")(), "Hello $world");
    TEST_EQUAL(Format("Hello $1")("world"), "Hello world");
    TEST_EQUAL(Format("Hello $1")("world", 42), "Hello world");
    TEST_EQUAL(Format("Hello $2")("world", 42), "Hello 42");
    TEST_EQUAL(Format("Hello $3")("world", 42), "Hello ");
    TEST_EQUAL(Format("Hello $1 $2")("world", 42), "Hello world 42");
    TEST_EQUAL(Format("Hello $2 $1")("world", 42), "Hello 42 world");
    TEST_EQUAL(Format("Hello ${1}")("world"), "Hello world");
    TEST_EQUAL(Format("Hello ${1}")("world", 42), "Hello world");
    TEST_EQUAL(Format("Hello ${2}")("world", 42), "Hello 42");
    TEST_EQUAL(Format("Hello ${3}")("world", 42), "Hello ");
    TEST_EQUAL(Format("Hello ${1} ${2}")("world", 42), "Hello world 42");
    TEST_EQUAL(Format("Hello ${2} ${1}")("world", 42), "Hello 42 world");
    TEST_EQUAL(Format("Hello ${1U}")("world"), "Hello WORLD");
    TEST_EQUAL(Format("Hello ${1<8}")("world"), "Hello world   ");
    TEST_EQUAL(Format("Hello ${1U<8}")("world"), "Hello WORLD   ");
    TEST_EQUAL(Format("Hello ${1 4}")(42), "Hello 0042");
    TEST_EQUAL(Format("Hello ${1x}")(42), "Hello 2a");
    TEST_EQUAL(Format("Hello ${1x4}")(42), "Hello 002a");
    TEST_EQUAL(Format("Hello $1x")(42), "Hello 2a");
    TEST_EQUAL(Format("Hello $1x4")(42), "Hello 002a");
    TEST_EQUAL(Format("Hello $1")(u"world"), "Hello world");
    TEST_EQUAL(Format("Hello $1")(U"world"), "Hello world");
    TEST_EQUAL(Format("Hello $1")(L"world"), "Hello world");

}

void test_unicorn_format_literals() {

    TEST_EQUAL(""_fmt(), "");
    TEST_EQUAL("Hello world"_fmt(), "Hello world");
    TEST_EQUAL("Hello $1"_fmt("world"), "Hello world");

}
