#include "unicorn/regex.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <iostream>
#include <string>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_regex_version() {

    Version ver;

    TRY(ver = Regex::compile_version());  TEST_COMPARE(ver, >=, Version(10,23));  std::cout << "... PCRE compile version: " << ver << "\n";
    TRY(ver = Regex::runtime_version());  TEST_COMPARE(ver, >=, Version(10,23));  std::cout << "... PCRE runtime version: " << ver << "\n";
    TRY(ver = Regex::unicode_version());  TEST_COMPARE(ver, >=, Version(8,0));    std::cout << "... PCRE Unicode version: " << ver << "\n";

}

void test_unicorn_regex_match() {

    Regex r;
    Regex::match m;
    std::string s;

    TEST(r.is_null());
    TEST_EQUAL(to_str(r), "");
    TEST_EQUAL(r.pattern(), "");
    TEST_EQUAL(r.flags(), 0);
    TEST_EQUAL(r.groups(), 0);
    s = "ABC123";
    TRY(m = r(s));
    TEST(! m);
    TEST(! m.full());
    TEST(! m.partial());
    TEST(! m.matched());

    TRY(r = Regex("[a-z]+"));
    TEST(! r.is_null());
    TEST_EQUAL(to_str(r), "[a-z]+");
    TEST_EQUAL(r.pattern(), "[a-z]+");
    TEST_EQUAL(r.flags(), 0);
    TEST_EQUAL(r.groups(), 1);
    s = "ABC123";
    TRY(m = r(s));
    TEST(! m);
    TEST(! m.full());
    TEST(! m.partial());
    TEST(! m.matched());
    s = "ABC123 xyz789";
    TRY(m = r(s));
    TEST(m);
    TEST(m.full());
    TEST(! m.partial());
    TEST(m.matched());
    TEST_EQUAL(m.offset(), 7);
    TEST_EQUAL(m.endpos(), 10);
    TEST_EQUAL(m.count(), 3);
    TEST_EQUAL(m.str(), "xyz");
    TEST(! m.matched(1));

    TRY(r = Regex("([a-z]+)(\\d+)"));
    TEST(! r.is_null());
    TEST_EQUAL(to_str(r), "([a-z]+)(\\d+)");
    TEST_EQUAL(r.pattern(), "([a-z]+)(\\d+)");
    TEST_EQUAL(r.flags(), 0);
    TEST_EQUAL(r.groups(), 3);
    s = "ABC123 xyz789";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.offset(), 7);
    TEST_EQUAL(m.endpos(), 13);
    TEST_EQUAL(m.count(), 6);
    TEST_EQUAL(m.str(), "xyz789");
    TEST(m.matched(1));
    TEST_EQUAL(m.str(1), "xyz");
    TEST(m.matched(2));
    TEST_EQUAL(m.str(2), "789");
    TEST(! m.matched(3));
    TEST_EQUAL(m.str(3), "");

    TRY(r = Regex("([a-z]+)(\\d+)", Regex::icase));
    TEST(! r.is_null());
    TEST_EQUAL(to_str(r), "([a-z]+)(\\d+)");
    TEST_EQUAL(r.pattern(), "([a-z]+)(\\d+)");
    TEST_EQUAL(r.flags(), Regex::icase);
    TEST_EQUAL(r.groups(), 3);
    s = "ABC123 xyz789";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.offset(), 0);
    TEST_EQUAL(m.endpos(), 6);
    TEST_EQUAL(m.count(), 6);
    TEST_EQUAL(m.str(), "ABC123");
    TEST(m.matched(1));
    TEST_EQUAL(m.str(1), "ABC");
    TEST(m.matched(2));
    TEST_EQUAL(m.str(2), "123");
    TEST(! m.matched(3));
    TEST_EQUAL(m.str(3), "");

    TRY(r = Regex("[a-z]+"));
    s = "abc123 xyz789";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "abc");
    TRY(m = r(s, 3));
    TEST(m);
    TEST_EQUAL(m.str(), "xyz");
    TRY(m = r(s, 12));
    TEST(! m);
    TRY(m = r(s, 13));
    TEST(! m);
    TRY(m = r(s, 14));
    TEST(! m);

    TRY(r = Regex("[[:alpha:]]+"));
    s = u8"αβγδε";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), u8"αβγδε");
    TEST_THROW(m = r(s, 1), Regex::error);
    TRY(m = r(s, 2));
    TEST(m);
    TEST_EQUAL(m.str(), u8"βγδε");
    TEST_THROW(m = r(s, 3), Regex::error);
    TRY(m = r(s, 4));
    TEST(m);
    TEST_EQUAL(m.str(), u8"γδε");
    TEST_THROW(m = r(s, 5), Regex::error);
    TRY(m = r(s, 6));
    TEST(m);
    TEST_EQUAL(m.str(), u8"δε");
    TEST_THROW(m = r(s, 7), Regex::error);
    TRY(m = r(s, 8));
    TEST(m);
    TEST_EQUAL(m.str(), u8"ε");
    TEST_THROW(m = r(s, 9), Regex::error);
    TRY(m = r(s, 10));
    TEST(! m);
    TRY(m = r(s, 11));
    TEST(! m);

    TRY(r = Regex("[a-z]+"));
    s = "abc123 xyz789";
    TRY(m = r(utf_begin(s)));
    TEST(m);
    TEST_EQUAL(m.str(), "abc");
    TRY(m = r(utf_iterator(s, 3)));
    TEST(m);
    TEST_EQUAL(m.str(), "xyz");

    TRY(r = Regex("([0-9]+)|([A-Z]+)|([a-z]+)"));
    s = "...123...456...";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "123");
    TEST_EQUAL(m.str(1), "123");
    TEST_EQUAL(m.str(2), "");
    TEST_EQUAL(m.str(3), "");
    TEST_EQUAL(m.first(), 1);
    TEST_EQUAL(m.last(), 1);
    s = "...abc...def...";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "abc");
    TEST_EQUAL(m.str(1), "");
    TEST_EQUAL(m.str(2), "");
    TEST_EQUAL(m.str(3), "abc");
    TEST_EQUAL(m.first(), 3);
    TEST_EQUAL(m.last(), 3);

    TRY(r = Regex("([0-9]*)([A-Z]*)([a-z]*)", Regex::not_empty));
    s = "...123ABC...";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "123ABC");
    TEST_EQUAL(m.str(1), "123");
    TEST_EQUAL(m.str(2), "ABC");
    TEST_EQUAL(m.str(3), "");
    TEST_EQUAL(m.first(), 1);
    TEST_EQUAL(m.last(), 2);
    s = "!@#$%^&*()";
    TRY(m = r(s));
    TEST(! m);
    TEST_EQUAL(m.first(), npos);
    TEST_EQUAL(m.last(), npos);

    TRY(r = Regex("^(a|b)c"));
    s = "bc";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "bc");
    TEST_EQUAL(m.mark(), "");
    TRY(r = Regex("^(*MARK:A)((*MARK:B)a|b)c"));
    s = "bc";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "bc");
    TEST_EQUAL(m.mark(), "A");
    s = "bx";
    TRY(m = r(s));
    TEST(! m);
    TEST_EQUAL(m.str(), "");
    TEST_EQUAL(m.mark(), "B");

    TEST_THROW(r = Regex("[z-a]"), Regex::error);

}

void test_unicorn_regex_compile_flags() {

    Regex r;
    Regex::match m;
    std::string s;

    // anchor = Anchor match at start of subject range
    TRY(r = Regex("[a-z]+", Regex::anchor));
    s = "Hello world";
    TRY(m = r(s));
    TEST(! m);
    s = "hello world";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "hello");

    // ascii = ASCII character properties
    TRY(r = Regex("[[:alpha:]]+"));
    s = u8"(αβγδε)";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), u8"αβγδε");
    TRY(r = Regex("[[:alpha:]]+", Regex::ascii));
    TRY(m = r(s));
    TEST(! m);

    // byte = Match byte strings instead of UTF-8
    TRY(r = Regex("\\(.....\\)"));
    s = u8"(αβγδε)";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), u8"(αβγδε)");
    TRY(r = Regex("\\(.....\\)", Regex::byte));
    TRY(m = r(s));
    TEST(! m);

    // crlf = Lines end with CRLF
    TRY(r = Regex("^[a-z]+$", Regex::multiline));
    s = "hello world\r\ngoodbye\r\n";
    TRY(m = r(s));
    TEST(! m);
    TRY(r = Regex("^[a-z]+$", Regex::crlf | Regex::multiline));
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "goodbye");

    // dollar_end = $ does not match line breaks at end of string
    TRY(r = Regex("^\\w+$"));
    s = "Hello\n";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "Hello");
    s = "Hello\n\n";
    TRY(m = r(s));
    TEST(! m);
    TRY(r = Regex("^\\w+$", Regex::dollar_end));
    s = "Hello\n";
    TRY(m = r(s));
    TEST(! m);

    // dot_all = . matches all characters including line breaks
    TRY(r = Regex(".+"));
    s = "Hello world\nGoodbye\n";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "Hello world");
    TRY(r = Regex(".+", Regex::dot_all));
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "Hello world\nGoodbye\n");

    // extended = Ignore whitespace and # comments
    TRY(r = Regex(R"(
        [[:alpha:]]+  # first word
        \s            # space
        [[:alpha:]]+  # second word
        )", Regex::extended));
    s = "Hello world\nGoodbye\n";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "Hello world");

    // first_line = Match must start in first line of string
    TRY(r = Regex("[a-z]{5}", Regex::first_line));
    s = "Hello world\nGoodbye\n";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "world");
    TRY(r = Regex("[a-z]{6}", Regex::first_line));
    TRY(m = r(s));
    TEST(! m);

    // full = Anchor match at both ends, matching complete string
    TRY(r = Regex("[a-z ]+", Regex::full));
    s = "Hello world";
    TRY(m = r(s));
    TEST(! m);
    s = "hello world";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "hello world");

    // icase = Case insensitive match
    TRY(r = Regex("[a-z]+"));
    s = "Hello world";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "ello");
    TRY(r = Regex("[a-z]+", Regex::icase));
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "Hello");

    // line = Match whole line
    // word = Match whole word
    TRY(r = Regex("[a-z]+"));
    s = "hello world\ngoodbye\n";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "hello");
    TRY(r = Regex("[a-z]+", Regex::line | Regex::multiline));
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "goodbye");
    TRY(r = Regex("[a-z]+", Regex::line | Regex::multiline | Regex::word));
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "goodbye");
    TRY(r = Regex("[a-z]+"));
    s = "Hello world";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "ello");
    TRY(r = Regex("[a-z]+", Regex::word));
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "world");

    // multiline = ^ and $ match beginning and end of each line
    TRY(r = Regex("[a-z]+$", Regex::multiline));
    s = "Hello world\nGoodbye\n";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "world");

    // no_capture = No automatic capture, named captures only
    TRY(r = Regex("([A-Z])([a-z]+)", Regex::no_capture));
    s = "Hello world";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.str(), "Hello");
    TEST(m.matched(0));
    TEST(! m.matched(1));
    TEST(! m.matched(2));

    // partial_hard = Hard partial match, prefer partial to full match
    TRY(r = Regex("\\b[a-z]+", Regex::partial_hard));
    s = "Hello world";
    TRY(m = r(s));
    TEST(m);
    TEST(! m.full());
    TEST(m.partial());
    TEST_EQUAL(m.str(), "world");
    TRY(r = Regex("\\b[a-z]+\\d+", Regex::partial_hard));
    TRY(m = r(s));
    TEST(m);
    TEST(! m.full());
    TEST(m.partial());
    TEST_EQUAL(m.str(), "world");

    // partial_soft = Soft partial match, prefer full to partial match
    TRY(r = Regex("\\b[a-z]+", Regex::partial_soft));
    s = "Hello world";
    TRY(m = r(s));
    TEST(m);
    TEST(m.full());
    TEST(! m.partial());
    TEST_EQUAL(m.str(), "world");
    TRY(r = Regex("\\b[a-z]+\\d+", Regex::partial_soft));
    TRY(m = r(s));
    TEST(m);
    TEST(! m.full());
    TEST(m.partial());
    TEST_EQUAL(m.str(), "world");

}

void test_unicorn_regex_runtime_flags() {

    Regex r;
    Regex::match m;
    std::string s;

    // anchor = Anchor match at start of subject range
    TRY(r = Regex("[a-z]+"));
    s = "Hello world";
    TRY(m = r(s, 0, Regex::anchor));
    TEST(! m);
    s = "hello world";
    TRY(m = r(s, 0, Regex::anchor));
    TEST(m);
    TEST_EQUAL(m.str(), "hello");

    // full = Anchor match at both ends, matching complete string
    TRY(r = Regex("[a-z ]+"));
    s = "Hello world";
    TRY(m = r(s, 0, Regex::full));
    TEST(! m);
    s = "hello world";
    TRY(m = r(s, 0, Regex::full));
    TEST(m);
    TEST_EQUAL(m.str(), "hello world");

    // not_empty = Do not match empty string
    // not_empty_start = Do not match empty string at start of subject string
    TRY(r = Regex("\\b[A-Z]*(?=[a-z])"));
    s = "hello world";
    TRY(m = r(s));
    TEST(m);
    TEST_EQUAL(m.offset(), 0);
    TEST_EQUAL(m.count(), 0);
    TEST_EQUAL(m.str(), "");
    TRY(m = r(s, 0, Regex::not_empty));
    TEST(! m);
    TRY(m = r(s, 0, Regex::not_empty_start));
    TEST(m);
    TEST_EQUAL(m.offset(), 6);
    TEST_EQUAL(m.count(), 0);
    TEST_EQUAL(m.str(), "");

    // not_line = Do not match ^ at start or $ at end
    TRY(r = Regex("^[A-Z][a-z]+", Regex::multiline));
    s = "Hello\nGoodbye";
    TRY(m = r(s, 0, Regex::not_line));
    TEST(m);
    TEST_EQUAL(m.str(), "Goodbye");
    TRY(r = Regex("[A-Z][a-z]+$", Regex::multiline));
    TRY(m = r(s, 0, Regex::not_line));
    TEST(m);
    TEST_EQUAL(m.str(), "Hello");

    // partial_hard = Hard partial match, prefer partial to full match
    TRY(r = Regex("\\b[a-z]+"));
    s = "Hello world";
    TRY(m = r(s, 0, Regex::partial_hard));
    TEST(m);
    TEST(! m.full());
    TEST(m.partial());
    TEST_EQUAL(m.str(), "world");
    TRY(r = Regex("\\b[a-z]+\\d+"));
    s = "Hello world";
    TRY(m = r(s, 0, Regex::partial_hard));
    TEST(m);
    TEST(! m.full());
    TEST(m.partial());
    TEST_EQUAL(m.str(), "world");

    // partial_soft = Soft partial match, prefer full to partial match
    TRY(r = Regex("\\b[a-z]+"));
    s = "Hello world";
    TRY(m = r(s, 0, Regex::partial_soft));
    TEST(m);
    TEST(m.full());
    TEST(! m.partial());
    TEST_EQUAL(m.str(), "world");
    TRY(r = Regex("\\b[a-z]+\\d+"));
    s = "Hello world";
    TRY(m = r(s, 0, Regex::partial_soft));
    TEST(m);
    TEST(! m.full());
    TEST(m.partial());
    TEST_EQUAL(m.str(), "world");

}

void test_unicorn_regex_grep() {

    Regex r;
    Regex::match_iterator mi;
    Regex::split_iterator si;
    Regex::match_range mr;
    Regex::split_range sr;
    Regex::partition_type p;
    std::string s = "Hello world. Goodbye.";
    size_t n = 0;

    TRY(r = Regex("\\d+"));
    TRY(mr = r.grep(s));
    TEST(mr.begin() == mr.end());
    TRY(n = r.count(s));
    TEST_EQUAL(n, 0);

    TRY(r = Regex("\\w+"));
    TRY(mr = r.grep(s));
    TEST(mr.begin() != mr.end());
    TRY(mi = mr.begin());
    TEST_EQUAL(mi->str(), "Hello");
    TRY(++mi);
    TEST(mi != mr.end());
    TEST_EQUAL(mi->str(), "world");
    TRY(++mi);
    TEST(mi != mr.end());
    TEST_EQUAL(mi->str(), "Goodbye");
    TRY(++mi);
    TEST(mi == mr.end());
    TRY(n = r.count(s));
    TEST_EQUAL(n, 3);

    TRY(r = Regex("@+"));
    TRY(sr = r.split(s));
    TEST(sr.begin() != sr.end());
    TRY(si = sr.begin());
    TEST_EQUAL(*si, s);
    TRY(++si);
    TEST(si == sr.end());

    TRY(r = Regex("[[:punct:]]+\\s*"));
    TRY(sr = r.split(s));
    TEST(sr.begin() != sr.end());
    TRY(si = sr.begin());
    TEST_EQUAL(*si, "Hello world");
    TRY(++si);
    TEST(si != sr.end());
    TEST_EQUAL(*si, "Goodbye");
    TRY(++si);
    TEST(si != sr.end());
    TEST_EQUAL(*si, "");
    TRY(++si);
    TEST(si == sr.end());

    TRY(r = Regex("@+"));
    TRY(p = r.partition(s));
    TEST_EQUAL(p.left, s);
    TEST_EQUAL(p.mid, "");
    TEST_EQUAL(p.right, "");

    TRY(r = Regex("[[:punct:]]+\\s*"));
    TRY(p = r.partition(s));
    TEST_EQUAL(p.left, "Hello world");
    TEST_EQUAL(p.mid, ". ");
    TEST_EQUAL(p.right, "Goodbye.");

}

void test_unicorn_regex_replace() {

    Regex r;
    std::string s1, s2;

    TRY(r = Regex("[a-z]"));
    s1 = "";

    s2 = "";  TRY(s2 = r.replace(s1, ""));                    TEST_EQUAL(s2, "");
    s2 = s1;  TRY(r.replace_in(s2, ""));                      TEST_EQUAL(s2, "");
    s2 = "";  TRY(s2 = r.replace(s1, "", 0, Regex::global));  TEST_EQUAL(s2, "");
    s2 = s1;  TRY(r.replace_in(s2, "", 0, Regex::global));    TEST_EQUAL(s2, "");

    s1 = "Hello world. Goodbye.";

    s2 = "";  TRY(s2 = r.replace(s1, "*"));                    TEST_EQUAL(s2, "H*llo world. Goodbye.");
    s2 = s1;  TRY(r.replace_in(s2, "*"));                      TEST_EQUAL(s2, "H*llo world. Goodbye.");
    s2 = "";  TRY(s2 = r.replace(s1, "*", 0, Regex::global));  TEST_EQUAL(s2, "H**** *****. G******.");
    s2 = s1;  TRY(r.replace_in(s2, "*", 0, Regex::global));    TEST_EQUAL(s2, "H**** *****. G******.");

    TRY(r = Regex("[a-z]+"));

    s2 = "";  TRY(s2 = r.replace(s1, "*"));                    TEST_EQUAL(s2, "H* world. Goodbye.");
    s2 = s1;  TRY(r.replace_in(s2, "*"));                      TEST_EQUAL(s2, "H* world. Goodbye.");
    s2 = "";  TRY(s2 = r.replace(s1, "*", 0, Regex::global));  TEST_EQUAL(s2, "H* *. G*.");
    s2 = s1;  TRY(r.replace_in(s2, "*", 0, Regex::global));    TEST_EQUAL(s2, "H* *. G*.");

    TRY(r = Regex("[a-z]+", Regex::icase));

    s2 = "";  TRY(s2 = r.replace(s1, "*"));                    TEST_EQUAL(s2, "* world. Goodbye.");
    s2 = s1;  TRY(r.replace_in(s2, "*"));                      TEST_EQUAL(s2, "* world. Goodbye.");
    s2 = "";  TRY(s2 = r.replace(s1, "*", 0, Regex::global));  TEST_EQUAL(s2, "* *. *.");
    s2 = s1;  TRY(r.replace_in(s2, "*", 0, Regex::global));    TEST_EQUAL(s2, "* *. *.");

    TRY(r = Regex("([a-z]+)(\\d+)?", Regex::icase));
    s1 = "abc123 def456 ghi789 XYZ";

    TRY(s2 = r.replace(s1, "<$1/$2>"));                                        TEST_EQUAL(s2, "<abc/123> def456 ghi789 XYZ");
    TRY(s2 = r.replace(s1, "<$1/$2>", 0, Regex::global));                      TEST_EQUAL(s2, "<abc/123> <def/456> <ghi/789> <XYZ/>");
    TRY(s2 = r.replace(s1, "\\Q$1$2\\E"));                                     TEST_EQUAL(s2, "$1$2 def456 ghi789 XYZ");
    TRY(s2 = r.replace(s1, "\\Q$1$2\\E", 0, Regex::global));                   TEST_EQUAL(s2, "$1$2 $1$2 $1$2 $1$2");
    TRY(s2 = r.replace(s1, "\\U$0\\E"));                                       TEST_EQUAL(s2, "ABC123 def456 ghi789 XYZ");
    TRY(s2 = r.replace(s1, "\\U$0\\E", 0, Regex::global));                     TEST_EQUAL(s2, "ABC123 DEF456 GHI789 XYZ");
    TRY(s2 = r.replace(s1, "\\L$0\\E"));                                       TEST_EQUAL(s2, "abc123 def456 ghi789 XYZ");
    TRY(s2 = r.replace(s1, "\\L$0\\E", 0, Regex::global));                     TEST_EQUAL(s2, "abc123 def456 ghi789 xyz");
    TRY(s2 = r.replace(s1, "\\u$0"));                                          TEST_EQUAL(s2, "Abc123 def456 ghi789 XYZ");
    TRY(s2 = r.replace(s1, "\\u$0", 0, Regex::global));                        TEST_EQUAL(s2, "Abc123 Def456 Ghi789 XYZ");
    TRY(s2 = r.replace(s1, "\\l$0"));                                          TEST_EQUAL(s2, "abc123 def456 ghi789 XYZ");
    TRY(s2 = r.replace(s1, "\\l$0", 0, Regex::global));                        TEST_EQUAL(s2, "abc123 def456 ghi789 xYZ");
    TRY(s2 = r.replace(s1, "<${1:-?}/${2:-?}>"));                              TEST_EQUAL(s2, "<abc/123> def456 ghi789 XYZ");
    TRY(s2 = r.replace(s1, "<${1:-?}/${2:-?}>", 0, Regex::global));            TEST_EQUAL(s2, "<abc/123> <def/456> <ghi/789> <XYZ/?>");
    TRY(s2 = r.replace(s1, "<${1:+[$1]:?}/${2:+[$2]:?}>"));                    TEST_EQUAL(s2, "<[abc]/[123]> def456 ghi789 XYZ");
    TRY(s2 = r.replace(s1, "<${1:+[$1]:?}/${2:+[$2]:?}>", 0, Regex::global));  TEST_EQUAL(s2, "<[abc]/[123]> <[def]/[456]> <[ghi]/[789]> <[XYZ]/?>");

}

void test_unicorn_regex_transform() {

    Regex::transform rt;
    std::string s1, s2;

    s1 = "";             s2 = "";  TRY(s2 = rt(s1));        TEST_EQUAL(s2, "");
    s1 = "";             s2 = s1;  TRY(rt.replace_in(s2));  TEST_EQUAL(s2, "");
    s1 = "Hello world";  s2 = "";  TRY(s2 = rt(s1));        TEST_EQUAL(s2, s1);
    s1 = "Hello world";  s2 = s1;  TRY(rt.replace_in(s2));  TEST_EQUAL(s2, s1);

    TRY(rt = Regex::transform("[a-z]+", "(\\U$0)"));

    s1 = "";             s2 = "";  TRY(s2 = rt(s1));        TEST_EQUAL(s2, "");
    s1 = "";             s2 = s1;  TRY(rt.replace_in(s2));  TEST_EQUAL(s2, "");
    s1 = "Hello world";  s2 = "";  TRY(s2 = rt(s1));        TEST_EQUAL(s2, "H(ELLO) world");
    s1 = "Hello world";  s2 = s1;  TRY(rt.replace_in(s2));  TEST_EQUAL(s2, "H(ELLO) world");

    TRY(rt = Regex::transform("[a-z]+", "(\\U$0)", Regex::global | Regex::icase));

    s1 = "";             s2 = "";  TRY(s2 = rt(s1));        TEST_EQUAL(s2, "");
    s1 = "";             s2 = s1;  TRY(rt.replace_in(s2));  TEST_EQUAL(s2, "");
    s1 = "Hello world";  s2 = "";  TRY(s2 = rt(s1));        TEST_EQUAL(s2, "(HELLO) (WORLD)");
    s1 = "Hello world";  s2 = s1;  TRY(rt.replace_in(s2));  TEST_EQUAL(s2, "(HELLO) (WORLD)");

}

void test_unicorn_regex_escape() {

    Regex r;
    Regex::match m;
    std::string s1, s2;

    s1 = "";                 TRY(s2 = Regex::escape(s1));  TRY(r = Regex(s2, Regex::full));  TRY(m = r(s1));  TEST(m);
    s1 = "Hello world";      TRY(s2 = Regex::escape(s1));  TRY(r = Regex(s2, Regex::full));  TRY(m = r(s1));  TEST(m);
    s1 = "$()*+.?[\\]^{|}";  TRY(s2 = Regex::escape(s1));  TRY(r = Regex(s2, Regex::full));  TRY(m = r(s1));  TEST(m);
    s1 = "\0\x1f\x7f\r\n"s;  TRY(s2 = Regex::escape(s1));  TRY(r = Regex(s2, Regex::full));  TRY(m = r(s1));  TEST(m);

}
