#include "unicorn/string.hpp"
#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "rs-core/unit-test.hpp"
#include <string>

using namespace RS::Unicorn;
using namespace std::literals;

namespace {

    // UTF-32    UTF-16     UTF-8
    // 0000004d  004d       4d
    // 00000430  0430       d0 b0
    // 00004e8c  4e8c       e4 ba 8c
    // 00010302  d800 df02  f0 90 8c 82
    // 0010fffd  dbff dffd  f4 8f bf bd

    const U8string utf8_example {"\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};
    const std::u16string utf16_example {0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
    const std::u32string utf32_example {0x430,0x4e8c,0x10302,0x10fffd};

    #if defined(UNICORN_WCHAR_UTF16)
        const std::wstring wide_example(utf16_example.begin(), utf16_example.end());
    #else
        const std::wstring wide_example(utf32_example.begin(), utf32_example.end());
    #endif

    void check_length() {

        U8string s8;
        std::u16string s16;
        std::u32string s32;

        TEST_EQUAL(str_length(""s, character_units), 0);
        TEST_EQUAL(str_length("Hello"s, character_units), 5);
        TEST_EQUAL(str_length(utf8_example, character_units), 4);

        TEST_EQUAL(str_length(u""s, character_units), 0);
        TEST_EQUAL(str_length(u"Hello"s, character_units), 5);
        TEST_EQUAL(str_length(utf16_example, character_units), 4);

        TEST_EQUAL(str_length(U""s, character_units), 0);
        TEST_EQUAL(str_length(U"Hello"s, character_units), 5);
        TEST_EQUAL(str_length(utf32_example, character_units), 4);

        TEST_EQUAL(str_length(u8""s, grapheme_units), 0);
        TEST_EQUAL(str_length(u8"aeiou"s, grapheme_units), 5);
        TEST_EQUAL(str_length(u8"áéíóú"s, grapheme_units), 5);                                // precomposed
        TEST_EQUAL(str_length(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, grapheme_units), 5);  // decomposed
        TEST_EQUAL(str_length(u8"\ufe00Hello \ufe01world"s, grapheme_units), 11);             // nonspacing mark

        TEST_EQUAL(str_length(u""s, grapheme_units), 0);
        TEST_EQUAL(str_length(u"aeiou"s, grapheme_units), 5);
        TEST_EQUAL(str_length(u"áéíóú"s, grapheme_units), 5);                                // precomposed
        TEST_EQUAL(str_length(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, grapheme_units), 5);  // decomposed
        TEST_EQUAL(str_length(u"\ufe00Hello \ufe01world"s, grapheme_units), 11);             // nonspacing mark

        TEST_EQUAL(str_length(U""s, grapheme_units), 0);
        TEST_EQUAL(str_length(U"aeiou"s, grapheme_units), 5);
        TEST_EQUAL(str_length(U"áéíóú"s, grapheme_units), 5);                                // precomposed
        TEST_EQUAL(str_length(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, grapheme_units), 5);  // decomposed
        TEST_EQUAL(str_length(U"\ufe00Hello \ufe01world"s, grapheme_units), 11);             // nonspacing mark

        TEST_EQUAL(str_length(u8"ÀÀÀ"s, narrow_context), 3);                    // neutral
        TEST_EQUAL(str_length(u8"ÀÀÀ"s, wide_context), 3);                      // neutral
        TEST_EQUAL(str_length(u8"ààà"s, narrow_context), 3);                    // ambiguous
        TEST_EQUAL(str_length(u8"ààà"s, wide_context), 6);                      // ambiguous
        TEST_EQUAL(str_length(u8"\u3000\u3000\u3000"s, narrow_context), 6);     // fullwidth
        TEST_EQUAL(str_length(u8"\u3000\u3000\u3000"s, wide_context), 6);       // fullwidth
        TEST_EQUAL(str_length(u8"\u20a9\u20a9\u20a9"s, narrow_context), 3);     // halfwidth
        TEST_EQUAL(str_length(u8"\u20a9\u20a9\u20a9"s, wide_context), 3);       // halfwidth
        TEST_EQUAL(str_length(u8"AAA"s, narrow_context), 3);                    // narrow
        TEST_EQUAL(str_length(u8"AAA"s, wide_context), 3);                      // narrow
        TEST_EQUAL(str_length(u8"\u3001\u3001\u3001"s, narrow_context), 6);     // wide
        TEST_EQUAL(str_length(u8"\u3001\u3001\u3001"s, wide_context), 6);       // wide
        TEST_EQUAL(str_length(u8"àààÀÀÀ"s, narrow_context), 6);                 // ambiguous + neutral
        TEST_EQUAL(str_length(u8"àààÀÀÀ"s, wide_context), 9);                   // ambiguous + neutral
        TEST_EQUAL(str_length(u8"ààà\u3000\u3000\u3000"s, narrow_context), 9);  // ambiguous + fullwidth
        TEST_EQUAL(str_length(u8"ààà\u3000\u3000\u3000"s, wide_context), 12);   // ambiguous + fullwidth
        TEST_EQUAL(str_length(u8"ààà\u20a9\u20a9\u20a9"s, narrow_context), 6);  // ambiguous + halfwidth
        TEST_EQUAL(str_length(u8"ààà\u20a9\u20a9\u20a9"s, wide_context), 9);    // ambiguous + halfwidth
        TEST_EQUAL(str_length(u8"àààAAA"s, narrow_context), 6);                 // ambiguous + narrow
        TEST_EQUAL(str_length(u8"àààAAA"s, wide_context), 9);                   // ambiguous + narrow
        TEST_EQUAL(str_length(u8"ààà\u3001\u3001\u3001"s, narrow_context), 9);  // ambiguous + wide
        TEST_EQUAL(str_length(u8"ààà\u3001\u3001\u3001"s, wide_context), 12);   // ambiguous + wide

        TEST_EQUAL(str_length(u"ÀÀÀ"s, narrow_context), 3);                    // neutral
        TEST_EQUAL(str_length(u"ÀÀÀ"s, wide_context), 3);                      // neutral
        TEST_EQUAL(str_length(u"ààà"s, narrow_context), 3);                    // ambiguous
        TEST_EQUAL(str_length(u"ààà"s, wide_context), 6);                      // ambiguous
        TEST_EQUAL(str_length(u"\u3000\u3000\u3000"s, narrow_context), 6);     // fullwidth
        TEST_EQUAL(str_length(u"\u3000\u3000\u3000"s, wide_context), 6);       // fullwidth
        TEST_EQUAL(str_length(u"\u20a9\u20a9\u20a9"s, narrow_context), 3);     // halfwidth
        TEST_EQUAL(str_length(u"\u20a9\u20a9\u20a9"s, wide_context), 3);       // halfwidth
        TEST_EQUAL(str_length(u"AAA"s, narrow_context), 3);                    // narrow
        TEST_EQUAL(str_length(u"AAA"s, wide_context), 3);                      // narrow
        TEST_EQUAL(str_length(u"\u3001\u3001\u3001"s, narrow_context), 6);     // wide
        TEST_EQUAL(str_length(u"\u3001\u3001\u3001"s, wide_context), 6);       // wide
        TEST_EQUAL(str_length(u"àààÀÀÀ"s, narrow_context), 6);                 // ambiguous + neutral
        TEST_EQUAL(str_length(u"àààÀÀÀ"s, wide_context), 9);                   // ambiguous + neutral
        TEST_EQUAL(str_length(u"ààà\u3000\u3000\u3000"s, narrow_context), 9);  // ambiguous + fullwidth
        TEST_EQUAL(str_length(u"ààà\u3000\u3000\u3000"s, wide_context), 12);   // ambiguous + fullwidth
        TEST_EQUAL(str_length(u"ààà\u20a9\u20a9\u20a9"s, narrow_context), 6);  // ambiguous + halfwidth
        TEST_EQUAL(str_length(u"ààà\u20a9\u20a9\u20a9"s, wide_context), 9);    // ambiguous + halfwidth
        TEST_EQUAL(str_length(u"àààAAA"s, narrow_context), 6);                 // ambiguous + narrow
        TEST_EQUAL(str_length(u"àààAAA"s, wide_context), 9);                   // ambiguous + narrow
        TEST_EQUAL(str_length(u"ààà\u3001\u3001\u3001"s, narrow_context), 9);  // ambiguous + wide
        TEST_EQUAL(str_length(u"ààà\u3001\u3001\u3001"s, wide_context), 12);   // ambiguous + wide

        TEST_EQUAL(str_length(U"ÀÀÀ"s, narrow_context), 3);                    // neutral
        TEST_EQUAL(str_length(U"ÀÀÀ"s, wide_context), 3);                      // neutral
        TEST_EQUAL(str_length(U"ààà"s, narrow_context), 3);                    // ambiguous
        TEST_EQUAL(str_length(U"ààà"s, wide_context), 6);                      // ambiguous
        TEST_EQUAL(str_length(U"\u3000\u3000\u3000"s, narrow_context), 6);     // fullwidth
        TEST_EQUAL(str_length(U"\u3000\u3000\u3000"s, wide_context), 6);       // fullwidth
        TEST_EQUAL(str_length(U"\u20a9\u20a9\u20a9"s, narrow_context), 3);     // halfwidth
        TEST_EQUAL(str_length(U"\u20a9\u20a9\u20a9"s, wide_context), 3);       // halfwidth
        TEST_EQUAL(str_length(U"AAA"s, narrow_context), 3);                    // narrow
        TEST_EQUAL(str_length(U"AAA"s, wide_context), 3);                      // narrow
        TEST_EQUAL(str_length(U"\u3001\u3001\u3001"s, narrow_context), 6);     // wide
        TEST_EQUAL(str_length(U"\u3001\u3001\u3001"s, wide_context), 6);       // wide
        TEST_EQUAL(str_length(U"àààÀÀÀ"s, narrow_context), 6);                 // ambiguous + neutral
        TEST_EQUAL(str_length(U"àààÀÀÀ"s, wide_context), 9);                   // ambiguous + neutral
        TEST_EQUAL(str_length(U"ààà\u3000\u3000\u3000"s, narrow_context), 9);  // ambiguous + fullwidth
        TEST_EQUAL(str_length(U"ààà\u3000\u3000\u3000"s, wide_context), 12);   // ambiguous + fullwidth
        TEST_EQUAL(str_length(U"ààà\u20a9\u20a9\u20a9"s, narrow_context), 6);  // ambiguous + halfwidth
        TEST_EQUAL(str_length(U"ààà\u20a9\u20a9\u20a9"s, wide_context), 9);    // ambiguous + halfwidth
        TEST_EQUAL(str_length(U"àààAAA"s, narrow_context), 6);                 // ambiguous + narrow
        TEST_EQUAL(str_length(U"àààAAA"s, wide_context), 9);                   // ambiguous + narrow
        TEST_EQUAL(str_length(U"ààà\u3001\u3001\u3001"s, narrow_context), 9);  // ambiguous + wide
        TEST_EQUAL(str_length(U"ààà\u3001\u3001\u3001"s, wide_context), 12);   // ambiguous + wide

        TEST_EQUAL(str_length(u8""s, grapheme_units | narrow_context), 0);
        TEST_EQUAL(str_length(u8""s, grapheme_units | wide_context), 0);
        TEST_EQUAL(str_length(u8"aeiou"s, grapheme_units | narrow_context), 5);                                // narrow
        TEST_EQUAL(str_length(u8"aeiou"s, grapheme_units | wide_context), 5);                                  // narrow
        TEST_EQUAL(str_length(u8"áéíóú"s, grapheme_units | narrow_context), 5);                                // precomposed neutral
        TEST_EQUAL(str_length(u8"áéíóú"s, grapheme_units | wide_context), 10);                                 // precomposed neutral
        TEST_EQUAL(str_length(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, grapheme_units | narrow_context), 5);  // decomposed narrow
        TEST_EQUAL(str_length(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, grapheme_units | wide_context), 5);    // decomposed narrow
        TEST_EQUAL(str_length(u8"ÀÀÀ"s, grapheme_units | narrow_context), 3);                                  // neutral
        TEST_EQUAL(str_length(u8"ÀÀÀ"s, grapheme_units | wide_context), 3);                                    // neutral
        TEST_EQUAL(str_length(u8"ààà"s, grapheme_units | narrow_context), 3);                                  // ambiguous
        TEST_EQUAL(str_length(u8"ààà"s, grapheme_units | wide_context), 6);                                    // ambiguous
        TEST_EQUAL(str_length(u8"\u3000\u3000\u3000"s, grapheme_units | narrow_context), 6);                   // fullwidth
        TEST_EQUAL(str_length(u8"\u3000\u3000\u3000"s, grapheme_units | wide_context), 6);                     // fullwidth
        TEST_EQUAL(str_length(u8"\u20a9\u20a9\u20a9"s, grapheme_units | narrow_context), 3);                   // halfwidth
        TEST_EQUAL(str_length(u8"\u20a9\u20a9\u20a9"s, grapheme_units | wide_context), 3);                     // halfwidth
        TEST_EQUAL(str_length(u8"AAA"s, grapheme_units | narrow_context), 3);                                  // narrow
        TEST_EQUAL(str_length(u8"AAA"s, grapheme_units | wide_context), 3);                                    // narrow
        TEST_EQUAL(str_length(u8"\u3001\u3001\u3001"s, grapheme_units | narrow_context), 6);                   // wide
        TEST_EQUAL(str_length(u8"\u3001\u3001\u3001"s, grapheme_units | wide_context), 6);                     // wide
        TEST_EQUAL(str_length(u8"àààÀÀÀ"s, grapheme_units | narrow_context), 6);                               // ambiguous + neutral
        TEST_EQUAL(str_length(u8"àààÀÀÀ"s, grapheme_units | wide_context), 9);                                 // ambiguous + neutral
        TEST_EQUAL(str_length(u8"ààà\u3000\u3000\u3000"s, grapheme_units | narrow_context), 9);                // ambiguous + fullwidth
        TEST_EQUAL(str_length(u8"ààà\u3000\u3000\u3000"s, grapheme_units | wide_context), 12);                 // ambiguous + fullwidth
        TEST_EQUAL(str_length(u8"ààà\u20a9\u20a9\u20a9"s, grapheme_units | narrow_context), 6);                // ambiguous + halfwidth
        TEST_EQUAL(str_length(u8"ààà\u20a9\u20a9\u20a9"s, grapheme_units | wide_context), 9);                  // ambiguous + halfwidth
        TEST_EQUAL(str_length(u8"àààAAA"s, grapheme_units | narrow_context), 6);                               // ambiguous + narrow
        TEST_EQUAL(str_length(u8"àààAAA"s, grapheme_units | wide_context), 9);                                 // ambiguous + narrow
        TEST_EQUAL(str_length(u8"ààà\u3001\u3001\u3001"s, grapheme_units | narrow_context), 9);                // ambiguous + wide
        TEST_EQUAL(str_length(u8"ààà\u3001\u3001\u3001"s, grapheme_units | wide_context), 12);                 // ambiguous + wide

        TEST_EQUAL(str_length(u""s, grapheme_units | narrow_context), 0);
        TEST_EQUAL(str_length(u""s, grapheme_units | wide_context), 0);
        TEST_EQUAL(str_length(u"aeiou"s, grapheme_units | narrow_context), 5);                                // narrow
        TEST_EQUAL(str_length(u"aeiou"s, grapheme_units | wide_context), 5);                                  // narrow
        TEST_EQUAL(str_length(u"áéíóú"s, grapheme_units | narrow_context), 5);                                // precomposed neutral
        TEST_EQUAL(str_length(u"áéíóú"s, grapheme_units | wide_context), 10);                                 // precomposed neutral
        TEST_EQUAL(str_length(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, grapheme_units | narrow_context), 5);  // decomposed narrow
        TEST_EQUAL(str_length(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, grapheme_units | wide_context), 5);    // decomposed narrow
        TEST_EQUAL(str_length(u"ÀÀÀ"s, grapheme_units | narrow_context), 3);                                  // neutral
        TEST_EQUAL(str_length(u"ÀÀÀ"s, grapheme_units | wide_context), 3);                                    // neutral
        TEST_EQUAL(str_length(u"ààà"s, grapheme_units | narrow_context), 3);                                  // ambiguous
        TEST_EQUAL(str_length(u"ààà"s, grapheme_units | wide_context), 6);                                    // ambiguous
        TEST_EQUAL(str_length(u"\u3000\u3000\u3000"s, grapheme_units | narrow_context), 6);                   // fullwidth
        TEST_EQUAL(str_length(u"\u3000\u3000\u3000"s, grapheme_units | wide_context), 6);                     // fullwidth
        TEST_EQUAL(str_length(u"\u20a9\u20a9\u20a9"s, grapheme_units | narrow_context), 3);                   // halfwidth
        TEST_EQUAL(str_length(u"\u20a9\u20a9\u20a9"s, grapheme_units | wide_context), 3);                     // halfwidth
        TEST_EQUAL(str_length(u"AAA"s, grapheme_units | narrow_context), 3);                                  // narrow
        TEST_EQUAL(str_length(u"AAA"s, grapheme_units | wide_context), 3);                                    // narrow
        TEST_EQUAL(str_length(u"\u3001\u3001\u3001"s, grapheme_units | narrow_context), 6);                   // wide
        TEST_EQUAL(str_length(u"\u3001\u3001\u3001"s, grapheme_units | wide_context), 6);                     // wide
        TEST_EQUAL(str_length(u"àààÀÀÀ"s, grapheme_units | narrow_context), 6);                               // ambiguous + neutral
        TEST_EQUAL(str_length(u"àààÀÀÀ"s, grapheme_units | wide_context), 9);                                 // ambiguous + neutral
        TEST_EQUAL(str_length(u"ààà\u3000\u3000\u3000"s, grapheme_units | narrow_context), 9);                // ambiguous + fullwidth
        TEST_EQUAL(str_length(u"ààà\u3000\u3000\u3000"s, grapheme_units | wide_context), 12);                 // ambiguous + fullwidth
        TEST_EQUAL(str_length(u"ààà\u20a9\u20a9\u20a9"s, grapheme_units | narrow_context), 6);                // ambiguous + halfwidth
        TEST_EQUAL(str_length(u"ààà\u20a9\u20a9\u20a9"s, grapheme_units | wide_context), 9);                  // ambiguous + halfwidth
        TEST_EQUAL(str_length(u"àààAAA"s, grapheme_units | narrow_context), 6);                               // ambiguous + narrow
        TEST_EQUAL(str_length(u"àààAAA"s, grapheme_units | wide_context), 9);                                 // ambiguous + narrow
        TEST_EQUAL(str_length(u"ààà\u3001\u3001\u3001"s, grapheme_units | narrow_context), 9);                // ambiguous + wide
        TEST_EQUAL(str_length(u"ààà\u3001\u3001\u3001"s, grapheme_units | wide_context), 12);                 // ambiguous + wide

        TEST_EQUAL(str_length(U""s, grapheme_units | narrow_context), 0);
        TEST_EQUAL(str_length(U""s, grapheme_units | wide_context), 0);
        TEST_EQUAL(str_length(U"aeiou"s, grapheme_units | narrow_context), 5);                                // narrow
        TEST_EQUAL(str_length(U"aeiou"s, grapheme_units | wide_context), 5);                                  // narrow
        TEST_EQUAL(str_length(U"áéíóú"s, grapheme_units | narrow_context), 5);                                // precomposed neutral
        TEST_EQUAL(str_length(U"áéíóú"s, grapheme_units | wide_context), 10);                                 // precomposed neutral
        TEST_EQUAL(str_length(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, grapheme_units | narrow_context), 5);  // decomposed narrow
        TEST_EQUAL(str_length(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, grapheme_units | wide_context), 5);    // decomposed narrow
        TEST_EQUAL(str_length(U"ÀÀÀ"s, grapheme_units | narrow_context), 3);                                  // neutral
        TEST_EQUAL(str_length(U"ÀÀÀ"s, grapheme_units | wide_context), 3);                                    // neutral
        TEST_EQUAL(str_length(U"ààà"s, grapheme_units | narrow_context), 3);                                  // ambiguous
        TEST_EQUAL(str_length(U"ààà"s, grapheme_units | wide_context), 6);                                    // ambiguous
        TEST_EQUAL(str_length(U"\u3000\u3000\u3000"s, grapheme_units | narrow_context), 6);                   // fullwidth
        TEST_EQUAL(str_length(U"\u3000\u3000\u3000"s, grapheme_units | wide_context), 6);                     // fullwidth
        TEST_EQUAL(str_length(U"\u20a9\u20a9\u20a9"s, grapheme_units | narrow_context), 3);                   // halfwidth
        TEST_EQUAL(str_length(U"\u20a9\u20a9\u20a9"s, grapheme_units | wide_context), 3);                     // halfwidth
        TEST_EQUAL(str_length(U"AAA"s, grapheme_units | narrow_context), 3);                                  // narrow
        TEST_EQUAL(str_length(U"AAA"s, grapheme_units | wide_context), 3);                                    // narrow
        TEST_EQUAL(str_length(U"\u3001\u3001\u3001"s, grapheme_units | narrow_context), 6);                   // wide
        TEST_EQUAL(str_length(U"\u3001\u3001\u3001"s, grapheme_units | wide_context), 6);                     // wide
        TEST_EQUAL(str_length(U"àààÀÀÀ"s, grapheme_units | narrow_context), 6);                               // ambiguous + neutral
        TEST_EQUAL(str_length(U"àààÀÀÀ"s, grapheme_units | wide_context), 9);                                 // ambiguous + neutral
        TEST_EQUAL(str_length(U"ààà\u3000\u3000\u3000"s, grapheme_units | narrow_context), 9);                // ambiguous + fullwidth
        TEST_EQUAL(str_length(U"ààà\u3000\u3000\u3000"s, grapheme_units | wide_context), 12);                 // ambiguous + fullwidth
        TEST_EQUAL(str_length(U"ààà\u20a9\u20a9\u20a9"s, grapheme_units | narrow_context), 6);                // ambiguous + halfwidth
        TEST_EQUAL(str_length(U"ààà\u20a9\u20a9\u20a9"s, grapheme_units | wide_context), 9);                  // ambiguous + halfwidth
        TEST_EQUAL(str_length(U"àààAAA"s, grapheme_units | narrow_context), 6);                               // ambiguous + narrow
        TEST_EQUAL(str_length(U"àààAAA"s, grapheme_units | wide_context), 9);                                 // ambiguous + narrow
        TEST_EQUAL(str_length(U"ààà\u3001\u3001\u3001"s, grapheme_units | narrow_context), 9);                // ambiguous + wide
        TEST_EQUAL(str_length(U"ààà\u3001\u3001\u3001"s, grapheme_units | wide_context), 12);                 // ambiguous + wide

        TRY(s8 = u8"");                                     TEST_EQUAL(str_length(utf_range(s8), character_units), 0);   TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), character_units), 0);
        TRY(s8 = u8"aeiou");                                TEST_EQUAL(str_length(utf_range(s8), character_units), 5);   TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), character_units), 5);
        TRY(s8 = u8"áéíóú");                                TEST_EQUAL(str_length(utf_range(s8), character_units), 5);   TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), character_units), 5);
        TRY(s8 = u8"a\u0301e\u0301i\u0301o\u0301u\u0301");  TEST_EQUAL(str_length(utf_range(s8), character_units), 10);  TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), character_units), 10);
        TRY(s8 = u8"");                                     TEST_EQUAL(str_length(utf_range(s8), grapheme_units), 0);    TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), grapheme_units), 0);
        TRY(s8 = u8"aeiou");                                TEST_EQUAL(str_length(utf_range(s8), grapheme_units), 5);    TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), grapheme_units), 5);
        TRY(s8 = u8"áéíóú");                                TEST_EQUAL(str_length(utf_range(s8), grapheme_units), 5);    TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), grapheme_units), 5);
        TRY(s8 = u8"a\u0301e\u0301i\u0301o\u0301u\u0301");  TEST_EQUAL(str_length(utf_range(s8), grapheme_units), 5);    TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), grapheme_units), 5);
        TRY(s8 = u8"ààà\u3000\u3000\u3000");                TEST_EQUAL(str_length(utf_range(s8), narrow_context), 9);    TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), narrow_context), 9);
        TRY(s8 = u8"ààà\u3000\u3000\u3000");                TEST_EQUAL(str_length(utf_range(s8), wide_context), 12);     TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), wide_context), 12);

    }

    void check_find_offset() {

        TEST_EQUAL(str_find_offset("ABC"s, 0, character_units), 0);
        TEST_EQUAL(str_find_offset("ABC"s, 1, character_units), 1);
        TEST_EQUAL(str_find_offset("ABC"s, 2, character_units), 2);
        TEST_EQUAL(str_find_offset("ABC"s, 3, character_units), 3);
        TEST_EQUAL(str_find_offset("ABC"s, 4, character_units), npos);
        TEST_EQUAL(str_find_offset("ABC"s, 0, grapheme_units), 0);
        TEST_EQUAL(str_find_offset("ABC"s, 1, grapheme_units), 1);
        TEST_EQUAL(str_find_offset("ABC"s, 2, grapheme_units), 2);
        TEST_EQUAL(str_find_offset("ABC"s, 3, grapheme_units), 3);
        TEST_EQUAL(str_find_offset("ABC"s, 4, grapheme_units), npos);
        TEST_EQUAL(str_find_offset("ABC"s, 0, narrow_context), 0);
        TEST_EQUAL(str_find_offset("ABC"s, 1, narrow_context), 1);
        TEST_EQUAL(str_find_offset("ABC"s, 2, narrow_context), 2);
        TEST_EQUAL(str_find_offset("ABC"s, 3, narrow_context), 3);
        TEST_EQUAL(str_find_offset("ABC"s, 4, narrow_context), npos);
        TEST_EQUAL(str_find_offset("ABC"s, 0, wide_context), 0);
        TEST_EQUAL(str_find_offset("ABC"s, 1, wide_context), 1);
        TEST_EQUAL(str_find_offset("ABC"s, 2, wide_context), 2);
        TEST_EQUAL(str_find_offset("ABC"s, 3, wide_context), 3);
        TEST_EQUAL(str_find_offset("ABC"s, 4, wide_context), npos);

        TEST_EQUAL(str_find_offset(utf8_example, 0, character_units), 0);
        TEST_EQUAL(str_find_offset(utf8_example, 1, character_units), 2);
        TEST_EQUAL(str_find_offset(utf8_example, 2, character_units), 5);
        TEST_EQUAL(str_find_offset(utf8_example, 3, character_units), 9);
        TEST_EQUAL(str_find_offset(utf8_example, 4, character_units), 13);
        TEST_EQUAL(str_find_offset(utf8_example, 5, character_units), npos);
        TEST_EQUAL(str_find_offset(utf8_example, 0, grapheme_units), 0);
        TEST_EQUAL(str_find_offset(utf8_example, 1, grapheme_units), 2);
        TEST_EQUAL(str_find_offset(utf8_example, 2, grapheme_units), 5);
        TEST_EQUAL(str_find_offset(utf8_example, 3, grapheme_units), 9);
        TEST_EQUAL(str_find_offset(utf8_example, 4, grapheme_units), 13);
        TEST_EQUAL(str_find_offset(utf8_example, 5, grapheme_units), npos);

        TEST_EQUAL(str_find_offset(u8"áéíóú"s, 0, grapheme_units), 0);                                   // precomposed
        TEST_EQUAL(str_find_offset(u8"áéíóú"s, 1, grapheme_units), 2);                                   // precomposed
        TEST_EQUAL(str_find_offset(u8"áéíóú"s, 2, grapheme_units), 4);                                   // precomposed
        TEST_EQUAL(str_find_offset(u8"áéíóú"s, 3, grapheme_units), 6);                                   // precomposed
        TEST_EQUAL(str_find_offset(u8"áéíóú"s, 4, grapheme_units), 8);                                   // precomposed
        TEST_EQUAL(str_find_offset(u8"áéíóú"s, 5, grapheme_units), 10);                                  // precomposed
        TEST_EQUAL(str_find_offset(u8"áéíóú"s, 6, grapheme_units), npos);                                // precomposed
        TEST_EQUAL(str_find_offset(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 0, grapheme_units), 0);     // decomposed
        TEST_EQUAL(str_find_offset(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, grapheme_units), 3);     // decomposed
        TEST_EQUAL(str_find_offset(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 2, grapheme_units), 6);     // decomposed
        TEST_EQUAL(str_find_offset(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 3, grapheme_units), 9);     // decomposed
        TEST_EQUAL(str_find_offset(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 4, grapheme_units), 12);    // decomposed
        TEST_EQUAL(str_find_offset(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, grapheme_units), 15);    // decomposed
        TEST_EQUAL(str_find_offset(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 6, grapheme_units), npos);  // decomposed

        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 0, narrow_context), 0);     // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 1, narrow_context), 3);     // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 2, narrow_context), 3);     // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 3, narrow_context), 6);     // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 4, narrow_context), 6);     // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 5, narrow_context), 9);     // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 6, narrow_context), 9);     // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 7, narrow_context), npos);  // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 0, wide_context), 0);       // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 1, wide_context), 3);       // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 2, wide_context), 3);       // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 3, wide_context), 6);       // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 4, wide_context), 6);       // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 5, wide_context), 9);       // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 6, wide_context), 9);       // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u3000\u3000\u3000"s, 7, wide_context), npos);    // fullwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 0, narrow_context), 0);     // halfwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 1, narrow_context), 3);     // halfwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 2, narrow_context), 6);     // halfwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 3, narrow_context), 9);     // halfwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 4, narrow_context), npos);  // halfwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 0, wide_context), 0);       // halfwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 1, wide_context), 3);       // halfwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 2, wide_context), 6);       // halfwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 3, wide_context), 9);       // halfwidth
        TEST_EQUAL(str_find_offset(u8"\u20a9\u20a9\u20a9"s, 4, wide_context), npos);    // halfwidth

        TEST_EQUAL(str_find_offset(u"ABC"s, 0, character_units), 0);
        TEST_EQUAL(str_find_offset(u"ABC"s, 1, character_units), 1);
        TEST_EQUAL(str_find_offset(u"ABC"s, 2, character_units), 2);
        TEST_EQUAL(str_find_offset(u"ABC"s, 3, character_units), 3);
        TEST_EQUAL(str_find_offset(u"ABC"s, 4, character_units), npos);
        TEST_EQUAL(str_find_offset(u"ABC"s, 0, grapheme_units), 0);
        TEST_EQUAL(str_find_offset(u"ABC"s, 1, grapheme_units), 1);
        TEST_EQUAL(str_find_offset(u"ABC"s, 2, grapheme_units), 2);
        TEST_EQUAL(str_find_offset(u"ABC"s, 3, grapheme_units), 3);
        TEST_EQUAL(str_find_offset(u"ABC"s, 4, grapheme_units), npos);
        TEST_EQUAL(str_find_offset(u"ABC"s, 0, narrow_context), 0);
        TEST_EQUAL(str_find_offset(u"ABC"s, 1, narrow_context), 1);
        TEST_EQUAL(str_find_offset(u"ABC"s, 2, narrow_context), 2);
        TEST_EQUAL(str_find_offset(u"ABC"s, 3, narrow_context), 3);
        TEST_EQUAL(str_find_offset(u"ABC"s, 4, narrow_context), npos);
        TEST_EQUAL(str_find_offset(u"ABC"s, 0, wide_context), 0);
        TEST_EQUAL(str_find_offset(u"ABC"s, 1, wide_context), 1);
        TEST_EQUAL(str_find_offset(u"ABC"s, 2, wide_context), 2);
        TEST_EQUAL(str_find_offset(u"ABC"s, 3, wide_context), 3);
        TEST_EQUAL(str_find_offset(u"ABC"s, 4, wide_context), npos);

        TEST_EQUAL(str_find_offset(utf16_example, 0, character_units), 0);
        TEST_EQUAL(str_find_offset(utf16_example, 1, character_units), 1);
        TEST_EQUAL(str_find_offset(utf16_example, 2, character_units), 2);
        TEST_EQUAL(str_find_offset(utf16_example, 3, character_units), 4);
        TEST_EQUAL(str_find_offset(utf16_example, 4, character_units), 6);
        TEST_EQUAL(str_find_offset(utf16_example, 5, character_units), npos);
        TEST_EQUAL(str_find_offset(utf16_example, 0, grapheme_units), 0);
        TEST_EQUAL(str_find_offset(utf16_example, 1, grapheme_units), 1);
        TEST_EQUAL(str_find_offset(utf16_example, 2, grapheme_units), 2);
        TEST_EQUAL(str_find_offset(utf16_example, 3, grapheme_units), 4);
        TEST_EQUAL(str_find_offset(utf16_example, 4, grapheme_units), 6);
        TEST_EQUAL(str_find_offset(utf16_example, 5, grapheme_units), npos);

        TEST_EQUAL(str_find_offset(u"áéíóú"s, 0, grapheme_units), 0);                                   // precomposed
        TEST_EQUAL(str_find_offset(u"áéíóú"s, 1, grapheme_units), 1);                                   // precomposed
        TEST_EQUAL(str_find_offset(u"áéíóú"s, 2, grapheme_units), 2);                                   // precomposed
        TEST_EQUAL(str_find_offset(u"áéíóú"s, 3, grapheme_units), 3);                                   // precomposed
        TEST_EQUAL(str_find_offset(u"áéíóú"s, 4, grapheme_units), 4);                                   // precomposed
        TEST_EQUAL(str_find_offset(u"áéíóú"s, 5, grapheme_units), 5);                                   // precomposed
        TEST_EQUAL(str_find_offset(u"áéíóú"s, 6, grapheme_units), npos);                                // precomposed
        TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 0, grapheme_units), 0);     // decomposed
        TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, grapheme_units), 2);     // decomposed
        TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 2, grapheme_units), 4);     // decomposed
        TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 3, grapheme_units), 6);     // decomposed
        TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 4, grapheme_units), 8);     // decomposed
        TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, grapheme_units), 10);    // decomposed
        TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 6, grapheme_units), npos);  // decomposed

        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 0, narrow_context), 0);     // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 1, narrow_context), 1);     // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 2, narrow_context), 1);     // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 3, narrow_context), 2);     // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 4, narrow_context), 2);     // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 5, narrow_context), 3);     // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 6, narrow_context), 3);     // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 7, narrow_context), npos);  // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 0, wide_context), 0);       // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 1, wide_context), 1);       // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 2, wide_context), 1);       // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 3, wide_context), 2);       // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 4, wide_context), 2);       // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 5, wide_context), 3);       // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 6, wide_context), 3);       // fullwidth
        TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 7, wide_context), npos);    // fullwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 0, narrow_context), 0);     // halfwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 1, narrow_context), 1);     // halfwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 2, narrow_context), 2);     // halfwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 3, narrow_context), 3);     // halfwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 4, narrow_context), npos);  // halfwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 0, wide_context), 0);       // halfwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 1, wide_context), 1);       // halfwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 2, wide_context), 2);       // halfwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 3, wide_context), 3);       // halfwidth
        TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 4, wide_context), npos);    // halfwidth

        TEST_EQUAL(str_find_offset(U"ABC"s, 0, character_units), 0);
        TEST_EQUAL(str_find_offset(U"ABC"s, 1, character_units), 1);
        TEST_EQUAL(str_find_offset(U"ABC"s, 2, character_units), 2);
        TEST_EQUAL(str_find_offset(U"ABC"s, 3, character_units), 3);
        TEST_EQUAL(str_find_offset(U"ABC"s, 4, character_units), npos);
        TEST_EQUAL(str_find_offset(U"ABC"s, 0, grapheme_units), 0);
        TEST_EQUAL(str_find_offset(U"ABC"s, 1, grapheme_units), 1);
        TEST_EQUAL(str_find_offset(U"ABC"s, 2, grapheme_units), 2);
        TEST_EQUAL(str_find_offset(U"ABC"s, 3, grapheme_units), 3);
        TEST_EQUAL(str_find_offset(U"ABC"s, 4, grapheme_units), npos);
        TEST_EQUAL(str_find_offset(U"ABC"s, 0, narrow_context), 0);
        TEST_EQUAL(str_find_offset(U"ABC"s, 1, narrow_context), 1);
        TEST_EQUAL(str_find_offset(U"ABC"s, 2, narrow_context), 2);
        TEST_EQUAL(str_find_offset(U"ABC"s, 3, narrow_context), 3);
        TEST_EQUAL(str_find_offset(U"ABC"s, 4, narrow_context), npos);
        TEST_EQUAL(str_find_offset(U"ABC"s, 0, wide_context), 0);
        TEST_EQUAL(str_find_offset(U"ABC"s, 1, wide_context), 1);
        TEST_EQUAL(str_find_offset(U"ABC"s, 2, wide_context), 2);
        TEST_EQUAL(str_find_offset(U"ABC"s, 3, wide_context), 3);
        TEST_EQUAL(str_find_offset(U"ABC"s, 4, wide_context), npos);

        TEST_EQUAL(str_find_offset(utf32_example, 0, character_units), 0);
        TEST_EQUAL(str_find_offset(utf32_example, 1, character_units), 1);
        TEST_EQUAL(str_find_offset(utf32_example, 2, character_units), 2);
        TEST_EQUAL(str_find_offset(utf32_example, 3, character_units), 3);
        TEST_EQUAL(str_find_offset(utf32_example, 4, character_units), 4);
        TEST_EQUAL(str_find_offset(utf32_example, 5, character_units), npos);
        TEST_EQUAL(str_find_offset(utf32_example, 0, grapheme_units), 0);
        TEST_EQUAL(str_find_offset(utf32_example, 1, grapheme_units), 1);
        TEST_EQUAL(str_find_offset(utf32_example, 2, grapheme_units), 2);
        TEST_EQUAL(str_find_offset(utf32_example, 3, grapheme_units), 3);
        TEST_EQUAL(str_find_offset(utf32_example, 4, grapheme_units), 4);
        TEST_EQUAL(str_find_offset(utf32_example, 5, grapheme_units), npos);

        TEST_EQUAL(str_find_offset(U"áéíóú"s, 0, grapheme_units), 0);                                   // precomposed
        TEST_EQUAL(str_find_offset(U"áéíóú"s, 1, grapheme_units), 1);                                   // precomposed
        TEST_EQUAL(str_find_offset(U"áéíóú"s, 2, grapheme_units), 2);                                   // precomposed
        TEST_EQUAL(str_find_offset(U"áéíóú"s, 3, grapheme_units), 3);                                   // precomposed
        TEST_EQUAL(str_find_offset(U"áéíóú"s, 4, grapheme_units), 4);                                   // precomposed
        TEST_EQUAL(str_find_offset(U"áéíóú"s, 5, grapheme_units), 5);                                   // precomposed
        TEST_EQUAL(str_find_offset(U"áéíóú"s, 6, grapheme_units), npos);                                // precomposed
        TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 0, grapheme_units), 0);     // decomposed
        TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, grapheme_units), 2);     // decomposed
        TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 2, grapheme_units), 4);     // decomposed
        TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 3, grapheme_units), 6);     // decomposed
        TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 4, grapheme_units), 8);     // decomposed
        TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, grapheme_units), 10);    // decomposed
        TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 6, grapheme_units), npos);  // decomposed

        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 0, narrow_context), 0);     // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 1, narrow_context), 1);     // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 2, narrow_context), 1);     // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 3, narrow_context), 2);     // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 4, narrow_context), 2);     // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 5, narrow_context), 3);     // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 6, narrow_context), 3);     // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 7, narrow_context), npos);  // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 0, wide_context), 0);       // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 1, wide_context), 1);       // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 2, wide_context), 1);       // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 3, wide_context), 2);       // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 4, wide_context), 2);       // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 5, wide_context), 3);       // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 6, wide_context), 3);       // fullwidth
        TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 7, wide_context), npos);    // fullwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 0, narrow_context), 0);     // halfwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 1, narrow_context), 1);     // halfwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 2, narrow_context), 2);     // halfwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 3, narrow_context), 3);     // halfwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 4, narrow_context), npos);  // halfwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 0, wide_context), 0);       // halfwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 1, wide_context), 1);       // halfwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 2, wide_context), 2);       // halfwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 3, wide_context), 3);       // halfwidth
        TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 4, wide_context), npos);    // halfwidth

    }

}

TEST_MODULE(unicorn, string_size) {

    check_length();
    check_find_offset();

}
