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
    TEST(cmp_e(u8"αβγδε"s, u8"αβγδε"s));
    TEST(! cmp_e(u8"αβδε"s, u8"αβγδ"s));
    TEST(! cmp_e(u8"αβγδ"s, u8"αβδε"s));

    TEST(! cmp_l(""s, ""s));
    TEST(! cmp_l("Hello"s, ""s));
    TEST(cmp_l(""s, "Hello"s));
    TEST(! cmp_l("Hello"s, "Hello"s));
    TEST(! cmp_l("Hello"s, "Hellfire"s));
    TEST(cmp_l("Hellfire"s, "Hello"s));
    TEST(! cmp_l(u8"αβγδε"s, u8"αβγδε"s));
    TEST(! cmp_l(u8"αβδε"s, u8"αβγδ"s));
    TEST(cmp_l(u8"αβγδ"s, u8"αβδε"s));

    TEST_EQUAL(cmp_t(""s, ""s), 0);
    TEST_EQUAL(cmp_t("Hello"s, ""s), 1);
    TEST_EQUAL(cmp_t(""s, "Hello"s), -1);
    TEST_EQUAL(cmp_t("Hello"s, "Hello"s), 0);
    TEST_EQUAL(cmp_t("Hello"s, "Hellfire"s), 1);
    TEST_EQUAL(cmp_t("Hellfire"s, "Hello"s), -1);
    TEST_EQUAL(cmp_t(u8"αβγδε"s, u8"αβγδε"s), 0);
    TEST_EQUAL(cmp_t(u8"αβδε"s, u8"αβγδ"s), 1);
    TEST_EQUAL(cmp_t(u8"αβγδ"s, u8"αβδε"s), -1);

}

void test_unicorn_string_compare_icase() {

    StringCompare<Strcmp::equal | Strcmp::icase> cmp_ei;
    StringCompare<Strcmp::less | Strcmp::icase> cmp_li;
    StringCompare<Strcmp::triple | Strcmp::icase> cmp_ti;
    StringCompare<Strcmp::equal | Strcmp::icase | Strcmp::fallback> cmp_eif;
    StringCompare<Strcmp::less | Strcmp::icase | Strcmp::fallback> cmp_lif;
    StringCompare<Strcmp::triple | Strcmp::icase | Strcmp::fallback> cmp_tif;

    TEST(cmp_ei(u8""s, u8""s));
    TEST(cmp_ei(u8"hello"s, u8"hello"s));
    TEST(cmp_ei(u8"Hello"s, u8"hello"s));
    TEST(cmp_ei(u8"hello"s, u8"Hello"s));
    TEST(cmp_ei(u8"HELLO"s, u8"hello"s));
    TEST(cmp_ei(u8"hello"s, u8"HELLO"s));
    TEST(! cmp_ei(u8""s, u8"hello"s));
    TEST(! cmp_ei(u8"hello"s, u8""s));
    TEST(! cmp_ei(u8"hello"s, u8"WORLD"s));
    TEST(! cmp_ei(u8"HELLO"s, u8"world"s));
    TEST(! cmp_ei(u8"world"s, u8"HELLO"s));
    TEST(! cmp_ei(u8"WORLD"s, u8"hello"s));
    TEST(! cmp_ei(u8"hello"s, u8"HELLO WORLD"s));
    TEST(! cmp_ei(u8"HELLO"s, u8"hello world"s));
    TEST(! cmp_ei(u8"hello world"s, u8"HELLO"s));
    TEST(! cmp_ei(u8"HELLO WORLD"s, u8"hello"s));
    TEST(! cmp_ei(u8"hello world"s, u8"HELLO!WORLD"s));
    TEST(! cmp_ei(u8"HELLO WORLD"s, u8"hello!world"s));

    TEST(! cmp_li(u8""s, u8""s));
    TEST(! cmp_li(u8"hello"s, u8"hello"s));
    TEST(! cmp_li(u8"Hello"s, u8"hello"s));
    TEST(! cmp_li(u8"hello"s, u8"Hello"s));
    TEST(! cmp_li(u8"HELLO"s, u8"hello"s));
    TEST(! cmp_li(u8"hello"s, u8"HELLO"s));
    TEST(cmp_li(u8""s, u8"hello"s));
    TEST(! cmp_li(u8"hello"s, u8""s));
    TEST(cmp_li(u8"hello"s, u8"WORLD"s));
    TEST(cmp_li(u8"HELLO"s, u8"world"s));
    TEST(! cmp_li(u8"world"s, u8"HELLO"s));
    TEST(! cmp_li(u8"WORLD"s, u8"hello"s));
    TEST(cmp_li(u8"hello"s, u8"HELLO WORLD"s));
    TEST(cmp_li(u8"HELLO"s, u8"hello world"s));
    TEST(! cmp_li(u8"hello world"s, u8"HELLO"s));
    TEST(! cmp_li(u8"HELLO WORLD"s, u8"hello"s));
    TEST(cmp_li(u8"hello world"s, u8"HELLO!WORLD"s));
    TEST(cmp_li(u8"HELLO WORLD"s, u8"hello!world"s));

    TEST_EQUAL(cmp_ti(u8""s, u8""s), 0);
    TEST_EQUAL(cmp_ti(u8"hello"s, u8"hello"s), 0);
    TEST_EQUAL(cmp_ti(u8"Hello"s, u8"hello"s), 0);
    TEST_EQUAL(cmp_ti(u8"hello"s, u8"Hello"s), 0);
    TEST_EQUAL(cmp_ti(u8"HELLO"s, u8"hello"s), 0);
    TEST_EQUAL(cmp_ti(u8"hello"s, u8"HELLO"s), 0);
    TEST_EQUAL(cmp_ti(u8""s, u8"hello"s), -1);
    TEST_EQUAL(cmp_ti(u8"hello"s, u8""s), 1);
    TEST_EQUAL(cmp_ti(u8"hello"s, u8"WORLD"s), -1);
    TEST_EQUAL(cmp_ti(u8"HELLO"s, u8"world"s), -1);
    TEST_EQUAL(cmp_ti(u8"world"s, u8"HELLO"s), 1);
    TEST_EQUAL(cmp_ti(u8"WORLD"s, u8"hello"s), 1);
    TEST_EQUAL(cmp_ti(u8"hello"s, u8"HELLO WORLD"s), -1);
    TEST_EQUAL(cmp_ti(u8"HELLO"s, u8"hello world"s), -1);
    TEST_EQUAL(cmp_ti(u8"hello world"s, u8"HELLO"s), 1);
    TEST_EQUAL(cmp_ti(u8"HELLO WORLD"s, u8"hello"s), 1);
    TEST_EQUAL(cmp_ti(u8"hello world"s, u8"HELLO!WORLD"s), -1);
    TEST_EQUAL(cmp_ti(u8"HELLO WORLD"s, u8"hello!world"s), -1);

    TEST(cmp_eif(u8""s, u8""s));
    TEST(cmp_eif(u8"hello"s, u8"hello"s));
    TEST(! cmp_eif(u8"Hello"s, u8"hello"s));
    TEST(! cmp_eif(u8"hello"s, u8"Hello"s));
    TEST(! cmp_eif(u8"HELLO"s, u8"hello"s));
    TEST(! cmp_eif(u8"hello"s, u8"HELLO"s));
    TEST(! cmp_eif(u8""s, u8"hello"s));
    TEST(! cmp_eif(u8"hello"s, u8""s));
    TEST(! cmp_eif(u8"hello"s, u8"WORLD"s));
    TEST(! cmp_eif(u8"HELLO"s, u8"world"s));
    TEST(! cmp_eif(u8"world"s, u8"HELLO"s));
    TEST(! cmp_eif(u8"WORLD"s, u8"hello"s));
    TEST(! cmp_eif(u8"hello"s, u8"HELLO WORLD"s));
    TEST(! cmp_eif(u8"HELLO"s, u8"hello world"s));
    TEST(! cmp_eif(u8"hello world"s, u8"HELLO"s));
    TEST(! cmp_eif(u8"HELLO WORLD"s, u8"hello"s));
    TEST(! cmp_eif(u8"hello world"s, u8"HELLO!WORLD"s));
    TEST(! cmp_eif(u8"HELLO WORLD"s, u8"hello!world"s));

    TEST(! cmp_lif(u8""s, u8""s));
    TEST(! cmp_lif(u8"hello"s, u8"hello"s));
    TEST(cmp_lif(u8"Hello"s, u8"hello"s));
    TEST(! cmp_lif(u8"hello"s, u8"Hello"s));
    TEST(cmp_lif(u8"HELLO"s, u8"hello"s));
    TEST(! cmp_lif(u8"hello"s, u8"HELLO"s));
    TEST(cmp_lif(u8""s, u8"hello"s));
    TEST(! cmp_lif(u8"hello"s, u8""s));
    TEST(cmp_lif(u8"hello"s, u8"WORLD"s));
    TEST(cmp_lif(u8"HELLO"s, u8"world"s));
    TEST(! cmp_lif(u8"world"s, u8"HELLO"s));
    TEST(! cmp_lif(u8"WORLD"s, u8"hello"s));
    TEST(cmp_lif(u8"hello"s, u8"HELLO WORLD"s));
    TEST(cmp_lif(u8"HELLO"s, u8"hello world"s));
    TEST(! cmp_lif(u8"hello world"s, u8"HELLO"s));
    TEST(! cmp_lif(u8"HELLO WORLD"s, u8"hello"s));
    TEST(cmp_lif(u8"hello world"s, u8"HELLO!WORLD"s));
    TEST(cmp_lif(u8"HELLO WORLD"s, u8"hello!world"s));

    TEST_EQUAL(cmp_tif(u8""s, u8""s), 0);
    TEST_EQUAL(cmp_tif(u8"hello"s, u8"hello"s), 0);
    TEST_EQUAL(cmp_tif(u8"Hello"s, u8"hello"s), -1);
    TEST_EQUAL(cmp_tif(u8"hello"s, u8"Hello"s), 1);
    TEST_EQUAL(cmp_tif(u8"HELLO"s, u8"hello"s), -1);
    TEST_EQUAL(cmp_tif(u8"hello"s, u8"HELLO"s), 1);
    TEST_EQUAL(cmp_tif(u8""s, u8"hello"s), -1);
    TEST_EQUAL(cmp_tif(u8"hello"s, u8""s), 1);
    TEST_EQUAL(cmp_tif(u8"hello"s, u8"WORLD"s), -1);
    TEST_EQUAL(cmp_tif(u8"HELLO"s, u8"world"s), -1);
    TEST_EQUAL(cmp_tif(u8"world"s, u8"HELLO"s), 1);
    TEST_EQUAL(cmp_tif(u8"WORLD"s, u8"hello"s), 1);
    TEST_EQUAL(cmp_tif(u8"hello"s, u8"HELLO WORLD"s), -1);
    TEST_EQUAL(cmp_tif(u8"HELLO"s, u8"hello world"s), -1);
    TEST_EQUAL(cmp_tif(u8"hello world"s, u8"HELLO"s), 1);
    TEST_EQUAL(cmp_tif(u8"HELLO WORLD"s, u8"hello"s), 1);
    TEST_EQUAL(cmp_tif(u8"hello world"s, u8"HELLO!WORLD"s), -1);
    TEST_EQUAL(cmp_tif(u8"HELLO WORLD"s, u8"hello!world"s), -1);

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
