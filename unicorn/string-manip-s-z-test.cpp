#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_manip_split() {

    Strings v;

    TRY(str_split(""s, overwrite(v)));                      TEST_EQUAL(v.size(), 0);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(str_split("Hello"s, overwrite(v)));                 TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "Hello");
    TRY(str_split("Hello world"s, overwrite(v)));           TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");
    TRY(str_split("\t Hello \t world \t"s, overwrite(v)));  TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");

    TRY(str_split_at(""s, overwrite(v), "<>"));                        TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(str_split_at("<>"s, overwrite(v), "<>"));                      TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "/");
    TRY(str_split_at("<><>"s, overwrite(v), "<>"));                    TEST_EQUAL(v.size(), 3);  TEST_EQUAL(str_join(v, "/"), "//");
    TRY(str_split_at("Hello"s, overwrite(v), "<>"));                   TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "Hello");
    TRY(str_split_at("<>Hello<>"s, overwrite(v), "<>"));               TEST_EQUAL(v.size(), 3);  TEST_EQUAL(str_join(v, "/"), "/Hello/");
    TRY(str_split_at("<><>Hello<><>"s, overwrite(v), "<>"));           TEST_EQUAL(v.size(), 5);  TEST_EQUAL(str_join(v, "/"), "//Hello//");
    TRY(str_split_at("Hello<>world"s, overwrite(v), "<>"));            TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");
    TRY(str_split_at("<>Hello<>world<>"s, overwrite(v), "<>"));        TEST_EQUAL(v.size(), 4);  TEST_EQUAL(str_join(v, "/"), "/Hello/world/");
    TRY(str_split_at("<><>Hello<><>world<><>"s, overwrite(v), "<>"));  TEST_EQUAL(v.size(), 7);  TEST_EQUAL(str_join(v, "/"), "//Hello//world//");

    TRY(str_split_by("**Hello**world**"s, overwrite(v), "*"));        TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");
    TRY(str_split_by("**Hello**world**"s, overwrite(v), "*"s));       TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");
    TRY(str_split_by("*****"s, overwrite(v), "@*"));                  TEST_EQUAL(v.size(), 0);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(str_split_by("*****"s, overwrite(v), "@*"s));                 TEST_EQUAL(v.size(), 0);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(str_split_by("“”,“€uro”,“∈lement”"s, overwrite(v), "“”,"));   TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "€uro/∈lement");
    TRY(str_split_by("“”,“€uro”,“∈lement”"s, overwrite(v), "“”,"s));  TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "€uro/∈lement");

    TRY(v = str_splitv(""s));                      TEST_EQUAL(v.size(), 0);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(v = str_splitv("Hello"s));                 TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "Hello");
    TRY(v = str_splitv("Hello world"s));           TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");
    TRY(v = str_splitv("\t Hello \t world \t"s));  TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");

    TRY(v = str_splitv_at(""s, "<>"));                        TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(v = str_splitv_at("<>"s, "<>"));                      TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "/");
    TRY(v = str_splitv_at("<><>"s, "<>"));                    TEST_EQUAL(v.size(), 3);  TEST_EQUAL(str_join(v, "/"), "//");
    TRY(v = str_splitv_at("Hello"s, "<>"));                   TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "Hello");
    TRY(v = str_splitv_at("<>Hello<>"s, "<>"));               TEST_EQUAL(v.size(), 3);  TEST_EQUAL(str_join(v, "/"), "/Hello/");
    TRY(v = str_splitv_at("<><>Hello<><>"s, "<>"));           TEST_EQUAL(v.size(), 5);  TEST_EQUAL(str_join(v, "/"), "//Hello//");
    TRY(v = str_splitv_at("Hello<>world"s, "<>"));            TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");
    TRY(v = str_splitv_at("<>Hello<>world<>"s, "<>"));        TEST_EQUAL(v.size(), 4);  TEST_EQUAL(str_join(v, "/"), "/Hello/world/");
    TRY(v = str_splitv_at("<><>Hello<><>world<><>"s, "<>"));  TEST_EQUAL(v.size(), 7);  TEST_EQUAL(str_join(v, "/"), "//Hello//world//");

    TRY(v = str_splitv_by("**Hello**world**"s, "*"));        TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");
    TRY(v = str_splitv_by("**Hello**world**"s, "*"s));       TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "Hello/world");
    TRY(v = str_splitv_by("*****"s, "@*"));                  TEST_EQUAL(v.size(), 0);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(v = str_splitv_by("*****"s, "@*"s));                 TEST_EQUAL(v.size(), 0);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(v = str_splitv_by("“”,“€uro”,“∈lement”"s, "“”,"));   TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "€uro/∈lement");
    TRY(v = str_splitv_by("“”,“€uro”,“∈lement”"s, "“”,"s));  TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "€uro/∈lement");

    TRY(str_split_lines(""s, overwrite(v)));              TEST_EQUAL(v.size(), 0);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(str_split_lines("\n"s, overwrite(v)));            TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(str_split_lines("\n\n"s, overwrite(v)));          TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "/");
    TRY(str_split_lines("\n\n\n"s, overwrite(v)));        TEST_EQUAL(v.size(), 3);  TEST_EQUAL(str_join(v, "/"), "//");
    TRY(str_split_lines("\r\n"s, overwrite(v)));          TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(str_split_lines("\r\n\r\n"s, overwrite(v)));      TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "/");
    TRY(str_split_lines("\r\n\r\n\r\n"s, overwrite(v)));  TEST_EQUAL(v.size(), 3);  TEST_EQUAL(str_join(v, "/"), "//");

    TRY(v = str_splitv_lines(""s));              TEST_EQUAL(v.size(), 0);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(v = str_splitv_lines("\n"s));            TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(v = str_splitv_lines("\n\n"s));          TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "/");
    TRY(v = str_splitv_lines("\n\n\n"s));        TEST_EQUAL(v.size(), 3);  TEST_EQUAL(str_join(v, "/"), "//");
    TRY(v = str_splitv_lines("\r\n"s));          TEST_EQUAL(v.size(), 1);  TEST_EQUAL(str_join(v, "/"), "");
    TRY(v = str_splitv_lines("\r\n\r\n"s));      TEST_EQUAL(v.size(), 2);  TEST_EQUAL(str_join(v, "/"), "/");
    TRY(v = str_splitv_lines("\r\n\r\n\r\n"s));  TEST_EQUAL(v.size(), 3);  TEST_EQUAL(str_join(v, "/"), "//");

    Ustring text =
        "Line one\n"
        "Line two\r"
        "Line three\r\n"
        "Line four\f"
        "Line five\u0085"
        "Line six\u2028"
        "Line seven\u2029";
    TRY(str_split_lines(text, overwrite(v)));
    TEST_EQUAL(v.size(), 7);
    TEST_EQUAL(str_join(v, "/"),
        "Line one/"
        "Line two/"
        "Line three/"
        "Line four/"
        "Line five/"
        "Line six/"
        "Line seven");
    text =
        "Line one\n\n"
        "Line two\r\r"
        "Line three\r\n\r\n"
        "Line four\f\f"
        "Line five\u0085\u0085"
        "Line six\u2028\u2028"
        "Line seven\u2029\u2029";
    TRY(str_split_lines(text, overwrite(v)));
    TEST_EQUAL(v.size(), 14);
    TEST_EQUAL(str_join(v, "/"),
        "Line one//"
        "Line two//"
        "Line three//"
        "Line four//"
        "Line five//"
        "Line six//"
        "Line seven/");

}

void test_unicorn_string_manip_squeeze() {

    Ustring s;

    TEST_EQUAL(str_squeeze(""s), ""s);
    TEST_EQUAL(str_squeeze("\t\t\t"s), " "s);
    TEST_EQUAL(str_squeeze("Hello world"s), "Hello world"s);
    TEST_EQUAL(str_squeeze("\t\t\tHello\t\t\tworld\t\t\t"s), " Hello world "s);
    TEST_EQUAL(str_squeeze("€uro ∈lement"s), "€uro ∈lement"s);
    TEST_EQUAL(str_squeeze("\u2028\u2028€uro\u2028\u2028∈lement\u2028\u2028"s), " €uro ∈lement "s);
    TEST_EQUAL(str_squeeze(""s, "+-*/"s), ""s);
    TEST_EQUAL(str_squeeze("/*-+"s, "+-*/"s), "+"s);
    TEST_EQUAL(str_squeeze("Hello world"s, "+-*/"s), "Hello world"s);
    TEST_EQUAL(str_squeeze("/*-+Hello/*-+world/*-+"s, "+-*/"s), "+Hello+world+"s);
    TEST_EQUAL(str_squeeze("∇∃∀€uro∇∃∀∈lement∇∃∀"s, "∀∃∇"s), "∀€uro∀∈lement∀"s);

    TEST_EQUAL(str_squeeze_trim(""s), ""s);
    TEST_EQUAL(str_squeeze_trim("\t\t\t"s), ""s);
    TEST_EQUAL(str_squeeze_trim("Hello world"s), "Hello world"s);
    TEST_EQUAL(str_squeeze_trim("\t\t\tHello\t\t\tworld\t\t\t"s), "Hello world"s);
    TEST_EQUAL(str_squeeze_trim("€uro ∈lement"s), "€uro ∈lement"s);
    TEST_EQUAL(str_squeeze_trim("\u2028\u2028€uro\u2028\u2028∈lement\u2028\u2028"s), "€uro ∈lement"s);
    TEST_EQUAL(str_squeeze_trim(""s, "+-*/"s), ""s);
    TEST_EQUAL(str_squeeze_trim("/*-+"s, "+-*/"s), ""s);
    TEST_EQUAL(str_squeeze_trim("Hello world"s, "+-*/"s), "Hello world"s);
    TEST_EQUAL(str_squeeze_trim("/*-+Hello/*-+world/*-+"s, "+-*/"s), "Hello+world"s);
    TEST_EQUAL(str_squeeze_trim("∇∃∀€uro∇∃∀∈lement∇∃∀"s, "∀∃∇"s), "€uro∀∈lement"s);

    s = ""s;                                                 TRY(str_squeeze_in(s));           TEST_EQUAL(s, ""s);
    s = "\t\t\t"s;                                           TRY(str_squeeze_in(s));           TEST_EQUAL(s, " "s);
    s = "Hello world"s;                                      TRY(str_squeeze_in(s));           TEST_EQUAL(s, "Hello world"s);
    s = "\t\t\tHello\t\t\tworld\t\t\t"s;                     TRY(str_squeeze_in(s));           TEST_EQUAL(s, " Hello world "s);
    s = "€uro ∈lement"s;                                     TRY(str_squeeze_in(s));           TEST_EQUAL(s, "€uro ∈lement"s);
    s = "\u2028\u2028€uro\u2028\u2028∈lement\u2028\u2028"s;  TRY(str_squeeze_in(s));           TEST_EQUAL(s, " €uro ∈lement "s);
    s = ""s;                                                 TRY(str_squeeze_in(s, "+-*/"s));  TEST_EQUAL(s, ""s);
    s = "/*-+"s;                                             TRY(str_squeeze_in(s, "+-*/"s));  TEST_EQUAL(s, "+"s);
    s = "Hello world"s;                                      TRY(str_squeeze_in(s, "+-*/"s));  TEST_EQUAL(s, "Hello world"s);
    s = "/*-+Hello/*-+world/*-+"s;                           TRY(str_squeeze_in(s, "+-*/"s));  TEST_EQUAL(s, "+Hello+world+"s);
    s = "∇∃∀€uro∇∃∀∈lement∇∃∀"s;                             TRY(str_squeeze_in(s, "∀∃∇"s));   TEST_EQUAL(s, "∀€uro∀∈lement∀"s);

    s = ""s;                                                 TRY(str_squeeze_trim_in(s));           TEST_EQUAL(s, ""s);
    s = "\t\t\t"s;                                           TRY(str_squeeze_trim_in(s));           TEST_EQUAL(s, ""s);
    s = "Hello world"s;                                      TRY(str_squeeze_trim_in(s));           TEST_EQUAL(s, "Hello world"s);
    s = "\t\t\tHello\t\t\tworld\t\t\t"s;                     TRY(str_squeeze_trim_in(s));           TEST_EQUAL(s, "Hello world"s);
    s = "€uro ∈lement"s;                                     TRY(str_squeeze_trim_in(s));           TEST_EQUAL(s, "€uro ∈lement"s);
    s = "\u2028\u2028€uro\u2028\u2028∈lement\u2028\u2028"s;  TRY(str_squeeze_trim_in(s));           TEST_EQUAL(s, "€uro ∈lement"s);
    s = ""s;                                                 TRY(str_squeeze_trim_in(s, "+-*/"s));  TEST_EQUAL(s, ""s);
    s = "/*-+"s;                                             TRY(str_squeeze_trim_in(s, "+-*/"s));  TEST_EQUAL(s, ""s);
    s = "Hello world"s;                                      TRY(str_squeeze_trim_in(s, "+-*/"s));  TEST_EQUAL(s, "Hello world"s);
    s = "/*-+Hello/*-+world/*-+"s;                           TRY(str_squeeze_trim_in(s, "+-*/"s));  TEST_EQUAL(s, "Hello+world"s);
    s = "∇∃∀€uro∇∃∀∈lement∇∃∀"s;                             TRY(str_squeeze_trim_in(s, "∀∃∇"s));   TEST_EQUAL(s, "€uro∀∈lement"s);

}

void test_unicorn_string_manip_substring() {

    // UTF-32    UTF-16     UTF-8
    // 0000004d  004d       4d
    // 00000430  0430       d0 b0
    // 00004e8c  4e8c       e4 ba 8c
    // 00010302  d800 df02  f0 90 8c 82
    // 0010fffd  dbff dffd  f4 8f bf bd

    const Ustring example {"\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};

    TEST_EQUAL(str_substring(""s, 0), "");
    TEST_EQUAL(str_substring(""s, 5), "");
    TEST_EQUAL(str_substring(""s, 0, 0), "");
    TEST_EQUAL(str_substring(""s, 0, 5), "");
    TEST_EQUAL(str_substring(""s, 5, 0), "");
    TEST_EQUAL(str_substring(""s, 5, 5), "");
    TEST_EQUAL(str_substring("Hello world"s, 0), "Hello world");
    TEST_EQUAL(str_substring("Hello world"s, 6), "world");
    TEST_EQUAL(str_substring("Hello world"s, 20), "");
    TEST_EQUAL(str_substring("Hello world"s, 0, 0), "");
    TEST_EQUAL(str_substring("Hello world"s, 0, 5), "Hello");
    TEST_EQUAL(str_substring("Hello world"s, 0, 20), "Hello world");
    TEST_EQUAL(str_substring("Hello world"s, 6, 0), "");
    TEST_EQUAL(str_substring("Hello world"s, 6, 5), "world");
    TEST_EQUAL(str_substring("Hello world"s, 6, 20), "world");
    TEST_EQUAL(str_substring("Hello world"s, 20, 0), "");
    TEST_EQUAL(str_substring("Hello world"s, 20, 20), "");

    TEST_EQUAL(utf_substring(""s, 0), "");
    TEST_EQUAL(utf_substring(""s, 5), "");
    TEST_EQUAL(utf_substring(""s, 0, 0), "");
    TEST_EQUAL(utf_substring(""s, 0, 5), "");
    TEST_EQUAL(utf_substring(""s, 5, 0), "");
    TEST_EQUAL(utf_substring(""s, 5, 5), "");
    TEST_EQUAL(utf_substring("Hello world"s, 0), "Hello world");
    TEST_EQUAL(utf_substring("Hello world"s, 6), "world");
    TEST_EQUAL(utf_substring("Hello world"s, 20), "");
    TEST_EQUAL(utf_substring("Hello world"s, 0, 0), "");
    TEST_EQUAL(utf_substring("Hello world"s, 0, 5), "Hello");
    TEST_EQUAL(utf_substring("Hello world"s, 0, 20), "Hello world");
    TEST_EQUAL(utf_substring("Hello world"s, 6, 0), "");
    TEST_EQUAL(utf_substring("Hello world"s, 6, 5), "world");
    TEST_EQUAL(utf_substring("Hello world"s, 6, 20), "world");
    TEST_EQUAL(utf_substring("Hello world"s, 20, 0), "");
    TEST_EQUAL(utf_substring("Hello world"s, 20, 20), "");
    TEST_EQUAL(utf_substring(example, 0), "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 1), "\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 2), "\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 3), "\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 4), "");
    TEST_EQUAL(utf_substring(example, 5), "");
    TEST_EQUAL(utf_substring(example, 0, 0), "");
    TEST_EQUAL(utf_substring(example, 0, 1), "\xd0\xb0");
    TEST_EQUAL(utf_substring(example, 0, 2), "\xd0\xb0\xe4\xba\x8c");
    TEST_EQUAL(utf_substring(example, 0, 3), "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82");
    TEST_EQUAL(utf_substring(example, 0, 4), "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 0, 5), "\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 1, 0), "");
    TEST_EQUAL(utf_substring(example, 1, 1), "\xe4\xba\x8c");
    TEST_EQUAL(utf_substring(example, 1, 2), "\xe4\xba\x8c\xf0\x90\x8c\x82");
    TEST_EQUAL(utf_substring(example, 1, 3), "\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 1, 4), "\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 2, 0), "");
    TEST_EQUAL(utf_substring(example, 2, 1), "\xf0\x90\x8c\x82");
    TEST_EQUAL(utf_substring(example, 2, 2), "\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 2, 3), "\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 3, 0), "");
    TEST_EQUAL(utf_substring(example, 3, 1), "\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 3, 2), "\xf4\x8f\xbf\xbd");
    TEST_EQUAL(utf_substring(example, 4, 0), "");
    TEST_EQUAL(utf_substring(example, 4, 1), "");
    TEST_EQUAL(utf_substring(example, 5, 0), "");
    TEST_EQUAL(utf_substring(example, 5, 1), "");

    TEST_EQUAL(utf_substring("áéíóú"s, 1, 0, Length::graphemes), "");
    TEST_EQUAL(utf_substring("áéíóú"s, 1, 1, Length::graphemes), "é");
    TEST_EQUAL(utf_substring("áéíóú"s, 1, 2, Length::graphemes), "éí");
    TEST_EQUAL(utf_substring("áéíóú"s, 1, 3, Length::graphemes), "éíó");
    TEST_EQUAL(utf_substring("áéíóú"s, 1, 4, Length::graphemes), "éíóú");
    TEST_EQUAL(utf_substring("áéíóú"s, 1, 5, Length::graphemes), "éíóú");
    TEST_EQUAL(utf_substring("áéíóú"s, 1, npos, Length::graphemes), "éíóú");
    TEST_EQUAL(utf_substring("áéíóú"s, 5, npos, Length::graphemes), "");
    TEST_EQUAL(utf_substring("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 0, Length::graphemes), "");
    TEST_EQUAL(utf_substring("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 1, Length::graphemes), "e\u0301");
    TEST_EQUAL(utf_substring("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 2, Length::graphemes), "e\u0301i\u0301");
    TEST_EQUAL(utf_substring("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 3, Length::graphemes), "e\u0301i\u0301o\u0301");
    TEST_EQUAL(utf_substring("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 4, Length::graphemes), "e\u0301i\u0301o\u0301u\u0301");
    TEST_EQUAL(utf_substring("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 5, Length::graphemes), "e\u0301i\u0301o\u0301u\u0301");
    TEST_EQUAL(utf_substring("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, npos, Length::graphemes), "e\u0301i\u0301o\u0301u\u0301");
    TEST_EQUAL(utf_substring("a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, npos, Length::graphemes), "");

}

void test_unicorn_string_manip_translate() {

    Ustring s;

    TEST_EQUAL(str_translate(""s, "", ""), "");
    TEST_EQUAL(str_translate("Hello world"s, "", ""), "Hello world");
    TEST_EQUAL(str_translate("Hello world"s, "", "abcde"), "Hello world");
    TEST_EQUAL(str_translate("Hello world"s, "abcde", ""), "Hello world");
    TEST_EQUAL(str_translate("Hello world"s, "abcde", "12345"), "H5llo worl4");
    TEST_EQUAL(str_translate("Hello world"s, "abcde", "*"), "H*llo worl*");
    TEST_EQUAL(str_translate("€uro ∈lement"s, "€∈", "∇√"), "∇uro √lement");
    TEST_EQUAL(str_translate("€uro ∈lement"s, "€∈", "*"), "*uro *lement");
    TEST_EQUAL(str_translate("€uro ∈lement"s, "e", "∇√"), "€uro ∈l∇m∇nt");

    s = "";              TRY(str_translate_in(s, "", ""));            TEST_EQUAL(s, "");
    s = "Hello world";   TRY(str_translate_in(s, "", ""));            TEST_EQUAL(s, "Hello world");
    s = "Hello world";   TRY(str_translate_in(s, "", "abcde"));       TEST_EQUAL(s, "Hello world");
    s = "Hello world";   TRY(str_translate_in(s, "abcde", ""));       TEST_EQUAL(s, "Hello world");
    s = "Hello world";   TRY(str_translate_in(s, "abcde", "12345"));  TEST_EQUAL(s, "H5llo worl4");
    s = "Hello world";   TRY(str_translate_in(s, "abcde", "*"));      TEST_EQUAL(s, "H*llo worl*");
    s = "€uro ∈lement";  TRY(str_translate_in(s, "€∈", "∇√"));        TEST_EQUAL(s, "∇uro √lement");
    s = "€uro ∈lement";  TRY(str_translate_in(s, "€∈", "*"));         TEST_EQUAL(s, "*uro *lement");
    s = "€uro ∈lement";  TRY(str_translate_in(s, "e", "∇√"));         TEST_EQUAL(s, "€uro ∈l∇m∇nt");

}

void test_unicorn_string_manip_trim() {

    Ustring s;

    TEST_EQUAL(str_trim(""s), "");
    TEST_EQUAL(str_trim("Hello"s), "Hello");
    TEST_EQUAL(str_trim("Hello world"s), "Hello world");
    TEST_EQUAL(str_trim(""s, ""), "");
    TEST_EQUAL(str_trim("Hello"s, ""), "Hello");
    TEST_EQUAL(str_trim("<<<>>>"s, "<>"), "");
    TEST_EQUAL(str_trim("<<<Hello>>>"s, "<>"), "Hello");
    TEST_EQUAL(str_trim("<<<Hello>>> <<<world>>>"s, "<>"), "Hello>>> <<<world");
    TEST_EQUAL(str_trim("≤≤≤≥≥≥"s, "≤≥"), "");
    TEST_EQUAL(str_trim("≤≤≤€uro≥≥≥"s, "≤≥"), "€uro");
    TEST_EQUAL(str_trim("≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, "≤≥"), "€uro≥≥≥ ≤≤≤∈lement");

    s = "";                          TRY(str_trim_in(s));          TEST_EQUAL(s, "");
    s = "Hello";                     TRY(str_trim_in(s));          TEST_EQUAL(s, "Hello");
    s = "Hello world";               TRY(str_trim_in(s));          TEST_EQUAL(s, "Hello world");
    s = "";                          TRY(str_trim_in(s, ""));    TEST_EQUAL(s, "");
    s = "Hello";                     TRY(str_trim_in(s, ""));    TEST_EQUAL(s, "Hello");
    s = "<<<>>>";                    TRY(str_trim_in(s, "<>"));  TEST_EQUAL(s, "");
    s = "<<<Hello>>>";               TRY(str_trim_in(s, "<>"));  TEST_EQUAL(s, "Hello");
    s = "<<<Hello>>> <<<world>>>";   TRY(str_trim_in(s, "<>"));  TEST_EQUAL(s, "Hello>>> <<<world");
    s = "≤≤≤≥≥≥";                    TRY(str_trim_in(s, "≤≥"));  TEST_EQUAL(s, "");
    s = "≤≤≤€uro≥≥≥";                TRY(str_trim_in(s, "≤≥"));  TEST_EQUAL(s, "€uro");
    s = "≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_in(s, "≤≥"));  TEST_EQUAL(s, "€uro≥≥≥ ≤≤≤∈lement");

    TEST_EQUAL(str_trim_left(""s), "");
    TEST_EQUAL(str_trim_left("Hello"s), "Hello");
    TEST_EQUAL(str_trim_left("Hello world"s), "Hello world");
    TEST_EQUAL(str_trim_left(""s, ""), "");
    TEST_EQUAL(str_trim_left("Hello"s, ""), "Hello");
    TEST_EQUAL(str_trim_left("<<<>>>"s, "<>"), "");
    TEST_EQUAL(str_trim_left("<<<Hello>>>"s, "<>"), "Hello>>>");
    TEST_EQUAL(str_trim_left("<<<Hello>>> <<<world>>>"s, "<>"), "Hello>>> <<<world>>>");
    TEST_EQUAL(str_trim_left("≤≤≤≥≥≥"s, "≤≥"), "");
    TEST_EQUAL(str_trim_left("≤≤≤€uro≥≥≥"s, "≤≥"), "€uro≥≥≥");
    TEST_EQUAL(str_trim_left("≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, "≤≥"), "€uro≥≥≥ ≤≤≤∈lement≥≥≥");

    s = "";                          TRY(str_trim_left_in(s));          TEST_EQUAL(s, "");
    s = "Hello";                     TRY(str_trim_left_in(s));          TEST_EQUAL(s, "Hello");
    s = "Hello world";               TRY(str_trim_left_in(s));          TEST_EQUAL(s, "Hello world");
    s = "";                          TRY(str_trim_left_in(s, ""));    TEST_EQUAL(s, "");
    s = "Hello";                     TRY(str_trim_left_in(s, ""));    TEST_EQUAL(s, "Hello");
    s = "<<<>>>";                    TRY(str_trim_left_in(s, "<>"));  TEST_EQUAL(s, "");
    s = "<<<Hello>>>";               TRY(str_trim_left_in(s, "<>"));  TEST_EQUAL(s, "Hello>>>");
    s = "<<<Hello>>> <<<world>>>";   TRY(str_trim_left_in(s, "<>"));  TEST_EQUAL(s, "Hello>>> <<<world>>>");
    s = "≤≤≤≥≥≥";                    TRY(str_trim_left_in(s, "≤≥"));  TEST_EQUAL(s, "");
    s = "≤≤≤€uro≥≥≥";                TRY(str_trim_left_in(s, "≤≥"));  TEST_EQUAL(s, "€uro≥≥≥");
    s = "≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_left_in(s, "≤≥"));  TEST_EQUAL(s, "€uro≥≥≥ ≤≤≤∈lement≥≥≥");

    TEST_EQUAL(str_trim_right(""s), "");
    TEST_EQUAL(str_trim_right("Hello"s), "Hello");
    TEST_EQUAL(str_trim_right("Hello world"s), "Hello world");
    TEST_EQUAL(str_trim_right(""s, ""), "");
    TEST_EQUAL(str_trim_right("Hello"s, ""), "Hello");
    TEST_EQUAL(str_trim_right("<<<>>>"s, "<>"), "");
    TEST_EQUAL(str_trim_right("<<<Hello>>>"s, "<>"), "<<<Hello");
    TEST_EQUAL(str_trim_right("<<<Hello>>> <<<world>>>"s, "<>"), "<<<Hello>>> <<<world");
    TEST_EQUAL(str_trim_right("≤≤≤≥≥≥"s, "≤≥"), "");
    TEST_EQUAL(str_trim_right("≤≤≤€uro≥≥≥"s, "≤≥"), "≤≤≤€uro");
    TEST_EQUAL(str_trim_right("≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, "≤≥"), "≤≤≤€uro≥≥≥ ≤≤≤∈lement");

    s = "";                          TRY(str_trim_right_in(s));          TEST_EQUAL(s, "");
    s = "Hello";                     TRY(str_trim_right_in(s));          TEST_EQUAL(s, "Hello");
    s = "Hello world";               TRY(str_trim_right_in(s));          TEST_EQUAL(s, "Hello world");
    s = "";                          TRY(str_trim_right_in(s, ""));    TEST_EQUAL(s, "");
    s = "Hello";                     TRY(str_trim_right_in(s, ""));    TEST_EQUAL(s, "Hello");
    s = "<<<>>>";                    TRY(str_trim_right_in(s, "<>"));  TEST_EQUAL(s, "");
    s = "<<<Hello>>>";               TRY(str_trim_right_in(s, "<>"));  TEST_EQUAL(s, "<<<Hello");
    s = "<<<Hello>>> <<<world>>>";   TRY(str_trim_right_in(s, "<>"));  TEST_EQUAL(s, "<<<Hello>>> <<<world");
    s = "≤≤≤≥≥≥";                    TRY(str_trim_right_in(s, "≤≥"));  TEST_EQUAL(s, "");
    s = "≤≤≤€uro≥≥≥";                TRY(str_trim_right_in(s, "≤≥"));  TEST_EQUAL(s, "≤≤≤€uro");
    s = "≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_right_in(s, "≤≥"));  TEST_EQUAL(s, "≤≤≤€uro≥≥≥ ≤≤≤∈lement");

}

void test_unicorn_string_manip_trim_if() {

    Ustring s;

    TEST_EQUAL(str_trim_if(""s, char_is_line_break), "");
    TEST_EQUAL(str_trim_if("Hello"s, char_is_line_break), "Hello");
    TEST_EQUAL(str_trim_if("\r\nHello\r\n"s, char_is_line_break), "Hello");
    TEST_EQUAL(str_trim_left_if(""s, char_is_line_break), "");
    TEST_EQUAL(str_trim_left_if("Hello"s, char_is_line_break), "Hello");
    TEST_EQUAL(str_trim_left_if("\r\nHello\r\n"s, char_is_line_break), "Hello\r\n");
    TEST_EQUAL(str_trim_right_if(""s, char_is_line_break), "");
    TEST_EQUAL(str_trim_right_if("Hello"s, char_is_line_break), "Hello");
    TEST_EQUAL(str_trim_right_if("\r\nHello\r\n"s, char_is_line_break), "\r\nHello");
    TEST_EQUAL(str_trim_if_not(""s, char_is_line_break), "");
    TEST_EQUAL(str_trim_if_not("Hello\r\nworld"s, char_is_line_break), "\r\n");
    TEST_EQUAL(str_trim_if_not("\r\nHello world\r\n"s, char_is_line_break), "\r\nHello world\r\n");
    TEST_EQUAL(str_trim_left_if_not(""s, char_is_line_break), "");
    TEST_EQUAL(str_trim_left_if_not("Hello\r\nworld"s, char_is_line_break), "\r\nworld");
    TEST_EQUAL(str_trim_left_if_not("\r\nHello world\r\n"s, char_is_line_break), "\r\nHello world\r\n");
    TEST_EQUAL(str_trim_right_if_not(""s, char_is_line_break), "");
    TEST_EQUAL(str_trim_right_if_not("Hello\r\nworld"s, char_is_line_break), "Hello\r\n");
    TEST_EQUAL(str_trim_right_if_not("\r\nHello world\r\n"s, char_is_line_break), "\r\nHello world\r\n");

    s = "";                     TRY(str_trim_in_if(s, char_is_line_break));            TEST_EQUAL(s, "");
    s = "Hello";                TRY(str_trim_in_if(s, char_is_line_break));            TEST_EQUAL(s, "Hello");
    s = "\r\nHello\r\n";        TRY(str_trim_in_if(s, char_is_line_break));            TEST_EQUAL(s, "Hello");
    s = "";                     TRY(str_trim_left_in_if(s, char_is_line_break));       TEST_EQUAL(s, "");
    s = "Hello";                TRY(str_trim_left_in_if(s, char_is_line_break));       TEST_EQUAL(s, "Hello");
    s = "\r\nHello\r\n";        TRY(str_trim_left_in_if(s, char_is_line_break));       TEST_EQUAL(s, "Hello\r\n");
    s = "";                     TRY(str_trim_right_in_if(s, char_is_line_break));      TEST_EQUAL(s, "");
    s = "Hello";                TRY(str_trim_right_in_if(s, char_is_line_break));      TEST_EQUAL(s, "Hello");
    s = "\r\nHello\r\n";        TRY(str_trim_right_in_if(s, char_is_line_break));      TEST_EQUAL(s, "\r\nHello");
    s = "";                     TRY(str_trim_in_if_not(s, char_is_line_break));        TEST_EQUAL(s, "");
    s = "Hello\r\nworld";       TRY(str_trim_in_if_not(s, char_is_line_break));        TEST_EQUAL(s, "\r\n");
    s = "\r\nHello world\r\n";  TRY(str_trim_in_if_not(s, char_is_line_break));        TEST_EQUAL(s, "\r\nHello world\r\n");
    s = "";                     TRY(str_trim_left_in_if_not(s, char_is_line_break));   TEST_EQUAL(s, "");
    s = "Hello\r\nworld";       TRY(str_trim_left_in_if_not(s, char_is_line_break));   TEST_EQUAL(s, "\r\nworld");
    s = "\r\nHello world\r\n";  TRY(str_trim_left_in_if_not(s, char_is_line_break));   TEST_EQUAL(s, "\r\nHello world\r\n");
    s = "";                     TRY(str_trim_right_in_if_not(s, char_is_line_break));  TEST_EQUAL(s, "");
    s = "Hello\r\nworld";       TRY(str_trim_right_in_if_not(s, char_is_line_break));  TEST_EQUAL(s, "Hello\r\n");
    s = "\r\nHello world\r\n";  TRY(str_trim_right_in_if_not(s, char_is_line_break));  TEST_EQUAL(s, "\r\nHello world\r\n");

}

void test_unicorn_string_manip_unify() {

    Ustring s;

    TEST_EQUAL(str_unify_lines(""s), "");
    TEST_EQUAL(str_unify_lines("Hello world\nGoodbye\n"s), "Hello world\nGoodbye\n");
    TEST_EQUAL(str_unify_lines("Hello world\nGoodbye"s), "Hello world\nGoodbye\n");
    TEST_EQUAL(str_unify_lines("Hello world\nGoodbye\n"s, "\r\n"), "Hello world\r\nGoodbye\r\n");
    TEST_EQUAL(str_unify_lines("Hello world\nGoodbye\n"s, U'*'), "Hello world*Goodbye*");
    TEST_EQUAL(str_unify_lines("Hello world\r\nGoodbye\r\n"s), "Hello world\nGoodbye\n");
    TEST_EQUAL(str_unify_lines("Hello world\r\nGoodbye\r\n"s, "\r\n"), "Hello world\r\nGoodbye\r\n");
    TEST_EQUAL(str_unify_lines("Hello world\r\nGoodbye\r\n"s, U'*'), "Hello world*Goodbye*");
    TEST_EQUAL(str_unify_lines("Hello world\u2028Goodbye\u2029"s), "Hello world\nGoodbye\n");
    TEST_EQUAL(str_unify_lines("Hello world\u2028Goodbye\u2029"s, "\r\n"), "Hello world\r\nGoodbye\r\n");
    TEST_EQUAL(str_unify_lines("Hello world\u2028Goodbye\u2029"s, U'*'), "Hello world*Goodbye*");

    s = "";                                TRY(str_unify_lines_in(s));          TEST_EQUAL(s, "");
    s = "Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s));          TEST_EQUAL(s, "Hello world\nGoodbye\n");
    s = "Hello world\nGoodbye";            TRY(str_unify_lines_in(s));          TEST_EQUAL(s, "Hello world\nGoodbye\n");
    s = "Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s, "\r\n"));  TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");
    s = "Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s));          TEST_EQUAL(s, "Hello world\nGoodbye\n");
    s = "Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s, "\r\n"));  TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");
    s = "Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s));          TEST_EQUAL(s, "Hello world\nGoodbye\n");
    s = "Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s, "\r\n"));  TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

}

void test_unicorn_string_manip_wrap() {

    Ustring s, t;

    TEST_EQUAL(str_wrap(""s), ""s);
    TEST_EQUAL(str_wrap("\r\n"s), ""s);

    s =
        "Lorem ipsum dolor sit amet,\r\n"
        "consectetur adipisicing elit,\r\n"
        "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
        "Ut enim ad minim veniam,\r\n"
        "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n";
    t =
        //...:....1....:....2....:....3....:....4....:....5
        "Lorem ipsum dolor sit amet, consectetur\n"
        "adipisicing elit, sed do eiusmod tempor\n"
        "incididunt ut labore et dolore magna\n"
        "aliqua. Ut enim ad minim veniam, quis\n"
        "nostrud exercitation ullamco laboris\n"
        "nisi ut aliquip ex ea commodo consequat.\n";
    TEST_EQUAL(str_wrap(s, Wrap::width=40), t);
    TRY(str_wrap_in(s, Wrap::width=40));
    TEST_EQUAL(s, t);

    s =
        "\r\n"
        "\r\n"
        "Lorem ipsum dolor sit amet,\r\n"
        "consectetur adipisicing elit,\r\n"
        "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
        "\r\n"
        "Lorem ipsum dolor sit amet,\r\n"
        "consectetur adipisicing elit,\r\n"
        "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
        "Ut enim ad minim veniam,\r\n"
        "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n"
        "\r\n"
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit.\r\n"
        "\r\n"
        "\r\n";
    t =
        //...:....1....:....2....:....3....:....4....:....5
        "Lorem ipsum dolor sit amet, consectetur\n"
        "adipisicing elit, sed do eiusmod tempor\n"
        "incididunt ut labore et dolore magna\n"
        "aliqua.\n"
        "\n"
        "Lorem ipsum dolor sit amet, consectetur\n"
        "adipisicing elit, sed do eiusmod tempor\n"
        "incididunt ut labore et dolore magna\n"
        "aliqua. Ut enim ad minim veniam, quis\n"
        "nostrud exercitation ullamco laboris\n"
        "nisi ut aliquip ex ea commodo consequat.\n"
        "\n"
        "Lorem ipsum dolor sit amet, consectetur\n"
        "adipisicing elit.\n";
    TEST_EQUAL(str_wrap(s, Wrap::width=40), t);
    TRY(str_wrap_in(s, Wrap::width=40));
    TEST_EQUAL(s, t);

    s =
        "\r\n"
        "\r\n"
        "Lorem ipsum dolor sit amet,"
        "    consectetur adipisicing elit,"
        "    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
        "Lorem ipsum dolor sit amet,"
        "    consectetur adipisicing elit,"
        "    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
        "    Ut enim ad minim veniam,"
        "    quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n"
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit.\r\n"
        "\r\n"
        "\r\n";
    t =
        //...:....1....:....2....:....3....:....4....:....5
        "Lorem ipsum dolor sit amet, consectetur\n"
        "adipisicing elit, sed do eiusmod tempor\n"
        "incididunt ut labore et dolore magna\n"
        "aliqua.\n"
        "\n"
        "Lorem ipsum dolor sit amet, consectetur\n"
        "adipisicing elit, sed do eiusmod tempor\n"
        "incididunt ut labore et dolore magna\n"
        "aliqua. Ut enim ad minim veniam, quis\n"
        "nostrud exercitation ullamco laboris\n"
        "nisi ut aliquip ex ea commodo consequat.\n"
        "\n"
        "Lorem ipsum dolor sit amet, consectetur\n"
        "adipisicing elit.\n";
    TEST_EQUAL(str_wrap(s, Wrap::width=40, Wrap::lines), t);
    TRY(str_wrap_in(s, Wrap::width=40, Wrap::lines));
    TEST_EQUAL(s, t);

    t =
        //...:....1....:....2....:....3....:....4....:....5
        "                    Lorem ipsum dolor sit amet,\n"
        "          consectetur adipisicing elit, sed do\n"
        "          eiusmod tempor incididunt ut labore et\n"
        "          dolore magna aliqua.\n"
        "\n"
        "                    Lorem ipsum dolor sit amet,\n"
        "          consectetur adipisicing elit, sed do\n"
        "          eiusmod tempor incididunt ut labore et\n"
        "          dolore magna aliqua. Ut enim ad minim\n"
        "          veniam, quis nostrud exercitation\n"
        "          ullamco laboris nisi ut aliquip ex ea\n"
        "          commodo consequat.\n"
        "\n"
        "                    Lorem ipsum dolor sit amet,\n"
        "          consectetur adipisicing elit.\n";
    TEST_EQUAL(str_wrap(s, Wrap::width=50, Wrap::margin=20, Wrap::margin2=10), t);
    TRY(str_wrap_in(s, Wrap::width=50, Wrap::margin=20, Wrap::margin2=10));
    TEST_EQUAL(s, t);

    t =
        //...:....1....:....2....:....3....:....4....:....5
        "          Lorem ipsum dolor sit amet,\n"
        "consectetur adipisicing elit, sed do\n"
        "eiusmod tempor incididunt ut labore et\n"
        "dolore magna aliqua.\n"
        "*****\n"
        "          Lorem ipsum dolor sit amet,\n"
        "consectetur adipisicing elit, sed do\n"
        "eiusmod tempor incididunt ut labore et\n"
        "dolore magna aliqua. Ut enim ad minim\n"
        "veniam, quis nostrud exercitation\n"
        "ullamco laboris nisi ut aliquip ex ea\n"
        "commodo consequat.\n"
        "*****\n"
        "          Lorem ipsum dolor sit amet,\n"
        "consectetur adipisicing elit.\n";
    TEST_EQUAL(str_wrap(s, Wrap::width=40, Wrap::margin=10, Wrap::margin2=0, Wrap::newpara="\n*****\n"), t);
    TRY(str_wrap_in(s, Wrap::width=40, Wrap::margin=10, Wrap::margin2=0, Wrap::newpara="\n*****\n"));
    TEST_EQUAL(s, t);

    s =
        "Lorem ipsum dolor sit amet,\r\n"
        "consectetur adipisicing elit,\r\n"
        "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
        "Ut enim ad minim veniam,\r\n"
        "quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r\n";
    t =
        //...:....1....:....2....:....3....:....4....:....5
        "Lorem ipsum dolor sit amet, consectetur\r\n"
        "adipisicing elit, sed do eiusmod tempor\r\n"
        "incididunt ut labore et dolore magna\r\n"
        "aliqua. Ut enim ad minim veniam, quis\r\n"
        "nostrud exercitation ullamco laboris\r\n"
        "nisi ut aliquip ex ea commodo consequat.\r\n";
    TEST_EQUAL(str_wrap(s, Wrap::newline="\r\n", Wrap::width=40), t);
    TRY(str_wrap_in(s, Wrap::newline="\r\n", Wrap::width=40));
    TEST_EQUAL(s, t);

    s =
        "Lorem ipsum dolor sit amet,\r\n"
        "consectetur adipisicing elit,\r\n"
        "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
        "\r\n"
        "\r\n"
        "    Lorem ipsum dolor sit amet, consectetur adipisicing elit,\r\n"
        "    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\r\n"
        "\r\n"
        "\r\n"
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit.\r\n";
    t =
        //...:....1....:....2....:....3....:....4....:....5
        "Lorem ipsum dolor sit amet, consectetur\n"
        "adipisicing elit, sed do eiusmod tempor\n"
        "incididunt ut labore et dolore magna\n"
        "aliqua.\n"
        "\n"
        "    Lorem ipsum dolor sit amet, consectetur adipisicing elit,\n"
        "    sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
        "\n"
        "Lorem ipsum dolor sit amet, consectetur\n"
        "adipisicing elit.\n";
    TEST_EQUAL(str_wrap(s, Wrap::preserve, Wrap::width=40), t);
    TRY(str_wrap_in(s, Wrap::preserve, Wrap::width=40));
    TEST_EQUAL(s, t);

}
