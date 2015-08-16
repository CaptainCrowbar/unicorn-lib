#include "crow/unit-test.hpp"
#include "unicorn/core.hpp"
#include "unicorn/json.hpp"
#include <sstream>
#include <string>
#include <vector>

using namespace std::literals;
using namespace Crow;
using namespace Unicorn;

namespace {

    void check_json_construction() {

        Json::Element e;

        TEST(e.type() == Json::null);
        TEST_EQUAL(e.str(), "null");

        TRY(e = true);
        TEST_EQUAL(e.type(), Json::boolean);
        TEST_EQUAL(e.boolean(), true);
        TEST_EQUAL(e.str(), "true");

        TRY(e = 42);
        TEST_EQUAL(e.type(), Json::number);
        TEST_EQUAL(e.number(), 42);
        TEST_EQUAL(e.str(), "42");

        TRY(e = 1234.5678);
        TEST_EQUAL(e.type(), Json::number);
        TEST_NEAR(e.number(), 1234.5678);
        TEST_EQUAL(e.str(), "1234.5678");

        TRY(e = "Hello world"s);
        TEST_EQUAL(e.type(), Json::string);
        TEST_EQUAL(e.string(), "Hello world");
        TEST_EQUAL(e.str(), "\"Hello world\"");

        TRY(e = "Don't panic!");
        TEST_EQUAL(e.type(), Json::string);
        TEST_EQUAL(e.string(), "Don't panic!");
        TEST_EQUAL(e.str(), "\"Don't panic!\"");

        TRY(e = Json::Array{});
        TEST_EQUAL(e.type(), Json::array);
        TEST_EQUAL(e.array().size(), 0);
        TEST_EQUAL(e.str(), "[]");

        TRY((e = Json::Array{10, "Alpha"}));
        TEST_EQUAL(e.type(), Json::array);
        TEST_EQUAL(e.array().size(), 2);
        TEST_EQUAL(e.str(), "[10,\"Alpha\"]");

        TRY((e = Json::Element{20, "Bravo"}));
        TEST_EQUAL(e.type(), Json::array);
        TEST_EQUAL(e.array().size(), 2);
        TEST_EQUAL(e.str(), "[20,\"Bravo\"]");

        TRY((e = {30, "Charlie"}));
        TEST_EQUAL(e.type(), Json::array);
        TEST_EQUAL(e.array().size(), 2);
        TEST_EQUAL(e.str(), "[30,\"Charlie\"]");

        TRY(e = Json::Object{});
        TEST_EQUAL(e.type(), Json::object);
        TEST_EQUAL(e.object().size(), 0);
        TEST_EQUAL(e.str(), "{}");

        TRY((e = Json::Object{{"Alpha", 42}, {"Bravo", "Don't panic!"}}));
        TEST_EQUAL(e.type(), Json::object);
        TEST_EQUAL(e.object().size(), 2);
        TEST_EQUAL(e.str(), "{\"Alpha\":42,\"Bravo\":\"Don't panic!\"}");

        TRY((e = {true, 42, "Hello", Json::Object{{"A", 1}, {"B", 2}, {"C", 3}, {"Z", {4, 5, 6}}}, -1234.5}));
        TEST_EQUAL(e.type(), Json::array);
        TEST_EQUAL(e.array().size(), 5);
        TEST_EQUAL(e.str(), "[true,42,\"Hello\",{\"A\":1,\"B\":2,\"C\":3,\"Z\":[4,5,6]},-1234.5]");

    }

    void check_json_parsing() {

        Json::Element e;
        u8string s;
        size_t pos = 0;

        TEST_THROW(Json::Element::read(s, pos), Json::EndOfFile);

        s = "true;";
        pos = 0;
        TRY(e = Json::Element::read(s, pos));
        TEST_EQUAL(e, Json::Element{true});
        TEST_EQUAL(pos, 4);

        s = "42;";
        pos = 0;
        TRY(e = Json::Element::read(s, pos));
        TEST_EQUAL(e, Json::Element{42});
        TEST_EQUAL(pos, 2);

        s = "\"Hello world\";";
        pos = 0;
        TRY(e = Json::Element::read(s, pos));
        TEST_EQUAL(e, Json::Element{"Hello world"});
        TEST_EQUAL(pos, 13);

        s = "[];";
        pos = 0;
        TRY(e = Json::Element::read(s, pos));
        TEST_EQUAL(e, Json::Element{Json::Array{}});
        TEST_EQUAL(pos, 2);

        s = "{};";
        pos = 0;
        TRY(e = Json::Element::read(s, pos));
        TEST_EQUAL(e, Json::Element{Json::Object{}});
        TEST_EQUAL(pos, 2);

        s = "[ 123 , 456 , 789 ]";
        TRY(e = Json::Element::read(s));
        TEST_EQUAL(e, (Json::Element{Json::Array{123,456,789}}));

        s = "[ \"Alpha\" , \"Bravo\" , \"Charlie\" ]";
        TRY(e = Json::Element::read(s));
        TEST_EQUAL(e, (Json::Element{Json::Array{"Alpha","Bravo","Charlie"}}));

        s = "{ \"Alpha\" : 10 , \"Bravo\" : 20 , \"Charlie\" : 30 }";
        TRY(e = Json::Element::read(s));
        TEST_EQUAL(e, (Json::Element{Json::Object{{"Alpha",10},{"Bravo",20},{"Charlie",30}}}));

        s = "[true, 42, \"Hello\", {\"A\":1, \"B\":2, \"C\":3, \"Z\":[4,5,6]}, -1234.5]";
        TRY(e = Json::Element::read(s));
        TEST_EQUAL(e.type(), Json::array);
        TEST_EQUAL(e.array().size(), 5);
        TEST_EQUAL(e.array()[0].type(), Json::boolean);
        TEST_EQUAL(e.array()[0].boolean(), true);
        TEST_EQUAL(e.array()[1].type(), Json::number);
        TEST_EQUAL(e.array()[1].number(), 42);
        TEST_EQUAL(e.array()[2].type(), Json::string);
        TEST_EQUAL(e.array()[2].string(), "Hello");
        TEST_EQUAL(e.array()[3].type(), Json::object);
        TEST_EQUAL(e.array()[3].object().size(), 4);
        TEST_EQUAL(e.array()[3].object()["Z"].type(), Json::array);
        TEST_EQUAL(e.array()[3].object()["Z"].array().size(), 3);
        TEST_EQUAL(e.array()[3].object()["Z"].array()[0], 4);
        TEST_EQUAL(e.array()[3].object()["Z"].array()[1], 5);
        TEST_EQUAL(e.array()[3].object()["Z"].array()[2], 6);
        TEST_EQUAL(e.array()[3].object()["A"].type(), Json::number);
        TEST_EQUAL(e.array()[3].object()["A"].number(), 1);
        TEST_EQUAL(e.array()[3].object()["B"].type(), Json::number);
        TEST_EQUAL(e.array()[3].object()["B"].number(), 2);
        TEST_EQUAL(e.array()[3].object()["C"].type(), Json::number);
        TEST_EQUAL(e.array()[3].object()["C"].number(), 3);
        TEST_EQUAL(e.array()[4].type(), Json::number);
        TEST_EQUAL(e.array()[4].number(), -1234.5);

    }

    void check_json_strings() {

        Json::Element e;
        u8string s;

        TRY(e = Json::Element::read("[\"\"]"));
        TEST_EQUAL(e.array()[0].string(), "");
        TEST_EQUAL(e.str(), "[\"\"]");

        TRY(e = Json::Element::read("[\"\\\"Hello\\\"\"]"));
        TEST_EQUAL(e.array()[0].string(), "\"Hello\"");
        TEST_EQUAL(e.str(), "[\"\\\"Hello\\\"\"]");

        TRY(e = Json::Element::read("[\"\\\\Hello\\\\\"]"));
        TEST_EQUAL(e.array()[0].string(), "\\Hello\\");
        TEST_EQUAL(e.str(), "[\"\\\\Hello\\\\\"]");

        TRY(e = Json::Element::read("[\"\\/Hello\\/\"]"));
        TEST_EQUAL(e.array()[0].string(), "/Hello/");
        TEST_EQUAL(e.str(), "[\"/Hello/\"]");

        TRY(e = Json::Element::read("[\"\x01\x1f\"]"));
        TEST_EQUAL(e.array()[0].string(), "\x01\x1f");
        TEST_EQUAL(e.str(), "[\"\\u0001\\u001f\"]");

        TRY(e = Json::Element::read("[\"\\u0001\\u001f\"]"));
        TEST_EQUAL(e.array()[0].string(), "\x01\x1f");
        TEST_EQUAL(e.str(), "[\"\\u0001\\u001f\"]");

        TRY(e = Json::Element::read("[\"€\"]"));
        TEST_EQUAL(e.array()[0].string(), "€");
        TEST_EQUAL(e.str(), "[\"€\"]");

        TRY(e = Json::Element::read("[\"\\u20ac\"]"));
        TEST_EQUAL(e.array()[0].string(), "€");
        TEST_EQUAL(e.str(), "[\"€\"]");

        TRY(e = Json::Element::read("[\"\U00010000\"]"));
        TEST_EQUAL(e.array()[0].string(), "\U00010000");
        TEST_EQUAL(e.str(), "[\"\U00010000\"]");

        TRY(e = Json::Element::read("[\"\\ud800\\udc00\"]"));
        TEST_EQUAL(e.array()[0].string(), "\U00010000");
        TEST_EQUAL(e.str(), "[\"\U00010000\"]");

    }

    void check_json_indexing() {

        Json::Element e;
        const auto& ce(e);
        u8string s;

        s = "[true, 42, \"Hello\", {\"A\":1, \"B\":2, \"C\":3, \"Z\":[4,5,6]}, -1234.5]";
        TRY(e = Json::Element::read(s));

        TEST_EQUAL(ce[0], true);
        TEST_EQUAL(ce[1], 42);
        TEST_EQUAL(ce[2], "Hello");
        TEST_EQUAL(ce[3]["A"], 1);
        TEST_EQUAL(ce[3]["B"], 2);
        TEST_EQUAL(ce[3]["C"], 3);
        TEST_EQUAL(ce[3]["Z"][0], 4);
        TEST_EQUAL(ce[3]["Z"][1], 5);
        TEST_EQUAL(ce[3]["Z"][2], 6);
        TEST_EQUAL(ce[4], -1234.5);

        TRY(e[0] = "Don't panic!");
        TEST_EQUAL(ce[0], "Don't panic!");
        TRY(e[3]["A"] = 123);
        TEST_EQUAL(ce[3]["A"], 123);
        TRY(e[3]["Z"][5] = "Goodbye");
        TEST_EQUAL(ce[3]["Z"][5], "Goodbye");

        TRY(s = ce.str());
        TEST_EQUAL(s,
            "[\"Don't panic!\",42,\"Hello\",{\"A\":123,\"B\":2,\"C\":3,\"Z\":[4,5,6,null,null,\"Goodbye\"]},-1234.5]");

    }

    u8string layout_str(const Json::Element& e, size_t max_array = 0) {
        std::ostringstream out;
        TRY(e.layout(out, max_array));
        return out.str();
    }

    void check_json_layout() {

        Json::Element e;

        TEST_EQUAL(layout_str(e),
            "null\n");

        TRY(e = true);
        TEST_EQUAL(layout_str(e),
            "true\n");

        TRY(e = 1234.5678);
        TEST_EQUAL(layout_str(e),
            "1234.5678\n");

        TRY(e = "Hello world");
        TEST_EQUAL(layout_str(e),
            "\"Hello world\"\n");

        TRY(e = Json::Array{});
        TEST_EQUAL(layout_str(e),
            "[]\n");

        TRY((e = {10, "Alpha"}));
        TEST_EQUAL(layout_str(e),
            "[\n"
            "    10,\n"
            "    \"Alpha\"\n"
            "]\n");
        TEST_EQUAL(layout_str(e, 10),
            "[10,\"Alpha\"]\n");

        TRY(e = Json::Object{});
        TEST_EQUAL(layout_str(e),
            "{}\n");

        TRY((e = Json::Object{{"Alpha", 42}, {"Bravo", "Don't panic!"}}));
        TEST_EQUAL(layout_str(e),
            "{\n"
            "    \"Alpha\": 42,\n"
            "    \"Bravo\": \"Don't panic!\"\n"
            "}\n");
        TEST_EQUAL(layout_str(e, 10),
            "{\n"
            "    \"Alpha\": 42,\n"
            "    \"Bravo\": \"Don't panic!\"\n"
            "}\n");

        TRY((e = {true, 42, "Hello", Json::Object{{"A", 1}, {"B", 2}, {"C", 3}, {"Z", {4, 5, 6}}}, -1234.5}));
        TEST_EQUAL(layout_str(e),
            "[\n"
            "    true,\n"
            "    42,\n"
            "    \"Hello\",\n"
            "    {\n"
            "        \"A\": 1,\n"
            "        \"B\": 2,\n"
            "        \"C\": 3,\n"
            "        \"Z\": [\n"
            "            4,\n"
            "            5,\n"
            "            6\n"
            "        ]\n"
            "    },\n"
            "    -1234.5\n"
            "]\n");
        TEST_EQUAL(layout_str(e, 10),
            "[\n"
            "    true,\n"
            "    42,\n"
            "    \"Hello\",\n"
            "    {\n"
            "        \"A\": 1,\n"
            "        \"B\": 2,\n"
            "        \"C\": 3,\n"
            "        \"Z\": [4,5,6]\n"
            "    },\n"
            "    -1234.5\n"
            "]\n");

    }

}

TEST_MODULE(unicorn, json) {

    check_json_construction();
    check_json_parsing();
    check_json_strings();
    check_json_indexing();
    check_json_layout();

}
