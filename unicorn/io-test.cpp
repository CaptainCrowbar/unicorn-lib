#include "unicorn/core.hpp"
#include "unicorn/io.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <algorithm>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>

using namespace std::literals;
using namespace Unicorn;

namespace {

    class TempFile {
    public:
        PRI_NO_COPY_MOVE(TempFile)
        explicit TempFile(const U8string& file): f(file) {}
        ~TempFile() { remove(f.data()); }
    private:
        U8string f;
    };

    const U8string testfile = "__test__";
    const U8string nonesuch = "__no_such_file__";

    void check_file_reader() {

        using u8vector = std::vector<U8string>;
        U8string s;
        u8vector vec;
        Irange<FileReader> range;
        TempFile tempfile(testfile);

        TEST_THROW(range = read_lines(nonesuch), std::system_error);
        TRY(range = read_lines(nonesuch, io_pretend));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TEST_THROW(std::copy(range.begin(), range.end(), overwrite(vec)), EncodingError);

        TRY(save_file(testfile, "Hello world\nGoodbye\n"));
        TRY(range = read_lines(testfile, io_bom));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));

        TRY(save_file(testfile, u8"\ufeffHello world\nGoodbye\n"));
        TRY(range = read_lines(testfile, io_bom));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));

        TRY(save_file(testfile, "Hello world\nGoodbye\n"));
        TRY(range = read_lines(testfile, io_lf));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));
        TRY(range = read_lines(testfile, io_crlf));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\r\n", "Goodbye\r\n"}));

        TRY(save_file(testfile, "Hello world\r\nGoodbye\r\n"));
        TRY(range = read_lines(testfile, io_lf));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));
        TRY(range = read_lines(testfile, io_crlf));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\r\n", "Goodbye\r\n"}));

        TRY(save_file(testfile, "Hello world\rGoodbye\r"));
        TRY(range = read_lines(testfile, io_lf));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));
        TRY(range = read_lines(testfile, io_crlf));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
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
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 4);
        TEST_EQUAL_RANGE(vec, (u8vector{
            "Hello",
            "North South",
            "East West",
            "Goodbye",
        }));
        TRY(range = read_lines(testfile));
        s.clear();
        for (auto fr = range.begin(); fr != range.end(); ++fr)
            TRY(s += dec(fr.line()) + ":"+ *fr);
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
        TRY(range = read_lines(testfile, io_stripws | io_notempty));
        s.clear();
        for (auto fr = range.begin(); fr != range.end(); ++fr)
            TRY(s += dec(fr.line()) + ":"+ *fr + "\n");
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
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world!!", "Goodbye!!"}));

        TRY(save_file(testfile, "Hello world!!Goodbye!!"));
        TRY(range = read_lines(testfile, io_striplf, ""s, "!!"s));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world", "Goodbye"}));

        TRY(save_file(testfile, "Hello world!!Goodbye!!"));
        TRY(range = read_lines(testfile, io_lf, ""s, "!!"s));
        TRY(std::copy(range.begin(), range.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        TEST_EQUAL_RANGE(vec, (u8vector{"Hello world\n", "Goodbye\n"}));

    }

    void check_file_writer() {

        std::string s;
        std::vector<U8string> vec;
        FileWriter writer;
        TempFile tempfile(testfile);

        vec = {
            "Last night I saw upon the stair\n",
            "A little man who wasn't there\n",
            "He wasn't there again today\n",
            "He must be from the NSA\n",
        };
        TRY(writer = FileWriter(testfile));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
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
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec = {u8"Hello €urope\n", "Goodbye\n"};
        TRY(writer = FileWriter(testfile));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, u8"Hello €urope\nGoodbye\n");

        // Error detection is not reliable on Windows
        #ifdef _XOPEN_SOURCE
            vec = {u8"Hello €urope\n", "Goodbye\n"};
            TRY(writer = FileWriter(testfile, err_throw, "ascii"s));
            TEST_THROW(std::copy(vec.begin(), vec.end(), writer), EncodingError);
        #endif

        vec = {"Hello world\n", "Goodbye\n"};
        TRY(writer = FileWriter(testfile, io_bom));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, u8"\ufeffHello world\nGoodbye\n");

        vec = {u8"\ufeffHello world\n", "Goodbye\n"};
        TRY(writer = FileWriter(testfile, io_bom));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, u8"\ufeffHello world\nGoodbye\n");

        TRY(writer = FileWriter(testfile));
        TRY(*writer++ = "Hello world\n");
        TRY(writer = FileWriter(testfile, io_append));
        TRY(*writer++ = "Goodbye\n");
        TRY(writer = FileWriter());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");

        TEST_THROW(FileWriter(testfile, io_protect), std::system_error);

        vec = {"Hello world\n", "Goodbye\n"};
        TRY(writer = FileWriter(testfile, io_lf));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");
        TRY(writer = FileWriter(testfile, io_crlf));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec = {"Hello world\r\n", "Goodbye\r\n"};
        TRY(writer = FileWriter(testfile, io_lf));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");
        TRY(writer = FileWriter(testfile, io_crlf));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec = {"Hello world\r", "Goodbye\r"};
        TRY(writer = FileWriter(testfile, io_lf));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");
        TRY(writer = FileWriter(testfile, io_crlf));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec = {"Hello world", "Goodbye"};
        TRY(writer = FileWriter(testfile, io_writeline));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\nGoodbye\n");

        vec = {"Hello world", "Goodbye"};
        TRY(writer = FileWriter(testfile, io_crlf | io_writeline));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "Hello world\r\nGoodbye\r\n");

        vec = {"North", "South\n", "East", "West\r\n"};
        TRY(writer = FileWriter(testfile, io_autoline));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "North\nSouth\nEast\nWest\r\n");

        vec = {"North", "South\n", "East", "West\r\n"};
        TRY(writer = FileWriter(testfile, io_lf | io_autoline));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "North\nSouth\nEast\nWest\n");

        vec = {"North", "South\n", "East", "West\r\n"};
        TRY(writer = FileWriter(testfile, io_crlf | io_autoline));
        TRY(std::copy(vec.begin(), vec.end(), writer));
        TRY(writer.flush());
        TRY(load_file(testfile, s));
        TEST_EQUAL(s, "North\r\nSouth\r\nEast\r\nWest\r\n");

    }

}

TEST_MODULE(unicorn, io) {

    check_file_reader();
    check_file_writer();

}
