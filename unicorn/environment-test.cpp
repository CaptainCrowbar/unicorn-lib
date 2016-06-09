#include "unicorn/core.hpp"
#include "unicorn/environment.hpp"
#include "prion/unit-test.hpp"
#include <string>
#include <vector>

using namespace Unicorn;

namespace {

    void check_query_functions() {

        u8string s;

        TEST(has_env("PATH"));
        TRY(s = get_env("PATH"));
        TEST(! s.empty());
        #if defined(PRI_TARGET_UNIX)
            TEST_MATCH(s, "^(.+:)?/usr/bin(:.+)?$");
        #else
            TEST_MATCH(s, "^(.+;)C:\\\\Windows(;.+)?$");
        #endif

        TEST(! has_env("__NO_SUCH_THING__"));
        TRY(s = get_env("__NO_SUCH_THING__"));
        TEST(s.empty());

    }

    void check_update_functions() {

        u8string s;

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
        u8string s;

        TEST(env.empty());
        TEST_EQUAL(env.size(), 0);
        TEST(! env.has("PATH"));
        TRY(s = env["PATH"]);
        TEST_EQUAL(s, "");

        TRY(env.load());
        TEST(! env.empty());
        TEST_COMPARE(env.size(), >, 0);
        TEST(env.has("PATH"));
        TRY(s = env["PATH"]);
        #if defined(PRI_TARGET_UNIX)
            TEST_MATCH(s, "^(.+:)?/usr/bin(:.+)?$");
        #else
            TEST_MATCH(s, "^(.+;)C:\\\\Windows(;.+)?$");
        #endif

        vector<u8string> keys, values;
        for (auto& kv: env) {
            TRY(keys.push_back(to_utf8(kv.first, err_replace)));
            TRY(values.push_back(to_utf8(kv.second, err_replace)));
        }
        TEST_EQUAL(keys.size(), env.size());
        TEST_EQUAL(values.size(), env.size());

    }

}

TEST_MODULE(unicorn, environment) {

    check_query_functions();
    check_update_functions();
    check_environment_block();

}
