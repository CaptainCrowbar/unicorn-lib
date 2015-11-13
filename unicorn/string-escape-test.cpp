#include "unicorn/core.hpp"
#include "unicorn/string.hpp"
#include "unicorn/string-escape.hpp" // TODO
#include "prion/unit-test.hpp"
#include <string>

using namespace std::literals;
using namespace Unicorn;

namespace {

    void check_encode_uri() {

        string s1, s2;

        s1 = "";                         TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "");
        s1 = "hello world";              TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "hello%20world");
        s1 = "http://www.example.com/";  TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "http://www.example.com/");
        s1 = "\"%<>\\^`{|}";             TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "!#$&'()*+,/:;=?@[]");
        s1 = "-._~";                     TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "-._~");
        s1 = u8"αβγδε";                  TRY(s2 = str_encode_uri(s1));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        s1 = "";                         TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "");
        s1 = "hello world";              TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "hello%20world");
        s1 = "http://www.example.com/";  TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "http%3a%2f%2fwww.example.com%2f");
        s1 = "\"%<>\\^`{|}";             TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%22%25%3c%3e%5c%5e%60%7b%7c%7d");
        s1 = "!#$&'()*+,/:;=?@[]";       TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%21%23%24%26%27%28%29%2a%2b%2c%2f%3a%3b%3d%3f%40%5b%5d");
        s1 = "-._~";                     TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "-._~");
        s1 = u8"αβγδε";                  TRY(s2 = str_encode_uri_component(s1));  TEST_EQUAL(s2, "%ce%b1%ce%b2%ce%b3%ce%b4%ce%b5");

        // | Characters                             | Behaviour
        // | ----------                             | ---------
        // | ``" % < > \ ^ ` { | }``                | Encoded by both `str_encode_uri()` and `str_encode_uri_component()`
        // | `! # $ & ' ( ) * + , / : ; = ? @ [ ]`  | Encoded by `str_encode_uri_component()` but not by `str_encode_uri()`
        // | `- . _ ~`                              | Left unencoded by both functions

        // TODO

    }

    void check_escape() {

        // TODO

    }

    void check_quote() {

        // TODO

    }

}

TEST_MODULE(unicorn, string_escape) {

    check_encode_uri();
    check_escape();
    check_quote();

}
