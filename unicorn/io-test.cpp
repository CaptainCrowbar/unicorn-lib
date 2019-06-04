#include "unicorn/io.hpp"
#include "unicorn/path.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <string>
#include <system_error>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

void test_unicorn_io_file_reader() {

    Path testfile = "__test__";
    Path nonesuch = "__no_such_file__";

    auto guard = scope_exit([=] { testfile.remove(); });

    Ustring s;
    Strings vec;
    Irange<FileReader> range;

    TEST_THROW(range = read_lines(nonesuch), std::system_error);
    TRY(range = read_lines(nonesuch, IO::pretend));
    TEST_EQUAL(range_count(range), 0);

    TRY(testfile.save(
        "Last night I saw upon the stair\n"
        "A little man who wasn't there\n"
        "He wasn't there again today\n"
        "He must be from the NSA\n"
    ));
    TRY(range = read_lines(testfile));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 4);
    TEST_EQUAL_RANGE(vec, (Strings{
        "Last night I saw upon the stair\n",
        "A little man who wasn't there\n",
        "He wasn't there again today\n",
        "He must be from the NSA\n",
    }));

    TRY(testfile.save(
        "Last night I saw upon the stair\r\n"
        "A little man who wasn't there\r\n"
        "He wasn't there again today\r\n"
        "He must be from the NSA\r\n"
    ));
    TRY(range = read_lines(testfile));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 4);
    TEST_EQUAL_RANGE(vec, (Strings{
        "Last night I saw upon the stair\r\n",
        "A little man who wasn't there\r\n",
        "He wasn't there again today\r\n",
        "He must be from the NSA\r\n",
    }));

    TRY(testfile.save(
        "Last night I saw upon the stair\n"
        "A little man who wasn't there\n"
        "He wasn't there again today\n"
        "He must be from the NSA"
    ));
    TRY(range = read_lines(testfile));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 4);
    TEST_EQUAL_RANGE(vec, (Strings{
        "Last night I saw upon the stair\n",
        "A little man who wasn't there\n",
        "He wasn't there again today\n",
        "He must be from the NSA",
    }));

    TRY(testfile.save(
        "Dollar\n"
        "\x80uro\n"
        "Pound\n"
    ));
    TRY(range = read_lines(testfile, {}, "windows-1252"s));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 3);
    TEST_EQUAL_RANGE(vec, (Strings{
        "Dollar\n",
        "€uro\n",
        "Pound\n",
    }));

    TRY(testfile.save(
        "Dollar\n"
        "\x80uro\n"
        "Pound\n"
    ));
    TRY(range = read_lines(testfile));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 3);
    TEST_EQUAL_RANGE(vec, (Strings{
        "Dollar\n",
        "\ufffduro\n",
        "Pound\n",
    }));

    TRY(testfile.save(
        "Dollar\n"
        "\x80uro\n"
        "Pound\n"
    ));
    TRY(range = read_lines(testfile, Utf::throws));
    TEST_THROW(std::copy(range.begin(), range.end(), overwrite(vec)), EncodingError);

    TRY(testfile.save("Hello world\nGoodbye\n"));
    TRY(range = read_lines(testfile, IO::bom));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));

    TRY(testfile.save("\ufeffHello world\nGoodbye\n"));
    TRY(range = read_lines(testfile, IO::bom));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));

    TRY(testfile.save("Hello world\nGoodbye\n"));
    TRY(range = read_lines(testfile, IO::lf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));
    TRY(range = read_lines(testfile, IO::crlf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\r\n", "Goodbye\r\n"}));

    TRY(testfile.save("Hello world\r\nGoodbye\r\n"));
    TRY(range = read_lines(testfile, IO::lf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));
    TRY(range = read_lines(testfile, IO::crlf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\r\n", "Goodbye\r\n"}));

    TRY(testfile.save("Hello world\rGoodbye\r"));
    TRY(range = read_lines(testfile, IO::lf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));
    TRY(range = read_lines(testfile, IO::crlf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\r\n", "Goodbye\r\n"}));

    TRY(testfile.save(
        "\n"
        "Hello\n"
        "    \n"
        "    North South    \n"
        "    East West    \n"
        "    \n"
        "Goodbye\n"
        "\n"
    ));
    TRY(range = read_lines(testfile));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 8);
    TEST_EQUAL_RANGE(vec, (Strings{
        "\n",
        "Hello\n",
        "    \n",
        "    North South    \n",
        "    East West    \n",
        "    \n",
        "Goodbye\n",
        "\n",
    }));
    TRY(range = read_lines(testfile, IO::striplf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 8);
    TEST_EQUAL_RANGE(vec, (Strings{
        "",
        "Hello",
        "    ",
        "    North South    ",
        "    East West    ",
        "    ",
        "Goodbye",
        "",
    }));
    TRY(range = read_lines(testfile, IO::striptws));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 8);
    TEST_EQUAL_RANGE(vec, (Strings{
        "",
        "Hello",
        "",
        "    North South",
        "    East West",
        "",
        "Goodbye",
        "",
    }));
    TRY(range = read_lines(testfile, IO::stripws));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 8);
    TEST_EQUAL_RANGE(vec, (Strings{
        "",
        "Hello",
        "",
        "North South",
        "East West",
        "",
        "Goodbye",
        "",
    }));
    TRY(range = read_lines(testfile, IO::notempty));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 6);
    TEST_EQUAL_RANGE(vec, (Strings{
        "Hello\n",
        "    \n",
        "    North South    \n",
        "    East West    \n",
        "    \n",
        "Goodbye\n",
    }));
    TRY(range = read_lines(testfile, IO::stripws | IO::notempty));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 4);
    TEST_EQUAL_RANGE(vec, (Strings{
        "Hello",
        "North South",
        "East West",
        "Goodbye",
    }));
    TRY(range = read_lines(testfile));
    s.clear();
    for (auto fr = range.begin(); fr != range.end(); ++fr)
        TRY(s += std::to_string(fr.line()) + ":"+ *fr);
    TEST_EQUAL(s,
        "1:\n"
        "2:Hello\n"
        "3:    \n"
        "4:    North South    \n"
        "5:    East West    \n"
        "6:    \n"
        "7:Goodbye\n"
        "8:\n"
    );
    TRY(range = read_lines(testfile, IO::stripws | IO::notempty));
    s.clear();
    for (auto fr = range.begin(); fr != range.end(); ++fr)
        TRY(s += std::to_string(fr.line()) + ":"+ *fr + "\n");
    TEST_EQUAL(s,
        "2:Hello\n"
        "4:North South\n"
        "5:East West\n"
        "7:Goodbye\n"
    );

    TRY(testfile.save("Hello world!!Goodbye!!"));
    TRY(range = read_lines(testfile, {}, ""s, "!!"s));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world!!", "Goodbye!!"}));

    TRY(testfile.save("Hello world!!Goodbye!!"));
    TRY(range = read_lines(testfile, IO::striplf, ""s, "!!"s));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world", "Goodbye"}));

    TRY(testfile.save("Hello world!!Goodbye!!"));
    TRY(range = read_lines(testfile, IO::lf, ""s, "!!"s));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));

}

void test_unicorn_io_file_writer() {

    Path testfile = "__test__";

    auto guard = scope_exit([=] { testfile.remove(); });

    std::string s;
    Strings vec;
    FileWriter writer;

    vec = {
        "Last night I saw upon the stair\n",
        "A little man who wasn't there\n",
        "He wasn't there again today\n",
        "He must be from the NSA\n",
    };
    TRY(writer = FileWriter(testfile));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s,
        "Last night I saw upon the stair\n"
        "A little man who wasn't there\n"
        "He wasn't there again today\n"
        "He must be from the NSA\n"
    );

    vec = {"Hello world\r\n", "Goodbye\r\n"};
    TRY(writer = FileWriter(testfile));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {"Hello €urope\n", "Goodbye\n"};
    TRY(writer = FileWriter(testfile));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello €urope\nGoodbye\n");

    // Error detection is not reliable on Windows
    #ifdef _XOPEN_SOURCE
        vec = {"Hello €urope\n", "Goodbye\n"};
        TRY(writer = FileWriter(testfile, Utf::throws, "ascii"s));
        TEST_THROW(std::copy(vec.begin(), vec.end(), writer), EncodingError);
    #endif

    vec = {"Hello world\n", "Goodbye\n"};
    TRY(writer = FileWriter(testfile, IO::bom));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "\ufeffHello world\nGoodbye\n");

    vec = {"\ufeffHello world\n", "Goodbye\n"};
    TRY(writer = FileWriter(testfile, IO::bom));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "\ufeffHello world\nGoodbye\n");

    TRY(writer = FileWriter(testfile));
    TRY(*writer++ = "Hello world\n");
    TRY(writer = FileWriter(testfile, IO::append));
    TRY(*writer++ = "Goodbye\n");
    TRY(writer = FileWriter());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");

    TEST_THROW(FileWriter(testfile, IO::protect), std::system_error);

    vec = {"Hello world\n", "Goodbye\n"};
    TRY(writer = FileWriter(testfile, IO::lf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");
    TRY(writer = FileWriter(testfile, IO::crlf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {"Hello world\r\n", "Goodbye\r\n"};
    TRY(writer = FileWriter(testfile, IO::lf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");
    TRY(writer = FileWriter(testfile, IO::crlf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {"Hello world\r", "Goodbye\r"};
    TRY(writer = FileWriter(testfile, IO::lf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");
    TRY(writer = FileWriter(testfile, IO::crlf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {"Hello world", "Goodbye"};
    TRY(writer = FileWriter(testfile, IO::writeline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");

    vec = {"Hello world", "Goodbye"};
    TRY(writer = FileWriter(testfile, IO::crlf | IO::writeline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {"North", "South\n", "East", "West\r\n"};
    TRY(writer = FileWriter(testfile, IO::autoline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "North\nSouth\nEast\nWest\r\n");

    vec = {"North", "South\n", "East", "West\r\n"};
    TRY(writer = FileWriter(testfile, IO::lf | IO::autoline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "North\nSouth\nEast\nWest\n");

    vec = {"North", "South\n", "East", "West\r\n"};
    TRY(writer = FileWriter(testfile, IO::crlf | IO::autoline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(testfile.load(s));
    TEST_EQUAL(s, "North\r\nSouth\r\nEast\r\nWest\r\n");

}
