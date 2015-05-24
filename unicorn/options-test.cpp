#include "crow/core.hpp"
#include "crow/unit-test.hpp"
#include "unicorn/options.hpp"
#include <string>
#include <vector>

using namespace std::literals;
using namespace Crow;
using namespace Unicorn;

namespace {

    void check_basic_options(Options& opt1) {

        Options opt2 = opt1;
        u8string cmdline;

        TEST_EQUAL(opt1.version(), "App version 1.0");
        TRY(opt1.add("alpha", 'a', "Alpha option", 0, "ABC"));
        TRY(opt1.add("--number", 'n', "Number option", 0, "123"));

        TRY(opt2 = opt1);
        cmdline = "app --version";
        TEST_THROW_EQUAL(opt2.parse(cmdline), HelpRequest, "App version 1.0");

        TRY(opt2 = opt1);
        cmdline = "app --help";
        TEST_THROW_EQUAL(opt2.parse(cmdline), HelpRequest,
            "\n"
            "App version 1.0\n"
            "\n"
            "Intro\n"
            "\n"
            "Options:\n"
            "    --alpha, -a <arg>   = Alpha option (default ABC)\n"
            "    --number, -n <arg>  = Number option (default 123)\n"
            "    --help, -h          = Show usage information\n"
            "    --version, -v       = Show version information\n"
            "\n"
            "Outro\n"
            "\n"
        );

        TRY(opt2 = opt1);
        cmdline = "app";
        TRY(opt2.parse(cmdline));
        TEST(! opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "ABC");
        TEST(! opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 123);

        TRY(opt2 = opt1);
        cmdline = "app --alpha xyz -n 999";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app --alpha=xyz -n=999";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app --alpha -n";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 0);

        TRY(opt2 = opt1);
        cmdline = "app -a uvw xyz";
        TEST_THROW(opt2.parse(cmdline), CommandLineError);

        TRY(opt2 = opt1);
        cmdline = "app -a xyz -n 999";
        TRY(opt2.parse(cmdline, opt_quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app -a \"xyz\" -n \"999\"";
        TRY(opt2.parse(cmdline, opt_quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "xyz");
        TEST(opt2.has("number"));
        TEST_EQUAL(opt2.get<int>("number"), 999);

        TRY(opt2 = opt1);
        cmdline = "app -a \"uvw xyz\"";
        TRY(opt2.parse(cmdline, opt_quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "uvw xyz");

        TRY(opt2 = opt1);
        cmdline = "app -a \"\"\"uvw\"\" \"\"xyz\"\"\"";
        TRY(opt2.parse(cmdline, opt_quoted));
        TEST(opt2.has("alpha"));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "\"uvw\" \"xyz\"");

    }

    void check_switch_options(Options& opt1) {

        Options opt2 = opt1;
        u8string cmdline;

        TRY(opt1.add("switch", 's', "Switch option", opt_boolean));

        TRY(opt2 = opt1);
        cmdline = "app";
        TRY(opt2.parse(cmdline));
        TEST(! opt2.has("switch"));
        TEST(! opt2.get<bool>("switch"));
        TEST_EQUAL(opt2.get<u8string>("switch"), "");

        TRY(opt2 = opt1);
        cmdline = "app -s";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("switch"));
        TEST(opt2.get<bool>("switch"));
        TEST_EQUAL(opt2.get<u8string>("switch"), "1");

    }

    void check_multiple_options(Options& opt1) {

        Options opt2 = opt1;
        u8string cmdline;
        std::vector<u8string> sv;

        TRY(opt1.add("list", 'l', "List option", opt_multiple));

        TRY(opt2 = opt1);
        cmdline = "app";
        TRY(opt2.parse(cmdline));
        TEST(! opt2.has("list"));
        TEST_EQUAL(opt2.get<u8string>("list"), "");
        TRY(sv = opt2.getlist<u8string>("list"));
        TEST(sv.empty());

        TRY(opt2 = opt1);
        cmdline = "app --list abc";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("list"));
        TEST_EQUAL(opt2.get<u8string>("list"), "abc");
        TRY(sv = opt2.getlist<u8string>("list"));
        TEST_EQUAL(sv.size(), 1);
        TEST_EQUAL(to_str(sv), "[abc]");

        TRY(opt2 = opt1);
        cmdline = "app --list abc def ghi";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("list"));
        TEST_EQUAL(opt2.get<u8string>("list"), "abc def ghi");
        TRY(sv = opt2.getlist<u8string>("list"));
        TEST_EQUAL(sv.size(), 3);
        TEST_EQUAL(to_str(sv), "[abc,def,ghi]");

    }

    void check_required_options(Options& opt1) {

        Options opt2 = opt1;
        u8string cmdline;

        TRY(opt1.add("required", 'r', "Required option", opt_required));

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST_THROW(opt2.parse(cmdline), CommandLineError);

        TRY(opt2 = opt1);
        cmdline = "app --required abc";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("required"));
        TEST_EQUAL(opt2.get<u8string>("required"), "abc");

        TRY(opt2 = opt1);
        cmdline = "app --version";
        TEST_THROW_EQUAL(opt2.parse(cmdline), HelpRequest, "App version 1.0");

    }

    void check_automatic_help(Options& opt1) {

        Options opt2 = opt1;
        u8string cmdline;

        TRY(opt1.autohelp());

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST_THROW_EQUAL(opt2.parse(cmdline), HelpRequest,
            "\n"
            "App version 1.0\n"
            "\n"
            "Intro\n"
            "\n"
            "Options:\n"
            "    --alpha, -a <arg>     = Alpha option (default ABC)\n"
            "    --number, -n <arg>    = Number option (default 123)\n"
            "    --switch, -s          = Switch option\n"
            "    --list, -l <arg> ...  = List option\n"
            "    --required, -r <arg>  = Required option (required)\n"
            "    --help, -h            = Show usage information\n"
            "    --version, -v         = Show version information\n"
            "\n"
            "Outro\n"
            "\n"
        );

    }

    void check_anonymous_arguments(Options& opt1) {

        Options opt2 = opt1;
        u8string cmdline;
        std::vector<u8string> sv;

        TRY(opt1.add("head", '\0', "First anonymous argument", opt_anon));

        TRY(opt2 = opt1);
        cmdline = "app --required abc";
        TRY(opt2.parse(cmdline));
        TEST(! opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "");

        TRY(opt2 = opt1);
        cmdline = "app --required abc def";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "def");

        TRY(opt2 = opt1);
        cmdline = "app --required abc def ghi";
        TEST_THROW(opt2.parse(cmdline), CommandLineError);

        TRY(opt1.add("tail", '\0', "Other anonymous arguments", opt_anon | opt_multiple));

        TRY(opt2 = opt1);
        cmdline = "app --required abc";
        TRY(opt2.parse(cmdline));
        TEST(! opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "");
        TEST(! opt2.has("tail"));
        TEST_EQUAL(opt2.get<u8string>("tail"), "");
        TRY(sv = opt2.getlist<u8string>("tail"));
        TEST(sv.empty());

        TRY(opt2 = opt1);
        cmdline = "app --required abc def";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "def");
        TEST(! opt2.has("tail"));
        TEST_EQUAL(opt2.get<u8string>("tail"), "");
        TRY(sv = opt2.getlist<u8string>("tail"));
        TEST(sv.empty());

        TRY(opt2 = opt1);
        cmdline = "app --required abc def ghi";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "def");
        TEST(opt2.has("tail"));
        TEST_EQUAL(opt2.get<u8string>("tail"), "ghi");
        TRY(sv = opt2.getlist<u8string>("tail"));
        TEST_EQUAL(sv.size(), 1);
        TEST_EQUAL(to_str(sv), "[ghi]");

        TRY(opt2 = opt1);
        cmdline = "app --required abc def ghi jkl";
        TRY(opt2.parse(cmdline));
        TEST(opt2.has("head"));
        TEST_EQUAL(opt2.get<u8string>("head"), "def");
        TEST(opt2.has("tail"));
        TEST_EQUAL(opt2.get<u8string>("tail"), "ghi jkl");
        TRY(sv = opt2.getlist<u8string>("tail"));
        TEST_EQUAL(sv.size(), 2);
        TEST_EQUAL(to_str(sv), "[ghi,jkl]");

    }

    void check_help_text(Options& opt1) {

        Options opt2 = opt1;
        u8string cmdline;

        TRY(opt2 = opt1);
        cmdline = "app";
        TEST_THROW_EQUAL(opt2.parse(cmdline), HelpRequest,
            "\n"
            "App version 1.0\n"
            "\n"
            "Intro\n"
            "\n"
            "Options:\n"
            "    --alpha, -a <arg>     = Alpha option (default ABC)\n"
            "    --number, -n <arg>    = Number option (default 123)\n"
            "    --switch, -s          = Switch option\n"
            "    --list, -l <arg> ...  = List option\n"
            "    --required, -r <arg>  = Required option (required)\n"
            "    [--head] <arg>        = First anonymous argument\n"
            "    [--tail] <arg> ...    = Other anonymous arguments\n"
            "    --help, -h            = Show usage information\n"
            "    --version, -v         = Show version information\n"
            "\n"
            "Outro\n"
            "\n"
        );

    }

    void check_group_options() {

        Options opt1("App");
        TRY(opt1.add("group1a", '\0', "Group 1 a", 0, "", "", "neddie"));
        TRY(opt1.add("group1b", '\0', "Group 1 b", 0, "", "", "neddie"));
        TRY(opt1.add("group2a", '\0', "Group 2 a", 0, "", "", "eccles"));
        TRY(opt1.add("group2b", '\0', "Group 2 b", 0, "", "", "eccles"));

        Options opt2 = opt1;
        u8string cmdline;

        TRY(opt2 = opt1);
        cmdline = "app --group1a abc";
        TRY(opt2.parse(cmdline));
        TEST_EQUAL(opt2.get<u8string>("group1a"), "abc");

        TRY(opt2 = opt1);
        cmdline = "app --group1a abc --group1b def";
        TEST_THROW(opt2.parse(cmdline), CommandLineError);

        TRY(opt2 = opt1);
        cmdline = "app --group1a abc --group2a def";
        TRY(opt2.parse(cmdline));
        TEST_EQUAL(opt2.get<u8string>("group1a"), "abc");
        TEST_EQUAL(opt2.get<u8string>("group2a"), "def");

    }

    void check_argument_patterns() {

        Options opt1("App");
        TRY(opt1.add("alpha", 'a', "Alpha", 0, "Hello", "[[:alpha:]]+"));
        TRY(opt1.add("number", 'n', "Number", 0, "42", "\\d+"));
        TEST_THROW(opt1.add("word", 'w', "Word", 0, "...", "\\w+"), OptionSpecError);

        Options opt2 = opt1;
        u8string cmdline;

        TRY(opt2 = opt1);
        cmdline = "app --alpha abc --number 123";
        TRY(opt2.parse(cmdline));
        TEST_EQUAL(opt2.get<u8string>("alpha"), "abc");
        TEST_EQUAL(opt2.get<int>("number"), 123);

        TRY(opt2 = opt1);
        cmdline = "app --alpha 123";
        TEST_THROW(opt2.parse(cmdline), CommandLineError);

        TRY(opt2 = opt1);
        cmdline = "app --number abc";
        TEST_THROW(opt2.parse(cmdline), CommandLineError);

        opt1 = Options("App");
        TRY(opt1.add("int", 'i', "Integer", opt_integer));
        TRY(opt1.add("float", 'f', "Float", opt_float));

        TRY(opt2 = opt1);
        cmdline = "app";
        TRY(opt2.parse(cmdline));
        TEST_EQUAL(opt2.get<int>("int"), 0);
        TEST_EQUAL(opt2.get<float>("float"), 0);

        TRY(opt2 = opt1);
        cmdline = "app --int 42 --float 1.234e5";
        TRY(opt2.parse(cmdline));
        TEST_EQUAL(opt2.get<int>("int"), 42);
        TEST_EQUAL(opt2.get<float>("float"), 123400);

        TRY(opt2 = opt1);
        cmdline = "app --int 1234.5";
        TEST_THROW(opt2.parse(cmdline), CommandLineError);

        TRY(opt2 = opt1);
        cmdline = "app --float 0x1234";
        TEST_THROW(opt2.parse(cmdline), CommandLineError);

        opt1 = Options("App");
        TRY(opt1.add("int", 'i', "Integer", opt_anon | opt_required));
        TRY(opt1.add("str", 's', "String", opt_required));

        TRY(opt2 = opt1);
        cmdline = "app -s hello 42";
        TRY(opt2.parse(cmdline));
        TEST_EQUAL(opt2.get<int>("int"), 42);
        TEST_EQUAL(opt2.get<u8string>("str"), "hello");

        TRY(opt2 = opt1);
        cmdline = "app 42 -s hello";
        TRY(opt2.parse(cmdline));
        TEST_EQUAL(opt2.get<int>("int"), 42);
        TEST_EQUAL(opt2.get<u8string>("str"), "hello");

    }

}

TEST_MODULE(unicorn, options) {

    Options opt1("App version 1.0", "Intro", "Outro");

    check_basic_options(opt1);
    check_switch_options(opt1);
    check_multiple_options(opt1);
    check_required_options(opt1);
    check_automatic_help(opt1);
    check_anonymous_arguments(opt1);
    check_help_text(opt1);
    check_group_options();
    check_argument_patterns();

}
