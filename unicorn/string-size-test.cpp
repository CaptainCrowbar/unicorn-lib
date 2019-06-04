#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <string>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

namespace {

    // UTF-32    UTF-16     UTF-8
    // 0000004d  004d       4d
    // 00000430  0430       d0 b0
    // 00004e8c  4e8c       e4 ba 8c
    // 00010302  d800 df02  f0 90 8c 82
    // 0010fffd  dbff dffd  f4 8f bf bd

    const Ustring utf8_example {"\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};
    const std::u16string utf16_example {0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
    const std::u32string utf32_example {0x430,0x4e8c,0x10302,0x10fffd};

    #if defined(RS_WCHAR_UTF16)
        const std::wstring wide_example(utf16_example.begin(), utf16_example.end());
    #else
        const std::wstring wide_example(utf32_example.begin(), utf32_example.end());
    #endif

}

void test_unicorn_string_size_measurement_flags() {

    Ustring s8;
    std::u16string s16;
    std::u32string s32;

    TEST_EQUAL(str_length(""s, Length::characters), 0);
    TEST_EQUAL(str_length("Hello"s, Length::characters), 5);
    TEST_EQUAL(str_length(utf8_example, Length::characters), 4);

    TEST_EQUAL(str_length(u""s, Length::characters), 0);
    TEST_EQUAL(str_length(u"Hello"s, Length::characters), 5);
    TEST_EQUAL(str_length(utf16_example, Length::characters), 4);

    TEST_EQUAL(str_length(U""s, Length::characters), 0);
    TEST_EQUAL(str_length(U"Hello"s, Length::characters), 5);
    TEST_EQUAL(str_length(utf32_example, Length::characters), 4);

    TEST_EQUAL(str_length(""s, Length::graphemes), 0);
    TEST_EQUAL(str_length("aeiou"s, Length::graphemes), 5);
    TEST_EQUAL(str_length("áéíóú"s, Length::graphemes), 5);                                // precomposed
    TEST_EQUAL(str_length("a\u0301e\u0301i\u0301o\u0301u\u0301"s, Length::graphemes), 5);  // decomposed
    TEST_EQUAL(str_length("\ufe00Hello \ufe01world"s, Length::graphemes), 11);             // nonspacing mark

    TEST_EQUAL(str_length(u""s, Length::graphemes), 0);
    TEST_EQUAL(str_length(u"aeiou"s, Length::graphemes), 5);
    TEST_EQUAL(str_length(u"áéíóú"s, Length::graphemes), 5);                                // precomposed
    TEST_EQUAL(str_length(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, Length::graphemes), 5);  // decomposed
    TEST_EQUAL(str_length(u"\ufe00Hello \ufe01world"s, Length::graphemes), 11);             // nonspacing mark

    TEST_EQUAL(str_length(U""s, Length::graphemes), 0);
    TEST_EQUAL(str_length(U"aeiou"s, Length::graphemes), 5);
    TEST_EQUAL(str_length(U"áéíóú"s, Length::graphemes), 5);                                // precomposed
    TEST_EQUAL(str_length(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, Length::graphemes), 5);  // decomposed
    TEST_EQUAL(str_length(U"\ufe00Hello \ufe01world"s, Length::graphemes), 11);             // nonspacing mark

    TEST_EQUAL(str_length("ÀÀÀ"s, Length::narrow), 3);                    // neutral
    TEST_EQUAL(str_length("ÀÀÀ"s, Length::wide), 3);                      // neutral
    TEST_EQUAL(str_length("ààà"s, Length::narrow), 3);                    // ambiguous
    TEST_EQUAL(str_length("ààà"s, Length::wide), 6);                      // ambiguous
    TEST_EQUAL(str_length("\u3000\u3000\u3000"s, Length::narrow), 6);     // fullwidth
    TEST_EQUAL(str_length("\u3000\u3000\u3000"s, Length::wide), 6);       // fullwidth
    TEST_EQUAL(str_length("\u20a9\u20a9\u20a9"s, Length::narrow), 3);     // halfwidth
    TEST_EQUAL(str_length("\u20a9\u20a9\u20a9"s, Length::wide), 3);       // halfwidth
    TEST_EQUAL(str_length("AAA"s, Length::narrow), 3);                    // narrow
    TEST_EQUAL(str_length("AAA"s, Length::wide), 3);                      // narrow
    TEST_EQUAL(str_length("\u3001\u3001\u3001"s, Length::narrow), 6);     // wide
    TEST_EQUAL(str_length("\u3001\u3001\u3001"s, Length::wide), 6);       // wide
    TEST_EQUAL(str_length("àààÀÀÀ"s, Length::narrow), 6);                 // ambiguous + neutral
    TEST_EQUAL(str_length("àààÀÀÀ"s, Length::wide), 9);                   // ambiguous + neutral
    TEST_EQUAL(str_length("ààà\u3000\u3000\u3000"s, Length::narrow), 9);  // ambiguous + fullwidth
    TEST_EQUAL(str_length("ààà\u3000\u3000\u3000"s, Length::wide), 12);   // ambiguous + fullwidth
    TEST_EQUAL(str_length("ààà\u20a9\u20a9\u20a9"s, Length::narrow), 6);  // ambiguous + halfwidth
    TEST_EQUAL(str_length("ààà\u20a9\u20a9\u20a9"s, Length::wide), 9);    // ambiguous + halfwidth
    TEST_EQUAL(str_length("àààAAA"s, Length::narrow), 6);                 // ambiguous + narrow
    TEST_EQUAL(str_length("àààAAA"s, Length::wide), 9);                   // ambiguous + narrow
    TEST_EQUAL(str_length("ààà\u3001\u3001\u3001"s, Length::narrow), 9);  // ambiguous + wide
    TEST_EQUAL(str_length("ààà\u3001\u3001\u3001"s, Length::wide), 12);   // ambiguous + wide

    TEST_EQUAL(str_length(u"ÀÀÀ"s, Length::narrow), 3);                    // neutral
    TEST_EQUAL(str_length(u"ÀÀÀ"s, Length::wide), 3);                      // neutral
    TEST_EQUAL(str_length(u"ààà"s, Length::narrow), 3);                    // ambiguous
    TEST_EQUAL(str_length(u"ààà"s, Length::wide), 6);                      // ambiguous
    TEST_EQUAL(str_length(u"\u3000\u3000\u3000"s, Length::narrow), 6);     // fullwidth
    TEST_EQUAL(str_length(u"\u3000\u3000\u3000"s, Length::wide), 6);       // fullwidth
    TEST_EQUAL(str_length(u"\u20a9\u20a9\u20a9"s, Length::narrow), 3);     // halfwidth
    TEST_EQUAL(str_length(u"\u20a9\u20a9\u20a9"s, Length::wide), 3);       // halfwidth
    TEST_EQUAL(str_length(u"AAA"s, Length::narrow), 3);                    // narrow
    TEST_EQUAL(str_length(u"AAA"s, Length::wide), 3);                      // narrow
    TEST_EQUAL(str_length(u"\u3001\u3001\u3001"s, Length::narrow), 6);     // wide
    TEST_EQUAL(str_length(u"\u3001\u3001\u3001"s, Length::wide), 6);       // wide
    TEST_EQUAL(str_length(u"àààÀÀÀ"s, Length::narrow), 6);                 // ambiguous + neutral
    TEST_EQUAL(str_length(u"àààÀÀÀ"s, Length::wide), 9);                   // ambiguous + neutral
    TEST_EQUAL(str_length(u"ààà\u3000\u3000\u3000"s, Length::narrow), 9);  // ambiguous + fullwidth
    TEST_EQUAL(str_length(u"ààà\u3000\u3000\u3000"s, Length::wide), 12);   // ambiguous + fullwidth
    TEST_EQUAL(str_length(u"ààà\u20a9\u20a9\u20a9"s, Length::narrow), 6);  // ambiguous + halfwidth
    TEST_EQUAL(str_length(u"ààà\u20a9\u20a9\u20a9"s, Length::wide), 9);    // ambiguous + halfwidth
    TEST_EQUAL(str_length(u"àààAAA"s, Length::narrow), 6);                 // ambiguous + narrow
    TEST_EQUAL(str_length(u"àààAAA"s, Length::wide), 9);                   // ambiguous + narrow
    TEST_EQUAL(str_length(u"ààà\u3001\u3001\u3001"s, Length::narrow), 9);  // ambiguous + wide
    TEST_EQUAL(str_length(u"ààà\u3001\u3001\u3001"s, Length::wide), 12);   // ambiguous + wide

    TEST_EQUAL(str_length(U"ÀÀÀ"s, Length::narrow), 3);                    // neutral
    TEST_EQUAL(str_length(U"ÀÀÀ"s, Length::wide), 3);                      // neutral
    TEST_EQUAL(str_length(U"ààà"s, Length::narrow), 3);                    // ambiguous
    TEST_EQUAL(str_length(U"ààà"s, Length::wide), 6);                      // ambiguous
    TEST_EQUAL(str_length(U"\u3000\u3000\u3000"s, Length::narrow), 6);     // fullwidth
    TEST_EQUAL(str_length(U"\u3000\u3000\u3000"s, Length::wide), 6);       // fullwidth
    TEST_EQUAL(str_length(U"\u20a9\u20a9\u20a9"s, Length::narrow), 3);     // halfwidth
    TEST_EQUAL(str_length(U"\u20a9\u20a9\u20a9"s, Length::wide), 3);       // halfwidth
    TEST_EQUAL(str_length(U"AAA"s, Length::narrow), 3);                    // narrow
    TEST_EQUAL(str_length(U"AAA"s, Length::wide), 3);                      // narrow
    TEST_EQUAL(str_length(U"\u3001\u3001\u3001"s, Length::narrow), 6);     // wide
    TEST_EQUAL(str_length(U"\u3001\u3001\u3001"s, Length::wide), 6);       // wide
    TEST_EQUAL(str_length(U"àààÀÀÀ"s, Length::narrow), 6);                 // ambiguous + neutral
    TEST_EQUAL(str_length(U"àààÀÀÀ"s, Length::wide), 9);                   // ambiguous + neutral
    TEST_EQUAL(str_length(U"ààà\u3000\u3000\u3000"s, Length::narrow), 9);  // ambiguous + fullwidth
    TEST_EQUAL(str_length(U"ààà\u3000\u3000\u3000"s, Length::wide), 12);   // ambiguous + fullwidth
    TEST_EQUAL(str_length(U"ààà\u20a9\u20a9\u20a9"s, Length::narrow), 6);  // ambiguous + halfwidth
    TEST_EQUAL(str_length(U"ààà\u20a9\u20a9\u20a9"s, Length::wide), 9);    // ambiguous + halfwidth
    TEST_EQUAL(str_length(U"àààAAA"s, Length::narrow), 6);                 // ambiguous + narrow
    TEST_EQUAL(str_length(U"àààAAA"s, Length::wide), 9);                   // ambiguous + narrow
    TEST_EQUAL(str_length(U"ààà\u3001\u3001\u3001"s, Length::narrow), 9);  // ambiguous + wide
    TEST_EQUAL(str_length(U"ààà\u3001\u3001\u3001"s, Length::wide), 12);   // ambiguous + wide

    TEST_EQUAL(str_length(""s, Length::graphemes | Length::narrow), 0);
    TEST_EQUAL(str_length(""s, Length::graphemes | Length::wide), 0);
    TEST_EQUAL(str_length("aeiou"s, Length::graphemes | Length::narrow), 5);                                // narrow
    TEST_EQUAL(str_length("aeiou"s, Length::graphemes | Length::wide), 5);                                  // narrow
    TEST_EQUAL(str_length("áéíóú"s, Length::graphemes | Length::narrow), 5);                                // precomposed neutral
    TEST_EQUAL(str_length("áéíóú"s, Length::graphemes | Length::wide), 10);                                 // precomposed neutral
    TEST_EQUAL(str_length("a\u0301e\u0301i\u0301o\u0301u\u0301"s, Length::graphemes | Length::narrow), 5);  // decomposed narrow
    TEST_EQUAL(str_length("a\u0301e\u0301i\u0301o\u0301u\u0301"s, Length::graphemes | Length::wide), 5);    // decomposed narrow
    TEST_EQUAL(str_length("ÀÀÀ"s, Length::graphemes | Length::narrow), 3);                                  // neutral
    TEST_EQUAL(str_length("ÀÀÀ"s, Length::graphemes | Length::wide), 3);                                    // neutral
    TEST_EQUAL(str_length("ààà"s, Length::graphemes | Length::narrow), 3);                                  // ambiguous
    TEST_EQUAL(str_length("ààà"s, Length::graphemes | Length::wide), 6);                                    // ambiguous
    TEST_EQUAL(str_length("\u3000\u3000\u3000"s, Length::graphemes | Length::narrow), 6);                   // fullwidth
    TEST_EQUAL(str_length("\u3000\u3000\u3000"s, Length::graphemes | Length::wide), 6);                     // fullwidth
    TEST_EQUAL(str_length("\u20a9\u20a9\u20a9"s, Length::graphemes | Length::narrow), 3);                   // halfwidth
    TEST_EQUAL(str_length("\u20a9\u20a9\u20a9"s, Length::graphemes | Length::wide), 3);                     // halfwidth
    TEST_EQUAL(str_length("AAA"s, Length::graphemes | Length::narrow), 3);                                  // narrow
    TEST_EQUAL(str_length("AAA"s, Length::graphemes | Length::wide), 3);                                    // narrow
    TEST_EQUAL(str_length("\u3001\u3001\u3001"s, Length::graphemes | Length::narrow), 6);                   // wide
    TEST_EQUAL(str_length("\u3001\u3001\u3001"s, Length::graphemes | Length::wide), 6);                     // wide
    TEST_EQUAL(str_length("àààÀÀÀ"s, Length::graphemes | Length::narrow), 6);                               // ambiguous + neutral
    TEST_EQUAL(str_length("àààÀÀÀ"s, Length::graphemes | Length::wide), 9);                                 // ambiguous + neutral
    TEST_EQUAL(str_length("ààà\u3000\u3000\u3000"s, Length::graphemes | Length::narrow), 9);                // ambiguous + fullwidth
    TEST_EQUAL(str_length("ààà\u3000\u3000\u3000"s, Length::graphemes | Length::wide), 12);                 // ambiguous + fullwidth
    TEST_EQUAL(str_length("ààà\u20a9\u20a9\u20a9"s, Length::graphemes | Length::narrow), 6);                // ambiguous + halfwidth
    TEST_EQUAL(str_length("ààà\u20a9\u20a9\u20a9"s, Length::graphemes | Length::wide), 9);                  // ambiguous + halfwidth
    TEST_EQUAL(str_length("àààAAA"s, Length::graphemes | Length::narrow), 6);                               // ambiguous + narrow
    TEST_EQUAL(str_length("àààAAA"s, Length::graphemes | Length::wide), 9);                                 // ambiguous + narrow
    TEST_EQUAL(str_length("ààà\u3001\u3001\u3001"s, Length::graphemes | Length::narrow), 9);                // ambiguous + wide
    TEST_EQUAL(str_length("ààà\u3001\u3001\u3001"s, Length::graphemes | Length::wide), 12);                 // ambiguous + wide

    TEST_EQUAL(str_length(u""s, Length::graphemes | Length::narrow), 0);
    TEST_EQUAL(str_length(u""s, Length::graphemes | Length::wide), 0);
    TEST_EQUAL(str_length(u"aeiou"s, Length::graphemes | Length::narrow), 5);                                // narrow
    TEST_EQUAL(str_length(u"aeiou"s, Length::graphemes | Length::wide), 5);                                  // narrow
    TEST_EQUAL(str_length(u"áéíóú"s, Length::graphemes | Length::narrow), 5);                                // precomposed neutral
    TEST_EQUAL(str_length(u"áéíóú"s, Length::graphemes | Length::wide), 10);                                 // precomposed neutral
    TEST_EQUAL(str_length(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, Length::graphemes | Length::narrow), 5);  // decomposed narrow
    TEST_EQUAL(str_length(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, Length::graphemes | Length::wide), 5);    // decomposed narrow
    TEST_EQUAL(str_length(u"ÀÀÀ"s, Length::graphemes | Length::narrow), 3);                                  // neutral
    TEST_EQUAL(str_length(u"ÀÀÀ"s, Length::graphemes | Length::wide), 3);                                    // neutral
    TEST_EQUAL(str_length(u"ààà"s, Length::graphemes | Length::narrow), 3);                                  // ambiguous
    TEST_EQUAL(str_length(u"ààà"s, Length::graphemes | Length::wide), 6);                                    // ambiguous
    TEST_EQUAL(str_length(u"\u3000\u3000\u3000"s, Length::graphemes | Length::narrow), 6);                   // fullwidth
    TEST_EQUAL(str_length(u"\u3000\u3000\u3000"s, Length::graphemes | Length::wide), 6);                     // fullwidth
    TEST_EQUAL(str_length(u"\u20a9\u20a9\u20a9"s, Length::graphemes | Length::narrow), 3);                   // halfwidth
    TEST_EQUAL(str_length(u"\u20a9\u20a9\u20a9"s, Length::graphemes | Length::wide), 3);                     // halfwidth
    TEST_EQUAL(str_length(u"AAA"s, Length::graphemes | Length::narrow), 3);                                  // narrow
    TEST_EQUAL(str_length(u"AAA"s, Length::graphemes | Length::wide), 3);                                    // narrow
    TEST_EQUAL(str_length(u"\u3001\u3001\u3001"s, Length::graphemes | Length::narrow), 6);                   // wide
    TEST_EQUAL(str_length(u"\u3001\u3001\u3001"s, Length::graphemes | Length::wide), 6);                     // wide
    TEST_EQUAL(str_length(u"àààÀÀÀ"s, Length::graphemes | Length::narrow), 6);                               // ambiguous + neutral
    TEST_EQUAL(str_length(u"àààÀÀÀ"s, Length::graphemes | Length::wide), 9);                                 // ambiguous + neutral
    TEST_EQUAL(str_length(u"ààà\u3000\u3000\u3000"s, Length::graphemes | Length::narrow), 9);                // ambiguous + fullwidth
    TEST_EQUAL(str_length(u"ààà\u3000\u3000\u3000"s, Length::graphemes | Length::wide), 12);                 // ambiguous + fullwidth
    TEST_EQUAL(str_length(u"ààà\u20a9\u20a9\u20a9"s, Length::graphemes | Length::narrow), 6);                // ambiguous + halfwidth
    TEST_EQUAL(str_length(u"ààà\u20a9\u20a9\u20a9"s, Length::graphemes | Length::wide), 9);                  // ambiguous + halfwidth
    TEST_EQUAL(str_length(u"àààAAA"s, Length::graphemes | Length::narrow), 6);                               // ambiguous + narrow
    TEST_EQUAL(str_length(u"àààAAA"s, Length::graphemes | Length::wide), 9);                                 // ambiguous + narrow
    TEST_EQUAL(str_length(u"ààà\u3001\u3001\u3001"s, Length::graphemes | Length::narrow), 9);                // ambiguous + wide
    TEST_EQUAL(str_length(u"ààà\u3001\u3001\u3001"s, Length::graphemes | Length::wide), 12);                 // ambiguous + wide

    TEST_EQUAL(str_length(U""s, Length::graphemes | Length::narrow), 0);
    TEST_EQUAL(str_length(U""s, Length::graphemes | Length::wide), 0);
    TEST_EQUAL(str_length(U"aeiou"s, Length::graphemes | Length::narrow), 5);                                // narrow
    TEST_EQUAL(str_length(U"aeiou"s, Length::graphemes | Length::wide), 5);                                  // narrow
    TEST_EQUAL(str_length(U"áéíóú"s, Length::graphemes | Length::narrow), 5);                                // precomposed neutral
    TEST_EQUAL(str_length(U"áéíóú"s, Length::graphemes | Length::wide), 10);                                 // precomposed neutral
    TEST_EQUAL(str_length(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, Length::graphemes | Length::narrow), 5);  // decomposed narrow
    TEST_EQUAL(str_length(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, Length::graphemes | Length::wide), 5);    // decomposed narrow
    TEST_EQUAL(str_length(U"ÀÀÀ"s, Length::graphemes | Length::narrow), 3);                                  // neutral
    TEST_EQUAL(str_length(U"ÀÀÀ"s, Length::graphemes | Length::wide), 3);                                    // neutral
    TEST_EQUAL(str_length(U"ààà"s, Length::graphemes | Length::narrow), 3);                                  // ambiguous
    TEST_EQUAL(str_length(U"ààà"s, Length::graphemes | Length::wide), 6);                                    // ambiguous
    TEST_EQUAL(str_length(U"\u3000\u3000\u3000"s, Length::graphemes | Length::narrow), 6);                   // fullwidth
    TEST_EQUAL(str_length(U"\u3000\u3000\u3000"s, Length::graphemes | Length::wide), 6);                     // fullwidth
    TEST_EQUAL(str_length(U"\u20a9\u20a9\u20a9"s, Length::graphemes | Length::narrow), 3);                   // halfwidth
    TEST_EQUAL(str_length(U"\u20a9\u20a9\u20a9"s, Length::graphemes | Length::wide), 3);                     // halfwidth
    TEST_EQUAL(str_length(U"AAA"s, Length::graphemes | Length::narrow), 3);                                  // narrow
    TEST_EQUAL(str_length(U"AAA"s, Length::graphemes | Length::wide), 3);                                    // narrow
    TEST_EQUAL(str_length(U"\u3001\u3001\u3001"s, Length::graphemes | Length::narrow), 6);                   // wide
    TEST_EQUAL(str_length(U"\u3001\u3001\u3001"s, Length::graphemes | Length::wide), 6);                     // wide
    TEST_EQUAL(str_length(U"àààÀÀÀ"s, Length::graphemes | Length::narrow), 6);                               // ambiguous + neutral
    TEST_EQUAL(str_length(U"àààÀÀÀ"s, Length::graphemes | Length::wide), 9);                                 // ambiguous + neutral
    TEST_EQUAL(str_length(U"ààà\u3000\u3000\u3000"s, Length::graphemes | Length::narrow), 9);                // ambiguous + fullwidth
    TEST_EQUAL(str_length(U"ààà\u3000\u3000\u3000"s, Length::graphemes | Length::wide), 12);                 // ambiguous + fullwidth
    TEST_EQUAL(str_length(U"ààà\u20a9\u20a9\u20a9"s, Length::graphemes | Length::narrow), 6);                // ambiguous + halfwidth
    TEST_EQUAL(str_length(U"ààà\u20a9\u20a9\u20a9"s, Length::graphemes | Length::wide), 9);                  // ambiguous + halfwidth
    TEST_EQUAL(str_length(U"àààAAA"s, Length::graphemes | Length::narrow), 6);                               // ambiguous + narrow
    TEST_EQUAL(str_length(U"àààAAA"s, Length::graphemes | Length::wide), 9);                                 // ambiguous + narrow
    TEST_EQUAL(str_length(U"ààà\u3001\u3001\u3001"s, Length::graphemes | Length::narrow), 9);                // ambiguous + wide
    TEST_EQUAL(str_length(U"ààà\u3001\u3001\u3001"s, Length::graphemes | Length::wide), 12);                 // ambiguous + wide

    TRY(s8 = "");                                     TEST_EQUAL(str_length(utf_range(s8), Length::characters), 0);   TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::characters), 0);
    TRY(s8 = "aeiou");                                TEST_EQUAL(str_length(utf_range(s8), Length::characters), 5);   TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::characters), 5);
    TRY(s8 = "áéíóú");                                TEST_EQUAL(str_length(utf_range(s8), Length::characters), 5);   TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::characters), 5);
    TRY(s8 = "a\u0301e\u0301i\u0301o\u0301u\u0301");  TEST_EQUAL(str_length(utf_range(s8), Length::characters), 10);  TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::characters), 10);
    TRY(s8 = "");                                     TEST_EQUAL(str_length(utf_range(s8), Length::graphemes), 0);    TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::graphemes), 0);
    TRY(s8 = "aeiou");                                TEST_EQUAL(str_length(utf_range(s8), Length::graphemes), 5);    TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::graphemes), 5);
    TRY(s8 = "áéíóú");                                TEST_EQUAL(str_length(utf_range(s8), Length::graphemes), 5);    TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::graphemes), 5);
    TRY(s8 = "a\u0301e\u0301i\u0301o\u0301u\u0301");  TEST_EQUAL(str_length(utf_range(s8), Length::graphemes), 5);    TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::graphemes), 5);
    TRY(s8 = "ààà\u3000\u3000\u3000");                TEST_EQUAL(str_length(utf_range(s8), Length::narrow), 9);       TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::narrow), 9);
    TRY(s8 = "ààà\u3000\u3000\u3000");                TEST_EQUAL(str_length(utf_range(s8), Length::wide), 12);        TEST_EQUAL(str_length(utf_begin(s8), utf_end(s8), Length::wide), 12);

    TRY(s8 = R"(¯\_(ツ)_/¯)");
    TEST_EQUAL(str_length(s8, Length::characters), 9);
    TEST_EQUAL(str_length(s8, Length::graphemes), 9);
    TEST_EQUAL(str_length(s8, Length::narrow), 10);
    TEST_EQUAL(str_length(s8, Length::wide), 10);
    TEST_EQUAL(str_length(s8, Length::graphemes | Length::narrow), 10);
    TEST_EQUAL(str_length(s8, Length::graphemes | Length::wide), 10);

}

void test_unicorn_string_size_find_offset() {

    TEST_EQUAL(str_find_offset("ABC"s, 0, Length::characters), 0);
    TEST_EQUAL(str_find_offset("ABC"s, 1, Length::characters), 1);
    TEST_EQUAL(str_find_offset("ABC"s, 2, Length::characters), 2);
    TEST_EQUAL(str_find_offset("ABC"s, 3, Length::characters), 3);
    TEST_EQUAL(str_find_offset("ABC"s, 4, Length::characters), npos);
    TEST_EQUAL(str_find_offset("ABC"s, 0, Length::graphemes), 0);
    TEST_EQUAL(str_find_offset("ABC"s, 1, Length::graphemes), 1);
    TEST_EQUAL(str_find_offset("ABC"s, 2, Length::graphemes), 2);
    TEST_EQUAL(str_find_offset("ABC"s, 3, Length::graphemes), 3);
    TEST_EQUAL(str_find_offset("ABC"s, 4, Length::graphemes), npos);
    TEST_EQUAL(str_find_offset("ABC"s, 0, Length::narrow), 0);
    TEST_EQUAL(str_find_offset("ABC"s, 1, Length::narrow), 1);
    TEST_EQUAL(str_find_offset("ABC"s, 2, Length::narrow), 2);
    TEST_EQUAL(str_find_offset("ABC"s, 3, Length::narrow), 3);
    TEST_EQUAL(str_find_offset("ABC"s, 4, Length::narrow), npos);
    TEST_EQUAL(str_find_offset("ABC"s, 0, Length::wide), 0);
    TEST_EQUAL(str_find_offset("ABC"s, 1, Length::wide), 1);
    TEST_EQUAL(str_find_offset("ABC"s, 2, Length::wide), 2);
    TEST_EQUAL(str_find_offset("ABC"s, 3, Length::wide), 3);
    TEST_EQUAL(str_find_offset("ABC"s, 4, Length::wide), npos);

    TEST_EQUAL(str_find_offset(utf8_example, 0, Length::characters), 0);
    TEST_EQUAL(str_find_offset(utf8_example, 1, Length::characters), 2);
    TEST_EQUAL(str_find_offset(utf8_example, 2, Length::characters), 5);
    TEST_EQUAL(str_find_offset(utf8_example, 3, Length::characters), 9);
    TEST_EQUAL(str_find_offset(utf8_example, 4, Length::characters), 13);
    TEST_EQUAL(str_find_offset(utf8_example, 5, Length::characters), npos);
    TEST_EQUAL(str_find_offset(utf8_example, 0, Length::graphemes), 0);
    TEST_EQUAL(str_find_offset(utf8_example, 1, Length::graphemes), 2);
    TEST_EQUAL(str_find_offset(utf8_example, 2, Length::graphemes), 5);
    TEST_EQUAL(str_find_offset(utf8_example, 3, Length::graphemes), 9);
    TEST_EQUAL(str_find_offset(utf8_example, 4, Length::graphemes), 13);
    TEST_EQUAL(str_find_offset(utf8_example, 5, Length::graphemes), npos);

    TEST_EQUAL(str_find_offset("áéíóú"s, 0, Length::graphemes), 0);                                   // precomposed
    TEST_EQUAL(str_find_offset("áéíóú"s, 1, Length::graphemes), 2);                                   // precomposed
    TEST_EQUAL(str_find_offset("áéíóú"s, 2, Length::graphemes), 4);                                   // precomposed
    TEST_EQUAL(str_find_offset("áéíóú"s, 3, Length::graphemes), 6);                                   // precomposed
    TEST_EQUAL(str_find_offset("áéíóú"s, 4, Length::graphemes), 8);                                   // precomposed
    TEST_EQUAL(str_find_offset("áéíóú"s, 5, Length::graphemes), 10);                                  // precomposed
    TEST_EQUAL(str_find_offset("áéíóú"s, 6, Length::graphemes), npos);                                // precomposed
    TEST_EQUAL(str_find_offset("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 0, Length::graphemes), 0);     // decomposed
    TEST_EQUAL(str_find_offset("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, Length::graphemes), 3);     // decomposed
    TEST_EQUAL(str_find_offset("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 2, Length::graphemes), 6);     // decomposed
    TEST_EQUAL(str_find_offset("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 3, Length::graphemes), 9);     // decomposed
    TEST_EQUAL(str_find_offset("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 4, Length::graphemes), 12);    // decomposed
    TEST_EQUAL(str_find_offset("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, Length::graphemes), 15);    // decomposed
    TEST_EQUAL(str_find_offset("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 6, Length::graphemes), npos);  // decomposed

    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 0, Length::narrow), 0);     // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 1, Length::narrow), 3);     // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 2, Length::narrow), 3);     // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 3, Length::narrow), 6);     // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 4, Length::narrow), 6);     // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 5, Length::narrow), 9);     // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 6, Length::narrow), 9);     // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 7, Length::narrow), npos);  // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 0, Length::wide), 0);       // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 1, Length::wide), 3);       // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 2, Length::wide), 3);       // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 3, Length::wide), 6);       // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 4, Length::wide), 6);       // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 5, Length::wide), 9);       // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 6, Length::wide), 9);       // fullwidth
    TEST_EQUAL(str_find_offset("\u3000\u3000\u3000"s, 7, Length::wide), npos);    // fullwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 0, Length::narrow), 0);     // halfwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 1, Length::narrow), 3);     // halfwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 2, Length::narrow), 6);     // halfwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 3, Length::narrow), 9);     // halfwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 4, Length::narrow), npos);  // halfwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 0, Length::wide), 0);       // halfwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 1, Length::wide), 3);       // halfwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 2, Length::wide), 6);       // halfwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 3, Length::wide), 9);       // halfwidth
    TEST_EQUAL(str_find_offset("\u20a9\u20a9\u20a9"s, 4, Length::wide), npos);    // halfwidth

    TEST_EQUAL(str_find_offset(u"ABC"s, 0, Length::characters), 0);
    TEST_EQUAL(str_find_offset(u"ABC"s, 1, Length::characters), 1);
    TEST_EQUAL(str_find_offset(u"ABC"s, 2, Length::characters), 2);
    TEST_EQUAL(str_find_offset(u"ABC"s, 3, Length::characters), 3);
    TEST_EQUAL(str_find_offset(u"ABC"s, 4, Length::characters), npos);
    TEST_EQUAL(str_find_offset(u"ABC"s, 0, Length::graphemes), 0);
    TEST_EQUAL(str_find_offset(u"ABC"s, 1, Length::graphemes), 1);
    TEST_EQUAL(str_find_offset(u"ABC"s, 2, Length::graphemes), 2);
    TEST_EQUAL(str_find_offset(u"ABC"s, 3, Length::graphemes), 3);
    TEST_EQUAL(str_find_offset(u"ABC"s, 4, Length::graphemes), npos);
    TEST_EQUAL(str_find_offset(u"ABC"s, 0, Length::narrow), 0);
    TEST_EQUAL(str_find_offset(u"ABC"s, 1, Length::narrow), 1);
    TEST_EQUAL(str_find_offset(u"ABC"s, 2, Length::narrow), 2);
    TEST_EQUAL(str_find_offset(u"ABC"s, 3, Length::narrow), 3);
    TEST_EQUAL(str_find_offset(u"ABC"s, 4, Length::narrow), npos);
    TEST_EQUAL(str_find_offset(u"ABC"s, 0, Length::wide), 0);
    TEST_EQUAL(str_find_offset(u"ABC"s, 1, Length::wide), 1);
    TEST_EQUAL(str_find_offset(u"ABC"s, 2, Length::wide), 2);
    TEST_EQUAL(str_find_offset(u"ABC"s, 3, Length::wide), 3);
    TEST_EQUAL(str_find_offset(u"ABC"s, 4, Length::wide), npos);

    TEST_EQUAL(str_find_offset(utf16_example, 0, Length::characters), 0);
    TEST_EQUAL(str_find_offset(utf16_example, 1, Length::characters), 1);
    TEST_EQUAL(str_find_offset(utf16_example, 2, Length::characters), 2);
    TEST_EQUAL(str_find_offset(utf16_example, 3, Length::characters), 4);
    TEST_EQUAL(str_find_offset(utf16_example, 4, Length::characters), 6);
    TEST_EQUAL(str_find_offset(utf16_example, 5, Length::characters), npos);
    TEST_EQUAL(str_find_offset(utf16_example, 0, Length::graphemes), 0);
    TEST_EQUAL(str_find_offset(utf16_example, 1, Length::graphemes), 1);
    TEST_EQUAL(str_find_offset(utf16_example, 2, Length::graphemes), 2);
    TEST_EQUAL(str_find_offset(utf16_example, 3, Length::graphemes), 4);
    TEST_EQUAL(str_find_offset(utf16_example, 4, Length::graphemes), 6);
    TEST_EQUAL(str_find_offset(utf16_example, 5, Length::graphemes), npos);

    TEST_EQUAL(str_find_offset(u"áéíóú"s, 0, Length::graphemes), 0);                                   // precomposed
    TEST_EQUAL(str_find_offset(u"áéíóú"s, 1, Length::graphemes), 1);                                   // precomposed
    TEST_EQUAL(str_find_offset(u"áéíóú"s, 2, Length::graphemes), 2);                                   // precomposed
    TEST_EQUAL(str_find_offset(u"áéíóú"s, 3, Length::graphemes), 3);                                   // precomposed
    TEST_EQUAL(str_find_offset(u"áéíóú"s, 4, Length::graphemes), 4);                                   // precomposed
    TEST_EQUAL(str_find_offset(u"áéíóú"s, 5, Length::graphemes), 5);                                   // precomposed
    TEST_EQUAL(str_find_offset(u"áéíóú"s, 6, Length::graphemes), npos);                                // precomposed
    TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 0, Length::graphemes), 0);     // decomposed
    TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, Length::graphemes), 2);     // decomposed
    TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 2, Length::graphemes), 4);     // decomposed
    TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 3, Length::graphemes), 6);     // decomposed
    TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 4, Length::graphemes), 8);     // decomposed
    TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, Length::graphemes), 10);    // decomposed
    TEST_EQUAL(str_find_offset(u"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 6, Length::graphemes), npos);  // decomposed

    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 0, Length::narrow), 0);     // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 1, Length::narrow), 1);     // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 2, Length::narrow), 1);     // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 3, Length::narrow), 2);     // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 4, Length::narrow), 2);     // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 5, Length::narrow), 3);     // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 6, Length::narrow), 3);     // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 7, Length::narrow), npos);  // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 0, Length::wide), 0);       // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 1, Length::wide), 1);       // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 2, Length::wide), 1);       // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 3, Length::wide), 2);       // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 4, Length::wide), 2);       // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 5, Length::wide), 3);       // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 6, Length::wide), 3);       // fullwidth
    TEST_EQUAL(str_find_offset(u"\u3000\u3000\u3000"s, 7, Length::wide), npos);    // fullwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 0, Length::narrow), 0);     // halfwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 1, Length::narrow), 1);     // halfwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 2, Length::narrow), 2);     // halfwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 3, Length::narrow), 3);     // halfwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 4, Length::narrow), npos);  // halfwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 0, Length::wide), 0);       // halfwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 1, Length::wide), 1);       // halfwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 2, Length::wide), 2);       // halfwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 3, Length::wide), 3);       // halfwidth
    TEST_EQUAL(str_find_offset(u"\u20a9\u20a9\u20a9"s, 4, Length::wide), npos);    // halfwidth

    TEST_EQUAL(str_find_offset(U"ABC"s, 0, Length::characters), 0);
    TEST_EQUAL(str_find_offset(U"ABC"s, 1, Length::characters), 1);
    TEST_EQUAL(str_find_offset(U"ABC"s, 2, Length::characters), 2);
    TEST_EQUAL(str_find_offset(U"ABC"s, 3, Length::characters), 3);
    TEST_EQUAL(str_find_offset(U"ABC"s, 4, Length::characters), npos);
    TEST_EQUAL(str_find_offset(U"ABC"s, 0, Length::graphemes), 0);
    TEST_EQUAL(str_find_offset(U"ABC"s, 1, Length::graphemes), 1);
    TEST_EQUAL(str_find_offset(U"ABC"s, 2, Length::graphemes), 2);
    TEST_EQUAL(str_find_offset(U"ABC"s, 3, Length::graphemes), 3);
    TEST_EQUAL(str_find_offset(U"ABC"s, 4, Length::graphemes), npos);
    TEST_EQUAL(str_find_offset(U"ABC"s, 0, Length::narrow), 0);
    TEST_EQUAL(str_find_offset(U"ABC"s, 1, Length::narrow), 1);
    TEST_EQUAL(str_find_offset(U"ABC"s, 2, Length::narrow), 2);
    TEST_EQUAL(str_find_offset(U"ABC"s, 3, Length::narrow), 3);
    TEST_EQUAL(str_find_offset(U"ABC"s, 4, Length::narrow), npos);
    TEST_EQUAL(str_find_offset(U"ABC"s, 0, Length::wide), 0);
    TEST_EQUAL(str_find_offset(U"ABC"s, 1, Length::wide), 1);
    TEST_EQUAL(str_find_offset(U"ABC"s, 2, Length::wide), 2);
    TEST_EQUAL(str_find_offset(U"ABC"s, 3, Length::wide), 3);
    TEST_EQUAL(str_find_offset(U"ABC"s, 4, Length::wide), npos);

    TEST_EQUAL(str_find_offset(utf32_example, 0, Length::characters), 0);
    TEST_EQUAL(str_find_offset(utf32_example, 1, Length::characters), 1);
    TEST_EQUAL(str_find_offset(utf32_example, 2, Length::characters), 2);
    TEST_EQUAL(str_find_offset(utf32_example, 3, Length::characters), 3);
    TEST_EQUAL(str_find_offset(utf32_example, 4, Length::characters), 4);
    TEST_EQUAL(str_find_offset(utf32_example, 5, Length::characters), npos);
    TEST_EQUAL(str_find_offset(utf32_example, 0, Length::graphemes), 0);
    TEST_EQUAL(str_find_offset(utf32_example, 1, Length::graphemes), 1);
    TEST_EQUAL(str_find_offset(utf32_example, 2, Length::graphemes), 2);
    TEST_EQUAL(str_find_offset(utf32_example, 3, Length::graphemes), 3);
    TEST_EQUAL(str_find_offset(utf32_example, 4, Length::graphemes), 4);
    TEST_EQUAL(str_find_offset(utf32_example, 5, Length::graphemes), npos);

    TEST_EQUAL(str_find_offset(U"áéíóú"s, 0, Length::graphemes), 0);                                   // precomposed
    TEST_EQUAL(str_find_offset(U"áéíóú"s, 1, Length::graphemes), 1);                                   // precomposed
    TEST_EQUAL(str_find_offset(U"áéíóú"s, 2, Length::graphemes), 2);                                   // precomposed
    TEST_EQUAL(str_find_offset(U"áéíóú"s, 3, Length::graphemes), 3);                                   // precomposed
    TEST_EQUAL(str_find_offset(U"áéíóú"s, 4, Length::graphemes), 4);                                   // precomposed
    TEST_EQUAL(str_find_offset(U"áéíóú"s, 5, Length::graphemes), 5);                                   // precomposed
    TEST_EQUAL(str_find_offset(U"áéíóú"s, 6, Length::graphemes), npos);                                // precomposed
    TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 0, Length::graphemes), 0);     // decomposed
    TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, Length::graphemes), 2);     // decomposed
    TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 2, Length::graphemes), 4);     // decomposed
    TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 3, Length::graphemes), 6);     // decomposed
    TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 4, Length::graphemes), 8);     // decomposed
    TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, Length::graphemes), 10);    // decomposed
    TEST_EQUAL(str_find_offset(U"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 6, Length::graphemes), npos);  // decomposed

    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 0, Length::narrow), 0);     // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 1, Length::narrow), 1);     // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 2, Length::narrow), 1);     // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 3, Length::narrow), 2);     // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 4, Length::narrow), 2);     // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 5, Length::narrow), 3);     // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 6, Length::narrow), 3);     // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 7, Length::narrow), npos);  // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 0, Length::wide), 0);       // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 1, Length::wide), 1);       // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 2, Length::wide), 1);       // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 3, Length::wide), 2);       // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 4, Length::wide), 2);       // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 5, Length::wide), 3);       // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 6, Length::wide), 3);       // fullwidth
    TEST_EQUAL(str_find_offset(U"\u3000\u3000\u3000"s, 7, Length::wide), npos);    // fullwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 0, Length::narrow), 0);     // halfwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 1, Length::narrow), 1);     // halfwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 2, Length::narrow), 2);     // halfwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 3, Length::narrow), 3);     // halfwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 4, Length::narrow), npos);  // halfwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 0, Length::wide), 0);       // halfwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 1, Length::wide), 1);       // halfwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 2, Length::wide), 2);       // halfwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 3, Length::wide), 3);       // halfwidth
    TEST_EQUAL(str_find_offset(U"\u20a9\u20a9\u20a9"s, 4, Length::wide), npos);    // halfwidth

}
