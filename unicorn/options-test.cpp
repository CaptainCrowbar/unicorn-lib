#include "unicorn/core.hpp"
#include "unicorn/options.hpp"
#include "rs-core/unit-test.hpp"
#include <sstream>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

namespace {

    std::ostringstream nowhere;

    void check_basic_options(Options& opt1) {

        Options opt2;
        U8string cmdline;

        TRY(opt1 = Options("App version 1.0"));
        TEST_EQUAL(opt1.version_text(), "App version 1.0");
        TRY(opt1.add("alpha", "Alpha option", Options::abbrev="a", Options::defvalue="ABC"));
        TRY(opt1.add("--number", "Number option", Options::abbrev="n", Options::defvalue="123"));

        {
            TRY(opt2 = opt1);
            cmdline = "app --version";
            std::ostringstream out;
            TEST(opt2.parse(cmdline, out));
            TEST_EQUAL(out.str(), "App version 1.0\n");
        }

        {
            TRY(opt2 = opt1);
            cmdline = "app --help";
            std::ostringstream out;
            TEST(opt2.parse(cmdline, out));
            TEST_EQUAL(out.str(),
                "\n"
                "App version 1.0\n"
                "\n"
                "Options:\n"
                "\n"
                "    --alpha, -a <arg>   = Alpha option (default \"ABC\")\n"
                "    --number, -n <arg>  = Number option (default 123)\n"
                "    --help, -h          = Show usage information\n"
                "    --version, -v       = Show version information\n"
                "\n"
            );
        }

        {
            TRY(opt2 = opt1);
            cmdline = "app";
            std::ostringstream out;
            TEST(! opt2.parse(cmdline, nowhere));
            TEST_EQUAL(out.str(), "");
            TEST(! opt2.has("alpha"));
            TEST_EQUAL(opt2.get<U8string>("alpha"), "ABC");
            TEST(! opt2.has("number"));
            TEST_EQUAL(opt2.get<int>("number"), 123);
            TEST_THROW_MATCH(opt2.has("nonexistent"), Options::spec_error, ": \"--nonexistent\"$");
            TEST_THROW_MATCH(opt2.get<U8string>("nonexistent"), Options::spec_error, ": \"--nonexistent\"$");
            TEST_THROW_MATCH(opt2.get_list<U8string>("nonexistent"), Options::spec_error, ": \"--nonexistent\"$");
        }

        TRY(opt2 = opt1);
        cmdline = "app --alpha xyz -n 999";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<U8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app --alpha=xyz -n=999";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<U8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app --alpha -n";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<U8string>("alpha"), "ABC");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 123);

        TRY(opt2 = opt1);
        cmdline = "app -a uvw xyz";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"xyz\"$");

        TRY(opt2 = opt1);
        cmdline = "app -a xyz -n 999";
        TEST(! opt2.parse(cmdline, nowhere, Options::quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<U8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app -a \"xyz\" -n \"999\"";
        TEST(! opt2.parse(cmdline, nowhere, Options::quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<U8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app -a \"uvw xyz\"";
        TEST(! opt2.parse(cmdline, nowhere, Options::quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<U8string>("alpha"), "uvw xyz");

        TRY(opt2 = opt1);
        cmdline = "app -a \"\"\"uvw\"\" \"\"xyz\"\"\"";
        TEST(! opt2.parse(cmdline, nowhere, Options::quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<U8string>("alpha"), "\"uvw\" \"xyz\"");

    }

    void check_boolean_options(Options& opt1) {

        Options opt2("Blank");
        U8string cmdline;

        TRY(opt1.add("--foo", "Positive option", Options::boolean, Options::abbrev="f"));
        TRY(opt1.add("--no-bar", "Negative option", Options::boolean));

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(! opt2.has("foo"));
        TEST(! opt2.has("bar"));
        TEST(! opt2.get<bool>("foo"));
        TEST(opt2.get<bool>("bar"));
        TEST_EQUAL(opt2.get<U8string>("foo"), "");
        TEST_EQUAL(opt2.get<U8string>("bar"), "1");

        TRY(opt2 = opt1);
        cmdline = "app --foo --bar";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("foo"));
        TEST(opt2.has("bar"));
        TEST(opt2.get<bool>("foo"));
        TEST(opt2.get<bool>("bar"));
        TEST_EQUAL(opt2.get<U8string>("foo"), "1");
        TEST_EQUAL(opt2.get<U8string>("bar"), "1");

        TRY(opt2 = opt1);
        cmdline = "app --no-foo --no-bar";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("foo"));
        TEST(opt2.has("bar"));
        TEST(! opt2.get<bool>("foo"));
        TEST(! opt2.get<bool>("bar"));
        TEST_EQUAL(opt2.get<U8string>("foo"), "0");
        TEST_EQUAL(opt2.get<U8string>("bar"), "0");

        TRY(opt2 = opt1);
        cmdline = "app -f";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("foo"));
        TEST(opt2.get<bool>("foo"));
        TEST_EQUAL(opt2.get<U8string>("foo"), "1");

    }

    void check_multiple_options(Options& opt1) {

        Options opt2("Blank");
        U8string cmdline;
        Strings sv;

        TRY(opt1.add("list", "List option", Options::multi, Options::abbrev="l"));

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(! opt2.has("list"));
        TEST_EQUAL(opt2.get<U8string>("list"), "");
        TRY(sv = opt2.get_list<U8string>("list"));
        TEST(sv.empty());

        TRY(opt2 = opt1);
        cmdline = "app --list abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("list"));
        TEST_EQUAL(opt2.get<U8string>("list"), "abc");
        TRY(sv = opt2.get_list<U8string>("list"));
        TEST_EQUAL(sv.size(), 1);
        TEST_EQUAL(to_str(sv), "[abc]");

        TRY(opt2 = opt1);
        cmdline = "app --list abc def ghi";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("list"));
        TEST_EQUAL(opt2.get<U8string>("list"), "abc def ghi");
        TRY(sv = opt2.get_list<U8string>("list"));
        TEST_EQUAL(sv.size(), 3);
        TEST_EQUAL(to_str(sv), "[abc,def,ghi]");

    }

    void check_required_options(Options& opt1) {

        Options opt2("Blank");
        U8string cmdline;

        TRY(opt1.add("required", "Required option", Options::required, Options::abbrev="r"));

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--required\"$");

        TRY(opt2 = opt1);
        cmdline = "app --required abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("required"));
        TEST_EQUAL(opt2.get<U8string>("required"), "abc");

        TRY(opt2 = opt1);
        cmdline = "app --version";
        std::ostringstream out;
        TEST(opt2.parse(cmdline, out));
        TEST_EQUAL(out.str(), "App version 1.0\n");

    }

    void check_anonymous_arguments(Options& opt1) {

        Options opt2("Blank");
        U8string cmdline;
        Strings sv;

        TRY(opt1.add("head", "First anonymous argument", Options::anon));

        TRY(opt2 = opt1);
        cmdline = "app --required abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(! opt2.has("head"));
        TEST_EQUAL(opt2.get<U8string>("head"), "");

        TRY(opt2 = opt1);
        cmdline = "app --required abc def";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<U8string>("head"), "def");

        TRY(opt2 = opt1);
        cmdline = "app --required abc def ghi";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"ghi\"$");

        TRY(opt1.add("tail", "Other anonymous arguments", Options::anon, Options::multi));

        TRY(opt2 = opt1);
        cmdline = "app --required abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(! opt2.has("head"));
        TEST_EQUAL(opt2.get<U8string>("head"), "");
        TEST(! opt2.has("tail"));
        TEST_EQUAL(opt2.get<U8string>("tail"), "");
        TRY(sv = opt2.get_list<U8string>("tail"));
        TEST(sv.empty());

        TRY(opt2 = opt1);
        cmdline = "app --required abc def";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<U8string>("head"), "def");
        TEST(! opt2.has("tail"));
        TEST_EQUAL(opt2.get<U8string>("tail"), "");
        TRY(sv = opt2.get_list<U8string>("tail"));
        TEST(sv.empty());

        TRY(opt2 = opt1);
        cmdline = "app --required abc def ghi";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<U8string>("head"), "def");
        TEST(opt2.has("tail"));
        TEST_EQUAL(opt2.get<U8string>("tail"), "ghi");
        TRY(sv = opt2.get_list<U8string>("tail"));
        TEST_EQUAL(sv.size(), 1);
        TEST_EQUAL(to_str(sv), "[ghi]");

        TRY(opt2 = opt1);
        cmdline = "app --required abc def ghi jkl";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<U8string>("head"), "def");
        TEST(opt2.has("tail"));
        TEST_EQUAL(opt2.get<U8string>("tail"), "ghi jkl");
        TRY(sv = opt2.get_list<U8string>("tail"));
        TEST_EQUAL(sv.size(), 2);
        TEST_EQUAL(to_str(sv), "[ghi,jkl]");

    }

    void check_group_options() {

        Options opt1("App");
        TRY(opt1.add("group1a", "Group 1 a", Options::group="neddie"));
        TRY(opt1.add("group1b", "Group 1 b", Options::group="neddie"));
        TRY(opt1.add("group2a", "Group 2 a", Options::group="eccles"));
        TRY(opt1.add("group2b", "Group 2 b", Options::group="eccles"));

        Options opt2("Blank");
        U8string cmdline;

        TRY(opt2 = opt1);
        cmdline = "app --group1a abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<U8string>("group1a"), "abc");

        TRY(opt2 = opt1);
        cmdline = "app --group1a abc --group1b def";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--group1a\", \"--group1b\"$");

        TRY(opt2 = opt1);
        cmdline = "app --group1a abc --group2a def";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<U8string>("group1a"), "abc");
        TEST_EQUAL(opt2.get<U8string>("group2a"), "def");

    }

    void check_argument_patterns() {

        Options opt1("App");
        TRY(opt1.add("alpha", "Alpha", Options::abbrev="a", Options::anon, Options::defvalue="Hello", Options::multi, Options::pattern="[[:alpha:]]+"));
        TRY(opt1.add("number", "Number", Options::abbrev="n", Options::defvalue="42", Options::multi, Options::pattern="\\d+"));
        TEST_THROW_MATCH(opt1.add("word", "Word", Options::defvalue="*", Options::pattern="\\w+"), Options::spec_error, ": \"word\"$");

        Options opt2("Blank");
        U8string cmdline;

        TRY(opt2 = opt1);
        cmdline = "app --alpha abc --number 123";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<U8string>("alpha"), "abc");
        TEST_EQUAL(opt2.get<int>("number"), 123);

        TRY(opt2 = opt1);
        cmdline = "app abc -n 123";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<U8string>("alpha"), "abc");
        TEST_EQUAL(opt2.get<int>("number"), 123);

        TRY(opt2 = opt1);
        cmdline = "app --alpha 123";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--alpha\", \"123\"$");

        TRY(opt2 = opt1);
        cmdline = "app --number abc";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--number\", \"abc\"$");

        TRY(opt2 = opt1);
        cmdline = "app -a abc 123 -n 456";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--alpha\", \"123\"$");

        TRY(opt2 = opt1);
        cmdline = "app -a abc -n 123 def";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--number\", \"def\"$");

        TRY(opt2 = opt1);
        cmdline = "app abc 123";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--alpha\", \"123\"$");

        TRY(opt1 = Options("App"));
        TRY(opt1.add("int", "Integer", Options::integer, Options::abbrev="i"));
        TRY(opt1.add("uint", "Unsigned integer", Options::uinteger, Options::abbrev="u"));
        TRY(opt1.add("float", "Float", Options::floating, Options::abbrev="f"));

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<int>("int"), 0);
        TEST_EQUAL(opt2.get<unsigned>("uint"), 0);
        TEST_EQUAL(opt2.get<float>("float"), 0);

        TRY(opt2 = opt1);
        cmdline = "app --int -42 --uint 42 --float 1.234e5";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<int>("int"), -42);
        TEST_EQUAL(opt2.get<unsigned>("uint"), 42);
        TEST_EQUAL(opt2.get<float>("float"), 123400);

        TRY(opt2 = opt1);
        cmdline = "app --int 0x42";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<int>("int"), 66);

        TRY(opt2 = opt1);
        cmdline = "app --int 24k";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<int>("int"), 24000);

        TRY(opt2 = opt1);
        cmdline = "app --int 25MB";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<int>("int"), 25000000);

        TRY(opt2 = opt1);
        cmdline = "app --float 2.5MB";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<float>("float"), 2500000);

        TRY(opt2 = opt1);
        cmdline = "app --int 1234.5";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--int\", \"1234.5\"$");

        TRY(opt2 = opt1);
        cmdline = "app --uint -1234";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--uint\", \"-1234\"$");

        TRY(opt2 = opt1);
        cmdline = "app --float 0x1234";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::command_error, ": \"--float\", \"0x1234\"$");

        TRY(opt1 = Options("App"));
        TRY(opt1.add("int", "Integer", Options::anon, Options::required, Options::abbrev="i"));
        TRY(opt1.add("str", "String", Options::required, Options::abbrev="s"));

        TRY(opt2 = opt1);
        cmdline = "app -s hello 42";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<int>("int"), 42);
        TEST_EQUAL(opt2.get<U8string>("str"), "hello");

        TRY(opt2 = opt1);
        cmdline = "app 42 -s hello";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<int>("int"), 42);
        TEST_EQUAL(opt2.get<U8string>("str"), "hello");

    }

    void check_help(Options& opt1) {

        Options opt2("Blank");
        U8string cmdline;

        {
            TRY(opt2 = opt1);
            cmdline = "app --help";
            std::ostringstream out;
            TEST(opt2.parse(cmdline, out));
            TEST_EQUAL(out.str(),
                "\n"
                "App version 1.0\n"
                "\n"
                "Options:\n"
                "\n"
                "    --alpha, -a <arg>     = Alpha option (default \"ABC\")\n"
                "    --number, -n <arg>    = Number option (default 123)\n"
                "    --foo, -f             = Positive option\n"
                "    --no-bar              = Negative option\n"
                "    --list, -l <arg> ...  = List option\n"
                "    --required, -r <arg>  = Required option (required)\n"
                "    [--head] <arg>        = First anonymous argument\n"
                "    [--tail] <arg> ...    = Other anonymous arguments\n"
                "    --help, -h            = Show usage information\n"
                "    --version, -v         = Show version information\n"
                "\n"
            );
        }

        {
            TRY(opt2 = opt1);
            TRY(opt2.add(Options::autohelp));
            cmdline = "app";
            std::ostringstream out;
            TEST(opt2.parse(cmdline, out));
            TEST_EQUAL(out.str(),
                "\n"
                "App version 1.0\n"
                "\n"
                "Options:\n"
                "\n"
                "    --alpha, -a <arg>     = Alpha option (default \"ABC\")\n"
                "    --number, -n <arg>    = Number option (default 123)\n"
                "    --foo, -f             = Positive option\n"
                "    --no-bar              = Negative option\n"
                "    --list, -l <arg> ...  = List option\n"
                "    --required, -r <arg>  = Required option (required)\n"
                "    [--head] <arg>        = First anonymous argument\n"
                "    [--tail] <arg> ...    = Other anonymous arguments\n"
                "    --help, -h            = Show usage information\n"
                "    --version, -v         = Show version information\n"
                "\n"
            );
        }

    }

    void check_inserted_info() {

        Options opt("App version 1.0");
        U8string help;

        TRY(opt = Options("App 1.0"));
        TRY(opt.add("Intro."));
        TRY(opt.add("alpha", "Alpha option"));
        TRY(help = opt.help_text());
        TEST_EQUAL(help,
            "\n"
            "App 1.0\n"
            "\n"
            "Intro.\n"
            "\n"
            "Options:\n"
            "\n"
            "    --alpha <arg>  = Alpha option\n"
            "\n"
        );

        TRY(opt = Options("App 1.0"));
        TRY(opt.add("alpha", "Alpha option"));
        TRY(opt.add("Outro."));
        TRY(help = opt.help_text());
        TEST_EQUAL(help,
            "\n"
            "App 1.0\n"
            "\n"
            "Options:\n"
            "\n"
            "    --alpha <arg>  = Alpha option\n"
            "\n"
            "Outro.\n"
            "\n"
        );

        TRY(opt = Options("App 1.0"));
        TRY(opt.add("Intro."));
        TRY(opt.add("alpha", "Alpha option"));
        TRY(opt.add("Some info."));
        TRY(opt.add("bravo", "Bravo option"));
        TRY(opt.add("charlie", "Charlie option"));
        TRY(opt.add("Some more info."));
        TRY(opt.add("Yet more info."));
        TRY(opt.add("delta", "Delta option"));
        TRY(opt.add("Outro."));
        TRY(help = opt.help_text());
        TEST_EQUAL(help,
            "\n"
            "App 1.0\n"
            "\n"
            "Intro.\n"
            "\n"
            "Options:\n"
            "\n"
            "    --alpha <arg>    = Alpha option\n"
            "\n"
            "Some info.\n"
            "\n"
            "    --bravo <arg>    = Bravo option\n"
            "    --charlie <arg>  = Charlie option\n"
            "\n"
            "Some more info.\n"
            "\n"
            "Yet more info.\n"
            "\n"
            "    --delta <arg>    = Delta option\n"
            "\n"
            "Outro.\n"
            "\n"
        );

    }

}

TEST_MODULE(unicorn, options) {

    Options opt1;

    check_basic_options(opt1);
    check_boolean_options(opt1);
    check_multiple_options(opt1);
    check_required_options(opt1);
    check_anonymous_arguments(opt1);
    check_group_options();
    check_argument_patterns();
    check_help(opt1);
    check_inserted_info();

}
