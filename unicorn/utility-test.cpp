#include "unicorn/utility.hpp"
#include "unicorn/unit-test.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <functional>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace RS;
using namespace RS::Meta;
using namespace std::chrono;
using namespace std::literals;

namespace {

    RS_ENUM(FooEnum, int16_t, 0, alpha, bravo, charlie);
    RS_ENUM_CLASS(BarEnum, int32_t, 1, alpha, bravo, charlie);

    Ustring make_str(std::nullptr_t) { return "null"; }

    template <typename T>
    Ustring make_str(const T& t) {
        std::ostringstream out;
        out << t;
        return out.str();
    }

    #define MAKE_STR_FOR_CONTAINER(Con) \
        template <typename T> \
        Ustring make_str(const Con<T>& c) { \
            if (c.empty()) \
                return "[]"; \
            std::ostringstream out; \
            out << '['; \
            for (auto& t: c) \
                out << t << ','; \
            Ustring s = out.str(); \
            s.back() = ']'; \
            return s; \
        }

    MAKE_STR_FOR_CONTAINER(std::set)
    MAKE_STR_FOR_CONTAINER(std::vector)

}

void test_unicorn_utility_preprocessor_macros() {

    FooEnum f = {};
    BarEnum b = {};
    std::vector<FooEnum> vf;
    std::vector<BarEnum> vb;

    TEST_TYPE(std::underlying_type_t<FooEnum>, int16_t);
    TEST_TYPE(std::underlying_type_t<BarEnum>, int32_t);

    TEST_EQUAL(sizeof(FooEnum), 2);
    TEST_EQUAL(sizeof(BarEnum), 4);

    TEST_EQUAL(int(alpha), 0);
    TEST_EQUAL(int(bravo), 1);
    TEST_EQUAL(int(charlie), 2);

    TEST_EQUAL(make_str(alpha), "alpha");
    TEST_EQUAL(make_str(bravo), "bravo");
    TEST_EQUAL(make_str(charlie), "charlie");
    TEST_EQUAL(make_str(FooEnum(0)), "alpha");
    TEST_EQUAL(make_str(FooEnum(4)), "4");
    TEST_EQUAL(make_str(FooEnum(99)), "99");
    TEST_EQUAL(make_str(nullptr), "null");

    TEST(! enum_is_valid(FooEnum(-1)));
    TEST(enum_is_valid(FooEnum(0)));
    TEST(enum_is_valid(FooEnum(1)));
    TEST(enum_is_valid(FooEnum(2)));
    TEST(! enum_is_valid(FooEnum(3)));

    TRY(vf = enum_values<FooEnum>());
    TEST_EQUAL(vf.size(), 3);
    TEST_EQUAL(make_str(vf), "[alpha,bravo,charlie]");
    TRY(vf = enum_nonzero_values<FooEnum>());
    TEST_EQUAL(vf.size(), 2);
    TEST_EQUAL(make_str(vf), "[bravo,charlie]");

    TEST_EQUAL(int(BarEnum::alpha), 1);
    TEST_EQUAL(int(BarEnum::bravo), 2);
    TEST_EQUAL(int(BarEnum::charlie), 3);

    TEST_EQUAL(make_str(BarEnum::alpha), "BarEnum::alpha");
    TEST_EQUAL(make_str(BarEnum::bravo), "BarEnum::bravo");
    TEST_EQUAL(make_str(BarEnum::charlie), "BarEnum::charlie");
    TEST_EQUAL(make_str(BarEnum(0)), "0");
    TEST_EQUAL(make_str(BarEnum(4)), "4");
    TEST_EQUAL(make_str(BarEnum(99)), "99");

    TEST(! enum_is_valid(BarEnum(0)));
    TEST(enum_is_valid(BarEnum(1)));
    TEST(enum_is_valid(BarEnum(2)));
    TEST(enum_is_valid(BarEnum(3)));
    TEST(! enum_is_valid(BarEnum(4)));

    TRY(vb = enum_values<BarEnum>());
    TEST_EQUAL(vb.size(), 3);
    TEST_EQUAL(make_str(vb), "[BarEnum::alpha,BarEnum::bravo,BarEnum::charlie]");

    TRY(vb = enum_nonzero_values<BarEnum>());
    TEST_EQUAL(vb.size(), 3);
    TEST_EQUAL(make_str(vb), "[BarEnum::alpha,BarEnum::bravo,BarEnum::charlie]");

    TEST(str_to_enum("alpha", f));    TEST_EQUAL(f, alpha);
    TEST(str_to_enum("bravo", f));    TEST_EQUAL(f, bravo);
    TEST(str_to_enum("charlie", f));  TEST_EQUAL(f, charlie);
    TEST(! str_to_enum("delta", f));
    TEST(str_to_enum("FooEnum::alpha", f));    TEST_EQUAL(f, alpha);
    TEST(str_to_enum("FooEnum::bravo", f));    TEST_EQUAL(f, bravo);
    TEST(str_to_enum("FooEnum::charlie", f));  TEST_EQUAL(f, charlie);
    TEST(! str_to_enum("FooEnum::delta", f));

    TEST(str_to_enum("alpha", b));    TEST_EQUAL(b, BarEnum::alpha);
    TEST(str_to_enum("bravo", b));    TEST_EQUAL(b, BarEnum::bravo);
    TEST(str_to_enum("charlie", b));  TEST_EQUAL(b, BarEnum::charlie);
    TEST(! str_to_enum("delta", b));
    TEST(str_to_enum("BarEnum::alpha", b));    TEST_EQUAL(b, BarEnum::alpha);
    TEST(str_to_enum("BarEnum::bravo", b));    TEST_EQUAL(b, BarEnum::bravo);
    TEST(str_to_enum("BarEnum::charlie", b));  TEST_EQUAL(b, BarEnum::charlie);
    TEST(! str_to_enum("BarEnum::delta", b));

}

void test_unicorn_utility_constants() {

    uint16_t u16_1 = 1;
    uint8_t u8_2[2];

    std::memcpy(u8_2, &u16_1, 2);
    TEST_COMPARE(little_endian_target, !=, big_endian_target);
    TEST_EQUAL(bool(u8_2[0]), little_endian_target);
    TEST_EQUAL(bool(u8_2[1]), big_endian_target);

}

void test_unicorn_utility_algorithms() {

    Ustring s1, s2;
    int n = 0;

    s1 = "Hello";
    TRY(std::copy(s1.begin(), s1.end(), append(s2)));
    TEST_EQUAL(s2, "Hello");
    s1 = " world";
    TRY(std::copy(s1.begin(), s1.end(), append(s2)));
    TEST_EQUAL(s2, "Hello world");
    s1 = "Goodbye";
    TRY(std::copy(s1.begin(), s1.end(), overwrite(s2)));
    TEST_EQUAL(s2, "Goodbye");

    std::set<int> set1, set2;
    set1 = {1, 2, 3};
    TRY(std::copy(set1.begin(), set1.end(), append(set2)));
    TEST_EQUAL(make_str(set2), "[1,2,3]");
    set1 = {4, 5, 6};
    TRY(std::copy(set1.begin(), set1.end(), append(set2)));
    TEST_EQUAL(make_str(set2), "[1,2,3,4,5,6]");
    set1 = {1, 2, 3};
    TRY(std::copy(set1.begin(), set1.end(), append(set2)));
    TEST_EQUAL(make_str(set2), "[1,2,3,4,5,6]");
    set1 = {7, 8, 9};
    TRY(std::copy(set1.begin(), set1.end(), overwrite(set2)));
    TEST_EQUAL(make_str(set2), "[7,8,9]");

    s1.clear();

    TRY("Hello"s >> append(s1));       TEST_EQUAL(s1, "Hello");
    TRY("World"s >> append(s1));       TEST_EQUAL(s1, "HelloWorld");
    TRY("Goodbye"s >> overwrite(s1));  TEST_EQUAL(s1, "Goodbye");

    s1 = "";               s2 = "";               TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 0);
    s1 = "";               s2 = "hello";          TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, -1);
    s1 = "hello";          s2 = "";               TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 1);
    s1 = "hello";          s2 = "world";          TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, -1);
    s1 = "hello";          s2 = "hello";          TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 0);
    s1 = "hello";          s2 = "goodbye";        TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 1);
    s1 = "hello";          s2 = "hello world";    TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, -1);
    s1 = "hello world";    s2 = "hello";          TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 1);
    s1 = "hello goodbye";  s2 = "hello world";    TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, -1);
    s1 = "hello world";    s2 = "hello goodbye";  TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 1);
    s1 = "";               s2 = "";               TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 0);
    s1 = "";               s2 = "hello";          TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, -1);
    s1 = "hello";          s2 = "";               TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 1);
    s1 = "hello";          s2 = "world";          TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 1);
    s1 = "hello";          s2 = "hello";          TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 0);
    s1 = "hello";          s2 = "goodbye";        TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, -1);
    s1 = "hello";          s2 = "hello world";    TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, -1);
    s1 = "hello world";    s2 = "hello";          TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 1);
    s1 = "hello goodbye";  s2 = "hello world";    TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 1);
    s1 = "hello world";    s2 = "hello goodbye";  TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, -1);

}

void test_unicorn_utility_arithmetic_functions() {

    int8_t s8 = -42;
    uint8_t u8 = 42;
    int16_t s16 = -42;
    uint16_t u16 = 42;
    int32_t s32 = -42;
    uint32_t u32 = 42;
    int64_t s64 = -42;
    uint64_t u64 = 42;

    TEST_EQUAL(sizeof(as_signed(s8)), 1);
    TEST_EQUAL(sizeof(as_signed(u8)), 1);
    TEST_EQUAL(sizeof(as_unsigned(s8)), 1);
    TEST_EQUAL(sizeof(as_unsigned(u8)), 1);
    TEST_EQUAL(sizeof(as_signed(s16)), 2);
    TEST_EQUAL(sizeof(as_signed(u16)), 2);
    TEST_EQUAL(sizeof(as_unsigned(s16)), 2);
    TEST_EQUAL(sizeof(as_unsigned(u16)), 2);
    TEST_EQUAL(sizeof(as_signed(s32)), 4);
    TEST_EQUAL(sizeof(as_signed(u32)), 4);
    TEST_EQUAL(sizeof(as_unsigned(s32)), 4);
    TEST_EQUAL(sizeof(as_unsigned(u32)), 4);
    TEST_EQUAL(sizeof(as_signed(s64)), 8);
    TEST_EQUAL(sizeof(as_signed(u64)), 8);
    TEST_EQUAL(sizeof(as_unsigned(s64)), 8);
    TEST_EQUAL(sizeof(as_unsigned(u64)), 8);

    TEST(std::is_signed<decltype(as_signed(s8))>::value);
    TEST(std::is_signed<decltype(as_signed(u8))>::value);
    TEST(std::is_unsigned<decltype(as_unsigned(s8))>::value);
    TEST(std::is_unsigned<decltype(as_unsigned(u8))>::value);
    TEST(std::is_signed<decltype(as_signed(s16))>::value);
    TEST(std::is_signed<decltype(as_signed(u16))>::value);
    TEST(std::is_unsigned<decltype(as_unsigned(s16))>::value);
    TEST(std::is_unsigned<decltype(as_unsigned(u16))>::value);
    TEST(std::is_signed<decltype(as_signed(s32))>::value);
    TEST(std::is_signed<decltype(as_signed(u32))>::value);
    TEST(std::is_unsigned<decltype(as_unsigned(s32))>::value);
    TEST(std::is_unsigned<decltype(as_unsigned(u32))>::value);
    TEST(std::is_signed<decltype(as_signed(s64))>::value);
    TEST(std::is_signed<decltype(as_signed(u64))>::value);
    TEST(std::is_unsigned<decltype(as_unsigned(s64))>::value);
    TEST(std::is_unsigned<decltype(as_unsigned(u64))>::value);

    TEST_EQUAL(as_signed(s8), -42);
    TEST_EQUAL(as_signed(u8), 42);
    TEST_EQUAL(as_unsigned(s8), 214);
    TEST_EQUAL(as_unsigned(u8), 42);
    TEST_EQUAL(as_signed(s16), -42);
    TEST_EQUAL(as_signed(u16), 42);
    TEST_EQUAL(as_unsigned(s16), 65494);
    TEST_EQUAL(as_unsigned(u16), 42);
    TEST_EQUAL(as_signed(s32), -42);
    TEST_EQUAL(as_signed(u32), 42);
    TEST_EQUAL(as_unsigned(s32), 4'294'967'254ul);
    TEST_EQUAL(as_unsigned(u32), 42);
    TEST_EQUAL(as_signed(s64), -42);
    TEST_EQUAL(as_signed(u64), 42);
    TEST_EQUAL(as_unsigned(s64), 18'446'744'073'709'551'574ull);
    TEST_EQUAL(as_unsigned(u64), 42);

    TEST_EQUAL(clamp(1, 2, 4), 2);
    TEST_EQUAL(clamp(2, 2, 4), 2);
    TEST_EQUAL(clamp(3, 2, 4), 3);
    TEST_EQUAL(clamp(4, 2, 4), 4);
    TEST_EQUAL(clamp(5, 2, 4), 4);
    TEST_EQUAL(clamp(1.0, 2, 4), 2);
    TEST_EQUAL(clamp(1.5, 2, 4), 2);
    TEST_EQUAL(clamp(2.0, 2, 4), 2);
    TEST_EQUAL(clamp(2.5, 2, 4), 2.5);
    TEST_EQUAL(clamp(3.0, 2, 4), 3);
    TEST_EQUAL(clamp(3.5, 2, 4), 3.5);
    TEST_EQUAL(clamp(4.0, 2, 4), 4);
    TEST_EQUAL(clamp(4.5, 2, 4), 4);
    TEST_EQUAL(clamp(5.0, 2, 4), 4);

    TEST_EQUAL(ibits(0), 0);
    TEST_EQUAL(ibits(1), 1);
    TEST_EQUAL(ibits(2), 1);
    TEST_EQUAL(ibits(3), 2);
    TEST_EQUAL(ibits(4), 1);
    TEST_EQUAL(ibits(5), 2);
    TEST_EQUAL(ibits(6), 2);
    TEST_EQUAL(ibits(7), 3);
    TEST_EQUAL(ibits(8), 1);
    TEST_EQUAL(ibits(9), 2);
    TEST_EQUAL(ibits(10), 2);
    TEST_EQUAL(ibits(0x7fff), 15);
    TEST_EQUAL(ibits(0x8000), 1);
    TEST_EQUAL(ibits(0x8888), 4);
    TEST_EQUAL(ibits(0xffff), 16);
    TEST_EQUAL(ibits(0x7fffffffffffffffull), 63);
    TEST_EQUAL(ibits(0x8000000000000000ull), 1);
    TEST_EQUAL(ibits(0xffffffffffffffffull), 64);

    TEST_EQUAL(ifloor2(0), 0);
    TEST_EQUAL(ifloor2(1), 1);
    TEST_EQUAL(ifloor2(2), 2);
    TEST_EQUAL(ifloor2(3), 2);
    TEST_EQUAL(ifloor2(4), 4);
    TEST_EQUAL(ifloor2(5), 4);
    TEST_EQUAL(ifloor2(6), 4);
    TEST_EQUAL(ifloor2(7), 4);
    TEST_EQUAL(ifloor2(8), 8);
    TEST_EQUAL(ifloor2(9), 8);
    TEST_EQUAL(ifloor2(10), 8);
    TEST_EQUAL(ifloor2(0x7ffful), 0x4000ul);
    TEST_EQUAL(ifloor2(0x8000ul), 0x8000ul);
    TEST_EQUAL(ifloor2(0x8001ul), 0x8000ul);
    TEST_EQUAL(ifloor2(0xfffful), 0x8000ul);

    TEST_EQUAL(iceil2(0), 0);
    TEST_EQUAL(iceil2(1), 1);
    TEST_EQUAL(iceil2(2), 2);
    TEST_EQUAL(iceil2(3), 4);
    TEST_EQUAL(iceil2(4), 4);
    TEST_EQUAL(iceil2(5), 8);
    TEST_EQUAL(iceil2(6), 8);
    TEST_EQUAL(iceil2(7), 8);
    TEST_EQUAL(iceil2(8), 8);
    TEST_EQUAL(iceil2(9), 16);
    TEST_EQUAL(iceil2(10), 16);
    TEST_EQUAL(iceil2(0x7ffful), 0x8000ul);
    TEST_EQUAL(iceil2(0x8000ul), 0x8000ul);
    TEST_EQUAL(iceil2(0x8001ul), 0x10000ul);
    TEST_EQUAL(iceil2(0xfffful), 0x10000ul);

    TEST(! ispow2(0));
    TEST(ispow2(1));
    TEST(ispow2(2));
    TEST(! ispow2(3));
    TEST(ispow2(4));
    TEST(! ispow2(5));
    TEST(! ispow2(6));
    TEST(! ispow2(7));
    TEST(ispow2(8));
    TEST(! ispow2(9));
    TEST(! ispow2(10));
    TEST(! ispow2(0x7fff));
    TEST(ispow2(0x8000));
    TEST(! ispow2(0x8001));
    TEST(! ispow2(0xffff));
    TEST(! ispow2(0x7fffffffffffffffull));
    TEST(ispow2(0x8000000000000000ull));
    TEST(! ispow2(0x8000000000000001ull));
    TEST(! ispow2(0xffffffffffffffffull));

    TEST_EQUAL(ilog2p1(0), 0);
    TEST_EQUAL(ilog2p1(1), 1);
    TEST_EQUAL(ilog2p1(2), 2);
    TEST_EQUAL(ilog2p1(3), 2);
    TEST_EQUAL(ilog2p1(4), 3);
    TEST_EQUAL(ilog2p1(5), 3);
    TEST_EQUAL(ilog2p1(6), 3);
    TEST_EQUAL(ilog2p1(7), 3);
    TEST_EQUAL(ilog2p1(8), 4);
    TEST_EQUAL(ilog2p1(9), 4);
    TEST_EQUAL(ilog2p1(10), 4);
    TEST_EQUAL(ilog2p1(0x7fff), 15);
    TEST_EQUAL(ilog2p1(0x8000), 16);
    TEST_EQUAL(ilog2p1(0x8888), 16);
    TEST_EQUAL(ilog2p1(0xffff), 16);
    TEST_EQUAL(ilog2p1(0x7fffffffffffffffull), 63);
    TEST_EQUAL(ilog2p1(0x8000000000000000ull), 64);
    TEST_EQUAL(ilog2p1(0xffffffffffffffffull), 64);

    TEST_EQUAL(letter_to_mask('A'), 1);
    TEST_EQUAL(letter_to_mask('B'), 2);
    TEST_EQUAL(letter_to_mask('C'), 4);
    TEST_EQUAL(letter_to_mask('Z'), 0x2000000ull);
    TEST_EQUAL(letter_to_mask('a'), 0x4000000ull);
    TEST_EQUAL(letter_to_mask('b'), 0x8000000ull);
    TEST_EQUAL(letter_to_mask('c'), 0x10000000ull);
    TEST_EQUAL(letter_to_mask('z'), 0x8000000000000ull);
    TEST_EQUAL(letter_to_mask('~'), 0);

    u16 = 0x1234;

    TEST_EQUAL(rotl(u16, 0), 0x1234);    TEST_EQUAL(rotr(u16, 0), 0x1234);
    TEST_EQUAL(rotl(u16, 4), 0x2341);    TEST_EQUAL(rotr(u16, 4), 0x4123);
    TEST_EQUAL(rotl(u16, 8), 0x3412);    TEST_EQUAL(rotr(u16, 8), 0x3412);
    TEST_EQUAL(rotl(u16, 12), 0x4123);   TEST_EQUAL(rotr(u16, 12), 0x2341);
    TEST_EQUAL(rotl(u16, 16), 0x1234);   TEST_EQUAL(rotr(u16, 16), 0x1234);
    TEST_EQUAL(rotl(u16, 20), 0x2341);   TEST_EQUAL(rotr(u16, 20), 0x4123);
    TEST_EQUAL(rotl(u16, 24), 0x3412);   TEST_EQUAL(rotr(u16, 24), 0x3412);
    TEST_EQUAL(rotl(u16, 28), 0x4123);   TEST_EQUAL(rotr(u16, 28), 0x2341);
    TEST_EQUAL(rotl(u16, 32), 0x1234);   TEST_EQUAL(rotr(u16, 32), 0x1234);
    TEST_EQUAL(rotl(u16, -4), 0x4123);   TEST_EQUAL(rotr(u16, -4), 0x2341);
    TEST_EQUAL(rotl(u16, -8), 0x3412);   TEST_EQUAL(rotr(u16, -8), 0x3412);
    TEST_EQUAL(rotl(u16, -12), 0x2341);  TEST_EQUAL(rotr(u16, -12), 0x4123);
    TEST_EQUAL(rotl(u16, -16), 0x1234);  TEST_EQUAL(rotr(u16, -16), 0x1234);
    TEST_EQUAL(rotl(u16, -20), 0x4123);  TEST_EQUAL(rotr(u16, -20), 0x2341);
    TEST_EQUAL(rotl(u16, -24), 0x3412);  TEST_EQUAL(rotr(u16, -24), 0x3412);
    TEST_EQUAL(rotl(u16, -28), 0x2341);  TEST_EQUAL(rotr(u16, -28), 0x4123);
    TEST_EQUAL(rotl(u16, -32), 0x1234);  TEST_EQUAL(rotr(u16, -32), 0x1234);

    u32 = 0x12345678;

    TEST_EQUAL(rotl(u32, 0), 0x12345678);    TEST_EQUAL(rotr(u32, 0), 0x12345678);
    TEST_EQUAL(rotl(u32, 4), 0x23456781);    TEST_EQUAL(rotr(u32, 4), 0x81234567);
    TEST_EQUAL(rotl(u32, 8), 0x34567812);    TEST_EQUAL(rotr(u32, 8), 0x78123456);
    TEST_EQUAL(rotl(u32, 12), 0x45678123);   TEST_EQUAL(rotr(u32, 12), 0x67812345);
    TEST_EQUAL(rotl(u32, 16), 0x56781234);   TEST_EQUAL(rotr(u32, 16), 0x56781234);
    TEST_EQUAL(rotl(u32, 20), 0x67812345);   TEST_EQUAL(rotr(u32, 20), 0x45678123);
    TEST_EQUAL(rotl(u32, 24), 0x78123456);   TEST_EQUAL(rotr(u32, 24), 0x34567812);
    TEST_EQUAL(rotl(u32, 28), 0x81234567);   TEST_EQUAL(rotr(u32, 28), 0x23456781);
    TEST_EQUAL(rotl(u32, 32), 0x12345678);   TEST_EQUAL(rotr(u32, 32), 0x12345678);
    TEST_EQUAL(rotl(u32, 36), 0x23456781);   TEST_EQUAL(rotr(u32, 36), 0x81234567);
    TEST_EQUAL(rotl(u32, 40), 0x34567812);   TEST_EQUAL(rotr(u32, 40), 0x78123456);
    TEST_EQUAL(rotl(u32, 44), 0x45678123);   TEST_EQUAL(rotr(u32, 44), 0x67812345);
    TEST_EQUAL(rotl(u32, 48), 0x56781234);   TEST_EQUAL(rotr(u32, 48), 0x56781234);
    TEST_EQUAL(rotl(u32, 52), 0x67812345);   TEST_EQUAL(rotr(u32, 52), 0x45678123);
    TEST_EQUAL(rotl(u32, 56), 0x78123456);   TEST_EQUAL(rotr(u32, 56), 0x34567812);
    TEST_EQUAL(rotl(u32, 60), 0x81234567);   TEST_EQUAL(rotr(u32, 60), 0x23456781);
    TEST_EQUAL(rotl(u32, 64), 0x12345678);   TEST_EQUAL(rotr(u32, 64), 0x12345678);
    TEST_EQUAL(rotl(u32, -4), 0x81234567);   TEST_EQUAL(rotr(u32, -4), 0x23456781);
    TEST_EQUAL(rotl(u32, -8), 0x78123456);   TEST_EQUAL(rotr(u32, -8), 0x34567812);
    TEST_EQUAL(rotl(u32, -12), 0x67812345);  TEST_EQUAL(rotr(u32, -12), 0x45678123);
    TEST_EQUAL(rotl(u32, -16), 0x56781234);  TEST_EQUAL(rotr(u32, -16), 0x56781234);
    TEST_EQUAL(rotl(u32, -20), 0x45678123);  TEST_EQUAL(rotr(u32, -20), 0x67812345);
    TEST_EQUAL(rotl(u32, -24), 0x34567812);  TEST_EQUAL(rotr(u32, -24), 0x78123456);
    TEST_EQUAL(rotl(u32, -28), 0x23456781);  TEST_EQUAL(rotr(u32, -28), 0x81234567);
    TEST_EQUAL(rotl(u32, -32), 0x12345678);  TEST_EQUAL(rotr(u32, -32), 0x12345678);
    TEST_EQUAL(rotl(u32, -36), 0x81234567);  TEST_EQUAL(rotr(u32, -36), 0x23456781);
    TEST_EQUAL(rotl(u32, -40), 0x78123456);  TEST_EQUAL(rotr(u32, -40), 0x34567812);
    TEST_EQUAL(rotl(u32, -44), 0x67812345);  TEST_EQUAL(rotr(u32, -44), 0x45678123);
    TEST_EQUAL(rotl(u32, -48), 0x56781234);  TEST_EQUAL(rotr(u32, -48), 0x56781234);
    TEST_EQUAL(rotl(u32, -52), 0x45678123);  TEST_EQUAL(rotr(u32, -52), 0x67812345);
    TEST_EQUAL(rotl(u32, -56), 0x34567812);  TEST_EQUAL(rotr(u32, -56), 0x78123456);
    TEST_EQUAL(rotl(u32, -60), 0x23456781);  TEST_EQUAL(rotr(u32, -60), 0x81234567);
    TEST_EQUAL(rotl(u32, -64), 0x12345678);  TEST_EQUAL(rotr(u32, -64), 0x12345678);

}

void test_unicorn_utility_date_and_time_functions() {

    using Dseconds = duration<double>;

    system_clock::time_point tp = {};
    system_clock::duration d = {};
    Ustring str;
    int64_t n1 = 0, n2 = 0;

    TRY(tp = make_date(2000, 1, 2, 3, 4, 5));
    TRY(str = format_date(tp));
    TEST_EQUAL(str, "2000-01-02 03:04:05");
    TRY(str = format_date(tp, 3));
    TEST_EQUAL(str, "2000-01-02 03:04:05.000");
    TRY(from_seconds(0.12345, d));
    TRY(tp += d);
    TRY(str = format_date(tp));
    TEST_EQUAL(str, "2000-01-02 03:04:05");
    TRY(str = format_date(tp, 3));
    TEST_EQUAL(str, "2000-01-02 03:04:05.123");
    TRY(from_seconds(0.44444, d));
    TRY(tp += d);
    TRY(str = format_date(tp));
    TEST_EQUAL(str, "2000-01-02 03:04:05");
    TRY(str = format_date(tp, 3));
    TEST_EQUAL(str, "2000-01-02 03:04:05.568");
    TRY(str = format_date(tp, "%d/%m/%Y %H:%M"));
    TEST_EQUAL(str, "02/01/2000 03:04");
    TRY(str = format_date(tp, ""));
    TEST_EQUAL(str, "");
    TRY(tp = make_date(2000, 1, 2, 3, 4, 5, local_zone));
    TRY(str = format_date(tp, 0, local_zone));
    TEST_EQUAL(str, "2000-01-02 03:04:05");

    TRY(str = format_time(Dseconds(0)));              TEST_EQUAL(str, "0s");
    TRY(str = format_time(Dseconds(0), 3));           TEST_EQUAL(str, "0.000s");
    TRY(str = format_time(Dseconds(0.25), 3));        TEST_EQUAL(str, "0.250s");
    TRY(str = format_time(Dseconds(0.5), 3));         TEST_EQUAL(str, "0.500s");
    TRY(str = format_time(Dseconds(0.75), 3));        TEST_EQUAL(str, "0.750s");
    TRY(str = format_time(Dseconds(1), 3));           TEST_EQUAL(str, "1.000s");
    TRY(str = format_time(Dseconds(1.25), 3));        TEST_EQUAL(str, "1.250s");
    TRY(str = format_time(Dseconds(59.999), 3));      TEST_EQUAL(str, "59.999s");
    TRY(str = format_time(Dseconds(60), 3));          TEST_EQUAL(str, "1m00.000s");
    TRY(str = format_time(Dseconds(1234), 3));        TEST_EQUAL(str, "20m34.000s");
    TRY(str = format_time(Dseconds(12345), 3));       TEST_EQUAL(str, "3h25m45.000s");
    TRY(str = format_time(Dseconds(123456), 3));      TEST_EQUAL(str, "1d10h17m36.000s");
    TRY(str = format_time(Dseconds(1234567), 3));     TEST_EQUAL(str, "14d06h56m07.000s");
    TRY(str = format_time(Dseconds(12345678), 3));    TEST_EQUAL(str, "142d21h21m18.000s");
    TRY(str = format_time(Dseconds(123456789), 3));   TEST_EQUAL(str, "1428d21h33m09.000s");
    TRY(str = format_time(Dseconds(-0.25), 3));       TEST_EQUAL(str, "-0.250s");
    TRY(str = format_time(Dseconds(-0.5), 3));        TEST_EQUAL(str, "-0.500s");
    TRY(str = format_time(Dseconds(-0.75), 3));       TEST_EQUAL(str, "-0.750s");
    TRY(str = format_time(Dseconds(-1), 3));          TEST_EQUAL(str, "-1.000s");
    TRY(str = format_time(Dseconds(-1.25), 3));       TEST_EQUAL(str, "-1.250s");
    TRY(str = format_time(Dseconds(-59.999), 3));     TEST_EQUAL(str, "-59.999s");
    TRY(str = format_time(Dseconds(-60), 3));         TEST_EQUAL(str, "-1m00.000s");
    TRY(str = format_time(Dseconds(-1234), 3));       TEST_EQUAL(str, "-20m34.000s");
    TRY(str = format_time(Dseconds(-12345), 3));      TEST_EQUAL(str, "-3h25m45.000s");
    TRY(str = format_time(Dseconds(-123456), 3));     TEST_EQUAL(str, "-1d10h17m36.000s");
    TRY(str = format_time(Dseconds(-1234567), 3));    TEST_EQUAL(str, "-14d06h56m07.000s");
    TRY(str = format_time(Dseconds(-12345678), 3));   TEST_EQUAL(str, "-142d21h21m18.000s");
    TRY(str = format_time(Dseconds(-123456789), 3));  TEST_EQUAL(str, "-1428d21h33m09.000s");
    TRY(str = format_time(nanoseconds(1), 10));       TEST_EQUAL(str, "0.0000000010s");
    TRY(str = format_time(microseconds(1), 7));       TEST_EQUAL(str, "0.0000010s");
    TRY(str = format_time(milliseconds(1), 4));       TEST_EQUAL(str, "0.0010s");
    TRY(str = format_time(seconds(1)));               TEST_EQUAL(str, "1s");
    TRY(str = format_time(minutes(1)));               TEST_EQUAL(str, "1m00s");
    TRY(str = format_time(hours(1)));                 TEST_EQUAL(str, "1h00m00s");
    TRY(str = format_time(nanoseconds(-1), 10));      TEST_EQUAL(str, "-0.0000000010s");
    TRY(str = format_time(microseconds(-1), 7));      TEST_EQUAL(str, "-0.0000010s");
    TRY(str = format_time(milliseconds(-1), 4));      TEST_EQUAL(str, "-0.0010s");
    TRY(str = format_time(seconds(-1)));              TEST_EQUAL(str, "-1s");
    TRY(str = format_time(minutes(-1)));              TEST_EQUAL(str, "-1m00s");
    TRY(str = format_time(hours(-1)));                TEST_EQUAL(str, "-1h00m00s");

    TRY(tp = make_date(1970, 1, 1, 0, 0, 0));
    TRY(n1 = int64_t(system_clock::to_time_t(tp)));
    TEST_EQUAL(n1, 0);
    TRY(tp = make_date(2000, 1, 2, 3, 4, 5));
    TRY(n1 = int64_t(system_clock::to_time_t(tp)));
    TEST_EQUAL(n1, 946'782'245);
    TRY(tp = make_date(2000, 1, 2, 3, 4, 5, local_zone));
    TRY(n2 = int64_t(system_clock::to_time_t(tp)));
    TEST_COMPARE(std::abs(n2 - n1), <=, 86400);

}

namespace {

    constexpr Kwarg<bool> flag;
    constexpr Kwarg<int> number;
    constexpr Kwarg<std::string, 1> str1;
    constexpr Kwarg<std::string, 2> str2;

    std::string tf(bool b) { return b ? "T" : "F"; }

    template <typename... Args>
    std::string test_kwtest(Args... args) {
        bool f = kwtest(flag, args...);
        bool n = kwtest(number, args...);
        bool s1 = kwtest(str1, args...);
        bool s2 = kwtest(str2, args...);
        return tf(f) + " " + tf(n) + " " + tf(s1) + " " + tf(s2);
    }

    template <typename... Args>
    std::string test_kwget(Args... args) {
        bool f = kwget(flag, false, args...);
        int n = kwget(number, -1, args...);
        std::string s1 = kwget(str1, "foo"s, args...);
        std::string s2 = kwget(str2, "bar"s, args...);
        return tf(f) + " " + std::to_string(n) + " " + s1 + " " + s2;
    }

}

void test_unicorn_utility_keyword_arguments() {

    std::string s;

    TRY(s = test_kwtest());                                    TEST_EQUAL(s, "F F F F");
    TRY(s = test_kwtest(flag=true));                           TEST_EQUAL(s, "T F F F");
    TRY(s = test_kwtest(flag));                                TEST_EQUAL(s, "T F F F");
    TRY(s = test_kwtest(number=42));                           TEST_EQUAL(s, "F T F F");
    TRY(s = test_kwtest(str1="hello"));                        TEST_EQUAL(s, "F F T F");
    TRY(s = test_kwtest(str2="goodbye"));                      TEST_EQUAL(s, "F F F T");
    TRY(s = test_kwtest(flag=true, number=42));                TEST_EQUAL(s, "T T F F");
    TRY(s = test_kwtest(number=42, flag=true));                TEST_EQUAL(s, "T T F F");
    TRY(s = test_kwtest(flag=true, number=42, str1="hello"));  TEST_EQUAL(s, "T T T F");
    TRY(s = test_kwtest(str1="hello", number=42, flag=true));  TEST_EQUAL(s, "T T T F");
    TRY(s = test_kwtest(flag, number=42, str1="hello"));       TEST_EQUAL(s, "T T T F");
    TRY(s = test_kwtest(str1="hello", number=42, flag));       TEST_EQUAL(s, "T T T F");
    TRY(s = test_kwtest(str1="hello", str2="goodbye"));        TEST_EQUAL(s, "F F T T");
    TRY(s = test_kwtest(str2="goodbye", str1="hello"));        TEST_EQUAL(s, "F F T T");

    TRY(s = test_kwget());                                    TEST_EQUAL(s, "F -1 foo bar");
    TRY(s = test_kwget(flag=true));                           TEST_EQUAL(s, "T -1 foo bar");
    TRY(s = test_kwget(flag));                                TEST_EQUAL(s, "T -1 foo bar");
    TRY(s = test_kwget(number=42));                           TEST_EQUAL(s, "F 42 foo bar");
    TRY(s = test_kwget(str1="hello"));                        TEST_EQUAL(s, "F -1 hello bar");
    TRY(s = test_kwget(str2="goodbye"));                      TEST_EQUAL(s, "F -1 foo goodbye");
    TRY(s = test_kwget(flag=true, number=42));                TEST_EQUAL(s, "T 42 foo bar");
    TRY(s = test_kwget(number=42, flag=true));                TEST_EQUAL(s, "T 42 foo bar");
    TRY(s = test_kwget(flag=true, number=42, str1="hello"));  TEST_EQUAL(s, "T 42 hello bar");
    TRY(s = test_kwget(str1="hello", number=42, flag=true));  TEST_EQUAL(s, "T 42 hello bar");
    TRY(s = test_kwget(flag, number=42, str1="hello"));       TEST_EQUAL(s, "T 42 hello bar");
    TRY(s = test_kwget(str1="hello", number=42, flag));       TEST_EQUAL(s, "T 42 hello bar");
    TRY(s = test_kwget(str1="hello", str2="goodbye"));        TEST_EQUAL(s, "F -1 hello goodbye");
    TRY(s = test_kwget(str2="goodbye", str1="hello"));        TEST_EQUAL(s, "F -1 hello goodbye");

}

void test_unicorn_utility_metaprogramming() {

    TEST(! is_iterator<int>);
    TEST(is_iterator<int*>);
    TEST(is_iterator<const int*>);
    TEST(! is_iterator<std::string>);
    TEST(is_iterator<std::string::iterator>);
    TEST(is_iterator<std::string::const_iterator>);

    TEST(! is_range<int>);
    TEST(! is_range<int*>);
    TEST(is_range<std::string>);
    TEST(is_range<std::vector<int>>);

    TEST_TYPE(IteratorValue<int>, void);
    TEST_TYPE(IteratorValue<int*>, int);
    TEST_TYPE(IteratorValue<const int*>, int);
    TEST_TYPE(IteratorValue<std::string>, void);
    TEST_TYPE(IteratorValue<std::string::iterator>, char);
    TEST_TYPE(IteratorValue<std::string::const_iterator>, char);

    TEST_TYPE(RangeIterator<std::string>, std::string::iterator);
    TEST_TYPE(RangeIterator<const std::string>, std::string::const_iterator);
    TEST_TYPE(RangeIterator<std::vector<int>>, std::vector<int>::iterator);
    TEST_TYPE(RangeIterator<const std::vector<int>>, std::vector<int>::const_iterator);
    TEST_TYPE(RangeIterator<Irange<int*>>, int*);
    TEST_TYPE(RangeIterator<Irange<const int*>>, const int*);

    TEST_TYPE(RangeValue<std::string>, char);
    TEST_TYPE(RangeValue<std::vector<int>>, int);
    TEST_TYPE(RangeValue<Irange<int*>>, int);
    TEST_TYPE(RangeValue<Irange<const int*>>, int);

}

void test_unicorn_utility_range_types() {

    TEST_EQUAL(range_count(""s), 0);
    TEST_EQUAL(range_count("Hello"s), 5);
    TEST_EQUAL(range_count(""), 1);
    TEST_EQUAL(range_count("Hello"), 6);
    TEST(range_empty(""s));
    TEST(! range_empty("Hello"s));

}

void test_unicorn_utility_scope_guards() {

    int n = 0;
    {
        n = 1;
        ScopeExit guard;
        TRY(guard = [&] { n = 2; });
        TEST_EQUAL(n, 1);
    }
    TEST_EQUAL(n, 2);
    n = 0;
    try {
        n = 1;
        ScopeExit guard;
        TRY(guard = [&] { n = 2; });
        TEST_EQUAL(n, 1);
        throw std::runtime_error("fail");
    }
    catch (...) {}
    TEST_EQUAL(n, 2);
    n = 0;
    {
        n = 1;
        ScopeExit guard;
        TRY(guard = [&] { n = 2; });
        TEST_EQUAL(n, 1);
        TRY(guard = [&] { n = 3; });
        TEST_EQUAL(n, 2);
    }
    TEST_EQUAL(n, 3);
    n = 0;
    {
        n = 1;
        ScopeExit guard1, guard2;
        TRY(guard1 = [&] { n = 2; });
        TEST_EQUAL(n, 1);
        TRY(guard2 = [&] { n = 3; });
        TEST_EQUAL(n, 1);
        TRY(guard2 = std::move(guard1));
        TEST_EQUAL(n, 3);
    }
    TEST_EQUAL(n, 2);

    n = 0;
    {
        n = 1;
        ScopeSuccess guard;
        TRY(guard = [&] { n = 2; });
        TEST_EQUAL(n, 1);
    }
    TEST_EQUAL(n, 2);
    n = 0;
    try {
        n = 1;
        ScopeSuccess guard;
        TRY(guard = [&] { n = 2; });
        TEST_EQUAL(n, 1);
        throw std::runtime_error("fail");
    }
    catch (...) {}
    TEST_EQUAL(n, 1);

    n = 0;
    {
        n = 1;
        ScopeFail guard;
        TRY(guard = [&] { n = 2; });
        TEST_EQUAL(n, 1);
    }
    TEST_EQUAL(n, 1);
    n = 0;
    try {
        n = 1;
        ScopeFail guard;
        TRY(guard = [&] { n = 2; });
        TEST_EQUAL(n, 1);
        throw std::runtime_error("fail");
    }
    catch (...) {}
    TEST_EQUAL(n, 2);

    n = 0;
    {
        n = 1;
        auto guard = scope_exit([&] { n = 2; });
        TEST_EQUAL(n, 1);
    }
    TEST_EQUAL(n, 2);
    n = 0;
    try {
        n = 1;
        auto guard = scope_exit([&] { n = 2; });
        TEST_EQUAL(n, 1);
        throw std::runtime_error("fail");
    }
    catch (...) {}
    TEST_EQUAL(n, 2);

    n = 0;
    {
        n = 1;
        auto guard = scope_success([&] { n = 2; });
        TEST_EQUAL(n, 1);
    }
    TEST_EQUAL(n, 2);
    n = 0;
    try {
        n = 1;
        auto guard = scope_success([&] { n = 2; });
        TEST_EQUAL(n, 1);
        throw std::runtime_error("fail");
    }
    catch (...) {}
    TEST_EQUAL(n, 1);

    n = 0;
    {
        n = 1;
        auto guard = scope_fail([&] { n = 2; });
        TEST_EQUAL(n, 1);
    }
    TEST_EQUAL(n, 1);
    n = 0;
    try {
        n = 1;
        auto guard = scope_fail([&] { n = 2; });
        TEST_EQUAL(n, 1);
        throw std::runtime_error("fail");
    }
    catch (...) {}
    TEST_EQUAL(n, 2);

}

void test_unicorn_utility_string_functions() {

    TEST(! ascii_isalnum('\0'));
    TEST(! ascii_isalnum('\t'));
    TEST(! ascii_isalnum('\n'));
    TEST(! ascii_isalnum('\x1f'));
    TEST(! ascii_isalnum(' '));
    TEST(! ascii_isalnum('!'));
    TEST(! ascii_isalnum('/'));
    TEST(ascii_isalnum('0'));
    TEST(ascii_isalnum('9'));
    TEST(! ascii_isalnum(':'));
    TEST(! ascii_isalnum('@'));
    TEST(ascii_isalnum('A'));
    TEST(ascii_isalnum('Z'));
    TEST(! ascii_isalnum('['));
    TEST(! ascii_isalnum('_'));
    TEST(! ascii_isalnum('`'));
    TEST(ascii_isalnum('a'));
    TEST(ascii_isalnum('z'));
    TEST(! ascii_isalnum('{'));
    TEST(! ascii_isalnum('~'));
    TEST(! ascii_isalnum('\x7f'));
    TEST(! ascii_isalnum('\x80'));

    TEST(! ascii_isalpha('\0'));
    TEST(! ascii_isalpha('\t'));
    TEST(! ascii_isalpha('\n'));
    TEST(! ascii_isalpha('\x1f'));
    TEST(! ascii_isalpha(' '));
    TEST(! ascii_isalpha('!'));
    TEST(! ascii_isalpha('/'));
    TEST(! ascii_isalpha('0'));
    TEST(! ascii_isalpha('9'));
    TEST(! ascii_isalpha(':'));
    TEST(! ascii_isalpha('@'));
    TEST(ascii_isalpha('A'));
    TEST(ascii_isalpha('Z'));
    TEST(! ascii_isalpha('['));
    TEST(! ascii_isalpha('_'));
    TEST(! ascii_isalpha('`'));
    TEST(ascii_isalpha('a'));
    TEST(ascii_isalpha('z'));
    TEST(! ascii_isalpha('{'));
    TEST(! ascii_isalpha('~'));
    TEST(! ascii_isalpha('\x7f'));
    TEST(! ascii_isalpha('\x80'));

    TEST(ascii_iscntrl('\0'));
    TEST(ascii_iscntrl('\t'));
    TEST(ascii_iscntrl('\n'));
    TEST(ascii_iscntrl('\x1f'));
    TEST(! ascii_iscntrl(' '));
    TEST(! ascii_iscntrl('!'));
    TEST(! ascii_iscntrl('/'));
    TEST(! ascii_iscntrl('0'));
    TEST(! ascii_iscntrl('9'));
    TEST(! ascii_iscntrl(':'));
    TEST(! ascii_iscntrl('@'));
    TEST(! ascii_iscntrl('A'));
    TEST(! ascii_iscntrl('Z'));
    TEST(! ascii_iscntrl('['));
    TEST(! ascii_iscntrl('_'));
    TEST(! ascii_iscntrl('`'));
    TEST(! ascii_iscntrl('a'));
    TEST(! ascii_iscntrl('z'));
    TEST(! ascii_iscntrl('{'));
    TEST(! ascii_iscntrl('~'));
    TEST(ascii_iscntrl('\x7f'));
    TEST(! ascii_iscntrl('\x80'));

    TEST(! ascii_isdigit('\0'));
    TEST(! ascii_isdigit('\t'));
    TEST(! ascii_isdigit('\n'));
    TEST(! ascii_isdigit('\x1f'));
    TEST(! ascii_isdigit(' '));
    TEST(! ascii_isdigit('!'));
    TEST(! ascii_isdigit('/'));
    TEST(ascii_isdigit('0'));
    TEST(ascii_isdigit('9'));
    TEST(! ascii_isdigit(':'));
    TEST(! ascii_isdigit('@'));
    TEST(! ascii_isdigit('A'));
    TEST(! ascii_isdigit('Z'));
    TEST(! ascii_isdigit('['));
    TEST(! ascii_isdigit('_'));
    TEST(! ascii_isdigit('`'));
    TEST(! ascii_isdigit('a'));
    TEST(! ascii_isdigit('z'));
    TEST(! ascii_isdigit('{'));
    TEST(! ascii_isdigit('~'));
    TEST(! ascii_isdigit('\x7f'));
    TEST(! ascii_isdigit('\x80'));

    TEST(! ascii_isgraph('\0'));
    TEST(! ascii_isgraph('\t'));
    TEST(! ascii_isgraph('\n'));
    TEST(! ascii_isgraph('\x1f'));
    TEST(! ascii_isgraph(' '));
    TEST(ascii_isgraph('!'));
    TEST(ascii_isgraph('/'));
    TEST(ascii_isgraph('0'));
    TEST(ascii_isgraph('9'));
    TEST(ascii_isgraph(':'));
    TEST(ascii_isgraph('@'));
    TEST(ascii_isgraph('A'));
    TEST(ascii_isgraph('Z'));
    TEST(ascii_isgraph('['));
    TEST(ascii_isgraph('_'));
    TEST(ascii_isgraph('`'));
    TEST(ascii_isgraph('a'));
    TEST(ascii_isgraph('z'));
    TEST(ascii_isgraph('{'));
    TEST(ascii_isgraph('~'));
    TEST(! ascii_isgraph('\x7f'));
    TEST(! ascii_isgraph('\x80'));

    TEST(! ascii_islower('\0'));
    TEST(! ascii_islower('\t'));
    TEST(! ascii_islower('\n'));
    TEST(! ascii_islower('\x1f'));
    TEST(! ascii_islower(' '));
    TEST(! ascii_islower('!'));
    TEST(! ascii_islower('/'));
    TEST(! ascii_islower('0'));
    TEST(! ascii_islower('9'));
    TEST(! ascii_islower(':'));
    TEST(! ascii_islower('@'));
    TEST(! ascii_islower('A'));
    TEST(! ascii_islower('Z'));
    TEST(! ascii_islower('['));
    TEST(! ascii_islower('_'));
    TEST(! ascii_islower('`'));
    TEST(ascii_islower('a'));
    TEST(ascii_islower('z'));
    TEST(! ascii_islower('{'));
    TEST(! ascii_islower('~'));
    TEST(! ascii_islower('\x7f'));
    TEST(! ascii_islower('\x80'));

    TEST(! ascii_isprint('\0'));
    TEST(! ascii_isprint('\t'));
    TEST(! ascii_isprint('\n'));
    TEST(! ascii_isprint('\x1f'));
    TEST(ascii_isprint(' '));
    TEST(ascii_isprint('!'));
    TEST(ascii_isprint('/'));
    TEST(ascii_isprint('0'));
    TEST(ascii_isprint('9'));
    TEST(ascii_isprint(':'));
    TEST(ascii_isprint('@'));
    TEST(ascii_isprint('A'));
    TEST(ascii_isprint('Z'));
    TEST(ascii_isprint('['));
    TEST(ascii_isprint('_'));
    TEST(ascii_isprint('`'));
    TEST(ascii_isprint('a'));
    TEST(ascii_isprint('z'));
    TEST(ascii_isprint('{'));
    TEST(ascii_isprint('~'));
    TEST(! ascii_isprint('\x7f'));
    TEST(! ascii_isprint('\x80'));

    TEST(! ascii_ispunct('\0'));
    TEST(! ascii_ispunct('\t'));
    TEST(! ascii_ispunct('\n'));
    TEST(! ascii_ispunct('\x1f'));
    TEST(! ascii_ispunct(' '));
    TEST(ascii_ispunct('!'));
    TEST(ascii_ispunct('/'));
    TEST(! ascii_ispunct('0'));
    TEST(! ascii_ispunct('9'));
    TEST(ascii_ispunct(':'));
    TEST(ascii_ispunct('@'));
    TEST(! ascii_ispunct('A'));
    TEST(! ascii_ispunct('Z'));
    TEST(ascii_ispunct('['));
    TEST(ascii_ispunct('_'));
    TEST(ascii_ispunct('`'));
    TEST(! ascii_ispunct('a'));
    TEST(! ascii_ispunct('z'));
    TEST(ascii_ispunct('{'));
    TEST(ascii_ispunct('~'));
    TEST(! ascii_ispunct('\x7f'));
    TEST(! ascii_ispunct('\x80'));

    TEST(! ascii_isspace('\0'));
    TEST(ascii_isspace('\t'));
    TEST(ascii_isspace('\n'));
    TEST(! ascii_isspace('\x1f'));
    TEST(ascii_isspace(' '));
    TEST(! ascii_isspace('!'));
    TEST(! ascii_isspace('/'));
    TEST(! ascii_isspace('0'));
    TEST(! ascii_isspace('9'));
    TEST(! ascii_isspace(':'));
    TEST(! ascii_isspace('@'));
    TEST(! ascii_isspace('A'));
    TEST(! ascii_isspace('Z'));
    TEST(! ascii_isspace('['));
    TEST(! ascii_isspace('_'));
    TEST(! ascii_isspace('`'));
    TEST(! ascii_isspace('a'));
    TEST(! ascii_isspace('z'));
    TEST(! ascii_isspace('{'));
    TEST(! ascii_isspace('~'));
    TEST(! ascii_isspace('\x7f'));
    TEST(! ascii_isspace('\x80'));

    TEST(! ascii_isupper('\0'));
    TEST(! ascii_isupper('\t'));
    TEST(! ascii_isupper('\n'));
    TEST(! ascii_isupper('\x1f'));
    TEST(! ascii_isupper(' '));
    TEST(! ascii_isupper('!'));
    TEST(! ascii_isupper('/'));
    TEST(! ascii_isupper('0'));
    TEST(! ascii_isupper('9'));
    TEST(! ascii_isupper(':'));
    TEST(! ascii_isupper('@'));
    TEST(ascii_isupper('A'));
    TEST(ascii_isupper('Z'));
    TEST(! ascii_isupper('['));
    TEST(! ascii_isupper('_'));
    TEST(! ascii_isupper('`'));
    TEST(! ascii_isupper('a'));
    TEST(! ascii_isupper('z'));
    TEST(! ascii_isupper('{'));
    TEST(! ascii_isupper('~'));
    TEST(! ascii_isupper('\x7f'));
    TEST(! ascii_isupper('\x80'));

    TEST(! ascii_isxdigit('\0'));
    TEST(! ascii_isxdigit('\t'));
    TEST(! ascii_isxdigit('\n'));
    TEST(! ascii_isxdigit('\x1f'));
    TEST(! ascii_isxdigit(' '));
    TEST(! ascii_isxdigit('!'));
    TEST(! ascii_isxdigit('/'));
    TEST(ascii_isxdigit('0'));
    TEST(ascii_isxdigit('9'));
    TEST(! ascii_isxdigit(':'));
    TEST(! ascii_isxdigit('@'));
    TEST(ascii_isxdigit('A'));
    TEST(!ascii_isxdigit('Z'));
    TEST(!ascii_isxdigit('['));
    TEST(!ascii_isxdigit('_'));
    TEST(!ascii_isxdigit('`'));
    TEST(ascii_isxdigit('a'));
    TEST(! ascii_isxdigit('z'));
    TEST(! ascii_isxdigit('{'));
    TEST(! ascii_isxdigit('~'));
    TEST(! ascii_isxdigit('\x7f'));
    TEST(! ascii_isxdigit('\x80'));

    TEST(! ascii_isalnum_w('\0'));
    TEST(! ascii_isalnum_w('\t'));
    TEST(! ascii_isalnum_w('\n'));
    TEST(! ascii_isalnum_w('\x1f'));
    TEST(! ascii_isalnum_w(' '));
    TEST(! ascii_isalnum_w('!'));
    TEST(! ascii_isalnum_w('/'));
    TEST(ascii_isalnum_w('0'));
    TEST(ascii_isalnum_w('9'));
    TEST(! ascii_isalnum_w(':'));
    TEST(! ascii_isalnum_w('@'));
    TEST(ascii_isalnum_w('A'));
    TEST(ascii_isalnum_w('Z'));
    TEST(! ascii_isalnum_w('['));
    TEST(ascii_isalnum_w('_'));
    TEST(! ascii_isalnum_w('`'));
    TEST(ascii_isalnum_w('a'));
    TEST(ascii_isalnum_w('z'));
    TEST(! ascii_isalnum_w('{'));
    TEST(! ascii_isalnum_w('~'));
    TEST(! ascii_isalnum_w('\x7f'));
    TEST(! ascii_isalnum_w('\x80'));

    TEST(! ascii_isalpha_w('\0'));
    TEST(! ascii_isalpha_w('\t'));
    TEST(! ascii_isalpha_w('\n'));
    TEST(! ascii_isalpha_w('\x1f'));
    TEST(! ascii_isalpha_w(' '));
    TEST(! ascii_isalpha_w('!'));
    TEST(! ascii_isalpha_w('/'));
    TEST(! ascii_isalpha_w('0'));
    TEST(! ascii_isalpha_w('9'));
    TEST(! ascii_isalpha_w(':'));
    TEST(! ascii_isalpha_w('@'));
    TEST(ascii_isalpha_w('A'));
    TEST(ascii_isalpha_w('Z'));
    TEST(! ascii_isalpha_w('['));
    TEST(ascii_isalpha_w('_'));
    TEST(! ascii_isalpha_w('`'));
    TEST(ascii_isalpha_w('a'));
    TEST(ascii_isalpha_w('z'));
    TEST(! ascii_isalpha_w('{'));
    TEST(! ascii_isalpha_w('~'));
    TEST(! ascii_isalpha_w('\x7f'));
    TEST(! ascii_isalpha_w('\x80'));

    TEST(! ascii_ispunct_w('\0'));
    TEST(! ascii_ispunct_w('\t'));
    TEST(! ascii_ispunct_w('\n'));
    TEST(! ascii_ispunct_w('\x1f'));
    TEST(! ascii_ispunct_w(' '));
    TEST(ascii_ispunct_w('!'));
    TEST(ascii_ispunct_w('/'));
    TEST(! ascii_ispunct_w('0'));
    TEST(! ascii_ispunct_w('9'));
    TEST(ascii_ispunct_w(':'));
    TEST(ascii_ispunct_w('@'));
    TEST(! ascii_ispunct_w('A'));
    TEST(! ascii_ispunct_w('Z'));
    TEST(ascii_ispunct_w('['));
    TEST(! ascii_ispunct_w('_'));
    TEST(ascii_ispunct_w('`'));
    TEST(! ascii_ispunct_w('a'));
    TEST(! ascii_ispunct_w('z'));
    TEST(ascii_ispunct_w('{'));
    TEST(ascii_ispunct_w('~'));
    TEST(! ascii_ispunct_w('\x7f'));
    TEST(! ascii_ispunct_w('\x80'));

    TEST(is_ascii('\0'));
    TEST(is_ascii('\x1f'));
    TEST(is_ascii(' '));
    TEST(is_ascii('\x7f'));
    TEST(! is_ascii('\x80'));
    TEST(! is_ascii('\xff'));

    TEST_EQUAL(ascii_tolower('A'), 'a');
    TEST_EQUAL(ascii_tolower('a'), 'a');
    TEST_EQUAL(ascii_tolower('\0'), '\0');
    TEST_EQUAL(ascii_tolower('@'), '@');
    TEST_EQUAL(ascii_tolower('\x7f'), '\x7f');
    TEST_EQUAL(ascii_tolower('\x80'), '\x80');
    TEST_EQUAL(ascii_tolower('\xff'), '\xff');

    TEST_EQUAL(ascii_toupper('A'), 'A');
    TEST_EQUAL(ascii_toupper('a'), 'A');
    TEST_EQUAL(ascii_toupper('\0'), '\0');
    TEST_EQUAL(ascii_toupper('@'), '@');
    TEST_EQUAL(ascii_toupper('\x7f'), '\x7f');
    TEST_EQUAL(ascii_toupper('\x80'), '\x80');
    TEST_EQUAL(ascii_toupper('\xff'), '\xff');

    TEST_EQUAL(ascii_lowercase("Hello World"s), "hello world");
    TEST_EQUAL(ascii_uppercase("Hello World"s), "HELLO WORLD");
    TEST_EQUAL(ascii_titlecase("hello world"s), "Hello World");
    TEST_EQUAL(ascii_titlecase("HELLO WORLD"s), "Hello World");

    TEST_EQUAL(ascii_sentencecase(""), "");
    TEST_EQUAL(ascii_sentencecase("hello world"), "Hello world");
    TEST_EQUAL(ascii_sentencecase("hello world. goodbye. hello again."), "Hello world. Goodbye. Hello again.");
    TEST_EQUAL(ascii_sentencecase("hello world\ngoodbye\nhello again"), "Hello world\ngoodbye\nhello again");
    TEST_EQUAL(ascii_sentencecase("hello world\n\ngoodbye\n\nhello again"), "Hello world\n\nGoodbye\n\nHello again");
    TEST_EQUAL(ascii_sentencecase("hello world\r\n\r\ngoodbye\r\n\r\nhello again"), "Hello world\r\n\r\nGoodbye\r\n\r\nHello again");

    TEST_EQUAL(bin(0, 1), "0");
    TEST_EQUAL(bin(0, 10), "0000000000");
    TEST_EQUAL(bin(42, 1), "101010");
    TEST_EQUAL(bin(42, 5), "101010");
    TEST_EQUAL(bin(42, 10), "0000101010");
    TEST_EQUAL(bin(0xabcdef, 20), "101010111100110111101111");
    TEST_EQUAL(bin(0xabcdef, 24), "101010111100110111101111");
    TEST_EQUAL(bin(0xabcdef, 28), "0000101010111100110111101111");
    TEST_EQUAL(bin(int8_t(0)), "00000000");
    TEST_EQUAL(bin(int16_t(0)), "0000000000000000");
    TEST_EQUAL(bin(int32_t(0)), "00000000000000000000000000000000");
    TEST_EQUAL(bin(int8_t(42)), "00101010");
    TEST_EQUAL(bin(int16_t(42)), "0000000000101010");
    TEST_EQUAL(bin(int32_t(42)), "00000000000000000000000000101010");
    TEST_EQUAL(bin(int8_t(-42)), "-00101010");
    TEST_EQUAL(bin(int16_t(-42)), "-0000000000101010");
    TEST_EQUAL(bin(int32_t(-42)), "-00000000000000000000000000101010");
    TEST_EQUAL(bin(uint32_t(0xabcdef)), "00000000101010111100110111101111");
    TEST_EQUAL(bin(uint64_t(0x123456789abcdefull)), "0000000100100011010001010110011110001001101010111100110111101111");

    TEST_EQUAL(dec(0), "0");
    TEST_EQUAL(dec(42), "42");
    TEST_EQUAL(dec(-42), "-42");
    TEST_EQUAL(dec(0, 4), "0000");
    TEST_EQUAL(dec(42, 4), "0042");
    TEST_EQUAL(dec(-42, 4), "-0042");

    TEST_EQUAL(hex(0, 1), "0");
    TEST_EQUAL(hex(0, 2), "00");
    TEST_EQUAL(hex(42, 1), "2a");
    TEST_EQUAL(hex(42, 2), "2a");
    TEST_EQUAL(hex(42, 3), "02a");
    TEST_EQUAL(hex(0xabcdef, 3), "abcdef");
    TEST_EQUAL(hex(0xabcdef, 6), "abcdef");
    TEST_EQUAL(hex(0xabcdef, 9), "000abcdef");
    TEST_EQUAL(hex(int8_t(0)), "00");
    TEST_EQUAL(hex(int16_t(0)), "0000");
    TEST_EQUAL(hex(int32_t(0)), "00000000");
    TEST_EQUAL(hex(int8_t(42)), "2a");
    TEST_EQUAL(hex(int16_t(42)), "002a");
    TEST_EQUAL(hex(int32_t(42)), "0000002a");
    TEST_EQUAL(hex(int8_t(-42)), "-2a");
    TEST_EQUAL(hex(int16_t(-42)), "-002a");
    TEST_EQUAL(hex(int32_t(-42)), "-0000002a");
    TEST_EQUAL(hex(uint32_t(0xabcdef)), "00abcdef");
    TEST_EQUAL(hex(uint64_t(0x123456789abcdefull)), "0123456789abcdef");

    TEST_EQUAL(binnum(""), 0);
    TEST_EQUAL(binnum("0"), 0);
    TEST_EQUAL(binnum("101010"), 42);
    TEST_EQUAL(binnum("000000101010"), 42);
    TEST_EQUAL(binnum("1111111111111111111111111111111111111111111111111111111111111110"), 0xfffffffffffffffeull);
    TEST_EQUAL(binnum("1111111111111111111111111111111111111111111111111111111111111111"), 0xffffffffffffffffull);
    TEST_EQUAL(binnum("10000000000000000000000000000000000000000000000000000000000000000"), 0xffffffffffffffffull);

    TEST_EQUAL(decnum(""), 0);
    TEST_EQUAL(decnum("0"), 0);
    TEST_EQUAL(decnum("42"), 42);
    TEST_EQUAL(decnum("0042"), 42);
    TEST_EQUAL(decnum("+42"), 42);
    TEST_EQUAL(decnum("-42"), -42);
    TEST_EQUAL(decnum("9223372036854775806"), 9'223'372'036'854'775'806ll); // 2^63-2
    TEST_EQUAL(decnum("9223372036854775807"), 9'223'372'036'854'775'807ll); // 2^63-1
    TEST_EQUAL(decnum("9223372036854775808"), 9'223'372'036'854'775'807ll); // 2^63
    TEST_EQUAL(decnum("9223372036854775809"), 9'223'372'036'854'775'807ll); // 2^63+1
    TEST_EQUAL(decnum("-9223372036854775806"), -9'223'372'036'854'775'806ll); // -(2^63-2)
    TEST_EQUAL(decnum("-9223372036854775807"), -9'223'372'036'854'775'807ll); // -(2^63-1)
    TEST_EQUAL(decnum("-9223372036854775808"), -9'223'372'036'854'775'807ll-1); // -2^63
    TEST_EQUAL(decnum("-9223372036854775809"), -9'223'372'036'854'775'807ll-1); // -(2^63+1)

    TEST_EQUAL(hexnum(""), 0);
    TEST_EQUAL(hexnum("0"), 0);
    TEST_EQUAL(hexnum("42"), 66);
    TEST_EQUAL(hexnum("0042"), 66);
    TEST_EQUAL(hexnum("fffffffffffffffe"), 0xfffffffffffffffeull);
    TEST_EQUAL(hexnum("ffffffffffffffff"), 0xffffffffffffffffull);
    TEST_EQUAL(hexnum("10000000000000000"), 0xffffffffffffffffull);

    TEST_EQUAL(fpnum(""), 0);
    TEST_EQUAL(fpnum("42"), 42);
    TEST_EQUAL(fpnum("-42"), -42);
    TEST_EQUAL(fpnum("1.234e5"), 123400);
    TEST_EQUAL(fpnum("-1.234e5"), -123400);
    TEST_NEAR(fpnum("1.23e-4"), 0.000123);
    TEST_NEAR(fpnum("-1.23e-4"), -0.000123);
    TEST_EQUAL(fpnum("1e9999"), HUGE_VAL);
    TEST_EQUAL(fpnum("-1e9999"), - HUGE_VAL);

    TEST_EQUAL(fp_format(0), "0");
    TEST_EQUAL(fp_format(0, 'e', 3), "0.000e0");
    TEST_EQUAL(fp_format(42, 'e', 3), "4.200e1");
    TEST_EQUAL(fp_format(-42, 'e', 3), "-4.200e1");
    TEST_EQUAL(fp_format(1.23456e8, 'e', 3), "1.235e8");
    TEST_EQUAL(fp_format(1.23456e-6, 'e', 3), "1.235e-6");
    TEST_EQUAL(fp_format(0, 'f', 3), "0.000");
    TEST_EQUAL(fp_format(42, 'f', 3), "42.000");
    TEST_EQUAL(fp_format(-42, 'f', 3), "-42.000");
    TEST_EQUAL(fp_format(1.23456e8, 'f', 3), "123456000.000");
    TEST_EQUAL(fp_format(1.23456e-6, 'f', 3), "0.000");
    TEST_EQUAL(fp_format(0, 'g', 3), "0");
    TEST_EQUAL(fp_format(42, 'g', 3), "42");
    TEST_EQUAL(fp_format(-42, 'g', 3), "-42");
    TEST_EQUAL(fp_format(1.23456e8, 'g', 3), "1.23e8");
    TEST_EQUAL(fp_format(1.23456e-6, 'g', 3), "1.23e-6");
    TEST_EQUAL(fp_format(0, 'z', 3), "0.00");
    TEST_EQUAL(fp_format(42, 'z', 3), "42.0");
    TEST_EQUAL(fp_format(-42, 'z', 3), "-42.0");
    TEST_EQUAL(fp_format(1.23456e8, 'z', 3), "1.23e8");
    TEST_EQUAL(fp_format(1.23456e-6, 'z', 3), "1.23e-6");
    TEST_EQUAL(fp_format(0.123, 'z', 3), "0.123");
    TEST_EQUAL(fp_format(1.23, 'z', 3), "1.23");
    TEST_EQUAL(fp_format(12.3, 'z', 3), "12.3");
    TEST_EQUAL(fp_format(123, 'z', 3), "123");

    TEST_EQUAL(opt_fp_format(0), "0");
    TEST_EQUAL(opt_fp_format(42), "42");
    TEST_EQUAL(opt_fp_format(42, 'f', 4), "42");
    TEST_EQUAL(opt_fp_format(42.0), "42");
    TEST_EQUAL(opt_fp_format(42.0, 'f', 4), "42.0000");
    TEST_EQUAL(opt_fp_format("Hello"), "Hello");

    std::vector<int> iv;
    Strings sv;
    std::map<int, std::string> ism;

    TEST_EQUAL(format_list(iv), "[]");
    TEST_EQUAL(format_list(sv), "[]");
    TEST_EQUAL(format_map(ism), "{}");
    TEST_EQUAL(format_list(iv, "<", "/", ">"), "<>");
    TEST_EQUAL(format_list(sv, "<", "/", ">"), "<>");
    TEST_EQUAL(format_map(ism, "<", "=", "/", ">"), "<>");
    TEST_EQUAL(to_str(iv), "[]");
    TEST_EQUAL(to_str(sv), "[]");
    TEST_EQUAL(to_str(ism), "{}");

    iv.push_back(1);
    sv.push_back("hello");
    ism[1] = "hello";

    TEST_EQUAL(format_list(iv), "[1]");
    TEST_EQUAL(format_list(sv), "[hello]");
    TEST_EQUAL(format_map(ism), "{1:hello}");
    TEST_EQUAL(format_list(iv, "<", "/", ">"), "<1>");
    TEST_EQUAL(format_list(sv, "<", "/", ">"), "<hello>");
    TEST_EQUAL(format_map(ism, "<", "=", "/", ">"), "<1=hello>");
    TEST_EQUAL(to_str(iv), "[1]");
    TEST_EQUAL(to_str(sv), "[hello]");
    TEST_EQUAL(to_str(ism), "{1:hello}");

    iv = {1,2,3};
    sv = {"hello","world","goodbye"};
    ism = {{1,"hello"},{2,"world"},{3,"goodbye"}};

    TEST_EQUAL(format_list(iv), "[1,2,3]");
    TEST_EQUAL(format_list(sv), "[hello,world,goodbye]");
    TEST_EQUAL(format_map(ism), "{1:hello,2:world,3:goodbye}");
    TEST_EQUAL(format_list(iv, "<", "/", ">"), "<1/2/3>");
    TEST_EQUAL(format_list(sv, "<", "/", ">"), "<hello/world/goodbye>");
    TEST_EQUAL(format_map(ism, "<", "=", "/", ">"), "<1=hello/2=world/3=goodbye>");
    TEST_EQUAL(to_str(iv), "[1,2,3]");
    TEST_EQUAL(to_str(sv), "[hello,world,goodbye]");
    TEST_EQUAL(to_str(ism), "{1:hello,2:world,3:goodbye}");

    std::string s;

    TRY(s = quote(""s));                      TEST_EQUAL(s, "\"\""s);
    TRY(s = quote("\"\""s));                  TEST_EQUAL(s, "\"\\\"\\\"\""s);
    TRY(s = quote("Hello world"s));           TEST_EQUAL(s, "\"Hello world\""s);
    TRY(s = quote("\\Hello\\world\\"s));      TEST_EQUAL(s, "\"\\\\Hello\\\\world\\\\\""s);
    TRY(s = quote("\"Hello\" \"world\""s));   TEST_EQUAL(s, "\"\\\"Hello\\\" \\\"world\\\"\""s);
    TRY(s = quote("\t\n\f\r"s));              TEST_EQUAL(s, "\"\\t\\n\\f\\r\""s);
    TRY(s = quote(u8"åß∂ƒ"s));                TEST_EQUAL(s, u8"\"åß∂ƒ\""s);

    TRY(s = bquote(""s));                      TEST_EQUAL(s, "\"\""s);
    TRY(s = bquote("\"\""s));                  TEST_EQUAL(s, "\"\\\"\\\"\""s);
    TRY(s = bquote("Hello world"s));           TEST_EQUAL(s, "\"Hello world\""s);
    TRY(s = bquote("\\Hello\\world\\"s));      TEST_EQUAL(s, "\"\\\\Hello\\\\world\\\\\""s);
    TRY(s = bquote("\"Hello\" \"world\""s));   TEST_EQUAL(s, "\"\\\"Hello\\\" \\\"world\\\"\""s);
    TRY(s = bquote("\t\n\f\r"s));              TEST_EQUAL(s, "\"\\t\\n\\f\\r\""s);
    TRY(s = bquote(u8"åß∂ƒ"s));                TEST_EQUAL(s, "\"\\xc3\\xa5\\xc3\\x9f\\xe2\\x88\\x82\\xc6\\x92\""s);
    TRY(s = bquote("\x00\x01\x7f\x80\xff"s));  TEST_EQUAL(s, "\"\\0\\x01\\x7f\\x80\\xff\""s);

    TEST_EQUAL(roman(0), "");
    TEST_EQUAL(roman(1), "I");
    TEST_EQUAL(roman(42), "XLII");
    TEST_EQUAL(roman(1111), "MCXI");
    TEST_EQUAL(roman(2222), "MMCCXXII");
    TEST_EQUAL(roman(3333), "MMMCCCXXXIII");
    TEST_EQUAL(roman(4444), "MMMMCDXLIV");
    TEST_EQUAL(roman(5555), "MMMMMDLV");
    TEST_EQUAL(roman(6666), "MMMMMMDCLXVI");
    TEST_EQUAL(roman(7777), "MMMMMMMDCCLXXVII");
    TEST_EQUAL(roman(8888), "MMMMMMMMDCCCLXXXVIII");
    TEST_EQUAL(roman(9999), "MMMMMMMMMCMXCIX");

    TEST_EQUAL(si_to_int("0"), 0);
    TEST_EQUAL(si_to_int("42"), 42);
    TEST_EQUAL(si_to_int("-42"), -42);
    TEST_EQUAL(si_to_int("0k"), 0);
    TEST_EQUAL(si_to_int("123k"), 123'000);
    TEST_EQUAL(si_to_int("123K"), 123'000);
    TEST_EQUAL(si_to_int("123 k"), 123'000);
    TEST_EQUAL(si_to_int("123 K"), 123'000);
    TEST_EQUAL(si_to_int("123 M"), 123'000'000ll);
    TEST_EQUAL(si_to_int("123 G"), 123'000'000'000ll);
    TEST_EQUAL(si_to_int("123 T"), 123'000'000'000'000ll);
    TEST_EQUAL(si_to_int("123 P"), 123'000'000'000'000'000ll);
    TEST_THROW(si_to_int("123 E"), std::range_error);
    TEST_THROW(si_to_int("123 Z"), std::range_error);
    TEST_THROW(si_to_int("123 Y"), std::range_error);
    TEST_EQUAL(si_to_int("-123k"), -123'000);
    TEST_EQUAL(si_to_int("-123K"), -123'000);
    TEST_EQUAL(si_to_int("-123 k"), -123'000);
    TEST_EQUAL(si_to_int("-123 K"), -123'000);
    TEST_THROW(si_to_int(""), std::invalid_argument);
    TEST_THROW(si_to_int("k9"), std::invalid_argument);

    TEST_EQUAL(si_to_float("0"), 0);
    TEST_EQUAL(si_to_float("42"), 42);
    TEST_EQUAL(si_to_float("-42"), -42);
    TEST_EQUAL(si_to_float("1234.5"), 1234.5);
    TEST_EQUAL(si_to_float("-1234.5"), -1234.5);
    TEST_EQUAL(si_to_float("0k"), 0);
    TEST_EQUAL(si_to_float("12.34k"), 12.34e3);
    TEST_EQUAL(si_to_float("12.34K"), 12.34e3);
    TEST_EQUAL(si_to_float("12.34 k"), 12.34e3);
    TEST_EQUAL(si_to_float("12.34 K"), 12.34e3);
    TEST_EQUAL(si_to_float("12.34 M"), 12.34e6);
    TEST_EQUAL(si_to_float("12.34 G"), 12.34e9);
    TEST_EQUAL(si_to_float("12.34 T"), 12.34e12);
    TEST_EQUAL(si_to_float("12.34 P"), 12.34e15);
    TEST_EQUAL(si_to_float("12.34 E"), 12.34e18);
    TEST_EQUAL(si_to_float("12.34 Z"), 12.34e21);
    TEST_EQUAL(si_to_float("12.34 Y"), 12.34e24);
    TEST_NEAR_EPSILON(si_to_float("12.34 m"), 12.34e-3, 1e-9);
    TEST_NEAR_EPSILON(si_to_float("12.34 u"), 12.34e-6, 1e-12);
    TEST_NEAR_EPSILON(si_to_float("12.34 n"), 12.34e-9, 1e-15);
    TEST_NEAR_EPSILON(si_to_float("12.34 p"), 12.34e-12, 1e-18);
    TEST_NEAR_EPSILON(si_to_float("12.34 f"), 12.34e-15, 1e-21);
    TEST_NEAR_EPSILON(si_to_float("12.34 a"), 12.34e-18, 1e-24);
    TEST_NEAR_EPSILON(si_to_float("12.34 z"), 12.34e-21, 1e-27);
    TEST_NEAR_EPSILON(si_to_float("12.34 y"), 12.34e-24, 1e-30);
    TEST_EQUAL(si_to_float("-12.34k"), -12.34e3);
    TEST_EQUAL(si_to_float("-12.34K"), -12.34e3);
    TEST_EQUAL(si_to_float("-12.34 k"), -12.34e3);
    TEST_EQUAL(si_to_float("-12.34 K"), -12.34e3);
    TEST_THROW(si_to_float("1e999999"), std::range_error);
    TEST_THROW(si_to_float(""), std::invalid_argument);
    TEST_THROW(si_to_float("k9"), std::invalid_argument);

    s = "Hello";

    TEST_EQUAL(to_str(true), "true");
    TEST_EQUAL(to_str(false), "false");
    TEST_EQUAL(to_str(0), "0");
    TEST_EQUAL(to_str(42), "42");
    TEST_EQUAL(to_str(-42), "-42");
    TEST_EQUAL(to_str(123.456), "123.456");
    TEST_EQUAL(to_str(s), "Hello");
    TEST_EQUAL(to_str(s.data()), "Hello");
    TEST_EQUAL(to_str(""s), "");
    TEST_EQUAL(to_str("Hello"s), "Hello");
    TEST_EQUAL(to_str('X'), "X");

    std::atomic<int> ai(42);                                      TEST_EQUAL(to_str(ai), "42");
    std::pair<int, std::string> is = {42, "Hello"};               TEST_EQUAL(to_str(is), "(42,Hello)");
    std::tuple<> t0 = {};                                         TEST_EQUAL(to_str(t0), "()");
    std::tuple<int> t1 = {42};                                    TEST_EQUAL(to_str(t1), "(42)");
    std::tuple<int, std::string> t2 = {42, "Hello"};              TEST_EQUAL(to_str(t2), "(42,Hello)");
    std::tuple<int, std::string, bool> t3 = {42, "Hello", true};  TEST_EQUAL(to_str(t3), "(42,Hello,true)");
    std::runtime_error ex1("Runtime error");                      TEST_EQUAL(to_str(ex1), "Runtime error");
    std::invalid_argument ex2("Invalid argument");                TEST_EQUAL(to_str(ex2), "Invalid argument");
    std::array<uint8_t, 5> ua = {{10,20,30,40,50}};               TEST_EQUAL(to_str(ua), "0a141e2832");
    std::vector<uint8_t> uv = {{60,70,80,90,100}};                TEST_EQUAL(to_str(uv), "3c46505a64");

}

void test_unicorn_utility_version_number() {

    Version v1, v2;

    TEST_EQUAL(v1[0], 0);
    TEST_EQUAL(v1[1], 0);
    TEST_EQUAL(v1[2], 0);
    TEST_EQUAL(v1.major(), 0);
    TEST_EQUAL(v1.minor(), 0);
    TEST_EQUAL(v1.patch(), 0);
    TEST_EQUAL(v1.suffix(), "");
    TEST_EQUAL(v1.size(), 1);
    TEST_EQUAL(v1.str(), "0.0");
    TEST_EQUAL(v1.str(0), "");
    TEST_EQUAL(v1.str(1), "0");
    TEST_EQUAL(v1.str(2), "0.0");
    TEST_EQUAL(v1.str(3), "0.0.0");
    TEST_EQUAL(v1.str(4), "0.0.0.0");
    TEST_EQUAL(v1.str(4, '-'), "0-0-0-0");
    TEST_EQUAL(v1.to32(), 0);

    TRY((v1 = {42}));
    TEST_EQUAL(v1[0], 42);
    TEST_EQUAL(v1[1], 0);
    TEST_EQUAL(v1[2], 0);
    TEST_EQUAL(v1.major(), 42);
    TEST_EQUAL(v1.minor(), 0);
    TEST_EQUAL(v1.patch(), 0);
    TEST_EQUAL(v1.suffix(), "");
    TEST_EQUAL(v1.size(), 1);
    TEST_EQUAL(v1.str(), "42.0");
    TEST_EQUAL(v1.to32(), 0x2a000000);

    TRY((v1 = {42,"beta"}));
    TEST_EQUAL(v1[0], 42);
    TEST_EQUAL(v1[1], 0);
    TEST_EQUAL(v1[2], 0);
    TEST_EQUAL(v1.major(), 42);
    TEST_EQUAL(v1.minor(), 0);
    TEST_EQUAL(v1.patch(), 0);
    TEST_EQUAL(v1.suffix(), "beta");
    TEST_EQUAL(v1.size(), 1);
    TEST_EQUAL(v1.str(), "42.0beta");
    TEST_EQUAL(v1.to32(), 0x2a000000);

    TRY((v1 = {1,2,3}));
    TEST_EQUAL(v1[0], 1);
    TEST_EQUAL(v1[1], 2);
    TEST_EQUAL(v1[2], 3);
    TEST_EQUAL(v1.major(), 1);
    TEST_EQUAL(v1.minor(), 2);
    TEST_EQUAL(v1.patch(), 3);
    TEST_EQUAL(v1.suffix(), "");
    TEST_EQUAL(v1.size(), 3);
    TEST_EQUAL(v1.str(), "1.2.3");
    TEST_EQUAL(v1.str(0), "1.2.3");
    TEST_EQUAL(v1.str(1), "1.2.3");
    TEST_EQUAL(v1.str(2), "1.2.3");
    TEST_EQUAL(v1.str(3), "1.2.3");
    TEST_EQUAL(v1.str(4), "1.2.3.0");
    TEST_EQUAL(v1.str(4, '-'), "1-2-3-0");
    TEST_EQUAL(v1.to32(), 0x01020300);

    TRY((v1 = {1,2,3,"beta"}));
    TEST_EQUAL(v1[0], 1);
    TEST_EQUAL(v1[1], 2);
    TEST_EQUAL(v1[2], 3);
    TEST_EQUAL(v1.major(), 1);
    TEST_EQUAL(v1.minor(), 2);
    TEST_EQUAL(v1.patch(), 3);
    TEST_EQUAL(v1.suffix(), "beta");
    TEST_EQUAL(v1.size(), 3);
    TEST_EQUAL(v1.str(), "1.2.3beta");
    TEST_EQUAL(v1.str(0), "1.2.3beta");
    TEST_EQUAL(v1.str(1), "1.2.3beta");
    TEST_EQUAL(v1.str(2), "1.2.3beta");
    TEST_EQUAL(v1.str(3), "1.2.3beta");
    TEST_EQUAL(v1.str(4), "1.2.3.0beta");
    TEST_EQUAL(v1.to32(), 0x01020300);

    TRY(v1 = Version::from32(0x12345678));
    TEST_EQUAL(v1[0], 18);
    TEST_EQUAL(v1[1], 52);
    TEST_EQUAL(v1[2], 86);
    TEST_EQUAL(v1[3], 120);
    TEST_EQUAL(v1.major(), 18);
    TEST_EQUAL(v1.minor(), 52);
    TEST_EQUAL(v1.patch(), 86);
    TEST_EQUAL(v1.suffix(), "");
    TEST_EQUAL(v1.size(), 4);
    TEST_EQUAL(v1.str(), "18.52.86.120");
    TEST_EQUAL(v1.to32(), 0x12345678);

    TRY(v1 = Version(""));
    TEST_EQUAL(v1[0], 0);
    TEST_EQUAL(v1[1], 0);
    TEST_EQUAL(v1[2], 0);
    TEST_EQUAL(v1.major(), 0);
    TEST_EQUAL(v1.minor(), 0);
    TEST_EQUAL(v1.patch(), 0);
    TEST_EQUAL(v1.suffix(), "");
    TEST_EQUAL(v1.size(), 1);
    TEST_EQUAL(v1.str(), "0.0");
    TEST_EQUAL(v1.to32(), 0);

    TRY(v1 = Version("1.2.3"));
    TEST_EQUAL(v1[0], 1);
    TEST_EQUAL(v1[1], 2);
    TEST_EQUAL(v1[2], 3);
    TEST_EQUAL(v1.major(), 1);
    TEST_EQUAL(v1.minor(), 2);
    TEST_EQUAL(v1.patch(), 3);
    TEST_EQUAL(v1.suffix(), "");
    TEST_EQUAL(v1.size(), 3);
    TEST_EQUAL(v1.str(), "1.2.3");
    TEST_EQUAL(v1.to32(), 0x01020300);

    TRY(v1 = Version("1.2.3beta"));
    TEST_EQUAL(v1[0], 1);
    TEST_EQUAL(v1[1], 2);
    TEST_EQUAL(v1[2], 3);
    TEST_EQUAL(v1.major(), 1);
    TEST_EQUAL(v1.minor(), 2);
    TEST_EQUAL(v1.patch(), 3);
    TEST_EQUAL(v1.suffix(), "beta");
    TEST_EQUAL(v1.size(), 3);
    TEST_EQUAL(v1.str(), "1.2.3beta");
    TEST_EQUAL(v1.to32(), 0x01020300);

    TRY(v1 = Version("1.2.3 beta"));
    TEST_EQUAL(v1[0], 1);
    TEST_EQUAL(v1[1], 2);
    TEST_EQUAL(v1[2], 3);
    TEST_EQUAL(v1.major(), 1);
    TEST_EQUAL(v1.minor(), 2);
    TEST_EQUAL(v1.patch(), 3);
    TEST_EQUAL(v1.suffix(), "");
    TEST_EQUAL(v1.size(), 3);
    TEST_EQUAL(v1.str(), "1.2.3");
    TEST_EQUAL(v1.to32(), 0x01020300);

    TRY(v1 = Version("beta"));
    TEST_EQUAL(v1[0], 0);
    TEST_EQUAL(v1[1], 0);
    TEST_EQUAL(v1[2], 0);
    TEST_EQUAL(v1.major(), 0);
    TEST_EQUAL(v1.minor(), 0);
    TEST_EQUAL(v1.patch(), 0);
    TEST_EQUAL(v1.size(), 1);
    TEST_EQUAL(v1.suffix(), "beta");
    TEST_EQUAL(v1.str(), "0.0beta");
    TEST_EQUAL(v1.to32(), 0);

    TRY(v1 = Version());            TRY(v2 = Version());           TEST_COMPARE(v1, ==, v2);
    TRY(v1 = Version());            TRY(v2 = Version("1.2.3"));    TEST_COMPARE(v1, <, v2);
    TRY(v1 = Version("1.2"));       TRY(v2 = Version("1"));        TEST_COMPARE(v1, >, v2);
    TRY(v1 = Version("1.2"));       TRY(v2 = Version("1.2"));      TEST_COMPARE(v1, ==, v2);
    TRY(v1 = Version("1.2"));       TRY(v2 = Version("1.1.1"));    TEST_COMPARE(v1, >, v2);
    TRY(v1 = Version("1.2"));       TRY(v2 = Version("1.2.1"));    TEST_COMPARE(v1, <, v2);
    TRY(v1 = Version("1.2"));       TRY(v2 = Version("1.2beta"));  TEST_COMPARE(v1, >, v2);
    TRY(v1 = Version("1.2alpha"));  TRY(v2 = Version("1.2beta"));  TEST_COMPARE(v1, <, v2);

}
