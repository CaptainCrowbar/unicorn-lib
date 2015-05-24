#include "crow/unit-test.hpp"
#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include <limits>
#include <string>

using namespace std::literals;
using namespace Crow;
using namespace Unicorn;

namespace {

    void check_decimal_integers() {

        int8_t i8;
        uint8_t u8;
        int16_t i16;
        uint16_t u16;
        int32_t i32;
        uint32_t u32;
        int64_t i64;
        uint64_t u64;

        TEST_EQUAL(str_to_integer(i8, "42xyz"s), 2);     TEST_EQUAL(i8, 42);
        TEST_EQUAL(str_to_integer(i8, "-42xyz"s), 3);    TEST_EQUAL(i8, -42);
        TEST_EQUAL(str_to_integer(i8, "xyz"s), 0);       TEST_EQUAL(i8, 0);
        TEST_EQUAL(str_to_integer(u8, "42xyz"s), 2);     TEST_EQUAL(u8, 42);
        TEST_EQUAL(str_to_integer(u8, "xyz"s), 0);       TEST_EQUAL(u8, 0);
        TEST_EQUAL(str_to_integer(i16, "42xyz"s), 2);    TEST_EQUAL(i16, 42);
        TEST_EQUAL(str_to_integer(i16, "-42xyz"s), 3);   TEST_EQUAL(i16, -42);
        TEST_EQUAL(str_to_integer(i16, "xyz"s), 0);      TEST_EQUAL(i16, 0);
        TEST_EQUAL(str_to_integer(u16, "42xyz"s), 2);    TEST_EQUAL(u16, 42);
        TEST_EQUAL(str_to_integer(u16, "xyz"s), 0);      TEST_EQUAL(u16, 0);
        TEST_EQUAL(str_to_integer(i32, "42xyz"s), 2);    TEST_EQUAL(i32, 42);
        TEST_EQUAL(str_to_integer(i32, "-42xyz"s), 3);   TEST_EQUAL(i32, -42);
        TEST_EQUAL(str_to_integer(i32, "xyz"s), 0);      TEST_EQUAL(i32, 0);
        TEST_EQUAL(str_to_integer(u32, "42xyz"s), 2);    TEST_EQUAL(u32, 42);
        TEST_EQUAL(str_to_integer(u32, "xyz"s), 0);      TEST_EQUAL(u32, 0);
        TEST_EQUAL(str_to_integer(i64, "42xyz"s), 2);    TEST_EQUAL(i64, 42);
        TEST_EQUAL(str_to_integer(i64, "-42xyz"s), 3);   TEST_EQUAL(i64, -42);
        TEST_EQUAL(str_to_integer(i64, "xyz"s), 0);      TEST_EQUAL(i64, 0);
        TEST_EQUAL(str_to_integer(u64, "42xyz"s), 2);    TEST_EQUAL(u64, 42);
        TEST_EQUAL(str_to_integer(u64, "xyz"s), 0);      TEST_EQUAL(u64, 0);

        TEST_EQUAL(str_to_integer<int8_t>("42xyz"s), 42);
        TEST_EQUAL(str_to_integer<int8_t>("-42xyz"s), -42);
        TEST_EQUAL(str_to_integer<int8_t>("xyz"s), 0);
        TEST_EQUAL(str_to_integer<uint8_t>("42xyz"s), 42);
        TEST_EQUAL(str_to_integer<uint8_t>("xyz"s), 0);
        TEST_EQUAL(str_to_integer<int16_t>("42xyz"s), 42);
        TEST_EQUAL(str_to_integer<int16_t>("-42xyz"s), -42);
        TEST_EQUAL(str_to_integer<int16_t>("xyz"s), 0);
        TEST_EQUAL(str_to_integer<uint16_t>("42xyz"s), 42);
        TEST_EQUAL(str_to_integer<uint16_t>("xyz"s), 0);
        TEST_EQUAL(str_to_integer<int32_t>("42xyz"s), 42);
        TEST_EQUAL(str_to_integer<int32_t>("-42xyz"s), -42);
        TEST_EQUAL(str_to_integer<int32_t>("xyz"s), 0);
        TEST_EQUAL(str_to_integer<uint32_t>("42xyz"s), 42);
        TEST_EQUAL(str_to_integer<uint32_t>("xyz"s), 0);
        TEST_EQUAL(str_to_integer<int64_t>("42xyz"s), 42);
        TEST_EQUAL(str_to_integer<int64_t>("-42xyz"s), -42);
        TEST_EQUAL(str_to_integer<int64_t>("xyz"s), 0);
        TEST_EQUAL(str_to_integer<uint64_t>("42xyz"s), 42);
        TEST_EQUAL(str_to_integer<uint64_t>("xyz"s), 0);

        TEST_EQUAL(str_to_integer<int8_t>("126"s), std::numeric_limits<int8_t>::max() - 1);
        TEST_EQUAL(str_to_integer<int8_t>("127"s), std::numeric_limits<int8_t>::max());
        TEST_EQUAL(str_to_integer<int8_t>("128"s), std::numeric_limits<int8_t>::max());
        TEST_EQUAL(str_to_integer<int8_t>("-127"s), std::numeric_limits<int8_t>::min() + 1);
        TEST_EQUAL(str_to_integer<int8_t>("-128"s), std::numeric_limits<int8_t>::min());
        TEST_EQUAL(str_to_integer<int8_t>("-129"s), std::numeric_limits<int8_t>::min());
        TEST_EQUAL(str_to_integer<uint8_t>("254"s), std::numeric_limits<uint8_t>::max() - 1);
        TEST_EQUAL(str_to_integer<uint8_t>("255"s), std::numeric_limits<uint8_t>::max());
        TEST_EQUAL(str_to_integer<uint8_t>("256"s), std::numeric_limits<uint8_t>::max());

        TEST_EQUAL(str_to_integer<int16_t>("32766"s), std::numeric_limits<int16_t>::max() - 1);
        TEST_EQUAL(str_to_integer<int16_t>("32767"s), std::numeric_limits<int16_t>::max());
        TEST_EQUAL(str_to_integer<int16_t>("32768"s), std::numeric_limits<int16_t>::max());
        TEST_EQUAL(str_to_integer<int16_t>("-32767"s), std::numeric_limits<int16_t>::min() + 1);
        TEST_EQUAL(str_to_integer<int16_t>("-32768"s), std::numeric_limits<int16_t>::min());
        TEST_EQUAL(str_to_integer<int16_t>("-32769"s), std::numeric_limits<int16_t>::min());
        TEST_EQUAL(str_to_integer<uint16_t>("65534"s), std::numeric_limits<uint16_t>::max() - 1);
        TEST_EQUAL(str_to_integer<uint16_t>("65535"s), std::numeric_limits<uint16_t>::max());
        TEST_EQUAL(str_to_integer<uint16_t>("65536"s), std::numeric_limits<uint16_t>::max());

        TEST_EQUAL(str_to_integer<int32_t>("2147483646"s), std::numeric_limits<int32_t>::max() - 1);
        TEST_EQUAL(str_to_integer<int32_t>("2147483647"s), std::numeric_limits<int32_t>::max());
        TEST_EQUAL(str_to_integer<int32_t>("2147483648"s), std::numeric_limits<int32_t>::max());
        TEST_EQUAL(str_to_integer<int32_t>("-2147483647"s), std::numeric_limits<int32_t>::min() + 1);
        TEST_EQUAL(str_to_integer<int32_t>("-2147483648"s), std::numeric_limits<int32_t>::min());
        TEST_EQUAL(str_to_integer<int32_t>("-2147483648"s), std::numeric_limits<int32_t>::min());
        TEST_EQUAL(str_to_integer<uint32_t>("4294967294"s), std::numeric_limits<uint32_t>::max() - 1);
        TEST_EQUAL(str_to_integer<uint32_t>("4294967295"s), std::numeric_limits<uint32_t>::max());
        TEST_EQUAL(str_to_integer<uint32_t>("4294967296"s), std::numeric_limits<uint32_t>::max());

        TEST_EQUAL(str_to_integer<int64_t>("9223372036854775806"s), std::numeric_limits<int64_t>::max() - 1);
        TEST_EQUAL(str_to_integer<int64_t>("9223372036854775807"s), std::numeric_limits<int64_t>::max());
        TEST_EQUAL(str_to_integer<int64_t>("9223372036854775808"s), std::numeric_limits<int64_t>::max());
        TEST_EQUAL(str_to_integer<int64_t>("-9223372036854775807"s), std::numeric_limits<int64_t>::min() + 1);
        TEST_EQUAL(str_to_integer<int64_t>("-9223372036854775808"s), std::numeric_limits<int64_t>::min());
        TEST_EQUAL(str_to_integer<int64_t>("-9223372036854775809"s), std::numeric_limits<int64_t>::min());
        TEST_EQUAL(str_to_integer<uint64_t>("18446744073709551614"s), std::numeric_limits<uint64_t>::max() - 1);
        TEST_EQUAL(str_to_integer<uint64_t>("18446744073709551615"s), std::numeric_limits<uint64_t>::max());
        TEST_EQUAL(str_to_integer<uint64_t>("18446744073709551616"s), std::numeric_limits<uint64_t>::max());

    }

    void check_hexadecimal_integers() {

        int8_t i8;
        uint8_t u8;
        int16_t i16;
        uint16_t u16;
        int32_t i32;
        uint32_t u32;
        int64_t i64;
        uint64_t u64;

        TEST_EQUAL(hex_to_integer(i8, "ffxyz"s), 2);   TEST_EQUAL(i8, 127);
        TEST_EQUAL(hex_to_integer(u8, "ffxyz"s), 2);   TEST_EQUAL(u8, 255);
        TEST_EQUAL(hex_to_integer(i16, "ffxyz"s), 2);  TEST_EQUAL(i16, 255);
        TEST_EQUAL(hex_to_integer(u16, "ffxyz"s), 2);  TEST_EQUAL(u16, 255);
        TEST_EQUAL(hex_to_integer(i32, "ffxyz"s), 2);  TEST_EQUAL(i32, 255);
        TEST_EQUAL(hex_to_integer(u32, "ffxyz"s), 2);  TEST_EQUAL(u32, 255);
        TEST_EQUAL(hex_to_integer(i64, "ffxyz"s), 2);  TEST_EQUAL(i64, 255);
        TEST_EQUAL(hex_to_integer(u64, "ffxyz"s), 2);  TEST_EQUAL(u64, 255);

        TEST_EQUAL(hex_to_integer<int8_t>("ffxyz"s), 127);
        TEST_EQUAL(hex_to_integer<uint8_t>("ffxyz"s), 255);
        TEST_EQUAL(hex_to_integer<int16_t>("ffxyz"s), 255);
        TEST_EQUAL(hex_to_integer<uint16_t>("ffxyz"s), 255);
        TEST_EQUAL(hex_to_integer<int32_t>("ffxyz"s), 255);
        TEST_EQUAL(hex_to_integer<uint32_t>("ffxyz"s), 255);
        TEST_EQUAL(hex_to_integer<int64_t>("ffxyz"s), 255);
        TEST_EQUAL(hex_to_integer<uint64_t>("ffxyz"s), 255);

        TEST_EQUAL(hex_to_integer<int8_t>("7e"s), std::numeric_limits<int8_t>::max() - 1);
        TEST_EQUAL(hex_to_integer<int8_t>("7f"s), std::numeric_limits<int8_t>::max());
        TEST_EQUAL(hex_to_integer<int8_t>("80"s), std::numeric_limits<int8_t>::max());
        TEST_EQUAL(hex_to_integer<int8_t>("-7f"s), std::numeric_limits<int8_t>::min() + 1);
        TEST_EQUAL(hex_to_integer<int8_t>("-80"s), std::numeric_limits<int8_t>::min());
        TEST_EQUAL(hex_to_integer<int8_t>("-81"s), std::numeric_limits<int8_t>::min());
        TEST_EQUAL(hex_to_integer<uint8_t>("fe"s), std::numeric_limits<uint8_t>::max() - 1);
        TEST_EQUAL(hex_to_integer<uint8_t>("ff"s), std::numeric_limits<uint8_t>::max());
        TEST_EQUAL(hex_to_integer<uint8_t>("100"s), std::numeric_limits<uint8_t>::max());

        TEST_EQUAL(hex_to_integer<int16_t>("7ffe"s), std::numeric_limits<int16_t>::max() - 1);
        TEST_EQUAL(hex_to_integer<int16_t>("7fff"s), std::numeric_limits<int16_t>::max());
        TEST_EQUAL(hex_to_integer<int16_t>("8000"s), std::numeric_limits<int16_t>::max());
        TEST_EQUAL(hex_to_integer<int16_t>("-7fff"s), std::numeric_limits<int16_t>::min() + 1);
        TEST_EQUAL(hex_to_integer<int16_t>("-8000"s), std::numeric_limits<int16_t>::min());
        TEST_EQUAL(hex_to_integer<int16_t>("-8001"s), std::numeric_limits<int16_t>::min());
        TEST_EQUAL(hex_to_integer<uint16_t>("fffe"s), std::numeric_limits<uint16_t>::max() - 1);
        TEST_EQUAL(hex_to_integer<uint16_t>("ffff"s), std::numeric_limits<uint16_t>::max());
        TEST_EQUAL(hex_to_integer<uint16_t>("10000"s), std::numeric_limits<uint16_t>::max());

        TEST_EQUAL(hex_to_integer<int32_t>("7ffffffe"s), std::numeric_limits<int32_t>::max() - 1);
        TEST_EQUAL(hex_to_integer<int32_t>("7fffffff"s), std::numeric_limits<int32_t>::max());
        TEST_EQUAL(hex_to_integer<int32_t>("80000000"s), std::numeric_limits<int32_t>::max());
        TEST_EQUAL(hex_to_integer<int32_t>("-7fffffff"s), std::numeric_limits<int32_t>::min() + 1);
        TEST_EQUAL(hex_to_integer<int32_t>("-80000000"s), std::numeric_limits<int32_t>::min());
        TEST_EQUAL(hex_to_integer<int32_t>("-80000001"s), std::numeric_limits<int32_t>::min());
        TEST_EQUAL(hex_to_integer<uint32_t>("fffffffe"s), std::numeric_limits<uint32_t>::max() - 1);
        TEST_EQUAL(hex_to_integer<uint32_t>("ffffffff"s), std::numeric_limits<uint32_t>::max());
        TEST_EQUAL(hex_to_integer<uint32_t>("100000000"s), std::numeric_limits<uint32_t>::max());

        TEST_EQUAL(hex_to_integer<int64_t>("7ffffffffffffffe"s), std::numeric_limits<int64_t>::max() - 1);
        TEST_EQUAL(hex_to_integer<int64_t>("7fffffffffffffff"s), std::numeric_limits<int64_t>::max());
        TEST_EQUAL(hex_to_integer<int64_t>("8000000000000000"s), std::numeric_limits<int64_t>::max());
        TEST_EQUAL(hex_to_integer<int64_t>("-7fffffffffffffff"s), std::numeric_limits<int64_t>::min() + 1);
        TEST_EQUAL(hex_to_integer<int64_t>("-8000000000000000"s), std::numeric_limits<int64_t>::min());
        TEST_EQUAL(hex_to_integer<int64_t>("-8000000000000001"s), std::numeric_limits<int64_t>::min());
        TEST_EQUAL(hex_to_integer<uint64_t>("fffffffffffffffe"s), std::numeric_limits<uint64_t>::max() - 1);
        TEST_EQUAL(hex_to_integer<uint64_t>("ffffffffffffffff"s), std::numeric_limits<uint64_t>::max());
        TEST_EQUAL(hex_to_integer<uint64_t>("10000000000000000"s), std::numeric_limits<uint64_t>::max());

    }

    void check_floating_point() {

        float f;
        double d;
        long double ld;

        TEST_EQUAL(str_to_real(f, "42xyz"s), 2);         TEST_EQUAL(f, 42);
        TEST_EQUAL(str_to_real(f, "-6.25e-2xyz"s), 8);   TEST_EQUAL(f, -0.0625);
        TEST_EQUAL(str_to_real(f, "xyz"s), 0);           TEST_EQUAL(f, 0);
        TEST_EQUAL(str_to_real(d, "42xyz"s), 2);         TEST_EQUAL(d, 42);
        TEST_EQUAL(str_to_real(d, "-6.25e-2xyz"s), 8);   TEST_EQUAL(d, -0.0625);
        TEST_EQUAL(str_to_real(d, "xyz"s), 0);           TEST_EQUAL(d, 0);
        TEST_EQUAL(str_to_real(ld, "42xyz"s), 2);        TEST_EQUAL(ld, 42);
        TEST_EQUAL(str_to_real(ld, "-6.25e-2xyz"s), 8);  TEST_EQUAL(ld, -0.0625);
        TEST_EQUAL(str_to_real(ld, "xyz"s), 0);          TEST_EQUAL(ld, 0);

        TEST_EQUAL(str_to_real<double>(""s), 0);
        TEST_EQUAL(str_to_real<double>("0"s), 0);
        TEST_EQUAL(str_to_real<double>("42"s), 42);
        TEST_EQUAL(str_to_real<double>("42 roads"s), 42);
        TEST_EQUAL(str_to_real<double>("0042"s), 42);
        TEST_EQUAL(str_to_real<double>("+42"s), 42);
        TEST_EQUAL(str_to_real<double>("-42"s), -42);
        TEST_EQUAL(str_to_real<double>("1234.5"s), 1234.5);
        TEST_EQUAL(str_to_real<double>("+1234.5"s), 1234.5);
        TEST_EQUAL(str_to_real<double>("-1234.5"s), -1234.5);
        TEST_EQUAL(str_to_real<double>("1.23e4"s), 12300);
        TEST_EQUAL(str_to_real<double>("+1.23e4"s), 12300);
        TEST_EQUAL(str_to_real<double>("-1.23e4"s), -12300);
        TEST_EQUAL(str_to_real<double>("2.5e-1"s), 0.25);
        TEST_EQUAL(str_to_real<double>("+2.5e-1"s), 0.25);
        TEST_EQUAL(str_to_real<double>("-2.5e-1"s), -0.25);
        TEST_EQUAL(str_to_real<double>(""s, 10), 0);
        TEST_EQUAL(str_to_real<double>("123456"s, 3), 456);
        TEST_EQUAL(str_to_real<double>("123456.75"s, 3), 456.75);
        TEST_EQUAL(str_to_real<double>("answer 42"s, 7), 42);

    }

}

TEST_MODULE(unicorn, string_conversion) {

    check_decimal_integers();
    check_hexadecimal_integers();
    check_floating_point();

}
