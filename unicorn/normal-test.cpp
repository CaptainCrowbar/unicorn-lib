#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/normal.hpp"
#include "unicorn/string.hpp"
#include "unicorn/ucd-tables.hpp"
#include "crow/thread.hpp"
#include "prion/unit-test.hpp"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>

PRI_LDLIB(crow)

using namespace std::literals;
using namespace Unicorn;

#define NORM_TEST(type, mode, index, expect) \
    do { \
        type mode ## index; \
        TRY(mode ## index = normalize(col[index], mode)); \
        TEST_EQUAL(mode ## index, col[expect]); \
        if (mode ## index != col[expect]) \
            FAIL("Normalization test " + dec(line) + ": " + str_join(row, ";")); \
    } while(false)

#define NORM_TEST_TYPE(type) \
    do { \
        std::vector<type> col(5); \
        for (size_t i = 0; i < 5; ++i) \
            recode(unicode[i], col[i]); \
        NORM_TEST(type, NFC, 0, 1); \
        NORM_TEST(type, NFC, 1, 1); \
        NORM_TEST(type, NFC, 2, 1); \
        NORM_TEST(type, NFC, 3, 3); \
        NORM_TEST(type, NFC, 4, 3); \
        NORM_TEST(type, NFD, 0, 2); \
        NORM_TEST(type, NFD, 1, 2); \
        NORM_TEST(type, NFD, 2, 2); \
        NORM_TEST(type, NFD, 3, 4); \
        NORM_TEST(type, NFD, 4, 4); \
        NORM_TEST(type, NFKC, 0, 3); \
        NORM_TEST(type, NFKC, 1, 3); \
        NORM_TEST(type, NFKC, 2, 3); \
        NORM_TEST(type, NFKC, 3, 3); \
        NORM_TEST(type, NFKC, 4, 3); \
        NORM_TEST(type, NFKD, 0, 4); \
        NORM_TEST(type, NFKD, 1, 4); \
        NORM_TEST(type, NFKD, 2, 4); \
        NORM_TEST(type, NFKD, 3, 4); \
        NORM_TEST(type, NFKD, 4, 4); \
    } while (false)

#define NORM_IDENTITY_TEST(type) \
    do { \
        type s; \
        recode(uni, s); \
        TEST_EQUAL(normalize(s, NFC), s); \
        TEST_EQUAL(normalize(s, NFD), s); \
        TEST_EQUAL(normalize(s, NFKC), s); \
        TEST_EQUAL(normalize(s, NFKD), s); \
    } while (false)

namespace {

    using UnicornDetail::normalization_test_table;
    using UnicornDetail::normalization_identity_table;

    void do_main_tests(size_t b, size_t e) {
        e = std::min(e, range_count(normalization_test_table));
        b = std::min(b, e);
        std::vector<u8string> hexcodes;
        std::vector<u32string> unicode;
        auto row_iter = std::begin(normalization_test_table) + b;
        for (size_t line = b; line < e; ++line) {
            auto& row = *row_iter++;
            unicode.clear();
            for (auto&& field: row) {
                unicode.push_back(u32string());
                str_split(u8string(field), overwrite(hexcodes));
                for (auto&& hc: hexcodes)
                    unicode.back() += char32_t(strtoul(hc.data(), nullptr, 16));
            }
            TEST_EQUAL(unicode.size(), 5);
            if (unicode.size() == 5) {
                NORM_TEST_TYPE(u8string);
                NORM_TEST_TYPE(u16string);
                NORM_TEST_TYPE(u32string);
                NORM_TEST_TYPE(wstring);
            }
        }
    }

    void do_identity_tests(const std::vector<char32_t>& uchars, size_t b, size_t e) {
        e = std::min(e, uchars.size());
        b = std::min(b, e);
        for (size_t i = b; i < e; ++i) {
            u32string uni{uchars[i]};
            NORM_IDENTITY_TEST(u8string);
            NORM_IDENTITY_TEST(u16string);
            NORM_IDENTITY_TEST(u32string);
            NORM_IDENTITY_TEST(wstring);
        }
    }

}

TEST_MODULE(unicorn, normal) {

    auto norm_tests = range_count(normalization_test_table);
    std::vector<char32_t> identity_chars;
    for (auto&& row: normalization_identity_table)
        for (char32_t u = row.key; u <= row.value; ++u)
            identity_chars.push_back(u);

    #if defined(PRI_TARGET_CYGWIN)

        // Multithreaded version seems to have problems on Cygwin
        do_main_tests(0, norm_tests);
        do_identity_tests(identity_chars, 0, 10000);

    #else

        auto thread_count = Crow::cpu_threads();
        auto per_thread = norm_tests / thread_count + 1;
        std::vector<Crow::Thread> threads;
        for (size_t base = 0; base < norm_tests; base += per_thread)
            threads.push_back(Crow::Thread([=] { do_main_tests(base, base + per_thread); }));
        for (auto& t: threads)
            t.wait();
        threads.clear();
        per_thread = identity_chars.size() / thread_count + 1;
        for (size_t base = 0; base < identity_chars.size(); base += per_thread)
            threads.push_back(Crow::Thread([=] { do_identity_tests(identity_chars, base, base + per_thread); }));
        for (auto& t: threads)
            t.wait();

    #endif

}
