#include "unicorn/core.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_regex_version_information() {

    Version pv, uv;

    TRY(pv = Regex::pcre_version());
    TRY(uv = Regex::unicode_version());
    TEST_COMPARE(pv, >=, (Version{8,0,0}));
    TEST_COMPARE(uv, >=, (Version{7,0,0}));

    std::cout << "... PCRE library version: " << pv.str(3) << "\n";
    std::cout << "... PCRE Unicode version: " << uv.str(3) << "\n";

}

void test_unicorn_regex_utf8() {

    U8string s;
    Regex r;
    Match m;
    Utf8Iterator u;

    TEST(r.empty());
    TEST_EQUAL(r.groups(), 1);
    TEST_EQUAL(r.pattern(), "");
    TEST_EQUAL(r.flags(), 0);

    TEST(! m.matched());
    TEST_EQUAL(U8string(m.s_begin(), m.s_end()), "");
    TEST_EQUAL(u_str(m.u_begin(), m.u_end()), "");

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
    TEST_EQUAL(U8string(m.s_begin(), m.s_end()), "ello");
    TEST_EQUAL(U8string(m.s_begin(0), m.s_end(0)), "ello");
    TEST_EQUAL(U8string(m.s_begin(1), m.s_end(1)), "");
    TEST_EQUAL(u_str(m.u_begin(), m.u_end()), "ello");
    TEST_EQUAL(u_str(m.u_begin(0), m.u_end(0)), "ello");
    TEST_EQUAL(u_str(m.u_begin(1), m.u_end(1)), "");
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
    TEST_EQUAL(U8string(m.s_begin(), m.s_end()), "");
    TEST_EQUAL(u_str(m.u_begin(), m.u_end()), "");

    TRY(r = Regex("[a-z]+", Regex::dfa));
    TEST_EQUAL(r.groups(), 1);
    TEST_EQUAL(r.pattern(), "[a-z]+");
    TEST_EQUAL(r.flags(), Regex::dfa);
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

    TRY(r = Regex("[a-z]*", Regex::notempty));
    TEST_EQUAL(r.groups(), 1);
    TEST_EQUAL(r.pattern(), "[a-z]*");
    TEST_EQUAL(r.flags(), Regex::notempty);
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

    TRY(r = Regex("[a-z]+", Regex::caseless));
    TEST_EQUAL(r.groups(), 1);
    TEST_EQUAL(r.pattern(), "[a-z]+");
    TEST_EQUAL(r.flags(), Regex::caseless);
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

    TRY(r = Regex("[a-z]+", Regex::caseless));
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

    TRY(r = Regex("([a-z]+) ([a-z]+)", Regex::caseless));
    TEST_EQUAL(r.groups(), 3);
    TEST_EQUAL(r.pattern(), "([a-z]+) ([a-z]+)");
    TEST_EQUAL(r.flags(), Regex::caseless);
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
    TEST_EQUAL(U8string(m.s_begin(), m.s_end()), "Hello world");
    TEST_EQUAL(U8string(m.s_begin(0), m.s_end(0)), "Hello world");
    TEST_EQUAL(u_str(m.u_begin(), m.u_end()), "Hello world");
    TEST_EQUAL(u_str(m.u_begin(0), m.u_end(0)), "Hello world");
    TEST_EQUAL(m.offset(1), 0);
    TEST_EQUAL(m.endpos(1), 5);
    TEST_EQUAL(m.count(1), 5);
    TEST_EQUAL(m.str(1), "Hello");
    TEST_EQUAL(m[1], "Hello");
    TEST_EQUAL(U8string(m.s_begin(1), m.s_end(1)), "Hello");
    TEST_EQUAL(u_str(m.u_begin(1), m.u_end(1)), "Hello");
    TEST_EQUAL(m.offset(2), 6);
    TEST_EQUAL(m.endpos(2), 11);
    TEST_EQUAL(m.count(2), 5);
    TEST_EQUAL(m.str(2), "world");
    TEST_EQUAL(m[2], "world");
    TEST_EQUAL(U8string(m.s_begin(2), m.s_end(2)), "world");
    TEST_EQUAL(u_str(m.u_begin(2), m.u_end(2)), "world");
    TEST(m.matched(0));
    TEST(m.matched(1));
    TEST(m.matched(2));
    TEST(! m.matched(3));

    TRY(r = Regex("(?<head>[a-z]+) (?<tail>[a-z]+)", Regex::caseless));
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

    TRY(r = Regex("[a-z]+", Regex::caseless));
    s = "Hello";          TRY(m = r.anchor(s));  TEST(m.matched());    TEST_EQUAL(m.str(), "Hello");
    s = "Hello world";    TRY(m = r.anchor(s));  TEST(m.matched());    TEST_EQUAL(m.str(), "Hello");
    s = "(Hello world)";  TRY(m = r.anchor(s));  TEST(! m.matched());  TEST_EQUAL(m.str(), "");
    s = "Hello";          TRY(m = r.match(s));   TEST(m.matched());    TEST_EQUAL(m.str(), "Hello");
    s = "Hello world";    TRY(m = r.match(s));   TEST(! m.matched());  TEST_EQUAL(m.str(), "");

    TRY(r = Regex("[a-z]+", Regex::caseless));
    s = "Hello world";  TEST_EQUAL(r.count(s), 2);
    s = "42";           TEST_EQUAL(r.count(s), 0);

    TRY(r = Regex("([a-z]+) ([a-z]+)", Regex::caseless));
    s = "Hello";  TRY(m = r.match(s));  TEST(! m);  TEST(! m.partial());  TEST(! m.full_or_partial());

    TRY(r = Regex("([a-z]+) ([a-z]+)", Regex::caseless | Regex::partialsoft));
    s = "Hello";        TRY(m = r.match(s));  TEST(! m);  TEST(m.partial());    TEST(m.full_or_partial());
    s = "Hello world";  TRY(m = r.match(s));  TEST(m);    TEST(! m.partial());  TEST(m.full_or_partial());
    s = "42";           TRY(m = r.match(s));  TEST(! m);  TEST(! m.partial());  TEST(! m.full_or_partial());

    TRY(r = Regex("([a-z]+) ([a-z]+)", Regex::caseless | Regex::partialhard));
    s = "Hello";        TRY(m = r.match(s));  TEST(! m);  TEST(m.partial());    TEST(m.full_or_partial());
    s = "Hello world";  TRY(m = r.match(s));  TEST(! m);  TEST(m.partial());    TEST(m.full_or_partial());
    s = "42";           TRY(m = r.match(s));  TEST(! m);  TEST(! m.partial());  TEST(! m.full_or_partial());

    TEST_THROW(r = Regex("]a-z["), RegexError);
    TEST_THROW(r = Regex("x\\yz"), RegexError);

    TRY(r = Regex("(\\w+) (\\w+)", Regex::optimize));
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
    TRY(r = Regex("[a-z]+"));
    TRY(m = r(s));
    TEST_EQUAL(m.str(), "ello");
    TRY(r = Regex("[a-z]+", Regex::caseless));
    TRY(m = r(s));
    TEST_EQUAL(m.str(), "Hello");

    // Abigail's famous prime number regex
    // http://neilk.net/blog/2000/06/01/abigails-regex-to-test-for-prime-numbers/

    auto is_prime = [] (int n) -> bool {
        if (n <= 3)
            return n >= 2;
        int limit = int(floor(sqrt(double(n))));
        for (int i = 2; i <= limit; ++i)
            if (n % i == 0)
                return false;
        return true;
    };

    TRY(r = Regex("^.?$|^(..+?)\\1+$"));

    for (int n = 0; n <= 100; ++n) {
        TEST_EQUAL(! r.match(U8string(n, 'x')), is_prime(n));
        TEST_EQUAL(! r.match(str_repeat(u8"€", n)), is_prime(n));
    }

}

void test_unicorn_regex_match_ranges() {

    U8string s;
    Regex r;
    Irange<MatchIterator> mr;
    Strings v;

    TRY(r = Regex("[a-z]+", Regex::caseless));

    s = "";               TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 0);
    s = "42";             TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 0);
    s = "Hello";          TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 1);  TRY(std::copy(mr.begin(), mr.end(), overwrite(v)));  TEST_EQUAL(RS::UnitTest::format_range(v), "[Hello]");
    s = "(Hello)";        TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 1);  TRY(std::copy(mr.begin(), mr.end(), overwrite(v)));  TEST_EQUAL(RS::UnitTest::format_range(v), "[Hello]");
    s = "Hello world";    TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 2);  TRY(std::copy(mr.begin(), mr.end(), overwrite(v)));  TEST_EQUAL(RS::UnitTest::format_range(v), "[Hello,world]");
    s = "(Hello world)";  TRY(mr = r.grep(s));  TEST_EQUAL(range_count(mr), 2);  TRY(std::copy(mr.begin(), mr.end(), overwrite(v)));  TEST_EQUAL(RS::UnitTest::format_range(v), "[Hello,world]");

}

void test_unicorn_regex_split_ranges() {

    U8string s;
    Regex r;
    Irange<SplitIterator> sr;

    TRY(r = Regex("/"));

    s = "";                  TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 1);  TEST_EQUAL(RS::UnitTest::format_range(sr), "[]");
    s = "/";                 TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 2);  TEST_EQUAL(RS::UnitTest::format_range(sr), "[,]");
    s = "Hello";             TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 1);  TEST_EQUAL(RS::UnitTest::format_range(sr), "[Hello]");
    s = "Hello/world";       TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 2);  TEST_EQUAL(RS::UnitTest::format_range(sr), "[Hello,world]");
    s = "/Hello/world/";     TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 4);  TEST_EQUAL(RS::UnitTest::format_range(sr), "[,Hello,world,]");
    s = "//Hello//world//";  TRY(sr = r.split(s));  TEST_EQUAL(range_count(sr), 7);  TEST_EQUAL(RS::UnitTest::format_range(sr), "[,,Hello,,world,,]");

}

void test_unicorn_regex_formatting() {

    U8string s;
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

    TRY(rf = RegexFormat("\\w+", "\\0\\a\\b\\t\\n\\v\\f\\r\\e"));
    s = "Hello world";  TEST_EQUAL(rf(s), "\0\a\b\t\n\v\f\r\x1b \0\a\b\t\n\v\f\r\x1b"s);

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

    TRY(rf = RegexFormat("([aeiou])|([a-z])", "\\U$1\\L$2", Regex::caseless));
    s = "Hello world";  TEST_EQUAL(rf(s), "hEllO wOrld");

    s = "Hello world";
    TRY(rf = RegexFormat("[a-z]+", "§"));
    TEST_EQUAL(rf(s), "H§ §");
    TRY(rf = RegexFormat("[a-z]+", "§", Regex::caseless));
    TEST_EQUAL(rf(s), "§ §");

    TRY(rf = RegexFormat("\\w+", "\\U$0"));
    s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s), "HELLOWORLD");
    s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s, 1), "HELLO");
    s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s, 2), "HELLOWORLD");
    s = "*** @@@@@ @@@@@ ***";  TEST_EQUAL(rf.extract(s), "");

    TRY(rf = RegexFormat("\\w+", "($0/$</$>)"));
    s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s), "(Hello/*** / )(world/ / ***)");
    s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s, 1), "(Hello/*** / world ***)");

    TRY(rf = RegexFormat("\\w+", "($0/$[/$])"));
    s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s), "(Hello/*** / world ***)(world/*** Hello / ***)");
    s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s, 1), "(Hello/*** / world ***)");

    TRY(rf = RegexFormat("\\w+", "($0:$_)"));
    s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s), "(Hello:*** Hello world ***)(world:*** Hello world ***)");
    s = "*** Hello world ***";  TEST_EQUAL(rf.extract(s, 1), "(Hello:*** Hello world ***)");

    TEST_THROW(RegexFormat("\\w", "\\x{d800}"), EncodingError);
    TEST_THROW(RegexFormat("\\w", "\\x{dfff}"), EncodingError);
    TEST_THROW(RegexFormat("\\w", "\\x{110000}"), EncodingError);

}

void test_unicorn_regex_transform() {

    Regex r;
    U8string s1, s2;
    auto sf = [] (const U8string& s) { return s + s; };
    auto mf = [] (const Match& m) { return m.first() + m.last(); };

    s1 = "Hello world";
    TRY(r = Regex("\\w+"));
    TRY(s2 = r.transform(s1, sf));
    TEST_EQUAL(s2, "HelloHello worldworld");
    TRY(s2 = r.transform(s1, sf, 1));
    TEST_EQUAL(s2, "HelloHello world");
    TRY(r.transform_in(s1, sf));
    TEST_EQUAL(s1, "HelloHello worldworld");

    s1 = "Hello world";
    TRY(r = Regex("(\\w)(\\w+)(\\w)"));
    TRY(s2 = r.transform(s1, mf));
    TEST_EQUAL(s2, "Ho wd");
    TRY(s2 = r.transform(s1, mf, 1));
    TEST_EQUAL(s2, "Ho world");
    TRY(r.transform_in(s1, mf));
    TEST_EQUAL(s1, "Ho wd");

}

void test_unicorn_regex_string_escaping() {

    TEST_EQUAL(Regex::escape(""), "");
    TEST_EQUAL(Regex::escape("Hello world"), "Hello world");
    TEST_EQUAL(Regex::escape("[$-*]{42}"), "\\[\\$-\\*\\]\\{42\\}");

}

void test_unicorn_regex_bytes() {

    Regex r;
    Match m;
    std::string s1, s2, s3;

    s1 = "(Hello world)";
    TRY(r = Regex("\\b[a-z]+\\b", Regex::byte));
    TRY(m = r(s1));
    TEST(m);
    TEST_EQUAL(m.str(), "world");
    TRY(r = Regex("[a-z]+", Regex::byte | Regex::caseless));

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

    Irange<MatchIterator> mr;
    Strings v;

    TRY(r = Regex("\\w+", Regex::byte));
    TRY(mr = r.grep(s1));
    TEST_EQUAL(range_count(mr), 2);
    TRY(std::copy(mr.begin(), mr.end(), overwrite(v)));
    TEST_EQUAL(RS::UnitTest::format_range(v), "[Hello,world]");

    Irange<SplitIterator> sr;

    TRY(r = Regex("[ ()]+", Regex::byte));
    TRY(sr = r.split(s1));
    TEST_EQUAL(range_count(sr), 4);
    TRY(std::copy(sr.begin(), sr.end(), overwrite(v)));
    TEST_EQUAL(RS::UnitTest::format_range(v), "[,Hello,world,]");

    RegexFormat rf;

    TRY(rf = RegexFormat("\\w+", "=$0=", Regex::byte));
    TRY(s2 = rf(s1));
    TEST_EQUAL(s2, "(=Hello= =world=)");
    TRY(rf = RegexFormat("\\w+", "\\xff", Regex::byte));
    TRY(s2 = rf(s1));
    TEST_EQUAL(s2, "(\xff \xff)");
    TRY(rf = RegexFormat("\\w+", "\\U$0\\E", Regex::byte));
    TRY(s2 = rf(s1));
    TEST_EQUAL(s2, "(HELLO WORLD)");
    TRY(rf = RegexFormat("\\w+", "\\L$0\\E", Regex::byte));
    s1 = "AEIOU \xc1\xc9\xcd\xd3\xda";
    TRY(s2 = rf(s1));
    TEST_EQUAL(s2, "aeiou \xc1\xc9\xcd\xd3\xda");
    TEST_THROW(RegexFormat("\\w", "\\x{100}", Regex::byte), EncodingError);

    Regex x;
    RegexFormat xf;
    size_t n1 = 0, n2 = 0;

    TRY(r = Regex("[\\x80-\\xff]", Regex::byte));
    TRY(x = Regex("[\\x80-\\xff]"));
    s1 = u8"Hello §¶ ΣΠ";
    TRY(n1 = r.count(s1));
    TRY(n2 = x.count(s1));
    TEST_EQUAL(n1, 8);
    TEST_EQUAL(n2, 2);
    TRY(rf = RegexFormat(r, "*"));
    TRY(xf = RegexFormat(x, "*"));
    TRY(s2 = rf(s1));
    TRY(s3 = xf(s1));
    TEST_EQUAL(s2, "Hello **** ****");
    TEST_EQUAL(s3, "Hello ** ΣΠ");

}

void test_unicorn_regex_literals() {

    using namespace RS::Unicorn::Literals;

    Regex r;

    TRY(r = "[a-z]+"_re);    TEST(r.match("hello"));  TEST(! r.match("HELLO"));  TEST(! r.match("12345"));
    TRY(r = "[a-z]+"_re_i);  TEST(r.match("hello"));  TEST(r.match("HELLO"));    TEST(! r.match("12345"));
    TRY(r = "[a-z]+"_re_b);  TEST(r.match("hello"));  TEST(! r.match("HELLO"));  TEST(! r.match("12345"));

}
