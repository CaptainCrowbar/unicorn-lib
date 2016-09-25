#include "unicorn/core.hpp"
#include "unicorn/options.hpp"
#include "prion/unit-test.hpp"
#include <sstream>
#include <string>
#include <vector>

using namespace std::literals;
using namespace Unicorn;

namespace {

    std::ostringstream nowhere;

    void check_basic_options(Options& opt1) {

        opt1 = Options("App version 1.0");
        Options opt2;
        u8string cmdline;

        TEST_EQUAL(opt1.version(), "App version 1.0");
        TRY(opt1.add("alpha", "Alpha option", opt_abbrev="a", opt_default="ABC"));
        TRY(opt1.add("--number", "Number option", opt_abbrev="n", opt_default="123"));

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
            TEST_EQUAL(opt2.get<u8string>("alpha"), "ABC");
            TEST(! opt2.has("number"));
            TEST_EQUAL(opt2.get<int>("number"), 123);
        }

        TRY(opt2 = opt1);
        cmdline = "app --alpha xyz -n 999";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app --alpha=xyz -n=999";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app --alpha -n";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "ABC");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 123);

        TRY(opt2 = opt1);
        cmdline = "app -a uvw xyz";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::CommandError, ": \"xyz\"$");

        TRY(opt2 = opt1);
        cmdline = "app -a xyz -n 999";
        TEST(! opt2.parse(cmdline, nowhere, opt_quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app -a \"xyz\" -n \"999\"";
        TEST(! opt2.parse(cmdline, nowhere, opt_quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app -a \"uvw xyz\"";
        TEST(! opt2.parse(cmdline, nowhere, opt_quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "uvw xyz");

        TRY(opt2 = opt1);
        cmdline = "app -a \"\"\"uvw\"\" \"\"xyz\"\"\"";
        TEST(! opt2.parse(cmdline, nowhere, opt_quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "\"uvw\" \"xyz\"");

    }

    void check_boolean_options(Options& opt1) {

        Options opt2("Blank");
        u8string cmdline;

        TRY(opt1.add("--foo", "Positive option", opt_bool, opt_abbrev="f"));
        TRY(opt1.add("--no-bar", "Negative option", opt_bool));

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(! opt2.has("foo"));
        TEST(! opt2.has("bar"));
        TEST(! opt2.get<bool>("foo"));
        TEST(opt2.get<bool>("bar"));
        TEST_EQUAL(opt2.get<u8string>("foo"), "");
        TEST_EQUAL(opt2.get<u8string>("bar"), "1");

        TRY(opt2 = opt1);
        cmdline = "app --foo --bar";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("foo"));
        TEST(opt2.has("bar"));
        TEST(opt2.get<bool>("foo"));
        TEST(opt2.get<bool>("bar"));
        TEST_EQUAL(opt2.get<u8string>("foo"), "1");
        TEST_EQUAL(opt2.get<u8string>("bar"), "1");

        TRY(opt2 = opt1);
        cmdline = "app --no-foo --no-bar";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("foo"));
        TEST(opt2.has("bar"));
        TEST(! opt2.get<bool>("foo"));
        TEST(! opt2.get<bool>("bar"));
        TEST_EQUAL(opt2.get<u8string>("foo"), "0");
        TEST_EQUAL(opt2.get<u8string>("bar"), "0");

        TRY(opt2 = opt1);
        cmdline = "app -f";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("foo"));
        TEST(opt2.get<bool>("foo"));
        TEST_EQUAL(opt2.get<u8string>("foo"), "1");

    }

    void check_multiple_options(Options& opt1) {

        Options opt2("Blank");
        u8string cmdline;
        vector<u8string> sv;

        TRY(opt1.add("list", "List option", opt_multi, opt_abbrev="l"));

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(! opt2.has("list"));
        TEST_EQUAL(opt2.get<u8string>("list"), "");
        TRY(sv = opt2.get_list<u8string>("list"));
        TEST(sv.empty());

        TRY(opt2 = opt1);
        cmdline = "app --list abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("list"));
        TEST_EQUAL(opt2.get<u8string>("list"), "abc");
        TRY(sv = opt2.get_list<u8string>("list"));
        TEST_EQUAL(sv.size(), 1);
        TEST_EQUAL(to_str(sv), "[abc]");

        TRY(opt2 = opt1);
        cmdline = "app --list abc def ghi";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("list"));
        TEST_EQUAL(opt2.get<u8string>("list"), "abc def ghi");
        TRY(sv = opt2.get_list<u8string>("list"));
        TEST_EQUAL(sv.size(), 3);
        TEST_EQUAL(to_str(sv), "[abc,def,ghi]");

    }

    void check_required_options(Options& opt1) {

        Options opt2("Blank");
        u8string cmdline;

        TRY(opt1.add("required", "Required option", opt_require, opt_abbrev="r"));

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::CommandError, ": \"--required\"$");

        TRY(opt2 = opt1);
        cmdline = "app --required abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("required"));
        TEST_EQUAL(opt2.get<u8string>("required"), "abc");

        TRY(opt2 = opt1);
        cmdline = "app --version";
        std::ostringstream out;
        TEST(opt2.parse(cmdline, out));
        TEST_EQUAL(out.str(), "App version 1.0\n");

    }

    void check_anonymous_arguments(Options& opt1) {

        Options opt2("Blank");
        u8string cmdline;
        vector<u8string> sv;

        TRY(opt1.add("head", "First anonymous argument", opt_anon));

        TRY(opt2 = opt1);
        cmdline = "app --required abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(! opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "");

        TRY(opt2 = opt1);
        cmdline = "app --required abc def";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "def");

        TRY(opt2 = opt1);
        cmdline = "app --required abc def ghi";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::CommandError, ": \"ghi\"$");

        TRY(opt1.add("tail", "Other anonymous arguments", opt_anon, opt_multi));

        TRY(opt2 = opt1);
        cmdline = "app --required abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(! opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "");
        TEST(! opt2.has("tail"));
        TEST_EQUAL(opt2.get<u8string>("tail"), "");
        TRY(sv = opt2.get_list<u8string>("tail"));
        TEST(sv.empty());

        TRY(opt2 = opt1);
        cmdline = "app --required abc def";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "def");
        TEST(! opt2.has("tail"));
        TEST_EQUAL(opt2.get<u8string>("tail"), "");
        TRY(sv = opt2.get_list<u8string>("tail"));
        TEST(sv.empty());

        TRY(opt2 = opt1);
        cmdline = "app --required abc def ghi";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "def");
        TEST(opt2.has("tail"));
        TEST_EQUAL(opt2.get<u8string>("tail"), "ghi");
        TRY(sv = opt2.get_list<u8string>("tail"));
        TEST_EQUAL(sv.size(), 1);
        TEST_EQUAL(to_str(sv), "[ghi]");

        TRY(opt2 = opt1);
        cmdline = "app --required abc def ghi jkl";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "def");
        TEST(opt2.has("tail"));
        TEST_EQUAL(opt2.get<u8string>("tail"), "ghi jkl");
        TRY(sv = opt2.get_list<u8string>("tail"));
        TEST_EQUAL(sv.size(), 2);
        TEST_EQUAL(to_str(sv), "[ghi,jkl]");

    }

    void check_group_options() {

        Options opt1("App");
        TRY(opt1.add("group1a", "Group 1 a", opt_group="neddie"));
        TRY(opt1.add("group1b", "Group 1 b", opt_group="neddie"));
        TRY(opt1.add("group2a", "Group 2 a", opt_group="eccles"));
        TRY(opt1.add("group2b", "Group 2 b", opt_group="eccles"));

        Options opt2("Blank");
        u8string cmdline;

        TRY(opt2 = opt1);
        cmdline = "app --group1a abc";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<u8string>("group1a"), "abc");

        TRY(opt2 = opt1);
        cmdline = "app --group1a abc --group1b def";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::CommandError, ": \"--group1a\", \"--group1b\"$");

        TRY(opt2 = opt1);
        cmdline = "app --group1a abc --group2a def";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<u8string>("group1a"), "abc");
        TEST_EQUAL(opt2.get<u8string>("group2a"), "def");

    }

    void check_argument_patterns() {

        Options opt1("App");
        TRY(opt1.add("alpha", "Alpha", opt_default="Hello", opt_pattern="[[:alpha:]]+"));
        TRY(opt1.add("number", "Number", opt_default="42", opt_pattern="\\d+"));
        TEST_THROW_MATCH(opt1.add("word", "Word", opt_default="*", opt_pattern="\\w+"), Options::SpecError, ": \"word\"$");

        Options opt2("Blank");
        u8string cmdline;

        TRY(opt2 = opt1);
        cmdline = "app --alpha abc --number 123";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "abc");
        TEST_EQUAL(opt2.get<int>("number"), 123);

        TRY(opt2 = opt1);
        cmdline = "app --alpha 123";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::CommandError, ": \"--alpha\", \"123\"$");

        TRY(opt2 = opt1);
        cmdline = "app --number abc";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::CommandError, ": \"--number\", \"abc\"$");

        opt1 = Options("App");
        TRY(opt1.add("int", "Integer", opt_int, opt_abbrev="i"));
        TRY(opt1.add("uint", "Unsigned integer", opt_uint, opt_abbrev="u"));
        TRY(opt1.add("float", "Float", opt_float, opt_abbrev="f"));

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
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::CommandError, ": \"--int\", \"1234.5\"$");

        TRY(opt2 = opt1);
        cmdline = "app --uint -1234";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::CommandError, ": \"--uint\", \"-1234\"$");

        TRY(opt2 = opt1);
        cmdline = "app --float 0x1234";
        TEST_THROW_MATCH(opt2.parse(cmdline), Options::CommandError, ": \"--float\", \"0x1234\"$");

        opt1 = Options("App");
        TRY(opt1.add("int", "Integer", opt_anon, opt_require, opt_abbrev="i"));
        TRY(opt1.add("str", "String", opt_require, opt_abbrev="s"));

        TRY(opt2 = opt1);
        cmdline = "app -s hello 42";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<int>("int"), 42);
        TEST_EQUAL(opt2.get<u8string>("str"), "hello");

        TRY(opt2 = opt1);
        cmdline = "app 42 -s hello";
        TEST(! opt2.parse(cmdline, nowhere));
        TEST_EQUAL(opt2.get<int>("int"), 42);
        TEST_EQUAL(opt2.get<u8string>("str"), "hello");

    }

    void check_help(Options& opt1) {

        Options opt2("Blank");
        u8string cmdline;

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
            TRY(opt2.autohelp());
            cmdline = "app";
            std::ostringstream out;
            TEST(opt2.parse(cmdline, out));
            TEST_EQUAL(out.str(),
                "\n"
                "App version 1.0\n"
                "\n"
                "Options:\n"
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
        u8string help;

        TRY(opt = Options("App 1.0"));
        TRY(opt.add("Intro"));
        TRY(opt.add("alpha", "Alpha option"));
        TRY(help = opt.help());
        TEST_EQUAL(help,
            "\n"
            "App 1.0\n"
            "\n"
            "Intro\n"
            "\n"
            "Options:\n"
            "    --alpha <arg>  = Alpha option\n"
            "\n"
        );

        TRY(opt = Options("App 1.0"));
        TRY(opt.add("alpha", "Alpha option"));
        TRY(opt.add("Outro"));
        TRY(help = opt.help());
        TEST_EQUAL(help,
            "\n"
            "App 1.0\n"
            "\n"
            "Options:\n"
            "    --alpha <arg>  = Alpha option\n"
            "\n"
            "Outro\n"
            "\n"
        );

        TRY(opt = Options("App 1.0"));
        TRY(opt.add("Intro"));
        TRY(opt.add("alpha", "Alpha option"));
        TRY(opt.add("Some info."));
        TRY(opt.add("bravo", "Bravo option"));
        TRY(opt.add("charlie", "Charlie option"));
        TRY(opt.add("Some more info."));
        TRY(opt.add("delta", "Delta option"));
        TRY(opt.add("Outro"));
        TRY(help = opt.help());
        TEST_EQUAL(help,
            "\n"
            "App 1.0\n"
            "\n"
            "Intro\n"
            "\n"
            "Options:\n"
            "    --alpha <arg>    = Alpha option\n"
            "\n"
            "Some info.\n"
            "\n"
            "    --bravo <arg>    = Bravo option\n"
            "    --charlie <arg>  = Charlie option\n"
            "\n"
            "Some more info.\n"
            "\n"
            "    --delta <arg>    = Delta option\n"
            "\n"
            "Outro\n"
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
