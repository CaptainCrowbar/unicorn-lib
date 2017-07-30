#include "unicorn/core.hpp"
#include "unicorn/environment.hpp"
#include "rs-core/unit-test.hpp"
#include <vector>

using namespace RS;
using namespace RS::Unicorn;

namespace {

    void check_query_functions() {

        U8string s;

        TEST(has_env("PATH"));
        TRY(s = get_env("PATH"));
        TEST(! s.empty());
        #ifdef _XOPEN_SOURCE
            TEST_MATCH(s, "^(.+:)?/usr/bin(:.+)?$");
        #else
            TEST_MATCH(s, "^(.+;)C:\\\\Windows(;.+)?$");
        #endif

        TEST(! has_env("__NO_SUCH_THING__"));
        TRY(s = get_env("__NO_SUCH_THING__"));
        TEST(s.empty());

    }

    void check_update_functions() {

        U8string s;

        TEST(! has_env("UNICORN_TEST_ENV"));
        TRY(set_env("UNICORN_TEST_ENV", "Hello world"));
        TEST(has_env("UNICORN_TEST_ENV"));
        TRY(s = get_env("UNICORN_TEST_ENV"));
        TEST_EQUAL(s, "Hello world");
        TRY(unset_env("UNICORN_TEST_ENV"));
        TEST(! has_env("UNICORN_TEST_ENV"));

    }

    void check_environment_block() {

        Environment env;
        U8string s;

        TEST(env.empty());
        TEST_EQUAL(env.size(), 0);
        TEST(! env.has("PATH"));
        TRY(s = env["PATH"]);
        TEST_EQUAL(s, "");

        TRY(env.set("alpha", "xray"));
        TRY(env.set("bravo", "yankee"));
        TRY(env.set("charlie", "zulu"));
        TEST_EQUAL(env.size(), 3);

        TEST(env.has("alpha"));    TRY(s = env["alpha"]);    TEST_EQUAL(s, "xray");
        TEST(env.has("bravo"));    TRY(s = env["bravo"]);    TEST_EQUAL(s, "yankee");
        TEST(env.has("charlie"));  TRY(s = env["charlie"]);  TEST_EQUAL(s, "zulu");
        TEST(! env.has("delta"));  TRY(s = env["delta"]);    TEST_EQUAL(s, "");

        TRY(env.clear());
        TEST(env.empty());

        TRY(env.load());
        TEST(! env.empty());
        TEST_COMPARE(env.size(), >, 0);
        TEST(env.has("PATH"));
        TRY(s = env["PATH"]);
        #ifdef _XOPEN_SOURCE
            TEST_MATCH(s, "^(.+:)?/usr/bin(:.+)?$");
        #else
            TEST_MATCH(s, "^(.+;)C:\\\\Windows(;.+)?$");
        #endif

        Strings keys, values;
        for (auto& kv: env) {
            TRY(keys.push_back(to_utf8(kv.first, Utf::replace)));
            TRY(values.push_back(to_utf8(kv.second, Utf::replace)));
        }
        TEST_EQUAL(keys.size(), env.size());
        TEST_EQUAL(values.size(), env.size());

    }

    void check_string_expansion() {

        U8string s, t, columns, lines, home;

        TRY(columns = get_env("COLUMNS"));
        TRY(lines = get_env("LINES"));
        TRY(home = get_env("HOME"));

        s = "Hello world";
        TRY(t = expand_env(s));
        TEST_EQUAL(t, s);

        s = "Hello world $COLUMNS $LINES $HOME ...";
        TRY(t = expand_env(s, Environment::posix));
        TEST_EQUAL(t, "Hello world " + columns + " " + lines + " " + home + " ...");;

        s = "Hello world ${COLUMNS} ${LINES} ${HOME} ...";
        TRY(t = expand_env(s, Environment::posix));
        TEST_EQUAL(t, "Hello world " + columns + " " + lines + " " + home + " ...");;

        s = "Hello world %COLUMNS% %LINES% %HOME% ...";
        TRY(t = expand_env(s, Environment::windows));
        TEST_EQUAL(t, "Hello world " + columns + " " + lines + " " + home + " ...");;

        s = "Hello world $COLUMNS ${LINES} %HOME% ...";
        TRY(t = expand_env(s, Environment::posix | Environment::windows));
        TEST_EQUAL(t, "Hello world " + columns + " " + lines + " " + home + " ...");;

        s = "$$ %%";
        TRY(t = expand_env(s, Environment::posix | Environment::windows));
        TEST_EQUAL(t, "$ %");

        Environment env;

        TRY(env.set("alpha", "xray"));
        TRY(env.set("bravo", "yankee"));
        TRY(env.set("charlie", "zulu"));

        s = "Hello $alpha $zulu world";
        TRY(t = env.expand(s, Environment::posix));
        TEST_EQUAL(t, "Hello xray  world");

        s = "Hello ${alpha} ${zulu} world";
        TRY(t = env.expand(s, Environment::posix));
        TEST_EQUAL(t, "Hello xray  world");

        s = "Hello %alpha% %zulu% world";
        TRY(t = env.expand(s, Environment::windows));
        TEST_EQUAL(t, "Hello xray  world");

        s = "Hello $alpha %zulu% world";
        TRY(t = env.expand(s, Environment::posix | Environment::windows));
        TEST_EQUAL(t, "Hello xray  world");

    }

}

TEST_MODULE(unicorn, environment) {

    check_query_functions();
    check_update_functions();
    check_environment_block();
    check_string_expansion();

}
