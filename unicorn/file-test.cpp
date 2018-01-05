#include "unicorn/core.hpp"
#include "unicorn/file.hpp"
#include "unicorn/utf.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;
using namespace std::literals;

#ifdef _XOPEN_SOURCE
    #define SLASH "/"
#else
    #define SLASH "\\\\"
#endif

namespace {

    template <typename C>
    void touch(const std::basic_string<C>& file) {
        std::ofstream(to_utf8(file));
    }

}

void test_unicorn_file_legal_names() {

    TEST(is_legal_mac_leaf_name("foo"s));
    TEST(! is_legal_mac_leaf_name("\x80\xff"s));
    TEST(! is_legal_mac_leaf_name("foo/bar"s));
    TEST(! is_legal_mac_leaf_name("/"s));

    TEST(is_legal_mac_path_name("foo"s));
    TEST(is_legal_mac_path_name("/"s));
    TEST(is_legal_mac_path_name("/foo"s));
    TEST(is_legal_mac_path_name("/foo/bar"s));
    TEST(is_legal_mac_path_name("//foo/bar"s));
    TEST(! is_legal_mac_path_name("//foo/\x80\xff"s));

    TEST(is_legal_posix_leaf_name("foo"s));
    TEST(is_legal_posix_leaf_name("\x80\xff"s));
    TEST(! is_legal_posix_leaf_name("foo/bar"s));
    TEST(! is_legal_posix_leaf_name("/"s));

    TEST(is_legal_posix_path_name("foo"s));
    TEST(is_legal_posix_path_name("/"s));
    TEST(is_legal_posix_path_name("/foo"s));
    TEST(is_legal_posix_path_name("/foo/bar"s));
    TEST(is_legal_posix_path_name("//foo/bar"s));
    TEST(is_legal_posix_path_name("//foo/\x80\xff"s));

    TEST(is_legal_windows_leaf_name(L"foo"s));
    TEST(is_legal_windows_leaf_name(L"\u0080\u00ff"s));
    TEST(! is_legal_windows_leaf_name(L"<foo>"s));
    TEST(! is_legal_windows_leaf_name(L"aux"s));
    TEST(! is_legal_windows_leaf_name(L"aux.txt"s));
    TEST(! is_legal_windows_leaf_name(L"foo\\bar"s));
    TEST(! is_legal_windows_leaf_name(L"C:\\"s));

    TEST(is_legal_windows_path_name(L"foo"s));
    TEST(is_legal_windows_path_name(L"C:\\"s));
    TEST(is_legal_windows_path_name(L"C:\\foo"s));
    TEST(is_legal_windows_path_name(L"C:\\foo/bar"s));
    TEST(is_legal_windows_path_name(L"\\\\foo\\bar"s));
    TEST(is_legal_windows_path_name(L"\\\\foo\\\u0080\u00ff"s));
    TEST(! is_legal_windows_path_name(L"@:\\"s));
    TEST(! is_legal_windows_path_name(L"C:\\<foo>"s));
    TEST(! is_legal_windows_path_name(L"C:\\aux"s));
    TEST(! is_legal_windows_path_name(L"C:\\aux.txt"s));

}

void test_unicorn_file_name_operations() {

    #ifdef _XOPEN_SOURCE

        TEST(! file_is_absolute(""s));
        TEST(file_is_absolute("/"s));
        TEST(! file_is_absolute("foo"s));
        TEST(file_is_absolute("/foo"s));
        TEST(! file_is_absolute("foo/bar"s));
        TEST(file_is_absolute("/foo/bar"s));
        TEST(file_is_absolute("//foo/bar"s));
        TEST(file_is_absolute("//foo/"s));
        TEST(file_is_absolute("//foo"s));

        TEST(! file_is_relative(""s));
        TEST(! file_is_relative("/"s));
        TEST(file_is_relative("foo"s));
        TEST(! file_is_relative("/foo"s));
        TEST(file_is_relative("foo/bar"s));
        TEST(! file_is_relative("/foo/bar"s));
        TEST(! file_is_relative("//foo/bar"s));
        TEST(! file_is_relative("//foo/"s));
        TEST(! file_is_relative("//foo"s));

        TEST(! file_is_root(""s));
        TEST(file_is_root("/"s));
        TEST(! file_is_root("foo"s));
        TEST(! file_is_root("/foo"s));
        TEST(! file_is_root("foo/bar"s));
        TEST(! file_is_root("/foo/bar"s));
        TEST(! file_is_root("//foo/bar"s));
        TEST(file_is_root("//foo/"s));
        TEST(file_is_root("//foo"s));

    #else

        TEST(! file_is_absolute(""s));
        TEST(! file_is_absolute("foo"s));
        TEST(! file_is_absolute("foo\\bar"s));
        TEST(file_is_absolute("C:\\"s));
        TEST(file_is_absolute("C:\\foo"s));
        TEST(! file_is_absolute("C:"s));
        TEST(! file_is_absolute("C:foo"s));
        TEST(! file_is_absolute("\\foo"s));
        TEST(file_is_absolute("\\\\foo\\bar"s));
        TEST(file_is_absolute("\\\\foo\\"s));
        TEST(file_is_absolute("\\\\foo"s));
        TEST(! file_is_absolute("\\\\?\\foo"s));
        TEST(! file_is_absolute("\\\\?\\foo\\bar"s));
        TEST(file_is_absolute("\\\\?\\C:\\"s));
        TEST(file_is_absolute("\\\\?\\C:\\foo"s));
        TEST(! file_is_absolute("\\\\?\\C:"s));
        TEST(! file_is_absolute("\\\\?\\C:foo"s));
        TEST(file_is_absolute("\\\\?\\\\\\foo\\bar"s));
        TEST(file_is_absolute("\\\\?\\\\\\foo\\"s));
        TEST(file_is_absolute("\\\\?\\\\\\foo"s));

        TEST(! file_is_relative(""s));
        TEST(file_is_relative("foo"s));
        TEST(file_is_relative("foo\\bar"s));
        TEST(! file_is_relative("C:\\"s));
        TEST(! file_is_relative("C:\\foo"s));
        TEST(! file_is_relative("C:"s));
        TEST(! file_is_relative("C:foo"s));
        TEST(! file_is_relative("\\foo"s));
        TEST(! file_is_relative("\\\\foo\\bar"s));
        TEST(! file_is_relative("\\\\foo\\"s));
        TEST(! file_is_relative("\\\\foo"s));
        TEST(file_is_relative("\\\\?\\foo"s));
        TEST(file_is_relative("\\\\?\\foo\\bar"s));
        TEST(! file_is_relative("\\\\?\\C:\\"s));
        TEST(! file_is_relative("\\\\?\\C:\\foo"s));
        TEST(file_is_relative("\\\\?\\C:"s));
        TEST(file_is_relative("\\\\?\\C:foo"s));
        TEST(! file_is_relative("\\\\?\\\\\\foo\\bar"s));
        TEST(! file_is_relative("\\\\?\\\\\\foo\\"s));
        TEST(! file_is_relative("\\\\?\\\\\\foo"s));

        TEST(! file_is_drive_absolute(""s));
        TEST(! file_is_drive_absolute("foo"s));
        TEST(! file_is_drive_absolute("foo\\bar"s));
        TEST(! file_is_drive_absolute("C:\\"s));
        TEST(! file_is_drive_absolute("C:\\foo"s));
        TEST(! file_is_drive_absolute("C:"s));
        TEST(! file_is_drive_absolute("C:foo"s));
        TEST(file_is_drive_absolute("\\foo"s));
        TEST(! file_is_drive_absolute("\\\\foo\\bar"s));
        TEST(! file_is_drive_absolute("\\\\foo\\"s));
        TEST(! file_is_drive_absolute("\\\\foo"s));
        TEST(! file_is_drive_absolute("\\\\?\\foo"s));
        TEST(! file_is_drive_absolute("\\\\?\\foo\\bar"s));
        TEST(! file_is_drive_absolute("\\\\?\\C:\\"s));
        TEST(! file_is_drive_absolute("\\\\?\\C:\\foo"s));
        TEST(! file_is_drive_absolute("\\\\?\\C:"s));
        TEST(! file_is_drive_absolute("\\\\?\\C:foo"s));
        TEST(! file_is_drive_absolute("\\\\?\\\\\\foo\\bar"s));
        TEST(! file_is_drive_absolute("\\\\?\\\\\\foo\\"s));
        TEST(! file_is_drive_absolute("\\\\?\\\\\\foo"s));

        TEST(! file_is_drive_relative(""s));
        TEST(! file_is_drive_relative("foo"s));
        TEST(! file_is_drive_relative("foo\\bar"s));
        TEST(! file_is_drive_relative("C:\\"s));
        TEST(! file_is_drive_relative("C:\\foo"s));
        TEST(file_is_drive_relative("C:"s));
        TEST(file_is_drive_relative("C:foo"s));
        TEST(! file_is_drive_relative("\\foo"s));
        TEST(! file_is_drive_relative("\\\\foo\\bar"s));
        TEST(! file_is_drive_relative("\\\\foo\\"s));
        TEST(! file_is_drive_relative("\\\\foo"s));
        TEST(! file_is_drive_relative("\\\\?\\foo"s));
        TEST(! file_is_drive_relative("\\\\?\\foo\\bar"s));
        TEST(! file_is_drive_relative("\\\\?\\C:\\"s));
        TEST(! file_is_drive_relative("\\\\?\\C:\\foo"s));
        TEST(! file_is_drive_relative("\\\\?\\C:"s));
        TEST(! file_is_drive_relative("\\\\?\\C:foo"s));
        TEST(! file_is_drive_relative("\\\\?\\\\\\foo\\bar"s));
        TEST(! file_is_drive_relative("\\\\?\\\\\\foo\\"s));
        TEST(! file_is_drive_relative("\\\\?\\\\\\foo"s));

        TEST(! file_is_root(""s));
        TEST(! file_is_root("foo"s));
        TEST(! file_is_root("foo\\bar"s));
        TEST(file_is_root("C:\\"s));
        TEST(! file_is_root("C:\\foo"s));
        TEST(! file_is_root("C:"s));
        TEST(! file_is_root("C:foo"s));
        TEST(! file_is_root("\\\\foo\\bar"s));
        TEST(file_is_root("\\\\foo\\"s));
        TEST(file_is_root("\\\\foo"s));
        TEST(! file_is_root("\\\\?\\foo"s));
        TEST(! file_is_root("\\\\?\\foo\\bar"s));
        TEST(file_is_root("\\\\?\\C:\\"s));
        TEST(! file_is_root("\\\\?\\C:\\foo"s));
        TEST(! file_is_root("\\\\?\\C:"s));
        TEST(! file_is_root("\\\\?\\C:foo"s));
        TEST(! file_is_root("\\\\?\\\\\\foo\\bar"s));
        TEST(file_is_root("\\\\?\\\\\\foo\\"s));
        TEST(file_is_root("\\\\?\\\\\\foo"s));

    #endif

}

void test_unicorn_file_path_operations() {

    std::pair<Ustring, Ustring> p;

    TRY(p = split_path(""s));           TEST_EQUAL(p.first, "");        TEST_EQUAL(p.second, "");
    TRY(p = split_file(""s));           TEST_EQUAL(p.first, "");        TEST_EQUAL(p.second, "");
    TRY(p = split_path("hello.txt"s));  TEST_EQUAL(p.first, "");        TEST_EQUAL(p.second, "hello.txt");
    TRY(p = split_file("hello.txt"s));  TEST_EQUAL(p.first, "hello");   TEST_EQUAL(p.second, ".txt");
    TRY(p = split_path("hello"s));      TEST_EQUAL(p.first, "");        TEST_EQUAL(p.second, "hello");
    TRY(p = split_file("hello"s));      TEST_EQUAL(p.first, "hello");   TEST_EQUAL(p.second, "");
    TRY(p = split_path(".hello"s));     TEST_EQUAL(p.first, "");        TEST_EQUAL(p.second, ".hello");
    TRY(p = split_file(".hello"s));     TEST_EQUAL(p.first, ".hello");  TEST_EQUAL(p.second, "");

    #ifdef _XOPEN_SOURCE

        TRY(p = split_path("/hello.txt"s));                               TEST_EQUAL(p.first, "/");            TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_path("/hello.txt"s, File::fullname));               TEST_EQUAL(p.first, "/");            TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_file("/hello.txt"s));                               TEST_EQUAL(p.first, "hello");        TEST_EQUAL(p.second, ".txt");
        TRY(p = split_path("abc/def/hello.txt"s));                        TEST_EQUAL(p.first, "abc/def");      TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_path("abc/def/hello.txt"s, File::fullname));        TEST_EQUAL(p.first, "abc/def/");     TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_file("abc/def/hello.txt"s));                        TEST_EQUAL(p.first, "hello");        TEST_EQUAL(p.second, ".txt");
        TRY(p = split_path("abc/def/hello"s));                            TEST_EQUAL(p.first, "abc/def");      TEST_EQUAL(p.second, "hello");
        TRY(p = split_path("abc/def/hello"s, File::fullname));            TEST_EQUAL(p.first, "abc/def/");     TEST_EQUAL(p.second, "hello");
        TRY(p = split_file("abc/def/hello"s));                            TEST_EQUAL(p.first, "hello");        TEST_EQUAL(p.second, "");
        TRY(p = split_path("abc/def/hello.world.txt"s));                  TEST_EQUAL(p.first, "abc/def");      TEST_EQUAL(p.second, "hello.world.txt");
        TRY(p = split_path("abc/def/hello.world.txt"s, File::fullname));  TEST_EQUAL(p.first, "abc/def/");     TEST_EQUAL(p.second, "hello.world.txt");
        TRY(p = split_file("abc/def/hello.world.txt"s));                  TEST_EQUAL(p.first, "hello.world");  TEST_EQUAL(p.second, ".txt");
        TRY(p = split_path("abc/def/.hello"s));                           TEST_EQUAL(p.first, "abc/def");      TEST_EQUAL(p.second, ".hello");
        TRY(p = split_path("abc/def/.hello"s, File::fullname));           TEST_EQUAL(p.first, "abc/def/");     TEST_EQUAL(p.second, ".hello");
        TRY(p = split_file("abc/def/.hello"s));                           TEST_EQUAL(p.first, ".hello");       TEST_EQUAL(p.second, "");
        TRY(p = split_path("abc/def/.hello.txt"s));                       TEST_EQUAL(p.first, "abc/def");      TEST_EQUAL(p.second, ".hello.txt");
        TRY(p = split_path("abc/def/.hello.txt"s, File::fullname));       TEST_EQUAL(p.first, "abc/def/");     TEST_EQUAL(p.second, ".hello.txt");
        TRY(p = split_file("abc/def/.hello.txt"s));                       TEST_EQUAL(p.first, ".hello");       TEST_EQUAL(p.second, ".txt");

        TEST_EQUAL(file_path(""), "");
        TEST_EQUAL(file_path("hello.txt"), "hello.txt");
        TEST_EQUAL(file_path("abc/def/hello.txt"), "abc/def/hello.txt");
        TEST_EQUAL(file_path("", ""), "");
        TEST_EQUAL(file_path("hello", ""), "hello");
        TEST_EQUAL(file_path("", "world"), "world");
        TEST_EQUAL(file_path("hello", "world"), "hello/world");
        TEST_EQUAL(file_path("hello", "world/"), "hello/world/");
        TEST_EQUAL(file_path("hello", "/world"), "/world");
        TEST_EQUAL(file_path("hello", "/world/"), "/world/");
        TEST_EQUAL(file_path("hello/", "world"), "hello/world");
        TEST_EQUAL(file_path("hello/", "world/"), "hello/world/");
        TEST_EQUAL(file_path("hello/", "/world"), "/world");
        TEST_EQUAL(file_path("hello/", "/world/"), "/world/");
        TEST_EQUAL(file_path("/hello", "world"), "/hello/world");
        TEST_EQUAL(file_path("/hello", "world/"), "/hello/world/");
        TEST_EQUAL(file_path("/hello", "/world"), "/world");
        TEST_EQUAL(file_path("/hello", "/world/"), "/world/");
        TEST_EQUAL(file_path("/hello/", "world"), "/hello/world");
        TEST_EQUAL(file_path("/hello/", "world/"), "/hello/world/");
        TEST_EQUAL(file_path("/hello/", "/world"), "/world");
        TEST_EQUAL(file_path("/hello/", "/world/"), "/world/");

    #else

        TRY(p = split_path("C:\\hello.txt"s));                              TEST_EQUAL(p.first, "C:\\");         TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_path("C:\\hello.txt"s, File::fullname));              TEST_EQUAL(p.first, "C:\\");         TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_file("C:\\hello.txt"s));                              TEST_EQUAL(p.first, "hello");        TEST_EQUAL(p.second, ".txt");
        TRY(p = split_path("C:hello.txt"s));                                TEST_EQUAL(p.first, "C:");           TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_path("C:hello.txt"s, File::fullname));                TEST_EQUAL(p.first, "C:");           TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_file("C:hello.txt"s));                                TEST_EQUAL(p.first, "hello");        TEST_EQUAL(p.second, ".txt");
        TRY(p = split_path("abc\\def\\hello.txt"s));                        TEST_EQUAL(p.first, "abc\\def");     TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_path("abc\\def\\hello.txt"s, File::fullname));        TEST_EQUAL(p.first, "abc\\def\\");   TEST_EQUAL(p.second, "hello.txt");
        TRY(p = split_file("abc\\def\\hello.txt"s));                        TEST_EQUAL(p.first, "hello");        TEST_EQUAL(p.second, ".txt");
        TRY(p = split_path("abc\\def\\hello"s));                            TEST_EQUAL(p.first, "abc\\def");     TEST_EQUAL(p.second, "hello");
        TRY(p = split_path("abc\\def\\hello"s, File::fullname));            TEST_EQUAL(p.first, "abc\\def\\");   TEST_EQUAL(p.second, "hello");
        TRY(p = split_file("abc\\def\\hello"s));                            TEST_EQUAL(p.first, "hello");        TEST_EQUAL(p.second, "");
        TRY(p = split_path("abc\\def\\hello.world.txt"s));                  TEST_EQUAL(p.first, "abc\\def");     TEST_EQUAL(p.second, "hello.world.txt");
        TRY(p = split_path("abc\\def\\hello.world.txt"s, File::fullname));  TEST_EQUAL(p.first, "abc\\def\\");   TEST_EQUAL(p.second, "hello.world.txt");
        TRY(p = split_file("abc\\def\\hello.world.txt"s));                  TEST_EQUAL(p.first, "hello.world");  TEST_EQUAL(p.second, ".txt");
        TRY(p = split_path("abc\\def\\.hello"s));                           TEST_EQUAL(p.first, "abc\\def");     TEST_EQUAL(p.second, ".hello");
        TRY(p = split_path("abc\\def\\.hello"s, File::fullname));           TEST_EQUAL(p.first, "abc\\def\\");   TEST_EQUAL(p.second, ".hello");
        TRY(p = split_file("abc\\def\\.hello"s));                           TEST_EQUAL(p.first, ".hello");       TEST_EQUAL(p.second, "");
        TRY(p = split_path("abc\\def\\.hello.txt"s));                       TEST_EQUAL(p.first, "abc\\def");     TEST_EQUAL(p.second, ".hello.txt");
        TRY(p = split_path("abc\\def\\.hello.txt"s, File::fullname));       TEST_EQUAL(p.first, "abc\\def\\");   TEST_EQUAL(p.second, ".hello.txt");
        TRY(p = split_file("abc\\def\\.hello.txt"s));                       TEST_EQUAL(p.first, ".hello");       TEST_EQUAL(p.second, ".txt");

        TEST_EQUAL(file_path(""), "");
        TEST_EQUAL(file_path("hello.txt"), "hello.txt");
        TEST_EQUAL(file_path("abc\\def\\hello.txt"), "abc\\def\\hello.txt");
        TEST_EQUAL(file_path("", ""), "");
        TEST_EQUAL(file_path("hello", ""), "hello");
        TEST_EQUAL(file_path("", "world"), "world");
        TEST_EQUAL(file_path("hello", "world"), "hello\\world");
        TEST_EQUAL(file_path("hello", "world\\"), "hello\\world\\");
        TEST_EQUAL(file_path("hello", "C:\\world"), "C:\\world");
        TEST_EQUAL(file_path("hello", "C:\\world\\"), "C:\\world\\");
        TEST_EQUAL(file_path("hello\\", "world"), "hello\\world");
        TEST_EQUAL(file_path("hello\\", "world\\"), "hello\\world\\");
        TEST_EQUAL(file_path("hello\\", "C:\\world"), "C:\\world");
        TEST_EQUAL(file_path("hello\\", "C:\\world\\"), "C:\\world\\");
        TEST_EQUAL(file_path("C:\\hello", "world"), "C:\\hello\\world");
        TEST_EQUAL(file_path("C:\\hello", "world\\"), "C:\\hello\\world\\");
        TEST_EQUAL(file_path("C:\\hello", "C:\\world"), "C:\\world");
        TEST_EQUAL(file_path("C:\\hello", "C:\\world\\"), "C:\\world\\");
        TEST_EQUAL(file_path("C:\\hello\\", "world"), "C:\\hello\\world");
        TEST_EQUAL(file_path("C:\\hello\\", "world\\"), "C:\\hello\\world\\");
        TEST_EQUAL(file_path("C:\\hello\\", "C:\\world"), "C:\\world");
        TEST_EQUAL(file_path("C:\\hello\\", "C:\\world\\"), "C:\\world\\");

    #endif

}

void test_unicorn_file_path_resolution() {

    Ustring cwd = current_directory(), res;

    #ifdef _XOPEN_SOURCE
        Ustring home = cstr(getenv("HOME"));
    #endif

    std::vector<std::pair<Ustring, Ustring>> tests = {
        #ifdef _XOPEN_SOURCE
            {  "",             "",                   },
            {  "Makefile",     cwd + "/Makefile",    },
            {  ".",            cwd,                  },
            {  "./Makefile",   cwd + "/Makefile",    },
            {  "~",            home,                 },
            {  "~/Documents",  home + "/Documents",  },
            {  "/",            "/",                  },
            {  "/usr",         "/usr",               },
            {  "/usr/local",   "/usr/local",         },
        #else
            {  "",             "",                  },
            {  "Makefile",     cwd + "\\Makefile",  },
            {  ".",            cwd,                 },
            {  ".\\Makefile",  cwd + "\\Makefile",  },
            {  "C:\\",         "C:\\",              },
            {  "C:\\Windows",  "C:\\Windows",       },
        #endif
    };

    for (auto& test: tests) {
        TRY(res = resolve_path(test.first));
        TEST_EQUAL(res, test.second);
    }

}

void test_unicorn_file_system_operations() {

    Ustring d1, d2, d3, f1, f2, f3, f4;
    Strings vec;
    FileId id1 = {}, id2 = {};
    size_t size1 = 0, size2 = 0;

    TEST(file_exists("."s));
    TEST(file_exists(".."s));
    TEST(file_exists("Makefile"s));
    TEST(file_exists("unicorn"s));
    TEST(! file_exists(""s));
    TEST(! file_exists("no such file"s));

    TEST_EQUAL(file_id(""s), FileId());
    TEST_EQUAL(file_id("no such file"s), FileId());
    TRY(id1 = file_id("Makefile"s));
    TRY(id2 = file_id("unicorn"s));
    TEST_COMPARE(id1, >, FileId());
    TEST_COMPARE(id2, >, FileId());
    TEST_COMPARE(id1, !=, id2);

    TEST(file_is_directory("."s));
    TEST(file_is_directory(".."s));
    TEST(file_is_directory("unicorn"s));
    TEST(! file_is_directory("Makefile"s));
    TEST(! file_is_directory(""s));
    TEST(! file_is_directory("no such file"s));
    #ifdef _XOPEN_SOURCE
        TEST(file_is_directory("/"s));
    #else
        TEST(file_is_directory("C:\\"s));
    #endif

    TEST(! file_is_hidden("."s));
    TEST(! file_is_hidden(".."s));
    TEST(! file_is_hidden("unicorn"s));
    TEST(! file_is_hidden("Makefile"s));
    TEST(! file_is_hidden("no such file"s));
    #ifdef _XOPEN_SOURCE
        TEST(! file_is_hidden("/"s));
    #else
        TEST(! file_is_hidden("C:\\"s));
    #endif

    TEST(! file_is_symlink("."s));
    TEST(! file_is_symlink(".."s));
    TEST(! file_is_symlink("unicorn"s));
    TEST(! file_is_symlink("Makefile"s));
    TEST(! file_is_symlink("no such file"s));
    #ifdef _XOPEN_SOURCE
        TEST(! file_is_symlink("/"s));
    #else
        TEST(! file_is_symlink("C:\\"s));
    #endif

    TRY(size1 = file_size("Makefile"s));
    TRY(size2 = file_size("Makefile"s, File::recurse));
    TEST_COMPARE(size1, >, 50);
    TEST_COMPARE(size2, >, 50);
    TEST_EQUAL(size1, size2);
    TEST_EQUAL(file_size("no such file"s), 0);
    TEST_EQUAL(file_size("no such file"s, File::recurse), 0);
    TEST_COMPARE(file_size("."s, File::recurse), >, 65000000);
    TEST_COMPARE(file_size("unicorn"s, File::recurse), >, 4000000);

    f1 = "__test_file_1";
    f2 = "__test_file_2";
    f3 = "__test_file_3";
    f4 = "__test_FILE_3";
    TEST(! file_exists(f1));
    TEST(! file_is_directory(f1));
    TEST(! file_exists(f2));
    TEST(! file_is_directory(f2));
    TRY(touch(f1));
    TEST(file_exists(f1));
    TRY(move_file(f1, f2));
    TEST(! file_exists(f1));
    TEST(file_exists(f2));
    TRY(copy_file(f2, f3));
    TEST(file_exists(f2));
    TEST(file_exists(f3));
    TRY(move_file(f3, f4));
    TEST(file_exists(f4));
    TRY(remove_file(f1));
    TRY(remove_file(f2));
    TRY(remove_file(f3));
    TRY(remove_file(f4));
    TEST(! file_exists(f1));
    TEST(! file_exists(f2));
    TEST(! file_exists(f3));
    TEST(! file_exists(f4));

    d1 = "__test_dir_1";
    d2 = "__test_dir_2";
    d3 = "__test_dir_3";
    TEST(! file_exists(d1));
    TEST(! file_is_directory(d1));
    TEST(! file_exists(d2));
    TEST(! file_is_directory(d2));
    TRY(make_directory(d1));
    TEST(file_exists(d1));
    TEST(file_is_directory(d1));
    TRY(make_directory(d1));
    TRY(move_file(d1, d2));
    TEST(! file_exists(d1));
    TEST(! file_is_directory(d1));
    TEST(file_exists(d2));
    TEST(file_is_directory(d2));
    TEST_THROW(copy_file(d2, d3), std::system_error);
    TRY(copy_file(d2, d3, File::recurse));
    TEST(file_exists(d2));
    TEST(file_is_directory(d2));
    TEST(file_exists(d3));
    TEST(file_is_directory(d3));
    TRY(remove_file(d1));
    TRY(remove_file(d2));
    TRY(remove_file(d3));
    TEST(! file_exists(d1));
    TEST(! file_is_directory(d1));
    TEST(! file_exists(d2));
    TEST(! file_is_directory(d2));
    TEST(! file_exists(d3));
    TEST(! file_is_directory(d3));

    d1 = "__test_dir_1";
    d2 = "__test_dir_1/test_dir_2";
    TEST(! file_exists(d1));
    TEST(! file_exists(d2));
    TEST_THROW(make_directory(d2), std::system_error);
    TRY(make_directory(d2, File::recurse));
    TEST(file_exists(d1));
    TEST(file_exists(d2));
    TEST(file_is_directory(d1));
    TEST(file_is_directory(d2));
    TRY(make_directory(d2, File::recurse));
    TEST_THROW(remove_file(d1), std::system_error);
    TRY(remove_file(d1, File::recurse));
    TEST(! file_exists(d1));
    TEST(! file_exists(d2));
    TRY(remove_file(d1, File::recurse));

    d1 = "__test_dir_1";
    auto dir = directory(d1);
    f1 = file_path(d1, "hello");
    f2 = file_path(d1, "world");
    TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 0);
    TEST_EQUAL(RS::UnitTest::format_range(vec), "[]");
    TRY(make_directory(d1));
    TEST(file_exists(d1));
    TEST(file_is_directory(d1));
    TRY(dir = directory(d1));
    TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 0);
    TEST_EQUAL(RS::UnitTest::format_range(vec), "[]");
    TRY(touch(f1));
    TEST(file_exists(f1));
    TRY(touch(f2));
    TEST(file_exists(f2));
    TEST_THROW(make_directory(f1), std::system_error);
    TRY(dir = directory(d1));
    TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    std::sort(vec.begin(), vec.end());
    TEST_EQUAL(RS::UnitTest::format_range(vec), "[hello,world]"s);
    TRY(dir = directory(d1, File::dotdot));
    TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 4);
    std::sort(vec.begin(), vec.end());
    TEST_EQUAL(RS::UnitTest::format_range(vec), "[.,..,hello,world]"s);
    TRY(dir = directory(d1, File::fullname));
    TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    std::sort(vec.begin(), vec.end());
    TEST_EQUAL(RS::UnitTest::format_range(vec), "[__test_dir_1" SLASH "hello,__test_dir_1" SLASH "world]"s);
    TEST_THROW(remove_file(d1), std::system_error);
    TEST(file_exists(d1));
    TEST(file_exists(f1));
    TEST(file_exists(f2));
    TRY(remove_file(d1, File::recurse));
    TEST(! file_exists(d1));
    TEST(! file_exists(f1));
    TEST(! file_exists(f2));
    TRY(dir = directory(d1));
    TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 0);
    TEST_EQUAL(RS::UnitTest::format_range(vec), "[]");

}
