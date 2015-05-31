#include "crow/unit-test.hpp"
#include "unicorn/core.hpp"
#include "unicorn/regex.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std::literals;
using namespace Crow;
using namespace Unicorn;
using namespace Unicorn::Literals;

namespace {

    void check_version_information() {

        auto ver = regex_version();
        std::cout << "... PCRE version: " << ver << "\n";
        TEST_COMPARE(ver, >=, (Version{8,0,0}));

    }

    void check_utf8_regex() {

        u8string s;
        Regex r;
        Match m;
        Utf8Iterator u;

        TEST(r.empty());
        TEST_EQUAL(r.groups(), 1);
        TEST_EQUAL(r.pattern(), "");
        TEST_EQUAL(r.flags(), 0);

        TEST(! m.matched());
        TEST_EQUAL(u_str(CROW_BOUNDS(m)), "");

        TRY(r = Regex("[a-z]+"));
        TEST(! r.empty());
        TEST_EQUAL(r.groups(), 1);
        TEST_EQUAL(r.pattern(), "[a-z]+");
        TEST_EQUAL(r.flags(), 0);
        s = "Hello world";
        TRY(m = r.search(s));
        TEST(m.matched());
        TEST(! m.partial());
        TEST(m.full_or_partial());
        TEST(! m.empty());
        TEST_EQUAL(m.groups(), 1);
        TEST_EQUAL(m.offset(), 1);
        TEST_EQUAL(m.endpos(), 5);
        TEST_EQUAL(m.count(), 4);
        TEST_EQUAL(m.str(), "ello");
        TEST_EQUAL(m.str(0), "ello");
        TEST_EQUAL(m.str(1), "");
        TEST_EQUAL(u_str(CROW_BOUNDS(m)), "ello");
        TEST_EQUAL(u_str(m.begin(0), m.end(0)), "ello");
        TEST_EQUAL(u_str(m.begin(1), m.end(1)), "");
        s = "42";
        TRY(m = r.search(s));
        TEST(! m.matched());
        TEST(! m.partial());
        TEST(! m.full_or_partial());
        TEST(m.empty());
        TEST_EQUAL(m.groups(), 0);
        TEST_EQUAL(m.offset(), npos);
        TEST_EQUAL(m.endpos(), npos);
        TEST_EQUAL(m.count(), 0);
        TEST_EQUAL(m.str(), "");
        TEST_EQUAL(u_str(CROW_BOUNDS(m)), "");

        TRY(r = Regex("[a-z]+", rx_dfa));
        TEST_EQUAL(r.groups(), 1);
        TEST_EQUAL(r.pattern(), "[a-z]+");
        TEST_EQUAL(r.flags(), rx_dfa);
        s = "Hello world";
        TRY(m = r.search(s));
        TEST(m.matched());
        TEST(! m.empty());
        TEST_EQUAL(m.offset(), 1);
        TEST_EQUAL(m.endpos(), 5);
        TEST_EQUAL(m.count(), 4);
        TEST_EQUAL(m.str(), "ello");
        s = "42";
        TRY(m = r.search(s));
        TEST(! m.matched());
        TEST(m.empty());
        TEST_EQUAL(m.offset(), npos);
        TEST_EQUAL(m.endpos(), npos);
        TEST_EQUAL(m.count(), 0);
        TEST_EQUAL(m.str(), "");

        TRY(r = Regex("[a-z]*", rx_notempty));
        TEST_EQUAL(r.groups(), 1);
        TEST_EQUAL(r.pattern(), "[a-z]*");
        TEST_EQUAL(r.flags(), rx_notempty);
        s = "Hello world";
        TRY(m = r.search(s));
        TEST(m.matched());
        TEST(! m.empty());
        TEST_EQUAL(m.groups(), 1);
        TEST_EQUAL(m.offset(), 1);
        TEST_EQUAL(m.endpos(), 5);
        TEST_EQUAL(m.count(), 4);
        TEST_EQUAL(m.str(), "ello");
        s = "42";
        TRY(m = r.search(s));
        TEST(! m.matched());
        TEST(m.empty());
        TEST_EQUAL(m.groups(), 0);
        TEST_EQUAL(m.offset(), npos);
        TEST_EQUAL(m.endpos(), npos);
        TEST_EQUAL(m.count(), 0);
        TEST_EQUAL(m.str(), "");

        TRY(r = Regex("[a-z]+", rx_caseless));
        TEST_EQUAL(r.groups(), 1);
        TEST_EQUAL(r.pattern(), "[a-z]+");
        TEST_EQUAL(r.flags(), rx_caseless);
        s = "Hello world";
        TRY(m = r.search(s));
        TEST(m.matched());
        TEST(! m.empty());
        TEST_EQUAL(m.groups(), 1);
        TEST_EQUAL(m.offset(), 0);
        TEST_EQUAL(m.endpos(), 5);
        TEST_EQUAL(m.count(), 5);
        TEST_EQUAL(m.str(), "Hello");
        s = "42";
        TRY(m = r.search(s));
        TEST(! m.matched());
        TEST(m.empty());
        TEST_EQUAL(m.groups(),0);
        TEST_EQUAL(m.offset(),npos);
        TEST_EQUAL(m.endpos(),npos);
        TEST_EQUAL(m.count(),0);
        TEST_EQUAL(m.str(),"");

        TRY(r = Regex("[a-z]+", rx_caseless));
        s = "Hello world";
        TRY(m = r.search(s, 0));   TEST(m.matched());    TEST_EQUAL(m.offset(), 0);     TEST_EQUAL(m.str(), "Hello");
        TRY(m = r.search(s, 1));   TEST(m.matched());    TEST_EQUAL(m.offset(), 1);     TEST_EQUAL(m.str(), "ello");
        TRY(m = r.search(s, 2));   TEST(m.matched());    TEST_EQUAL(m.offset(), 2);     TEST_EQUAL(m.str(), "llo");
        TRY(m = r.search(s, 3));   TEST(m.matched());    TEST_EQUAL(m.offset(), 3);     TEST_EQUAL(m.str(), "lo");
        TRY(m = r.search(s, 4));   TEST(m.matched());    TEST_EQUAL(m.offset(), 4);     TEST_EQUAL(m.str(), "o");
        TRY(m = r.search(s, 5));   TEST(m.matched());    TEST_EQUAL(m.offset(), 6);     TEST_EQUAL(m.str(), "world");
        TRY(m = r.search(s, 6));   TEST(m.matched());    TEST_EQUAL(m.offset(), 6);     TEST_EQUAL(m.str(), "world");
        TRY(m = r.search(s, 7));   TEST(m.matched());    TEST_EQUAL(m.offset(), 7);     TEST_EQUAL(m.str(), "orld");
        TRY(m = r.search(s, 8));   TEST(m.matched());    TEST_EQUAL(m.offset(), 8);     TEST_EQUAL(m.str(), "rld");
        TRY(m = r.search(s, 9));   TEST(m.matched());    TEST_EQUAL(m.offset(), 9);     TEST_EQUAL(m.str(), "ld");
        TRY(m = r.search(s, 10));  TEST(m.matched());    TEST_EQUAL(m.offset(), 10);    TEST_EQUAL(m.str(), "d");
        TRY(m = r.search(s, 11));  TEST(! m.matched());  TEST_EQUAL(m.offset(), npos);  TEST_EQUAL(m.str(), "");
        TRY(m = r.search(s, 12));  TEST(! m.matched());  TEST_EQUAL(m.offset(), npos);  TEST_EQUAL(m.str(), "");
        TRY(u = utf_begin(s));
        TRY(m = r.search(u));    TEST(m.matched());    TEST_EQUAL(m.offset(), 0);     TEST_EQUAL(m.str(), "Hello");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 1);     TEST_EQUAL(m.str(), "ello");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 2);     TEST_EQUAL(m.str(), "llo");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 3);     TEST_EQUAL(m.str(), "lo");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 4);     TEST_EQUAL(m.str(), "o");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 6);     TEST_EQUAL(m.str(), "world");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 6);     TEST_EQUAL(m.str(), "world");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 7);     TEST_EQUAL(m.str(), "orld");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 8);     TEST_EQUAL(m.str(), "rld");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 9);     TEST_EQUAL(m.str(), "ld");
        TRY(m = r.search(++u));  TEST(m.matched());    TEST_EQUAL(m.offset(), 10);    TEST_EQUAL(m.str(), "d");
        TRY(m = r.search(++u));  TEST(! m.matched());  TEST_EQUAL(m.offset(), npos);  TEST_EQUAL(m.str(), "");

        TRY(r = Regex("([a-z]+) ([a-z]+)", rx_caseless));
        TEST_EQUAL(r.groups(), 3);
        TEST_EQUAL(r.pattern(), "([a-z]+) ([a-z]+)");
        TEST_EQUAL(r.flags(), rx_caseless);
        s = "Hello world";
        TRY(m = r.search(s));
        TEST(m.matched());
        TEST(! m.empty());
        TEST_EQUAL(m.groups(), 3);
        TEST_EQUAL(m.offset(), 0);
        TEST_EQUAL(m.endpos(), 11);
        TEST_EQUAL(m.count(), 11);
        TEST_EQUAL(m.str(), "Hello world");
        TEST_EQUAL(m.str(0), "Hello world");
        TEST_EQUAL(m[0], "Hello world");
        TEST_EQUAL(u_str(CROW_BOUNDS(m)), "Hello world");
        TEST_EQUAL(u_str(m.begin(0), m.end(0)), "Hello world");
        TEST_EQUAL(m.offset(1), 0);
        TEST_EQUAL(m.endpos(1), 5);
        TEST_EQUAL(m.count(1), 5);
        TEST_EQUAL(m.str(1), "Hello");
        TEST_EQUAL(m[1], "Hello");
        TEST_EQUAL(u_str(m.begin(1), m.end(1)), "Hello");
        TEST_EQUAL(m.offset(2), 6);
        TEST_EQUAL(m.endpos(2), 11);
        TEST_EQUAL(m.count(2), 5);
        TEST_EQUAL(m.str(2), "world");
        TEST_EQUAL(m[2], "world");
        TEST_EQUAL(u_str(m.begin(2), m.end(2)), "world");
        TEST(m.matched(0));
        TEST(m.matched(1));
        TEST(m.matched(2));
        TEST(! m.matched(3));

        TRY(r = Regex("(?<head>[a-z]+) (?<tail>[a-z]+)", rx_caseless));
        TEST_EQUAL(r.named("head"), 1);
        TEST_EQUAL(r.named("tail"), 2);
        TEST_EQUAL(r.named("none"), npos);
        TEST_EQUAL(r.named(""), npos);
        s = "Hello world";
        TRY(m = r.search(s));
        TEST(m.matched());
        TEST_EQUAL(m.named("head"), "Hello");
        TEST_EQUAL(m.named("tail"), "world");
        TEST_EQUAL(m.named("none"), "");
        TEST_EQUAL(m.named(""), "");

        TRY(r = Regex("[a-z]+", rx_caseless));
        s = "Hello";          TRY(m = r.anchor(s));  TEST(m.matched());    TEST_EQUAL(m.str(), "Hello");
        s = "Hello world";    TRY(m = r.anchor(s));  TEST(m.matched());    TEST_EQUAL(m.str(), "Hello");
        s = "(Hello world)";  TRY(m = r.anchor(s));  TEST(! m.matched());  TEST_EQUAL(m.str(), "");
        s = "Hello";          TRY(m = r.match(s));   TEST(m.matched());    TEST_EQUAL(m.str(), "Hello");
        s = "Hello world";    TRY(m = r.match(s));   TEST(! m.matched());  TEST_EQUAL(m.str(), "");

        TRY(r = Regex("[a-z]+", rx_caseless));
        s = "Hello world";  TEST_EQUAL(r.count(s), 2);
        s = "42";           TEST_EQUAL(r.count(s), 0);

        TRY(r = Regex("([a-z]+) ([a-z]+)", rx_caseless));
        s = "Hello";  TRY(m = r.match(s));  TEST(! m);  TEST(! m.partial());  TEST(! m.full_or_partial());

        TRY(r = Regex("([a-z]+) ([a-z]+)", rx_caseless | rx_partialsoft));
        s = "Hello";        TRY(m = r.match(s));  TEST(! m);  TEST(m.partial());    TEST(m.full_or_partial());
        s = "Hello world";  TRY(m = r.match(s));  TEST(m);    TEST(! m.partial());  TEST(m.full_or_partial());
        s = "42";           TRY(m = r.match(s));  TEST(! m);  TEST(! m.partial());  TEST(! m.full_or_partial());

        TRY(r = Regex("([a-z]+) ([a-z]+)", rx_caseless | rx_partialhard));
        s = "Hello";        TRY(m = r.match(s));  TEST(! m);  TEST(m.partial());    TEST(m.full_or_partial());
        s = "Hello world";  TRY(m = r.match(s));  TEST(! m);  TEST(m.partial());    TEST(m.full_or_partial());
        s = "42";           TRY(m = r.match(s));  TEST(! m);  TEST(! m.partial());  TEST(! m.full_or_partial());

        TEST_THROW(r = Regex("]a-z["), RegexError);
        TEST_THROW(r = Regex("x\\yz"), RegexError);

        TRY(r = Regex("(\\w+) (\\w+)", rx_optimize));
        s = "Hello";
        TRY(m = r.match(s));
        TEST(! m);
        s = "Hello world";
        TRY(m = r.match(s));
        TEST(m);
        TEST_EQUAL(m.groups(), 3);
        TEST_EQUAL(m[0], "Hello world");
        TEST_EQUAL(m[1], "Hello");
        TEST_EQUAL(m[2], "world");

        s = "Hello world";
        TRY(r = regex("[a-z]+"));
        TRY(m = r(s));
        TEST_EQUAL(m.str(), "ello");
        TRY(r = regex("[a-z]+", rx_caseless));
        TRY(m = r(s));
        TEST_EQUAL(m.str(), "Hello");

        s = "Hello world";
        TRY(r = "/[a-z]+/"_re);
        TRY(m = r(s));
        TEST_EQUAL(m.str(), "ello");
        TRY(r = "/[a-z]+/i"_re);
        TRY(m = r(s));
        TEST_EQUAL(m.str(), "Hello");
        s = "Hello/world";
        TRY(r = "/[a-z]+\\/[a-z]+/"_re);
        TRY(m = r(s));
        TEST_EQUAL(m.str(), "ello/world");

    }

    void check_match_ranges() {

        u8string s;
        Regex r;
        Irange<MatchIterator> mr;
        std::vector<u8string> v;

        TRY(r = Regex("[a-z]+", rx_caseless));

        s = "";               TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 0);
        s = "42";             TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 0);
        s = "Hello";          TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 1);  TRY(std::copy(CROW_BOUNDS(mr), overwrite(v)));  TEST_EQUAL(Test::format_range(v), "[Hello]");
        s = "(Hello)";        TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 1);  TRY(std::copy(CROW_BOUNDS(mr), overwrite(v)));  TEST_EQUAL(Test::format_range(v), "[Hello]");
        s = "Hello world";    TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 2);  TRY(std::copy(CROW_BOUNDS(mr), overwrite(v)));  TEST_EQUAL(Test::format_range(v), "[Hello,world]");
        s = "(Hello world)";  TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 2);  TRY(std::copy(CROW_BOUNDS(mr), overwrite(v)));  TEST_EQUAL(Test::format_range(v), "[Hello,world]");

    }

    void check_split_ranges() {

        u8string s;
        Regex r;
        Irange<SplitIterator> sr;

        TRY(r = Regex("/"));

        s = "";                  TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 1);  TEST_EQUAL(Test::format_range(sr), "[]");
        s = "/";                 TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 2);  TEST_EQUAL(Test::format_range(sr), "[,]");
        s = "Hello";             TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 1);  TEST_EQUAL(Test::format_range(sr), "[Hello]");
        s = "Hello/world";       TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 2);  TEST_EQUAL(Test::format_range(sr), "[Hello,world]");
        s = "/Hello/world/";     TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 4);  TEST_EQUAL(Test::format_range(sr), "[,Hello,world,]");
        s = "//Hello//world//";  TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 7);  TEST_EQUAL(Test::format_range(sr), "[,,Hello,,world,,]");

    }

    void check_regex_formatting() {

        u8string s;
        RegexFormat rf;

        TRY(rf = RegexFormat("\\s+", "*"));

        s = "Hello";                TEST_EQUAL(rf.format(s), "Hello");
        s = "Hello";                TEST_EQUAL(rf.format(s, 1), "Hello");
        s = "Hello world";          TEST_EQUAL(rf.format(s), "Hello*world");
        s = "Hello world";          TEST_EQUAL(rf.format(s, 1), "Hello*world");
        s = "\r\nHello world\r\n";  TEST_EQUAL(rf.format(s), "*Hello*world*");
        s = "\r\nHello world\r\n";  TEST_EQUAL(rf.format(s, 1), "*Hello world\r\n");

        TRY(rf = RegexFormat("[A-Z]+|[a-z]+", "($0)"));
        s = "Hello";  TEST_EQUAL(rf.format(s), "(H)(ello)");

        TRY(rf = RegexFormat("[A-Z]+|[a-z]+", "($&)"));
        s = "Hello";  TEST_EQUAL(rf.format(s), "(H)(ello)");

        TRY(rf = RegexFormat("([A-Z]+)([a-z]+)", "($1-$2)"));
        s = "Hello World";  TEST_EQUAL(rf.format(s), "(H-ello) (W-orld)");
        s = "Hello World";  TEST_EQUAL(rf.format(s, 1), "(H-ello) World");

        TRY(rf = RegexFormat("([A-Z]+)([a-z]+)", "(${1}-${2})"));
        s = "Hello World";  TEST_EQUAL(rf.format(s), "(H-ello) (W-orld)");
        s = "Hello World";  TEST_EQUAL(rf.format(s, 1), "(H-ello) World");

        TRY(rf = RegexFormat("(?<head>[A-Z]+)(?<tail>[a-z]+)", "($head-$tail)"));
        s = "Hello World";  TEST_EQUAL(rf.format(s), "(H-ello) (W-orld)");
        s = "Hello World";  TEST_EQUAL(rf.format(s, 1), "(H-ello) World");

        TRY(rf = RegexFormat("(?<head>[A-Z]+)(?<tail>[a-z]+)", "(${head}-${tail})"));
        s = "Hello World";  TEST_EQUAL(rf.format(s), "(H-ello) (W-orld)");
        s = "Hello World";  TEST_EQUAL(rf.format(s, 1), "(H-ello) World");

        TRY(rf = RegexFormat("([A-Z]+)([a-z]+)", "($-/$+)"));
        s = "Hello World";  TEST_EQUAL(rf.format(s), "(H/ello) (W/orld)");

        TRY(rf = RegexFormat("[A-Z]+[a-z]+", "($-/$+)"));
        s = "Hello World";  TEST_EQUAL(rf.format(s), "(/) (/)");

        TRY(rf = RegexFormat("(_*)([A-Z]+)(_*)([a-z]+)(_*)", "($-/$+)"));
        s = "Hello World";  TEST_EQUAL(rf.format(s), "(H/ello) (W/orld)");

        TRY(rf = RegexFormat("\\w", "$$"));
        s = "Hello";  TEST_EQUAL(rf(s), "$$$$$");

        TRY(rf = RegexFormat("([A-Z]+)([a-z]+)", "(\\1-\\2)"));
        s = "Hello World";  TEST_EQUAL(rf.format(s), "(H-ello) (W-orld)");
        s = "Hello World";  TEST_EQUAL(rf.format(s, 1), "(H-ello) World");

        TRY(rf = RegexFormat("\\w", "\\0"));
        s = "Hello";  TEST_EQUAL(rf(s), "\0\0\0\0\0"s);

        TRY(rf = RegexFormat("\\w+", "\\a\\b\\t\\n\\v\\f\\r\\e"));
        s = "Hello";  TEST_EQUAL(rf(s), "\a\b\t\n\v\f\r\x1b");

        TRY(rf = RegexFormat("\\w+", "(\\Q\\1$1\\E)"));
        s = "Hello";  TEST_EQUAL(rf(s), "(\\1$1)");

        TRY(rf = RegexFormat("\\w+", "\\Q\\1$1\\"));
        s = "Hello";  TEST_EQUAL(rf(s), "\\1$1\\");

        TRY(rf = RegexFormat("\\w", "\\xa7"));
        s = "Hello";  TEST_EQUAL(rf(s), u8"§§§§§");

        TRY(rf = RegexFormat("\\w", "\\x{20ac}"));
        s = "Hello";  TEST_EQUAL(rf(s), u8"€€€€€");

        TRY(rf = RegexFormat("\\S+", "\\l$0"));
        s = "AEIOU ÁÉÍÓÚ ΑΒΓΔΕ";  TEST_EQUAL(rf(s), u8"aEIOU áÉÍÓÚ αΒΓΔΕ");

        TRY(rf = RegexFormat("\\S+", "\\u$0"));
        s = "aeiou áéíóú αβγδε";  TEST_EQUAL(rf(s), u8"Aeiou Áéíóú Αβγδε");

        TRY(rf = RegexFormat("\\S+", "\\L$0"));
        s = "AEIOU ÁÉÍÓÚ ΑΒΓΔΕ";  TEST_EQUAL(rf(s), u8"aeiou áéíóú αβγδε");

        TRY(rf = RegexFormat("\\S+", "\\U$0"));
        s = "aeiou áéíóú αβγδε";  TEST_EQUAL(rf(s), u8"AEIOU ÁÉÍÓÚ ΑΒΓΔΕ");

        TRY(rf = RegexFormat("([aeiou])|([a-z])", "\\U$1\\L$2", rx_caseless));
        s = "Hello world";  TEST_EQUAL(rf(s), "hEllO wOrld");

        s = "Hello world";
        TRY(rf = regex_format("[a-z]+", "§"));
        TEST_EQUAL(rf(s), "H§ §");
        TRY(rf = regex_format("[a-z]+", "§", rx_caseless));
        TEST_EQUAL(rf(s), "§ §");

        TRY(rf = RegexFormat("\\w+", "\\U$0"));
        s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s), "HELLOWORLD");
        s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s, 1), "HELLO");
        s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s, 2), "HELLOWORLD");
        s = "*** @@@@@ @@@@@ ***";  TEST_EQUAL(rf.extract(s), "");

        TRY(rf = RegexFormat("\\w+", "($0/$</$>)"));
        s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s), "(Hello/*** / )(world/ / ***)");
        s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s, 1), "(Hello/*** / world ***)");

        TRY(rf = RegexFormat("\\w+", "($0:$*)"));
        s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s), "(Hello:*** Hello world ***)(world:*** Hello world ***)");
        s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s, 1), "(Hello:*** Hello world ***)");

        s = "Hello world";
        TRY(rf = "/[a-z]+/§/"_rf);
        TEST_EQUAL(rf(s), "H§ §");
        TRY(rf = "/[a-z]+/§/i"_rf);
        TEST_EQUAL(rf(s), "§ §");

    }

    void check_utility_functions() {

        TEST_EQUAL(regex_escape(""), "");
        TEST_EQUAL(regex_escape("Hello world"), "Hello world");
        TEST_EQUAL(regex_escape("[$-*]{42}"), "\\[\\$-\\*\\]\\{42\\}");

    }

    void check_utf16_regex() {

        #if defined(UNICORN_PCRE16)

            Regex16 r;
            Match16 m;
            u16string s = u"(Hello world)", t;

            TRY(r = Regex16(u"\\b[a-z]+\\b"));
            TRY(m = r(s));
            TEST(m);
            TEST_EQUAL(m.str(), u"world");

            Irange<MatchIterator16> mr;
            std::vector<u16string> v;

            TRY(r = Regex16(u"\\w+"));
            TRY(mr = r.grep(s));
            TEST_EQUAL(range_count(mr), 2);
            TRY(std::copy(CROW_BOUNDS(mr), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[Hello,world]");

            Irange<SplitIterator16> sr;

            TRY(r = Regex16(u"[ ()]+"));
            TRY(sr = r.split(s));
            TEST_EQUAL(range_count(sr), 4);
            TRY(std::copy(CROW_BOUNDS(sr), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[,Hello,world,]");

            RegexFormat16 rf;

            TRY(rf = RegexFormat16(u"\\w+", u"=$0="));
            TRY(t = rf(s));
            TEST_EQUAL(t, u"(=Hello= =world=)");

            TEST_EQUAL(regex_escape(u""), u"");
            TEST_EQUAL(regex_escape(u"Hello world"), u"Hello world");
            TEST_EQUAL(regex_escape(u"[$-*]{42}"), u"\\[\\$-\\*\\]\\{42\\}");

            s = u"Hello world";
            TRY(r = u"/[a-z]+/"_re);
            TRY(m = r(s));
            TEST_EQUAL(m.str(), u"ello");
            TRY(r = u"/[a-z]+/i"_re);
            TRY(m = r(s));
            TEST_EQUAL(m.str(), u"Hello");
            TRY(rf = u"/[a-z]+/§/"_rf);
            TEST_EQUAL(rf(s), u"H§ §");
            TRY(rf = u"/[a-z]+/§/i"_rf);
            TEST_EQUAL(rf(s), u"§ §");

        #endif

    }

    void check_utf32_regex() {

        #if defined(UNICORN_PCRE32)

            Regex32 r;
            Match32 m;
            u32string s = U"(Hello world)", t;

            TRY(r = Regex32(U"\\b[a-z]+\\b"));
            TRY(m = r(s));
            TEST(m);
            TEST_EQUAL(m.str(), U"world");

            Irange<MatchIterator32> mr;
            std::vector<u32string> v;

            TRY(r = Regex32(U"\\w+"));
            TRY(mr = r.grep(s));
            TEST_EQUAL(range_count(mr), 2);
            TRY(std::copy(CROW_BOUNDS(mr), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[Hello,world]");

            Irange<SplitIterator32> sr;

            TRY(r = Regex32(U"[ ()]+"));
            TRY(sr = r.split(s));
            TEST_EQUAL(range_count(sr), 4);
            TRY(std::copy(CROW_BOUNDS(sr), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[,Hello,world,]");

            RegexFormat32 rf;

            TRY(rf = RegexFormat32(U"\\w+", U"=$0="));
            TRY(t = rf(s));
            TEST_EQUAL(t, U"(=Hello= =world=)");

            TEST_EQUAL(regex_escape(U""), U"");
            TEST_EQUAL(regex_escape(U"Hello world"), U"Hello world");
            TEST_EQUAL(regex_escape(U"[$-*]{42}"), U"\\[\\$-\\*\\]\\{42\\}");

            s = U"Hello world";
            TRY(r = U"/[a-z]+/"_re);
            TRY(m = r(s));
            TEST_EQUAL(m.str(), U"ello");
            TRY(r = U"/[a-z]+/i"_re);
            TRY(m = r(s));
            TEST_EQUAL(m.str(), U"Hello");
            TRY(rf = U"/[a-z]+/§/"_rf);
            TEST_EQUAL(rf(s), U"H§ §");
            TRY(rf = U"/[a-z]+/§/i"_rf);
            TEST_EQUAL(rf(s), U"§ §");

        #endif

    }

    void check_wchar_regex() {

        #if defined(UNICORN_PCRE_WCHAR)

            WideRegex r;
            WideMatch m;
            wstring s = L"(Hello world)", t;

            TRY(r = WideRegex(L"\\b[a-z]+\\b"));
            TRY(m = r(s));
            TEST(m);
            TEST_EQUAL(m.str(), L"world");

            Irange<WideMatchIterator> mr;
            std::vector<wstring> v;

            TRY(r = WideRegex(L"\\w+"));
            TRY(mr = r.grep(s));
            TEST_EQUAL(range_count(mr), 2);
            TRY(std::copy(CROW_BOUNDS(mr), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[Hello,world]");

            Irange<WideSplitIterator> sr;

            TRY(r = WideRegex(L"[ ()]+"));
            TRY(sr = r.split(s));
            TEST_EQUAL(range_count(sr), 4);
            TRY(std::copy(CROW_BOUNDS(sr), overwrite(v)));
            TEST_EQUAL(Test::format_range(v), "[,Hello,world,]");

            WideRegexFormat rf;

            TRY(rf = WideRegexFormat(L"\\w+", L"=$0="));
            TRY(t = rf(s));
            TEST_EQUAL(t, L"(=Hello= =world=)");

            TEST_EQUAL(regex_escape(L""), L"");
            TEST_EQUAL(regex_escape(L"Hello world"), L"Hello world");
            TEST_EQUAL(regex_escape(L"[$-*]{42}"), L"\\[\\$-\\*\\]\\{42\\}");

            s = L"Hello world";
            TRY(r = L"/[a-z]+/"_re);
            TRY(m = r(s));
            TEST_EQUAL(m.str(), L"ello");
            TRY(r = L"/[a-z]+/i"_re);
            TRY(m = r(s));
            TEST_EQUAL(m.str(), L"Hello");
            TRY(rf = L"/[a-z]+/§/"_rf);
            TEST_EQUAL(rf(s), L"H§ §");
            TRY(rf = L"/[a-z]+/§/i"_rf);
            TEST_EQUAL(rf(s), L"§ §");

        #endif

    }

    void check_byte_regex() {

        ByteRegex r;
        ByteMatch m;
        string s1, s2, s3;

        s1 = "(Hello world)";
        TRY(r = ByteRegex("\\b[a-z]+\\b"));
        TRY(m = r(s1));
        TEST(m);
        TEST_EQUAL(m.str(), "world");
        TRY(r = ByteRegex("[a-z]+", rx_caseless));

        TRY(m = r.search(s1, 0));   TEST(m.matched());    TEST_EQUAL(m.offset(), 1);     TEST_EQUAL(m.str(), "Hello");
        TRY(m = r.search(s1, 1));   TEST(m.matched());    TEST_EQUAL(m.offset(), 1);     TEST_EQUAL(m.str(), "Hello");
        TRY(m = r.search(s1, 2));   TEST(m.matched());    TEST_EQUAL(m.offset(), 2);     TEST_EQUAL(m.str(), "ello");
        TRY(m = r.search(s1, 3));   TEST(m.matched());    TEST_EQUAL(m.offset(), 3);     TEST_EQUAL(m.str(), "llo");
        TRY(m = r.search(s1, 4));   TEST(m.matched());    TEST_EQUAL(m.offset(), 4);     TEST_EQUAL(m.str(), "lo");
        TRY(m = r.search(s1, 5));   TEST(m.matched());    TEST_EQUAL(m.offset(), 5);     TEST_EQUAL(m.str(), "o");
        TRY(m = r.search(s1, 6));   TEST(m.matched());    TEST_EQUAL(m.offset(), 7);     TEST_EQUAL(m.str(), "world");
        TRY(m = r.search(s1, 7));   TEST(m.matched());    TEST_EQUAL(m.offset(), 7);     TEST_EQUAL(m.str(), "world");
        TRY(m = r.search(s1, 8));   TEST(m.matched());    TEST_EQUAL(m.offset(), 8);     TEST_EQUAL(m.str(), "orld");
        TRY(m = r.search(s1, 9));   TEST(m.matched());    TEST_EQUAL(m.offset(), 9);     TEST_EQUAL(m.str(), "rld");
        TRY(m = r.search(s1, 10));  TEST(m.matched());    TEST_EQUAL(m.offset(), 10);    TEST_EQUAL(m.str(), "ld");
        TRY(m = r.search(s1, 11));  TEST(m.matched());    TEST_EQUAL(m.offset(), 11);    TEST_EQUAL(m.str(), "d");
        TRY(m = r.search(s1, 12));  TEST(! m.matched());  TEST_EQUAL(m.offset(), npos);  TEST_EQUAL(m.str(), "");
        TRY(m = r.search(s1, 13));  TEST(! m.matched());  TEST_EQUAL(m.offset(), npos);  TEST_EQUAL(m.str(), "");
        TRY(m = r.search(s1, 14));  TEST(! m.matched());  TEST_EQUAL(m.offset(), npos);  TEST_EQUAL(m.str(), "");
        TRY(m = r.search(s1, 15));  TEST(! m.matched());  TEST_EQUAL(m.offset(), npos);  TEST_EQUAL(m.str(), "");

        Irange<ByteMatchIterator> mr;
        std::vector<string> v;

        TRY(r = ByteRegex("\\w+"));
        TRY(mr = r.grep(s1));
        TEST_EQUAL(range_count(mr), 2);
        TRY(std::copy(CROW_BOUNDS(mr), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[Hello,world]");

        Irange<ByteSplitIterator> sr;

        TRY(r = ByteRegex("[ ()]+"));
        TRY(sr = r.split(s1));
        TEST_EQUAL(range_count(sr), 4);
        TRY(std::copy(CROW_BOUNDS(sr), overwrite(v)));
        TEST_EQUAL(Test::format_range(v), "[,Hello,world,]");

        ByteRegexFormat rf;

        TRY(rf = ByteRegexFormat("\\w+", "=$0="));
        TRY(s2 = rf(s1));
        TEST_EQUAL(s2, "(=Hello= =world=)");
        TRY(rf = ByteRegexFormat("\\w+", "\\xff"));
        TRY(s2 = rf(s1));
        TEST_EQUAL(s2, "(\xff \xff)");
        TRY(rf = ByteRegexFormat("\\w+", "\\U$0\\E"));
        TRY(s2 = rf(s1));
        TEST_EQUAL(s2, "(HELLO WORLD)");
        TRY(rf = ByteRegexFormat("\\w+", "\\L$0\\E"));
        s1 = "AEIOU \xc1\xc9\xcd\xd3\xda";
        TRY(s2 = rf(s1));
        TEST_EQUAL(s2, "aeiou \xc1\xc9\xcd\xd3\xda");

        Regex x;
        RegexFormat xf;
        size_t n1, n2;

        TRY(r = ByteRegex("[\\x80-\\xff]"));
        TRY(x = Regex("[\\x80-\\xff]"));
        s1 = u8"Hello §¶ ΣΠ";
        TRY(n1 = r.count(s1));
        TRY(n2 = x.count(s1));
        TEST_EQUAL(n1, 8);
        TEST_EQUAL(n2, 2);
        TRY(rf = ByteRegexFormat(r, "*"));
        TRY(xf = RegexFormat(x, "*"));
        TRY(s2 = rf(s1));
        TRY(s3 = xf(s1));
        TEST_EQUAL(s2, "Hello **** ****");
        TEST_EQUAL(s3, "Hello ** ΣΠ");

        s1 = "Hello world";
        TRY(r = "/[a-z]+/"_bre);
        TRY(m = r(s1));
        TEST_EQUAL(m.str(), "ello");
        TRY(r = "/[a-z]+/i"_bre);
        TRY(m = r(s1));
        TEST_EQUAL(m.str(), "Hello");
        TRY(rf = "/[a-z]+/*/"_brf);
        TEST_EQUAL(rf(s1), "H* *");
        TRY(rf = "/[a-z]+/*/i"_brf);
        TEST_EQUAL(rf(s1), "* *");

    }

}

TEST_MODULE(unicorn, regex) {

    check_version_information();
    check_utf8_regex();
    check_match_ranges();
    check_split_ranges();
    check_regex_formatting();
    check_utility_functions();
    check_utf16_regex();
    check_utf32_regex();
    check_wchar_regex();
    check_byte_regex();

}
