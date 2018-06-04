#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <limits>
#include <stdexcept>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_conversion_decimal_integers() {

    Ustring s;
    int8_t i8;
    uint8_t u8;
    int16_t i16;
    uint16_t u16;
    int32_t i32;
    uint32_t u32;
    int64_t i64;
    uint64_t u64;

    s = "42xyz";   TEST_EQUAL(str_to_int(i8, s), 2);   TEST_EQUAL(i8, 42);
    s = "-42xyz";  TEST_EQUAL(str_to_int(i8, s), 3);   TEST_EQUAL(i8, -42);
    s = "xyz";     TEST_EQUAL(str_to_int(i8, s), 0);   TEST_EQUAL(i8, 0);
    s = "42xyz";   TEST_EQUAL(str_to_int(u8, s), 2);   TEST_EQUAL(u8, 42);
    s = "xyz";     TEST_EQUAL(str_to_int(u8, s), 0);   TEST_EQUAL(u8, 0);
    s = "42xyz";   TEST_EQUAL(str_to_int(i16, s), 2);  TEST_EQUAL(i16, 42);
    s = "-42xyz";  TEST_EQUAL(str_to_int(i16, s), 3);  TEST_EQUAL(i16, -42);
    s = "xyz";     TEST_EQUAL(str_to_int(i16, s), 0);  TEST_EQUAL(i16, 0);
    s = "42xyz";   TEST_EQUAL(str_to_int(u16, s), 2);  TEST_EQUAL(u16, 42);
    s = "xyz";     TEST_EQUAL(str_to_int(u16, s), 0);  TEST_EQUAL(u16, 0);
    s = "42xyz";   TEST_EQUAL(str_to_int(i32, s), 2);  TEST_EQUAL(i32, 42);
    s = "-42xyz";  TEST_EQUAL(str_to_int(i32, s), 3);  TEST_EQUAL(i32, -42);
    s = "xyz";     TEST_EQUAL(str_to_int(i32, s), 0);  TEST_EQUAL(i32, 0);
    s = "42xyz";   TEST_EQUAL(str_to_int(u32, s), 2);  TEST_EQUAL(u32, 42);
    s = "xyz";     TEST_EQUAL(str_to_int(u32, s), 0);  TEST_EQUAL(u32, 0);
    s = "42xyz";   TEST_EQUAL(str_to_int(i64, s), 2);  TEST_EQUAL(i64, 42);
    s = "-42xyz";  TEST_EQUAL(str_to_int(i64, s), 3);  TEST_EQUAL(i64, -42);
    s = "xyz";     TEST_EQUAL(str_to_int(i64, s), 0);  TEST_EQUAL(i64, 0);
    s = "42xyz";   TEST_EQUAL(str_to_int(u64, s), 2);  TEST_EQUAL(u64, 42);
    s = "xyz";     TEST_EQUAL(str_to_int(u64, s), 0);  TEST_EQUAL(u64, 0);

    TEST_EQUAL(str_to_int<int8_t>("42xyz"s), 42);
    TEST_EQUAL(str_to_int<int8_t>("-42xyz"s), -42);
    TEST_EQUAL(str_to_int<int8_t>("xyz"s), 0);
    TEST_EQUAL(str_to_int<uint8_t>("42xyz"s), 42);
    TEST_EQUAL(str_to_int<uint8_t>("xyz"s), 0);
    TEST_EQUAL(str_to_int<int16_t>("42xyz"s), 42);
    TEST_EQUAL(str_to_int<int16_t>("-42xyz"s), -42);
    TEST_EQUAL(str_to_int<int16_t>("xyz"s), 0);
    TEST_EQUAL(str_to_int<uint16_t>("42xyz"s), 42);
    TEST_EQUAL(str_to_int<uint16_t>("xyz"s), 0);
    TEST_EQUAL(str_to_int<int32_t>("42xyz"s), 42);
    TEST_EQUAL(str_to_int<int32_t>("-42xyz"s), -42);
    TEST_EQUAL(str_to_int<int32_t>("xyz"s), 0);
    TEST_EQUAL(str_to_int<uint32_t>("42xyz"s), 42);
    TEST_EQUAL(str_to_int<uint32_t>("xyz"s), 0);
    TEST_EQUAL(str_to_int<int64_t>("42xyz"s), 42);
    TEST_EQUAL(str_to_int<int64_t>("-42xyz"s), -42);
    TEST_EQUAL(str_to_int<int64_t>("xyz"s), 0);
    TEST_EQUAL(str_to_int<uint64_t>("42xyz"s), 42);
    TEST_EQUAL(str_to_int<uint64_t>("xyz"s), 0);

    TEST_EQUAL(str_to_int<int8_t>("126"s), std::numeric_limits<int8_t>::max() - 1);
    TEST_EQUAL(str_to_int<int8_t>("127"s), std::numeric_limits<int8_t>::max());
    TEST_EQUAL(str_to_int<int8_t>("128"s), std::numeric_limits<int8_t>::max());
    TEST_EQUAL(str_to_int<int8_t>("-127"s), std::numeric_limits<int8_t>::min() + 1);
    TEST_EQUAL(str_to_int<int8_t>("-128"s), std::numeric_limits<int8_t>::min());
    TEST_EQUAL(str_to_int<int8_t>("-129"s), std::numeric_limits<int8_t>::min());
    TEST_EQUAL(str_to_int<uint8_t>("254"s), std::numeric_limits<uint8_t>::max() - 1);
    TEST_EQUAL(str_to_int<uint8_t>("255"s), std::numeric_limits<uint8_t>::max());
    TEST_EQUAL(str_to_int<uint8_t>("256"s), std::numeric_limits<uint8_t>::max());

    TEST_EQUAL(str_to_int<int16_t>("32766"s), std::numeric_limits<int16_t>::max() - 1);
    TEST_EQUAL(str_to_int<int16_t>("32767"s), std::numeric_limits<int16_t>::max());
    TEST_EQUAL(str_to_int<int16_t>("32768"s), std::numeric_limits<int16_t>::max());
    TEST_EQUAL(str_to_int<int16_t>("-32767"s), std::numeric_limits<int16_t>::min() + 1);
    TEST_EQUAL(str_to_int<int16_t>("-32768"s), std::numeric_limits<int16_t>::min());
    TEST_EQUAL(str_to_int<int16_t>("-32769"s), std::numeric_limits<int16_t>::min());
    TEST_EQUAL(str_to_int<uint16_t>("65534"s), std::numeric_limits<uint16_t>::max() - 1);
    TEST_EQUAL(str_to_int<uint16_t>("65535"s), std::numeric_limits<uint16_t>::max());
    TEST_EQUAL(str_to_int<uint16_t>("65536"s), std::numeric_limits<uint16_t>::max());

    TEST_EQUAL(str_to_int<int32_t>("2147483646"s), std::numeric_limits<int32_t>::max() - 1);
    TEST_EQUAL(str_to_int<int32_t>("2147483647"s), std::numeric_limits<int32_t>::max());
    TEST_EQUAL(str_to_int<int32_t>("2147483648"s), std::numeric_limits<int32_t>::max());
    TEST_EQUAL(str_to_int<int32_t>("-2147483647"s), std::numeric_limits<int32_t>::min() + 1);
    TEST_EQUAL(str_to_int<int32_t>("-2147483648"s), std::numeric_limits<int32_t>::min());
    TEST_EQUAL(str_to_int<int32_t>("-2147483649"s), std::numeric_limits<int32_t>::min());
    TEST_EQUAL(str_to_int<uint32_t>("4294967294"s), std::numeric_limits<uint32_t>::max() - 1);
    TEST_EQUAL(str_to_int<uint32_t>("4294967295"s), std::numeric_limits<uint32_t>::max());
    TEST_EQUAL(str_to_int<uint32_t>("4294967296"s), std::numeric_limits<uint32_t>::max());

    TEST_EQUAL(str_to_int<int64_t>("9223372036854775806"s), std::numeric_limits<int64_t>::max() - 1);
    TEST_EQUAL(str_to_int<int64_t>("9223372036854775807"s), std::numeric_limits<int64_t>::max());
    TEST_EQUAL(str_to_int<int64_t>("9223372036854775808"s), std::numeric_limits<int64_t>::max());
    TEST_EQUAL(str_to_int<int64_t>("-9223372036854775807"s), std::numeric_limits<int64_t>::min() + 1);
    TEST_EQUAL(str_to_int<int64_t>("-9223372036854775808"s), std::numeric_limits<int64_t>::min());
    TEST_EQUAL(str_to_int<int64_t>("-9223372036854775809"s), std::numeric_limits<int64_t>::min());
    TEST_EQUAL(str_to_int<uint64_t>("18446744073709551614"s), std::numeric_limits<uint64_t>::max() - 1);
    TEST_EQUAL(str_to_int<uint64_t>("18446744073709551615"s), std::numeric_limits<uint64_t>::max());
    TEST_EQUAL(str_to_int<uint64_t>("18446744073709551616"s), std::numeric_limits<uint64_t>::max());

    TEST_THROW(str_to_int<int8_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<int8_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<int8_t>("128"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<int8_t>("-129"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<int8_t>("42xyz"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint8_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint8_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint8_t>("256"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<uint8_t>("42xyz"s, Utf::throws), std::invalid_argument);

    TEST_THROW(str_to_int<int16_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<int16_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<int16_t>("32768"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<int16_t>("-32769"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<int16_t>("42xyz"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint16_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint16_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint16_t>("65536"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<uint16_t>("42xyz"s, Utf::throws), std::invalid_argument);

    TEST_THROW(str_to_int<int32_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<int32_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<int32_t>("2147483648"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<int32_t>("-2147483649"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<int32_t>("42xyz"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint32_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint32_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint32_t>("4294967296"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<uint32_t>("42xyz"s, Utf::throws), std::invalid_argument);

    TEST_THROW(str_to_int<int64_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<int64_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<int64_t>("9223372036854775808"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<int64_t>("-9223372036854775809"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<int64_t>("42xyz"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint64_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint64_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_int<uint64_t>("18446744073709551616"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_int<uint64_t>("42xyz"s, Utf::throws), std::invalid_argument);

}

void test_unicorn_string_conversion_hexadecimal_integers() {

    Ustring s = "ffxyz";
    int8_t i8;
    uint8_t u8;
    int16_t i16;
    uint16_t u16;
    int32_t i32;
    uint32_t u32;
    int64_t i64;
    uint64_t u64;

    TEST_EQUAL(hex_to_int(i8, s), 2);   TEST_EQUAL(i8, 127);
    TEST_EQUAL(hex_to_int(u8, s), 2);   TEST_EQUAL(u8, 255);
    TEST_EQUAL(hex_to_int(i16, s), 2);  TEST_EQUAL(i16, 255);
    TEST_EQUAL(hex_to_int(u16, s), 2);  TEST_EQUAL(u16, 255);
    TEST_EQUAL(hex_to_int(i32, s), 2);  TEST_EQUAL(i32, 255);
    TEST_EQUAL(hex_to_int(u32, s), 2);  TEST_EQUAL(u32, 255);
    TEST_EQUAL(hex_to_int(i64, s), 2);  TEST_EQUAL(i64, 255);
    TEST_EQUAL(hex_to_int(u64, s), 2);  TEST_EQUAL(u64, 255);

    TEST_EQUAL(hex_to_int<int8_t>("ffxyz"s), 127);
    TEST_EQUAL(hex_to_int<uint8_t>("ffxyz"s), 255);
    TEST_EQUAL(hex_to_int<int16_t>("ffxyz"s), 255);
    TEST_EQUAL(hex_to_int<uint16_t>("ffxyz"s), 255);
    TEST_EQUAL(hex_to_int<int32_t>("ffxyz"s), 255);
    TEST_EQUAL(hex_to_int<uint32_t>("ffxyz"s), 255);
    TEST_EQUAL(hex_to_int<int64_t>("ffxyz"s), 255);
    TEST_EQUAL(hex_to_int<uint64_t>("ffxyz"s), 255);

    TEST_EQUAL(hex_to_int<int8_t>("7e"s), std::numeric_limits<int8_t>::max() - 1);
    TEST_EQUAL(hex_to_int<int8_t>("7f"s), std::numeric_limits<int8_t>::max());
    TEST_EQUAL(hex_to_int<int8_t>("80"s), std::numeric_limits<int8_t>::max());
    TEST_EQUAL(hex_to_int<int8_t>("-7f"s), std::numeric_limits<int8_t>::min() + 1);
    TEST_EQUAL(hex_to_int<int8_t>("-80"s), std::numeric_limits<int8_t>::min());
    TEST_EQUAL(hex_to_int<int8_t>("-81"s), std::numeric_limits<int8_t>::min());
    TEST_EQUAL(hex_to_int<uint8_t>("fe"s), std::numeric_limits<uint8_t>::max() - 1);
    TEST_EQUAL(hex_to_int<uint8_t>("ff"s), std::numeric_limits<uint8_t>::max());
    TEST_EQUAL(hex_to_int<uint8_t>("100"s), std::numeric_limits<uint8_t>::max());

    TEST_EQUAL(hex_to_int<int16_t>("7ffe"s), std::numeric_limits<int16_t>::max() - 1);
    TEST_EQUAL(hex_to_int<int16_t>("7fff"s), std::numeric_limits<int16_t>::max());
    TEST_EQUAL(hex_to_int<int16_t>("8000"s), std::numeric_limits<int16_t>::max());
    TEST_EQUAL(hex_to_int<int16_t>("-7fff"s), std::numeric_limits<int16_t>::min() + 1);
    TEST_EQUAL(hex_to_int<int16_t>("-8000"s), std::numeric_limits<int16_t>::min());
    TEST_EQUAL(hex_to_int<int16_t>("-8001"s), std::numeric_limits<int16_t>::min());
    TEST_EQUAL(hex_to_int<uint16_t>("fffe"s), std::numeric_limits<uint16_t>::max() - 1);
    TEST_EQUAL(hex_to_int<uint16_t>("ffff"s), std::numeric_limits<uint16_t>::max());
    TEST_EQUAL(hex_to_int<uint16_t>("10000"s), std::numeric_limits<uint16_t>::max());

    TEST_EQUAL(hex_to_int<int32_t>("7ffffffe"s), std::numeric_limits<int32_t>::max() - 1);
    TEST_EQUAL(hex_to_int<int32_t>("7fffffff"s), std::numeric_limits<int32_t>::max());
    TEST_EQUAL(hex_to_int<int32_t>("80000000"s), std::numeric_limits<int32_t>::max());
    TEST_EQUAL(hex_to_int<int32_t>("-7fffffff"s), std::numeric_limits<int32_t>::min() + 1);
    TEST_EQUAL(hex_to_int<int32_t>("-80000000"s), std::numeric_limits<int32_t>::min());
    TEST_EQUAL(hex_to_int<int32_t>("-80000001"s), std::numeric_limits<int32_t>::min());
    TEST_EQUAL(hex_to_int<uint32_t>("fffffffe"s), std::numeric_limits<uint32_t>::max() - 1);
    TEST_EQUAL(hex_to_int<uint32_t>("ffffffff"s), std::numeric_limits<uint32_t>::max());
    TEST_EQUAL(hex_to_int<uint32_t>("100000000"s), std::numeric_limits<uint32_t>::max());

    TEST_EQUAL(hex_to_int<int64_t>("7ffffffffffffffe"s), std::numeric_limits<int64_t>::max() - 1);
    TEST_EQUAL(hex_to_int<int64_t>("7fffffffffffffff"s), std::numeric_limits<int64_t>::max());
    TEST_EQUAL(hex_to_int<int64_t>("8000000000000000"s), std::numeric_limits<int64_t>::max());
    TEST_EQUAL(hex_to_int<int64_t>("-7fffffffffffffff"s), std::numeric_limits<int64_t>::min() + 1);
    TEST_EQUAL(hex_to_int<int64_t>("-8000000000000000"s), std::numeric_limits<int64_t>::min());
    TEST_EQUAL(hex_to_int<int64_t>("-8000000000000001"s), std::numeric_limits<int64_t>::min());
    TEST_EQUAL(hex_to_int<uint64_t>("fffffffffffffffe"s), std::numeric_limits<uint64_t>::max() - 1);
    TEST_EQUAL(hex_to_int<uint64_t>("ffffffffffffffff"s), std::numeric_limits<uint64_t>::max());
    TEST_EQUAL(hex_to_int<uint64_t>("10000000000000000"s), std::numeric_limits<uint64_t>::max());

    TEST_THROW(hex_to_int<int8_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<int8_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<int8_t>("80"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<int8_t>("-81"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<int8_t>("42xyz"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint8_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint8_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint8_t>("100"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<uint8_t>("42xyz"s, Utf::throws), std::invalid_argument);

    TEST_THROW(hex_to_int<int16_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<int16_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<int16_t>("8000"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<int16_t>("-8001"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<int16_t>("42xyz"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint16_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint16_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint16_t>("10000"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<uint16_t>("42xyz"s, Utf::throws), std::invalid_argument);

    TEST_THROW(hex_to_int<int32_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<int32_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<int32_t>("80000000"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<int32_t>("-80000001"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<int32_t>("42xyz"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint32_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint32_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint32_t>("100000000"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<uint32_t>("42xyz"s, Utf::throws), std::invalid_argument);

    TEST_THROW(hex_to_int<int64_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<int64_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<int64_t>("8000000000000000"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<int64_t>("-8000000000000001"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<int64_t>("42xyz"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint64_t>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint64_t>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(hex_to_int<uint64_t>("10000000000000000"s, Utf::throws), std::range_error);
    TEST_THROW(hex_to_int<uint64_t>("42xyz"s, Utf::throws), std::invalid_argument);

}

void test_unicorn_string_conversion_floating_point() {

    Ustring s;
    float f;
    double d;
    long double ld;

    s = "42xyz";        TEST_EQUAL(str_to_float(f, s), 2);   TEST_EQUAL(f, 42);
    s = "-6.25e-2xyz";  TEST_EQUAL(str_to_float(f, s), 8);   TEST_EQUAL(f, -0.0625);
    s = "xyz";          TEST_EQUAL(str_to_float(f, s), 0);   TEST_EQUAL(f, 0);
    s = "42xyz";        TEST_EQUAL(str_to_float(d, s), 2);   TEST_EQUAL(d, 42);
    s = "-6.25e-2xyz";  TEST_EQUAL(str_to_float(d, s), 8);   TEST_EQUAL(d, -0.0625);
    s = "xyz";          TEST_EQUAL(str_to_float(d, s), 0);   TEST_EQUAL(d, 0);
    s = "42xyz";        TEST_EQUAL(str_to_float(ld, s), 2);  TEST_EQUAL(ld, 42);
    s = "-6.25e-2xyz";  TEST_EQUAL(str_to_float(ld, s), 8);  TEST_EQUAL(ld, -0.0625);
    s = "xyz";          TEST_EQUAL(str_to_float(ld, s), 0);  TEST_EQUAL(ld, 0);

    TEST_EQUAL(str_to_float<double>(""s), 0);
    TEST_EQUAL(str_to_float<double>("0"s), 0);
    TEST_EQUAL(str_to_float<double>("42"s), 42);
    TEST_EQUAL(str_to_float<double>("42 roads"s), 42);
    TEST_EQUAL(str_to_float<double>("0042"s), 42);
    TEST_EQUAL(str_to_float<double>("+42"s), 42);
    TEST_EQUAL(str_to_float<double>("-42"s), -42);
    TEST_EQUAL(str_to_float<double>("1234.5"s), 1234.5);
    TEST_EQUAL(str_to_float<double>("+1234.5"s), 1234.5);
    TEST_EQUAL(str_to_float<double>("-1234.5"s), -1234.5);
    TEST_EQUAL(str_to_float<double>("1.23e4"s), 12300);
    TEST_EQUAL(str_to_float<double>("+1.23e4"s), 12300);
    TEST_EQUAL(str_to_float<double>("-1.23e4"s), -12300);
    TEST_EQUAL(str_to_float<double>("2.5e-1"s), 0.25);
    TEST_EQUAL(str_to_float<double>("+2.5e-1"s), 0.25);
    TEST_EQUAL(str_to_float<double>("-2.5e-1"s), -0.25);

    TEST_EQUAL(str_to_float<double>(""s), 0);
    TEST_EQUAL(str_to_float<double>("hello"s), 0);
    TEST_EQUAL(str_to_float<double>("1e9999"s), std::numeric_limits<double>::max());
    TEST_EQUAL(str_to_float<double>("-1e9999"s), - std::numeric_limits<double>::max());

    TEST_THROW(str_to_float<double>(""s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_float<double>("hello"s, Utf::throws), std::invalid_argument);
    TEST_THROW(str_to_float<double>("1e9999"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_float<double>("-1e9999"s, Utf::throws), std::range_error);
    TEST_THROW(str_to_float<double>("42xyz"s, Utf::throws), std::invalid_argument);

    s = "123456";     TEST_EQUAL(str_to_float<double>(utf_iterator(s, 3)), 456);
    s = "123456.75";  TEST_EQUAL(str_to_float<double>(utf_iterator(s, 3)), 456.75);
    s = "answer 42";  TEST_EQUAL(str_to_float<double>(utf_iterator(s, 7)), 42);

}
