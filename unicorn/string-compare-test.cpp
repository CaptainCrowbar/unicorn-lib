#include "unicorn/string.hpp"
#include "unicorn/unit-test.hpp"

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_string_compare_basic() {

    StringCompare<Strcmp::equal> cmp_e;
    StringCompare<Strcmp::less> cmp_l;
    StringCompare<Strcmp::triple> cmp_t;

    TEST(cmp_e(""s, ""s));
    TEST(! cmp_e("Hello"s, ""s));
    TEST(! cmp_e(""s, "Hello"s));
    TEST(cmp_e("Hello"s, "Hello"s));
    TEST(! cmp_e("Hello"s, "Hellfire"s));
    TEST(! cmp_e("Hellfire"s, "Hello"s));
    TEST(cmp_e("αβγδε"s, "αβγδε"s));
    TEST(! cmp_e("αβδε"s, "αβγδ"s));
    TEST(! cmp_e("αβγδ"s, "αβδε"s));

    TEST(! cmp_l(""s, ""s));
    TEST(! cmp_l("Hello"s, ""s));
    TEST(cmp_l(""s, "Hello"s));
    TEST(! cmp_l("Hello"s, "Hello"s));
    TEST(! cmp_l("Hello"s, "Hellfire"s));
    TEST(cmp_l("Hellfire"s, "Hello"s));
    TEST(! cmp_l("αβγδε"s, "αβγδε"s));
    TEST(! cmp_l("αβδε"s, "αβγδ"s));
    TEST(cmp_l("αβγδ"s, "αβδε"s));

    TEST_EQUAL(cmp_t(""s, ""s), 0);
    TEST_EQUAL(cmp_t("Hello"s, ""s), 1);
    TEST_EQUAL(cmp_t(""s, "Hello"s), -1);
    TEST_EQUAL(cmp_t("Hello"s, "Hello"s), 0);
    TEST_EQUAL(cmp_t("Hello"s, "Hellfire"s), 1);
    TEST_EQUAL(cmp_t("Hellfire"s, "Hello"s), -1);
    TEST_EQUAL(cmp_t("αβγδε"s, "αβγδε"s), 0);
    TEST_EQUAL(cmp_t("αβδε"s, "αβγδ"s), 1);
    TEST_EQUAL(cmp_t("αβγδ"s, "αβδε"s), -1);

}

void test_unicorn_string_compare_icase() {

    StringCompare<Strcmp::equal | Strcmp::icase> cmp_ei;
    StringCompare<Strcmp::less | Strcmp::icase> cmp_li;
    StringCompare<Strcmp::triple | Strcmp::icase> cmp_ti;
    StringCompare<Strcmp::equal | Strcmp::icase | Strcmp::fallback> cmp_eif;
    StringCompare<Strcmp::less | Strcmp::icase | Strcmp::fallback> cmp_lif;
    StringCompare<Strcmp::triple | Strcmp::icase | Strcmp::fallback> cmp_tif;

    TEST(cmp_ei(""s, ""s));
    TEST(cmp_ei("hello"s, "hello"s));
    TEST(cmp_ei("Hello"s, "hello"s));
    TEST(cmp_ei("hello"s, "Hello"s));
    TEST(cmp_ei("HELLO"s, "hello"s));
    TEST(cmp_ei("hello"s, "HELLO"s));
    TEST(! cmp_ei(""s, "hello"s));
    TEST(! cmp_ei("hello"s, ""s));
    TEST(! cmp_ei("hello"s, "WORLD"s));
    TEST(! cmp_ei("HELLO"s, "world"s));
    TEST(! cmp_ei("world"s, "HELLO"s));
    TEST(! cmp_ei("WORLD"s, "hello"s));
    TEST(! cmp_ei("hello"s, "HELLO WORLD"s));
    TEST(! cmp_ei("HELLO"s, "hello world"s));
    TEST(! cmp_ei("hello world"s, "HELLO"s));
    TEST(! cmp_ei("HELLO WORLD"s, "hello"s));
    TEST(! cmp_ei("hello world"s, "HELLO!WORLD"s));
    TEST(! cmp_ei("HELLO WORLD"s, "hello!world"s));

    TEST(! cmp_li(""s, ""s));
    TEST(! cmp_li("hello"s, "hello"s));
    TEST(! cmp_li("Hello"s, "hello"s));
    TEST(! cmp_li("hello"s, "Hello"s));
    TEST(! cmp_li("HELLO"s, "hello"s));
    TEST(! cmp_li("hello"s, "HELLO"s));
    TEST(cmp_li(""s, "hello"s));
    TEST(! cmp_li("hello"s, ""s));
    TEST(cmp_li("hello"s, "WORLD"s));
    TEST(cmp_li("HELLO"s, "world"s));
    TEST(! cmp_li("world"s, "HELLO"s));
    TEST(! cmp_li("WORLD"s, "hello"s));
    TEST(cmp_li("hello"s, "HELLO WORLD"s));
    TEST(cmp_li("HELLO"s, "hello world"s));
    TEST(! cmp_li("hello world"s, "HELLO"s));
    TEST(! cmp_li("HELLO WORLD"s, "hello"s));
    TEST(cmp_li("hello world"s, "HELLO!WORLD"s));
    TEST(cmp_li("HELLO WORLD"s, "hello!world"s));

    TEST_EQUAL(cmp_ti(""s, ""s), 0);
    TEST_EQUAL(cmp_ti("hello"s, "hello"s), 0);
    TEST_EQUAL(cmp_ti("Hello"s, "hello"s), 0);
    TEST_EQUAL(cmp_ti("hello"s, "Hello"s), 0);
    TEST_EQUAL(cmp_ti("HELLO"s, "hello"s), 0);
    TEST_EQUAL(cmp_ti("hello"s, "HELLO"s), 0);
    TEST_EQUAL(cmp_ti(""s, "hello"s), -1);
    TEST_EQUAL(cmp_ti("hello"s, ""s), 1);
    TEST_EQUAL(cmp_ti("hello"s, "WORLD"s), -1);
    TEST_EQUAL(cmp_ti("HELLO"s, "world"s), -1);
    TEST_EQUAL(cmp_ti("world"s, "HELLO"s), 1);
    TEST_EQUAL(cmp_ti("WORLD"s, "hello"s), 1);
    TEST_EQUAL(cmp_ti("hello"s, "HELLO WORLD"s), -1);
    TEST_EQUAL(cmp_ti("HELLO"s, "hello world"s), -1);
    TEST_EQUAL(cmp_ti("hello world"s, "HELLO"s), 1);
    TEST_EQUAL(cmp_ti("HELLO WORLD"s, "hello"s), 1);
    TEST_EQUAL(cmp_ti("hello world"s, "HELLO!WORLD"s), -1);
    TEST_EQUAL(cmp_ti("HELLO WORLD"s, "hello!world"s), -1);

    TEST(cmp_eif(""s, ""s));
    TEST(cmp_eif("hello"s, "hello"s));
    TEST(! cmp_eif("Hello"s, "hello"s));
    TEST(! cmp_eif("hello"s, "Hello"s));
    TEST(! cmp_eif("HELLO"s, "hello"s));
    TEST(! cmp_eif("hello"s, "HELLO"s));
    TEST(! cmp_eif(""s, "hello"s));
    TEST(! cmp_eif("hello"s, ""s));
    TEST(! cmp_eif("hello"s, "WORLD"s));
    TEST(! cmp_eif("HELLO"s, "world"s));
    TEST(! cmp_eif("world"s, "HELLO"s));
    TEST(! cmp_eif("WORLD"s, "hello"s));
    TEST(! cmp_eif("hello"s, "HELLO WORLD"s));
    TEST(! cmp_eif("HELLO"s, "hello world"s));
    TEST(! cmp_eif("hello world"s, "HELLO"s));
    TEST(! cmp_eif("HELLO WORLD"s, "hello"s));
    TEST(! cmp_eif("hello world"s, "HELLO!WORLD"s));
    TEST(! cmp_eif("HELLO WORLD"s, "hello!world"s));

    TEST(! cmp_lif(""s, ""s));
    TEST(! cmp_lif("hello"s, "hello"s));
    TEST(cmp_lif("Hello"s, "hello"s));
    TEST(! cmp_lif("hello"s, "Hello"s));
    TEST(cmp_lif("HELLO"s, "hello"s));
    TEST(! cmp_lif("hello"s, "HELLO"s));
    TEST(cmp_lif(""s, "hello"s));
    TEST(! cmp_lif("hello"s, ""s));
    TEST(cmp_lif("hello"s, "WORLD"s));
    TEST(cmp_lif("HELLO"s, "world"s));
    TEST(! cmp_lif("world"s, "HELLO"s));
    TEST(! cmp_lif("WORLD"s, "hello"s));
    TEST(cmp_lif("hello"s, "HELLO WORLD"s));
    TEST(cmp_lif("HELLO"s, "hello world"s));
    TEST(! cmp_lif("hello world"s, "HELLO"s));
    TEST(! cmp_lif("HELLO WORLD"s, "hello"s));
    TEST(cmp_lif("hello world"s, "HELLO!WORLD"s));
    TEST(cmp_lif("HELLO WORLD"s, "hello!world"s));

    TEST_EQUAL(cmp_tif(""s, ""s), 0);
    TEST_EQUAL(cmp_tif("hello"s, "hello"s), 0);
    TEST_EQUAL(cmp_tif("Hello"s, "hello"s), -1);
    TEST_EQUAL(cmp_tif("hello"s, "Hello"s), 1);
    TEST_EQUAL(cmp_tif("HELLO"s, "hello"s), -1);
    TEST_EQUAL(cmp_tif("hello"s, "HELLO"s), 1);
    TEST_EQUAL(cmp_tif(""s, "hello"s), -1);
    TEST_EQUAL(cmp_tif("hello"s, ""s), 1);
    TEST_EQUAL(cmp_tif("hello"s, "WORLD"s), -1);
    TEST_EQUAL(cmp_tif("HELLO"s, "world"s), -1);
    TEST_EQUAL(cmp_tif("world"s, "HELLO"s), 1);
    TEST_EQUAL(cmp_tif("WORLD"s, "hello"s), 1);
    TEST_EQUAL(cmp_tif("hello"s, "HELLO WORLD"s), -1);
    TEST_EQUAL(cmp_tif("HELLO"s, "hello world"s), -1);
    TEST_EQUAL(cmp_tif("hello world"s, "HELLO"s), 1);
    TEST_EQUAL(cmp_tif("HELLO WORLD"s, "hello"s), 1);
    TEST_EQUAL(cmp_tif("hello world"s, "HELLO!WORLD"s), -1);
    TEST_EQUAL(cmp_tif("HELLO WORLD"s, "hello!world"s), -1);

}

void test_unicorn_string_compare_natural() {

    StringCompare<Strcmp::equal | Strcmp::natural> cmp_en;
    StringCompare<Strcmp::less | Strcmp::natural> cmp_ln;
    StringCompare<Strcmp::triple | Strcmp::natural> cmp_tn;
    StringCompare<Strcmp::equal | Strcmp::natural | Strcmp::fallback> cmp_enf;
    StringCompare<Strcmp::less | Strcmp::natural | Strcmp::fallback> cmp_lnf;
    StringCompare<Strcmp::triple | Strcmp::natural | Strcmp::fallback> cmp_tnf;

    TEST(cmp_en(""s, ""s));                          TEST(cmp_enf(""s, ""s));
    TEST(cmp_en("abc 123", "abc 123"));              TEST(cmp_enf("abc 123", "abc 123"));
    TEST(cmp_en("ABC 123", "abc 123"));              TEST(! cmp_enf("ABC 123", "abc 123"));
    TEST(cmp_en("abc 123", "ABC 123"));              TEST(! cmp_enf("abc 123", "ABC 123"));
    TEST(! cmp_en(""s, "abc 123"));                  TEST(! cmp_enf(""s, "abc 123"));
    TEST(! cmp_en("abc 123", ""s));                  TEST(! cmp_enf("abc 123", ""s));
    TEST(! cmp_en("abc 123", "abc 45"));             TEST(! cmp_enf("abc 123", "abc 45"));
    TEST(! cmp_en("abc 45", "abc 123"));             TEST(! cmp_enf("abc 45", "abc 123"));
    TEST(! cmp_en("abc 45", "ABC 67"));              TEST(! cmp_enf("abc 45", "ABC 67"));
    TEST(! cmp_en("ABC 67", "abc 45"));              TEST(! cmp_enf("ABC 67", "abc 45"));
    TEST(! cmp_en("abc 45", "abc 000123"));          TEST(! cmp_enf("abc 45", "abc 000123"));
    TEST(! cmp_en("abc 000123", "abc 45"));          TEST(! cmp_enf("abc 000123", "abc 45"));
    TEST(! cmp_en("abc 123", "abc 123 something"));  TEST(! cmp_enf("abc 123", "abc 123 something"));
    TEST(! cmp_en("abc 123 something", "abc 123"));  TEST(! cmp_enf("abc 123 something", "abc 123"));
    TEST(! cmp_en("abc 123", "abc 123 456"));        TEST(! cmp_enf("abc 123", "abc 123 456"));
    TEST(! cmp_en("abc 123 456", "abc 123"));        TEST(! cmp_enf("abc 123 456", "abc 123"));
    TEST(! cmp_en("abc 123", "+abc 123"));           TEST(! cmp_enf("abc 123", "+abc 123"));
    TEST(! cmp_en("+abc 123", "abc 123"));           TEST(! cmp_enf("+abc 123", "abc 123"));

    TEST(! cmp_ln(""s, ""s));                        TEST(! cmp_lnf(""s, ""s));
    TEST(! cmp_ln("abc 123", "abc 123"));            TEST(! cmp_lnf("abc 123", "abc 123"));
    TEST(! cmp_ln("ABC 123", "abc 123"));            TEST(cmp_lnf("ABC 123", "abc 123"));
    TEST(! cmp_ln("abc 123", "ABC 123"));            TEST(! cmp_lnf("abc 123", "ABC 123"));
    TEST(cmp_ln(""s, "abc 123"));                    TEST(cmp_lnf(""s, "abc 123"));
    TEST(! cmp_ln("abc 123", ""s));                  TEST(! cmp_lnf("abc 123", ""s));
    TEST(! cmp_ln("abc 123", "abc 45"));             TEST(! cmp_lnf("abc 123", "abc 45"));
    TEST(cmp_ln("abc 45", "abc 123"));               TEST(cmp_lnf("abc 45", "abc 123"));
    TEST(cmp_ln("abc 45", "ABC 67"));                TEST(cmp_lnf("abc 45", "ABC 67"));
    TEST(! cmp_ln("ABC 67", "abc 45"));              TEST(! cmp_lnf("ABC 67", "abc 45"));
    TEST(cmp_ln("abc 45", "abc 000123"));            TEST(cmp_lnf("abc 45", "abc 000123"));
    TEST(! cmp_ln("abc 000123", "abc 45"));          TEST(! cmp_lnf("abc 000123", "abc 45"));
    TEST(cmp_ln("abc 123", "abc 123 something"));    TEST(cmp_lnf("abc 123", "abc 123 something"));
    TEST(! cmp_ln("abc 123 something", "abc 123"));  TEST(! cmp_lnf("abc 123 something", "abc 123"));
    TEST(cmp_ln("abc 123", "abc 123 456"));          TEST(cmp_lnf("abc 123", "abc 123 456"));
    TEST(! cmp_ln("abc 123 456", "abc 123"));        TEST(! cmp_lnf("abc 123 456", "abc 123"));
    TEST(! cmp_ln("abc 123", "+abc 123"));           TEST(! cmp_lnf("abc 123", "+abc 123"));
    TEST(cmp_ln("+abc 123", "abc 123"));             TEST(cmp_lnf("+abc 123", "abc 123"));

    TEST_EQUAL(cmp_tn(""s, ""s), 0);                         TEST_EQUAL(cmp_tnf(""s, ""s), 0);
    TEST_EQUAL(cmp_tn("abc 123", "abc 123"), 0);             TEST_EQUAL(cmp_tnf("abc 123", "abc 123"), 0);
    TEST_EQUAL(cmp_tn("ABC 123", "abc 123"), 0);             TEST_EQUAL(cmp_tnf("ABC 123", "abc 123"), -1);
    TEST_EQUAL(cmp_tn("abc 123", "ABC 123"), 0);             TEST_EQUAL(cmp_tnf("abc 123", "ABC 123"), 1);
    TEST_EQUAL(cmp_tn(""s, "abc 123"), -1);                  TEST_EQUAL(cmp_tnf(""s, "abc 123"), -1);
    TEST_EQUAL(cmp_tn("abc 123", ""s), 1);                   TEST_EQUAL(cmp_tnf("abc 123", ""s), 1);
    TEST_EQUAL(cmp_tn("abc 123", "abc 45"), 1);              TEST_EQUAL(cmp_tnf("abc 123", "abc 45"), 1);
    TEST_EQUAL(cmp_tn("abc 45", "abc 123"), -1);             TEST_EQUAL(cmp_tnf("abc 45", "abc 123"), -1);
    TEST_EQUAL(cmp_tn("abc 45", "ABC 67"), -1);              TEST_EQUAL(cmp_tnf("abc 45", "ABC 67"), -1);
    TEST_EQUAL(cmp_tn("ABC 67", "abc 45"), 1);               TEST_EQUAL(cmp_tnf("ABC 67", "abc 45"), 1);
    TEST_EQUAL(cmp_tn("abc 45", "abc 000123"), -1);          TEST_EQUAL(cmp_tnf("abc 45", "abc 000123"), -1);
    TEST_EQUAL(cmp_tn("abc 000123", "abc 45"), 1);           TEST_EQUAL(cmp_tnf("abc 000123", "abc 45"), 1);
    TEST_EQUAL(cmp_tn("abc 123", "abc 123 something"), -1);  TEST_EQUAL(cmp_tnf("abc 123", "abc 123 something"), -1);
    TEST_EQUAL(cmp_tn("abc 123 something", "abc 123"), 1);   TEST_EQUAL(cmp_tnf("abc 123 something", "abc 123"), 1);
    TEST_EQUAL(cmp_tn("abc 123", "abc 123 456"), -1);        TEST_EQUAL(cmp_tnf("abc 123", "abc 123 456"), -1);
    TEST_EQUAL(cmp_tn("abc 123 456", "abc 123"), 1);         TEST_EQUAL(cmp_tnf("abc 123 456", "abc 123"), 1);
    TEST_EQUAL(cmp_tn("abc 123", "+abc 123"), 1);            TEST_EQUAL(cmp_tnf("abc 123", "+abc 123"), 1);
    TEST_EQUAL(cmp_tn("+abc 123", "abc 123"), -1);           TEST_EQUAL(cmp_tnf("+abc 123", "abc 123"), -1);

}
