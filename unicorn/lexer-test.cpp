#include "unicorn/core.hpp"
#include "unicorn/lexer.hpp"
#include "unicorn/regex.hpp"
#include "prion/unit-test.hpp"
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <string>

using namespace Unicorn;
using namespace Unicorn::Literals;
using namespace std::literals;

namespace {

    template <typename Range>
    void lexdump(const Range& tokens, u8string& text) {
        text.clear();
        for (auto& t: tokens) {
            text += dec(t.tag);
            text += ": ";
            text += u8string(t);
            text += "\n";
        }
    }

    void check_unicode_lexer() {

        Lexer lex;
        TokenIterator it;
        Irange<TokenIterator> range;
        u8string s, t;
        vector<u8string> v;

        TRY(range = lex(s));
        TEST(range.begin() == range.end());

        TRY(lex.match(0, "\\s+"));
        TRY(lex.match(0, "#[^\\n]*"));
        TRY(lex.match(1, "[a-z]\\w*", rx_caseless));
        TRY(lex.match(2, "\\d+"));
        TRY(lex.match(3, "[-+*/=]"));
        TRY(lex.exact(4, "<magic>"));

        TRY(range = lex(s));
        TEST(range.begin() == range.end());

        s = "Hello world\n";
        TRY(range = lex(s));
        TEST_EQUAL(std::distance(range.begin(), range.end()), 2);
        TRY(std::copy(range.begin(), range.end(), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,world]");
        TRY(it = range.begin());
        REQUIRE(it != range.end());
        TEST_EQUAL(it->offset, 0);
        TEST_EQUAL(it->count, 5);
        TEST_EQUAL(u8string(*it), "Hello");
        TEST_EQUAL(it->tag, 1);
        TRY(++it);
        REQUIRE(it != range.end());
        TEST_EQUAL(it->offset, 6);
        TEST_EQUAL(it->count, 5);
        TEST_EQUAL(u8string(*it), "world");
        TEST_EQUAL(it->tag, 1);
        TRY(++it);
        TEST(it == range.end());

        s = "Hello world\n"
            "# Comment\n"
            "2 + 2 = 4\n"
            "<magic>\n"
            "Goodbye\n";
        TRY(range = lex(s));

        TRY(lexdump(range, t));
        TEST_EQUAL(t,
            "1: Hello\n"
            "1: world\n"
            "2: 2\n"
            "3: +\n"
            "2: 2\n"
            "3: =\n"
            "2: 4\n"
            "4: <magic>\n"
            "1: Goodbye\n"
        );

        TEST_EQUAL(std::distance(range.begin(), range.end()), 9);
        TRY(std::copy(range.begin(), range.end(), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,world,2,+,2,=,4,<magic>,Goodbye]");

        TRY(lex.custom(5, [] (const u8string& str, size_t pos) -> size_t {
            if (pos >= str.size() || str[pos] != '(')
                return 0;
            int depth = 1;
            auto i = pos;
            for (++i; i < str.size() && depth > 0; ++i) {
                depth += int(str[i] == '(');
                depth -= int(str[i] == ')');
            }
            if (depth == 0)
                return i - pos;
            else
                return 0;
        }));
        s = "Hello"
            "(a b c)"
            "(a b c (d e f) g h i)"
            "Goodbye";
        TRY(range = lex(s));
        TEST_EQUAL(std::distance(range.begin(), range.end()), 4);
        TRY(std::copy(range.begin(), range.end(), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,(a b c),(a b c (d e f) g h i),Goodbye]");

        s = "Hello@world";
        TRY(range = lex(s));
        TRY(it = range.begin());
        TEST_THROW_EQUAL(++it, SyntaxError, "Syntax error at offset 5: Unexpected \"@\"");

    }

    void check_byte_lexer() {

        Lexer lex;
        TokenIterator it;
        Irange<TokenIterator> range;
        string s;
        vector<string> v;

        TRY(lex = Lexer(rx_byte));

        TRY(lex.match(0, "\\s+"));
        TRY(lex.match(0, "#[^\\n]*"));
        TRY(lex.match(1, "[a-z]\\w*", rx_caseless));
        TRY(lex.match(2, "\\d+"));
        TRY(lex.match(3, "[-+*/=]"));
        TRY(lex.exact(4, "<magic>"));

        s = "Hello world\n"
            "# Comment\n"
            "2 + 2 = 4\n"
            "<magic>\n"
            "Goodbye\n";
        TRY(range = lex(s));
        TEST_EQUAL(std::distance(range.begin(), range.end()), 9);
        TRY(std::copy(range.begin(), range.end(), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,world,2,+,2,=,4,<magic>,Goodbye]");

        TRY(lex.custom(5, [] (const string& str, size_t pos) -> size_t {
            if (pos >= str.size() || str[pos] != '(')
                return 0;
            int depth = 1;
            auto i = pos;
            for (++i; i < str.size() && depth > 0; ++i) {
                depth += int(str[i] == '(');
                depth -= int(str[i] == ')');
            }
            if (depth == 0)
                return i - pos;
            else
                return 0;
        }));
        s = "Hello"
            "(a b c)"
            "(a b c (d e f) g h i)"
            "Goodbye";
        TRY(range = lex(s));
        TEST_EQUAL(std::distance(range.begin(), range.end()), 4);
        TRY(std::copy(range.begin(), range.end(), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,(a b c),(a b c (d e f) g h i),Goodbye]");

        s = "Hello@world";
        TRY(range = lex(s));
        TRY(it = range.begin());
        TEST_THROW_EQUAL(++it, SyntaxError, "Syntax error at offset 5: Unexpected \"@\"");

    }

}

TEST_MODULE(unicorn, lexer) {

    check_unicode_lexer();
    check_byte_lexer();

}
