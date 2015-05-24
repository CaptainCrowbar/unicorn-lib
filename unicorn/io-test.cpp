#include "crow/core.hpp"
#include "crow/unit-test.hpp"
#include "unicorn/io.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

using namespace std::literals;
using namespace Unicorn;

namespace {

    class TempFile {
    public:
        explicit TempFile(const u8string& file): f(file) {}
        ~TempFile() { remove(f.c_str()); }
    private:
        u8string f;
        TempFile(const TempFile&) = delete;
        TempFile(TempFile&&) = delete;
        TempFile& operator=(const TempFile&) = delete;
        TempFile& operator=(TempFile&&) = delete;
    };

    const u8string testfile = "__test__";
    const u8string nonesuch = "__no_such_file__";

    void check_simple_file_io() {

        string s;
        TempFile tempfile(testfile);

        TRY(load_file("README.md"s, s));
        TEST_EQUAL(s.substr(0, 20), "# Unicorn Library #\n");
        TRY(save_file(testfile, "Hello world\n"s));
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\n");
        TRY(save_file(testfile, "Goodbye\n"s, io_append));
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");

    }

    void check_file_reader() {

        using u8vector = std::vector<u8string>;
        u8vector vec;
        Irange<FileReader> range;
        TempFile tempfile(testfile);

        TEST_THROW(range = read_lines(nonesuch), ReadError);
        TRY(range = read_lines(nonesuch, io_nofail));
        TEST_EQUAL(range_count(range), 0);

        TRY(save_file(testfile,
            "Last night I saw upon the stair\n"
            "A little man who wasn't there\n"
            "He wasn't there again today\n"
            "He must be from the NSA\n"
        ));
        TRY(range = read_lines(testfile));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 4);
        TEST_EQUAL_RANGE(vec, (u8vector{
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
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 4);
        TEST_EQUAL_RANGE(vec, (u8vector{
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
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 4);
        TEST_EQUAL_RANGE(vec, (u8vector{
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
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 3);
        TEST_EQUAL_RANGE(vec, (u8vector{
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
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 3);
        TEST_EQUAL_RANGE(vec, (u8vector{
            u8"Dollar\n",
            u8"\ufffduro\n",
            u8"Pound\n",
        }));

        TRY(save_file(testfile,
            "Dollar\n"
            "\x80uro\n"
            "Pound\n"
        ));
        TRY(range = read_lines(testfile, err_throw));
        TEST_THROW(std::copy(CROW_BOUNDS(range), overwrite(vec)), EncodingError);

        TRY(save_file(testfile, "Hello world\nGoodbye\n"));
        TRY(range = read_lines(testfile, io_bom));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));

        TRY(save_file(testfile, u8"\ufeffHello world\nGoodbye\n"));
        TRY(range = read_lines(testfile, io_bom));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));

        TRY(save_file(testfile, "Hello world\nGoodbye\n"));
        TRY(range = read_lines(testfile, io_lf));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));
        TRY(range = read_lines(testfile, io_crlf));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\r\n", "Goodbye\r\n"}));

        TRY(save_file(testfile, "Hello world\r\nGoodbye\r\n"));
        TRY(range = read_lines(testfile, io_lf));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));
        TRY(range = read_lines(testfile, io_crlf));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\r\n", "Goodbye\r\n"}));

        TRY(save_file(testfile, "Hello world\rGoodbye\r"));
        TRY(range = read_lines(testfile, io_lf));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));
        TRY(range = read_lines(testfile, io_crlf));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\r\n", "Goodbye\r\n"}));

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
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 8);
        TEST_EQUAL_RANGE(vec, (u8vector{
            "\n",
            "Hello\n",
            "    \n",
            "    North South    \n",
            "    East West    \n",
            "    \n",
            "Goodbye\n",
            "\n",
        }));
        TRY(range = read_lines(testfile, io_striplf));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 8);
        TEST_EQUAL_RANGE(vec, (u8vector{
            "",
            "Hello",
            "    ",
            "    North South    ",
            "    East West    ",
            "    ",
            "Goodbye",
            "",
        }));
        TRY(range = read_lines(testfile, io_striptws));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 8);
        TEST_EQUAL_RANGE(vec, (u8vector{
            "",
            "Hello",
            "",
            "    North South",
            "    East West",
            "",
            "Goodbye",
            "",
        }));
        TRY(range = read_lines(testfile, io_stripws));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 8);
        TEST_EQUAL_RANGE(vec, (u8vector{
            "",
            "Hello",
            "",
            "North South",
            "East West",
            "",
            "Goodbye",
            "",
        }));
        TRY(range = read_lines(testfile, io_notempty));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 6);
        TEST_EQUAL_RANGE(vec, (u8vector{
            "Hello\n",
            "    \n",
            "    North South    \n",
            "    East West    \n",
            "    \n",
            "Goodbye\n",
        }));
        TRY(range = read_lines(testfile, io_stripws | io_notempty));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 4);
        TEST_EQUAL_RANGE(vec, (u8vector{
            "Hello",
            "North South",
            "East West",
            "Goodbye",
        }));

        TRY(save_file(testfile, "Hello world!!Goodbye!!"));
        TRY(range = read_lines(testfile, {}, ""s, "!!"s));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world!!", "Goodbye!!"}));

        TRY(save_file(testfile, "Hello world!!Goodbye!!"));
        TRY(range = read_lines(testfile, io_striplf, ""s, "!!"s));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world", "Goodbye"}));

        TRY(save_file(testfile, "Hello world!!Goodbye!!"));
        TRY(range = read_lines(testfile, io_lf, ""s, "!!"s));
        TRY(std::copy(CROW_BOUNDS(range), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));

    }

    void check_file_writer() {

        string s;
        std::vector<u32string> vec32;
        FileWriter writer;
        TempFile tempfile(testfile);

        vec32 = {
            U"Last night I saw upon the stair\n",
            U"A little man who wasn't there\n",
            U"He wasn't there again today\n",
            U"He must be from the NSA\n",
        };
        TRY(writer = FileWriter(testfile));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s,
            "Last night I saw upon the stair\n"
            "A little man who wasn't there\n"
            "He wasn't there again today\n"
            "He must be from the NSA\n"
        );

        vec32 = {U"Hello world\r\n", U"Goodbye\r\n"};
        TRY(writer = FileWriter(testfile));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec32 = {U"Hello €urope\n", U"Goodbye\n"};
        TRY(writer = FileWriter(testfile));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, u8"Hello €urope\nGoodbye\n");

        // Error detection is not reliable on Windows
        #if defined(_XOPEN_SOURCE)
            vec32 = {U"Hello €urope\n", U"Goodbye\n"};
            TRY(writer = FileWriter(testfile, err_throw, "ascii"s));
            TEST_THROW(std::copy(CROW_BOUNDS(vec32), writer), EncodingError);
        #endif

        vec32 = {U"Hello world\n", U"Goodbye\n"};
        TRY(writer = FileWriter(testfile, io_bom));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, u8"\ufeffHello world\nGoodbye\n");

        vec32 = {U"\ufeffHello world\n", U"Goodbye\n"};
        TRY(writer = FileWriter(testfile, io_bom));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, u8"\ufeffHello world\nGoodbye\n");

        TRY(writer = FileWriter(testfile));
        TRY(*writer++ = U"Hello world\n");
        TRY(writer = FileWriter(testfile, io_append));
        TRY(*writer++ = U"Goodbye\n");
        TRY(writer = FileWriter());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");

        vec32 = {U"Hello world\n", U"Goodbye\n"};
        TRY(writer = FileWriter(testfile, io_lf));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");
        TRY(writer = FileWriter(testfile, io_crlf));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec32 = {U"Hello world\r\n", U"Goodbye\r\n"};
        TRY(writer = FileWriter(testfile, io_lf));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");
        TRY(writer = FileWriter(testfile, io_crlf));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec32 = {U"Hello world\r", U"Goodbye\r"};
        TRY(writer = FileWriter(testfile, io_lf));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");
        TRY(writer = FileWriter(testfile, io_crlf));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec32 = {U"Hello world", U"Goodbye"};
        TRY(writer = FileWriter(testfile, io_writeline));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");

        vec32 = {U"Hello world", U"Goodbye"};
        TRY(writer = FileWriter(testfile, io_crlf | io_writeline));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec32 = {U"North", U"South\n", U"East", U"West\r\n"};
        TRY(writer = FileWriter(testfile, io_autoline));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "North\nSouth\nEast\nWest\r\n");

        vec32 = {U"North", U"South\n", U"East", U"West\r\n"};
        TRY(writer = FileWriter(testfile, io_lf | io_autoline));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "North\nSouth\nEast\nWest\n");

        vec32 = {U"North", U"South\n", U"East", U"West\r\n"};
        TRY(writer = FileWriter(testfile, io_crlf | io_autoline));
        TRY(std::copy(CROW_BOUNDS(vec32), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "North\r\nSouth\r\nEast\r\nWest\r\n");

    }

}

TEST_MODULE(unicorn, io) {

    check_simple_file_io();
    check_file_reader();
    check_file_writer();

}
