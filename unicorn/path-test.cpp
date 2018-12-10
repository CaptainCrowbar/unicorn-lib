#include "unicorn/path.hpp"
#include "unicorn/unit-test.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <chrono>
#include <ios>
#include <iterator>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::chrono;
using namespace std::literals;

namespace {

    template <typename T, typename Range>
    size_t index_of(const T& t, const Range& range) {
        using std::begin;
        using std::end;
        auto it = std::find(begin(range), end(range), t);
        return it == end(range) ? npos : it - begin(range);
    }

}

void test_unicorn_path_legal_names() {

    TEST(Path().is_legal());
    TEST(Path("foo").is_legal());
    TEST(Path("foo/bar").is_legal());
    TEST(! Path("foo\0bar"s).is_legal());

    TRY(Path("", Path::legal_name));
    TRY(Path("foo", Path::legal_name));
    TRY(Path("foo/bar", Path::legal_name));
    TEST_THROW(Path("foo\0bar"s, Path::legal_name), std::invalid_argument);

    #ifdef _XOPEN_SOURCE

        TEST(Path("\x80\xff").is_legal());
        TEST(Path("/").is_legal());
        TEST(Path("/foo").is_legal());
        TEST(Path("/foo/bar").is_legal());
        TEST(Path("//foo/bar").is_legal());
        TEST(Path("//foo/\x80\xff").is_legal());

    #else

        TEST(Path("C:/").is_legal());
        TEST(Path("C:/foo").is_legal());
        TEST(Path("C:/foo/bar").is_legal());
        TEST(Path("C:foo/bar").is_legal());
        TEST(Path("/foo/bar").is_legal());
        TEST(Path("//foo/bar").is_legal());
        TEST(! Path("<foo>").is_legal());
        TEST(! Path("@:/").is_legal());
        TEST(! Path("C:/<foo>").is_legal());

    #endif

}

void test_unicorn_path_unicode_names() {

    Path file;

    TRY(file = u8"αβγδε.txt");  TEST_EQUAL(file.name(), u8"αβγδε.txt");
    TRY(file = u"αβγδε.txt");   TEST_EQUAL(file.name(), u8"αβγδε.txt");
    TRY(file = U"αβγδε.txt");   TEST_EQUAL(file.name(), u8"αβγδε.txt");
    TRY(file = L"αβγδε.txt");   TEST_EQUAL(file.name(), u8"αβγδε.txt");

    #ifdef _XOPEN_SOURCE

        TRY(file = "/foo/bar\x80\xff");
        TEST_EQUAL(file.os_name(), "/foo/bar\x80\xff");
        TEST_EQUAL(file.name(), u8"/foo/bar\ufffd\ufffd");
        TEST_EQUAL(file.name(Utf::ignore), "/foo/bar\x80\xff");
        TEST_EQUAL(file.name(Utf::replace), u8"/foo/bar\ufffd\ufffd");
        TEST_THROW(file.name(Utf::throws), EncodingError);
        TEST_EQUAL(file.as_url(Utf::ignore), "file:///foo/bar\x80\xff");
        TEST_EQUAL(file.as_url(Utf::replace), u8"file:///foo/bar\ufffd\ufffd");
        TEST_THROW(file.as_url(Utf::throws), EncodingError);
        TRY(file = Path("/foo/bar\x80\xff", Utf::ignore));
        TEST_EQUAL(file.os_name(), "/foo/bar\x80\xff");
        TRY(file = Path("/foo/bar\x80\xff", Utf::replace));
        TEST_EQUAL(file.os_name(), u8"/foo/bar\ufffd\ufffd");
        TEST_THROW(file = Path("/foo/bar\x80\xff", Utf::throws), EncodingError);

    #else

        std::wstring crap = {0xdc00, 0xd800};

        TRY(file = L"C:/foo/bar" + crap);
        TEST_EQUAL(file.os_name(), L"C:\\foo\\bar" + crap);
        TEST_EQUAL(file.name(), u8"C:\\foo\\bar\ufffd\ufffd");
        TEST_EQUAL(file.name(Utf::replace), u8"C:\\foo\\bar\ufffd\ufffd");
        TEST_THROW(file.name(Utf::throws), EncodingError);
        TEST_EQUAL(file.as_url(Utf::replace), u8"file:///C:/foo/bar\ufffd\ufffd");
        TEST_THROW(file.as_url(Utf::throws), EncodingError);
        TRY(file = Path(L"C:/foo/bar" + crap, Utf::ignore));
        TEST_EQUAL(file.os_name(), L"C:\\foo\\bar" + crap);
        TRY(file = Path(L"C:/foo/bar" + crap, Utf::replace));
        TEST_EQUAL(file.os_name(), L"C:\\foo\\bar\ufffd\ufffd");
        TEST_THROW(file = Path(L"C:/foo/bar" + crap, Utf::throws), EncodingError);

    #endif

}

void test_unicorn_path_name_normalization() {

    Path file;

    #ifdef _XOPEN_SOURCE

        #ifdef __CYGWIN__

            TRY(file = "hello/world");      TEST_EQUAL(file.name(), "hello/world");
            TRY(file = "/hello/world");     TEST_EQUAL(file.name(), "/hello/world");
            TRY(file = "//hello/world");    TEST_EQUAL(file.name(), "//hello/world");
            TRY(file = "///hello/world");   TEST_EQUAL(file.name(), "/hello/world");
            TRY(file = "////hello/world");  TEST_EQUAL(file.name(), "/hello/world");

        #else

            TRY(file = "hello/world");      TEST_EQUAL(file.name(), "hello/world");
            TRY(file = "/hello/world");     TEST_EQUAL(file.name(), "/hello/world");
            TRY(file = "//hello/world");    TEST_EQUAL(file.name(), "/hello/world");
            TRY(file = "///hello/world");   TEST_EQUAL(file.name(), "/hello/world");
            TRY(file = "////hello/world");  TEST_EQUAL(file.name(), "/hello/world");

        #endif

        TRY(file = "hello/./world");  TEST_EQUAL(file.name(), "hello/world");
        TRY(file = "hello/world/");   TEST_EQUAL(file.name(), "hello/world");
        TRY(file = "hello/world/.");  TEST_EQUAL(file.name(), "hello/world");
        TRY(file = "hello//world");   TEST_EQUAL(file.name(), "hello/world");

    #else

        TRY(file = "hello/world");      TEST_EQUAL(file.name(), "hello\\world");
        TRY(file = "/hello/world");     TEST_EQUAL(file.name(), "\\hello\\world");
        TRY(file = "//hello/world");    TEST_EQUAL(file.name(), "\\\\hello\\world");
        TRY(file = "///hello/world");   TEST_EQUAL(file.name(), "\\\\hello\\world");
        TRY(file = "////hello/world");  TEST_EQUAL(file.name(), "\\\\hello\\world");
        TRY(file = "hello/./world");    TEST_EQUAL(file.name(), "hello\\world");
        TRY(file = "hello/world/");     TEST_EQUAL(file.name(), "hello\\world");
        TRY(file = "hello/world/.");    TEST_EQUAL(file.name(), "hello\\world");
        TRY(file = "hello//world");     TEST_EQUAL(file.name(), "hello\\world");
        TRY(file = "//hello");          TEST_EQUAL(file.name(), "\\\\hello\\");
        TRY(file = "C:/hello/world");   TEST_EQUAL(file.name(), "C:\\hello\\world");
        TRY(file = "c:/hello/world");   TEST_EQUAL(file.name(), "C:\\hello\\world");

    #endif

}

void test_unicorn_path_name_properties() {

    Path file;

    #ifdef _XOPEN_SOURCE

        TRY(file = "");           TEST(! file.is_absolute());  TEST(! file.is_relative());  TEST(! file.is_root());
        TRY(file = "/");          TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(file.is_root());
        TRY(file = "foo");        TEST(! file.is_absolute());  TEST(file.is_relative());    TEST(! file.is_root());
        TRY(file = "/foo");       TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_root());
        TRY(file = "foo/bar");    TEST(! file.is_absolute());  TEST(file.is_relative());    TEST(! file.is_root());
        TRY(file = "/foo/bar");   TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_root());
        TRY(file = "//foo/bar");  TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_root());

        TRY(file = "");           TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()), 1);
        TRY(file = "/");          TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()), 1);
        TRY(file = "foo");        TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()), 1);
        TRY(file = "/foo");       TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()), 1);
        TRY(file = "foo/bar");    TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()), 1);
        TRY(file = "/foo/bar");   TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()), 1);
        TRY(file = "//foo/bar");  TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()), 1);

        TRY(file = "");           TEST_EQUAL(file.path_form(), Path::form::empty);
        TRY(file = "/");          TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "foo");        TEST_EQUAL(file.path_form(), Path::form::relative);
        TRY(file = "/foo");       TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "foo/bar");    TEST_EQUAL(file.path_form(), Path::form::relative);
        TRY(file = "/foo/bar");   TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "//foo/bar");  TEST_EQUAL(file.path_form(), Path::form::absolute);

        TRY(file = "");         TEST(! file.is_leaf());
        TRY(file = "/");        TEST(! file.is_leaf());
        TRY(file = "/foo");     TEST(! file.is_leaf());
        TRY(file = "foo");      TEST(file.is_leaf());
        TRY(file = "foo/bar");  TEST(! file.is_leaf());

        TRY(file = "/");         TEST_EQUAL(file.as_url(), "file:///");
        TRY(file = "/foo");      TEST_EQUAL(file.as_url(), "file:///foo");
        TRY(file = "/foo/bar");  TEST_EQUAL(file.as_url(), "file:///foo/bar");

        TEST_THROW(Path().as_url(), std::invalid_argument);

    #else

        TRY(file = "");               TEST(! file.is_absolute());  TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "foo");            TEST(! file.is_absolute());  TEST(file.is_relative());    TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "foo/bar");        TEST(! file.is_absolute());  TEST(file.is_relative());    TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "C:/");            TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(file.is_root());
        TRY(file = "C:/foo");         TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "C:");             TEST(! file.is_absolute());  TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(file.is_drive_relative());    TEST(! file.is_root());
        TRY(file = "C:foo");          TEST(! file.is_absolute());  TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(file.is_drive_relative());    TEST(! file.is_root());
        TRY(file = "/");              TEST(! file.is_absolute());  TEST(! file.is_relative());  TEST(file.is_drive_absolute());    TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "/foo");           TEST(! file.is_absolute());  TEST(! file.is_relative());  TEST(file.is_drive_absolute());    TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "//foo/bar");      TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "//foo/");         TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(file.is_root());
        TRY(file = "//foo");          TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(file.is_root());
        TRY(file = "//?/foo");        TEST(! file.is_absolute());  TEST(file.is_relative());    TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "//?/foo/bar");    TEST(! file.is_absolute());  TEST(file.is_relative());    TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "//?/C:/");        TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(file.is_root());
        TRY(file = "//?/C:/foo");     TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "//?/C:");         TEST(! file.is_absolute());  TEST(file.is_relative());    TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "//?/C:foo");      TEST(! file.is_absolute());  TEST(file.is_relative());    TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "//?///foo/bar");  TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(! file.is_root());
        TRY(file = "//?///foo/");     TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(file.is_root());
        TRY(file = "//?///foo");      TEST(file.is_absolute());    TEST(! file.is_relative());  TEST(! file.is_drive_absolute());  TEST(! file.is_drive_relative());  TEST(file.is_root());

        TRY(file = "");               TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "foo");            TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "foo/bar");        TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "C:/");            TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "C:/foo");         TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "C:");             TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "C:foo");          TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "/");              TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "/foo");           TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//foo/bar");      TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//foo/");         TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//foo");          TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//?/foo");        TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//?/foo/bar");    TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//?/C:/");        TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//?/C:/foo");     TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//?/C:");         TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//?/C:foo");      TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//?///foo/bar");  TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//?///foo/");     TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);
        TRY(file = "//?///foo");      TEST_EQUAL(int(file.empty()) + int(file.is_absolute()) + int(file.is_relative()) + int(file.is_drive_absolute()) + int(file.is_drive_relative()), 1);

        TRY(file = "");               TEST_EQUAL(file.path_form(), Path::form::empty);
        TRY(file = "foo");            TEST_EQUAL(file.path_form(), Path::form::relative);
        TRY(file = "foo/bar");        TEST_EQUAL(file.path_form(), Path::form::relative);
        TRY(file = "C:/");            TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "C:/foo");         TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "C:");             TEST_EQUAL(file.path_form(), Path::form::drive_relative);
        TRY(file = "C:foo");          TEST_EQUAL(file.path_form(), Path::form::drive_relative);
        TRY(file = "/");              TEST_EQUAL(file.path_form(), Path::form::drive_absolute);
        TRY(file = "/foo");           TEST_EQUAL(file.path_form(), Path::form::drive_absolute);
        TRY(file = "//foo/bar");      TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "//foo/");         TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "//foo");          TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "//?/foo");        TEST_EQUAL(file.path_form(), Path::form::relative);
        TRY(file = "//?/foo/bar");    TEST_EQUAL(file.path_form(), Path::form::relative);
        TRY(file = "//?/C:/");        TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "//?/C:/foo");     TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "//?/C:");         TEST_EQUAL(file.path_form(), Path::form::relative);
        TRY(file = "//?/C:foo");      TEST_EQUAL(file.path_form(), Path::form::relative);
        TRY(file = "//?///foo/bar");  TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "//?///foo/");     TEST_EQUAL(file.path_form(), Path::form::absolute);
        TRY(file = "//?///foo");      TEST_EQUAL(file.path_form(), Path::form::absolute);

        TRY(file = "");         TEST(! file.is_leaf());
        TRY(file = "C:/");      TEST(! file.is_leaf());
        TRY(file = "C:/foo");   TEST(! file.is_leaf());
        TRY(file = "foo");      TEST(file.is_leaf());
        TRY(file = "foo/bar");  TEST(! file.is_leaf());

        TRY(file = "C:/");         TEST_EQUAL(file.as_url(), "file:///C:/");
        TRY(file = "C:/foo");      TEST_EQUAL(file.as_url(), "file:///C:/foo");
        TRY(file = "C:/foo/bar");  TEST_EQUAL(file.as_url(), "file:///C:/foo/bar");

        TEST_THROW(Path().as_url(), std::invalid_argument);

    #endif

}

void test_unicorn_path_name_breakdown() {

    Path file;
    Strings vec;

    #ifdef _XOPEN_SOURCE

        TRY(file = "");            TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 0);  TEST_EQUAL(to_str(vec), "[]");           TEST_EQUAL(Path::join(vec), file);
        TRY(file = "/");           TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 1);  TEST_EQUAL(to_str(vec), "[/]");          TEST_EQUAL(Path::join(vec), file);
        TRY(file = "foo");         TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 1);  TEST_EQUAL(to_str(vec), "[foo]");        TEST_EQUAL(Path::join(vec), file);
        TRY(file = "/foo");        TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 2);  TEST_EQUAL(to_str(vec), "[/,foo]");      TEST_EQUAL(Path::join(vec), file);
        TRY(file = "foo/bar");     TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 2);  TEST_EQUAL(to_str(vec), "[foo,bar]");    TEST_EQUAL(Path::join(vec), file);
        TRY(file = "/foo/bar");    TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 3);  TEST_EQUAL(to_str(vec), "[/,foo,bar]");  TEST_EQUAL(Path::join(vec), file);
        TRY(file = "///foo/bar");  TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 3);  TEST_EQUAL(to_str(vec), "[/,foo,bar]");  TEST_EQUAL(Path::join(vec), file);

        #ifdef __CYGWIN__
            TRY(file = "//foo/bar");  TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 2);  TEST_EQUAL(to_str(vec), "[//foo/,bar]");
        #else
            TRY(file = "//foo/bar");  TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 3);  TEST_EQUAL(to_str(vec), "[/,foo,bar]");
        #endif

        TRY(file = "");                         TEST_EQUAL(file.split_leaf().first, "");             TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = "hello.txt");                TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "hello.world.txt");          TEST_EQUAL(file.split_leaf().first, "hello.world");  TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "hello");                    TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = ".hello");                   TEST_EQUAL(file.split_leaf().first, ".hello");       TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = "/hello.txt");               TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "abc/def/hello.txt");        TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "abc/def/hello");            TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = "abc/def/hello.world.txt");  TEST_EQUAL(file.split_leaf().first, "hello.world");  TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "abc/def/.hello");           TEST_EQUAL(file.split_leaf().first, ".hello");       TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = "abc/def/.hello.txt");       TEST_EQUAL(file.split_leaf().first, ".hello");       TEST_EQUAL(file.split_leaf().second, ".txt");

        TRY(file = "");                         TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path());
        TRY(file = "hello.txt");                TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path("hello.txt"));
        TRY(file = "hello.world.txt");          TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path("hello.world.txt"));
        TRY(file = "hello");                    TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path("hello"));
        TRY(file = ".hello");                   TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path(".hello"));
        TRY(file = "/hello.txt");               TEST_EQUAL(file.split_path().first, Path("/"));        TEST_EQUAL(file.split_path().second, Path("hello.txt"));
        TRY(file = "abc/def/hello.txt");        TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path("hello.txt"));
        TRY(file = "abc/def/hello");            TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path("hello"));
        TRY(file = "abc/def/hello.world.txt");  TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path("hello.world.txt"));
        TRY(file = "abc/def/.hello");           TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path(".hello"));
        TRY(file = "abc/def/.hello.txt");       TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path(".hello.txt"));

        TRY(file = "");          TEST_EQUAL(file.split_root().first, "");   TEST_EQUAL(file.split_root().second, "");
        TRY(file = "/");         TEST_EQUAL(file.split_root().first, "/");  TEST_EQUAL(file.split_root().second, "");
        TRY(file = "/foo");      TEST_EQUAL(file.split_root().first, "/");  TEST_EQUAL(file.split_root().second, "foo");
        TRY(file = "/foo/bar");  TEST_EQUAL(file.split_root().first, "/");  TEST_EQUAL(file.split_root().second, "foo/bar");
        TRY(file = "foo/bar");   TEST_EQUAL(file.split_root().first, "");   TEST_EQUAL(file.split_root().second, "foo/bar");

    #else

        TRY(file = "");                TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 0);  TEST_EQUAL(to_str(vec), "[]");                   TEST_EQUAL(Path::join(vec), file);
        TRY(file = "C:/");             TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 1);  TEST_EQUAL(to_str(vec), "[C:\\]");               TEST_EQUAL(Path::join(vec), file);
        TRY(file = "foo");             TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 1);  TEST_EQUAL(to_str(vec), "[foo]");                TEST_EQUAL(Path::join(vec), file);
        TRY(file = "C:/foo");          TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 2);  TEST_EQUAL(to_str(vec), "[C:\\,foo]");           TEST_EQUAL(Path::join(vec), file);
        TRY(file = "foo/bar");         TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 2);  TEST_EQUAL(to_str(vec), "[foo,bar]");            TEST_EQUAL(Path::join(vec), file);
        TRY(file = "C:/foo/bar");      TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 3);  TEST_EQUAL(to_str(vec), "[C:\\,foo,bar]");       TEST_EQUAL(Path::join(vec), file);
        TRY(file = "C:foo/bar");       TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 3);  TEST_EQUAL(to_str(vec), "[C:,foo,bar]");         TEST_EQUAL(Path::join(vec), file);
        TRY(file = "/foo/bar");        TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 3);  TEST_EQUAL(to_str(vec), "[\\,foo,bar]");         TEST_EQUAL(Path::join(vec), file);
        TRY(file = "//foo/");          TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 1);  TEST_EQUAL(to_str(vec), "[\\\\foo\\]");          TEST_EQUAL(Path::join(vec), file);
        TRY(file = "//foo/bar");       TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 2);  TEST_EQUAL(to_str(vec), "[\\\\foo\\,bar]");      TEST_EQUAL(Path::join(vec), file);
        TRY(file = "//foo/bar/zap");   TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 3);  TEST_EQUAL(to_str(vec), "[\\\\foo\\,bar,zap]");  TEST_EQUAL(Path::join(vec), file);
        TRY(file = "///foo/bar/zap");  TRY(vec = file.breakdown());  TEST_EQUAL(vec.size(), 3);  TEST_EQUAL(to_str(vec), "[\\\\foo\\,bar,zap]");  TEST_EQUAL(Path::join(vec), file);

        TRY(file = "");                         TEST_EQUAL(file.split_leaf().first, "");             TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = "hello.txt");                TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "hello.world.txt");          TEST_EQUAL(file.split_leaf().first, "hello.world");  TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "hello");                    TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = ".hello");                   TEST_EQUAL(file.split_leaf().first, ".hello");       TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = "C:/hello.txt");             TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "C:hello.txt");              TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "abc/def/hello.txt");        TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "abc/def/hello");            TEST_EQUAL(file.split_leaf().first, "hello");        TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = "abc/def/hello.world.txt");  TEST_EQUAL(file.split_leaf().first, "hello.world");  TEST_EQUAL(file.split_leaf().second, ".txt");
        TRY(file = "abc/def/.hello");           TEST_EQUAL(file.split_leaf().first, ".hello");       TEST_EQUAL(file.split_leaf().second, "");
        TRY(file = "abc/def/.hello.txt");       TEST_EQUAL(file.split_leaf().first, ".hello");       TEST_EQUAL(file.split_leaf().second, ".txt");

        TRY(file = "");                         TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path());
        TRY(file = "hello.txt");                TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path("hello.txt"));
        TRY(file = "hello.world.txt");          TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path("hello.world.txt"));
        TRY(file = "hello");                    TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path("hello"));
        TRY(file = ".hello");                   TEST_EQUAL(file.split_path().first, Path());           TEST_EQUAL(file.split_path().second, Path(".hello"));
        TRY(file = "C:/hello.txt");             TEST_EQUAL(file.split_path().first, Path("C:/"));      TEST_EQUAL(file.split_path().second, Path("hello.txt"));
        TRY(file = "C:hello.txt");              TEST_EQUAL(file.split_path().first, Path("C:"));       TEST_EQUAL(file.split_path().second, Path("hello.txt"));
        TRY(file = "abc/def/hello.txt");        TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path("hello.txt"));
        TRY(file = "abc/def/hello");            TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path("hello"));
        TRY(file = "abc/def/hello.world.txt");  TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path("hello.world.txt"));
        TRY(file = "abc/def/.hello");           TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path(".hello"));
        TRY(file = "abc/def/.hello.txt");       TEST_EQUAL(file.split_path().first, Path("abc/def"));  TEST_EQUAL(file.split_path().second, Path(".hello.txt"));

        TRY(file = "");            TEST_EQUAL(file.split_root().first, Path());       TEST_EQUAL(file.split_root().second, Path());
        TRY(file = "C:/");         TEST_EQUAL(file.split_root().first, Path("C:/"));  TEST_EQUAL(file.split_root().second, Path());
        TRY(file = "C:/foo");      TEST_EQUAL(file.split_root().first, Path("C:/"));  TEST_EQUAL(file.split_root().second, Path("foo"));
        TRY(file = "C:/foo/bar");  TEST_EQUAL(file.split_root().first, Path("C:/"));  TEST_EQUAL(file.split_root().second, Path("foo/bar"));
        TRY(file = "foo/bar");     TEST_EQUAL(file.split_root().first, Path());       TEST_EQUAL(file.split_root().second, Path("foo/bar"));
        TRY(file = "C:foo/bar");   TEST_EQUAL(file.split_root().first, Path("C:"));   TEST_EQUAL(file.split_root().second, Path("foo/bar"));
        TRY(file = "/foo/bar");    TEST_EQUAL(file.split_root().first, Path("/"));    TEST_EQUAL(file.split_root().second, Path("foo/bar"));

    #endif

}

void test_unicorn_path_name_combination() {

    Path file1, file2, file3;

    #ifdef _XOPEN_SOURCE

        TRY(file1 = "");         TRY(file1 /= "");         TEST_EQUAL(file1.name(), "");
        TRY(file1 = "hello");    TRY(file1 /= "");         TEST_EQUAL(file1.name(), "hello");
        TRY(file1 = "");         TRY(file1 /= "world");    TEST_EQUAL(file1.name(), "world");
        TRY(file1 = "hello");    TRY(file1 /= "world");    TEST_EQUAL(file1.name(), "hello/world");
        TRY(file1 = "hello");    TRY(file1 /= "world/");   TEST_EQUAL(file1.name(), "hello/world");
        TRY(file1 = "hello");    TRY(file1 /= "/world");   TEST_EQUAL(file1.name(), "/world");
        TRY(file1 = "hello");    TRY(file1 /= "/world/");  TEST_EQUAL(file1.name(), "/world");
        TRY(file1 = "hello/");   TRY(file1 /= "world");    TEST_EQUAL(file1.name(), "hello/world");
        TRY(file1 = "hello/");   TRY(file1 /= "world/");   TEST_EQUAL(file1.name(), "hello/world");
        TRY(file1 = "hello/");   TRY(file1 /= "/world");   TEST_EQUAL(file1.name(), "/world");
        TRY(file1 = "hello/");   TRY(file1 /= "/world/");  TEST_EQUAL(file1.name(), "/world");
        TRY(file1 = "/hello");   TRY(file1 /= "world");    TEST_EQUAL(file1.name(), "/hello/world");
        TRY(file1 = "/hello");   TRY(file1 /= "world/");   TEST_EQUAL(file1.name(), "/hello/world");
        TRY(file1 = "/hello");   TRY(file1 /= "/world");   TEST_EQUAL(file1.name(), "/world");
        TRY(file1 = "/hello");   TRY(file1 /= "/world/");  TEST_EQUAL(file1.name(), "/world");
        TRY(file1 = "/hello/");  TRY(file1 /= "world");    TEST_EQUAL(file1.name(), "/hello/world");
        TRY(file1 = "/hello/");  TRY(file1 /= "world/");   TEST_EQUAL(file1.name(), "/hello/world");
        TRY(file1 = "/hello/");  TRY(file1 /= "/world");   TEST_EQUAL(file1.name(), "/world");
        TRY(file1 = "/hello/");  TRY(file1 /= "/world/");  TEST_EQUAL(file1.name(), "/world");

        TRY(file1 = "");                 TRY(file2 = "");                   TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "");
        TRY(file1 = "");                 TRY(file2 = "foo");                TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "");
        TRY(file1 = "");                 TRY(file2 = "/foo");               TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "");
        TRY(file1 = "foo");              TRY(file2 = "foo");                TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "foo");
        TRY(file1 = "foo");              TRY(file2 = "bar");                TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "");
        TRY(file1 = "foo");              TRY(file2 = "foo/bar");            TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "foo");
        TRY(file1 = "foo/bar");          TRY(file2 = "foo/bar");            TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "foo/bar");
        TRY(file1 = "foo/bar");          TRY(file2 = "foo/bar/zap");        TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "foo/bar");
        TRY(file1 = "foo/bar");          TRY(file2 = "foozle/bar");         TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "");
        TRY(file1 = "foo/bar");          TRY(file2 = "foo/barfle");         TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "foo");
        TRY(file1 = "foo/bar/zapgun");   TRY(file2 = "foo/bar/zapruder");   TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "foo/bar");
        TRY(file1 = "/foo");             TRY(file2 = "/foo");               TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "/foo");
        TRY(file1 = "/foo");             TRY(file2 = "/bar");               TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "/");
        TRY(file1 = "/foo");             TRY(file2 = "/foo/bar");           TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "/foo");
        TRY(file1 = "/foo/bar");         TRY(file2 = "/foo/bar");           TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "/foo/bar");
        TRY(file1 = "/foo/bar");         TRY(file2 = "/foo/bar/zap");       TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "/foo/bar");
        TRY(file1 = "/foo/bar");         TRY(file2 = "/foozle/bar");        TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "/");
        TRY(file1 = "/foo/bar");         TRY(file2 = "/foo/barfle");        TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "/foo");
        TRY(file1 = "/foo/bar/zapgun");  TRY(file2 = "/foo/bar/zapruder");  TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "/foo/bar");
        TRY(file1 = "foo");              TRY(file2 = "/foo");               TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3.name(), "");

        TRY(file1 = "/a/b/c");      TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), ".");
        TRY(file1 = "/a");          TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../..");
        TRY(file1 = "/a/b");        TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "..");
        TRY(file1 = "/a/b/c/d");    TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "d");
        TRY(file1 = "/a/b/c/d/e");  TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "d/e");
        TRY(file1 = "/a/b/c");      TRY(file2 = "/a");          TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "b/c");
        TRY(file1 = "/a/b/c");      TRY(file2 = "/a/b");        TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "c");
        TRY(file1 = "/a/b/c");      TRY(file2 = "/a/b/c/d");    TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "..");
        TRY(file1 = "/a/b/c");      TRY(file2 = "/a/b/c/d/e");  TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../..");
        TRY(file1 = "/a/b/x");      TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../x");
        TRY(file1 = "/a/b/x/y");    TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../x/y");
        TRY(file1 = "/a/x");        TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../x");
        TRY(file1 = "/a/x/y");      TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../x/y");
        TRY(file1 = "/a/x/y/z");    TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../x/y/z");
        TRY(file1 = "/x");          TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../../x");
        TRY(file1 = "/x/y");        TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../../x/y");
        TRY(file1 = "/x/y/z");      TRY(file2 = "/a/b/c");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../../x/y/z");
        TRY(file1 = "a/b/c");       TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), ".");
        TRY(file1 = "a");           TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../..");
        TRY(file1 = "a/b");         TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "..");
        TRY(file1 = "a/b/c/d");     TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "d");
        TRY(file1 = "a/b/c/d/e");   TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "d/e");
        TRY(file1 = "a/b/c");       TRY(file2 = "a");           TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "b/c");
        TRY(file1 = "a/b/c");       TRY(file2 = "a/b");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "c");
        TRY(file1 = "a/b/c");       TRY(file2 = "a/b/c/d");     TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "..");
        TRY(file1 = "a/b/c");       TRY(file2 = "a/b/c/d/e");   TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../..");
        TRY(file1 = "a/b/x");       TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../x");
        TRY(file1 = "a/b/x/y");     TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../x/y");
        TRY(file1 = "a/x");         TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../x");
        TRY(file1 = "a/x/y");       TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../x/y");
        TRY(file1 = "a/x/y/z");     TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../x/y/z");
        TRY(file1 = "x");           TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../../x");
        TRY(file1 = "x/y");         TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../../x/y");
        TRY(file1 = "x/y/z");       TRY(file2 = "a/b/c");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3.name(), "../../../x/y/z");

        TRY(file1 = "");        TRY(file2 = "");        TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "");        TRY(file2 = "a/b/c");   TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "");        TRY(file2 = "/a/b/c");  TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "a/b/c");   TRY(file2 = "");        TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "/a/b/c");  TRY(file2 = "");        TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "a/b/c");   TRY(file2 = "/a/b/c");  TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "/a/b/c");  TRY(file2 = "a/b/c");   TEST_THROW(file1.relative_to(file2), std::invalid_argument);

    #else

        TRY(file1 = "");             TRY(file1 /= "");             TEST_EQUAL(file1.name(), "");
        TRY(file1 = "hello");        TRY(file1 /= "");             TEST_EQUAL(file1.name(), "hello");
        TRY(file1 = "");             TRY(file1 /= "world");        TEST_EQUAL(file1.name(), "world");
        TRY(file1 = "hello");        TRY(file1 /= "world");        TEST_EQUAL(file1.name(), "hello\\world");
        TRY(file1 = "hello");        TRY(file1 /= "world\\");      TEST_EQUAL(file1.name(), "hello\\world");
        TRY(file1 = "hello");        TRY(file1 /= "C:\\world");    TEST_EQUAL(file1.name(), "C:\\world");
        TRY(file1 = "hello");        TRY(file1 /= "C:\\world\\");  TEST_EQUAL(file1.name(), "C:\\world");
        TRY(file1 = "hello\\");      TRY(file1 /= "world");        TEST_EQUAL(file1.name(), "hello\\world");
        TRY(file1 = "hello\\");      TRY(file1 /= "world\\");      TEST_EQUAL(file1.name(), "hello\\world");
        TRY(file1 = "hello\\");      TRY(file1 /= "C:\\world");    TEST_EQUAL(file1.name(), "C:\\world");
        TRY(file1 = "hello\\");      TRY(file1 /= "C:\\world\\");  TEST_EQUAL(file1.name(), "C:\\world");
        TRY(file1 = "C:\\hello");    TRY(file1 /= "world");        TEST_EQUAL(file1.name(), "C:\\hello\\world");
        TRY(file1 = "C:\\hello");    TRY(file1 /= "world\\");      TEST_EQUAL(file1.name(), "C:\\hello\\world");
        TRY(file1 = "C:\\hello");    TRY(file1 /= "C:\\world");    TEST_EQUAL(file1.name(), "C:\\world");
        TRY(file1 = "C:\\hello");    TRY(file1 /= "C:\\world\\");  TEST_EQUAL(file1.name(), "C:\\world");
        TRY(file1 = "C:\\hello\\");  TRY(file1 /= "world");        TEST_EQUAL(file1.name(), "C:\\hello\\world");
        TRY(file1 = "C:\\hello\\");  TRY(file1 /= "world\\");      TEST_EQUAL(file1.name(), "C:\\hello\\world");
        TRY(file1 = "C:\\hello\\");  TRY(file1 /= "C:\\world");    TEST_EQUAL(file1.name(), "C:\\world");
        TRY(file1 = "C:\\hello\\");  TRY(file1 /= "C:\\world\\");  TEST_EQUAL(file1.name(), "C:\\world");

        TRY(file1 = "");                      TRY(file2 = "");                        TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "");
        TRY(file1 = "");                      TRY(file2 = "foo");                     TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "");
        TRY(file1 = "");                      TRY(file2 = "C:\\foo");                 TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "");
        TRY(file1 = "foo");                   TRY(file2 = "foo");                     TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "foo");
        TRY(file1 = "foo");                   TRY(file2 = "bar");                     TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "");
        TRY(file1 = "foo");                   TRY(file2 = "foo\\bar");                TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "foo");
        TRY(file1 = "foo\\bar");              TRY(file2 = "foo\\bar");                TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "foo\\bar");
        TRY(file1 = "foo\\bar");              TRY(file2 = "foo\\bar\\zap");           TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "foo\\bar");
        TRY(file1 = "foo\\bar");              TRY(file2 = "foozle\\bar");             TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "");
        TRY(file1 = "foo\\bar");              TRY(file2 = "foo\\barfle");             TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "foo");
        TRY(file1 = "foo\\bar\\zapgun");      TRY(file2 = "foo\\bar\\zapruder");      TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "foo\\bar");
        TRY(file1 = "C:\\foo");               TRY(file2 = "C:\\foo");                 TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:\\foo");
        TRY(file1 = "C:\\foo");               TRY(file2 = "C:\\bar");                 TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:\\");
        TRY(file1 = "C:\\foo");               TRY(file2 = "C:\\foo\\bar");            TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:\\foo");
        TRY(file1 = "C:\\foo\\bar");          TRY(file2 = "C:\\foo\\bar");            TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:\\foo\\bar");
        TRY(file1 = "C:\\foo\\bar");          TRY(file2 = "C:\\foo\\bar\\zap");       TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:\\foo\\bar");
        TRY(file1 = "C:\\foo\\bar");          TRY(file2 = "C:\\foozle\\bar");         TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:\\");
        TRY(file1 = "C:\\foo\\bar");          TRY(file2 = "C:\\foo\\barfle");         TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:\\foo");
        TRY(file1 = "C:\\foo\\bar\\zapgun");  TRY(file2 = "C:\\foo\\bar\\zapruder");  TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:\\foo\\bar");
        TRY(file1 = "foo");                   TRY(file2 = "C:\\foo");                 TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "");
        TRY(file1 = "C:\\");                  TRY(file2 = "D:\\");                    TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "");
        TRY(file1 = "C:");                    TRY(file2 = "C:");                      TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:");
        TRY(file1 = "C:foo");                 TRY(file2 = "C:foo");                   TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "C:foo");
        TRY(file1 = "C:");                    TRY(file2 = "D:");                      TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "");
        TRY(file1 = "\\");                    TRY(file2 = "\\");                      TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "\\");
        TRY(file1 = "\\foo");                 TRY(file2 = "\\foo");                   TRY(file3 = Path::common(file1, file2));  TEST_EQUAL(file3, "\\foo");

        TRY(file1 = "C:\\a\\b\\c");         TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, ".");
        TRY(file1 = "C:\\a");               TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..");
        TRY(file1 = "C:\\a\\b");            TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..");
        TRY(file1 = "C:\\a\\b\\c\\d");      TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "d");
        TRY(file1 = "C:\\a\\b\\c\\d\\e");   TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "d\\e");
        TRY(file1 = "C:\\a\\b\\c");         TRY(file2 = "C:\\a");               TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "b\\c");
        TRY(file1 = "C:\\a\\b\\c");         TRY(file2 = "C:\\a\\b");            TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "c");
        TRY(file1 = "C:\\a\\b\\c");         TRY(file2 = "C:\\a\\b\\c\\d");      TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..");
        TRY(file1 = "C:\\a\\b\\c");         TRY(file2 = "C:\\a\\b\\c\\d\\e");   TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..");
        TRY(file1 = "C:\\a\\b\\x");         TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\x");
        TRY(file1 = "C:\\a\\b\\x\\y");      TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\x\\y");
        TRY(file1 = "C:\\a\\x");            TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\x");
        TRY(file1 = "C:\\a\\x\\y");         TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\x\\y");
        TRY(file1 = "C:\\a\\x\\y\\z");      TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\x\\y\\z");
        TRY(file1 = "C:\\x");               TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\..\\x");
        TRY(file1 = "C:\\x\\y");            TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\..\\x\\y");
        TRY(file1 = "C:\\x\\y\\z");         TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\..\\x\\y\\z");
        TRY(file1 = "a\\b\\c");             TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, ".");
        TRY(file1 = "a");                   TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..");
        TRY(file1 = "a\\b");                TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..");
        TRY(file1 = "a\\b\\c\\d");          TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "d");
        TRY(file1 = "a\\b\\c\\d\\e");       TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "d\\e");
        TRY(file1 = "a\\b\\c");             TRY(file2 = "a");                   TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "b\\c");
        TRY(file1 = "a\\b\\c");             TRY(file2 = "a\\b");                TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "c");
        TRY(file1 = "a\\b\\c");             TRY(file2 = "a\\b\\c\\d");          TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..");
        TRY(file1 = "a\\b\\c");             TRY(file2 = "a\\b\\c\\d\\e");       TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..");
        TRY(file1 = "a\\b\\x");             TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\x");
        TRY(file1 = "a\\b\\x\\y");          TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\x\\y");
        TRY(file1 = "a\\x");                TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\x");
        TRY(file1 = "a\\x\\y");             TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\x\\y");
        TRY(file1 = "a\\x\\y\\z");          TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\x\\y\\z");
        TRY(file1 = "x");                   TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\..\\x");
        TRY(file1 = "x\\y");                TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\..\\x\\y");
        TRY(file1 = "x\\y\\z");             TRY(file2 = "a\\b\\c");             TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\..\\x\\y\\z");
        TRY(file1 = "C:\\a\\b\\c");         TRY(file2 = "D:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "C:\\a\\b\\c");
        TRY(file1 = "C:\\a\\b\\c");         TRY(file2 = "\\\\hello\\a\\b\\c");  TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "C:\\a\\b\\c");
        TRY(file1 = "\\\\hello\\a\\b\\c");  TRY(file2 = "\\\\hello\\a\\b\\c");  TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, ".");
        TRY(file1 = "\\\\hello\\a\\b\\c");  TRY(file2 = "\\\\hello");           TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "a\\b\\c");
        TRY(file1 = "\\\\hello");           TRY(file2 = "\\\\hello\\a\\b\\c");  TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\..");
        TRY(file1 = "\\\\hello\\x\\y\\z");  TRY(file2 = "\\\\hello\\a\\b\\c");  TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "..\\..\\..\\x\\y\\z");
        TRY(file1 = "\\\\hello\\a\\b\\c");  TRY(file2 = "C:\\a\\b\\c");         TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "\\\\hello\\a\\b\\c");
        TRY(file1 = "\\\\hello");           TRY(file2 = "\\\\world");           TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "\\\\hello");
        TRY(file1 = "\\\\hello\\a\\b\\c");  TRY(file2 = "\\\\world\\a\\b\\c");  TRY(file3 = file1.relative_to(file2));  TEST_EQUAL(file3, "\\\\hello\\a\\b\\c");

        TRY(file1 = "");             TRY(file2 = "");             TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "");             TRY(file2 = "a\\b\\c");      TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "");             TRY(file2 = "C:\\a\\b\\c");  TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "a\\b\\c");      TRY(file2 = "");             TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "C:\\a\\b\\c");  TRY(file2 = "");             TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "a\\b\\c");      TRY(file2 = "C:\\a\\b\\c");  TEST_THROW(file1.relative_to(file2), std::invalid_argument);
        TRY(file1 = "C:\\a\\b\\c");  TRY(file2 = "a\\b\\c");      TEST_THROW(file1.relative_to(file2), std::invalid_argument);

    #endif

}

void test_unicorn_path_name_manipulation() {

    Path file1, file2;

    TRY(file1 = "hello");            TRY(file2 = file1.change_ext(".doc"));  TEST_EQUAL(file2.name(), "hello.doc");
    TRY(file1 = "hello");            TRY(file2 = file1.change_ext("doc"));   TEST_EQUAL(file2.name(), "hello.doc");
    TRY(file1 = "hello");            TRY(file2 = file1.change_ext(""));      TEST_EQUAL(file2.name(), "hello");
    TRY(file1 = "hello.txt");        TRY(file2 = file1.change_ext(".doc"));  TEST_EQUAL(file2.name(), "hello.doc");
    TRY(file1 = "hello.txt");        TRY(file2 = file1.change_ext("doc"));   TEST_EQUAL(file2.name(), "hello.doc");
    TRY(file1 = "hello.txt");        TRY(file2 = file1.change_ext(""));      TEST_EQUAL(file2.name(), "hello");
    TRY(file1 = "hello.world.txt");  TRY(file2 = file1.change_ext(".doc"));  TEST_EQUAL(file2.name(), "hello.world.doc");
    TRY(file1 = "hello.world.txt");  TRY(file2 = file1.change_ext("doc"));   TEST_EQUAL(file2.name(), "hello.world.doc");
    TRY(file1 = "hello.world.txt");  TRY(file2 = file1.change_ext(""));      TEST_EQUAL(file2.name(), "hello.world");
    TRY(file1 = ".hello");           TRY(file2 = file1.change_ext(".doc"));  TEST_EQUAL(file2.name(), ".hello.doc");
    TRY(file1 = ".hello");           TRY(file2 = file1.change_ext("doc"));   TEST_EQUAL(file2.name(), ".hello.doc");
    TRY(file1 = ".hello");           TRY(file2 = file1.change_ext(""));      TEST_EQUAL(file2.name(), ".hello");

    TRY(file1 = "");  TEST_THROW(file1.change_ext(".doc"), std::invalid_argument);

    #ifdef _XOPEN_SOURCE

        TRY(file1 = "/foo/bar/hello");      TRY(file2 = file1.change_ext(".doc"));  TEST_EQUAL(file2.name(), "/foo/bar/hello.doc");
        TRY(file1 = "/foo/bar/hello");      TRY(file2 = file1.change_ext("doc"));   TEST_EQUAL(file2.name(), "/foo/bar/hello.doc");
        TRY(file1 = "/foo/bar/hello");      TRY(file2 = file1.change_ext(""));      TEST_EQUAL(file2.name(), "/foo/bar/hello");
        TRY(file1 = "/foo/bar/hello.txt");  TRY(file2 = file1.change_ext(".doc"));  TEST_EQUAL(file2.name(), "/foo/bar/hello.doc");
        TRY(file1 = "/foo/bar/hello.txt");  TRY(file2 = file1.change_ext("doc"));   TEST_EQUAL(file2.name(), "/foo/bar/hello.doc");
        TRY(file1 = "/foo/bar/hello.txt");  TRY(file2 = file1.change_ext(""));      TEST_EQUAL(file2.name(), "/foo/bar/hello");

        TRY(file1 = "/");  TEST_THROW(file1.change_ext(".doc"), std::invalid_argument);

    #else

        TRY(file1 = "\\foo\\bar\\hello");      TRY(file2 = file1.change_ext(".doc"));  TEST_EQUAL(file2.name(), "\\foo\\bar\\hello.doc");
        TRY(file1 = "\\foo\\bar\\hello");      TRY(file2 = file1.change_ext("doc"));   TEST_EQUAL(file2.name(), "\\foo\\bar\\hello.doc");
        TRY(file1 = "\\foo\\bar\\hello");      TRY(file2 = file1.change_ext(""));      TEST_EQUAL(file2.name(), "\\foo\\bar\\hello");
        TRY(file1 = "\\foo\\bar\\hello.txt");  TRY(file2 = file1.change_ext(".doc"));  TEST_EQUAL(file2.name(), "\\foo\\bar\\hello.doc");
        TRY(file1 = "\\foo\\bar\\hello.txt");  TRY(file2 = file1.change_ext("doc"));   TEST_EQUAL(file2.name(), "\\foo\\bar\\hello.doc");
        TRY(file1 = "\\foo\\bar\\hello.txt");  TRY(file2 = file1.change_ext(""));      TEST_EQUAL(file2.name(), "\\foo\\bar\\hello");

        TRY(file1 = "\\");    TEST_THROW(file1.change_ext(".doc"), std::invalid_argument);
        TRY(file1 = "C:\\");  TEST_THROW(file1.change_ext(".doc"), std::invalid_argument);

    #endif

}

void test_unicorn_path_name_comparison() {

    Path file1, file2, file3;

    TRY(file1 = "foo/bar/hello.txt");
    TRY(file2 = "foo/bar/Hello.txt");
    TRY(file3 = "foo/bar/goodbye.txt");

    TEST(! Path::equal(Path::cmp::cased)(file1, file2));
    TEST(! Path::equal(Path::cmp::cased)(file1, file3));
    TEST(! Path::less(Path::cmp::cased)(file1, file2));
    TEST(Path::less(Path::cmp::cased)(file2, file1));
    TEST(! Path::less(Path::cmp::cased)(file1, file3));
    TEST(Path::less(Path::cmp::cased)(file3, file1));
    TEST(Path::less(Path::cmp::cased)(file2, file3));
    TEST(! Path::less(Path::cmp::cased)(file3, file2));

    TEST(Path::equal(Path::cmp::icase)(file1, file2));
    TEST(! Path::equal(Path::cmp::icase)(file1, file3));
    TEST(! Path::less(Path::cmp::icase)(file1, file2));
    TEST(! Path::less(Path::cmp::icase)(file2, file1));
    TEST(! Path::less(Path::cmp::icase)(file1, file3));
    TEST(Path::less(Path::cmp::icase)(file3, file1));
    TEST(! Path::less(Path::cmp::icase)(file2, file3));
    TEST(Path::less(Path::cmp::icase)(file3, file2));

    #if defined(__APPLE__) || defined(__CYGWIN__) || defined(_WIN32)

        TEST(Path::equal()(file1, file2));
        TEST(! Path::equal()(file1, file3));
        TEST(! Path::less()(file1, file2));
        TEST(! Path::less()(file2, file1));
        TEST(! Path::less()(file1, file3));
        TEST(Path::less()(file3, file1));
        TEST(! Path::less()(file2, file3));
        TEST(Path::less()(file3, file2));

    #else

        TEST(! Path::equal()(file1, file2));
        TEST(! Path::equal()(file1, file3));
        TEST(! Path::less()(file1, file2));
        TEST(Path::less()(file2, file1));
        TEST(! Path::less()(file1, file3));
        TEST(Path::less()(file3, file1));
        TEST(Path::less()(file2, file3));
        TEST(! Path::less()(file3, file2));

    #endif

}

void test_unicorn_path_resolution() {

    Path cwd, f1, f2;
    Ustring cwdstr;

    TRY(cwd = Path::current_directory());
    TRY(cwdstr = cwd.name());
    std::replace(cwdstr.begin(), cwdstr.end(), '\\', '/');
    TEST_MATCH(cwdstr, "/unicorn-lib$");

    #ifdef _XOPEN_SOURCE

        #ifdef _XOPEN_SOURCE
            Path home = cstr(getenv("HOME"));
        #endif

        TRY(f1 = Path());               TRY(f2 = f1.resolve());  TEST_EQUAL(f2, cwd);
        TRY(f1 = Path("Makefile"));     TRY(f2 = f1.resolve());  TEST_EQUAL(f2, cwd / "Makefile");
        TRY(f1 = Path("."));            TRY(f2 = f1.resolve());  TEST_EQUAL(f2, cwd);
        TRY(f1 = Path("./Makefile"));   TRY(f2 = f1.resolve());  TEST_EQUAL(f2, cwd / "Makefile");
        TRY(f1 = Path("~"));            TRY(f2 = f1.resolve());  TEST_EQUAL(f2, home);
        TRY(f1 = Path("~/Documents"));  TRY(f2 = f1.resolve());  TEST_EQUAL(f2, home / "Documents");
        TRY(f1 = Path("/"));            TRY(f2 = f1.resolve());  TEST_EQUAL(f2, Path("/"));
        TRY(f1 = Path("/usr"));         TRY(f2 = f1.resolve());  TEST_EQUAL(f2, Path("/usr"));
        TRY(f1 = Path("/usr/local"));   TRY(f2 = f1.resolve());  TEST_EQUAL(f2, Path("/usr/local"));

    #else

        TRY(f1 = Path());              TRY(f2 = f1.resolve());  TEST_EQUAL(f2, cwd);
        TRY(f1 = Path("Makefile"));    TRY(f2 = f1.resolve());  TEST_EQUAL(f2, cwd / "Makefile");
        TRY(f1 = Path("."));           TRY(f2 = f1.resolve());  TEST_EQUAL(f2, cwd);
        TRY(f1 = Path("./Makefile"));  TRY(f2 = f1.resolve());  TEST_EQUAL(f2, cwd / "Makefile");
        TRY(f1 = Path("C:/"));         TRY(f2 = f1.resolve());  TEST_EQUAL(f2, Path("C:/"));
        TRY(f1 = Path("C:/Windows"));  TRY(f2 = f1.resolve());  TEST_EQUAL(f2, Path("C:/Windows"));

    #endif

}

void test_unicorn_path_directory_iterators() {

    Path dir;
    Path::directory_range range;
    std::vector<Path> files;

    TRY(range = dir.directory());
    REQUIRE(! range.empty());
    TRY(std::copy(range.begin(), range.end(), overwrite(files)));
    TRY(std::sort(files.begin(), files.end()));
    TEST_EQUAL(files[0], Path(".git"));

    TRY(dir = "unicorn");
    TRY(range = dir.directory());
    REQUIRE(! range.empty());
    TRY(std::copy(range.begin(), range.end(), overwrite(files)));
    TRY(std::sort(files.begin(), files.end()));
    TEST_EQUAL(files[0], Path("unicorn/character-test.cpp"));

}

void test_unicorn_path_file_system_queries() {

    static const Path null = "";
    static const Path dot1 = ".";
    static const Path dot2 = "..";
    static const Path make = "Makefile";
    static const Path bdir = "build";
    static const Path nosf = "no such file";

    #ifdef _XOPEN_SOURCE
        static const Path root = "/";
    #else
        static const Path root = "C:/";
    #endif

    Path::id_type id0 = {}, id1 = {}, id2 = {};

    TEST(! null.exists());  TEST(! null.is_directory());  TEST(! null.is_file());  TEST(! null.is_special());  TEST(! null.is_hidden());  TEST(! null.is_symlink());
    TEST(dot1.exists());    TEST(dot1.is_directory());    TEST(! dot1.is_file());  TEST(! dot1.is_special());  TEST(! dot1.is_hidden());  TEST(! dot1.is_symlink());
    TEST(dot2.exists());    TEST(dot2.is_directory());    TEST(! dot2.is_file());  TEST(! dot2.is_special());  TEST(! dot2.is_hidden());  TEST(! dot2.is_symlink());
    TEST(make.exists());    TEST(! make.is_directory());  TEST(make.is_file());    TEST(! make.is_special());  TEST(! make.is_hidden());  TEST(! make.is_symlink());
    TEST(bdir.exists());    TEST(bdir.is_directory());    TEST(! bdir.is_file());  TEST(! bdir.is_special());  TEST(! bdir.is_hidden());  TEST(! bdir.is_symlink());
    TEST(! nosf.exists());  TEST(! nosf.is_directory());  TEST(! nosf.is_file());  TEST(! nosf.is_special());  TEST(! nosf.is_hidden());  TEST(! nosf.is_symlink());
    TEST(root.exists());    TEST(root.is_directory());    TEST(! root.is_file());  TEST(! root.is_special());  TEST(! root.is_hidden());  TEST(! root.is_symlink());

    #ifdef _XOPEN_SOURCE
        TEST(Path("/").exists());
        TEST(! Path("C:/").exists());
        TEST(Path(".git").is_hidden());
    #else
        TEST(Path("C:/").exists());
        TEST(Path("C:").exists());
        TEST(Path("/").exists());
    #endif

    TEST_EQUAL(Path().id(), id0);
    TEST_EQUAL(Path("no such file").id(), id0);
    TRY(id1 = Path("Makefile").id());
    TRY(id2 = Path("build").id());
    TEST_COMPARE(id1, >, id0);
    TEST_COMPARE(id2, >, id0);
    TEST_COMPARE(id2, !=, id1);

    TEST_COMPARE(Path("Makefile").size(), >, 10'000);
    TEST_COMPARE(Path("Makefile").size(Path::recurse), >, 10'000);
    TEST_EQUAL(Path("no such file").size(), 0);
    TEST_EQUAL(Path("no such file").size(Path::recurse), 0);
    TEST_COMPARE(Path(".").size(Path::recurse), >, 1'000'000);
    TEST_COMPARE(Path("build").size(Path::recurse), >, 1'000'000);

}

void test_unicorn_path_file_system_updates() {

    Path d1, d2, d3, f1, f2, f3, f4;
    std::vector<Path> files;
    Path::directory_range range;

    TRY(f1 = "__test_file_1__");
    TRY(f2 = "__test_file_2__");
    TRY(f3 = "__test_file_3__");
    TRY(f4 = "__test_FILE_3__");
    TEST(! f1.exists());
    TEST(! f1.is_directory());
    TEST(! f2.exists());
    TEST(! f2.is_directory());
    TRY(f1.create());
    TEST(f1.exists());
    TRY(f1.move_to(f2));
    TEST(! f1.exists());
    TEST(f2.exists());
    TRY(f2.copy_to(f3));
    TEST(f2.exists());
    TEST(f3.exists());
    TRY(f3.move_to(f4));
    TEST(f4.exists());
    TRY(f1.remove());
    TRY(f2.remove());
    TRY(f3.remove());
    TRY(f4.remove());
    TEST(! f1.exists());
    TEST(! f2.exists());
    TEST(! f3.exists());
    TEST(! f4.exists());

    TRY(d1 = "__test_dir_1__");
    TRY(d2 = "__test_dir_2__");
    TRY(d3 = "__test_dir_3__");
    TEST(! d1.exists());
    TEST(! d1.is_directory());
    TEST(! d2.exists());
    TEST(! d2.is_directory());
    TRY(d1.make_directory());
    TEST(d1.exists());
    TEST(d1.is_directory());
    TRY(d1.make_directory());
    TRY(d1.move_to(d2));
    TEST(! d1.exists());
    TEST(! d1.is_directory());
    TEST(d2.exists());
    TEST(d2.is_directory());
    TEST_THROW(d2.copy_to(d3), std::system_error);
    TRY(d2.copy_to(d3, Path::recurse));
    TEST(d2.exists());
    TEST(d2.is_directory());
    TEST(d3.exists());
    TEST(d3.is_directory());
    TRY(d1.remove());
    TRY(d2.remove());
    TRY(d3.remove());
    TEST(! d1.exists());
    TEST(! d1.is_directory());
    TEST(! d2.exists());
    TEST(! d2.is_directory());
    TEST(! d3.exists());
    TEST(! d3.is_directory());

    TRY(d1 = "__test_dir_1__");
    TRY(d2 = "__test_dir_1__/subdir");
    TEST(! d1.exists());
    TEST(! d2.exists());
    TEST_THROW(d2.make_directory(), std::system_error);
    TRY(d2.make_directory(Path::recurse));
    TEST(d1.exists());
    TEST(d2.exists());
    TEST(d1.is_directory());
    TEST(d2.is_directory());
    TRY(d2.make_directory(Path::recurse));
    TEST_THROW(d1.remove(), std::system_error);
    TRY(d1.remove(Path::recurse));
    std::this_thread::sleep_for(10ms);
    TEST(! d1.exists());
    TEST(! d2.exists());

    TRY(d1 = "__test_dir_1__");
    TRY(f1 = d1 / "hello");
    TRY(f2 = d1 / "world");
    TRY(range = d1.directory());
    TRY(std::copy(range.begin(), range.end(), overwrite(files)));
    TEST_EQUAL(files.size(), 0);
    TEST_EQUAL(to_str(files), "[]");
    TRY(d1.make_directory());
    TEST(d1.exists());
    TEST(d1.is_directory());
    TRY(range = d1.directory());
    TRY(std::copy(range.begin(), range.end(), overwrite(files)));
    TEST_EQUAL(files.size(), 0);
    TEST_EQUAL(to_str(files), "[]");
    TRY(f1.create());
    TEST(f1.exists());
    TRY(f2.create());
    TEST(f2.exists());
    TEST_THROW(f1.make_directory(), std::system_error);
    TRY(range = d1.directory());
    TRY(std::copy(range.begin(), range.end(), overwrite(files)));
    TEST_EQUAL(files.size(), 2);
    std::sort(files.begin(), files.end());
    #ifdef _XOPEN_SOURCE
        TEST_EQUAL(to_str(files), "[__test_dir_1__/hello,__test_dir_1__/world]");
    #else
        TEST_EQUAL(to_str(files), "[__test_dir_1__\\hello,__test_dir_1__\\world]");
    #endif
    TEST_THROW(d1.remove(), std::system_error);
    TEST(d1.exists());
    TEST(f1.exists());
    TEST(f2.exists());
    TRY(d1.remove(Path::recurse));
    TEST(! d1.exists());
    TEST(! f1.exists());
    TEST(! f2.exists());
    TRY(range = d1.directory());
    TRY(std::copy(range.begin(), range.end(), overwrite(files)));
    TEST_EQUAL(files.size(), 0);
    TEST_EQUAL(to_str(files), "[]");

}

void test_unicorn_path_current_directory() {

    Path dot = ".", parent = "..", test = "__test_cwd__";
    Path abs_dot, abs_test, cwd;

    TRY(abs_dot = dot.resolve());
    TRY(abs_test = test.resolve());
    TRY(cwd = Path::current_directory());
    TEST_EQUAL(cwd, abs_dot);
    TRY(test.make_directory());
    TEST(test.exists());
    TEST(test.is_directory());
    TRY(test.change_directory());
    TRY(cwd = Path::current_directory());
    TEST_EQUAL(cwd, abs_test);
    TRY(parent.change_directory());
    TRY(cwd = Path::current_directory());
    TEST_EQUAL(cwd, abs_dot);
    TRY(test.remove());

}

void test_unicorn_path_deep_search() {

    Path root = "__test_root__";
    Path::deep_search_range range;
    std::vector<Path> files;
    Ustring s;
    auto guard = scope_exit([=] { root.remove(Path::recurse); });

    TEST(! root.exists());
    TRY(range = root.deep_search());
    TEST(range.empty());

    TRY(root.make_directory());
    TEST(root.exists());
    TEST(root.is_directory());
    TRY(range = root.deep_search());
    TEST(range.empty());

    TRY((root / "foo").make_directory());
    TRY((root / "bar").make_directory());
    TRY((root / "zap").make_directory());
    TRY((root / "foo/alpha").create());
    TRY((root / "foo/bravo").create());
    TRY((root / "foo/charlie").create());
    TRY((root / "bar/foobar").make_directory());
    TRY((root / "bar/foobar/xray").create());
    TRY((root / "bar/foobar/yankee").create());
    TRY((root / "bar/foobar/zulu").create());

    TRY(range = root.deep_search());
    TRY(std::copy(range.begin(), range.end(), overwrite(files)));
    TEST_EQUAL(files.size(), 10);
    TEST_COMPARE(index_of(Path("__test_root__/foo"), files), <, index_of(Path("__test_root__/foo/alpha"), files));
    TEST_COMPARE(index_of(Path("__test_root__/bar"), files), <, index_of(Path("__test_root__/bar/foobar"), files));
    TEST_COMPARE(index_of(Path("__test_root__/bar/foobar"), files), <, index_of(Path("__test_root__/bar/foobar/xray"), files));
    std::sort(files.begin(), files.end());
    TRY(s = to_str(files));
    std::replace(s.begin(), s.end(), '\\', '/');
    TEST_EQUAL(s, "["
        "__test_root__/bar,"
        "__test_root__/bar/foobar,"
        "__test_root__/bar/foobar/xray,"
        "__test_root__/bar/foobar/yankee,"
        "__test_root__/bar/foobar/zulu,"
        "__test_root__/foo,"
        "__test_root__/foo/alpha,"
        "__test_root__/foo/bravo,"
        "__test_root__/foo/charlie,"
        "__test_root__/zap"
    "]");

    TRY(range = root.deep_search(Path::bottom_up));
    TRY(std::copy(range.begin(), range.end(), overwrite(files)));
    TEST_EQUAL(files.size(), 10);
    TEST_COMPARE(index_of(Path("__test_root__/foo"), files), >, index_of(Path("__test_root__/foo/alpha"), files));
    TEST_COMPARE(index_of(Path("__test_root__/bar"), files), >, index_of(Path("__test_root__/bar/foobar"), files));
    TEST_COMPARE(index_of(Path("__test_root__/bar/foobar"), files), >, index_of(Path("__test_root__/bar/foobar/xray"), files));
    std::sort(files.begin(), files.end());
    TRY(s = to_str(files));
    std::replace(s.begin(), s.end(), '\\', '/');
    TEST_EQUAL(s, "["
        "__test_root__/bar,"
        "__test_root__/bar/foobar,"
        "__test_root__/bar/foobar/xray,"
        "__test_root__/bar/foobar/yankee,"
        "__test_root__/bar/foobar/zulu,"
        "__test_root__/foo,"
        "__test_root__/foo/alpha,"
        "__test_root__/foo/bravo,"
        "__test_root__/foo/charlie,"
        "__test_root__/zap"
    "]");

}

void test_unicorn_path_io() {

    Path makefile = "Makefile";
    Path testfile = "__test_io__";
    Path nofile = "__no_such_file__";
    Ustring s;

    TRY(makefile.load(s));
    TEST_EQUAL(s.size(), makefile.size());
    TEST_EQUAL(s.substr(0, 25), "# Grand Unified Makefile\n");
    TRY(makefile.load(s, 10));
    TEST_EQUAL(s.size(), 10);
    TEST_EQUAL(s.substr(0, 25), "# Grand Un");

    TRY(testfile.save("Hello world\n"s));
    TEST(testfile.exists());
    TEST_EQUAL(testfile.size(), 12);

    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\n");
    TRY(testfile.save("Goodbye\n"s, Path::append));
    TEST_EQUAL(testfile.size(), 20);
    TRY(testfile.load(s));
    TEST_EQUAL(s, "Hello world\nGoodbye\n");

    TEST_THROW(nofile.load(s), std::ios::failure);
    TRY(nofile.load(s, npos, Path::may_fail));
    TEST(s.empty());

    TRY(testfile.remove());
    TEST(! testfile.exists());

}

void test_unicorn_path_links() {

    Path file = "__test_sym_file__";
    Path link = "__test_sym_link__";
    size_t bytes = 1'000'000;
    Ustring text(bytes, 'x');

    TEST(! file.exists());
    TEST(! link.exists());

    TRY(file.save(text));
    TEST(file.exists());
    TEST_EQUAL(file.size(), bytes);

    #ifdef _XOPEN_SOURCE
        TRY(file.make_symlink(link));
        TEST(link.exists());
        TEST(link.is_symlink());
        TEST_EQUAL(link.resolve_symlink(), file);
        TEST_EQUAL(link.size(), bytes);
        TEST_EQUAL(link.size(Path::no_follow), file.name().size());
    #else
        TEST_THROW(file.make_symlink(link), std::system_error);
        TRY(file.make_symlink(link, Path::may_copy));
        TEST(link.exists());
        TEST(! link.is_symlink());
        TEST_EQUAL(link.size(), bytes);
    #endif

    TRY(link.remove());
    TEST(! link.exists());
    TEST(file.exists());
    TRY(file.remove());
    TEST(! file.exists());

}

void test_unicorn_path_metadata() {

    Path file = "__test_meta_file__";
    seconds sec;
    system_clock::time_point date, epoch;

    TEST(! file.exists());
    TRY(date = file.access_time());
    TRY(sec = duration_cast<seconds>(date - epoch));
    TEST_EQUAL(sec.count(), 0);
    TRY(date = file.modify_time());
    TRY(sec = duration_cast<seconds>(date - epoch));
    TEST_EQUAL(sec.count(), 0);

    TRY(file.create());
    TEST(file.exists());
    TRY(date = file.access_time());
    TRY(sec = duration_cast<seconds>(system_clock::now() - date));
    TEST_NEAR_EPSILON(sec.count(), 0, 2);
    TRY(date = file.modify_time());
    TRY(sec = duration_cast<seconds>(system_clock::now() - date));
    TEST_NEAR_EPSILON(sec.count(), 0, 2);

    TRY(file.set_access_time(system_clock::now() - 1min));
    TRY(date = file.access_time());
    TRY(sec = duration_cast<seconds>(system_clock::now() - date));
    TEST_NEAR_EPSILON(sec.count(), 60, 2);

    TRY(file.set_modify_time(system_clock::now() - 2min));
    TRY(date = file.modify_time());
    TRY(sec = duration_cast<seconds>(system_clock::now() - date));
    TEST_NEAR_EPSILON(sec.count(), 120, 2);

    TRY(file.remove());
    TEST(! file.exists());

}
