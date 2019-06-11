#include "unicorn/character.hpp"
#include "unicorn/ucd-tables.hpp"
#include "unicorn/unit-test.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

#define TEST_PROPERTY_STRING(type, value) TEST_EQUAL(to_str(type::value), #type "::" #value)

void test_unicorn_character_version_information() {

    auto v1 = unicorn_version(), v2 = unicode_version();
    std::cout << "... Unicorn version: " << v1.str(3) << "\n";
    std::cout << "... Unicode version: " << v2.str(3) << "\n";
    TEST_COMPARE(v1, >=, (Version{0,1,0}));
    TEST_COMPARE(v2, >=, (Version{8,0,0}));

}

void test_unicorn_character_basic_functions() {

    TEST_EQUAL(char_as_hex(0), "U+0000");
    TEST_EQUAL(char_as_hex(U'A'), "U+0041");
    TEST_EQUAL(char_as_hex(0x123), "U+0123");
    TEST_EQUAL(char_as_hex(0x4567), "U+4567");
    TEST_EQUAL(char_as_hex(0x89abc), "U+89ABC");
    TEST_EQUAL(char_as_hex(0x100def), "U+100DEF");

    TEST(! char_is_digit(0));
    TEST(char_is_digit(U'0'));
    TEST(char_is_digit(U'9'));
    TEST(! char_is_digit(U'A'));
    TEST(! char_is_digit(U'F'));
    TEST(! char_is_digit(U'G'));
    TEST(! char_is_digit(U'Z'));
    TEST(! char_is_digit(U'a'));
    TEST(! char_is_digit(U'f'));
    TEST(! char_is_digit(U'g'));
    TEST(! char_is_digit(U'z'));
    TEST(! char_is_digit(U'!'));
    TEST(! char_is_digit(U'~'));
    TEST(! char_is_digit(0xff));
    TEST(! char_is_digit(0x100));

    TEST(! char_is_xdigit(0));
    TEST(char_is_xdigit(U'0'));
    TEST(char_is_xdigit(U'9'));
    TEST(char_is_xdigit(U'A'));
    TEST(char_is_xdigit(U'F'));
    TEST(! char_is_xdigit(U'G'));
    TEST(! char_is_xdigit(U'Z'));
    TEST(char_is_xdigit(U'a'));
    TEST(char_is_xdigit(U'f'));
    TEST(! char_is_xdigit(U'g'));
    TEST(! char_is_xdigit(U'z'));
    TEST(! char_is_xdigit(U'!'));
    TEST(! char_is_xdigit(U'~'));
    TEST(! char_is_xdigit(0xff));
    TEST(! char_is_xdigit(0x100));

    TEST(char_is_ascii(0));
    TEST(char_is_ascii(U'\t'));
    TEST(char_is_ascii(U'\n'));
    TEST(char_is_ascii(0x1f));
    TEST(char_is_ascii(U' '));
    TEST(char_is_ascii(U'!'));
    TEST(char_is_ascii(U'/'));
    TEST(char_is_ascii(U'0'));
    TEST(char_is_ascii(U'9'));
    TEST(char_is_ascii(U':'));
    TEST(char_is_ascii(U'@'));
    TEST(char_is_ascii(U'A'));
    TEST(char_is_ascii(U'Z'));
    TEST(char_is_ascii(U'['));
    TEST(char_is_ascii(U'`'));
    TEST(char_is_ascii(U'a'));
    TEST(char_is_ascii(U'z'));
    TEST(char_is_ascii(U'{'));
    TEST(char_is_ascii(U'~'));
    TEST(char_is_ascii(0x7f));
    TEST(! char_is_ascii(0x80));

    TEST(char_is_latin1(0));
    TEST(char_is_latin1(U'A'));
    TEST(char_is_latin1(0xff));
    TEST(! char_is_latin1(0x100));

    TEST(char_is_bmp(0));
    TEST(char_is_bmp(U'A'));
    TEST(! char_is_bmp(0xd800));
    TEST(char_is_bmp(0xffff));
    TEST(! char_is_bmp(0x10000));
    TEST(! char_is_bmp(0x10ffff));
    TEST(! char_is_bmp(0x110000));

    TEST(! char_is_astral(0));
    TEST(! char_is_astral(U'A'));
    TEST(! char_is_astral(0xd800));
    TEST(! char_is_astral(0xffff));
    TEST(char_is_astral(0x10000));
    TEST(char_is_astral(0x10ffff));
    TEST(! char_is_astral(0x110000));

    TEST(char_is_unicode(0));
    TEST(char_is_unicode(U'A'));
    TEST(char_is_unicode(0xd7ff));
    TEST(! char_is_unicode(0xd800));
    TEST(! char_is_unicode(0xdfff));
    TEST(char_is_unicode(0xe000));
    TEST(char_is_unicode(0x10ffff));
    TEST(! char_is_unicode(0x110000));
    TEST(! char_is_unicode(0xffffffff));

    TEST(! char_is_surrogate(0xd7ff));
    TEST(char_is_surrogate(0xd800));
    TEST(char_is_surrogate(0xdbff));
    TEST(char_is_surrogate(0xdc00));
    TEST(char_is_surrogate(0xdfff));
    TEST(! char_is_surrogate(0xe000));

    TEST(! char_is_high_surrogate(0xd7ff));
    TEST(char_is_high_surrogate(0xd800));
    TEST(char_is_high_surrogate(0xdbff));
    TEST(! char_is_high_surrogate(0xdc00));
    TEST(! char_is_high_surrogate(0xdfff));
    TEST(! char_is_high_surrogate(0xe000));

    TEST(! char_is_low_surrogate(0xd7ff));
    TEST(! char_is_low_surrogate(0xd800));
    TEST(! char_is_low_surrogate(0xdbff));
    TEST(char_is_low_surrogate(0xdc00));
    TEST(char_is_low_surrogate(0xdfff));
    TEST(! char_is_low_surrogate(0xe000));

    TEST(! char_is_private_use(0xdfff));
    TEST(char_is_private_use(0xe000));
    TEST(char_is_private_use(0xf8ff));
    TEST(! char_is_private_use(0xf900));
    TEST(! char_is_private_use(0xeffff));
    TEST(char_is_private_use(0xf0000));
    TEST(char_is_private_use(0x10fffd));
    TEST(! char_is_private_use(0x10fffe));

    TEST(! char_is_noncharacter(0));
    TEST(! char_is_noncharacter(U'A'));
    TEST(! char_is_noncharacter(0xfdcf));
    TEST(char_is_noncharacter(0xfdd0));
    TEST(char_is_noncharacter(0xfdef));
    TEST(! char_is_noncharacter(0xfdf0));
    TEST(! char_is_noncharacter(0xfffd));
    TEST(char_is_noncharacter(0xfffe));
    TEST(char_is_noncharacter(0xffff));
    TEST(! char_is_noncharacter(0x10000));
    TEST(! char_is_noncharacter(0x1fffd));
    TEST(char_is_noncharacter(0x1fffe));
    TEST(char_is_noncharacter(0x1ffff));
    TEST(! char_is_noncharacter(0x20000));
    TEST(! char_is_noncharacter(0x10fffd));
    TEST(char_is_noncharacter(0x10fffe));
    TEST(char_is_noncharacter(0x10ffff));

    TEST_EQUAL(char_to_uint('\0'), 0);
    TEST_EQUAL(char_to_uint('A'), 65);
    TEST_EQUAL(char_to_uint('\xff'), 0xff);
    TEST_EQUAL(char_to_uint(u'\0'), 0);
    TEST_EQUAL(char_to_uint(u'A'), 65);
    TEST_EQUAL(char_to_uint(u'\xff'), 0xff);
    TEST_EQUAL(char_to_uint(u'\uf000'), 0xf000);
    TEST_EQUAL(char_to_uint(U'\0'), 0);
    TEST_EQUAL(char_to_uint(U'A'), 65);
    TEST_EQUAL(char_to_uint(U'\xff'), 0xff);
    TEST_EQUAL(char_to_uint(U'\uf000'), 0xf000);
    TEST_EQUAL(char_to_uint(U'\U0010f000'), 0x10f000);
    TEST_EQUAL(char_to_uint(L'\0'), 0);
    TEST_EQUAL(char_to_uint(L'A'), 65);
    TEST_EQUAL(char_to_uint(L'\xff'), 0xff);
    TEST_EQUAL(char_to_uint(L'\uf000'), 0xf000);

    TEST(is_character_type<char>);
    TEST(is_character_type<char16_t>);
    TEST(is_character_type<char32_t>);
    TEST(is_character_type<wchar_t>);
    TEST(! is_character_type<signed char>);
    TEST(! is_character_type<unsigned char>);
    TEST(! is_character_type<void>);
    TEST(! is_character_type<int>);
    TEST(! is_character_type<std::string>);

}

void test_unicorn_character_general_category() {

    TEST_COMPARE(GC::Cc, ==, GC::Cc);
    TEST_COMPARE(GC::Cc, <=, GC::Cc);
    TEST_COMPARE(GC::Cc, >=, GC::Cc);
    TEST_COMPARE(GC::Cc, !=, GC::Zs);
    TEST_COMPARE(GC::Cc, <, GC::Zs);
    TEST_COMPARE(GC::Cc, <=, GC::Zs);
    TEST_COMPARE(GC::Zs, !=, GC::Cc);
    TEST_COMPARE(GC::Zs, >, GC::Cc);
    TEST_COMPARE(GC::Zs, >=, GC::Cc);
    TEST_COMPARE(GC::Zs, ==, GC::Zs);
    TEST_COMPARE(GC::Zs, <=, GC::Zs);
    TEST_COMPARE(GC::Zs, >=, GC::Zs);

    TEST_EQUAL(decode_gc(GC(0x436e)), "Cn");
    TEST_EQUAL(decode_gc(GC(0x4c75)), "Lu");

    TEST_EQUAL(encode_gc('C', 'n'), GC(0x436e));
    TEST_EQUAL(encode_gc('L', 'u'), GC(0x4c75));
    TEST_EQUAL(encode_gc(""), GC(0));
    TEST_EQUAL(encode_gc("Cn"), GC(0x436e));
    TEST_EQUAL(encode_gc("Lu"), GC(0x4c75));
    TEST_EQUAL(encode_gc(""s), GC(0));
    TEST_EQUAL(encode_gc("Cn"s), GC(0x436e));
    TEST_EQUAL(encode_gc("Lu"s), GC(0x4c75));

    TEST_EQUAL(to_str(GC::Cc), "Cc");
    TEST_EQUAL(to_str(GC::Cf), "Cf");
    TEST_EQUAL(to_str(GC::Cn), "Cn");
    TEST_EQUAL(to_str(GC::Co), "Co");
    TEST_EQUAL(to_str(GC::Cs), "Cs");
    TEST_EQUAL(to_str(GC::Ll), "Ll");
    TEST_EQUAL(to_str(GC::Lm), "Lm");
    TEST_EQUAL(to_str(GC::Lo), "Lo");
    TEST_EQUAL(to_str(GC::Lt), "Lt");
    TEST_EQUAL(to_str(GC::Lu), "Lu");
    TEST_EQUAL(to_str(GC::Mc), "Mc");
    TEST_EQUAL(to_str(GC::Me), "Me");
    TEST_EQUAL(to_str(GC::Mn), "Mn");
    TEST_EQUAL(to_str(GC::Nd), "Nd");
    TEST_EQUAL(to_str(GC::Nl), "Nl");
    TEST_EQUAL(to_str(GC::No), "No");
    TEST_EQUAL(to_str(GC::Pc), "Pc");
    TEST_EQUAL(to_str(GC::Pd), "Pd");
    TEST_EQUAL(to_str(GC::Pe), "Pe");
    TEST_EQUAL(to_str(GC::Pf), "Pf");
    TEST_EQUAL(to_str(GC::Pi), "Pi");
    TEST_EQUAL(to_str(GC::Po), "Po");
    TEST_EQUAL(to_str(GC::Ps), "Ps");
    TEST_EQUAL(to_str(GC::Sc), "Sc");
    TEST_EQUAL(to_str(GC::Sk), "Sk");
    TEST_EQUAL(to_str(GC::Sm), "Sm");
    TEST_EQUAL(to_str(GC::So), "So");
    TEST_EQUAL(to_str(GC::Zl), "Zl");
    TEST_EQUAL(to_str(GC::Zp), "Zp");
    TEST_EQUAL(to_str(GC::Zs), "Zs");

    TEST_EQUAL(Ustring(gc_name(GC::Cc)), "control"s);
    TEST_EQUAL(Ustring(gc_name(GC::Cf)), "format"s);
    TEST_EQUAL(Ustring(gc_name(GC::Cn)), "unassigned"s);
    TEST_EQUAL(Ustring(gc_name(GC::Co)), "private use"s);
    TEST_EQUAL(Ustring(gc_name(GC::Cs)), "surrogate"s);
    TEST_EQUAL(Ustring(gc_name(GC::Ll)), "lowercase letter"s);
    TEST_EQUAL(Ustring(gc_name(GC::Lm)), "modifier letter"s);
    TEST_EQUAL(Ustring(gc_name(GC::Lo)), "other letter"s);
    TEST_EQUAL(Ustring(gc_name(GC::Lt)), "titlecase letter"s);
    TEST_EQUAL(Ustring(gc_name(GC::Lu)), "uppercase letter"s);
    TEST_EQUAL(Ustring(gc_name(GC::Mc)), "spacing mark"s);
    TEST_EQUAL(Ustring(gc_name(GC::Me)), "enclosing mark"s);
    TEST_EQUAL(Ustring(gc_name(GC::Mn)), "nonspacing mark"s);
    TEST_EQUAL(Ustring(gc_name(GC::Nd)), "decimal number"s);
    TEST_EQUAL(Ustring(gc_name(GC::Nl)), "letter number"s);
    TEST_EQUAL(Ustring(gc_name(GC::No)), "other number"s);
    TEST_EQUAL(Ustring(gc_name(GC::Pc)), "connector punctuation"s);
    TEST_EQUAL(Ustring(gc_name(GC::Pd)), "dash punctuation"s);
    TEST_EQUAL(Ustring(gc_name(GC::Pe)), "close punctuation"s);
    TEST_EQUAL(Ustring(gc_name(GC::Pf)), "final punctuation"s);
    TEST_EQUAL(Ustring(gc_name(GC::Pi)), "initial punctuation"s);
    TEST_EQUAL(Ustring(gc_name(GC::Po)), "other punctuation"s);
    TEST_EQUAL(Ustring(gc_name(GC::Ps)), "open punctuation"s);
    TEST_EQUAL(Ustring(gc_name(GC::Sc)), "currency symbol"s);
    TEST_EQUAL(Ustring(gc_name(GC::Sk)), "modifier symbol"s);
    TEST_EQUAL(Ustring(gc_name(GC::Sm)), "math symbol"s);
    TEST_EQUAL(Ustring(gc_name(GC::So)), "other symbol"s);
    TEST_EQUAL(Ustring(gc_name(GC::Zl)), "line separator"s);
    TEST_EQUAL(Ustring(gc_name(GC::Zp)), "paragraph separator"s);
    TEST_EQUAL(Ustring(gc_name(GC::Zs)), "space separator"s);

    TEST_EQUAL(char_general_category(0), encode_gc("Cc"));
    TEST_EQUAL(char_general_category(0x9), encode_gc("Cc"));
    TEST_EQUAL(char_general_category(0xad), encode_gc("Cf"));
    TEST_EQUAL(char_general_category(0x180e), encode_gc("Cf"));
    TEST_EQUAL(char_general_category(0x378), encode_gc("Cn"));
    TEST_EQUAL(char_general_category(0x110000), encode_gc("Cn"));
    TEST_EQUAL(char_general_category(0xe000), encode_gc("Co"));
    TEST_EQUAL(char_general_category(0x10fffd), encode_gc("Co"));
    TEST_EQUAL(char_general_category(0xd800), encode_gc("Cs"));
    TEST_EQUAL(char_general_category(0xdfff), encode_gc("Cs"));
    TEST_EQUAL(char_general_category(0x61), encode_gc("Ll"));
    TEST_EQUAL(char_general_category(0x6a), encode_gc("Ll"));
    TEST_EQUAL(char_general_category(0x2b0), encode_gc("Lm"));
    TEST_EQUAL(char_general_category(0x2b9), encode_gc("Lm"));
    TEST_EQUAL(char_general_category(0xaa), encode_gc("Lo"));
    TEST_EQUAL(char_general_category(0x5d1), encode_gc("Lo"));
    TEST_EQUAL(char_general_category(0x1c5), encode_gc("Lt"));
    TEST_EQUAL(char_general_category(0x1f8d), encode_gc("Lt"));
    TEST_EQUAL(char_general_category(0x41), encode_gc("Lu"));
    TEST_EQUAL(char_general_category(0x4a), encode_gc("Lu"));
    TEST_EQUAL(char_general_category(0x903), encode_gc("Mc"));
    TEST_EQUAL(char_general_category(0x94e), encode_gc("Mc"));
    TEST_EQUAL(char_general_category(0x488), encode_gc("Me"));
    TEST_EQUAL(char_general_category(0xa670), encode_gc("Me"));
    TEST_EQUAL(char_general_category(0x300), encode_gc("Mn"));
    TEST_EQUAL(char_general_category(0x309), encode_gc("Mn"));
    TEST_EQUAL(char_general_category(0x30), encode_gc("Nd"));
    TEST_EQUAL(char_general_category(0x39), encode_gc("Nd"));
    TEST_EQUAL(char_general_category(0x16ee), encode_gc("Nl"));
    TEST_EQUAL(char_general_category(0x2166), encode_gc("Nl"));
    TEST_EQUAL(char_general_category(0xb2), encode_gc("No"));
    TEST_EQUAL(char_general_category(0x9f7), encode_gc("No"));
    TEST_EQUAL(char_general_category(0x5f), encode_gc("Pc"));
    TEST_EQUAL(char_general_category(0xff3f), encode_gc("Pc"));
    TEST_EQUAL(char_general_category(0x2d), encode_gc("Pd"));
    TEST_EQUAL(char_general_category(0x2014), encode_gc("Pd"));
    TEST_EQUAL(char_general_category(0x29), encode_gc("Pe"));
    TEST_EQUAL(char_general_category(0x2309), encode_gc("Pe"));
    TEST_EQUAL(char_general_category(0xbb), encode_gc("Pf"));
    TEST_EQUAL(char_general_category(0x2e21), encode_gc("Pf"));
    TEST_EQUAL(char_general_category(0xab), encode_gc("Pi"));
    TEST_EQUAL(char_general_category(0x2e0c), encode_gc("Pi"));
    TEST_EQUAL(char_general_category(0x21), encode_gc("Po"));
    TEST_EQUAL(char_general_category(0x2f), encode_gc("Po"));
    TEST_EQUAL(char_general_category(0x28), encode_gc("Ps"));
    TEST_EQUAL(char_general_category(0x207d), encode_gc("Ps"));
    TEST_EQUAL(char_general_category(0x24), encode_gc("Sc"));
    TEST_EQUAL(char_general_category(0x9fb), encode_gc("Sc"));
    TEST_EQUAL(char_general_category(0x5e), encode_gc("Sk"));
    TEST_EQUAL(char_general_category(0x2c5), encode_gc("Sk"));
    TEST_EQUAL(char_general_category(0x2b), encode_gc("Sm"));
    TEST_EQUAL(char_general_category(0xf7), encode_gc("Sm"));
    TEST_EQUAL(char_general_category(0xa6), encode_gc("So"));
    TEST_EQUAL(char_general_category(0x6fd), encode_gc("So"));
    TEST_EQUAL(char_general_category(0x2028), encode_gc("Zl"));
    TEST_EQUAL(char_general_category(0x2029), encode_gc("Zp"));
    TEST_EQUAL(char_general_category(0x20), encode_gc("Zs"));
    TEST_EQUAL(char_general_category(0x2006), encode_gc("Zs"));

    TEST_EQUAL(char_primary_category(0), 'C');
    TEST_EQUAL(char_primary_category(0xad), 'C');
    TEST_EQUAL(char_primary_category(0x378), 'C');
    TEST_EQUAL(char_primary_category(0xe000), 'C');
    TEST_EQUAL(char_primary_category(0xd800), 'C');
    TEST_EQUAL(char_primary_category(0x61), 'L');
    TEST_EQUAL(char_primary_category(0x2b0), 'L');
    TEST_EQUAL(char_primary_category(0xaa), 'L');
    TEST_EQUAL(char_primary_category(0x1c5), 'L');
    TEST_EQUAL(char_primary_category(0x41), 'L');
    TEST_EQUAL(char_primary_category(0x903), 'M');
    TEST_EQUAL(char_primary_category(0x488), 'M');
    TEST_EQUAL(char_primary_category(0x300), 'M');
    TEST_EQUAL(char_primary_category(0x30), 'N');
    TEST_EQUAL(char_primary_category(0x16ee), 'N');
    TEST_EQUAL(char_primary_category(0xb2), 'N');
    TEST_EQUAL(char_primary_category(0x5f), 'P');
    TEST_EQUAL(char_primary_category(0x2d), 'P');
    TEST_EQUAL(char_primary_category(0x29), 'P');
    TEST_EQUAL(char_primary_category(0xbb), 'P');
    TEST_EQUAL(char_primary_category(0xab), 'P');
    TEST_EQUAL(char_primary_category(0x21), 'P');
    TEST_EQUAL(char_primary_category(0x28), 'P');
    TEST_EQUAL(char_primary_category(0x24), 'S');
    TEST_EQUAL(char_primary_category(0x5e), 'S');
    TEST_EQUAL(char_primary_category(0x2b), 'S');
    TEST_EQUAL(char_primary_category(0xa6), 'S');
    TEST_EQUAL(char_primary_category(0x2028), 'Z');
    TEST_EQUAL(char_primary_category(0x2029), 'Z');
    TEST_EQUAL(char_primary_category(0x20), 'Z');

    TEST(gc_predicate("Cc")(0));
    TEST(gc_predicate("Cf")(0xad));
    TEST(gc_predicate("Cn")(0x378));
    TEST(gc_predicate("Co")(0xe000));
    TEST(gc_predicate("Cs")(0xd800));
    TEST(gc_predicate("Ll")(0x61));
    TEST(gc_predicate("Lm")(0x2b0));
    TEST(gc_predicate("Lo")(0xaa));
    TEST(gc_predicate("Lt")(0x1c5));
    TEST(gc_predicate("Lu")(0x41));
    TEST(gc_predicate("Mc")(0x903));
    TEST(gc_predicate("Me")(0x488));
    TEST(gc_predicate("Mn")(0x300));
    TEST(gc_predicate("Nd")(0x30));
    TEST(gc_predicate("Nl")(0x16ee));
    TEST(gc_predicate("No")(0xb2));
    TEST(gc_predicate("Pc")(0x5f));
    TEST(gc_predicate("Pd")(0x2d));
    TEST(gc_predicate("Pe")(0x29));
    TEST(gc_predicate("Pf")(0xbb));
    TEST(gc_predicate("Pi")(0xab));
    TEST(gc_predicate("Po")(0x21));
    TEST(gc_predicate("Ps")(0x28));
    TEST(gc_predicate("Sc")(0x24));
    TEST(gc_predicate("Sk")(0x5e));
    TEST(gc_predicate("Sm")(0x2b));
    TEST(gc_predicate("So")(0xa6));
    TEST(gc_predicate("Zl")(0x2028));
    TEST(gc_predicate("Zp")(0x2029));
    TEST(gc_predicate("Zs")(0x20));

    TEST(! gc_predicate("Cc", false)(0));
    TEST(! gc_predicate("Cf", false)(0xad));
    TEST(! gc_predicate("Cn", false)(0x378));
    TEST(! gc_predicate("Co", false)(0xe000));
    TEST(! gc_predicate("Cs", false)(0xd800));
    TEST(! gc_predicate("Ll", false)(0x61));
    TEST(! gc_predicate("Lm", false)(0x2b0));
    TEST(! gc_predicate("Lo", false)(0xaa));
    TEST(! gc_predicate("Lt", false)(0x1c5));
    TEST(! gc_predicate("Lu", false)(0x41));
    TEST(! gc_predicate("Mc", false)(0x903));
    TEST(! gc_predicate("Me", false)(0x488));
    TEST(! gc_predicate("Mn", false)(0x300));
    TEST(! gc_predicate("Nd", false)(0x30));
    TEST(! gc_predicate("Nl", false)(0x16ee));
    TEST(! gc_predicate("No", false)(0xb2));
    TEST(! gc_predicate("Pc", false)(0x5f));
    TEST(! gc_predicate("Pd", false)(0x2d));
    TEST(! gc_predicate("Pe", false)(0x29));
    TEST(! gc_predicate("Pf", false)(0xbb));
    TEST(! gc_predicate("Pi", false)(0xab));
    TEST(! gc_predicate("Po", false)(0x21));
    TEST(! gc_predicate("Ps", false)(0x28));
    TEST(! gc_predicate("Sc", false)(0x24));
    TEST(! gc_predicate("Sk", false)(0x5e));
    TEST(! gc_predicate("Sm", false)(0x2b));
    TEST(! gc_predicate("So", false)(0xa6));
    TEST(! gc_predicate("Zl", false)(0x2028));
    TEST(! gc_predicate("Zp", false)(0x2029));
    TEST(! gc_predicate("Zs", false)(0x20));

    TEST(gc_predicate(encode_gc("Cc"))(0));
    TEST(gc_predicate(encode_gc("Cf"))(0xad));
    TEST(gc_predicate(encode_gc("Cn"))(0x378));
    TEST(gc_predicate(encode_gc("Co"))(0xe000));
    TEST(gc_predicate(encode_gc("Cs"))(0xd800));
    TEST(gc_predicate(encode_gc("Ll"))(0x61));
    TEST(gc_predicate(encode_gc("Lm"))(0x2b0));
    TEST(gc_predicate(encode_gc("Lo"))(0xaa));
    TEST(gc_predicate(encode_gc("Lt"))(0x1c5));
    TEST(gc_predicate(encode_gc("Lu"))(0x41));
    TEST(gc_predicate(encode_gc("Mc"))(0x903));
    TEST(gc_predicate(encode_gc("Me"))(0x488));
    TEST(gc_predicate(encode_gc("Mn"))(0x300));
    TEST(gc_predicate(encode_gc("Nd"))(0x30));
    TEST(gc_predicate(encode_gc("Nl"))(0x16ee));
    TEST(gc_predicate(encode_gc("No"))(0xb2));
    TEST(gc_predicate(encode_gc("Pc"))(0x5f));
    TEST(gc_predicate(encode_gc("Pd"))(0x2d));
    TEST(gc_predicate(encode_gc("Pe"))(0x29));
    TEST(gc_predicate(encode_gc("Pf"))(0xbb));
    TEST(gc_predicate(encode_gc("Pi"))(0xab));
    TEST(gc_predicate(encode_gc("Po"))(0x21));
    TEST(gc_predicate(encode_gc("Ps"))(0x28));
    TEST(gc_predicate(encode_gc("Sc"))(0x24));
    TEST(gc_predicate(encode_gc("Sk"))(0x5e));
    TEST(gc_predicate(encode_gc("Sm"))(0x2b));
    TEST(gc_predicate(encode_gc("So"))(0xa6));
    TEST(gc_predicate(encode_gc("Zl"))(0x2028));
    TEST(gc_predicate(encode_gc("Zp"))(0x2029));
    TEST(gc_predicate(encode_gc("Zs"))(0x20));

    TEST(gc_predicate("C")(0));
    TEST(gc_predicate("C")(0xad));
    TEST(gc_predicate("C")(0x378));
    TEST(gc_predicate("C")(0xe000));
    TEST(gc_predicate("C")(0xd800));
    TEST(gc_predicate("L")(0x61));
    TEST(gc_predicate("L")(0x2b0));
    TEST(gc_predicate("L")(0xaa));
    TEST(gc_predicate("L")(0x1c5));
    TEST(gc_predicate("L")(0x41));
    TEST(gc_predicate("M")(0x903));
    TEST(gc_predicate("M")(0x488));
    TEST(gc_predicate("M")(0x300));
    TEST(gc_predicate("N")(0x30));
    TEST(gc_predicate("N")(0x16ee));
    TEST(gc_predicate("N")(0xb2));
    TEST(gc_predicate("P")(0x5f));
    TEST(gc_predicate("P")(0x2d));
    TEST(gc_predicate("P")(0x29));
    TEST(gc_predicate("P")(0xbb));
    TEST(gc_predicate("P")(0xab));
    TEST(gc_predicate("P")(0x21));
    TEST(gc_predicate("P")(0x28));
    TEST(gc_predicate("S")(0x24));
    TEST(gc_predicate("S")(0x5e));
    TEST(gc_predicate("S")(0x2b));
    TEST(gc_predicate("S")(0xa6));
    TEST(gc_predicate("Z")(0x2028));
    TEST(gc_predicate("Z")(0x2029));
    TEST(gc_predicate("Z")(0x20));

    TEST(! gc_predicate("Cs")(0));
    TEST(! gc_predicate("Cc")(0xad));
    TEST(! gc_predicate("Cf")(0x378));
    TEST(! gc_predicate("Cn")(0xe000));
    TEST(! gc_predicate("Co")(0xd800));
    TEST(! gc_predicate("Lu")(0x61));
    TEST(! gc_predicate("Ll")(0x2b0));
    TEST(! gc_predicate("Lm")(0xaa));
    TEST(! gc_predicate("Lo")(0x1c5));
    TEST(! gc_predicate("Lt")(0x41));
    TEST(! gc_predicate("Mn")(0x903));
    TEST(! gc_predicate("Mc")(0x488));
    TEST(! gc_predicate("Me")(0x300));
    TEST(! gc_predicate("No")(0x30));
    TEST(! gc_predicate("Nd")(0x16ee));
    TEST(! gc_predicate("Nl")(0xb2));
    TEST(! gc_predicate("Ps")(0x5f));
    TEST(! gc_predicate("Pc")(0x2d));
    TEST(! gc_predicate("Pd")(0x29));
    TEST(! gc_predicate("Pe")(0xbb));
    TEST(! gc_predicate("Pf")(0xab));
    TEST(! gc_predicate("Pi")(0x21));
    TEST(! gc_predicate("Po")(0x28));
    TEST(! gc_predicate("So")(0x24));
    TEST(! gc_predicate("Sc")(0x5e));
    TEST(! gc_predicate("Sk")(0x2b));
    TEST(! gc_predicate("Sm")(0xa6));
    TEST(! gc_predicate("Zs")(0x2028));
    TEST(! gc_predicate("Zl")(0x2029));
    TEST(! gc_predicate("Zp")(0x20));

    TEST(gc_predicate("Cs", false)(0));
    TEST(gc_predicate("Cc", false)(0xad));
    TEST(gc_predicate("Cf", false)(0x378));
    TEST(gc_predicate("Cn", false)(0xe000));
    TEST(gc_predicate("Co", false)(0xd800));
    TEST(gc_predicate("Lu", false)(0x61));
    TEST(gc_predicate("Ll", false)(0x2b0));
    TEST(gc_predicate("Lm", false)(0xaa));
    TEST(gc_predicate("Lo", false)(0x1c5));
    TEST(gc_predicate("Lt", false)(0x41));
    TEST(gc_predicate("Mn", false)(0x903));
    TEST(gc_predicate("Mc", false)(0x488));
    TEST(gc_predicate("Me", false)(0x300));
    TEST(gc_predicate("No", false)(0x30));
    TEST(gc_predicate("Nd", false)(0x16ee));
    TEST(gc_predicate("Nl", false)(0xb2));
    TEST(gc_predicate("Ps", false)(0x5f));
    TEST(gc_predicate("Pc", false)(0x2d));
    TEST(gc_predicate("Pd", false)(0x29));
    TEST(gc_predicate("Pe", false)(0xbb));
    TEST(gc_predicate("Pf", false)(0xab));
    TEST(gc_predicate("Pi", false)(0x21));
    TEST(gc_predicate("Po", false)(0x28));
    TEST(gc_predicate("So", false)(0x24));
    TEST(gc_predicate("Sc", false)(0x5e));
    TEST(gc_predicate("Sk", false)(0x2b));
    TEST(gc_predicate("Sm", false)(0xa6));
    TEST(gc_predicate("Zs", false)(0x2028));
    TEST(gc_predicate("Zl", false)(0x2029));
    TEST(gc_predicate("Zp", false)(0x20));

    TEST(gc_predicate("Ccfnos")(0));
    TEST(gc_predicate("Ccfnos")(0xad));
    TEST(gc_predicate("Ccfnos")(0x378));
    TEST(gc_predicate("Ccfnos")(0xe000));
    TEST(gc_predicate("Ccfnos")(0xd800));
    TEST(gc_predicate("Llmotu")(0x61));
    TEST(gc_predicate("Llmotu")(0x2b0));
    TEST(gc_predicate("Llmotu")(0xaa));
    TEST(gc_predicate("Llmotu")(0x1c5));
    TEST(gc_predicate("Llmotu")(0x41));
    TEST(gc_predicate("Mcen")(0x903));
    TEST(gc_predicate("Mcen")(0x488));
    TEST(gc_predicate("Mcen")(0x300));
    TEST(gc_predicate("Ndlo")(0x30));
    TEST(gc_predicate("Ndlo")(0x16ee));
    TEST(gc_predicate("Ndlo")(0xb2));
    TEST(gc_predicate("Pcdefios")(0x5f));
    TEST(gc_predicate("Pcdefios")(0x2d));
    TEST(gc_predicate("Pcdefios")(0x29));
    TEST(gc_predicate("Pcdefios")(0xbb));
    TEST(gc_predicate("Pcdefios")(0xab));
    TEST(gc_predicate("Pcdefios")(0x21));
    TEST(gc_predicate("Pcdefios")(0x28));
    TEST(gc_predicate("Sckmo")(0x24));
    TEST(gc_predicate("Sckmo")(0x5e));
    TEST(gc_predicate("Sckmo")(0x2b));
    TEST(gc_predicate("Sckmo")(0xa6));
    TEST(gc_predicate("Zlps")(0x2028));
    TEST(gc_predicate("Zlps")(0x2029));
    TEST(gc_predicate("Zlps")(0x20));

    TEST(gc_predicate("Zs,Cc,Cf,Cn,Co,Cs")(0));
    TEST(gc_predicate("Zs,Cc,Cf,Cn,Co,Cs")(0xad));
    TEST(gc_predicate("Zs,Cc,Cf,Cn,Co,Cs")(0x378));
    TEST(gc_predicate("Zs,Cc,Cf,Cn,Co,Cs")(0xe000));
    TEST(gc_predicate("Zs,Cc,Cf,Cn,Co,Cs")(0xd800));
    TEST(gc_predicate("Zs,Ll,Lm,Lo,Lt,Lu")(0x61));
    TEST(gc_predicate("Zs,Ll,Lm,Lo,Lt,Lu")(0x2b0));
    TEST(gc_predicate("Zs,Ll,Lm,Lo,Lt,Lu")(0xaa));
    TEST(gc_predicate("Zs,Ll,Lm,Lo,Lt,Lu")(0x1c5));
    TEST(gc_predicate("Zs,Ll,Lm,Lo,Lt,Lu")(0x41));
    TEST(gc_predicate("Zs,Mc,Me,Mn")(0x903));
    TEST(gc_predicate("Zs,Mc,Me,Mn")(0x488));
    TEST(gc_predicate("Zs,Mc,Me,Mn")(0x300));
    TEST(gc_predicate("Zs,Nd,Nl,No")(0x30));
    TEST(gc_predicate("Zs,Nd,Nl,No")(0x16ee));
    TEST(gc_predicate("Zs,Nd,Nl,No")(0xb2));
    TEST(gc_predicate("Zs,Pc,Pd,Pe,Pf,Pi,Po,Ps")(0x5f));
    TEST(gc_predicate("Zs,Pc,Pd,Pe,Pf,Pi,Po,Ps")(0x2d));
    TEST(gc_predicate("Zs,Pc,Pd,Pe,Pf,Pi,Po,Ps")(0x29));
    TEST(gc_predicate("Zs,Pc,Pd,Pe,Pf,Pi,Po,Ps")(0xbb));
    TEST(gc_predicate("Zs,Pc,Pd,Pe,Pf,Pi,Po,Ps")(0xab));
    TEST(gc_predicate("Zs,Pc,Pd,Pe,Pf,Pi,Po,Ps")(0x21));
    TEST(gc_predicate("Zs,Pc,Pd,Pe,Pf,Pi,Po,Ps")(0x28));
    TEST(gc_predicate("Zs,Sc,Sk,Sm,So")(0x24));
    TEST(gc_predicate("Zs,Sc,Sk,Sm,So")(0x5e));
    TEST(gc_predicate("Zs,Sc,Sk,Sm,So")(0x2b));
    TEST(gc_predicate("Zs,Sc,Sk,Sm,So")(0xa6));
    TEST(gc_predicate("Cn,Zl,Zp,Zs")(0x2028));
    TEST(gc_predicate("Cn,Zl,Zp,Zs")(0x2029));
    TEST(gc_predicate("Cn,Zl,Zp,Zs")(0x20));

    TEST(gc_predicate("Z,C")(0));
    TEST(gc_predicate("Z,C")(0xad));
    TEST(gc_predicate("Z,C")(0x378));
    TEST(gc_predicate("Z,C")(0xe000));
    TEST(gc_predicate("Z,C")(0xd800));
    TEST(gc_predicate("Z,L")(0x61));
    TEST(gc_predicate("Z,L")(0x2b0));
    TEST(gc_predicate("Z,L")(0xaa));
    TEST(gc_predicate("Z,L")(0x1c5));
    TEST(gc_predicate("Z,L")(0x41));
    TEST(gc_predicate("Z,M")(0x903));
    TEST(gc_predicate("Z,M")(0x488));
    TEST(gc_predicate("Z,M")(0x300));
    TEST(gc_predicate("Z,N")(0x30));
    TEST(gc_predicate("Z,N")(0x16ee));
    TEST(gc_predicate("Z,N")(0xb2));
    TEST(gc_predicate("Z,P")(0x5f));
    TEST(gc_predicate("Z,P")(0x2d));
    TEST(gc_predicate("Z,P")(0x29));
    TEST(gc_predicate("Z,P")(0xbb));
    TEST(gc_predicate("Z,P")(0xab));
    TEST(gc_predicate("Z,P")(0x21));
    TEST(gc_predicate("Z,P")(0x28));
    TEST(gc_predicate("Z,S")(0x24));
    TEST(gc_predicate("Z,S")(0x5e));
    TEST(gc_predicate("Z,S")(0x2b));
    TEST(gc_predicate("Z,S")(0xa6));
    TEST(gc_predicate("C,Z")(0x2028));
    TEST(gc_predicate("C,Z")(0x2029));
    TEST(gc_predicate("C,Z")(0x20));

    TEST(gc_predicate("LC")(0x61));
    TEST(gc_predicate("LC")(0x1c5));
    TEST(gc_predicate("LC")(0x41));
    TEST(! gc_predicate("LC")(0));
    TEST(! gc_predicate("LC")(0x30));
    TEST(! gc_predicate("LC")(0x10ffff));

    TEST(gc_predicate("L&")(0x61));
    TEST(gc_predicate("L&")(0x1c5));
    TEST(gc_predicate("L&")(0x41));
    TEST(! gc_predicate("L&")(0));
    TEST(! gc_predicate("L&")(0x30));
    TEST(! gc_predicate("L&")(0x10ffff));

    TEST(gc_predicate("Zs,L&")(0x61));
    TEST(gc_predicate("Zs,L&")(0x1c5));
    TEST(gc_predicate("Zs,L&")(0x41));
    TEST(! gc_predicate("Zs,L&")(0));
    TEST(! gc_predicate("Zs,L&")(0x30));
    TEST(! gc_predicate("Zs,L&")(0x10ffff));

    TEST(char_is_alphanumeric('0'));
    TEST(char_is_alphanumeric('9'));
    TEST(char_is_alphanumeric('A'));
    TEST(char_is_alphanumeric('Z'));
    TEST(char_is_alphanumeric('a'));
    TEST(char_is_alphanumeric('z'));
    TEST(char_is_alphanumeric(0xaa));
    TEST(char_is_alphanumeric(0xb2));
    TEST(char_is_alphanumeric(0x1c5));
    TEST(char_is_alphanumeric(0x2b0));
    TEST(char_is_alphanumeric(0x2b9));
    TEST(char_is_alphanumeric(0x5d1));
    TEST(char_is_alphanumeric(0x9f7));
    TEST(char_is_alphanumeric(0x16ee));
    TEST(char_is_alphanumeric(0x1f8d));
    TEST(char_is_alphanumeric(0x2166));

    TEST(char_is_control(0));
    TEST(char_is_control(9));

    TEST(char_is_format(0xad));
    TEST(char_is_format(0x180e));

    TEST(char_is_letter('A'));
    TEST(char_is_letter('Z'));
    TEST(char_is_letter('a'));
    TEST(char_is_letter('z'));
    TEST(char_is_letter(0xaa));
    TEST(char_is_letter(0x1c5));
    TEST(char_is_letter(0x2b0));
    TEST(char_is_letter(0x2b9));
    TEST(char_is_letter(0x5d1));
    TEST(char_is_letter(0x1f8d));

    TEST(char_is_mark(0x903));
    TEST(char_is_mark(0x94e));
    TEST(char_is_mark(0x488));
    TEST(char_is_mark(0xa670));
    TEST(char_is_mark(0x300));
    TEST(char_is_mark(0x309));

    TEST(char_is_number('0'));
    TEST(char_is_number('9'));
    TEST(char_is_number(0xb2));
    TEST(char_is_number(0x9f7));
    TEST(char_is_number(0x16ee));
    TEST(char_is_number(0x2166));

    TEST(char_is_punctuation('!'));
    TEST(char_is_punctuation('('));
    TEST(char_is_punctuation(')'));
    TEST(char_is_punctuation('-'));
    TEST(char_is_punctuation('/'));
    TEST(char_is_punctuation('_'));
    TEST(char_is_punctuation(0xab));
    TEST(char_is_punctuation(0xbb));
    TEST(char_is_punctuation(0x2014));
    TEST(char_is_punctuation(0x207d));
    TEST(char_is_punctuation(0x2309));
    TEST(char_is_punctuation(0x2e0c));
    TEST(char_is_punctuation(0x2e21));
    TEST(char_is_punctuation(0xff3f));

    TEST(char_is_symbol(0x24));
    TEST(char_is_symbol(0x2b));
    TEST(char_is_symbol(0x5e));
    TEST(char_is_symbol(0xa6));
    TEST(char_is_symbol(0xf7));
    TEST(char_is_symbol(0x2c5));
    TEST(char_is_symbol(0x6fd));
    TEST(char_is_symbol(0x9fb));

    TEST(char_is_separator(0x20));
    TEST(char_is_separator(0x2006));
    TEST(char_is_separator(0x2028));
    TEST(char_is_separator(0x2029));

    TEST(char_is_alphanumeric('A'));    TEST(char_is_alphanumeric_w('A'));
    TEST(char_is_alphanumeric('Z'));    TEST(char_is_alphanumeric_w('Z'));
    TEST(! char_is_alphanumeric('_'));  TEST(char_is_alphanumeric_w('_'));
    TEST(char_is_alphanumeric('a'));    TEST(char_is_alphanumeric_w('a'));
    TEST(char_is_alphanumeric('z'));    TEST(char_is_alphanumeric_w('z'));
    TEST(char_is_letter('A'));          TEST(char_is_letter_w('A'));
    TEST(char_is_letter('Z'));          TEST(char_is_letter_w('Z'));
    TEST(! char_is_letter('_'));        TEST(char_is_letter_w('_'));
    TEST(char_is_letter('a'));          TEST(char_is_letter_w('a'));
    TEST(char_is_letter('z'));          TEST(char_is_letter_w('z'));
    TEST(! char_is_punctuation('A'));   TEST(! char_is_punctuation_w('A'));
    TEST(! char_is_punctuation('Z'));   TEST(! char_is_punctuation_w('Z'));
    TEST(char_is_punctuation('_'));     TEST(! char_is_punctuation_w('_'));
    TEST(! char_is_punctuation('a'));   TEST(! char_is_punctuation_w('a'));
    TEST(! char_is_punctuation('z'));   TEST(! char_is_punctuation_w('z'));

    std::vector<GC> v;
    TRY(v = gc_list());
    TEST_EQUAL(v.size(), 30);
    TEST_EQUAL(v.at(0), GC::Cc);
    TEST_EQUAL(v.at(29), GC::Zs);

}

void test_unicorn_character_boolean_properties() {

    TEST(char_is_assigned(0));
    TEST(char_is_assigned('A'));
    TEST(char_is_assigned(0x377)); // greek small letter pamphylian digamma
    TEST(! char_is_assigned(0x378));
    TEST(! char_is_assigned(0x379));
    TEST(char_is_assigned(0x37a)); // greek ypogegrammeni
    TEST(char_is_assigned(0x4e00)); // cjk ideograph, first
    TEST(char_is_assigned(0x9fcc)); // cjk ideograph, last
    TEST(char_is_assigned(0xac00)); // hangul syllable, first
    TEST(char_is_assigned(0xd7a3)); // hangul syllable, last
    TEST(! char_is_assigned(0xfdd0));
    TEST(! char_is_assigned(0xfdef));
    TEST(char_is_assigned(0xfeff)); // zero width no-break space (byte order mark)
    TEST(! char_is_assigned(0xffef));
    TEST(char_is_assigned(0xe01ef)); // variation selector-256
    TEST(! char_is_assigned(0x10ffff));
    TEST(! char_is_assigned(0x110000));

    TEST(! char_is_white_space(0));
    TEST(char_is_white_space('\n'));
    TEST(char_is_white_space(' '));
    TEST(! char_is_white_space('!'));
    TEST(! char_is_white_space('A'));
    TEST(char_is_white_space(0x2028));
    TEST(char_is_white_space(0x3000));
    TEST(! char_is_white_space(0x10ffff));

    TEST(! char_is_inline_space(0));
    TEST(! char_is_inline_space('\n'));
    TEST(char_is_inline_space(' '));
    TEST(! char_is_inline_space('!'));
    TEST(! char_is_inline_space('A'));
    TEST(! char_is_inline_space(0x2028));
    TEST(char_is_inline_space(0x3000));
    TEST(! char_is_inline_space(0x10ffff));

    TEST(! char_is_line_break(0));
    TEST(! char_is_line_break('\t')); // tab
    TEST(char_is_line_break('\n')); // line feed
    TEST(char_is_line_break('\v')); // vertical tab
    TEST(char_is_line_break('\f')); // form feed
    TEST(char_is_line_break('\r')); // carriage return
    TEST(char_is_line_break(0x85)); // next line
    TEST(char_is_line_break(0x2028)); // line separator
    TEST(char_is_line_break(0x2029)); // paragraph separator
    TEST(! char_is_line_break(0x3000)); // ideographic space
    TEST(! char_is_line_break(0x10ffff));

    TEST(! char_is_id_start(0));
    TEST(! char_is_id_start('!'));
    TEST(! char_is_id_start('0'));
    TEST(char_is_id_start('A'));
    TEST(! char_is_id_start(0xa1)); // inverted exclamation mark
    TEST(char_is_id_start(0x3a9)); // greek capital letter omega
    TEST(! char_is_id_start(0x10ffff));

    TEST(! char_is_id_nonstart(0));
    TEST(! char_is_id_nonstart('!'));
    TEST(char_is_id_nonstart('0'));
    TEST(! char_is_id_nonstart('A'));
    TEST(! char_is_id_nonstart(0xa1)); // inverted exclamation mark
    TEST(! char_is_id_nonstart(0x3a9)); // greek capital letter omega
    TEST(! char_is_id_nonstart(0x10ffff));

    TEST(! char_is_id_continue(0));
    TEST(! char_is_id_continue('!'));
    TEST(char_is_id_continue('0'));
    TEST(char_is_id_continue('A'));
    TEST(! char_is_id_continue(0xa1)); // inverted exclamation mark
    TEST(char_is_id_continue(0x3a9)); // greek capital letter omega
    TEST(! char_is_id_continue(0x10ffff));

    TEST(! char_is_xid_start(0));
    TEST(! char_is_xid_start('!'));
    TEST(! char_is_xid_start('0'));
    TEST(char_is_xid_start('A'));
    TEST(! char_is_xid_start(0xa1)); // inverted exclamation mark
    TEST(char_is_xid_start(0x3a9)); // greek capital letter omega
    TEST(! char_is_xid_start(0x10ffff));

    TEST(! char_is_xid_nonstart(0));
    TEST(! char_is_xid_nonstart('!'));
    TEST(char_is_xid_nonstart('0'));
    TEST(! char_is_xid_nonstart('A'));
    TEST(! char_is_xid_nonstart(0xa1)); // inverted exclamation mark
    TEST(! char_is_xid_nonstart(0x3a9)); // greek capital letter omega
    TEST(! char_is_xid_nonstart(0x10ffff));

    TEST(! char_is_xid_continue(0));
    TEST(! char_is_xid_continue('!'));
    TEST(char_is_xid_continue('0'));
    TEST(char_is_xid_continue('A'));
    TEST(! char_is_xid_continue(0xa1)); // inverted exclamation mark
    TEST(char_is_xid_continue(0x3a9)); // greek capital letter omega
    TEST(! char_is_xid_continue(0x10ffff));

    TEST(! char_is_pattern_syntax(0));
    TEST(char_is_pattern_syntax('!'));
    TEST(! char_is_pattern_syntax('0'));
    TEST(! char_is_pattern_syntax('A'));
    TEST(char_is_pattern_syntax(0xa1)); // inverted exclamation mark
    TEST(! char_is_pattern_syntax(0x3a9)); // greek capital letter omega
    TEST(! char_is_pattern_syntax(0x10ffff));

    TEST(! char_is_pattern_white_space(0));
    TEST(char_is_pattern_white_space('\n'));
    TEST(char_is_pattern_white_space(' '));
    TEST(! char_is_pattern_white_space('!'));
    TEST(! char_is_pattern_white_space('A'));
    TEST(char_is_pattern_white_space(0x2028)); // line separator
    TEST(! char_is_pattern_white_space(0x3000)); // ideographic space
    TEST(! char_is_pattern_white_space(0x10ffff));

    TEST(! char_is_default_ignorable(0));
    TEST(! char_is_default_ignorable('A'));
    TEST(char_is_default_ignorable(0xad)); // soft hyphen
    TEST(char_is_default_ignorable(0x34f)); // combining grapheme joiner
    TEST(char_is_default_ignorable(0x61c)); // arabic letter mark
    TEST(char_is_default_ignorable(0x200b)); // zero width space
    TEST(char_is_default_ignorable(0xfeff)); // zero width no-break space (byte order mark)
    TEST(! char_is_default_ignorable(0x10ffff));

    TEST(! char_is_soft_dotted('I'));
    TEST(! char_is_soft_dotted('J'));
    TEST(char_is_soft_dotted('i'));
    TEST(char_is_soft_dotted('j'));

}

void test_unicorn_character_arabic_shaping_properties() {

    TEST_PROPERTY_STRING(Joining_Type, Dual_Joining);
    TEST_PROPERTY_STRING(Joining_Type, Join_Causing);
    TEST_PROPERTY_STRING(Joining_Type, Left_Joining);
    TEST_PROPERTY_STRING(Joining_Type, Non_Joining);
    TEST_PROPERTY_STRING(Joining_Type, Right_Joining);
    TEST_PROPERTY_STRING(Joining_Type, Transparent);

    TEST_PROPERTY_STRING(Joining_Group, No_Joining_Group);
    TEST_PROPERTY_STRING(Joining_Group, Ain);
    TEST_PROPERTY_STRING(Joining_Group, Alaph);
    TEST_PROPERTY_STRING(Joining_Group, Alef);
    TEST_PROPERTY_STRING(Joining_Group, Beh);
    TEST_PROPERTY_STRING(Joining_Group, Beth);
    TEST_PROPERTY_STRING(Joining_Group, Burushaski_Yeh_Barree);
    TEST_PROPERTY_STRING(Joining_Group, Dalath_Rish);
    TEST_PROPERTY_STRING(Joining_Group, Dal);
    TEST_PROPERTY_STRING(Joining_Group, E);
    TEST_PROPERTY_STRING(Joining_Group, Farsi_Yeh);
    TEST_PROPERTY_STRING(Joining_Group, Feh);
    TEST_PROPERTY_STRING(Joining_Group, Fe);
    TEST_PROPERTY_STRING(Joining_Group, Final_Semkath);
    TEST_PROPERTY_STRING(Joining_Group, Gaf);
    TEST_PROPERTY_STRING(Joining_Group, Gamal);
    TEST_PROPERTY_STRING(Joining_Group, Hah);
    TEST_PROPERTY_STRING(Joining_Group, Heh_Goal);
    TEST_PROPERTY_STRING(Joining_Group, Heh);
    TEST_PROPERTY_STRING(Joining_Group, Heth);
    TEST_PROPERTY_STRING(Joining_Group, He);
    TEST_PROPERTY_STRING(Joining_Group, Kaf);
    TEST_PROPERTY_STRING(Joining_Group, Kaph);
    TEST_PROPERTY_STRING(Joining_Group, Khaph);
    TEST_PROPERTY_STRING(Joining_Group, Knotted_Heh);
    TEST_PROPERTY_STRING(Joining_Group, Lamadh);
    TEST_PROPERTY_STRING(Joining_Group, Lam);
    TEST_PROPERTY_STRING(Joining_Group, Meem);
    TEST_PROPERTY_STRING(Joining_Group, Mim);
    TEST_PROPERTY_STRING(Joining_Group, Noon);
    TEST_PROPERTY_STRING(Joining_Group, Nun);
    TEST_PROPERTY_STRING(Joining_Group, Nya);
    TEST_PROPERTY_STRING(Joining_Group, Pe);
    TEST_PROPERTY_STRING(Joining_Group, Qaf);
    TEST_PROPERTY_STRING(Joining_Group, Qaph);
    TEST_PROPERTY_STRING(Joining_Group, Reh);
    TEST_PROPERTY_STRING(Joining_Group, Reversed_Pe);
    TEST_PROPERTY_STRING(Joining_Group, Rohingya_Yeh);
    TEST_PROPERTY_STRING(Joining_Group, Sadhe);
    TEST_PROPERTY_STRING(Joining_Group, Sad);
    TEST_PROPERTY_STRING(Joining_Group, Seen);
    TEST_PROPERTY_STRING(Joining_Group, Semkath);
    TEST_PROPERTY_STRING(Joining_Group, Shin);
    TEST_PROPERTY_STRING(Joining_Group, Swash_Kaf);
    TEST_PROPERTY_STRING(Joining_Group, Syriac_Waw);
    TEST_PROPERTY_STRING(Joining_Group, Tah);
    TEST_PROPERTY_STRING(Joining_Group, Taw);
    TEST_PROPERTY_STRING(Joining_Group, Teh_Marbuta_Goal);
    TEST_PROPERTY_STRING(Joining_Group, Teh_Marbuta);
    TEST_PROPERTY_STRING(Joining_Group, Teth);
    TEST_PROPERTY_STRING(Joining_Group, Waw);
    TEST_PROPERTY_STRING(Joining_Group, Yeh_Barree);
    TEST_PROPERTY_STRING(Joining_Group, Yeh_With_Tail);
    TEST_PROPERTY_STRING(Joining_Group, Yeh);
    TEST_PROPERTY_STRING(Joining_Group, Yudh_He);
    TEST_PROPERTY_STRING(Joining_Group, Yudh);
    TEST_PROPERTY_STRING(Joining_Group, Zain);
    TEST_PROPERTY_STRING(Joining_Group, Zhain);

    TEST_EQUAL(to_str(joining_type(0x640)), "Joining_Type::Join_Causing");   // tatweel
    TEST_EQUAL(to_str(joining_type(0x620)), "Joining_Type::Dual_Joining");   // dotless yeh with separate ring below
    TEST_EQUAL(to_str(joining_type(0x622)), "Joining_Type::Right_Joining");  // alef with madda above
    TEST_EQUAL(to_str(joining_type(0x600)), "Joining_Type::Non_Joining");    // arabic number sign

    TEST_EQUAL(to_str(joining_group(0x600)), "Joining_Group::No_Joining_Group");  // arabic number sign
    TEST_EQUAL(to_str(joining_group(0x639)), "Joining_Group::Ain");               // ain
    TEST_EQUAL(to_str(joining_group(0x622)), "Joining_Group::Alef");              // alef with madda above
    TEST_EQUAL(to_str(joining_group(0x628)), "Joining_Group::Beh");               // beh
    TEST_EQUAL(to_str(joining_group(0x62F)), "Joining_Group::Dal");               // dal
    TEST_EQUAL(to_str(joining_group(0x63D)), "Joining_Group::Farsi_Yeh");         // farsi yeh with inverted v above
    TEST_EQUAL(to_str(joining_group(0x641)), "Joining_Group::Feh");               // feh
    TEST_EQUAL(to_str(joining_group(0x63B)), "Joining_Group::Gaf");               // keheh with 2 dots above
    TEST_EQUAL(to_str(joining_group(0x62C)), "Joining_Group::Hah");               // hah with dot below
    TEST_EQUAL(to_str(joining_group(0x647)), "Joining_Group::Heh");               // heh
    TEST_EQUAL(to_str(joining_group(0x6C1)), "Joining_Group::Heh_Goal");          // heh goal
    TEST_EQUAL(to_str(joining_group(0x643)), "Joining_Group::Kaf");               // kaf
    TEST_EQUAL(to_str(joining_group(0x6BE)), "Joining_Group::Knotted_Heh");       // knotted heh
    TEST_EQUAL(to_str(joining_group(0x644)), "Joining_Group::Lam");               // lam
    TEST_EQUAL(to_str(joining_group(0x645)), "Joining_Group::Meem");              // meem
    TEST_EQUAL(to_str(joining_group(0x646)), "Joining_Group::Noon");              // noon
    TEST_EQUAL(to_str(joining_group(0x6BD)), "Joining_Group::Nya");               // nya
    TEST_EQUAL(to_str(joining_group(0x642)), "Joining_Group::Qaf");               // qaf
    TEST_EQUAL(to_str(joining_group(0x631)), "Joining_Group::Reh");               // reh
    TEST_EQUAL(to_str(joining_group(0x635)), "Joining_Group::Sad");               // sad
    TEST_EQUAL(to_str(joining_group(0x633)), "Joining_Group::Seen");              // seen
    TEST_EQUAL(to_str(joining_group(0x6AA)), "Joining_Group::Swash_Kaf");         // swash kaf
    TEST_EQUAL(to_str(joining_group(0x637)), "Joining_Group::Tah");               // tah
    TEST_EQUAL(to_str(joining_group(0x629)), "Joining_Group::Teh_Marbuta");       // teh marbuta
    TEST_EQUAL(to_str(joining_group(0x6C3)), "Joining_Group::Teh_Marbuta_Goal");  // teh marbuta goal
    TEST_EQUAL(to_str(joining_group(0x624)), "Joining_Group::Waw");               // waw with hamza above
    TEST_EQUAL(to_str(joining_group(0x620)), "Joining_Group::Yeh");               // dotless yeh with separate ring below
    TEST_EQUAL(to_str(joining_group(0x6D2)), "Joining_Group::Yeh_Barree");        // yeh barree
    TEST_EQUAL(to_str(joining_group(0x6CD)), "Joining_Group::Yeh_With_Tail");     // yeh with tail

}

void test_unicorn_character_bidirectional_properties() {

    TEST_PROPERTY_STRING(Bidi_Class, AL);
    TEST_PROPERTY_STRING(Bidi_Class, AN);
    TEST_PROPERTY_STRING(Bidi_Class, B);
    TEST_PROPERTY_STRING(Bidi_Class, BN);
    TEST_PROPERTY_STRING(Bidi_Class, CS);
    TEST_PROPERTY_STRING(Bidi_Class, EN);
    TEST_PROPERTY_STRING(Bidi_Class, ES);
    TEST_PROPERTY_STRING(Bidi_Class, ET);
    TEST_PROPERTY_STRING(Bidi_Class, FSI);
    TEST_PROPERTY_STRING(Bidi_Class, L);
    TEST_PROPERTY_STRING(Bidi_Class, LRE);
    TEST_PROPERTY_STRING(Bidi_Class, LRI);
    TEST_PROPERTY_STRING(Bidi_Class, LRO);
    TEST_PROPERTY_STRING(Bidi_Class, NSM);
    TEST_PROPERTY_STRING(Bidi_Class, ON);
    TEST_PROPERTY_STRING(Bidi_Class, PDF);
    TEST_PROPERTY_STRING(Bidi_Class, PDI);
    TEST_PROPERTY_STRING(Bidi_Class, R);
    TEST_PROPERTY_STRING(Bidi_Class, RLE);
    TEST_PROPERTY_STRING(Bidi_Class, RLI);
    TEST_PROPERTY_STRING(Bidi_Class, RLO);
    TEST_PROPERTY_STRING(Bidi_Class, S);
    TEST_PROPERTY_STRING(Bidi_Class, WS);

    TEST_EQUAL(to_str(bidi_class(0)), "Bidi_Class::BN");
    TEST_EQUAL(to_str(bidi_class('\t')), "Bidi_Class::S");
    TEST_EQUAL(to_str(bidi_class('\n')), "Bidi_Class::B");
    TEST_EQUAL(to_str(bidi_class(' ')), "Bidi_Class::WS");
    TEST_EQUAL(to_str(bidi_class('!')), "Bidi_Class::ON");
    TEST_EQUAL(to_str(bidi_class('#')), "Bidi_Class::ET");
    TEST_EQUAL(to_str(bidi_class('+')), "Bidi_Class::ES");
    TEST_EQUAL(to_str(bidi_class(',')), "Bidi_Class::CS");
    TEST_EQUAL(to_str(bidi_class('0')), "Bidi_Class::EN");
    TEST_EQUAL(to_str(bidi_class('A')), "Bidi_Class::L");
    TEST_EQUAL(to_str(bidi_class(0x300)), "Bidi_Class::NSM");
    TEST_EQUAL(to_str(bidi_class(0x378)), "Bidi_Class::L");
    TEST_EQUAL(to_str(bidi_class(0x590)), "Bidi_Class::R");
    TEST_EQUAL(to_str(bidi_class(0x5de)), "Bidi_Class::R");
    TEST_EQUAL(to_str(bidi_class(0x608)), "Bidi_Class::AL");
    TEST_EQUAL(to_str(bidi_class(0x660)), "Bidi_Class::AN");
    TEST_EQUAL(to_str(bidi_class(0x202a)), "Bidi_Class::LRE");
    TEST_EQUAL(to_str(bidi_class(0x202b)), "Bidi_Class::RLE");
    TEST_EQUAL(to_str(bidi_class(0x202c)), "Bidi_Class::PDF");
    TEST_EQUAL(to_str(bidi_class(0x202d)), "Bidi_Class::LRO");
    TEST_EQUAL(to_str(bidi_class(0x202e)), "Bidi_Class::RLO");
    TEST_EQUAL(to_str(bidi_class(0x2066)), "Bidi_Class::LRI");
    TEST_EQUAL(to_str(bidi_class(0x2067)), "Bidi_Class::RLI");
    TEST_EQUAL(to_str(bidi_class(0x2068)), "Bidi_Class::FSI");
    TEST_EQUAL(to_str(bidi_class(0x2069)), "Bidi_Class::PDI");
    TEST_EQUAL(to_str(bidi_class(0x20bb)), "Bidi_Class::ET");
    TEST_EQUAL(to_str(bidi_class(0x10ffff)), "Bidi_Class::BN");

    TEST(char_is_bidi_mirrored('('));
    TEST(char_is_bidi_mirrored(')'));
    TEST(! char_is_bidi_mirrored('A'));

    TEST_EQUAL(bidi_mirroring_glyph('('), ')');
    TEST_EQUAL(bidi_mirroring_glyph(')'), '(');
    TEST_EQUAL(bidi_mirroring_glyph('A'), 0);

    TEST_EQUAL(bidi_paired_bracket('('), ')');
    TEST_EQUAL(bidi_paired_bracket(')'), '(');
    TEST_EQUAL(bidi_paired_bracket('A'), 0);

    TEST_EQUAL(bidi_paired_bracket_type('('), 'o');
    TEST_EQUAL(bidi_paired_bracket_type(')'), 'c');
    TEST_EQUAL(bidi_paired_bracket_type('A'), 'n');

}

void test_unicorn_character_block_properties() {

    TEST_EQUAL(char_block(0), "Basic Latin");
    TEST_EQUAL(char_block('A'), "Basic Latin");
    TEST_EQUAL(char_block(0xb5), "Latin-1 Supplement"); // micro sign
    TEST_EQUAL(char_block(0x391), "Greek and Coptic"); // greek capital letter alpha
    TEST_EQUAL(char_block(0x860), "");
    TEST_EQUAL(char_block(0x20ac), "Currency Symbols"); // euro sign
    TEST_EQUAL(char_block(0x10fffd), "Supplementary Private Use Area-B");
    TEST_EQUAL(char_block(0x110000), "");

    std::vector<BlockInfo> blocks;

    TRY(blocks = unicode_block_list());
    TEST(! blocks.empty());
    TEST_EQUAL(blocks[0].name, "Basic Latin");
    TEST_EQUAL(blocks[0].first, 0);
    TEST_EQUAL(blocks[0].last, 0x7f);

}

void test_unicorn_character_case_folding_properties() {

    TEST(! char_is_uppercase('\''));   // apostrophe
    TEST(char_is_uppercase('A'));      // latin capital letter a
    TEST(! char_is_uppercase('a'));    // latin small letter a
    TEST(char_is_uppercase(0xc0));     // latin capital letter a with grave
    TEST(! char_is_uppercase(0x1c5));  // latin capital letter d with small letter z with caron
    TEST(char_is_uppercase(0x391));    // greek capital letter alpha
    TEST(char_is_uppercase(0x2102));   // double-struck capital c
    TEST(char_is_uppercase(0x1d7ca));  // mathematical bold capital digamma

    TEST(! char_is_lowercase('\''));   // apostrophe
    TEST(! char_is_lowercase('A'));    // latin capital letter a
    TEST(char_is_lowercase('a'));      // latin small letter a
    TEST(char_is_lowercase(0xdf));     // latin small letter sharp s
    TEST(! char_is_lowercase(0x1c5));  // latin capital letter d with small letter z with caron
    TEST(char_is_lowercase(0x3b1));    // greek small letter alpha
    TEST(char_is_lowercase(0x2071));   // superscript latin small letter i
    TEST(char_is_lowercase(0x1d7cb));  // mathematical bold small digamma

    TEST(! char_is_titlecase('\''));  // apostrophe
    TEST(! char_is_titlecase('A'));   // latin capital letter a
    TEST(! char_is_titlecase('a'));   // latin small letter a
    TEST(char_is_titlecase(0x1c5));   // latin capital letter d with small letter z with caron
    TEST(char_is_titlecase(0x1c8));   // latin capital letter l with small letter j
    TEST(char_is_titlecase(0x1cb));   // latin capital letter n with small letter j
    TEST(char_is_titlecase(0x1f2));   // latin capital letter d with small letter z
    TEST(char_is_titlecase(0x1f88));  // greek capital letter alpha with psili and prosgegrammeni

    TEST_EQUAL(char_case('\''), Case::none);      // apostrophe
    TEST_EQUAL(char_case('A'), Case::upper);      // latin capital letter a
    TEST_EQUAL(char_case('a'), Case::lower);      // latin small letter a
    TEST_EQUAL(char_case(0xc0), Case::upper);     // latin capital letter a with grave
    TEST_EQUAL(char_case(0xdf), Case::lower);     // latin small letter sharp s
    TEST_EQUAL(char_case(0x1c5), Case::title);    // latin capital letter d with small letter z with caron
    TEST_EQUAL(char_case(0x1c8), Case::title);    // latin capital letter l with small letter j
    TEST_EQUAL(char_case(0x1cb), Case::title);    // latin capital letter n with small letter j
    TEST_EQUAL(char_case(0x1f2), Case::title);    // latin capital letter d with small letter z
    TEST_EQUAL(char_case(0x391), Case::upper);    // greek capital letter alpha
    TEST_EQUAL(char_case(0x3b1), Case::lower);    // greek small letter alpha
    TEST_EQUAL(char_case(0x1f88), Case::title);   // greek capital letter alpha with psili and prosgegrammeni
    TEST_EQUAL(char_case(0x2071), Case::lower);   // superscript latin small letter i
    TEST_EQUAL(char_case(0x2102), Case::upper);   // double-struck capital c
    TEST_EQUAL(char_case(0x1d7ca), Case::upper);  // mathematical bold capital digamma
    TEST_EQUAL(char_case(0x1d7cb), Case::lower);  // mathematical bold small digamma

    TEST(! char_is_case('\'', Case::upper));   // apostrophe
    TEST(char_is_case('A', Case::upper));      // latin capital letter a
    TEST(! char_is_case('a', Case::upper));    // latin small letter a
    TEST(char_is_case(0xc0, Case::upper));     // latin capital letter a with grave
    TEST(! char_is_case(0x1c5, Case::upper));  // latin capital letter d with small letter z with caron
    TEST(char_is_case(0x391, Case::upper));    // greek capital letter alpha
    TEST(char_is_case(0x2102, Case::upper));   // double-struck capital c
    TEST(char_is_case(0x1d7ca, Case::upper));  // mathematical bold capital digamma

    TEST(! char_is_case('\'', Case::lower));   // apostrophe
    TEST(! char_is_case('A', Case::lower));    // latin capital letter a
    TEST(char_is_case('a', Case::lower));      // latin small letter a
    TEST(char_is_case(0xdf, Case::lower));     // latin small letter sharp s
    TEST(! char_is_case(0x1c5, Case::lower));  // latin capital letter d with small letter z with caron
    TEST(char_is_case(0x3b1, Case::lower));    // greek small letter alpha
    TEST(char_is_case(0x2071, Case::lower));   // superscript latin small letter i
    TEST(char_is_case(0x1d7cb, Case::lower));  // mathematical bold small digamma

    TEST(! char_is_case('\'', Case::title));  // apostrophe
    TEST(! char_is_case('A', Case::title));   // latin capital letter a
    TEST(! char_is_case('a', Case::title));   // latin small letter a
    TEST(char_is_case(0x1c5, Case::title));   // latin capital letter d with small letter z with caron
    TEST(char_is_case(0x1c8, Case::title));   // latin capital letter l with small letter j
    TEST(char_is_case(0x1cb, Case::title));   // latin capital letter n with small letter j
    TEST(char_is_case(0x1f2, Case::title));   // latin capital letter d with small letter z
    TEST(char_is_case(0x1f88, Case::title));  // greek capital letter alpha with psili and prosgegrammeni

    TEST(! char_is_cased(0));        // null
    TEST(! char_is_cased('\''));     // apostrophe
    TEST(! char_is_cased('0'));      // digit zero
    TEST(! char_is_cased('@'));      // commercial at
    TEST(char_is_cased('A'));        // latin capital letter a
    TEST(char_is_cased('a'));        // latin small letter a
    TEST(! char_is_cased(0xa7));     // section sign
    TEST(! char_is_cased(0xa8));     // diaeresis
    TEST(char_is_cased(0xc0));       // latin capital letter a with grave
    TEST(char_is_cased(0xdf));       // latin small letter sharp s
    TEST(char_is_cased(0x1c5));      // latin capital letter d with small letter z with caron
    TEST(char_is_cased(0x2b0));      // modifier letter small h
    TEST(! char_is_cased(0x20ac));   // euro sign
    TEST(! char_is_cased(0x2200));   // for all
    TEST(! char_is_cased(0x3005));   // ideographic iteration mark
    TEST(! char_is_cased(0xe01ef));  // variation selector-256

    TEST(! char_is_case_ignorable(0));       // null
    TEST(char_is_case_ignorable('\''));      // apostrophe
    TEST(! char_is_case_ignorable('0'));     // digit zero
    TEST(! char_is_case_ignorable('@'));     // commercial at
    TEST(! char_is_case_ignorable('A'));     // latin capital letter a
    TEST(! char_is_case_ignorable('a'));     // latin small letter a
    TEST(! char_is_case_ignorable(0xa7));    // section sign
    TEST(char_is_case_ignorable(0xa8));      // diaeresis
    TEST(! char_is_case_ignorable(0xc0));    // latin capital letter a with grave
    TEST(! char_is_case_ignorable(0xdf));    // latin small letter sharp s
    TEST(! char_is_case_ignorable(0x1c5));   // latin capital letter d with small letter z with caron
    TEST(char_is_case_ignorable(0x2b0));     // modifier letter small h
    TEST(! char_is_case_ignorable(0x20ac));  // euro sign
    TEST(! char_is_case_ignorable(0x2200));  // for all
    TEST(char_is_case_ignorable(0x3005));    // ideographic iteration mark
    TEST(char_is_case_ignorable(0xe01ef));   // variation selector-256

    TEST_EQUAL(char_to_simple_uppercase('@'), '@');      // ampersand
    TEST_EQUAL(char_to_simple_uppercase('A'), 'A');      // latin capital letter a
    TEST_EQUAL(char_to_simple_uppercase('a'), 'A');      // latin small letter a
    TEST_EQUAL(char_to_simple_uppercase(0xa7), 0xa7);    // section sign
    TEST_EQUAL(char_to_simple_uppercase(0xc0), 0xc0);    // latin capital letter a with grave
    TEST_EQUAL(char_to_simple_uppercase(0xe0), 0xc0);    // latin small letter a with grave
    TEST_EQUAL(char_to_simple_uppercase(0xff), 0x178);   // latin small letter y with diaeresis
    TEST_EQUAL(char_to_simple_uppercase(0x178), 0x178);  // latin capital letter y with diaeresis
    TEST_EQUAL(char_to_simple_uppercase(0x1c4), 0x1c4);  // latin capital letter dz with caron
    TEST_EQUAL(char_to_simple_uppercase(0x1c5), 0x1c4);  // latin capital letter d with small letter z with caron
    TEST_EQUAL(char_to_simple_uppercase(0x1c6), 0x1c4);  // latin small letter dz with caron
    TEST_EQUAL(char_to_simple_uppercase(0xb5), 0x39c);   // micro sign
    TEST_EQUAL(char_to_simple_uppercase(0x130), 0x130);  // latin capital letter i with dot above

    TEST_EQUAL(char_to_simple_lowercase('@'), '@');      // ampersand
    TEST_EQUAL(char_to_simple_lowercase('A'), 'a');      // latin capital letter a
    TEST_EQUAL(char_to_simple_lowercase('a'), 'a');      // latin small letter a
    TEST_EQUAL(char_to_simple_lowercase(0xa7), 0xa7);    // section sign
    TEST_EQUAL(char_to_simple_lowercase(0xc0), 0xe0);    // latin capital letter a with grave
    TEST_EQUAL(char_to_simple_lowercase(0xe0), 0xe0);    // latin small letter a with grave
    TEST_EQUAL(char_to_simple_lowercase(0xff), 0xff);    // latin small letter y with diaeresis
    TEST_EQUAL(char_to_simple_lowercase(0x178), 0xff);   // latin capital letter y with diaeresis
    TEST_EQUAL(char_to_simple_lowercase(0x1c4), 0x1c6);  // latin capital letter dz with caron
    TEST_EQUAL(char_to_simple_lowercase(0x1c5), 0x1c6);  // latin capital letter d with small letter z with caron
    TEST_EQUAL(char_to_simple_lowercase(0x1c6), 0x1c6);  // latin small letter dz with caron
    TEST_EQUAL(char_to_simple_lowercase(0xb5), 0xb5);    // micro sign
    TEST_EQUAL(char_to_simple_lowercase(0x130), 0x69);   // latin capital letter i with dot above

    TEST_EQUAL(char_to_simple_titlecase('@'), '@');      // ampersand
    TEST_EQUAL(char_to_simple_titlecase('A'), 'A');      // latin capital letter a
    TEST_EQUAL(char_to_simple_titlecase('a'), 'A');      // latin small letter a
    TEST_EQUAL(char_to_simple_titlecase(0xa7), 0xa7);    // section sign
    TEST_EQUAL(char_to_simple_titlecase(0xc0), 0xc0);    // latin capital letter a with grave
    TEST_EQUAL(char_to_simple_titlecase(0xe0), 0xc0);    // latin small letter a with grave
    TEST_EQUAL(char_to_simple_titlecase(0xff), 0x178);   // latin small letter y with diaeresis
    TEST_EQUAL(char_to_simple_titlecase(0x178), 0x178);  // latin capital letter y with diaeresis
    TEST_EQUAL(char_to_simple_titlecase(0x1c4), 0x1c5);  // latin capital letter dz with caron
    TEST_EQUAL(char_to_simple_titlecase(0x1c5), 0x1c5);  // latin capital letter d with small letter z with caron
    TEST_EQUAL(char_to_simple_titlecase(0x1c6), 0x1c5);  // latin small letter dz with caron
    TEST_EQUAL(char_to_simple_titlecase(0xb5), 0x39c);   // micro sign
    TEST_EQUAL(char_to_simple_titlecase(0x130), 0x130);  // latin capital letter i with dot above

    TEST_EQUAL(char_to_simple_casefold('@'), '@');      // ampersand
    TEST_EQUAL(char_to_simple_casefold('A'), 'a');      // latin capital letter a
    TEST_EQUAL(char_to_simple_casefold('a'), 'a');      // latin small letter a
    TEST_EQUAL(char_to_simple_casefold(0xa7), 0xa7);    // section sign
    TEST_EQUAL(char_to_simple_casefold(0xc0), 0xe0);    // latin capital letter a with grave
    TEST_EQUAL(char_to_simple_casefold(0xe0), 0xe0);    // latin small letter a with grave
    TEST_EQUAL(char_to_simple_casefold(0xff), 0xff);    // latin small letter y with diaeresis
    TEST_EQUAL(char_to_simple_casefold(0x178), 0xff);   // latin capital letter y with diaeresis
    TEST_EQUAL(char_to_simple_casefold(0x1c4), 0x1c6);  // latin capital letter dz with caron
    TEST_EQUAL(char_to_simple_casefold(0x1c5), 0x1c6);  // latin capital letter d with small letter z with caron
    TEST_EQUAL(char_to_simple_casefold(0x1c6), 0x1c6);  // latin small letter dz with caron
    TEST_EQUAL(char_to_simple_casefold(0xb5), 0x3bc);   // micro sign
    TEST_EQUAL(char_to_simple_casefold(0x130), 0x130);  // latin capital letter i with dot above

    TEST_EQUAL(char_to_simple_case('@', Case::upper), '@');      // ampersand
    TEST_EQUAL(char_to_simple_case('A', Case::upper), 'A');      // latin capital letter a
    TEST_EQUAL(char_to_simple_case('a', Case::upper), 'A');      // latin small letter a
    TEST_EQUAL(char_to_simple_case(0xa7, Case::upper), 0xa7);    // section sign
    TEST_EQUAL(char_to_simple_case(0xc0, Case::upper), 0xc0);    // latin capital letter a with grave
    TEST_EQUAL(char_to_simple_case(0xe0, Case::upper), 0xc0);    // latin small letter a with grave
    TEST_EQUAL(char_to_simple_case(0xff, Case::upper), 0x178);   // latin small letter y with diaeresis
    TEST_EQUAL(char_to_simple_case(0x178, Case::upper), 0x178);  // latin capital letter y with diaeresis
    TEST_EQUAL(char_to_simple_case(0x1c4, Case::upper), 0x1c4);  // latin capital letter dz with caron
    TEST_EQUAL(char_to_simple_case(0x1c5, Case::upper), 0x1c4);  // latin capital letter d with small letter z with caron
    TEST_EQUAL(char_to_simple_case(0x1c6, Case::upper), 0x1c4);  // latin small letter dz with caron
    TEST_EQUAL(char_to_simple_case(0xb5, Case::upper), 0x39c);   // micro sign
    TEST_EQUAL(char_to_simple_case(0x130, Case::upper), 0x130);  // latin capital letter i with dot above

    TEST_EQUAL(char_to_simple_case('@', Case::lower), '@');      // ampersand
    TEST_EQUAL(char_to_simple_case('A', Case::lower), 'a');      // latin capital letter a
    TEST_EQUAL(char_to_simple_case('a', Case::lower), 'a');      // latin small letter a
    TEST_EQUAL(char_to_simple_case(0xa7, Case::lower), 0xa7);    // section sign
    TEST_EQUAL(char_to_simple_case(0xc0, Case::lower), 0xe0);    // latin capital letter a with grave
    TEST_EQUAL(char_to_simple_case(0xe0, Case::lower), 0xe0);    // latin small letter a with grave
    TEST_EQUAL(char_to_simple_case(0xff, Case::lower), 0xff);    // latin small letter y with diaeresis
    TEST_EQUAL(char_to_simple_case(0x178, Case::lower), 0xff);   // latin capital letter y with diaeresis
    TEST_EQUAL(char_to_simple_case(0x1c4, Case::lower), 0x1c6);  // latin capital letter dz with caron
    TEST_EQUAL(char_to_simple_case(0x1c5, Case::lower), 0x1c6);  // latin capital letter d with small letter z with caron
    TEST_EQUAL(char_to_simple_case(0x1c6, Case::lower), 0x1c6);  // latin small letter dz with caron
    TEST_EQUAL(char_to_simple_case(0xb5, Case::lower), 0xb5);    // micro sign
    TEST_EQUAL(char_to_simple_case(0x130, Case::lower), 0x69);   // latin capital letter i with dot above

    TEST_EQUAL(char_to_simple_case('@', Case::title), '@');      // ampersand
    TEST_EQUAL(char_to_simple_case('A', Case::title), 'A');      // latin capital letter a
    TEST_EQUAL(char_to_simple_case('a', Case::title), 'A');      // latin small letter a
    TEST_EQUAL(char_to_simple_case(0xa7, Case::title), 0xa7);    // section sign
    TEST_EQUAL(char_to_simple_case(0xc0, Case::title), 0xc0);    // latin capital letter a with grave
    TEST_EQUAL(char_to_simple_case(0xe0, Case::title), 0xc0);    // latin small letter a with grave
    TEST_EQUAL(char_to_simple_case(0xff, Case::title), 0x178);   // latin small letter y with diaeresis
    TEST_EQUAL(char_to_simple_case(0x178, Case::title), 0x178);  // latin capital letter y with diaeresis
    TEST_EQUAL(char_to_simple_case(0x1c4, Case::title), 0x1c5);  // latin capital letter dz with caron
    TEST_EQUAL(char_to_simple_case(0x1c5, Case::title), 0x1c5);  // latin capital letter d with small letter z with caron
    TEST_EQUAL(char_to_simple_case(0x1c6, Case::title), 0x1c5);  // latin small letter dz with caron
    TEST_EQUAL(char_to_simple_case(0xb5, Case::title), 0x39c);   // micro sign
    TEST_EQUAL(char_to_simple_case(0x130, Case::title), 0x130);  // latin capital letter i with dot above

    TEST_EQUAL(char_to_simple_case('@', Case::fold), '@');      // ampersand
    TEST_EQUAL(char_to_simple_case('A', Case::fold), 'a');      // latin capital letter a
    TEST_EQUAL(char_to_simple_case('a', Case::fold), 'a');      // latin small letter a
    TEST_EQUAL(char_to_simple_case(0xa7, Case::fold), 0xa7);    // section sign
    TEST_EQUAL(char_to_simple_case(0xc0, Case::fold), 0xe0);    // latin capital letter a with grave
    TEST_EQUAL(char_to_simple_case(0xe0, Case::fold), 0xe0);    // latin small letter a with grave
    TEST_EQUAL(char_to_simple_case(0xff, Case::fold), 0xff);    // latin small letter y with diaeresis
    TEST_EQUAL(char_to_simple_case(0x178, Case::fold), 0xff);   // latin capital letter y with diaeresis
    TEST_EQUAL(char_to_simple_case(0x1c4, Case::fold), 0x1c6);  // latin capital letter dz with caron
    TEST_EQUAL(char_to_simple_case(0x1c5, Case::fold), 0x1c6);  // latin capital letter d with small letter z with caron
    TEST_EQUAL(char_to_simple_case(0x1c6, Case::fold), 0x1c6);  // latin small letter dz with caron
    TEST_EQUAL(char_to_simple_case(0xb5, Case::fold), 0x3bc);   // micro sign
    TEST_EQUAL(char_to_simple_case(0x130, Case::fold), 0x130);  // latin capital letter i with dot above

    char32_t buf[100];

    // U+00DF = latin small letter sharp s
    // U+0130 = latin capital letter i with dot above
    // U+FB03 = latin small ligature ffi

    TEST_EQUAL(char_to_full_uppercase('@', buf), 1);     TEST_EQUAL(buf[0], '@');
    TEST_EQUAL(char_to_full_uppercase('A', buf), 1);     TEST_EQUAL(buf[0], 'A');
    TEST_EQUAL(char_to_full_uppercase('a', buf), 1);     TEST_EQUAL(buf[0], 'A');
    TEST_EQUAL(char_to_full_uppercase(0xdf, buf), 2);    TEST_EQUAL(buf[0], 0x53); TEST_EQUAL(buf[1], 0x53);
    TEST_EQUAL(char_to_full_uppercase(0x130, buf), 1);   TEST_EQUAL(buf[0], 0x130);
    TEST_EQUAL(char_to_full_uppercase(0xfb03, buf), 3);  TEST_EQUAL(buf[0], 0x46); TEST_EQUAL(buf[1], 0x46); TEST_EQUAL(buf[2], 0x49);
    TEST_EQUAL(char_to_full_lowercase('@', buf), 1);     TEST_EQUAL(buf[0], '@');
    TEST_EQUAL(char_to_full_lowercase('A', buf), 1);     TEST_EQUAL(buf[0], 'a');
    TEST_EQUAL(char_to_full_lowercase('a', buf), 1);     TEST_EQUAL(buf[0], 'a');
    TEST_EQUAL(char_to_full_lowercase(0xdf, buf), 1);    TEST_EQUAL(buf[0], 0xdf);
    TEST_EQUAL(char_to_full_lowercase(0x130, buf), 2);   TEST_EQUAL(buf[0], 0x69); TEST_EQUAL(buf[1], 0x307);
    TEST_EQUAL(char_to_full_lowercase(0xfb03, buf), 1);  TEST_EQUAL(buf[0], 0xfb03);
    TEST_EQUAL(char_to_full_titlecase('@', buf), 1);     TEST_EQUAL(buf[0], '@');
    TEST_EQUAL(char_to_full_titlecase('A', buf), 1);     TEST_EQUAL(buf[0], 'A');
    TEST_EQUAL(char_to_full_titlecase('a', buf), 1);     TEST_EQUAL(buf[0], 'A');
    TEST_EQUAL(char_to_full_titlecase(0xdf, buf), 2);    TEST_EQUAL(buf[0], 0x53); TEST_EQUAL(buf[1], 0x73);
    TEST_EQUAL(char_to_full_titlecase(0x130, buf), 1);   TEST_EQUAL(buf[0], 0x130);
    TEST_EQUAL(char_to_full_titlecase(0xfb03, buf), 3);  TEST_EQUAL(buf[0], 0x46); TEST_EQUAL(buf[1], 0x66); TEST_EQUAL(buf[2], 0x69);
    TEST_EQUAL(char_to_full_casefold('@', buf), 1);      TEST_EQUAL(buf[0], '@');
    TEST_EQUAL(char_to_full_casefold('A', buf), 1);      TEST_EQUAL(buf[0], 'a');
    TEST_EQUAL(char_to_full_casefold('a', buf), 1);      TEST_EQUAL(buf[0], 'a');
    TEST_EQUAL(char_to_full_casefold(0xdf, buf), 2);     TEST_EQUAL(buf[0], 0x73); TEST_EQUAL(buf[1], 0x73);
    TEST_EQUAL(char_to_full_casefold(0x130, buf), 2);    TEST_EQUAL(buf[0], 0x69); TEST_EQUAL(buf[1], 0x307);
    TEST_EQUAL(char_to_full_casefold(0xfb03, buf), 3);   TEST_EQUAL(buf[0], 0x66); TEST_EQUAL(buf[1], 0x66); TEST_EQUAL(buf[2], 0x69);

    TEST_EQUAL(char_to_full_case('@', buf, Case::upper), 1);     TEST_EQUAL(buf[0], '@');
    TEST_EQUAL(char_to_full_case('A', buf, Case::upper), 1);     TEST_EQUAL(buf[0], 'A');
    TEST_EQUAL(char_to_full_case('a', buf, Case::upper), 1);     TEST_EQUAL(buf[0], 'A');
    TEST_EQUAL(char_to_full_case(0xdf, buf, Case::upper), 2);    TEST_EQUAL(buf[0], 0x53); TEST_EQUAL(buf[1], 0x53);
    TEST_EQUAL(char_to_full_case(0x130, buf, Case::upper), 1);   TEST_EQUAL(buf[0], 0x130);
    TEST_EQUAL(char_to_full_case(0xfb03, buf, Case::upper), 3);  TEST_EQUAL(buf[0], 0x46); TEST_EQUAL(buf[1], 0x46); TEST_EQUAL(buf[2], 0x49);
    TEST_EQUAL(char_to_full_case('@', buf, Case::lower), 1);     TEST_EQUAL(buf[0], '@');
    TEST_EQUAL(char_to_full_case('A', buf, Case::lower), 1);     TEST_EQUAL(buf[0], 'a');
    TEST_EQUAL(char_to_full_case('a', buf, Case::lower), 1);     TEST_EQUAL(buf[0], 'a');
    TEST_EQUAL(char_to_full_case(0xdf, buf, Case::lower), 1);    TEST_EQUAL(buf[0], 0xdf);
    TEST_EQUAL(char_to_full_case(0x130, buf, Case::lower), 2);   TEST_EQUAL(buf[0], 0x69); TEST_EQUAL(buf[1], 0x307);
    TEST_EQUAL(char_to_full_case(0xfb03, buf, Case::lower), 1);  TEST_EQUAL(buf[0], 0xfb03);
    TEST_EQUAL(char_to_full_case('@', buf, Case::title), 1);     TEST_EQUAL(buf[0], '@');
    TEST_EQUAL(char_to_full_case('A', buf, Case::title), 1);     TEST_EQUAL(buf[0], 'A');
    TEST_EQUAL(char_to_full_case('a', buf, Case::title), 1);     TEST_EQUAL(buf[0], 'A');
    TEST_EQUAL(char_to_full_case(0xdf, buf, Case::title), 2);    TEST_EQUAL(buf[0], 0x53); TEST_EQUAL(buf[1], 0x73);
    TEST_EQUAL(char_to_full_case(0x130, buf, Case::title), 1);   TEST_EQUAL(buf[0], 0x130);
    TEST_EQUAL(char_to_full_case(0xfb03, buf, Case::title), 3);  TEST_EQUAL(buf[0], 0x46); TEST_EQUAL(buf[1], 0x66); TEST_EQUAL(buf[2], 0x69);
    TEST_EQUAL(char_to_full_case('@', buf, Case::fold), 1);      TEST_EQUAL(buf[0], '@');
    TEST_EQUAL(char_to_full_case('A', buf, Case::fold), 1);      TEST_EQUAL(buf[0], 'a');
    TEST_EQUAL(char_to_full_case('a', buf, Case::fold), 1);      TEST_EQUAL(buf[0], 'a');
    TEST_EQUAL(char_to_full_case(0xdf, buf, Case::fold), 2);     TEST_EQUAL(buf[0], 0x73); TEST_EQUAL(buf[1], 0x73);
    TEST_EQUAL(char_to_full_case(0x130, buf, Case::fold), 2);    TEST_EQUAL(buf[0], 0x69); TEST_EQUAL(buf[1], 0x307);
    TEST_EQUAL(char_to_full_case(0xfb03, buf, Case::fold), 3);   TEST_EQUAL(buf[0], 0x66); TEST_EQUAL(buf[1], 0x66); TEST_EQUAL(buf[2], 0x69);

}

void test_unicorn_character_names() {

    TEST_EQUAL(char_name(0), "");
    TEST_EQUAL(char_name(' '), "SPACE");
    TEST_EQUAL(char_name('0'), "DIGIT ZERO");
    TEST_EQUAL(char_name('A'), "LATIN CAPITAL LETTER A");
    TEST_EQUAL(char_name('a'), "LATIN SMALL LETTER A");
    TEST_EQUAL(char_name(0x391), "GREEK CAPITAL LETTER ALPHA");
    TEST_EQUAL(char_name(0x20ac), "EURO SIGN");
    TEST_EQUAL(char_name(0x4e00), "CJK UNIFIED IDEOGRAPH-4E00");
    TEST_EQUAL(char_name(0xd4db), "HANGUL SYLLABLE PWILH");
    TEST_EQUAL(char_name(0xfffd), "REPLACEMENT CHARACTER");
    TEST_EQUAL(char_name(0xe01ef), "VARIATION SELECTOR-256");

    TEST_EQUAL(char_name(0, Cname::lower), "");
    TEST_EQUAL(char_name('A', Cname::lower), "latin capital letter a");
    TEST_EQUAL(char_name(0x20ac, Cname::lower), "euro sign");

    TEST_EQUAL(char_name(0, Cname::prefix), "U+0000");
    TEST_EQUAL(char_name('A', Cname::prefix), "U+0041 LATIN CAPITAL LETTER A");
    TEST_EQUAL(char_name(0x20ac, Cname::prefix), "U+20AC EURO SIGN");

    TEST_EQUAL(char_name(0, Cname::lower | Cname::prefix), "U+0000");
    TEST_EQUAL(char_name('A', Cname::lower | Cname::prefix), "U+0041 latin capital letter a");
    TEST_EQUAL(char_name(0x20ac, Cname::lower | Cname::prefix), "U+20AC euro sign");

    TEST_EQUAL(char_name(0x1a2), "LATIN CAPITAL LETTER OI");
    TEST_EQUAL(char_name(0x1a2, Cname::update), "LATIN CAPITAL LETTER GHA");
    TEST_EQUAL(char_name(0x1d0c5), "BYZANTINE MUSICAL SYMBOL FHTORA SKLIRON CHROMA VASIS");
    TEST_EQUAL(char_name(0x1d0c5, Cname::update), "BYZANTINE MUSICAL SYMBOL FTHORA SKLIRON CHROMA VASIS");

    TEST_EQUAL(char_name(0), "");
    TEST_EQUAL(char_name(0xa), "");
    TEST_EQUAL(char_name(0x7f), "");
    TEST_EQUAL(char_name(0x9f), "");
    TEST_EQUAL(char_name(0, Cname::control), "NULL");
    TEST_EQUAL(char_name(0xa, Cname::control), "LINE FEED");
    TEST_EQUAL(char_name(0x7f, Cname::control), "DELETE");
    TEST_EQUAL(char_name(0x9f, Cname::control), "APPLICATION PROGRAM COMMAND");
    TEST_EQUAL(char_name(0, Cname::label), "<control-0000>");
    TEST_EQUAL(char_name(0xa, Cname::label), "<control-000A>");
    TEST_EQUAL(char_name(0x7f, Cname::label), "<control-007F>");
    TEST_EQUAL(char_name(0x9f, Cname::label), "<control-009F>");

    TEST_EQUAL(char_name(0x20ff), "");
    TEST_EQUAL(char_name(0xd800), "");
    TEST_EQUAL(char_name(0xe000), "");
    TEST_EQUAL(char_name(0xfdd0), "");
    TEST_EQUAL(char_name(0x110000), "");
    TEST_EQUAL(char_name(0x20ff, Cname::label), "<reserved-20FF>");
    TEST_EQUAL(char_name(0xd800, Cname::label), "<surrogate-D800>");
    TEST_EQUAL(char_name(0xe000, Cname::label), "<private-use-E000>");
    TEST_EQUAL(char_name(0xfdd0, Cname::label), "<noncharacter-FDD0>");
    TEST_EQUAL(char_name(0x110000, Cname::label), "<noncharacter-110000>");

    TEST_EQUAL(char_name(0, Cname::all), "U+0000 null");
    TEST_EQUAL(char_name(0xa, Cname::all), "U+000A line feed");
    TEST_EQUAL(char_name(' ', Cname::all), "U+0020 space");
    TEST_EQUAL(char_name('0', Cname::all), "U+0030 digit zero");
    TEST_EQUAL(char_name('A', Cname::all), "U+0041 latin capital letter a");
    TEST_EQUAL(char_name('a', Cname::all), "U+0061 latin small letter a");
    TEST_EQUAL(char_name(0x7f, Cname::all), "U+007F delete");
    TEST_EQUAL(char_name(0x9f, Cname::all), "U+009F application program command");
    TEST_EQUAL(char_name(0x1a2, Cname::all), "U+01A2 latin capital letter gha");
    TEST_EQUAL(char_name(0x391, Cname::all), "U+0391 greek capital letter alpha");
    TEST_EQUAL(char_name(0x20ac, Cname::all), "U+20AC euro sign");
    TEST_EQUAL(char_name(0x20ff, Cname::all), "U+20FF <reserved-20ff>");
    TEST_EQUAL(char_name(0x4e00, Cname::all), "U+4E00 cjk unified ideograph-4e00");
    TEST_EQUAL(char_name(0xd4db, Cname::all), "U+D4DB hangul syllable pwilh");
    TEST_EQUAL(char_name(0xd800, Cname::all), "U+D800 <surrogate-d800>");
    TEST_EQUAL(char_name(0xe000, Cname::all), "U+E000 <private-use-e000>");
    TEST_EQUAL(char_name(0xfdd0, Cname::all), "U+FDD0 <noncharacter-fdd0>");
    TEST_EQUAL(char_name(0xfffd, Cname::all), "U+FFFD replacement character");
    TEST_EQUAL(char_name(0x1d0c5, Cname::all), "U+1D0C5 byzantine musical symbol fthora skliron chroma vasis");
    TEST_EQUAL(char_name(0xe01ef, Cname::all), "U+E01EF variation selector-256");
    TEST_EQUAL(char_name(0x110000, Cname::all), "U+110000 <noncharacter-110000>");

    for (char32_t c = 0; c <= 0x10ffff; ++c)
        TEST_COMPARE(char_name(c, Cname::control | Cname::label), !=, "");

}

void test_unicorn_character_decomposition_properties() {

    std::map<char32_t, int> decomp_census;
    for (auto&& entry: UnicornDetail::canonical_table)
        ++decomp_census[entry.key];
    for (auto&& entry: UnicornDetail::short_compatibility_table)
        ++decomp_census[entry.key];
    for (auto&& entry: UnicornDetail::long_compatibility_table)
        ++decomp_census[entry.key];
    for (auto&& entry: decomp_census)
        if (entry.second > 1)
            FAIL("U+" + hex(entry.first) + " appears in " + std::to_string(entry.second) + " decomposition tables");

    for (auto&& entry: UnicornDetail::composition_table)
        TEST_EQUAL(combining_class(entry.key[0]), 0);

    TEST_EQUAL(combining_class(0), 0);
    TEST_EQUAL(combining_class('A'), 0);
    TEST_EQUAL(combining_class(0x334), 1);    // combining tilde overlay
    TEST_EQUAL(combining_class(0x94d), 9);    // devanagari sign virama
    TEST_EQUAL(combining_class(0x316), 220);  // combining grave accent below
    TEST_EQUAL(combining_class(0x300), 230);  // combining grave accent
    TEST_EQUAL(combining_class(0x10ffff), 0);

    char32_t buf[100];

    #define DECOMPOSITION_TEST(mode, chr, length, expect) \
        do { \
            size_t n; \
            TRY(n = mode##_decomposition(char_to_uint(chr), buf)); \
            TEST_EQUAL(n, length); \
            std::u32string decomp(buf + 0, buf + n); \
            TEST_EQUAL(decomp, expect); \
        } while (false)

    DECOMPOSITION_TEST(canonical, 'A', 0, std::u32string{});
    DECOMPOSITION_TEST(canonical, 0xc0, 2, (std::u32string{'A',0x300}));
    DECOMPOSITION_TEST(canonical, 0xff, 2, (std::u32string{'y',0x308}));
    DECOMPOSITION_TEST(compatibility, 'A', 0, std::u32string{});
    DECOMPOSITION_TEST(compatibility, 0xb5, 1, (std::u32string{0x3bc}));
    DECOMPOSITION_TEST(compatibility, 0xbd, 3, (std::u32string{'1',0x2044,'2'}));
    DECOMPOSITION_TEST(compatibility, 0xfdfa, 18,
        (std::u32string{0x635,0x644,0x649,' ',0x627,0x644,0x644,0x647,' ',
        0x639,0x644,0x64a,0x647,' ',0x648,0x633,0x644,0x645}));
    DECOMPOSITION_TEST(canonical, 0xd4db, 2, (std::u32string{0xd4cc,0x11b6}));
    DECOMPOSITION_TEST(canonical, 0xd4cc, 2, (std::u32string{0x1111,0x1171}));

    TEST_EQUAL(canonical_composition(0x41, 0x42), 0);
    TEST_EQUAL(canonical_composition(0x41, 0x300), 0xc0);
    TEST_EQUAL(canonical_composition(0x79, 0x308), 0xff);
    TEST_EQUAL(canonical_composition(0x1111, 0x1171), 0xd4cc); // Hangul
    TEST_EQUAL(canonical_composition(0xd4cc, 0x11b6), 0xd4db); // Hangul

}

void test_unicorn_character_enumeration_properties() {

    TEST_PROPERTY_STRING(East_Asian_Width, N);
    TEST_PROPERTY_STRING(East_Asian_Width, A);
    TEST_PROPERTY_STRING(East_Asian_Width, F);
    TEST_PROPERTY_STRING(East_Asian_Width, H);
    TEST_PROPERTY_STRING(East_Asian_Width, Na);
    TEST_PROPERTY_STRING(East_Asian_Width, W);

    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, Other);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, Control);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, CR);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, EOT);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, Extend);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, L);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, LF);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, LV);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, LVT);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, Prepend);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, Regional_Indicator);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, SOT);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, SpacingMark);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, T);
    TEST_PROPERTY_STRING(Grapheme_Cluster_Break, V);

    TEST_PROPERTY_STRING(Hangul_Syllable_Type, NA);
    TEST_PROPERTY_STRING(Hangul_Syllable_Type, L);
    TEST_PROPERTY_STRING(Hangul_Syllable_Type, LV);
    TEST_PROPERTY_STRING(Hangul_Syllable_Type, LVT);
    TEST_PROPERTY_STRING(Hangul_Syllable_Type, T);
    TEST_PROPERTY_STRING(Hangul_Syllable_Type, V);

    TEST_PROPERTY_STRING(Indic_Positional_Category, NA);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Bottom);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Bottom_And_Right);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Left);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Left_And_Right);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Overstruck);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Right);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Top);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Top_And_Bottom);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Top_And_Bottom_And_Right);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Top_And_Left);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Top_And_Left_And_Right);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Top_And_Right);
    TEST_PROPERTY_STRING(Indic_Positional_Category, Visual_Order_Left);

    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Other);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Avagraha);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Bindu);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Brahmi_Joining_Number);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Cantillation_Mark);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Consonant);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Consonant_Dead);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Consonant_Final);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Consonant_Head_Letter);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Consonant_Medial);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Consonant_Placeholder);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Consonant_Preceding_Repha);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Consonant_Subjoined);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Consonant_Succeeding_Repha);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Gemination_Mark);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Invisible_Stacker);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Joiner);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Modifying_Letter);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Non_Joiner);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Nukta);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Number);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Number_Joiner);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Pure_Killer);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Register_Shifter);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Tone_Letter);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Tone_Mark);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Virama);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Visarga);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Vowel);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Vowel_Dependent);
    TEST_PROPERTY_STRING(Indic_Syllabic_Category, Vowel_Independent);

    TEST_PROPERTY_STRING(Line_Break, XX);
    TEST_PROPERTY_STRING(Line_Break, AI);
    TEST_PROPERTY_STRING(Line_Break, AL);
    TEST_PROPERTY_STRING(Line_Break, B2);
    TEST_PROPERTY_STRING(Line_Break, BA);
    TEST_PROPERTY_STRING(Line_Break, BB);
    TEST_PROPERTY_STRING(Line_Break, BK);
    TEST_PROPERTY_STRING(Line_Break, CB);
    TEST_PROPERTY_STRING(Line_Break, CJ);
    TEST_PROPERTY_STRING(Line_Break, CL);
    TEST_PROPERTY_STRING(Line_Break, CM);
    TEST_PROPERTY_STRING(Line_Break, CP);
    TEST_PROPERTY_STRING(Line_Break, CR);
    TEST_PROPERTY_STRING(Line_Break, EX);
    TEST_PROPERTY_STRING(Line_Break, GL);
    TEST_PROPERTY_STRING(Line_Break, H2);
    TEST_PROPERTY_STRING(Line_Break, H3);
    TEST_PROPERTY_STRING(Line_Break, HL);
    TEST_PROPERTY_STRING(Line_Break, HY);
    TEST_PROPERTY_STRING(Line_Break, ID);
    TEST_PROPERTY_STRING(Line_Break, IN_);
    TEST_PROPERTY_STRING(Line_Break, IS);
    TEST_PROPERTY_STRING(Line_Break, JL);
    TEST_PROPERTY_STRING(Line_Break, JT);
    TEST_PROPERTY_STRING(Line_Break, JV);
    TEST_PROPERTY_STRING(Line_Break, LF);
    TEST_PROPERTY_STRING(Line_Break, NL);
    TEST_PROPERTY_STRING(Line_Break, NS);
    TEST_PROPERTY_STRING(Line_Break, NU);
    TEST_PROPERTY_STRING(Line_Break, OP);
    TEST_PROPERTY_STRING(Line_Break, PO);
    TEST_PROPERTY_STRING(Line_Break, PR);
    TEST_PROPERTY_STRING(Line_Break, QU);
    TEST_PROPERTY_STRING(Line_Break, RI);
    TEST_PROPERTY_STRING(Line_Break, SA);
    TEST_PROPERTY_STRING(Line_Break, SG);
    TEST_PROPERTY_STRING(Line_Break, SP);
    TEST_PROPERTY_STRING(Line_Break, SY);
    TEST_PROPERTY_STRING(Line_Break, WJ);
    TEST_PROPERTY_STRING(Line_Break, ZW);

    TEST_PROPERTY_STRING(Numeric_Type, None);
    TEST_PROPERTY_STRING(Numeric_Type, Decimal);
    TEST_PROPERTY_STRING(Numeric_Type, Digit);
    TEST_PROPERTY_STRING(Numeric_Type, Numeric);

    TEST_PROPERTY_STRING(Sentence_Break, Other);
    TEST_PROPERTY_STRING(Sentence_Break, ATerm);
    TEST_PROPERTY_STRING(Sentence_Break, Close);
    TEST_PROPERTY_STRING(Sentence_Break, CR);
    TEST_PROPERTY_STRING(Sentence_Break, EOT);
    TEST_PROPERTY_STRING(Sentence_Break, Extend);
    TEST_PROPERTY_STRING(Sentence_Break, Format);
    TEST_PROPERTY_STRING(Sentence_Break, LF);
    TEST_PROPERTY_STRING(Sentence_Break, Lower);
    TEST_PROPERTY_STRING(Sentence_Break, Numeric);
    TEST_PROPERTY_STRING(Sentence_Break, OLetter);
    TEST_PROPERTY_STRING(Sentence_Break, SContinue);
    TEST_PROPERTY_STRING(Sentence_Break, Sep);
    TEST_PROPERTY_STRING(Sentence_Break, SOT);
    TEST_PROPERTY_STRING(Sentence_Break, Sp);
    TEST_PROPERTY_STRING(Sentence_Break, STerm);
    TEST_PROPERTY_STRING(Sentence_Break, Upper);

    TEST_PROPERTY_STRING(Word_Break, Other);
    TEST_PROPERTY_STRING(Word_Break, ALetter);
    TEST_PROPERTY_STRING(Word_Break, CR);
    TEST_PROPERTY_STRING(Word_Break, Double_Quote);
    TEST_PROPERTY_STRING(Word_Break, EOT);
    TEST_PROPERTY_STRING(Word_Break, Extend);
    TEST_PROPERTY_STRING(Word_Break, ExtendNumLet);
    TEST_PROPERTY_STRING(Word_Break, Format);
    TEST_PROPERTY_STRING(Word_Break, Hebrew_Letter);
    TEST_PROPERTY_STRING(Word_Break, Katakana);
    TEST_PROPERTY_STRING(Word_Break, LF);
    TEST_PROPERTY_STRING(Word_Break, MidLetter);
    TEST_PROPERTY_STRING(Word_Break, MidNum);
    TEST_PROPERTY_STRING(Word_Break, MidNumLet);
    TEST_PROPERTY_STRING(Word_Break, Newline);
    TEST_PROPERTY_STRING(Word_Break, Numeric);
    TEST_PROPERTY_STRING(Word_Break, Regional_Indicator);
    TEST_PROPERTY_STRING(Word_Break, Single_Quote);
    TEST_PROPERTY_STRING(Word_Break, SOT);

    TEST_EQUAL(to_str(east_asian_width(0)), "East_Asian_Width::N");
    TEST_EQUAL(to_str(east_asian_width(0xa1)), "East_Asian_Width::A");
    TEST_EQUAL(to_str(east_asian_width(0x3000)), "East_Asian_Width::F");
    TEST_EQUAL(to_str(east_asian_width(0x20a9)), "East_Asian_Width::H");
    TEST_EQUAL(to_str(east_asian_width(0x20)), "East_Asian_Width::Na");
    TEST_EQUAL(to_str(east_asian_width(0x1100)), "East_Asian_Width::W");

    TEST_EQUAL(to_str(grapheme_cluster_break(0x10ffff)), "Grapheme_Cluster_Break::Other");
    TEST_EQUAL(to_str(grapheme_cluster_break(0xd)), "Grapheme_Cluster_Break::CR");
    TEST_EQUAL(to_str(grapheme_cluster_break(0)), "Grapheme_Cluster_Break::Control");
    TEST_EQUAL(to_str(grapheme_cluster_break(0x300)), "Grapheme_Cluster_Break::Extend");
    TEST_EQUAL(to_str(grapheme_cluster_break(0x1100)), "Grapheme_Cluster_Break::L");
    TEST_EQUAL(to_str(grapheme_cluster_break(0xa)), "Grapheme_Cluster_Break::LF");
    TEST_EQUAL(to_str(grapheme_cluster_break(0xac00)), "Grapheme_Cluster_Break::LV");
    TEST_EQUAL(to_str(grapheme_cluster_break(0xac01)), "Grapheme_Cluster_Break::LVT");
    TEST_EQUAL(to_str(grapheme_cluster_break(0x1f1e6)), "Grapheme_Cluster_Break::Regional_Indicator");
    TEST_EQUAL(to_str(grapheme_cluster_break(0x903)), "Grapheme_Cluster_Break::SpacingMark");
    TEST_EQUAL(to_str(grapheme_cluster_break(0x11a8)), "Grapheme_Cluster_Break::T");
    TEST_EQUAL(to_str(grapheme_cluster_break(0x1160)), "Grapheme_Cluster_Break::V");

    TEST_EQUAL(to_str(hangul_syllable_type(0)), "Hangul_Syllable_Type::NA");
    TEST_EQUAL(to_str(hangul_syllable_type(0x1100)), "Hangul_Syllable_Type::L");
    TEST_EQUAL(to_str(hangul_syllable_type(0xac00)), "Hangul_Syllable_Type::LV");
    TEST_EQUAL(to_str(hangul_syllable_type(0xac01)), "Hangul_Syllable_Type::LVT");
    TEST_EQUAL(to_str(hangul_syllable_type(0x11a8)), "Hangul_Syllable_Type::T");
    TEST_EQUAL(to_str(hangul_syllable_type(0x1160)), "Hangul_Syllable_Type::V");

    TEST_EQUAL(to_str(indic_positional_category(0)), "Indic_Positional_Category::NA");
    TEST_EQUAL(to_str(indic_positional_category(0x941)), "Indic_Positional_Category::Bottom");
    TEST_EQUAL(to_str(indic_positional_category(0x1b3b)), "Indic_Positional_Category::Bottom_And_Right");
    TEST_EQUAL(to_str(indic_positional_category(0x93f)), "Indic_Positional_Category::Left");
    TEST_EQUAL(to_str(indic_positional_category(0x9cb)), "Indic_Positional_Category::Left_And_Right");
    TEST_EQUAL(to_str(indic_positional_category(0x10a01)), "Indic_Positional_Category::Overstruck");
    TEST_EQUAL(to_str(indic_positional_category(0x93b)), "Indic_Positional_Category::Right");
    TEST_EQUAL(to_str(indic_positional_category(0x93a)), "Indic_Positional_Category::Top");
    TEST_EQUAL(to_str(indic_positional_category(0xc48)), "Indic_Positional_Category::Top_And_Bottom");
    TEST_EQUAL(to_str(indic_positional_category(0x1b3d)), "Indic_Positional_Category::Top_And_Bottom_And_Right");
    TEST_EQUAL(to_str(indic_positional_category(0xb48)), "Indic_Positional_Category::Top_And_Left");
    TEST_EQUAL(to_str(indic_positional_category(0xb4c)), "Indic_Positional_Category::Top_And_Left_And_Right");
    TEST_EQUAL(to_str(indic_positional_category(0xac9)), "Indic_Positional_Category::Top_And_Right");
    TEST_EQUAL(to_str(indic_positional_category(0xe40)), "Indic_Positional_Category::Visual_Order_Left");

    TEST_EQUAL(to_str(indic_syllabic_category(0)), "Indic_Syllabic_Category::Other");
    TEST_EQUAL(to_str(indic_syllabic_category(0x93d)), "Indic_Syllabic_Category::Avagraha");
    TEST_EQUAL(to_str(indic_syllabic_category(0x900)), "Indic_Syllabic_Category::Bindu");
    TEST_EQUAL(to_str(indic_syllabic_category(0x11052)), "Indic_Syllabic_Category::Brahmi_Joining_Number");
    TEST_EQUAL(to_str(indic_syllabic_category(0xa8e0)), "Indic_Syllabic_Category::Cantillation_Mark");
    TEST_EQUAL(to_str(indic_syllabic_category(0x915)), "Indic_Syllabic_Category::Consonant");
    TEST_EQUAL(to_str(indic_syllabic_category(0x9ce)), "Indic_Syllabic_Category::Consonant_Dead");
    TEST_EQUAL(to_str(indic_syllabic_category(0x1930)), "Indic_Syllabic_Category::Consonant_Final");
    TEST_EQUAL(to_str(indic_syllabic_category(0xf88)), "Indic_Syllabic_Category::Consonant_Head_Letter");
    TEST_EQUAL(to_str(indic_syllabic_category(0xa75)), "Indic_Syllabic_Category::Consonant_Medial");
    TEST_EQUAL(to_str(indic_syllabic_category(0x2d)), "Indic_Syllabic_Category::Consonant_Placeholder");
    TEST_EQUAL(to_str(indic_syllabic_category(0xd4e)), "Indic_Syllabic_Category::Consonant_Preceding_Repha");
    TEST_EQUAL(to_str(indic_syllabic_category(0xf8d)), "Indic_Syllabic_Category::Consonant_Subjoined");
    TEST_EQUAL(to_str(indic_syllabic_category(0x17cc)), "Indic_Syllabic_Category::Consonant_Succeeding_Repha");
    TEST_EQUAL(to_str(indic_syllabic_category(0xa71)), "Indic_Syllabic_Category::Gemination_Mark");
    TEST_EQUAL(to_str(indic_syllabic_category(0x1039)), "Indic_Syllabic_Category::Invisible_Stacker");
    TEST_EQUAL(to_str(indic_syllabic_category(0x200d)), "Indic_Syllabic_Category::Joiner");
    TEST_EQUAL(to_str(indic_syllabic_category(0xb83)), "Indic_Syllabic_Category::Modifying_Letter");
    TEST_EQUAL(to_str(indic_syllabic_category(0x200c)), "Indic_Syllabic_Category::Non_Joiner");
    TEST_EQUAL(to_str(indic_syllabic_category(0x93c)), "Indic_Syllabic_Category::Nukta");
    TEST_EQUAL(to_str(indic_syllabic_category(0x30)), "Indic_Syllabic_Category::Number");
    TEST_EQUAL(to_str(indic_syllabic_category(0x1107f)), "Indic_Syllabic_Category::Number_Joiner");
    TEST_EQUAL(to_str(indic_syllabic_category(0xe3a)), "Indic_Syllabic_Category::Pure_Killer");
    TEST_EQUAL(to_str(indic_syllabic_category(0x17c9)), "Indic_Syllabic_Category::Register_Shifter");
    TEST_EQUAL(to_str(indic_syllabic_category(0x1970)), "Indic_Syllabic_Category::Tone_Letter");
    TEST_EQUAL(to_str(indic_syllabic_category(0x94d)), "Indic_Syllabic_Category::Virama");
    TEST_EQUAL(to_str(indic_syllabic_category(0x903)), "Indic_Syllabic_Category::Visarga");
    TEST_EQUAL(to_str(indic_syllabic_category(0x1963)), "Indic_Syllabic_Category::Vowel");
    TEST_EQUAL(to_str(indic_syllabic_category(0x93a)), "Indic_Syllabic_Category::Vowel_Dependent");
    TEST_EQUAL(to_str(indic_syllabic_category(0x904)), "Indic_Syllabic_Category::Vowel_Independent");

    TEST_EQUAL(to_str(line_break(0xe000)), "Line_Break::XX");
    TEST_EQUAL(to_str(line_break(0x10ffff)), "Line_Break::XX");
    TEST_EQUAL(to_str(line_break(0xa7)), "Line_Break::AI");
    TEST_EQUAL(to_str(line_break(0x23)), "Line_Break::AL");
    TEST_EQUAL(to_str(line_break(0x2014)), "Line_Break::B2");
    TEST_EQUAL(to_str(line_break(0x9)), "Line_Break::BA");
    TEST_EQUAL(to_str(line_break(0xb4)), "Line_Break::BB");
    TEST_EQUAL(to_str(line_break(0xb)), "Line_Break::BK");
    TEST_EQUAL(to_str(line_break(0xfffc)), "Line_Break::CB");
    TEST_EQUAL(to_str(line_break(0x3041)), "Line_Break::CJ");
    TEST_EQUAL(to_str(line_break(0x7d)), "Line_Break::CL");
    TEST_EQUAL(to_str(line_break(0)), "Line_Break::CM");
    TEST_EQUAL(to_str(line_break(0x29)), "Line_Break::CP");
    TEST_EQUAL(to_str(line_break(0xd)), "Line_Break::CR");
    TEST_EQUAL(to_str(line_break(0x21)), "Line_Break::EX");
    TEST_EQUAL(to_str(line_break(0xa0)), "Line_Break::GL");
    TEST_EQUAL(to_str(line_break(0xac00)), "Line_Break::H2");
    TEST_EQUAL(to_str(line_break(0xac01)), "Line_Break::H3");
    TEST_EQUAL(to_str(line_break(0x5d0)), "Line_Break::HL");
    TEST_EQUAL(to_str(line_break(0x2d)), "Line_Break::HY");
    TEST_EQUAL(to_str(line_break(0x231a)), "Line_Break::ID");
    TEST_EQUAL(to_str(line_break(0x2024)), "Line_Break::IN_");
    TEST_EQUAL(to_str(line_break(0x2c)), "Line_Break::IS");
    TEST_EQUAL(to_str(line_break(0x1100)), "Line_Break::JL");
    TEST_EQUAL(to_str(line_break(0x11a8)), "Line_Break::JT");
    TEST_EQUAL(to_str(line_break(0x1160)), "Line_Break::JV");
    TEST_EQUAL(to_str(line_break(0xa)), "Line_Break::LF");
    TEST_EQUAL(to_str(line_break(0x85)), "Line_Break::NL");
    TEST_EQUAL(to_str(line_break(0x17d6)), "Line_Break::NS");
    TEST_EQUAL(to_str(line_break(0x30)), "Line_Break::NU");
    TEST_EQUAL(to_str(line_break(0x28)), "Line_Break::OP");
    TEST_EQUAL(to_str(line_break(0x25)), "Line_Break::PO");
    TEST_EQUAL(to_str(line_break(0x24)), "Line_Break::PR");
    TEST_EQUAL(to_str(line_break(0x22)), "Line_Break::QU");
    TEST_EQUAL(to_str(line_break(0x1f1e6)), "Line_Break::RI");
    TEST_EQUAL(to_str(line_break(0xe01)), "Line_Break::SA");
    TEST_EQUAL(to_str(line_break(0xd800)), "Line_Break::SG");
    TEST_EQUAL(to_str(line_break(0x20)), "Line_Break::SP");
    TEST_EQUAL(to_str(line_break(0x2f)), "Line_Break::SY");
    TEST_EQUAL(to_str(line_break(0x2060)), "Line_Break::WJ");
    TEST_EQUAL(to_str(line_break(0x200b)), "Line_Break::ZW");

    TEST_EQUAL(to_str(numeric_type(0)), "Numeric_Type::None");
    TEST_EQUAL(to_str(numeric_type('A')), "Numeric_Type::None");
    TEST_EQUAL(to_str(numeric_type('0')), "Numeric_Type::Decimal");
    TEST_EQUAL(to_str(numeric_type(0xb9)), "Numeric_Type::Digit");
    TEST_EQUAL(to_str(numeric_type(0xbd)), "Numeric_Type::Numeric");

    TEST_EQUAL(to_str(sentence_break(0)), "Sentence_Break::Other");
    TEST_EQUAL(to_str(sentence_break(0x10ffff)), "Sentence_Break::Other");
    TEST_EQUAL(to_str(sentence_break(0x2e)), "Sentence_Break::ATerm");
    TEST_EQUAL(to_str(sentence_break(0xd)), "Sentence_Break::CR");
    TEST_EQUAL(to_str(sentence_break(0x22)), "Sentence_Break::Close");
    TEST_EQUAL(to_str(sentence_break(0x300)), "Sentence_Break::Extend");
    TEST_EQUAL(to_str(sentence_break(0xad)), "Sentence_Break::Format");
    TEST_EQUAL(to_str(sentence_break(0xa)), "Sentence_Break::LF");
    TEST_EQUAL(to_str(sentence_break(0x61)), "Sentence_Break::Lower");
    TEST_EQUAL(to_str(sentence_break(0x30)), "Sentence_Break::Numeric");
    TEST_EQUAL(to_str(sentence_break(0x1bb)), "Sentence_Break::OLetter");
    TEST_EQUAL(to_str(sentence_break(0x2c)), "Sentence_Break::SContinue");
    TEST_EQUAL(to_str(sentence_break(0x21)), "Sentence_Break::STerm");
    TEST_EQUAL(to_str(sentence_break(0x85)), "Sentence_Break::Sep");
    TEST_EQUAL(to_str(sentence_break(0x9)), "Sentence_Break::Sp");
    TEST_EQUAL(to_str(sentence_break(0x41)), "Sentence_Break::Upper");

    TEST_EQUAL(to_str(word_break(0)), "Word_Break::Other");
    TEST_EQUAL(to_str(word_break(0x10ffff)), "Word_Break::Other");
    TEST_EQUAL(to_str(word_break(0x41)), "Word_Break::ALetter");
    TEST_EQUAL(to_str(word_break(0xd)), "Word_Break::CR");
    TEST_EQUAL(to_str(word_break(0x22)), "Word_Break::Double_Quote");
    TEST_EQUAL(to_str(word_break(0x300)), "Word_Break::Extend");
    TEST_EQUAL(to_str(word_break(0x5f)), "Word_Break::ExtendNumLet");
    TEST_EQUAL(to_str(word_break(0xad)), "Word_Break::Format");
    TEST_EQUAL(to_str(word_break(0x5d0)), "Word_Break::Hebrew_Letter");
    TEST_EQUAL(to_str(word_break(0x3031)), "Word_Break::Katakana");
    TEST_EQUAL(to_str(word_break(0xa)), "Word_Break::LF");
    TEST_EQUAL(to_str(word_break(0x3a)), "Word_Break::MidLetter");
    TEST_EQUAL(to_str(word_break(0x2c)), "Word_Break::MidNum");
    TEST_EQUAL(to_str(word_break(0x2e)), "Word_Break::MidNumLet");
    TEST_EQUAL(to_str(word_break(0xb)), "Word_Break::Newline");
    TEST_EQUAL(to_str(word_break(0x30)), "Word_Break::Numeric");
    TEST_EQUAL(to_str(word_break(0x1f1e6)), "Word_Break::Regional_Indicator");
    TEST_EQUAL(to_str(word_break(0x27)), "Word_Break::Single_Quote");

}

void test_unicorn_character_numeric_properties() {

    { auto x = numeric_value(0);     TEST_EQUAL(x.first, 0);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('A');   TEST_EQUAL(x.first, 0);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('0');   TEST_EQUAL(x.first, 0);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('1');   TEST_EQUAL(x.first, 1);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('2');   TEST_EQUAL(x.first, 2);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('3');   TEST_EQUAL(x.first, 3);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('4');   TEST_EQUAL(x.first, 4);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('5');   TEST_EQUAL(x.first, 5);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('6');   TEST_EQUAL(x.first, 6);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('7');   TEST_EQUAL(x.first, 7);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('8');   TEST_EQUAL(x.first, 8);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value('9');   TEST_EQUAL(x.first, 9);  TEST_EQUAL(x.second, 1); }
    { auto x = numeric_value(0xbc);  TEST_EQUAL(x.first, 1);  TEST_EQUAL(x.second, 4); }
    { auto x = numeric_value(0xbd);  TEST_EQUAL(x.first, 1);  TEST_EQUAL(x.second, 2); }
    { auto x = numeric_value(0xbe);  TEST_EQUAL(x.first, 3);  TEST_EQUAL(x.second, 4); }

}

void test_unicorn_character_script_properties() {

    TEST_EQUAL(script_name("Grek"), "Greek");
    TEST_EQUAL(script_name("Latn"), "Latin");
    TEST_EQUAL(script_name("Zyyy"), "Common");
    TEST_EQUAL(script_name("Aaaa"), "");
    TEST_EQUAL(script_name("GREK"), "Greek");
    TEST_EQUAL(script_name("LATN"), "Latin");
    TEST_EQUAL(script_name("ZYYY"), "Common");
    TEST_EQUAL(script_name("AAAA"), "");
    TEST_EQUAL(script_name("grek"), "Greek");
    TEST_EQUAL(script_name("latn"), "Latin");
    TEST_EQUAL(script_name("zyyy"), "Common");
    TEST_EQUAL(script_name("aaaa"), "");

    TEST_EQUAL(char_script(0), "Zyyy");
    TEST_EQUAL(char_script(0x41), "Latn");
    TEST_EQUAL(char_script(0x370), "Grek");
    TEST_EQUAL(char_script(0x400), "Cyrs");
    TEST_EQUAL(char_script(0x531), "Armn");
    TEST_EQUAL(char_script(0x591), "Hebr");
    TEST_EQUAL(char_script(0x600), "Aran");
    TEST_EQUAL(char_script(0x10fffd), "Zzzz");

    TEST_EQUAL(script_name(char_script(0)), "Common");
    TEST_EQUAL(script_name(char_script(0x41)), "Latin");
    TEST_EQUAL(script_name(char_script(0x370)), "Greek");
    TEST_EQUAL(script_name(char_script(0x400)), "Cyrillic");
    TEST_EQUAL(script_name(char_script(0x531)), "Armenian");
    TEST_EQUAL(script_name(char_script(0x591)), "Hebrew");
    TEST_EQUAL(script_name(char_script(0x600)), "Arabic");
    TEST_EQUAL(script_name(char_script(0x10fffd)), "Unknown");

    TEST_EQUAL(to_str(char_script_list(0)), "[Zyyy]");
    TEST_EQUAL(to_str(char_script_list(0x41)), "[Latn]");
    TEST_EQUAL(to_str(char_script_list(0x10fffd)), "[Zzzz]");
    TEST_EQUAL(to_str(char_script_list(0x1cd0)), "[Deva,Gran]");
    TEST_EQUAL(to_str(char_script_list(0x342)), "[Grek]");
    TEST_EQUAL(to_str(char_script_list(0x363)), "[Latn]");
    TEST_EQUAL(to_str(char_script_list(0x64b)), "[Arab,Syrc]");
    TEST_EQUAL(to_str(char_script_list(0x660)), "[Arab,Thaa]");
    TEST_EQUAL(to_str(char_script_list(0x589)), "[Armn,Geor]");
    TEST_EQUAL(to_str(char_script_list(0x640)), "[Arab,Mand,Mani,Phlp,Syrc]");
    TEST_EQUAL(to_str(char_script_list(0x964)), "[Beng,Deva,Gran,Gujr,Guru,Knda,Mahj,Mlym,Orya,Sind,"
                                                "Sinh,Sylo,Takr,Taml,Telu,Tirh]");

}

void test_unicorn_character_test_all_the_things() {

    for (char32_t c = 0; c <= 0x110000; ++c)
        char_is_latin1(c);

    #define CALL_ALL_THE_PROPERTIES(func) \
        for (char32_t c = 0; c <= 0x110000; ++c) \
            func(c);

    CALL_ALL_THE_PROPERTIES(char_is_latin1);
    CALL_ALL_THE_PROPERTIES(char_is_surrogate);
    CALL_ALL_THE_PROPERTIES(char_is_bmp);
    CALL_ALL_THE_PROPERTIES(char_is_astral);
    CALL_ALL_THE_PROPERTIES(char_is_unicode);
    CALL_ALL_THE_PROPERTIES(char_is_high_surrogate);
    CALL_ALL_THE_PROPERTIES(char_is_low_surrogate);
    CALL_ALL_THE_PROPERTIES(char_is_noncharacter);
    CALL_ALL_THE_PROPERTIES(char_is_private_use);
    CALL_ALL_THE_PROPERTIES(char_general_category);
    CALL_ALL_THE_PROPERTIES(char_primary_category);
    CALL_ALL_THE_PROPERTIES(char_is_format);
    CALL_ALL_THE_PROPERTIES(char_is_control);
    CALL_ALL_THE_PROPERTIES(char_is_letter);
    CALL_ALL_THE_PROPERTIES(char_is_mark);
    CALL_ALL_THE_PROPERTIES(char_is_number);
    CALL_ALL_THE_PROPERTIES(char_is_punctuation);
    CALL_ALL_THE_PROPERTIES(char_is_symbol);
    CALL_ALL_THE_PROPERTIES(char_is_separator);
    CALL_ALL_THE_PROPERTIES(char_is_assigned);
    CALL_ALL_THE_PROPERTIES(char_is_unassigned);
    CALL_ALL_THE_PROPERTIES(char_is_white_space);
    CALL_ALL_THE_PROPERTIES(char_is_line_break);
    CALL_ALL_THE_PROPERTIES(char_is_inline_space);
    CALL_ALL_THE_PROPERTIES(char_is_id_start);
    CALL_ALL_THE_PROPERTIES(char_is_id_nonstart);
    CALL_ALL_THE_PROPERTIES(char_is_id_continue);
    CALL_ALL_THE_PROPERTIES(char_is_xid_start);
    CALL_ALL_THE_PROPERTIES(char_is_xid_nonstart);
    CALL_ALL_THE_PROPERTIES(char_is_xid_continue);
    CALL_ALL_THE_PROPERTIES(char_is_pattern_syntax);
    CALL_ALL_THE_PROPERTIES(char_is_pattern_white_space);
    CALL_ALL_THE_PROPERTIES(char_is_default_ignorable);
    CALL_ALL_THE_PROPERTIES(char_is_soft_dotted);
    CALL_ALL_THE_PROPERTIES(joining_group);
    CALL_ALL_THE_PROPERTIES(joining_type);
    CALL_ALL_THE_PROPERTIES(bidi_class);
    CALL_ALL_THE_PROPERTIES(char_is_bidi_mirrored);
    CALL_ALL_THE_PROPERTIES(bidi_mirroring_glyph);
    CALL_ALL_THE_PROPERTIES(bidi_paired_bracket);
    CALL_ALL_THE_PROPERTIES(bidi_paired_bracket_type);
    CALL_ALL_THE_PROPERTIES(char_is_uppercase);
    CALL_ALL_THE_PROPERTIES(char_is_lowercase);
    CALL_ALL_THE_PROPERTIES(char_is_titlecase);
    CALL_ALL_THE_PROPERTIES(char_is_cased);
    CALL_ALL_THE_PROPERTIES(char_is_case_ignorable);
    CALL_ALL_THE_PROPERTIES(char_to_simple_uppercase);
    CALL_ALL_THE_PROPERTIES(char_to_simple_lowercase);
    CALL_ALL_THE_PROPERTIES(char_to_simple_titlecase);
    CALL_ALL_THE_PROPERTIES(char_to_simple_casefold);
    CALL_ALL_THE_PROPERTIES(combining_class);
    CALL_ALL_THE_PROPERTIES(east_asian_width);
    CALL_ALL_THE_PROPERTIES(grapheme_cluster_break);
    CALL_ALL_THE_PROPERTIES(hangul_syllable_type);
    CALL_ALL_THE_PROPERTIES(indic_positional_category);
    CALL_ALL_THE_PROPERTIES(indic_syllabic_category);
    CALL_ALL_THE_PROPERTIES(line_break);
    CALL_ALL_THE_PROPERTIES(sentence_break);
    CALL_ALL_THE_PROPERTIES(word_break);
    CALL_ALL_THE_PROPERTIES(numeric_type);
    CALL_ALL_THE_PROPERTIES(numeric_value);

    char32_t buf[100];

    #define DECOMPOSE_ALL_THE_THINGS(func) \
        for (char32_t c = 0; c <= 0x110000; ++c) \
            func(c, buf);

    DECOMPOSE_ALL_THE_THINGS(char_to_full_uppercase);
    DECOMPOSE_ALL_THE_THINGS(char_to_full_lowercase);
    DECOMPOSE_ALL_THE_THINGS(char_to_full_titlecase);
    DECOMPOSE_ALL_THE_THINGS(char_to_full_casefold);
    DECOMPOSE_ALL_THE_THINGS(canonical_decomposition);
    DECOMPOSE_ALL_THE_THINGS(compatibility_decomposition);

}
