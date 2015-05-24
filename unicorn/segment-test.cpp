#include "crow/core.hpp"
#include "crow/unit-test.hpp"
#include "unicorn/segment.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/ucd-tables.hpp"
#include "unicorn/utf.hpp"
#include <cstdlib>
#include <string>
#include <vector>

using namespace std::literals;
using namespace Unicorn;

namespace {

    u32string decode_hex(const u8string& code) {
        std::vector<u8string> hexcodes;
        str_split_by(code, append(hexcodes), " /");
        u32string str;
        std::transform(CROW_BOUNDS(hexcodes), append(str), hexnum);
        return str;
    }

    struct SplitGraphemes {
        template <typename String>
        void operator()(const String& src, std::vector<String>& dst) const {
            for (auto& segment: grapheme_range(src))
                dst.push_back(u_str(segment));
        }
    };

    struct SplitWords {
        template <typename String>
        void operator()(const String& src, std::vector<String>& dst) const {
            for (auto& segment: word_range(src))
                dst.push_back(u_str(segment));
        }
    };

    struct SplitSentences {
        template <typename String>
        void operator()(const String& src, std::vector<String>& dst) const {
            for (auto& segment: sentence_range(src))
                dst.push_back(u_str(segment));
        }
    };

    template <typename Split>
    void segmentation_test(const u8string& name, Irange<char const* const*> table) {
        size_t lnum = 0;
        for (u8string line: table) {
            ++lnum;
            size_t prev_failures = Test::test_failures();
            auto source32 = decode_hex(line);
            // Some of the examples contain mismatched surrogates
            if (! valid_string(source32))
                continue;
            auto source8 = to_utf8(source32);
            auto source16 = to_utf16(source32);
            auto wsource = to_wstring(source32);
            std::vector<u8string> breakdown;
            str_split_by(line, append(breakdown), "/");
            std::vector<u32string> expect32;
            std::transform(CROW_BOUNDS(breakdown), append(expect32), decode_hex);
            std::vector<u8string> expect8(expect32.size());
            std::vector<u16string> expect16(expect32.size());
            std::vector<wstring> wexpect(expect32.size());
            for (size_t i = 0; i < expect32.size(); ++i) {
                expect8[i] = recode<char>(expect32[i]);
                expect16[i] = recode<char16_t>(expect32[i]);
                wexpect[i] = recode<wchar_t>(expect32[i]);
            }
            std::vector<u8string> segments8;
            std::vector<u16string> segments16;
            std::vector<u32string> segments32;
            std::vector<wstring> wsegments;
            TRY(Split()(source8, segments8));
            TRY(Split()(source16, segments16));
            TRY(Split()(source32, segments32));
            TRY(Split()(wsource, wsegments));
            TEST_EQUAL_RANGE(segments8, expect8);
            TEST_EQUAL_RANGE(segments16, expect16);
            TEST_EQUAL_RANGE(segments32, expect32);
            TEST_EQUAL_RANGE(wsegments, wexpect);
            if (Test::test_failures() > prev_failures) {
                FAIL(name + " " + dec(lnum) + ": " + line);
                for (auto c: source32)
                    std::cout << "\t" << hex(c) << " " << property_value(word_break(c)) << "\n";
                break;
            }
        }
    }

    #define WORD_SEGMENTATION_TEST(source, all, selected) \
        do { \
            auto s = cstr(source); \
            decltype(word_range(s)) segments; \
            TRY(segments = word_range(s)); \
            decltype(s) result; \
            for (auto& subrange: segments) { \
                str_append_char(result, '['); \
                TRY(result += u_str(subrange)); \
                str_append_char(result, ']'); \
            } \
            TEST_EQUAL(result, all); \
            TRY(segments = word_range(s, alpha_words)); \
            result.clear(); \
            for (auto& subrange: segments) { \
                str_append_char(result, '['); \
                TRY(result += u_str(subrange)); \
                str_append_char(result, ']'); \
            } \
            TEST_EQUAL(result, selected); \
        } while (false)

    #define BLOCK_SEGMENTATION_TEST(function, mode, source, unstripped, stripped) \
        do { \
            auto s = cstr(source); \
            decltype(function(s, mode)) segments; \
            TRY(segments = function(s, mode)); \
            decltype(s) result; \
            for (auto& subrange: segments) { \
                str_append_char(result, '['); \
                TRY(result += u_str(subrange)); \
                str_append_char(result, ']'); \
            } \
            TEST_EQUAL(result, unstripped); \
            TRY(segments = function(s, mode | strip_breaks)); \
            result.clear(); \
            for (auto& subrange: segments) { \
                str_append_char(result, '['); \
                TRY(result += u_str(subrange)); \
                str_append_char(result, ']'); \
            } \
            TEST_EQUAL(result, stripped); \
        } while (false)

    void check_word_segmentation_utf8() {

        WORD_SEGMENTATION_TEST("", "", "");
        WORD_SEGMENTATION_TEST("Hello world", "[Hello][ ][world]", "[Hello][world]");
        WORD_SEGMENTATION_TEST("Hello-world", "[Hello][-][world]", "[Hello][world]");

        WORD_SEGMENTATION_TEST(

            "\"Don't panic!\" - Douglas Adams",

            "[\"]"
            "[Don't]"
            "[ ]"
            "[panic]"
            "[!]"
            "[\"]"
            "[ ]"
            "[-]"
            "[ ]"
            "[Douglas]"
            "[ ]"
            "[Adams]",

            "[Don't]"
            "[panic]"
            "[Douglas]"
            "[Adams]"

        );

    }

    void check_word_segmentation_utf16() {

        WORD_SEGMENTATION_TEST(u"", u"", u"");
        WORD_SEGMENTATION_TEST(u"Hello world", u"[Hello][ ][world]", u"[Hello][world]");
        WORD_SEGMENTATION_TEST(u"Hello-world", u"[Hello][-][world]", u"[Hello][world]");

        WORD_SEGMENTATION_TEST(

            u"\"Don't panic!\" - Douglas Adams",

            u"[\"]"
            u"[Don't]"
            u"[ ]"
            u"[panic]"
            u"[!]"
            u"[\"]"
            u"[ ]"
            u"[-]"
            u"[ ]"
            u"[Douglas]"
            u"[ ]"
            u"[Adams]",

            u"[Don't]"
            u"[panic]"
            u"[Douglas]"
            u"[Adams]"

        );

    }

    void check_word_segmentation_utf32() {

        WORD_SEGMENTATION_TEST(U"", U"", U"");
        WORD_SEGMENTATION_TEST(U"Hello world", U"[Hello][ ][world]", U"[Hello][world]");
        WORD_SEGMENTATION_TEST(U"Hello-world", U"[Hello][-][world]", U"[Hello][world]");

        WORD_SEGMENTATION_TEST(

            U"\"Don't panic!\" - Douglas Adams",

            U"[\"]"
            U"[Don't]"
            U"[ ]"
            U"[panic]"
            U"[!]"
            U"[\"]"
            U"[ ]"
            U"[-]"
            U"[ ]"
            U"[Douglas]"
            U"[ ]"
            U"[Adams]",

            U"[Don't]"
            U"[panic]"
            U"[Douglas]"
            U"[Adams]"

        );

    }

    void check_line_segmentation_utf8() {

        // Line breaking characters:
        // U+000A line feed
        // U+000B vertical tab
        // U+000C form feed
        // U+000D carriage return
        // U+0085 next line
        // U+2028 line separator
        // U+2029 paragraph separator

        BLOCK_SEGMENTATION_TEST(line_range, 0, "", "", "");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "\n", "[\n]", "[]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "\r", "[\r]", "[]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "\r\n", "[\r\n]", "[]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "\n\r", "[\n][\r]", "[][]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello world", "[Hello world]", "[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello world\n", "[Hello world\n]", "[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello world\r", "[Hello world\r]", "[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello world\r\n", "[Hello world\r\n]", "[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello world\n\r", "[Hello world\n][\r]", "[Hello world][]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello\nworld\n", "[Hello\n][world\n]", "[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello\vworld\v", "[Hello\v][world\v]", "[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello\fworld\f", "[Hello\f][world\f]", "[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello\rworld\r", "[Hello\r][world\r]", "[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello\u0085world\u0085", "[Hello\u0085][world\u0085]", "[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello\u2028world\u2028", "[Hello\u2028][world\u2028]", "[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, "Hello\u2029world\u2029", "[Hello\u2029][world\u2029]", "[Hello][world]");

        BLOCK_SEGMENTATION_TEST(line_range, 0,

            "Last night I saw upon the stair\r\n"
            "A little man who wasn't there\r\n"
            "He wasn't there again today\r\n"
            "He must be from the NSA\r\n",

            "[Last night I saw upon the stair\r\n]"
            "[A little man who wasn't there\r\n]"
            "[He wasn't there again today\r\n]"
            "[He must be from the NSA\r\n]",

            "[Last night I saw upon the stair]"
            "[A little man who wasn't there]"
            "[He wasn't there again today]"
            "[He must be from the NSA]"

        );

    }

    void check_line_segmentation_utf16() {

        BLOCK_SEGMENTATION_TEST(line_range, 0, u"", u"", u"");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"\n", u"[\n]", u"[]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"\r", u"[\r]", u"[]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"\r\n", u"[\r\n]", u"[]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"\n\r", u"[\n][\r]", u"[][]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello world", u"[Hello world]", u"[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello world\n", u"[Hello world\n]", u"[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello world\r", u"[Hello world\r]", u"[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello world\r\n", u"[Hello world\r\n]", u"[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello world\n\r", u"[Hello world\n][\r]", u"[Hello world][]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello\nworld\n", u"[Hello\n][world\n]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello\vworld\v", u"[Hello\v][world\v]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello\fworld\f", u"[Hello\f][world\f]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello\rworld\r", u"[Hello\r][world\r]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello\u0085world\u0085", u"[Hello\u0085][world\u0085]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello\u2028world\u2028", u"[Hello\u2028][world\u2028]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, u"Hello\u2029world\u2029", u"[Hello\u2029][world\u2029]", u"[Hello][world]");

        BLOCK_SEGMENTATION_TEST(line_range, 0,

            u"Last night I saw upon the stair\r\n"
            u"A little man who wasn't there\r\n"
            u"He wasn't there again today\r\n"
            u"He must be from the NSA\r\n",

            u"[Last night I saw upon the stair\r\n]"
            u"[A little man who wasn't there\r\n]"
            u"[He wasn't there again today\r\n]"
            u"[He must be from the NSA\r\n]",

            u"[Last night I saw upon the stair]"
            u"[A little man who wasn't there]"
            u"[He wasn't there again today]"
            u"[He must be from the NSA]"

        );

    }

    void check_line_segmentation_utf32() {

        BLOCK_SEGMENTATION_TEST(line_range, 0, U"", U"", U"");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"\n", U"[\n]", U"[]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"\r", U"[\r]", U"[]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"\r\n", U"[\r\n]", U"[]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"\n\r", U"[\n][\r]", U"[][]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello world", U"[Hello world]", U"[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello world\n", U"[Hello world\n]", U"[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello world\r", U"[Hello world\r]", U"[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello world\r\n", U"[Hello world\r\n]", U"[Hello world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello world\n\r", U"[Hello world\n][\r]", U"[Hello world][]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello\nworld\n", U"[Hello\n][world\n]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello\vworld\v", U"[Hello\v][world\v]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello\fworld\f", U"[Hello\f][world\f]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello\rworld\r", U"[Hello\r][world\r]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello\u0085world\u0085", U"[Hello\u0085][world\u0085]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello\u2028world\u2028", U"[Hello\u2028][world\u2028]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(line_range, 0, U"Hello\u2029world\u2029", U"[Hello\u2029][world\u2029]", U"[Hello][world]");

        BLOCK_SEGMENTATION_TEST(line_range, 0,

            U"Last night I saw upon the stair\r\n"
            U"A little man who wasn't there\r\n"
            U"He wasn't there again today\r\n"
            U"He must be from the NSA\r\n",

            U"[Last night I saw upon the stair\r\n]"
            U"[A little man who wasn't there\r\n]"
            U"[He wasn't there again today\r\n]"
            U"[He must be from the NSA\r\n]",

            U"[Last night I saw upon the stair]"
            U"[A little man who wasn't there]"
            U"[He wasn't there again today]"
            U"[He must be from the NSA]"

        );

    }

    void check_paragraph_segmentation_utf8() {

        BLOCK_SEGMENTATION_TEST(paragraph_range, 0, "", "", "");

        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u8"Hello\n\nworld\n\n", u8"[Hello\n\nworld\n\n]", u8"[Hello\n\nworld\n\n]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u8"Hello\r\rworld\r\r", u8"[Hello\r\rworld\r\r]", u8"[Hello\r\rworld\r\r]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u8"Hello\r\nworld\r\n", u8"[Hello\r\nworld\r\n]", u8"[Hello\r\nworld\r\n]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u8"Hello\n\rworld\n\r", u8"[Hello\n\rworld\n\r]", u8"[Hello\n\rworld\n\r]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u8"Hello\v\vworld\v\v", u8"[Hello\v\vworld\v\v]", u8"[Hello\v\vworld\v\v]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u8"Hello\f\fworld\f\f", u8"[Hello\f\fworld\f\f]", u8"[Hello\f\fworld\f\f]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u8"Hello\u0085\u0085world\u0085\u0085", u8"[Hello\u0085\u0085world\u0085\u0085]", u8"[Hello\u0085\u0085world\u0085\u0085]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u8"Hello\u2028\u2028world\u2028\u2028", u8"[Hello\u2028\u2028world\u2028\u2028]", u8"[Hello\u2028\u2028world\u2028\u2028]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u8"Hello\u2029\u2029world\u2029\u2029", u8"[Hello\u2029][\u2029][world\u2029][\u2029]", u8"[Hello][][world][]");

        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u8"Hello\n\nworld\n\n", u8"[Hello\n][\n][world\n][\n]", u8"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u8"Hello\r\rworld\r\r", u8"[Hello\r][\r][world\r][\r]", u8"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u8"Hello\r\nworld\r\n", u8"[Hello\r\n][world\r\n]", u8"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u8"Hello\n\rworld\n\r", u8"[Hello\n][\r][world\n][\r]", u8"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u8"Hello\v\vworld\v\v", u8"[Hello\v][\v][world\v][\v]", u8"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u8"Hello\f\fworld\f\f", u8"[Hello\f\fworld\f\f]", u8"[Hello\f\fworld\f\f]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u8"Hello\u0085\u0085world\u0085\u0085", u8"[Hello\u0085][\u0085][world\u0085][\u0085]", u8"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u8"Hello\u2028\u2028world\u2028\u2028", u8"[Hello\u2028\u2028world\u2028\u2028]", u8"[Hello\u2028\u2028world\u2028\u2028]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u8"Hello\u2029\u2029world\u2029\u2029", u8"[Hello\u2029][\u2029][world\u2029][\u2029]", u8"[Hello][][world][]");

        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u8"Hello\n\nworld\n\n", u8"[Hello\n\n][world\n\n]", u8"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u8"Hello\r\rworld\r\r", u8"[Hello\r\r][world\r\r]", u8"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u8"Hello\r\nworld\r\n", u8"[Hello\r\nworld\r\n]", u8"[Hello\r\nworld\r\n]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u8"Hello\n\rworld\n\r", u8"[Hello\n\r][world\n\r]", u8"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u8"Hello\v\vworld\v\v", u8"[Hello\v\v][world\v\v]", u8"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u8"Hello\f\fworld\f\f", u8"[Hello\f\fworld\f\f]", u8"[Hello\f\fworld\f\f]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u8"Hello\u0085\u0085world\u0085\u0085", u8"[Hello\u0085\u0085][world\u0085\u0085]", u8"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u8"Hello\u2028\u2028world\u2028\u2028", u8"[Hello\u2028\u2028world\u2028\u2028]", u8"[Hello\u2028\u2028world\u2028\u2028]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u8"Hello\u2029\u2029world\u2029\u2029", u8"[Hello\u2029][\u2029][world\u2029][\u2029]", u8"[Hello][][world][]");

    }

    void check_paragraph_segmentation_utf16() {

        BLOCK_SEGMENTATION_TEST(paragraph_range, 0, u"", u"", u"");

        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u"Hello\n\nworld\n\n", u"[Hello\n\nworld\n\n]", u"[Hello\n\nworld\n\n]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u"Hello\r\rworld\r\r", u"[Hello\r\rworld\r\r]", u"[Hello\r\rworld\r\r]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u"Hello\r\nworld\r\n", u"[Hello\r\nworld\r\n]", u"[Hello\r\nworld\r\n]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u"Hello\n\rworld\n\r", u"[Hello\n\rworld\n\r]", u"[Hello\n\rworld\n\r]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u"Hello\v\vworld\v\v", u"[Hello\v\vworld\v\v]", u"[Hello\v\vworld\v\v]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u"Hello\f\fworld\f\f", u"[Hello\f\fworld\f\f]", u"[Hello\f\fworld\f\f]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u"Hello\u0085\u0085world\u0085\u0085", u"[Hello\u0085\u0085world\u0085\u0085]", u"[Hello\u0085\u0085world\u0085\u0085]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u"Hello\u2028\u2028world\u2028\u2028", u"[Hello\u2028\u2028world\u2028\u2028]", u"[Hello\u2028\u2028world\u2028\u2028]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, u"Hello\u2029\u2029world\u2029\u2029", u"[Hello\u2029][\u2029][world\u2029][\u2029]", u"[Hello][][world][]");

        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u"Hello\n\nworld\n\n", u"[Hello\n][\n][world\n][\n]", u"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u"Hello\r\rworld\r\r", u"[Hello\r][\r][world\r][\r]", u"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u"Hello\r\nworld\r\n", u"[Hello\r\n][world\r\n]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u"Hello\n\rworld\n\r", u"[Hello\n][\r][world\n][\r]", u"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u"Hello\v\vworld\v\v", u"[Hello\v][\v][world\v][\v]", u"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u"Hello\f\fworld\f\f", u"[Hello\f\fworld\f\f]", u"[Hello\f\fworld\f\f]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u"Hello\u0085\u0085world\u0085\u0085", u"[Hello\u0085][\u0085][world\u0085][\u0085]", u"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u"Hello\u2028\u2028world\u2028\u2028", u"[Hello\u2028\u2028world\u2028\u2028]", u"[Hello\u2028\u2028world\u2028\u2028]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, u"Hello\u2029\u2029world\u2029\u2029", u"[Hello\u2029][\u2029][world\u2029][\u2029]", u"[Hello][][world][]");

        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u"Hello\n\nworld\n\n", u"[Hello\n\n][world\n\n]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u"Hello\r\rworld\r\r", u"[Hello\r\r][world\r\r]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u"Hello\r\nworld\r\n", u"[Hello\r\nworld\r\n]", u"[Hello\r\nworld\r\n]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u"Hello\n\rworld\n\r", u"[Hello\n\r][world\n\r]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u"Hello\v\vworld\v\v", u"[Hello\v\v][world\v\v]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u"Hello\f\fworld\f\f", u"[Hello\f\fworld\f\f]", u"[Hello\f\fworld\f\f]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u"Hello\u0085\u0085world\u0085\u0085", u"[Hello\u0085\u0085][world\u0085\u0085]", u"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u"Hello\u2028\u2028world\u2028\u2028", u"[Hello\u2028\u2028world\u2028\u2028]", u"[Hello\u2028\u2028world\u2028\u2028]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, u"Hello\u2029\u2029world\u2029\u2029", u"[Hello\u2029][\u2029][world\u2029][\u2029]", u"[Hello][][world][]");

    }

    void check_paragraph_segmentation_utf32() {

        BLOCK_SEGMENTATION_TEST(paragraph_range, 0, U"", U"", U"");

        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, U"Hello\n\nworld\n\n", U"[Hello\n\nworld\n\n]", U"[Hello\n\nworld\n\n]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, U"Hello\r\rworld\r\r", U"[Hello\r\rworld\r\r]", U"[Hello\r\rworld\r\r]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, U"Hello\r\nworld\r\n", U"[Hello\r\nworld\r\n]", U"[Hello\r\nworld\r\n]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, U"Hello\n\rworld\n\r", U"[Hello\n\rworld\n\r]", U"[Hello\n\rworld\n\r]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, U"Hello\v\vworld\v\v", U"[Hello\v\vworld\v\v]", U"[Hello\v\vworld\v\v]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, U"Hello\f\fworld\f\f", U"[Hello\f\fworld\f\f]", U"[Hello\f\fworld\f\f]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, U"Hello\u0085\u0085world\u0085\u0085", U"[Hello\u0085\u0085world\u0085\u0085]", U"[Hello\u0085\u0085world\u0085\u0085]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, U"Hello\u2028\u2028world\u2028\u2028", U"[Hello\u2028\u2028world\u2028\u2028]", U"[Hello\u2028\u2028world\u2028\u2028]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, unicode_paras, U"Hello\u2029\u2029world\u2029\u2029", U"[Hello\u2029][\u2029][world\u2029][\u2029]", U"[Hello][][world][]");

        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, U"Hello\n\nworld\n\n", U"[Hello\n][\n][world\n][\n]", U"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, U"Hello\r\rworld\r\r", U"[Hello\r][\r][world\r][\r]", U"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, U"Hello\r\nworld\r\n", U"[Hello\r\n][world\r\n]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, U"Hello\n\rworld\n\r", U"[Hello\n][\r][world\n][\r]", U"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, U"Hello\v\vworld\v\v", U"[Hello\v][\v][world\v][\v]", U"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, U"Hello\f\fworld\f\f", U"[Hello\f\fworld\f\f]", U"[Hello\f\fworld\f\f]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, U"Hello\u0085\u0085world\u0085\u0085", U"[Hello\u0085][\u0085][world\u0085][\u0085]", U"[Hello][][world][]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, U"Hello\u2028\u2028world\u2028\u2028", U"[Hello\u2028\u2028world\u2028\u2028]", U"[Hello\u2028\u2028world\u2028\u2028]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, line_paras, U"Hello\u2029\u2029world\u2029\u2029", U"[Hello\u2029][\u2029][world\u2029][\u2029]", U"[Hello][][world][]");

        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, U"Hello\n\nworld\n\n", U"[Hello\n\n][world\n\n]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, U"Hello\r\rworld\r\r", U"[Hello\r\r][world\r\r]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, U"Hello\r\nworld\r\n", U"[Hello\r\nworld\r\n]", U"[Hello\r\nworld\r\n]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, U"Hello\n\rworld\n\r", U"[Hello\n\r][world\n\r]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, U"Hello\v\vworld\v\v", U"[Hello\v\v][world\v\v]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, U"Hello\f\fworld\f\f", U"[Hello\f\fworld\f\f]", U"[Hello\f\fworld\f\f]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, U"Hello\u0085\u0085world\u0085\u0085", U"[Hello\u0085\u0085][world\u0085\u0085]", U"[Hello][world]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, U"Hello\u2028\u2028world\u2028\u2028", U"[Hello\u2028\u2028world\u2028\u2028]", U"[Hello\u2028\u2028world\u2028\u2028]");
        BLOCK_SEGMENTATION_TEST(paragraph_range, multiline_paras, U"Hello\u2029\u2029world\u2029\u2029", U"[Hello\u2029][\u2029][world\u2029][\u2029]", U"[Hello][][world][]");

    }

}

TEST_MODULE(unicorn, segment) {

    segmentation_test<SplitGraphemes>("Grapheme break test", UnicornDetail::grapheme_break_test_table);
    segmentation_test<SplitWords>("Word break test", UnicornDetail::word_break_test_table);
    segmentation_test<SplitSentences>("Sentence break test", UnicornDetail::sentence_break_test_table);

    check_word_segmentation_utf8();
    check_word_segmentation_utf16();
    check_word_segmentation_utf32();
    check_line_segmentation_utf8();
    check_line_segmentation_utf16();
    check_line_segmentation_utf32();
    check_paragraph_segmentation_utf8();
    check_paragraph_segmentation_utf16();
    check_paragraph_segmentation_utf32();

}
