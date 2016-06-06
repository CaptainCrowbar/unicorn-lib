#include "unicorn/core.hpp"
#include "unicorn/text-table.hpp"
#include "prion/unit-test.hpp"
#include <sstream>
#include <string>

using namespace Unicorn;
using namespace std::literals;

namespace {

    void check_layout() {

        Table tab;
        u8string s;

        TRY(s = tab.str());
        TEST_EQUAL(s, "");

        {
            std::ostringstream out;
            TRY(tab.write(out));
            u8string s = out.str();
            TEST_EQUAL(s, "");
        }

        TRY(tab << "alpha" << "bravo" << "charlie" << '\n');
        TRY(tab << 1 << 2 << 3 << '\n');
        TRY(s = tab.str());
        TEST_EQUAL(s,
            "alpha  bravo  charlie\n"
            "1      2      3\n"
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

        TRY(s = tab.str(tab_margin=4, tab_spacing=3));
        TEST_EQUAL(s,
            "    alpha   bravo   charlie\n"
            "    1       2       3\n"
        );

        TRY(tab.clear());
        TRY(tab << "alpha" << "bravo" << "charlie" << '\n');
        TRY(tab << '=');
        TRY(tab << 10 << 20 << 30 << '\n');
        TRY(tab << 40 << '\n');
        TRY(tab << "" << 50 << '\n');
        TRY(tab << "" << "" << 60 << '\n');
        TRY(tab << "" << "" << "" << 70 << '\n');
        TRY(s = tab.str());
        TEST_EQUAL(s,
            "alpha  bravo  charlie  --\n"
            "=====  =====  =======  ==\n"
            "10     20     30       --\n"
            "40     --     --       --\n"
            "--     50     --       --\n"
            "--     --     60       --\n"
            "--     --     --       70\n"
        );

        TRY(tab.clear());
        TRY(tab << "alpha" << "bravo" << "charlie" << '\n');
        TRY(tab << '=');
        TRY(tab << 10 << 20 << 30 << '\n');
        TRY(tab << 10 << 9 << 8 << '\n');
        TRY(tab << 10 << 9 << 10 << '\n');
        TRY(tab << 10 << 9 << 10 << '\n');
        TRY(s = tab.str());
        TEST_EQUAL(s,
            "alpha  bravo  charlie\n"
            "=====  =====  =======\n"
            "10     20     30\n"
            "10     9      8\n"
            "10     9      10\n"
            "10     9      10\n"
        );
        TRY(s = tab.str(tab_unfill));
        TEST_EQUAL(s,
            "alpha  bravo  charlie\n"
            "=====  =====  =======\n"
            "10     20     30\n"
            "''     9      8\n"
            "''     ''     10\n"
            "''     ''     ''\n"
        );

    }

    void check_unicode_length() {

        Table tab;
        u8string s;

        TRY(tab << u8"UPPER" << u8"lower" << u8"..." << '\n');
        TRY(tab << u8"ΑΒΓΔΕ" << u8"αβγδε" << u8"..." << '\n');
        TRY(s = tab.str());
        TEST_EQUAL(s,
            u8"UPPER  lower  ...\n"
            u8"ΑΒΓΔΕ  αβγδε  ...\n"
        );

        TRY(tab.clear());
        TRY(tab << u8"水星" << u8"金星" << u8"地球" << u8"火星" << u8"..." << '\n');
        TRY(tab << u8"木星" << u8"土星" << u8"天王星" << u8"海王星" << u8"..." << '\n');
        TRY(s = tab.str(tab_flags=grapheme_units|wide_context));
        TEST_EQUAL(s,
            u8"水星  金星  地球    火星    ...\n"
            u8"木星  土星  天王星  海王星  ...\n"
        );

    }

    void check_utf_strings() {

        Table tab;
        u8string s;

        TRY(tab << 1 << u8"abcde" << u8"αβγδε" << '\n');
        TRY(tab << 2 << u8"abcde"s << u8"αβγδε"s << '\n');
        TRY(tab << 3 << u"abcde" << u"αβγδε" << '\n');
        TRY(tab << 4 << u"abcde"s << u"αβγδε"s << '\n');
        TRY(tab << 5 << U"abcde" << U"αβγδε" << '\n');
        TRY(tab << 6 << U"abcde"s << U"αβγδε"s << '\n');
        TRY(tab << 7 << L"abcde" << L"αβγδε" << '\n');
        TRY(tab << 8 << L"abcde"s << L"αβγδε"s << '\n');
        TRY(s = tab.str());
        TEST_EQUAL(s,
            u8"1  abcde  αβγδε\n"
            u8"2  abcde  αβγδε\n"
            u8"3  abcde  αβγδε\n"
            u8"4  abcde  αβγδε\n"
            u8"5  abcde  αβγδε\n"
            u8"6  abcde  αβγδε\n"
            u8"7  abcde  αβγδε\n"
            u8"8  abcde  αβγδε\n"
        );

    }

    void check_example_from_docs() {

        Table tab;
        u8string s;

        TRY(tab << "Name" << "Number" << "Hex" << "Float" << '\n'
            << '=');
        TRY(tab.format("", "", "0x$1x3", "$1fs2"));
        TRY(tab << "Patrick McGoohan" << 6 << 6 << 6 << '\n'
            << "James Bond" << 007 << 007 << 007 << '\n'
            << "Douglas Adams" << 42 << 42 << 42 << '\n'
            << "Maxwell Smart" << 86 << 86 << 86 << '\n');
        TRY(s = tab.str());
        TEST_EQUAL(s,
            "Name              Number  Hex    Float\n"
            "================  ======  =====  ======\n"
            "Patrick McGoohan  6       0x006  +6.00\n"
            "James Bond        7       0x007  +7.00\n"
            "Douglas Adams     42      0x02a  +42.00\n"
            "Maxwell Smart     86      0x056  +86.00\n"
        );

    }

}

TEST_MODULE(unicorn, text_table) {

    check_layout();
    check_unicode_length();
    check_utf_strings();
    check_example_from_docs();

}
