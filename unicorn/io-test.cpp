#include "unicorn/io.hpp"
#include "unicorn/path.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

namespace {

    const Ustring testfile = "__test__";
    const Ustring nonesuch = "__no_such_file__";

    std::string load_file(const Ustring& file) {
        static constexpr size_t block = 1024;
        std::string buf;
        std::ifstream in(file, std::ios::binary);
        while (in) {
            size_t ofs = buf.size();
            buf.resize(ofs + block);
            in.read(buf.data() + ofs, block);
            buf.resize(ofs + in.gcount());
        }
        return buf;
    }

    void save_file(const Ustring& file, const std::string& text) {
        std::ofstream out(file, std::ios::binary);
        out.write(text.data(), text.size());
    }

}

void test_unicorn_io_file_reader() {

    Ustring s;
    Strings vec;
    Irange<FileReader> range;
    auto guard = scope_exit([=] { std::remove(testfile.data()); });

    TEST_THROW(range = read_lines(nonesuch), std::system_error);
    TRY(range = read_lines(nonesuch, IO::pretend));
    TEST_EQUAL(range_count(range), 0);

    TRY(save_file(testfile,
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

    TRY(save_file(testfile,
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

    TRY(save_file(testfile,
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

    TRY(save_file(testfile,
        "Dollar\n"
        "\x80uro\n"
        "Pound\n"
    ));
    TRY(range = read_lines(testfile, {}, "windows-1252"s));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 3);
    TEST_EQUAL_RANGE(vec, (Strings{
        u8"Dollar\n",
        u8"€uro\n",
        u8"Pound\n",
    }));

    TRY(save_file(testfile,
        "Dollar\n"
        "\x80uro\n"
        "Pound\n"
    ));
    TRY(range = read_lines(testfile));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 3);
    TEST_EQUAL_RANGE(vec, (Strings{
        u8"Dollar\n",
        u8"\ufffduro\n",
        u8"Pound\n",
    }));

    TRY(save_file(testfile,
        "Dollar\n"
        "\x80uro\n"
        "Pound\n"
    ));
    TRY(range = read_lines(testfile, Utf::throws));
    TEST_THROW(std::copy(range.begin(), range.end(), overwrite(vec)), EncodingError);

    TRY(save_file(testfile, "Hello world\nGoodbye\n"));
    TRY(range = read_lines(testfile, IO::bom));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));

    TRY(save_file(testfile, u8"\ufeffHello world\nGoodbye\n"));
    TRY(range = read_lines(testfile, IO::bom));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));

    TRY(save_file(testfile, "Hello world\nGoodbye\n"));
    TRY(range = read_lines(testfile, IO::lf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));
    TRY(range = read_lines(testfile, IO::crlf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\r\n", "Goodbye\r\n"}));

    TRY(save_file(testfile, "Hello world\r\nGoodbye\r\n"));
    TRY(range = read_lines(testfile, IO::lf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));
    TRY(range = read_lines(testfile, IO::crlf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\r\n", "Goodbye\r\n"}));

    TRY(save_file(testfile, "Hello world\rGoodbye\r"));
    TRY(range = read_lines(testfile, IO::lf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));
    TRY(range = read_lines(testfile, IO::crlf));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\r\n", "Goodbye\r\n"}));

    TRY(save_file(testfile,
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

    TRY(save_file(testfile, "Hello world!!Goodbye!!"));
    TRY(range = read_lines(testfile, {}, ""s, "!!"s));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world!!", "Goodbye!!"}));

    TRY(save_file(testfile, "Hello world!!Goodbye!!"));
    TRY(range = read_lines(testfile, IO::striplf, ""s, "!!"s));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world", "Goodbye"}));

    TRY(save_file(testfile, "Hello world!!Goodbye!!"));
    TRY(range = read_lines(testfile, IO::lf, ""s, "!!"s));
    TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL_RANGE(vec, (Strings{"Hello world\n", "Goodbye\n"}));

}

void test_unicorn_io_file_writer() {

    std::string s;
    Strings vec;
    FileWriter writer;
    auto guard = scope_exit([=] { std::remove(testfile.data()); });

    vec = {
        "Last night I saw upon the stair\n",
        "A little man who wasn't there\n",
        "He wasn't there again today\n",
        "He must be from the NSA\n",
    };
    TRY(writer = FileWriter(testfile));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
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
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {u8"Hello €urope\n", "Goodbye\n"};
    TRY(writer = FileWriter(testfile));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, u8"Hello €urope\nGoodbye\n");

    // Error detection is not reliable on Windows
    #ifdef _XOPEN_SOURCE
        vec = {u8"Hello €urope\n", "Goodbye\n"};
        TRY(writer = FileWriter(testfile, Utf::throws, "ascii"s));
        TEST_THROW(std::copy(vec.begin(), vec.end(), writer), EncodingError);
    #endif

    vec = {"Hello world\n", "Goodbye\n"};
    TRY(writer = FileWriter(testfile, IO::bom));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, u8"\ufeffHello world\nGoodbye\n");

    vec = {u8"\ufeffHello world\n", "Goodbye\n"};
    TRY(writer = FileWriter(testfile, IO::bom));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, u8"\ufeffHello world\nGoodbye\n");

    TRY(writer = FileWriter(testfile));
    TRY(*writer++ = "Hello world\n");
    TRY(writer = FileWriter(testfile, IO::append));
    TRY(*writer++ = "Goodbye\n");
    TRY(writer = FileWriter());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");

    TEST_THROW(FileWriter(testfile, IO::protect), std::system_error);

    vec = {"Hello world\n", "Goodbye\n"};
    TRY(writer = FileWriter(testfile, IO::lf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");
    TRY(writer = FileWriter(testfile, IO::crlf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {"Hello world\r\n", "Goodbye\r\n"};
    TRY(writer = FileWriter(testfile, IO::lf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");
    TRY(writer = FileWriter(testfile, IO::crlf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {"Hello world\r", "Goodbye\r"};
    TRY(writer = FileWriter(testfile, IO::lf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");
    TRY(writer = FileWriter(testfile, IO::crlf));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {"Hello world", "Goodbye"};
    TRY(writer = FileWriter(testfile, IO::writeline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");

    vec = {"Hello world", "Goodbye"};
    TRY(writer = FileWriter(testfile, IO::crlf | IO::writeline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

    vec = {"North", "South\n", "East", "West\r\n"};
    TRY(writer = FileWriter(testfile, IO::autoline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "North\nSouth\nEast\nWest\r\n");

    vec = {"North", "South\n", "East", "West\r\n"};
    TRY(writer = FileWriter(testfile, IO::lf | IO::autoline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "North\nSouth\nEast\nWest\n");

    vec = {"North", "South\n", "East", "West\r\n"};
    TRY(writer = FileWriter(testfile, IO::crlf | IO::autoline));
    TRY(std::copy(vec.begin(), vec.end(), writer));
    TRY(writer.flush());
    TRY(s = load_file(testfile));
    TEST_EQUAL(s, "North\r\nSouth\r\nEast\r\nWest\r\n");

}
