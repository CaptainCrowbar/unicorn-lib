#include "unicorn/core.hpp"
#include "unicorn/lexer.hpp"
#include "unicorn/unit-test.hpp"
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void lexdump(const Lexer::token_range& tokens, u8string& text) {
        text.clear();
        for (auto& t: tokens) {
            text += dec(t.tag);
            text += ": ";
            text += u8string(t);
            text += "\n";
        }
    }

    void check_utf8_lexer() {

        Lexer lex;
        Lexer::token_iterator it;
        Lexer::token_range range;
        u8string s, t;
        std::vector<u8string> v;

        TRY(range = lex(s));
        TEST(std::begin(range) == std::end(range));

        TRY(lex.match(0, "\\s+"));
        TRY(lex.match(0, "#[^\\n]*"));
        TRY(lex.match(1, "[a-z]\\w+", rx_caseless));
        TRY(lex.match(2, "\\d+"));
        TRY(lex.match(3, "[-+*/=]"));
        TRY(lex.exact(4, "<magic>"));

        TRY(range = lex(s));
        TEST(std::begin(range) == std::end(range));

        s = "Hello world\n";
        TRY(range = lex(s));
        TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 2);
        TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,world]");
        TRY(it = std::begin(range));
        REQUIRE(it != std::end(range));
        TEST_EQUAL(it->offset, 0);
        TEST_EQUAL(it->count, 5);
        TEST_EQUAL(u8string(*it), "Hello");
        TEST_EQUAL(it->tag, 1);
        TRY(++it);
        REQUIRE(it != std::end(range));
        TEST_EQUAL(it->offset, 6);
        TEST_EQUAL(it->count, 5);
        TEST_EQUAL(u8string(*it), "world");
        TEST_EQUAL(it->tag, 1);
        TRY(++it);
        TEST(it == std::end(range));

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

        TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 9);
        TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,world,2,+,2,=,4,<magic>,Goodbye]");

        TRY(lex.call(5, [] (const u8string& str, size_t pos) -> size_t {
            if (pos >= str.size() || str[pos] != '(')
                return 0;
            int depth = 1;
            auto i = pos;
            for (++i; i < str.size() && depth > 0; ++i) {
                depth += static_cast<int>(str[i] == '(');
                depth -= static_cast<int>(str[i] == ')');
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
        TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 4);
        TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,(a b c),(a b c (d e f) g h i),Goodbye]");

        s = "Hello@world";
        TRY(range = lex(s));
        TRY(it = std::begin(range));
        TEST_THROW_EQUAL(++it, SyntaxError, "Syntax error at offset 5: Unexpected \"@\"");

    }

    void check_utf16_lexer() {

        #if ! defined(UNICORN_NO_PCRE16)

            Lexer16 lex;
            Lexer16::token_iterator it;
            Lexer16::token_range range;
            std::u16string s;
            std::vector<std::u16string> v;

            TRY(lex.match(0, u"\\s+"));
            TRY(lex.match(0, u"#[^\\n]*"));
            TRY(lex.match(1, u"[a-z]\\w+", rx_caseless));
            TRY(lex.match(2, u"\\d+"));
            TRY(lex.match(3, u"[-+*/=]"));
            TRY(lex.exact(4, u"<magic>"));

            s = u"Hello world\n"
                u"# Comment\n"
                u"2 + 2 = 4\n"
                u"<magic>\n"
                u"Goodbye\n";
            TRY(range = lex(s));
            TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 9);
            TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[Hello,world,2,+,2,=,4,<magic>,Goodbye]");

            TRY(lex.call(5, [] (const std::u16string& str, size_t pos) -> size_t {
                if (pos >= str.size() || str[pos] != u'(')
                    return 0;
                int depth = 1;
                auto i = pos;
                for (++i; i < str.size() && depth > 0; ++i) {
                    depth += static_cast<int>(str[i] == u'(');
                    depth -= static_cast<int>(str[i] == u')');
                }
                if (depth == 0)
                    return i - pos;
                else
                    return 0;
            }));
            s = u"Hello"
                u"(a b c)"
                u"(a b c (d e f) g h i)"
                u"Goodbye";
            TRY(range = lex(s));
            TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 4);
            TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[Hello,(a b c),(a b c (d e f) g h i),Goodbye]");

            s = u"Hello@world";
            TRY(range = lex(s));
            TRY(it = std::begin(range));
            TEST_THROW_EQUAL(++it, SyntaxError, "Syntax error at offset 5: Unexpected \"@\"");

        #endif

    }

    void check_utf32_lexer() {

        #if ! defined(UNICORN_NO_PCRE32)

            Lexer32 lex;
            Lexer32::token_iterator it;
            Lexer32::token_range range;
            std::u32string s;
            std::vector<std::u32string> v;

            TRY(lex.match(0, U"\\s+"));
            TRY(lex.match(0, U"#[^\\n]*"));
            TRY(lex.match(1, U"[a-z]\\w+", rx_caseless));
            TRY(lex.match(2, U"\\d+"));
            TRY(lex.match(3, U"[-+*/=]"));
            TRY(lex.exact(4, U"<magic>"));

            s = U"Hello world\n"
                U"# Comment\n"
                U"2 + 2 = 4\n"
                U"<magic>\n"
                U"Goodbye\n";
            TRY(range = lex(s));
            TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 9);
            TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[Hello,world,2,+,2,=,4,<magic>,Goodbye]");

            TRY(lex.call(5, [] (const std::u32string& str, size_t pos) -> size_t {
                if (pos >= str.size() || str[pos] != U'(')
                    return 0;
                int depth = 1;
                auto i = pos;
                for (++i; i < str.size() && depth > 0; ++i) {
                    depth += static_cast<int>(str[i] == u'(');
                    depth -= static_cast<int>(str[i] == u')');
                }
                if (depth == 0)
                    return i - pos;
                else
                    return 0;
            }));
            s = U"Hello"
                U"(a b c)"
                U"(a b c (d e f) g h i)"
                U"Goodbye";
            TRY(range = lex(s));
            TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 4);
            TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[Hello,(a b c),(a b c (d e f) g h i),Goodbye]");

            s = U"Hello@world";
            TRY(range = lex(s));
            TRY(it = std::begin(range));
            TEST_THROW_EQUAL(++it, SyntaxError, "Syntax error at offset 5: Unexpected \"@\"");

        #endif

    }

    void check_wchar_lexer() {

        #if ! defined(UNICORN_NO_PCRE_WCHAR)

            WideLexer lex;
            WideLexer::token_iterator it;
            WideLexer::token_range range;
            std::wstring s;
            std::vector<std::wstring> v;

            TRY(lex.match(0, L"\\s+"));
            TRY(lex.match(0, L"#[^\\n]*"));
            TRY(lex.match(1, L"[a-z]\\w+", rx_caseless));
            TRY(lex.match(2, L"\\d+"));
            TRY(lex.match(3, L"[-+*/=]"));
            TRY(lex.exact(4, L"<magic>"));

            s = L"Hello world\n"
                L"# Comment\n"
                L"2 + 2 = 4\n"
                L"<magic>\n"
                L"Goodbye\n";
            TRY(range = lex(s));
            TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 9);
            TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[Hello,world,2,+,2,=,4,<magic>,Goodbye]");

            TRY(lex.call(5, [] (const std::wstring& str, size_t pos) -> size_t {
                if (pos >= str.size() || str[pos] != L'(')
                    return 0;
                int depth = 1;
                auto i = pos;
                for (++i; i < str.size() && depth > 0; ++i) {
                    depth += static_cast<int>(str[i] == L'(');
                    depth -= static_cast<int>(str[i] == L')');
                }
                if (depth == 0)
                    return i - pos;
                else
                    return 0;
            }));
            s = L"Hello"
                L"(a b c)"
                L"(a b c (d e f) g h i)"
                L"Goodbye";
            TRY(range = lex(s));
            TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 4);
            TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[Hello,(a b c),(a b c (d e f) g h i),Goodbye]");

            s = L"Hello@world";
            TRY(range = lex(s));
            TRY(it = std::begin(range));
            TEST_THROW_EQUAL(++it, SyntaxError, "Syntax error at offset 5: Unexpected \"@\"");

        #endif

    }

    void check_byte_lexer() {

        ByteLexer lex;
        ByteLexer::token_iterator it;
        ByteLexer::token_range range;
        std::string s;
        std::vector<std::string> v;

        TRY(lex.match(0, "\\s+"));
        TRY(lex.match(0, "#[^\\n]*"));
        TRY(lex.match(1, "[a-z]\\w+", rx_caseless));
        TRY(lex.match(2, "\\d+"));
        TRY(lex.match(3, "[-+*/=]"));
        TRY(lex.exact(4, "<magic>"));

        s = "Hello world\n"
            "# Comment\n"
            "2 + 2 = 4\n"
            "<magic>\n"
            "Goodbye\n";
        TRY(range = lex(s));
        TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 9);
        TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,world,2,+,2,=,4,<magic>,Goodbye]");

        TRY(lex.call(5, [] (const std::string& str, size_t pos) -> size_t {
            if (pos >= str.size() || str[pos] != '(')
                return 0;
            int depth = 1;
            auto i = pos;
            for (++i; i < str.size() && depth > 0; ++i) {
                depth += static_cast<int>(str[i] == '(');
                depth -= static_cast<int>(str[i] == ')');
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
        TEST_EQUAL(std::distance(CROW_BOUNDS(range)), 4);
        TRY(std::copy(CROW_BOUNDS(range), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,(a b c),(a b c (d e f) g h i),Goodbye]");

        s = "Hello@world";
        TRY(range = lex(s));
        TRY(it = std::begin(range));
        TEST_THROW_EQUAL(++it, SyntaxError, "Syntax error at offset 5: Unexpected \"@\"");

    }

}

TEST_MODULE(unicorn, lexer) {

    check_utf8_lexer();
    check_utf16_lexer();
    check_utf32_lexer();
    check_wchar_lexer();
    check_byte_lexer();

}
