#include "unicorn/core.hpp"
#include "unicorn/table.hpp"
#include "prion/unit-test.hpp"
#include <sstream>
#include <string>

using namespace Unicorn;

namespace {

    void check_layout() {

        Table tab;
        u8string s;
        u16string s16;
        u32string s32;
        wstring ws;

        TRY(s = tab.str());
        TEST_EQUAL(s, "");

        {
            std::ostringstream out;
            TRY(tab.write(out));
            u8string s = out.str();
            TEST_EQUAL(s, "");
        }

        TRY(tab << "alpha" << "bravo" << "charlie" << Table::endl);
        TRY(tab << 1 << 2 << 3 << Table::endl);
        TRY(s = tab.str());
        TEST_EQUAL(s,
            "alpha  bravo  charlie\n"
            "1      2      3\n"
        );
        TRY(s16 = tab.as_string<char16_t>());
        TEST_EQUAL(s16,
            u"alpha  bravo  charlie\n"
            u"1      2      3\n"
        );
        TRY(s32 = tab.as_string<char32_t>());
        TEST_EQUAL(s32,
            U"alpha  bravo  charlie\n"
            U"1      2      3\n"
        );
        TRY(ws = tab.as_string<wchar_t>());
        TEST_EQUAL(ws,
            L"alpha  bravo  charlie\n"
            L"1      2      3\n"
        );

        {
            std::ostringstream out;
            TRY(tab.write(out));
            u8string s = out.str();
            TEST_EQUAL(s,
                "alpha  bravo  charlie\n"
                "1      2      3\n"
            );
        }

        {
            std::wostringstream out;
            TRY(tab.write(out));
            wstring s = out.str();
            TEST_EQUAL(s,
                L"alpha  bravo  charlie\n"
                L"1      2      3\n"
            );
        }

        TRY(s = tab.str(Table::margin=4, Table::spacing=3));
        TEST_EQUAL(s,
            "    alpha   bravo   charlie\n"
            "    1       2       3\n"
        );

        TRY(tab.clear_all());
        TRY(tab << "alpha" << "bravo" << "charlie" << Table::endl);
        TRY(tab.div());
        TRY(tab << 10 << 20 << 30 << Table::endl);
        TRY(tab << 40 << Table::endl);
        TRY(tab << "" << 50 << Table::endl);
        TRY(tab << "" << "" << 60 << Table::endl);
        TRY(tab << "" << "" << "" << 70 << Table::endl);
        TRY(s = tab.str());
        TEST_EQUAL(s,
            "alpha  bravo  charlie  --\n"
            "-----  -----  -------  --\n"
            "10     20     30       --\n"
            "40     --     --       --\n"
            "--     50     --       --\n"
            "--     --     60       --\n"
            "--     --     --       70\n"
        );

        TRY(tab.clear_all());
        TRY(tab << "alpha" << "bravo" << "charlie" << Table::endl);
        TRY(tab.div());
        TRY(tab << 10 << 20 << 30 << Table::endl);
        TRY(tab << 10 << 9 << 8 << Table::endl);
        TRY(tab << 10 << 9 << 10 << Table::endl);
        TRY(tab << 10 << 9 << 10 << Table::endl);
        TRY(s = tab.str());
        TEST_EQUAL(s,
            "alpha  bravo  charlie\n"
            "-----  -----  -------\n"
            "10     20     30\n"
            "10     9      8\n"
            "10     9      10\n"
            "10     9      10\n"
        );
        TRY(s = tab.str(Table::unfill));
        TEST_EQUAL(s,
            "alpha  bravo  charlie\n"
            "-----  -----  -------\n"
            "10     20     30\n"
            "''     9      8\n"
            "''     ''     10\n"
            "''     ''     ''\n"
        );

    }

    void check_unicode_length() {

        Table tab;
        u8string s;

        TRY(tab << u8"UPPER" << u8"lower" << u8"..." << Table::endl);
        TRY(tab << u8"ΑΒΓΔΕ" << u8"αβγδε" << u8"..." << Table::endl);
        TRY(s = tab.str());
        TEST_EQUAL(s,
            u8"UPPER  lower  ...\n"
            u8"ΑΒΓΔΕ  αβγδε  ...\n"
        );

        TRY(tab.clear_all());
        TRY(tab << u8"水星" << u8"金星" << u8"地球" << u8"火星" << u8"..." << Table::endl);
        TRY(tab << u8"木星" << u8"土星" << u8"天王星" << u8"海王星" << u8"..." << Table::endl);
        TRY(s = tab.str(Table::flags=Unicorn::grapheme_units|Unicorn::wide_context));
        TEST_EQUAL(s,
            u8"水星  金星  地球    火星    ...\n"
            u8"木星  土星  天王星  海王星  ...\n"
        );

    }

    void check_example_from_docs() {

        Table tab;
        u8string s;

        TRY(tab << "Name" << "Number" << "Hex" << "Float" << Table::endl);
        TRY(tab.div());
        TRY(tab.format("", "", "0x$1x4", "$1fs4"));
        TRY(tab << "Patrick McGoohan" << 6 << 6 << 6.0 << Table::endl);
        TRY(tab << "James Bond" << 007 << 007 << 007.0 << Table::endl);
        TRY(tab << "Douglas Adams" << 42 << 42 << 42.0 << Table::endl);
        TRY(tab << "Maxwell Smart" << 86 << 86 << 86.0 << Table::endl);
        TRY(s = tab.str());
        TEST_EQUAL(s,
            "Name              Number  Hex     Float\n"
            "----------------  ------  ------  --------\n"
            "Patrick McGoohan  6       0x0006  +6.0000\n"
            "James Bond        7       0x0007  +7.0000\n"
            "Douglas Adams     42      0x002a  +42.0000\n"
            "Maxwell Smart     86      0x0056  +86.0000\n"
        );

    }

}

TEST_MODULE(unicorn, table) {

    check_layout();
    check_unicode_length();
    check_example_from_docs();

}
