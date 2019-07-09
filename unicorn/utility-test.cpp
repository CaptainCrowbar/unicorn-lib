#include "unicorn/utility.hpp"
#include "unicorn/unit-test.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <functional>
#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using namespace RS;
using namespace RS::Meta;
using namespace std::chrono;
using namespace std::literals;

namespace {

    RS_ENUM(FooEnum, int16_t, 0, alpha, bravo, charlie);
    RS_ENUM(BarEnum, int16_t, 0, delta, echo, foxtrot);
    RS_ENUM_CLASS(ZapEnum, int32_t, 1, golf, hotel, india);
    RS_ENUM_CLASS(ThogEnum, int32_t, 1, golf, hotel, india);

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

    class Base {
    public:
        virtual ~Base() noexcept {}
        virtual int get() const = 0;
    };

    class Derived1: public Base {
    public:
        virtual int get() const { return 1; }
    };

    class Derived2: public Base {
    public:
        virtual int get() const { return 2; }
    };

    enum class Mask { z = 0, a = 1, b = 2 };
    RS_BITMASK_OPERATORS(Mask);

    struct Wrap {
        enum class mask { z = 0, a = 1, b = 2 };
    };
    RS_BITMASK_OPERATORS(Wrap::mask);

}

void test_unicorn_utility_preprocessor_macros() {

    FooEnum f = {};
    ZapEnum z = {};
    std::vector<FooEnum> vf;
    std::vector<ZapEnum> vz;

    TEST_TYPE(std::underlying_type_t<FooEnum>, int16_t);
    TEST_TYPE(std::underlying_type_t<ZapEnum>, int32_t);

    TEST_EQUAL(sizeof(FooEnum), 2);
    TEST_EQUAL(sizeof(ZapEnum), 4);

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

    TEST_EQUAL(int(ZapEnum::golf), 1);
    TEST_EQUAL(int(ZapEnum::hotel), 2);
    TEST_EQUAL(int(ZapEnum::india), 3);

    TEST_EQUAL(make_str(ZapEnum::golf), "ZapEnum::golf");
    TEST_EQUAL(make_str(ZapEnum::hotel), "ZapEnum::hotel");
    TEST_EQUAL(make_str(ZapEnum::india), "ZapEnum::india");
    TEST_EQUAL(make_str(ZapEnum(0)), "0");
    TEST_EQUAL(make_str(ZapEnum(4)), "4");
    TEST_EQUAL(make_str(ZapEnum(99)), "99");

    TEST(! enum_is_valid(ZapEnum(0)));
    TEST(enum_is_valid(ZapEnum(1)));
    TEST(enum_is_valid(ZapEnum(2)));
    TEST(enum_is_valid(ZapEnum(3)));
    TEST(! enum_is_valid(ZapEnum(4)));

    TRY(vz = enum_values<ZapEnum>());
    TEST_EQUAL(vz.size(), 3);
    TEST_EQUAL(make_str(vz), "[ZapEnum::golf,ZapEnum::hotel,ZapEnum::india]");

    TEST(str_to_enum("alpha", f));             TEST_EQUAL(f, alpha);
    TEST(str_to_enum("bravo", f));             TEST_EQUAL(f, bravo);
    TEST(str_to_enum("charlie", f));           TEST_EQUAL(f, charlie);
    TEST(! str_to_enum("zulu", f));
    TEST(str_to_enum("FooEnum::alpha", f));    TEST_EQUAL(f, alpha);
    TEST(str_to_enum("FooEnum::bravo", f));    TEST_EQUAL(f, bravo);
    TEST(str_to_enum("FooEnum::charlie", f));  TEST_EQUAL(f, charlie);
    TEST(! str_to_enum("FooEnum::zulu", f));

    TEST(str_to_enum("golf", z));             TEST_EQUAL(z, ZapEnum::golf);
    TEST(str_to_enum("hotel", z));            TEST_EQUAL(z, ZapEnum::hotel);
    TEST(str_to_enum("india", z));            TEST_EQUAL(z, ZapEnum::india);
    TEST(! str_to_enum("zulu", z));
    TEST(str_to_enum("ZapEnum::golf", z));    TEST_EQUAL(z, ZapEnum::golf);
    TEST(str_to_enum("ZapEnum::hotel", z));   TEST_EQUAL(z, ZapEnum::hotel);
    TEST(str_to_enum("ZapEnum::india", z));   TEST_EQUAL(z, ZapEnum::india);
    TEST(! str_to_enum("ZapEnum::zulu", z));

    Mask m1, m2, m3;

    m1 = Mask::a;  TEST_EQUAL(int(m1), 1);
    m2 = Mask::b;  TEST_EQUAL(int(m2), 2);
    m3 = m1 & m2;  TEST_EQUAL(int(m3), 0);
    m3 = m1 | m2;  TEST_EQUAL(int(m3), 3);
    m3 = m1 ^ m2;  TEST_EQUAL(int(m3), 3);

    Wrap::mask w1, w2, w3;

    w1 = Wrap::mask::a;  TEST_EQUAL(int(w1), 1);
    w2 = Wrap::mask::b;  TEST_EQUAL(int(w2), 2);
    w3 = w1 & w2;        TEST_EQUAL(int(w3), 0);
    w3 = w1 | w2;        TEST_EQUAL(int(w3), 3);
    w3 = w1 ^ w2;        TEST_EQUAL(int(w3), 3);

}

void test_unicorn_utility_constants() {

    uint16_t u16_1 = 1;
    uint8_t u8_2[2];

    std::memcpy(u8_2, &u16_1, 2);
    TEST_COMPARE(little_endian_target, !=, big_endian_target);
    TEST_EQUAL(bool(u8_2[0]), little_endian_target);
    TEST_EQUAL(bool(u8_2[1]), big_endian_target);

    static constexpr auto b0 = setbit<0>;    TEST_TYPE_OF(b0, uint8_t);    TEST_EQUAL(b0, 1);
    static constexpr auto b1 = setbit<1>;    TEST_TYPE_OF(b1, uint8_t);    TEST_EQUAL(b1, 2);
    static constexpr auto b7 = setbit<7>;    TEST_TYPE_OF(b7, uint8_t);    TEST_EQUAL(b7, 0x80);
    static constexpr auto b8 = setbit<8>;    TEST_TYPE_OF(b8, uint16_t);   TEST_EQUAL(b8, 0x100);
    static constexpr auto b9 = setbit<9>;    TEST_TYPE_OF(b9, uint16_t);   TEST_EQUAL(b9, 0x200);
    static constexpr auto b15 = setbit<15>;  TEST_TYPE_OF(b15, uint16_t);  TEST_EQUAL(b15, 0x8000);
    static constexpr auto b16 = setbit<16>;  TEST_TYPE_OF(b16, uint32_t);  TEST_EQUAL(b16, 0x1'0000ul);
    static constexpr auto b17 = setbit<17>;  TEST_TYPE_OF(b17, uint32_t);  TEST_EQUAL(b17, 0x2'0000ul);
    static constexpr auto b31 = setbit<31>;  TEST_TYPE_OF(b31, uint32_t);  TEST_EQUAL(b31, 0x8000'0000ul);
    static constexpr auto b32 = setbit<32>;  TEST_TYPE_OF(b32, uint64_t);  TEST_EQUAL(b32, 0x1'0000'0000ull);
    static constexpr auto b33 = setbit<33>;  TEST_TYPE_OF(b33, uint64_t);  TEST_EQUAL(b33, 0x2'0000'0000ull);
    static constexpr auto b63 = setbit<63>;  TEST_TYPE_OF(b63, uint64_t);  TEST_EQUAL(b63, 0x8000'0000'0000'0000ull);

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

    TEST_EQUAL(popcount(0), 0);
    TEST_EQUAL(popcount(1), 1);
    TEST_EQUAL(popcount(2), 1);
    TEST_EQUAL(popcount(3), 2);
    TEST_EQUAL(popcount(4), 1);
    TEST_EQUAL(popcount(5), 2);
    TEST_EQUAL(popcount(6), 2);
    TEST_EQUAL(popcount(7), 3);
    TEST_EQUAL(popcount(8), 1);
    TEST_EQUAL(popcount(9), 2);
    TEST_EQUAL(popcount(10), 2);
    TEST_EQUAL(popcount(0x7fff), 15);
    TEST_EQUAL(popcount(0x8000), 1);
    TEST_EQUAL(popcount(0x8888), 4);
    TEST_EQUAL(popcount(0xffff), 16);
    TEST_EQUAL(popcount(0x7fffffffffffffffull), 63);
    TEST_EQUAL(popcount(0x8000000000000000ull), 1);
    TEST_EQUAL(popcount(0xffffffffffffffffull), 64);

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

    Irange<Ustring::iterator> r;
    Irange<Ustring::const_iterator> cr;
    Ustring s = "Hello", t;

    TRY((r = {s.begin(), s.end()}));
    TEST_EQUAL(r.size(), 5);
    TRY(std::copy(r.begin(), r.end(), overwrite(t)));
    TEST_EQUAL(t, "Hello");
    TRY(cr = r);
    TEST_EQUAL(cr.size(), 5);
    TRY(std::copy(cr.begin(), cr.end(), overwrite(t)));
    TEST_EQUAL(t, "Hello");

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

    iv.push_back(1);
    sv.push_back("hello");
    ism[1] = "hello";

    TEST_EQUAL(format_list(iv), "[1]");
    TEST_EQUAL(format_list(sv), "[hello]");
    TEST_EQUAL(format_map(ism), "{1:hello}");
    TEST_EQUAL(format_list(iv, "<", "/", ">"), "<1>");
    TEST_EQUAL(format_list(sv, "<", "/", ">"), "<hello>");
    TEST_EQUAL(format_map(ism, "<", "=", "/", ">"), "<1=hello>");

    iv = {1,2,3};
    sv = {"hello","world","goodbye"};
    ism = {{1,"hello"},{2,"world"},{3,"goodbye"}};

    TEST_EQUAL(format_list(iv), "[1,2,3]");
    TEST_EQUAL(format_list(sv), "[hello,world,goodbye]");
    TEST_EQUAL(format_map(ism), "{1:hello,2:world,3:goodbye}");
    TEST_EQUAL(format_list(iv, "<", "/", ">"), "<1/2/3>");
    TEST_EQUAL(format_list(sv, "<", "/", ">"), "<hello/world/goodbye>");
    TEST_EQUAL(format_map(ism, "<", "=", "/", ">"), "<1=hello/2=world/3=goodbye>");

    std::string s;

    TRY(s = quote(""s));                     TEST_EQUAL(s, "\"\""s);
    TRY(s = quote("\"\""s));                 TEST_EQUAL(s, "\"\\\"\\\"\""s);
    TRY(s = quote("Hello world"s));          TEST_EQUAL(s, "\"Hello world\""s);
    TRY(s = quote("\\Hello\\world\\"s));     TEST_EQUAL(s, "\"\\\\Hello\\\\world\\\\\""s);
    TRY(s = quote("\"Hello\" \"world\""s));  TEST_EQUAL(s, "\"\\\"Hello\\\" \\\"world\\\"\""s);
    TRY(s = quote("\t\n\f\r"s));             TEST_EQUAL(s, "\"\\t\\n\\f\\r\""s);
    TRY(s = quote("åß∂ƒ"s));                 TEST_EQUAL(s, "\"åß∂ƒ\""s);

    TRY(s = bquote(""s));                      TEST_EQUAL(s, "\"\""s);
    TRY(s = bquote("\"\""s));                  TEST_EQUAL(s, "\"\\\"\\\"\""s);
    TRY(s = bquote("Hello world"s));           TEST_EQUAL(s, "\"Hello world\""s);
    TRY(s = bquote("\\Hello\\world\\"s));      TEST_EQUAL(s, "\"\\\\Hello\\\\world\\\\\""s);
    TRY(s = bquote("\"Hello\" \"world\""s));   TEST_EQUAL(s, "\"\\\"Hello\\\" \\\"world\\\"\""s);
    TRY(s = bquote("\t\n\f\r"s));              TEST_EQUAL(s, "\"\\t\\n\\f\\r\""s);
    TRY(s = bquote("åß∂ƒ"s));                  TEST_EQUAL(s, "\"\\xc3\\xa5\\xc3\\x9f\\xe2\\x88\\x82\\xc6\\x92\""s);
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

    TEST_EQUAL(unqualify(""), "");
    TEST_EQUAL(unqualify("alpha"), "alpha");
    TEST_EQUAL(unqualify("alpha.bravo"), "bravo");
    TEST_EQUAL(unqualify("alpha.bravo.charlie"), "charlie");
    TEST_EQUAL(unqualify("alpha::bravo"), "bravo");
    TEST_EQUAL(unqualify("alpha::bravo::charlie"), "charlie");
    TEST_EQUAL(unqualify("alpha-bravo"), "alpha-bravo");
    TEST_EQUAL(unqualify("alpha-bravo-charlie"), "alpha-bravo-charlie");

}

void test_unicorn_utility_type_names() {

    Ustring s;

    const std::type_info& v_info = typeid(void);
    const std::type_info& i_info = typeid(int);
    const std::type_info& s_info = typeid(std::string);
    auto v_index = std::type_index(typeid(void));
    auto i_index = std::type_index(typeid(int));
    auto s_index = std::type_index(typeid(std::string));

    TEST_EQUAL(type_name(v_info), "void");
    TEST_MATCH(type_name(i_info), "^(signed )?int$");
    TEST_MATCH(type_name(s_info), "^(class )?std::([^:]+::)*(string|basic_string ?<.+>)$");
    TEST_EQUAL(type_name(v_index), "void");
    TEST_MATCH(type_name(i_index), "^(signed )?int$");
    TEST_MATCH(type_name(s_index), "^(class )?std::([^:]+::)*(string|basic_string ?<.+>)$");
    TEST_EQUAL(type_name<void>(), "void");
    TEST_MATCH(type_name<int>(), "^(signed )?int$");
    TEST_MATCH(type_name<std::string>(), "^(class )?std::([^:]+::)*(string|basic_string ?<.+>)$");
    TEST_MATCH(type_name(42), "^(signed )?int$");
    TEST_MATCH(type_name(s), "^(class )?std::([^:]+::)*(string|basic_string ?<.+>)$");

    Derived1 d;
    Base& b(d);
    const std::type_info& d_info = typeid(d);
    const std::type_info& b_info = typeid(b);
    auto d_index = std::type_index(typeid(d));
    auto b_index = std::type_index(typeid(b));

    TEST_MATCH(type_name(d), "::Derived1$");
    TEST_MATCH(type_name(b), "::Derived1$");
    TEST_MATCH(type_name(d_info), "::Derived1$");
    TEST_MATCH(type_name(b_info), "::Derived1$");
    TEST_MATCH(type_name(d_index), "::Derived1$");
    TEST_MATCH(type_name(b_index), "::Derived1$");

}

namespace {

    struct FromView {
        int num = 0;
        FromView() = default;
        explicit FromView(std::string_view s) {
            if (s.empty() || ! ascii_isdigit(s[0]))
                throw std::runtime_error("Bad number");
            num = int(decnum(s));
        }
    };

    struct FromString {
        int num = 0;
        FromString() = default;
        explicit FromString(const std::string& s) {
            if (! ascii_isdigit(s[0]))
                throw std::runtime_error("Bad number");
            num = int(decnum(s));
        }
    };

    struct FromChars {
        int num = 0;
        FromChars() = default;
        explicit FromChars(const char* s) {
            if (! s || ! ascii_isdigit(*s))
                throw std::runtime_error("Bad number");
            num = int(decnum(s));
        }
    };

    struct FromIstream {
        int num = 0;
    };

    std::istream& operator>>(std::istream& i, FromIstream& t) {
        std::string s;
        if (i >> s) {
            if (! ascii_isdigit(s[0]))
                throw std::runtime_error("Bad number");
            t.num = int(decnum(s));
        }
        return i;
    }

    struct FromOverload {
        int num = 0;
    };

    bool from_str(std::string_view s, FromOverload& t) {
        if (s.empty() || ! ascii_isdigit(s[0]))
            return false;
        t.num = int(decnum(s));
        return true;
    }

}

void test_unicorn_utility_conversion_from_string() {

    FromView fv;
    FromString fs;
    FromChars fc;
    FromIstream fi;
    FromOverload fx;
    std::string good = "42", bad = "bad";

    TEST(from_str(good, fv));  TEST_EQUAL(fv.num, 42);
    TEST(from_str(good, fs));  TEST_EQUAL(fv.num, 42);
    TEST(from_str(good, fc));  TEST_EQUAL(fv.num, 42);
    TEST(from_str(good, fi));  TEST_EQUAL(fv.num, 42);
    TEST(from_str(good, fx));  TEST_EQUAL(fx.num, 42);

    TRY(fv = FromStr<FromView>()(good));      TEST_EQUAL(fv.num, 42);
    TRY(fs = FromStr<FromString>()(good));    TEST_EQUAL(fv.num, 42);
    TRY(fc = FromStr<FromChars>()(good));     TEST_EQUAL(fv.num, 42);
    TRY(fi = FromStr<FromIstream>()(good));   TEST_EQUAL(fv.num, 42);
    TRY(fx = FromStr<FromOverload>()(good));  TEST_EQUAL(fx.num, 42);

    TEST(! from_str(bad, fv));  TEST_EQUAL(fv.num, 42);
    TEST(! from_str(bad, fs));  TEST_EQUAL(fv.num, 42);
    TEST(! from_str(bad, fc));  TEST_EQUAL(fv.num, 42);
    TEST(! from_str(bad, fi));  TEST_EQUAL(fv.num, 42);
    TEST(! from_str(bad, fx));  TEST_EQUAL(fx.num, 42);

    TEST_THROW(from_str<FromView>(bad), std::invalid_argument);
    TEST_THROW(from_str<FromString>(bad), std::invalid_argument);
    TEST_THROW(from_str<FromChars>(bad), std::invalid_argument);
    TEST_THROW(from_str<FromIstream>(bad), std::invalid_argument);
    TEST_THROW(from_str<FromOverload>(bad), std::invalid_argument);

    TEST_EQUAL(from_str<Ustring>(""), "");
    TEST_EQUAL(from_str<Ustring>("hello"), "hello");
    TEST_EQUAL(from_str<int>(""), 0);
    TEST_EQUAL(from_str<int>("0"), 0);
    TEST_EQUAL(from_str<int>("42"), 42);
    TEST_THROW(from_str<int>("hello"), std::invalid_argument);
    TEST_EQUAL(from_str<double>(""), 0);
    TEST_EQUAL(from_str<double>("0"), 0);
    TEST_EQUAL(from_str<double>("1234.5"), 1234.5);
    TEST_THROW(from_str<double>("hello"), std::invalid_argument);
    TEST(! from_str<bool>(""));
    TEST(! from_str<bool>("0"));
    TEST(from_str<bool>("1"));
    TEST(! from_str<bool>("false"));
    TEST(from_str<bool>("true"));
    TEST_THROW(from_str<bool>("hello"), std::invalid_argument);

    TEST_EQUAL(from_str<FooEnum>("alpha"), alpha);
    TEST_EQUAL(from_str<FooEnum>("FooEnum::alpha"), alpha);
    TEST_EQUAL(from_str<BarEnum>("delta"), delta);
    TEST_EQUAL(from_str<BarEnum>("BarEnum::delta"), delta);
    TEST_EQUAL(from_str<ZapEnum>("golf"), ZapEnum::golf);
    TEST_EQUAL(from_str<ZapEnum>("ZapEnum::golf"), ZapEnum::golf);
    TEST_EQUAL(from_str<ThogEnum>("golf"), ThogEnum::golf);
    TEST_EQUAL(from_str<ThogEnum>("ThogEnum::golf"), ThogEnum::golf);
    TEST_THROW(from_str<FooEnum>("ALPHA"), std::invalid_argument);
    TEST_THROW(from_str<BarEnum>("DELTA"), std::invalid_argument);
    TEST_THROW(from_str<ZapEnum>("GOLF"), std::invalid_argument);
    TEST_THROW(from_str<ThogEnum>("GOLF"), std::invalid_argument);

}

namespace {

    struct ToString {
        int num = 0;
        explicit operator std::string() const { return "TS-" + std::to_string(num); }
    };

    struct ToView {
        int num = 0;
        explicit operator std::string_view() const { static std::string s; s = "TV-" + std::to_string(num); return s; }
    };

    struct ToChars {
        int num = 0;
        explicit operator const char*() const { static std::string s; s = "TC-" + std::to_string(num); return s.data(); }
    };

    struct ToOstream {
        int num = 0;
    };

    std::ostream& operator<<(std::ostream& o, const ToOstream& t) {
        return o << "TO-" << t.num;
    }

    struct ToOverload {
        int num = 0;
    };

    std::string to_str(const ToOverload& t) {
        return "TX-" + std::to_string(t.num);
    }

    struct HasStr {
        int num = 0;
        std::string str() const { return "HS-" + std::to_string(num); }
    };

    struct HasToString {
        int num = 0;
    };

    std::string to_string(const HasToString& t) {
        return "HT-" +  std::to_string(t.num);
    }

    struct NoConversion {};

}

void test_unicorn_utility_conversion_to_string() {

    bool b = true;
    char c = 'c';
    int i = 42;
    float f = -1.25;
    std::string s = "Hello string";
    std::string_view sv = "Hello view";
    const char* cp = "Hello chars";
    char ca[] = "Hello array";
    ToString ts {101};
    ToView tv {102};
    ToChars tc {103};
    ToOstream to {104};
    ToOverload tx {105};
    HasStr hs {106};
    HasToString ht {107};
    std::runtime_error ex("Runtime error");
    std::array<uint8_t, 4> ba = {0x12,0x34,0x56,0x78};
    std::vector<uint8_t> bv = {0x9a,0xbc,0xde,0xf0};
    std::optional<int> o1;
    std::optional<int> o2 = 42;
    std::shared_ptr<int> sp1;
    std::shared_ptr<int> sp2 = std::make_shared<int>(42);
    std::unique_ptr<int> up1;
    std::unique_ptr<int> up2 = std::make_unique<int>(42);
    std::pair<std::string, int> p = {"Answer", 42};
    std::tuple<int, std::string, bool> t = {42, "hello", true};
    std::vector<int> v = {2,3,5,7,11,13,17,19};
    std::map<int, std::string> m = {
        {1, "alpha"},
        {2, "bravo"},
        {3, "charlie"},
        {4, "delta"},
        {5, "echo"},
    };
    NoConversion nc;
    std::string out;
    ToStr convert;

    TRY(out = to_str(b));    TEST_EQUAL(out, "true");
    TRY(out = to_str(c));    TEST_EQUAL(out, "c");
    TRY(out = to_str(i));    TEST_EQUAL(out, "42");
    TRY(out = to_str(f));    TEST_EQUAL(out, "-1.25");
    TRY(out = to_str(s));    TEST_EQUAL(out, "Hello string");
    TRY(out = to_str(sv));   TEST_EQUAL(out, "Hello view");
    TRY(out = to_str(cp));   TEST_EQUAL(out, "Hello chars");
    TRY(out = to_str(ca));   TEST_EQUAL(out, "Hello array");
    TRY(out = to_str(ts));   TEST_EQUAL(out, "TS-101");
    TRY(out = to_str(tv));   TEST_EQUAL(out, "TV-102");
    TRY(out = to_str(tc));   TEST_EQUAL(out, "TC-103");
    TRY(out = to_str(to));   TEST_EQUAL(out, "TO-104");
    TRY(out = to_str(tx));   TEST_EQUAL(out, "TX-105");
    TRY(out = to_str(hs));   TEST_EQUAL(out, "HS-106");
    TRY(out = to_str(ht));   TEST_EQUAL(out, "HT-107");
    TRY(out = to_str(ex));   TEST_EQUAL(out, "Runtime error");
    TRY(out = to_str(ba));   TEST_EQUAL(out, "12 34 56 78");
    TRY(out = to_str(bv));   TEST_EQUAL(out, "9a bc de f0");
    TRY(out = to_str(o1));   TEST_EQUAL(out, "null");
    TRY(out = to_str(o2));   TEST_EQUAL(out, "42");
    TRY(out = to_str(sp1));  TEST_EQUAL(out, "null");
    TRY(out = to_str(sp2));  TEST_EQUAL(out, "42");
    TRY(out = to_str(up1));  TEST_EQUAL(out, "null");
    TRY(out = to_str(up2));  TEST_EQUAL(out, "42");
    TRY(out = to_str(p));    TEST_EQUAL(out, "(Answer,42)");
    TRY(out = to_str(t));    TEST_EQUAL(out, "(42,hello,true)");
    TRY(out = to_str(v));    TEST_EQUAL(out, "[2,3,5,7,11,13,17,19]");
    TRY(out = to_str(m));    TEST_EQUAL(out, "{1:alpha,2:bravo,3:charlie,4:delta,5:echo}");
    TRY(out = to_str(nc));   TEST_MATCH(out, "::NoConversion$");

    TRY(out = convert(b));    TEST_EQUAL(out, "true");
    TRY(out = convert(c));    TEST_EQUAL(out, "c");
    TRY(out = convert(i));    TEST_EQUAL(out, "42");
    TRY(out = convert(f));    TEST_EQUAL(out, "-1.25");
    TRY(out = convert(s));    TEST_EQUAL(out, "Hello string");
    TRY(out = convert(sv));   TEST_EQUAL(out, "Hello view");
    TRY(out = convert(cp));   TEST_EQUAL(out, "Hello chars");
    TRY(out = convert(ca));   TEST_EQUAL(out, "Hello array");
    TRY(out = convert(ts));   TEST_EQUAL(out, "TS-101");
    TRY(out = convert(tv));   TEST_EQUAL(out, "TV-102");
    TRY(out = convert(tc));   TEST_EQUAL(out, "TC-103");
    TRY(out = convert(to));   TEST_EQUAL(out, "TO-104");
    TRY(out = convert(tx));   TEST_EQUAL(out, "TX-105");
    TRY(out = convert(hs));   TEST_EQUAL(out, "HS-106");
    TRY(out = convert(ht));   TEST_EQUAL(out, "HT-107");
    TRY(out = convert(ex));   TEST_EQUAL(out, "Runtime error");
    TRY(out = convert(ba));   TEST_EQUAL(out, "12 34 56 78");
    TRY(out = convert(bv));   TEST_EQUAL(out, "9a bc de f0");
    TRY(out = convert(o1));   TEST_EQUAL(out, "null");
    TRY(out = convert(o2));   TEST_EQUAL(out, "42");
    TRY(out = convert(sp1));  TEST_EQUAL(out, "null");
    TRY(out = convert(sp2));  TEST_EQUAL(out, "42");
    TRY(out = convert(up1));  TEST_EQUAL(out, "null");
    TRY(out = convert(up2));  TEST_EQUAL(out, "42");
    TRY(out = convert(p));    TEST_EQUAL(out, "(Answer,42)");
    TRY(out = convert(t));    TEST_EQUAL(out, "(42,hello,true)");
    TRY(out = convert(v));    TEST_EQUAL(out, "[2,3,5,7,11,13,17,19]");
    TRY(out = convert(m));    TEST_EQUAL(out, "{1:alpha,2:bravo,3:charlie,4:delta,5:echo}");
    TRY(out = convert(nc));   TEST_MATCH(out, "::NoConversion$");

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
