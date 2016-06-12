#include "unicorn/core.hpp"
#include "unicorn/segment.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/ucd-tables.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <cstdlib>
#include <string>
#include <vector>

using namespace std::literals;
using namespace Unicorn;

namespace {

    u32string decode_hex(const u8string& code) {
        vector<u8string> hexcodes;
        str_split_by(code, append(hexcodes), " /");
        u32string str;
        std::transform(hexcodes.begin(), hexcodes.end(), append(str), hexnum);
        return str;
    }

    struct SplitGraphemes {
        template <typename String>
        void operator()(const String& src, vector<String>& dst) const {
            for (auto& segment: grapheme_range(src))
                dst.push_back(u_str(segment));
        }
    };

    struct SplitWords {
        template <typename String>
        void operator()(const String& src, vector<String>& dst) const {
            for (auto& segment: word_range(src))
                dst.push_back(u_str(segment));
        }
    };

    struct SplitSentences {
        template <typename String>
        void operator()(const String& src, vector<String>& dst) const {
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
            vector<u8string> breakdown;
            str_split_by(line, append(breakdown), "/");
            vector<u32string> expect32;
            std::transform(breakdown.begin(), breakdown.end(), append(expect32), decode_hex);
            vector<u8string> expect8(expect32.size());
            vector<u16string> expect16(expect32.size());
            vector<wstring> wexpect(expect32.size());
            for (size_t i = 0; i < expect32.size(); ++i) {
                expect8[i] = recode<char>(expect32[i]);
                expect16[i] = recode<char16_t>(expect32[i]);
                wexpect[i] = recode<wchar_t>(expect32[i]);
            }
            vector<u8string> segments8;
            vector<u16string> segments16;
            vector<u32string> segments32;
            vector<wstring> wsegments;
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
                    cout << "\t" << hex(c) << " " << word_break(c) << "\n";
                break;
            }
        }
    }

    #define WORD_SEGMENTATION_TEST(source, all, graphic, alpha) \
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
            TRY(segments = word_range(s, graphic_words)); \
            result.clear(); \
            for (auto& subrange: segments) { \
                str_append_char(result, '['); \
                TRY(result += u_str(subrange)); \
                str_append_char(result, ']'); \
            } \
            TEST_EQUAL(result, graphic); \
            TRY(segments = word_range(s, alpha_words)); \
            result.clear(); \
            for (auto& subrange: segments) { \
                str_append_char(result, '['); \
                TRY(result += u_str(subrange)); \
                str_append_char(result, ']'); \
            } \
            TEST_EQUAL(result, alpha); \
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

    void check_word_segmentation() {

        WORD_SEGMENTATION_TEST("", "", "", "");
        WORD_SEGMENTATION_TEST("Hello world", "[Hello][ ][world]", "[Hello][world]", "[Hello][world]");
        WORD_SEGMENTATION_TEST("Hello-world", "[Hello][-][world]", "[Hello][-][world]", "[Hello][world]");

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

            "[\"]"
            "[Don't]"
            "[panic]"
            "[!]"
            "[\"]"
            "[-]"
            "[Douglas]"
            "[Adams]",

            "[Don't]"
            "[panic]"
            "[Douglas]"
            "[Adams]"

        );

    }

    void check_line_segmentation() {

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

    void check_paragraph_segmentation() {

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

}

TEST_MODULE(unicorn, segment) {

    segmentation_test<SplitGraphemes>("Grapheme break test", UnicornDetail::grapheme_break_test_table);
    segmentation_test<SplitWords>("Word break test", UnicornDetail::word_break_test_table);
    segmentation_test<SplitSentences>("Sentence break test", UnicornDetail::sentence_break_test_table);

    check_word_segmentation();
    check_line_segmentation();
    check_paragraph_segmentation();

}
