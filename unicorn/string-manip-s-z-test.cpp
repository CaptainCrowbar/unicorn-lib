#include "unicorn/string.hpp"
#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "rs-core/unit-test.hpp"
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

namespace {

    void check_split() {

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

        U8string text =
            u8"Line one\n"
            u8"Line two\r"
            u8"Line three\r\n"
            u8"Line four\f"
            u8"Line five\u0085"
            u8"Line six\u2028"
            u8"Line seven\u2029";
        TRY(str_split_lines(text, overwrite(v)));
        TEST_EQUAL(v.size(), 7);
        TEST_EQUAL(str_join(v, "/"),
            u8"Line one/"
            u8"Line two/"
            u8"Line three/"
            u8"Line four/"
            u8"Line five/"
            u8"Line six/"
            u8"Line seven");
        text =
            u8"Line one\n\n"
            u8"Line two\r\r"
            u8"Line three\r\n\r\n"
            u8"Line four\f\f"
            u8"Line five\u0085\u0085"
            u8"Line six\u2028\u2028"
            u8"Line seven\u2029\u2029";
        TRY(str_split_lines(text, overwrite(v)));
        TEST_EQUAL(v.size(), 14);
        TEST_EQUAL(str_join(v, "/"),
            u8"Line one//"
            u8"Line two//"
            u8"Line three//"
            u8"Line four//"
            u8"Line five//"
            u8"Line six//"
            u8"Line seven/");

    }

    void check_squeeze() {

        U8string s;

        TEST_EQUAL(str_squeeze(u8""s), u8""s);
        TEST_EQUAL(str_squeeze(u8"\t\t\t"s), u8" "s);
        TEST_EQUAL(str_squeeze(u8"Hello world"s), u8"Hello world"s);
        TEST_EQUAL(str_squeeze(u8"\t\t\tHello\t\t\tworld\t\t\t"s), u8" Hello world "s);
        TEST_EQUAL(str_squeeze(u8"€uro ∈lement"s), u8"€uro ∈lement"s);
        TEST_EQUAL(str_squeeze(u8"\u2028\u2028€uro\u2028\u2028∈lement\u2028\u2028"s), u8" €uro ∈lement "s);
        TEST_EQUAL(str_squeeze(u8""s, u8"+-*/"s), u8""s);
        TEST_EQUAL(str_squeeze(u8"/*-+"s, u8"+-*/"s), u8"+"s);
        TEST_EQUAL(str_squeeze(u8"Hello world"s, u8"+-*/"s), u8"Hello world"s);
        TEST_EQUAL(str_squeeze(u8"/*-+Hello/*-+world/*-+"s, u8"+-*/"s), u8"+Hello+world+"s);
        TEST_EQUAL(str_squeeze(u8"∇∃∀€uro∇∃∀∈lement∇∃∀"s, u8"∀∃∇"s), u8"∀€uro∀∈lement∀"s);

        TEST_EQUAL(str_squeeze_trim(u8""s), u8""s);
        TEST_EQUAL(str_squeeze_trim(u8"\t\t\t"s), u8""s);
        TEST_EQUAL(str_squeeze_trim(u8"Hello world"s), u8"Hello world"s);
        TEST_EQUAL(str_squeeze_trim(u8"\t\t\tHello\t\t\tworld\t\t\t"s), u8"Hello world"s);
        TEST_EQUAL(str_squeeze_trim(u8"€uro ∈lement"s), u8"€uro ∈lement"s);
        TEST_EQUAL(str_squeeze_trim(u8"\u2028\u2028€uro\u2028\u2028∈lement\u2028\u2028"s), u8"€uro ∈lement"s);
        TEST_EQUAL(str_squeeze_trim(u8""s, u8"+-*/"s), u8""s);
        TEST_EQUAL(str_squeeze_trim(u8"/*-+"s, u8"+-*/"s), u8""s);
        TEST_EQUAL(str_squeeze_trim(u8"Hello world"s, u8"+-*/"s), u8"Hello world"s);
        TEST_EQUAL(str_squeeze_trim(u8"/*-+Hello/*-+world/*-+"s, u8"+-*/"s), u8"Hello+world"s);
        TEST_EQUAL(str_squeeze_trim(u8"∇∃∀€uro∇∃∀∈lement∇∃∀"s, u8"∀∃∇"s), u8"€uro∀∈lement"s);

        s = u8""s;                                                 TRY(str_squeeze_in(s));             TEST_EQUAL(s, u8""s);
        s = u8"\t\t\t"s;                                           TRY(str_squeeze_in(s));             TEST_EQUAL(s, u8" "s);
        s = u8"Hello world"s;                                      TRY(str_squeeze_in(s));             TEST_EQUAL(s, u8"Hello world"s);
        s = u8"\t\t\tHello\t\t\tworld\t\t\t"s;                     TRY(str_squeeze_in(s));             TEST_EQUAL(s, u8" Hello world "s);
        s = u8"€uro ∈lement"s;                                     TRY(str_squeeze_in(s));             TEST_EQUAL(s, u8"€uro ∈lement"s);
        s = u8"\u2028\u2028€uro\u2028\u2028∈lement\u2028\u2028"s;  TRY(str_squeeze_in(s));             TEST_EQUAL(s, u8" €uro ∈lement "s);
        s = u8""s;                                                 TRY(str_squeeze_in(s, u8"+-*/"s));  TEST_EQUAL(s, u8""s);
        s = u8"/*-+"s;                                             TRY(str_squeeze_in(s, u8"+-*/"s));  TEST_EQUAL(s, u8"+"s);
        s = u8"Hello world"s;                                      TRY(str_squeeze_in(s, u8"+-*/"s));  TEST_EQUAL(s, u8"Hello world"s);
        s = u8"/*-+Hello/*-+world/*-+"s;                           TRY(str_squeeze_in(s, u8"+-*/"s));  TEST_EQUAL(s, u8"+Hello+world+"s);
        s = u8"∇∃∀€uro∇∃∀∈lement∇∃∀"s;                             TRY(str_squeeze_in(s, u8"∀∃∇"s));   TEST_EQUAL(s, u8"∀€uro∀∈lement∀"s);

        s = u8""s;                                                 TRY(str_squeeze_trim_in(s));             TEST_EQUAL(s, u8""s);
        s = u8"\t\t\t"s;                                           TRY(str_squeeze_trim_in(s));             TEST_EQUAL(s, u8""s);
        s = u8"Hello world"s;                                      TRY(str_squeeze_trim_in(s));             TEST_EQUAL(s, u8"Hello world"s);
        s = u8"\t\t\tHello\t\t\tworld\t\t\t"s;                     TRY(str_squeeze_trim_in(s));             TEST_EQUAL(s, u8"Hello world"s);
        s = u8"€uro ∈lement"s;                                     TRY(str_squeeze_trim_in(s));             TEST_EQUAL(s, u8"€uro ∈lement"s);
        s = u8"\u2028\u2028€uro\u2028\u2028∈lement\u2028\u2028"s;  TRY(str_squeeze_trim_in(s));             TEST_EQUAL(s, u8"€uro ∈lement"s);
        s = u8""s;                                                 TRY(str_squeeze_trim_in(s, u8"+-*/"s));  TEST_EQUAL(s, u8""s);
        s = u8"/*-+"s;                                             TRY(str_squeeze_trim_in(s, u8"+-*/"s));  TEST_EQUAL(s, u8""s);
        s = u8"Hello world"s;                                      TRY(str_squeeze_trim_in(s, u8"+-*/"s));  TEST_EQUAL(s, u8"Hello world"s);
        s = u8"/*-+Hello/*-+world/*-+"s;                           TRY(str_squeeze_trim_in(s, u8"+-*/"s));  TEST_EQUAL(s, u8"Hello+world"s);
        s = u8"∇∃∀€uro∇∃∀∈lement∇∃∀"s;                             TRY(str_squeeze_trim_in(s, u8"∀∃∇"s));   TEST_EQUAL(s, u8"€uro∀∈lement"s);

    }

    void check_substring() {

        // UTF-32    UTF-16     UTF-8
        // 0000004d  004d       4d
        // 00000430  0430       d0 b0
        // 00004e8c  4e8c       e4 ba 8c
        // 00010302  d800 df02  f0 90 8c 82
        // 0010fffd  dbff dffd  f4 8f bf bd

        const U8string example {"\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd"};

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

        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 0, Length::graphemes), u8"");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 1, Length::graphemes), u8"é");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 2, Length::graphemes), u8"éí");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 3, Length::graphemes), u8"éíó");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 4, Length::graphemes), u8"éíóú");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, 5, Length::graphemes), u8"éíóú");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 1, npos, Length::graphemes), u8"éíóú");
        TEST_EQUAL(utf_substring(u8"áéíóú"s, 5, npos, Length::graphemes), u8"");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 0, Length::graphemes), u8"");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 1, Length::graphemes), u8"e\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 2, Length::graphemes), u8"e\u0301i\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 3, Length::graphemes), u8"e\u0301i\u0301o\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 4, Length::graphemes), u8"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, 5, Length::graphemes), u8"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 1, npos, Length::graphemes), u8"e\u0301i\u0301o\u0301u\u0301");
        TEST_EQUAL(utf_substring(u8"a\u0301e\u0301i\u0301o\u0301u\u0301"s, 5, npos, Length::graphemes), u8"");

    }

    void check_translate() {

        U8string s;

        TEST_EQUAL(str_translate(""s, "", ""), "");
        TEST_EQUAL(str_translate("Hello world"s, "", ""), "Hello world");
        TEST_EQUAL(str_translate("Hello world"s, "", "abcde"), "Hello world");
        TEST_EQUAL(str_translate("Hello world"s, "abcde", ""), "Hello world");
        TEST_EQUAL(str_translate("Hello world"s, "abcde", "12345"), "H5llo worl4");
        TEST_EQUAL(str_translate("Hello world"s, "abcde", "*"), "H*llo worl*");
        TEST_EQUAL(str_translate("€uro ∈lement"s, "€∈", "∇√"), "∇uro √lement");
        TEST_EQUAL(str_translate("€uro ∈lement"s, "€∈", "*"), "*uro *lement");
        TEST_EQUAL(str_translate("€uro ∈lement"s, "e", "∇√"), "€uro ∈l∇m∇nt");

        s = u8"";              TRY(str_translate_in(s, u8"", u8""));            TEST_EQUAL(s, u8"");
        s = u8"Hello world";   TRY(str_translate_in(s, u8"", u8""));            TEST_EQUAL(s, u8"Hello world");
        s = u8"Hello world";   TRY(str_translate_in(s, u8"", u8"abcde"));       TEST_EQUAL(s, u8"Hello world");
        s = u8"Hello world";   TRY(str_translate_in(s, u8"abcde", u8""));       TEST_EQUAL(s, u8"Hello world");
        s = u8"Hello world";   TRY(str_translate_in(s, u8"abcde", u8"12345"));  TEST_EQUAL(s, u8"H5llo worl4");
        s = u8"Hello world";   TRY(str_translate_in(s, u8"abcde", u8"*"));      TEST_EQUAL(s, u8"H*llo worl*");
        s = u8"€uro ∈lement";  TRY(str_translate_in(s, u8"€∈", u8"∇√"));        TEST_EQUAL(s, u8"∇uro √lement");
        s = u8"€uro ∈lement";  TRY(str_translate_in(s, u8"€∈", u8"*"));         TEST_EQUAL(s, u8"*uro *lement");
        s = u8"€uro ∈lement";  TRY(str_translate_in(s, u8"e", u8"∇√"));         TEST_EQUAL(s, u8"€uro ∈l∇m∇nt");

    }

    void check_trim() {

        U8string s;

        TEST_EQUAL(str_trim(u8""s), u8"");
        TEST_EQUAL(str_trim(u8"Hello"s), u8"Hello");
        TEST_EQUAL(str_trim(u8"Hello world"s), u8"Hello world");
        TEST_EQUAL(str_trim(u8""s, u8""), u8"");
        TEST_EQUAL(str_trim(u8"Hello"s, u8""), u8"Hello");
        TEST_EQUAL(str_trim(u8"<<<>>>"s, u8"<>"), u8"");
        TEST_EQUAL(str_trim(u8"<<<Hello>>>"s, u8"<>"), u8"Hello");
        TEST_EQUAL(str_trim(u8"<<<Hello>>> <<<world>>>"s, u8"<>"), u8"Hello>>> <<<world");
        TEST_EQUAL(str_trim(u8"≤≤≤≥≥≥"s, u8"≤≥"), u8"");
        TEST_EQUAL(str_trim(u8"≤≤≤€uro≥≥≥"s, u8"≤≥"), u8"€uro");
        TEST_EQUAL(str_trim(u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, u8"≤≥"), u8"€uro≥≥≥ ≤≤≤∈lement");

        s = u8"";                          TRY(str_trim_in(s));          TEST_EQUAL(s, u8"");
        s = u8"Hello";                     TRY(str_trim_in(s));          TEST_EQUAL(s, u8"Hello");
        s = u8"Hello world";               TRY(str_trim_in(s));          TEST_EQUAL(s, u8"Hello world");
        s = u8"";                          TRY(str_trim_in(s, u8""));    TEST_EQUAL(s, u8"");
        s = u8"Hello";                     TRY(str_trim_in(s, u8""));    TEST_EQUAL(s, u8"Hello");
        s = u8"<<<>>>";                    TRY(str_trim_in(s, u8"<>"));  TEST_EQUAL(s, u8"");
        s = u8"<<<Hello>>>";               TRY(str_trim_in(s, u8"<>"));  TEST_EQUAL(s, u8"Hello");
        s = u8"<<<Hello>>> <<<world>>>";   TRY(str_trim_in(s, u8"<>"));  TEST_EQUAL(s, u8"Hello>>> <<<world");
        s = u8"≤≤≤≥≥≥";                    TRY(str_trim_in(s, u8"≤≥"));  TEST_EQUAL(s, u8"");
        s = u8"≤≤≤€uro≥≥≥";                TRY(str_trim_in(s, u8"≤≥"));  TEST_EQUAL(s, u8"€uro");
        s = u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_in(s, u8"≤≥"));  TEST_EQUAL(s, u8"€uro≥≥≥ ≤≤≤∈lement");

        TEST_EQUAL(str_trim_left(u8""s), u8"");
        TEST_EQUAL(str_trim_left(u8"Hello"s), u8"Hello");
        TEST_EQUAL(str_trim_left(u8"Hello world"s), u8"Hello world");
        TEST_EQUAL(str_trim_left(u8""s, u8""), u8"");
        TEST_EQUAL(str_trim_left(u8"Hello"s, u8""), u8"Hello");
        TEST_EQUAL(str_trim_left(u8"<<<>>>"s, u8"<>"), u8"");
        TEST_EQUAL(str_trim_left(u8"<<<Hello>>>"s, u8"<>"), u8"Hello>>>");
        TEST_EQUAL(str_trim_left(u8"<<<Hello>>> <<<world>>>"s, u8"<>"), u8"Hello>>> <<<world>>>");
        TEST_EQUAL(str_trim_left(u8"≤≤≤≥≥≥"s, u8"≤≥"), u8"");
        TEST_EQUAL(str_trim_left(u8"≤≤≤€uro≥≥≥"s, u8"≤≥"), u8"€uro≥≥≥");
        TEST_EQUAL(str_trim_left(u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, u8"≤≥"), u8"€uro≥≥≥ ≤≤≤∈lement≥≥≥");

        s = u8"";                          TRY(str_trim_left_in(s));          TEST_EQUAL(s, u8"");
        s = u8"Hello";                     TRY(str_trim_left_in(s));          TEST_EQUAL(s, u8"Hello");
        s = u8"Hello world";               TRY(str_trim_left_in(s));          TEST_EQUAL(s, u8"Hello world");
        s = u8"";                          TRY(str_trim_left_in(s, u8""));    TEST_EQUAL(s, u8"");
        s = u8"Hello";                     TRY(str_trim_left_in(s, u8""));    TEST_EQUAL(s, u8"Hello");
        s = u8"<<<>>>";                    TRY(str_trim_left_in(s, u8"<>"));  TEST_EQUAL(s, u8"");
        s = u8"<<<Hello>>>";               TRY(str_trim_left_in(s, u8"<>"));  TEST_EQUAL(s, u8"Hello>>>");
        s = u8"<<<Hello>>> <<<world>>>";   TRY(str_trim_left_in(s, u8"<>"));  TEST_EQUAL(s, u8"Hello>>> <<<world>>>");
        s = u8"≤≤≤≥≥≥";                    TRY(str_trim_left_in(s, u8"≤≥"));  TEST_EQUAL(s, u8"");
        s = u8"≤≤≤€uro≥≥≥";                TRY(str_trim_left_in(s, u8"≤≥"));  TEST_EQUAL(s, u8"€uro≥≥≥");
        s = u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_left_in(s, u8"≤≥"));  TEST_EQUAL(s, u8"€uro≥≥≥ ≤≤≤∈lement≥≥≥");

        TEST_EQUAL(str_trim_right(u8""s), u8"");
        TEST_EQUAL(str_trim_right(u8"Hello"s), u8"Hello");
        TEST_EQUAL(str_trim_right(u8"Hello world"s), u8"Hello world");
        TEST_EQUAL(str_trim_right(u8""s, u8""), u8"");
        TEST_EQUAL(str_trim_right(u8"Hello"s, u8""), u8"Hello");
        TEST_EQUAL(str_trim_right(u8"<<<>>>"s, u8"<>"), u8"");
        TEST_EQUAL(str_trim_right(u8"<<<Hello>>>"s, u8"<>"), u8"<<<Hello");
        TEST_EQUAL(str_trim_right(u8"<<<Hello>>> <<<world>>>"s, u8"<>"), u8"<<<Hello>>> <<<world");
        TEST_EQUAL(str_trim_right(u8"≤≤≤≥≥≥"s, u8"≤≥"), u8"");
        TEST_EQUAL(str_trim_right(u8"≤≤≤€uro≥≥≥"s, u8"≤≥"), u8"≤≤≤€uro");
        TEST_EQUAL(str_trim_right(u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥"s, u8"≤≥"), u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement");

        s = u8"";                          TRY(str_trim_right_in(s));          TEST_EQUAL(s, u8"");
        s = u8"Hello";                     TRY(str_trim_right_in(s));          TEST_EQUAL(s, u8"Hello");
        s = u8"Hello world";               TRY(str_trim_right_in(s));          TEST_EQUAL(s, u8"Hello world");
        s = u8"";                          TRY(str_trim_right_in(s, u8""));    TEST_EQUAL(s, u8"");
        s = u8"Hello";                     TRY(str_trim_right_in(s, u8""));    TEST_EQUAL(s, u8"Hello");
        s = u8"<<<>>>";                    TRY(str_trim_right_in(s, u8"<>"));  TEST_EQUAL(s, u8"");
        s = u8"<<<Hello>>>";               TRY(str_trim_right_in(s, u8"<>"));  TEST_EQUAL(s, u8"<<<Hello");
        s = u8"<<<Hello>>> <<<world>>>";   TRY(str_trim_right_in(s, u8"<>"));  TEST_EQUAL(s, u8"<<<Hello>>> <<<world");
        s = u8"≤≤≤≥≥≥";                    TRY(str_trim_right_in(s, u8"≤≥"));  TEST_EQUAL(s, u8"");
        s = u8"≤≤≤€uro≥≥≥";                TRY(str_trim_right_in(s, u8"≤≥"));  TEST_EQUAL(s, u8"≤≤≤€uro");
        s = u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement≥≥≥";  TRY(str_trim_right_in(s, u8"≤≥"));  TEST_EQUAL(s, u8"≤≤≤€uro≥≥≥ ≤≤≤∈lement");

    }

    void check_trim_if() {

        U8string s;

        TEST_EQUAL(str_trim_if(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_if(u8"Hello"s, char_is_line_break), u8"Hello");
        TEST_EQUAL(str_trim_if(u8"\r\nHello\r\n"s, char_is_line_break), u8"Hello");
        TEST_EQUAL(str_trim_left_if(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_left_if(u8"Hello"s, char_is_line_break), u8"Hello");
        TEST_EQUAL(str_trim_left_if(u8"\r\nHello\r\n"s, char_is_line_break), u8"Hello\r\n");
        TEST_EQUAL(str_trim_right_if(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_right_if(u8"Hello"s, char_is_line_break), u8"Hello");
        TEST_EQUAL(str_trim_right_if(u8"\r\nHello\r\n"s, char_is_line_break), u8"\r\nHello");
        TEST_EQUAL(str_trim_if_not(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_if_not(u8"Hello\r\nworld"s, char_is_line_break), u8"\r\n");
        TEST_EQUAL(str_trim_if_not(u8"\r\nHello world\r\n"s, char_is_line_break), u8"\r\nHello world\r\n");
        TEST_EQUAL(str_trim_left_if_not(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_left_if_not(u8"Hello\r\nworld"s, char_is_line_break), u8"\r\nworld");
        TEST_EQUAL(str_trim_left_if_not(u8"\r\nHello world\r\n"s, char_is_line_break), u8"\r\nHello world\r\n");
        TEST_EQUAL(str_trim_right_if_not(u8""s, char_is_line_break), u8"");
        TEST_EQUAL(str_trim_right_if_not(u8"Hello\r\nworld"s, char_is_line_break), u8"Hello\r\n");
        TEST_EQUAL(str_trim_right_if_not(u8"\r\nHello world\r\n"s, char_is_line_break), u8"\r\nHello world\r\n");

        s = u8"";                     TRY(str_trim_in_if(s, char_is_line_break));            TEST_EQUAL(s, u8"");
        s = u8"Hello";                TRY(str_trim_in_if(s, char_is_line_break));            TEST_EQUAL(s, u8"Hello");
        s = u8"\r\nHello\r\n";        TRY(str_trim_in_if(s, char_is_line_break));            TEST_EQUAL(s, u8"Hello");
        s = u8"";                     TRY(str_trim_left_in_if(s, char_is_line_break));       TEST_EQUAL(s, u8"");
        s = u8"Hello";                TRY(str_trim_left_in_if(s, char_is_line_break));       TEST_EQUAL(s, u8"Hello");
        s = u8"\r\nHello\r\n";        TRY(str_trim_left_in_if(s, char_is_line_break));       TEST_EQUAL(s, u8"Hello\r\n");
        s = u8"";                     TRY(str_trim_right_in_if(s, char_is_line_break));      TEST_EQUAL(s, u8"");
        s = u8"Hello";                TRY(str_trim_right_in_if(s, char_is_line_break));      TEST_EQUAL(s, u8"Hello");
        s = u8"\r\nHello\r\n";        TRY(str_trim_right_in_if(s, char_is_line_break));      TEST_EQUAL(s, u8"\r\nHello");
        s = u8"";                     TRY(str_trim_in_if_not(s, char_is_line_break));        TEST_EQUAL(s, u8"");
        s = u8"Hello\r\nworld";       TRY(str_trim_in_if_not(s, char_is_line_break));        TEST_EQUAL(s, u8"\r\n");
        s = u8"\r\nHello world\r\n";  TRY(str_trim_in_if_not(s, char_is_line_break));        TEST_EQUAL(s, u8"\r\nHello world\r\n");
        s = u8"";                     TRY(str_trim_left_in_if_not(s, char_is_line_break));   TEST_EQUAL(s, u8"");
        s = u8"Hello\r\nworld";       TRY(str_trim_left_in_if_not(s, char_is_line_break));   TEST_EQUAL(s, u8"\r\nworld");
        s = u8"\r\nHello world\r\n";  TRY(str_trim_left_in_if_not(s, char_is_line_break));   TEST_EQUAL(s, u8"\r\nHello world\r\n");
        s = u8"";                     TRY(str_trim_right_in_if_not(s, char_is_line_break));  TEST_EQUAL(s, u8"");
        s = u8"Hello\r\nworld";       TRY(str_trim_right_in_if_not(s, char_is_line_break));  TEST_EQUAL(s, u8"Hello\r\n");
        s = u8"\r\nHello world\r\n";  TRY(str_trim_right_in_if_not(s, char_is_line_break));  TEST_EQUAL(s, u8"\r\nHello world\r\n");

    }

    void check_unify() {

        U8string s;

        TEST_EQUAL(str_unify_lines(u8""s), u8"");
        TEST_EQUAL(str_unify_lines(u8"Hello world\nGoodbye\n"s), u8"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\nGoodbye"s), u8"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\nGoodbye\n"s, u8"\r\n"), u8"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\nGoodbye\n"s, U'*'), u8"Hello world*Goodbye*");
        TEST_EQUAL(str_unify_lines(u8"Hello world\r\nGoodbye\r\n"s), u8"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\r\nGoodbye\r\n"s, u8"\r\n"), u8"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\r\nGoodbye\r\n"s, U'*'), u8"Hello world*Goodbye*");
        TEST_EQUAL(str_unify_lines(u8"Hello world\u2028Goodbye\u2029"s), u8"Hello world\nGoodbye\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\u2028Goodbye\u2029"s, u8"\r\n"), u8"Hello world\r\nGoodbye\r\n");
        TEST_EQUAL(str_unify_lines(u8"Hello world\u2028Goodbye\u2029"s, U'*'), u8"Hello world*Goodbye*");

        s = u8"";                                TRY(str_unify_lines_in(s));            TEST_EQUAL(s, u8"");
        s = u8"Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s));            TEST_EQUAL(s, u8"Hello world\nGoodbye\n");
        s = u8"Hello world\nGoodbye";            TRY(str_unify_lines_in(s));            TEST_EQUAL(s, u8"Hello world\nGoodbye\n");
        s = u8"Hello world\nGoodbye\n";          TRY(str_unify_lines_in(s, u8"\r\n"));  TEST_EQUAL(s, u8"Hello world\r\nGoodbye\r\n");
        s = u8"Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s));            TEST_EQUAL(s, u8"Hello world\nGoodbye\n");
        s = u8"Hello world\r\nGoodbye\r\n";      TRY(str_unify_lines_in(s, u8"\r\n"));  TEST_EQUAL(s, u8"Hello world\r\nGoodbye\r\n");
        s = u8"Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s));            TEST_EQUAL(s, u8"Hello world\nGoodbye\n");
        s = u8"Hello world\u2028Goodbye\u2029";  TRY(str_unify_lines_in(s, u8"\r\n"));  TEST_EQUAL(s, u8"Hello world\r\nGoodbye\r\n");

    }

    void check_wrap() {

        U8string s, t;

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

}

TEST_MODULE(unicorn, string_manipulation_s_z) {

    check_split();
    check_squeeze();
    check_substring();
    check_translate();
    check_trim();
    check_trim_if();
    check_unify();
    check_wrap();

}
