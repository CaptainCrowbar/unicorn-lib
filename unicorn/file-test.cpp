#include "unicorn/core.hpp"
#include "unicorn/file.hpp"
#include "unicorn/utf.hpp"
#include "prion/unit-test.hpp"
#include <algorithm>
#include <fstream>
#include <string>
#include <system_error>
#include <utility>

using namespace std::literals;
using namespace Unicorn;

#if defined(PRI_TARGET_UNIX)
    #define SLASH "/"
#else
    #define SLASH "\\\\"
#endif

namespace {

    using UnicornDetail::normalize_file;

    template <typename C>
    void touch(const basic_string<C>& file) {
        std::ofstream(to_utf8(file));
    }

    void check_legal_file_names() {

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

    void check_file_name_operations() {

        #if defined(PRI_TARGET_UNIX)

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

            TEST(! file_is_absolute(u""s));
            TEST(file_is_absolute(u"/"s));
            TEST(! file_is_absolute(u"foo"s));
            TEST(file_is_absolute(u"/foo"s));
            TEST(! file_is_absolute(u"foo/bar"s));
            TEST(file_is_absolute(u"/foo/bar"s));
            TEST(file_is_absolute(u"//foo/bar"s));
            TEST(file_is_absolute(u"//foo/"s));
            TEST(file_is_absolute(u"//foo"s));

            TEST(! file_is_relative(u""s));
            TEST(! file_is_relative(u"/"s));
            TEST(file_is_relative(u"foo"s));
            TEST(! file_is_relative(u"/foo"s));
            TEST(file_is_relative(u"foo/bar"s));
            TEST(! file_is_relative(u"/foo/bar"s));
            TEST(! file_is_relative(u"//foo/bar"s));
            TEST(! file_is_relative(u"//foo/"s));
            TEST(! file_is_relative(u"//foo"s));

            TEST(! file_is_absolute(U""s));
            TEST(file_is_absolute(U"/"s));
            TEST(! file_is_absolute(U"foo"s));
            TEST(file_is_absolute(U"/foo"s));
            TEST(! file_is_absolute(U"foo/bar"s));
            TEST(file_is_absolute(U"/foo/bar"s));
            TEST(file_is_absolute(U"//foo/bar"s));
            TEST(file_is_absolute(U"//foo/"s));
            TEST(file_is_absolute(U"//foo"s));

            TEST(! file_is_relative(U""s));
            TEST(! file_is_relative(U"/"s));
            TEST(file_is_relative(U"foo"s));
            TEST(! file_is_relative(U"/foo"s));
            TEST(file_is_relative(U"foo/bar"s));
            TEST(! file_is_relative(U"/foo/bar"s));
            TEST(! file_is_relative(U"//foo/bar"s));
            TEST(! file_is_relative(U"//foo/"s));
            TEST(! file_is_relative(U"//foo"s));

            TEST(! file_is_root(""s));
            TEST(file_is_root("/"s));
            TEST(! file_is_root("foo"s));
            TEST(! file_is_root("/foo"s));
            TEST(! file_is_root("foo/bar"s));
            TEST(! file_is_root("/foo/bar"s));
            TEST(! file_is_root("//foo/bar"s));
            TEST(file_is_root("//foo/"s));
            TEST(file_is_root("//foo"s));

            TEST(! file_is_root(u""s));
            TEST(file_is_root(u"/"s));
            TEST(! file_is_root(u"foo"s));
            TEST(! file_is_root(u"/foo"s));
            TEST(! file_is_root(u"foo/bar"s));
            TEST(! file_is_root(u"/foo/bar"s));
            TEST(! file_is_root(u"//foo/bar"s));
            TEST(file_is_root(u"//foo/"s));
            TEST(file_is_root(u"//foo"s));

            TEST(! file_is_root(U""s));
            TEST(file_is_root(U"/"s));
            TEST(! file_is_root(U"foo"s));
            TEST(! file_is_root(U"/foo"s));
            TEST(! file_is_root(U"foo/bar"s));
            TEST(! file_is_root(U"/foo/bar"s));
            TEST(! file_is_root(U"//foo/bar"s));
            TEST(file_is_root(U"//foo/"s));
            TEST(file_is_root(U"//foo"s));

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

            TEST(! file_is_absolute(u""s));
            TEST(! file_is_absolute(u"foo"s));
            TEST(! file_is_absolute(u"foo\\bar"s));
            TEST(file_is_absolute(u"C:\\"s));
            TEST(file_is_absolute(u"C:\\foo"s));
            TEST(! file_is_absolute(u"C:"s));
            TEST(! file_is_absolute(u"C:foo"s));
            TEST(! file_is_absolute(u"\\foo"s));
            TEST(file_is_absolute(u"\\\\foo\\bar"s));
            TEST(file_is_absolute(u"\\\\foo\\"s));
            TEST(file_is_absolute(u"\\\\foo"s));
            TEST(! file_is_absolute(u"\\\\?\\foo"s));
            TEST(! file_is_absolute(u"\\\\?\\foo\\bar"s));
            TEST(file_is_absolute(u"\\\\?\\C:\\"s));
            TEST(file_is_absolute(u"\\\\?\\C:\\foo"s));
            TEST(! file_is_absolute(u"\\\\?\\C:"s));
            TEST(! file_is_absolute(u"\\\\?\\C:foo"s));
            TEST(file_is_absolute(u"\\\\?\\\\\\foo\\bar"s));
            TEST(file_is_absolute(u"\\\\?\\\\\\foo\\"s));
            TEST(file_is_absolute(u"\\\\?\\\\\\foo"s));

            TEST(! file_is_relative(u""s));
            TEST(file_is_relative(u"foo"s));
            TEST(file_is_relative(u"foo\\bar"s));
            TEST(! file_is_relative(u"C:\\"s));
            TEST(! file_is_relative(u"C:\\foo"s));
            TEST(! file_is_relative(u"C:"s));
            TEST(! file_is_relative(u"C:foo"s));
            TEST(! file_is_relative(u"\\foo"s));
            TEST(! file_is_relative(u"\\\\foo\\bar"s));
            TEST(! file_is_relative(u"\\\\foo\\"s));
            TEST(! file_is_relative(u"\\\\foo"s));
            TEST(file_is_relative(u"\\\\?\\foo"s));
            TEST(file_is_relative(u"\\\\?\\foo\\bar"s));
            TEST(! file_is_relative(u"\\\\?\\C:\\"s));
            TEST(! file_is_relative(u"\\\\?\\C:\\foo"s));
            TEST(file_is_relative(u"\\\\?\\C:"s));
            TEST(file_is_relative(u"\\\\?\\C:foo"s));
            TEST(! file_is_relative(u"\\\\?\\\\\\foo\\bar"s));
            TEST(! file_is_relative(u"\\\\?\\\\\\foo\\"s));
            TEST(! file_is_relative(u"\\\\?\\\\\\foo"s));

            TEST(! file_is_drive_absolute(u""s));
            TEST(! file_is_drive_absolute(u"foo"s));
            TEST(! file_is_drive_absolute(u"foo\\bar"s));
            TEST(! file_is_drive_absolute(u"C:\\"s));
            TEST(! file_is_drive_absolute(u"C:\\foo"s));
            TEST(! file_is_drive_absolute(u"C:"s));
            TEST(! file_is_drive_absolute(u"C:foo"s));
            TEST(file_is_drive_absolute(u"\\foo"s));
            TEST(! file_is_drive_absolute(u"\\\\foo\\bar"s));
            TEST(! file_is_drive_absolute(u"\\\\foo\\"s));
            TEST(! file_is_drive_absolute(u"\\\\foo"s));
            TEST(! file_is_drive_absolute(u"\\\\?\\foo"s));
            TEST(! file_is_drive_absolute(u"\\\\?\\foo\\bar"s));
            TEST(! file_is_drive_absolute(u"\\\\?\\C:\\"s));
            TEST(! file_is_drive_absolute(u"\\\\?\\C:\\foo"s));
            TEST(! file_is_drive_absolute(u"\\\\?\\C:"s));
            TEST(! file_is_drive_absolute(u"\\\\?\\C:foo"s));
            TEST(! file_is_drive_absolute(u"\\\\?\\\\\\foo\\bar"s));
            TEST(! file_is_drive_absolute(u"\\\\?\\\\\\foo\\"s));
            TEST(! file_is_drive_absolute(u"\\\\?\\\\\\foo"s));

            TEST(! file_is_drive_relative(u""s));
            TEST(! file_is_drive_relative(u"foo"s));
            TEST(! file_is_drive_relative(u"foo\\bar"s));
            TEST(! file_is_drive_relative(u"C:\\"s));
            TEST(! file_is_drive_relative(u"C:\\foo"s));
            TEST(file_is_drive_relative(u"C:"s));
            TEST(file_is_drive_relative(u"C:foo"s));
            TEST(! file_is_drive_relative(u"\\foo"s));
            TEST(! file_is_drive_relative(u"\\\\foo\\bar"s));
            TEST(! file_is_drive_relative(u"\\\\foo\\"s));
            TEST(! file_is_drive_relative(u"\\\\foo"s));
            TEST(! file_is_drive_relative(u"\\\\?\\foo"s));
            TEST(! file_is_drive_relative(u"\\\\?\\foo\\bar"s));
            TEST(! file_is_drive_relative(u"\\\\?\\C:\\"s));
            TEST(! file_is_drive_relative(u"\\\\?\\C:\\foo"s));
            TEST(! file_is_drive_relative(u"\\\\?\\C:"s));
            TEST(! file_is_drive_relative(u"\\\\?\\C:foo"s));
            TEST(! file_is_drive_relative(u"\\\\?\\\\\\foo\\bar"s));
            TEST(! file_is_drive_relative(u"\\\\?\\\\\\foo\\"s));
            TEST(! file_is_drive_relative(u"\\\\?\\\\\\foo"s));

            TEST(! file_is_absolute(U""s));
            TEST(! file_is_absolute(U"foo"s));
            TEST(! file_is_absolute(U"foo\\bar"s));
            TEST(file_is_absolute(U"C:\\"s));
            TEST(file_is_absolute(U"C:\\foo"s));
            TEST(! file_is_absolute(U"C:"s));
            TEST(! file_is_absolute(U"C:foo"s));
            TEST(! file_is_absolute(U"\\foo"s));
            TEST(file_is_absolute(U"\\\\foo\\bar"s));
            TEST(file_is_absolute(U"\\\\foo\\"s));
            TEST(file_is_absolute(U"\\\\foo"s));
            TEST(! file_is_absolute(U"\\\\?\\foo"s));
            TEST(! file_is_absolute(U"\\\\?\\foo\\bar"s));
            TEST(file_is_absolute(U"\\\\?\\C:\\"s));
            TEST(file_is_absolute(U"\\\\?\\C:\\foo"s));
            TEST(! file_is_absolute(U"\\\\?\\C:"s));
            TEST(! file_is_absolute(U"\\\\?\\C:foo"s));
            TEST(file_is_absolute(U"\\\\?\\\\\\foo\\bar"s));
            TEST(file_is_absolute(U"\\\\?\\\\\\foo\\"s));
            TEST(file_is_absolute(U"\\\\?\\\\\\foo"s));

            TEST(! file_is_relative(U""s));
            TEST(file_is_relative(U"foo"s));
            TEST(file_is_relative(U"foo\\bar"s));
            TEST(! file_is_relative(U"C:\\"s));
            TEST(! file_is_relative(U"C:\\foo"s));
            TEST(! file_is_relative(U"C:"s));
            TEST(! file_is_relative(U"C:foo"s));
            TEST(! file_is_relative(U"\\foo"s));
            TEST(! file_is_relative(U"\\\\foo\\bar"s));
            TEST(! file_is_relative(U"\\\\foo\\"s));
            TEST(! file_is_relative(U"\\\\foo"s));
            TEST(file_is_relative(U"\\\\?\\foo"s));
            TEST(file_is_relative(U"\\\\?\\foo\\bar"s));
            TEST(! file_is_relative(U"\\\\?\\C:\\"s));
            TEST(! file_is_relative(U"\\\\?\\C:\\foo"s));
            TEST(file_is_relative(U"\\\\?\\C:"s));
            TEST(file_is_relative(U"\\\\?\\C:foo"s));
            TEST(! file_is_relative(U"\\\\?\\\\\\foo\\bar"s));
            TEST(! file_is_relative(U"\\\\?\\\\\\foo\\"s));
            TEST(! file_is_relative(U"\\\\?\\\\\\foo"s));

            TEST(! file_is_drive_absolute(U""s));
            TEST(! file_is_drive_absolute(U"foo"s));
            TEST(! file_is_drive_absolute(U"foo\\bar"s));
            TEST(! file_is_drive_absolute(U"C:\\"s));
            TEST(! file_is_drive_absolute(U"C:\\foo"s));
            TEST(! file_is_drive_absolute(U"C:"s));
            TEST(! file_is_drive_absolute(U"C:foo"s));
            TEST(file_is_drive_absolute(U"\\foo"s));
            TEST(! file_is_drive_absolute(U"\\\\foo\\bar"s));
            TEST(! file_is_drive_absolute(U"\\\\foo\\"s));
            TEST(! file_is_drive_absolute(U"\\\\foo"s));
            TEST(! file_is_drive_absolute(U"\\\\?\\foo"s));
            TEST(! file_is_drive_absolute(U"\\\\?\\foo\\bar"s));
            TEST(! file_is_drive_absolute(U"\\\\?\\C:\\"s));
            TEST(! file_is_drive_absolute(U"\\\\?\\C:\\foo"s));
            TEST(! file_is_drive_absolute(U"\\\\?\\C:"s));
            TEST(! file_is_drive_absolute(U"\\\\?\\C:foo"s));
            TEST(! file_is_drive_absolute(U"\\\\?\\\\\\foo\\bar"s));
            TEST(! file_is_drive_absolute(U"\\\\?\\\\\\foo\\"s));
            TEST(! file_is_drive_absolute(U"\\\\?\\\\\\foo"s));

            TEST(! file_is_drive_relative(U""s));
            TEST(! file_is_drive_relative(U"foo"s));
            TEST(! file_is_drive_relative(U"foo\\bar"s));
            TEST(! file_is_drive_relative(U"C:\\"s));
            TEST(! file_is_drive_relative(U"C:\\foo"s));
            TEST(file_is_drive_relative(U"C:"s));
            TEST(file_is_drive_relative(U"C:foo"s));
            TEST(! file_is_drive_relative(U"\\foo"s));
            TEST(! file_is_drive_relative(U"\\\\foo\\bar"s));
            TEST(! file_is_drive_relative(U"\\\\foo\\"s));
            TEST(! file_is_drive_relative(U"\\\\foo"s));
            TEST(! file_is_drive_relative(U"\\\\?\\foo"s));
            TEST(! file_is_drive_relative(U"\\\\?\\foo\\bar"s));
            TEST(! file_is_drive_relative(U"\\\\?\\C:\\"s));
            TEST(! file_is_drive_relative(U"\\\\?\\C:\\foo"s));
            TEST(! file_is_drive_relative(U"\\\\?\\C:"s));
            TEST(! file_is_drive_relative(U"\\\\?\\C:foo"s));
            TEST(! file_is_drive_relative(U"\\\\?\\\\\\foo\\bar"s));
            TEST(! file_is_drive_relative(U"\\\\?\\\\\\foo\\"s));
            TEST(! file_is_drive_relative(U"\\\\?\\\\\\foo"s));

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

            TEST(! file_is_root(u""s));
            TEST(! file_is_root(u"foo"s));
            TEST(! file_is_root(u"foo\\bar"s));
            TEST(file_is_root(u"C:\\"s));
            TEST(! file_is_root(u"C:\\foo"s));
            TEST(! file_is_root(u"C:"s));
            TEST(! file_is_root(u"C:foo"s));
            TEST(! file_is_root(u"\\\\foo\\bar"s));
            TEST(file_is_root(u"\\\\foo\\"s));
            TEST(file_is_root(u"\\\\foo"s));
            TEST(! file_is_root(u"\\\\?\\foo"s));
            TEST(! file_is_root(u"\\\\?\\foo\\bar"s));
            TEST(file_is_root(u"\\\\?\\C:\\"s));
            TEST(! file_is_root(u"\\\\?\\C:\\foo"s));
            TEST(! file_is_root(u"\\\\?\\C:"s));
            TEST(! file_is_root(u"\\\\?\\C:foo"s));
            TEST(! file_is_root(u"\\\\?\\\\\\foo\\bar"s));
            TEST(file_is_root(u"\\\\?\\\\\\foo\\"s));
            TEST(file_is_root(u"\\\\?\\\\\\foo"s));

            TEST(! file_is_root(U""s));
            TEST(! file_is_root(U"foo"s));
            TEST(! file_is_root(U"foo\\bar"s));
            TEST(file_is_root(U"C:\\"s));
            TEST(! file_is_root(U"C:\\foo"s));
            TEST(! file_is_root(U"C:"s));
            TEST(! file_is_root(U"C:foo"s));
            TEST(! file_is_root(U"\\\\foo\\bar"s));
            TEST(file_is_root(U"\\\\foo\\"s));
            TEST(file_is_root(U"\\\\foo"s));
            TEST(! file_is_root(U"\\\\?\\foo"s));
            TEST(! file_is_root(U"\\\\?\\foo\\bar"s));
            TEST(file_is_root(U"\\\\?\\C:\\"s));
            TEST(! file_is_root(U"\\\\?\\C:\\foo"s));
            TEST(! file_is_root(U"\\\\?\\C:"s));
            TEST(! file_is_root(U"\\\\?\\C:foo"s));
            TEST(! file_is_root(U"\\\\?\\\\\\foo\\bar"s));
            TEST(file_is_root(U"\\\\?\\\\\\foo\\"s));
            TEST(file_is_root(U"\\\\?\\\\\\foo"s));

        #endif

    }

    void check_file_path_operations() {

        pair<u8string, u8string> p8;
        pair<u16string, u16string> p16;
        pair<u32string, u32string> p32;

        TRY(p8 = split_path(""s));           TEST_EQUAL(p8.first, "");        TEST_EQUAL(p8.second, "");
        TRY(p8 = split_file(""s));           TEST_EQUAL(p8.first, "");        TEST_EQUAL(p8.second, "");
        TRY(p8 = split_path("hello.txt"s));  TEST_EQUAL(p8.first, "");        TEST_EQUAL(p8.second, "hello.txt");
        TRY(p8 = split_file("hello.txt"s));  TEST_EQUAL(p8.first, "hello");   TEST_EQUAL(p8.second, ".txt");
        TRY(p8 = split_path("hello"s));      TEST_EQUAL(p8.first, "");        TEST_EQUAL(p8.second, "hello");
        TRY(p8 = split_file("hello"s));      TEST_EQUAL(p8.first, "hello");   TEST_EQUAL(p8.second, "");
        TRY(p8 = split_path(".hello"s));     TEST_EQUAL(p8.first, "");        TEST_EQUAL(p8.second, ".hello");
        TRY(p8 = split_file(".hello"s));     TEST_EQUAL(p8.first, ".hello");  TEST_EQUAL(p8.second, "");

        TRY(p16 = split_path(u""s));           TEST_EQUAL(p16.first, u"");        TEST_EQUAL(p16.second, u"");
        TRY(p16 = split_file(u""s));           TEST_EQUAL(p16.first, u"");        TEST_EQUAL(p16.second, u"");
        TRY(p16 = split_path(u"hello.txt"s));  TEST_EQUAL(p16.first, u"");        TEST_EQUAL(p16.second, u"hello.txt");
        TRY(p16 = split_file(u"hello.txt"s));  TEST_EQUAL(p16.first, u"hello");   TEST_EQUAL(p16.second, u".txt");
        TRY(p16 = split_path(u"hello"s));      TEST_EQUAL(p16.first, u"");        TEST_EQUAL(p16.second, u"hello");
        TRY(p16 = split_file(u"hello"s));      TEST_EQUAL(p16.first, u"hello");   TEST_EQUAL(p16.second, u"");
        TRY(p16 = split_path(u".hello"s));     TEST_EQUAL(p16.first, u"");        TEST_EQUAL(p16.second, u".hello");
        TRY(p16 = split_file(u".hello"s));     TEST_EQUAL(p16.first, u".hello");  TEST_EQUAL(p16.second, u"");

        TRY(p32 = split_path(U""s));           TEST_EQUAL(p32.first, U"");        TEST_EQUAL(p32.second, U"");
        TRY(p32 = split_file(U""s));           TEST_EQUAL(p32.first, U"");        TEST_EQUAL(p32.second, U"");
        TRY(p32 = split_path(U"hello.txt"s));  TEST_EQUAL(p32.first, U"");        TEST_EQUAL(p32.second, U"hello.txt");
        TRY(p32 = split_file(U"hello.txt"s));  TEST_EQUAL(p32.first, U"hello");   TEST_EQUAL(p32.second, U".txt");
        TRY(p32 = split_path(U"hello"s));      TEST_EQUAL(p32.first, U"");        TEST_EQUAL(p32.second, U"hello");
        TRY(p32 = split_file(U"hello"s));      TEST_EQUAL(p32.first, U"hello");   TEST_EQUAL(p32.second, U"");
        TRY(p32 = split_path(U".hello"s));     TEST_EQUAL(p32.first, U"");        TEST_EQUAL(p32.second, U".hello");
        TRY(p32 = split_file(U".hello"s));     TEST_EQUAL(p32.first, U".hello");  TEST_EQUAL(p32.second, U"");

        #if defined(PRI_TARGET_UNIX)

            TRY(p8 = split_path("/hello.txt"s));                            TEST_EQUAL(p8.first, "/");            TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_path("/hello.txt"s, fs_fullname));               TEST_EQUAL(p8.first, "/");            TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_file("/hello.txt"s));                            TEST_EQUAL(p8.first, "hello");        TEST_EQUAL(p8.second, ".txt");
            TRY(p8 = split_path("abc/def/hello.txt"s));                     TEST_EQUAL(p8.first, "abc/def");      TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_path("abc/def/hello.txt"s, fs_fullname));        TEST_EQUAL(p8.first, "abc/def/");     TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_file("abc/def/hello.txt"s));                     TEST_EQUAL(p8.first, "hello");        TEST_EQUAL(p8.second, ".txt");
            TRY(p8 = split_path("abc/def/hello"s));                         TEST_EQUAL(p8.first, "abc/def");      TEST_EQUAL(p8.second, "hello");
            TRY(p8 = split_path("abc/def/hello"s, fs_fullname));            TEST_EQUAL(p8.first, "abc/def/");     TEST_EQUAL(p8.second, "hello");
            TRY(p8 = split_file("abc/def/hello"s));                         TEST_EQUAL(p8.first, "hello");        TEST_EQUAL(p8.second, "");
            TRY(p8 = split_path("abc/def/hello.world.txt"s));               TEST_EQUAL(p8.first, "abc/def");      TEST_EQUAL(p8.second, "hello.world.txt");
            TRY(p8 = split_path("abc/def/hello.world.txt"s, fs_fullname));  TEST_EQUAL(p8.first, "abc/def/");     TEST_EQUAL(p8.second, "hello.world.txt");
            TRY(p8 = split_file("abc/def/hello.world.txt"s));               TEST_EQUAL(p8.first, "hello.world");  TEST_EQUAL(p8.second, ".txt");
            TRY(p8 = split_path("abc/def/.hello"s));                        TEST_EQUAL(p8.first, "abc/def");      TEST_EQUAL(p8.second, ".hello");
            TRY(p8 = split_path("abc/def/.hello"s, fs_fullname));           TEST_EQUAL(p8.first, "abc/def/");     TEST_EQUAL(p8.second, ".hello");
            TRY(p8 = split_file("abc/def/.hello"s));                        TEST_EQUAL(p8.first, ".hello");       TEST_EQUAL(p8.second, "");
            TRY(p8 = split_path("abc/def/.hello.txt"s));                    TEST_EQUAL(p8.first, "abc/def");      TEST_EQUAL(p8.second, ".hello.txt");
            TRY(p8 = split_path("abc/def/.hello.txt"s, fs_fullname));       TEST_EQUAL(p8.first, "abc/def/");     TEST_EQUAL(p8.second, ".hello.txt");
            TRY(p8 = split_file("abc/def/.hello.txt"s));                    TEST_EQUAL(p8.first, ".hello");       TEST_EQUAL(p8.second, ".txt");

            TRY(p16 = split_path(u"/hello.txt"s));                            TEST_EQUAL(p16.first, u"/");            TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_path(u"/hello.txt"s, fs_fullname));               TEST_EQUAL(p16.first, u"/");            TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_file(u"/hello.txt"s));                            TEST_EQUAL(p16.first, u"hello");        TEST_EQUAL(p16.second, u".txt");
            TRY(p16 = split_path(u"abc/def/hello.txt"s));                     TEST_EQUAL(p16.first, u"abc/def");      TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_path(u"abc/def/hello.txt"s, fs_fullname));        TEST_EQUAL(p16.first, u"abc/def/");     TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_file(u"abc/def/hello.txt"s));                     TEST_EQUAL(p16.first, u"hello");        TEST_EQUAL(p16.second, u".txt");
            TRY(p16 = split_path(u"abc/def/hello"s));                         TEST_EQUAL(p16.first, u"abc/def");      TEST_EQUAL(p16.second, u"hello");
            TRY(p16 = split_path(u"abc/def/hello"s, fs_fullname));            TEST_EQUAL(p16.first, u"abc/def/");     TEST_EQUAL(p16.second, u"hello");
            TRY(p16 = split_file(u"abc/def/hello"s));                         TEST_EQUAL(p16.first, u"hello");        TEST_EQUAL(p16.second, u"");
            TRY(p16 = split_path(u"abc/def/hello.world.txt"s));               TEST_EQUAL(p16.first, u"abc/def");      TEST_EQUAL(p16.second, u"hello.world.txt");
            TRY(p16 = split_path(u"abc/def/hello.world.txt"s, fs_fullname));  TEST_EQUAL(p16.first, u"abc/def/");     TEST_EQUAL(p16.second, u"hello.world.txt");
            TRY(p16 = split_file(u"abc/def/hello.world.txt"s));               TEST_EQUAL(p16.first, u"hello.world");  TEST_EQUAL(p16.second, u".txt");
            TRY(p16 = split_path(u"abc/def/.hello"s));                        TEST_EQUAL(p16.first, u"abc/def");      TEST_EQUAL(p16.second, u".hello");
            TRY(p16 = split_path(u"abc/def/.hello"s, fs_fullname));           TEST_EQUAL(p16.first, u"abc/def/");     TEST_EQUAL(p16.second, u".hello");
            TRY(p16 = split_file(u"abc/def/.hello"s));                        TEST_EQUAL(p16.first, u".hello");       TEST_EQUAL(p16.second, u"");
            TRY(p16 = split_path(u"abc/def/.hello.txt"s));                    TEST_EQUAL(p16.first, u"abc/def");      TEST_EQUAL(p16.second, u".hello.txt");
            TRY(p16 = split_path(u"abc/def/.hello.txt"s, fs_fullname));       TEST_EQUAL(p16.first, u"abc/def/");     TEST_EQUAL(p16.second, u".hello.txt");
            TRY(p16 = split_file(u"abc/def/.hello.txt"s));                    TEST_EQUAL(p16.first, u".hello");       TEST_EQUAL(p16.second, u".txt");

            TRY(p32 = split_path(U"/hello.txt"s));                            TEST_EQUAL(p32.first, U"/");            TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_path(U"/hello.txt"s, fs_fullname));               TEST_EQUAL(p32.first, U"/");            TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_file(U"/hello.txt"s));                            TEST_EQUAL(p32.first, U"hello");        TEST_EQUAL(p32.second, U".txt");
            TRY(p32 = split_path(U"abc/def/hello.txt"s));                     TEST_EQUAL(p32.first, U"abc/def");      TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_path(U"abc/def/hello.txt"s, fs_fullname));        TEST_EQUAL(p32.first, U"abc/def/");     TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_file(U"abc/def/hello.txt"s));                     TEST_EQUAL(p32.first, U"hello");        TEST_EQUAL(p32.second, U".txt");
            TRY(p32 = split_path(U"abc/def/hello"s));                         TEST_EQUAL(p32.first, U"abc/def");      TEST_EQUAL(p32.second, U"hello");
            TRY(p32 = split_path(U"abc/def/hello"s, fs_fullname));            TEST_EQUAL(p32.first, U"abc/def/");     TEST_EQUAL(p32.second, U"hello");
            TRY(p32 = split_file(U"abc/def/hello"s));                         TEST_EQUAL(p32.first, U"hello");        TEST_EQUAL(p32.second, U"");
            TRY(p32 = split_path(U"abc/def/hello.world.txt"s));               TEST_EQUAL(p32.first, U"abc/def");      TEST_EQUAL(p32.second, U"hello.world.txt");
            TRY(p32 = split_path(U"abc/def/hello.world.txt"s, fs_fullname));  TEST_EQUAL(p32.first, U"abc/def/");     TEST_EQUAL(p32.second, U"hello.world.txt");
            TRY(p32 = split_file(U"abc/def/hello.world.txt"s));               TEST_EQUAL(p32.first, U"hello.world");  TEST_EQUAL(p32.second, U".txt");
            TRY(p32 = split_path(U"abc/def/.hello"s));                        TEST_EQUAL(p32.first, U"abc/def");      TEST_EQUAL(p32.second, U".hello");
            TRY(p32 = split_path(U"abc/def/.hello"s, fs_fullname));           TEST_EQUAL(p32.first, U"abc/def/");     TEST_EQUAL(p32.second, U".hello");
            TRY(p32 = split_file(U"abc/def/.hello"s));                        TEST_EQUAL(p32.first, U".hello");       TEST_EQUAL(p32.second, U"");
            TRY(p32 = split_path(U"abc/def/.hello.txt"s));                    TEST_EQUAL(p32.first, U"abc/def");      TEST_EQUAL(p32.second, U".hello.txt");
            TRY(p32 = split_path(U"abc/def/.hello.txt"s, fs_fullname));       TEST_EQUAL(p32.first, U"abc/def/");     TEST_EQUAL(p32.second, U".hello.txt");
            TRY(p32 = split_file(U"abc/def/.hello.txt"s));                    TEST_EQUAL(p32.first, U".hello");       TEST_EQUAL(p32.second, U".txt");

            TEST_EQUAL(file_path(""), "");
            TEST_EQUAL(file_path("hello.txt"), "hello.txt");
            TEST_EQUAL(file_path("abc/def/hello.txt"), "abc/def/hello.txt");
            TEST_EQUAL(file_path("", ""), "");
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

            TEST_EQUAL(file_path(u""), u"");
            TEST_EQUAL(file_path(u"hello.txt"), u"hello.txt");
            TEST_EQUAL(file_path(u"abc/def/hello.txt"), u"abc/def/hello.txt");
            TEST_EQUAL(file_path(u"", u""), u"");
            TEST_EQUAL(file_path(u"hello", u"world"), u"hello/world");
            TEST_EQUAL(file_path(u"hello", u"world/"), u"hello/world/");
            TEST_EQUAL(file_path(u"hello", u"/world"), u"/world");
            TEST_EQUAL(file_path(u"hello", u"/world/"), u"/world/");
            TEST_EQUAL(file_path(u"hello/", u"world"), u"hello/world");
            TEST_EQUAL(file_path(u"hello/", u"world/"), u"hello/world/");
            TEST_EQUAL(file_path(u"hello/", u"/world"), u"/world");
            TEST_EQUAL(file_path(u"hello/", u"/world/"), u"/world/");
            TEST_EQUAL(file_path(u"/hello", u"world"), u"/hello/world");
            TEST_EQUAL(file_path(u"/hello", u"world/"), u"/hello/world/");
            TEST_EQUAL(file_path(u"/hello", u"/world"), u"/world");
            TEST_EQUAL(file_path(u"/hello", u"/world/"), u"/world/");
            TEST_EQUAL(file_path(u"/hello/", u"world"), u"/hello/world");
            TEST_EQUAL(file_path(u"/hello/", u"world/"), u"/hello/world/");
            TEST_EQUAL(file_path(u"/hello/", u"/world"), u"/world");
            TEST_EQUAL(file_path(u"/hello/", u"/world/"), u"/world/");

            TEST_EQUAL(file_path(U""), U"");
            TEST_EQUAL(file_path(U"hello.txt"), U"hello.txt");
            TEST_EQUAL(file_path(U"abc/def/hello.txt"), U"abc/def/hello.txt");
            TEST_EQUAL(file_path(U"", U""), U"");
            TEST_EQUAL(file_path(U"hello", U"world"), U"hello/world");
            TEST_EQUAL(file_path(U"hello", U"world/"), U"hello/world/");
            TEST_EQUAL(file_path(U"hello", U"/world"), U"/world");
            TEST_EQUAL(file_path(U"hello", U"/world/"), U"/world/");
            TEST_EQUAL(file_path(U"hello/", U"world"), U"hello/world");
            TEST_EQUAL(file_path(U"hello/", U"world/"), U"hello/world/");
            TEST_EQUAL(file_path(U"hello/", U"/world"), U"/world");
            TEST_EQUAL(file_path(U"hello/", U"/world/"), U"/world/");
            TEST_EQUAL(file_path(U"/hello", U"world"), U"/hello/world");
            TEST_EQUAL(file_path(U"/hello", U"world/"), U"/hello/world/");
            TEST_EQUAL(file_path(U"/hello", U"/world"), U"/world");
            TEST_EQUAL(file_path(U"/hello", U"/world/"), U"/world/");
            TEST_EQUAL(file_path(U"/hello/", U"world"), U"/hello/world");
            TEST_EQUAL(file_path(U"/hello/", U"world/"), U"/hello/world/");
            TEST_EQUAL(file_path(U"/hello/", U"/world"), U"/world");
            TEST_EQUAL(file_path(U"/hello/", U"/world/"), U"/world/");

        #else

            TRY(p8 = split_path("C:\\hello.txt"s));                           TEST_EQUAL(p8.first, "C:\\");         TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_path("C:\\hello.txt"s, fs_fullname));              TEST_EQUAL(p8.first, "C:\\");         TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_file("C:\\hello.txt"s));                           TEST_EQUAL(p8.first, "hello");        TEST_EQUAL(p8.second, ".txt");
            TRY(p8 = split_path("C:hello.txt"s));                             TEST_EQUAL(p8.first, "C:");           TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_path("C:hello.txt"s, fs_fullname));                TEST_EQUAL(p8.first, "C:");           TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_file("C:hello.txt"s));                             TEST_EQUAL(p8.first, "hello");        TEST_EQUAL(p8.second, ".txt");
            TRY(p8 = split_path("abc\\def\\hello.txt"s));                     TEST_EQUAL(p8.first, "abc\\def");     TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_path("abc\\def\\hello.txt"s, fs_fullname));        TEST_EQUAL(p8.first, "abc\\def\\");   TEST_EQUAL(p8.second, "hello.txt");
            TRY(p8 = split_file("abc\\def\\hello.txt"s));                     TEST_EQUAL(p8.first, "hello");        TEST_EQUAL(p8.second, ".txt");
            TRY(p8 = split_path("abc\\def\\hello"s));                         TEST_EQUAL(p8.first, "abc\\def");     TEST_EQUAL(p8.second, "hello");
            TRY(p8 = split_path("abc\\def\\hello"s, fs_fullname));            TEST_EQUAL(p8.first, "abc\\def\\");   TEST_EQUAL(p8.second, "hello");
            TRY(p8 = split_file("abc\\def\\hello"s));                         TEST_EQUAL(p8.first, "hello");        TEST_EQUAL(p8.second, "");
            TRY(p8 = split_path("abc\\def\\hello.world.txt"s));               TEST_EQUAL(p8.first, "abc\\def");     TEST_EQUAL(p8.second, "hello.world.txt");
            TRY(p8 = split_path("abc\\def\\hello.world.txt"s, fs_fullname));  TEST_EQUAL(p8.first, "abc\\def\\");   TEST_EQUAL(p8.second, "hello.world.txt");
            TRY(p8 = split_file("abc\\def\\hello.world.txt"s));               TEST_EQUAL(p8.first, "hello.world");  TEST_EQUAL(p8.second, ".txt");
            TRY(p8 = split_path("abc\\def\\.hello"s));                        TEST_EQUAL(p8.first, "abc\\def");     TEST_EQUAL(p8.second, ".hello");
            TRY(p8 = split_path("abc\\def\\.hello"s, fs_fullname));           TEST_EQUAL(p8.first, "abc\\def\\");   TEST_EQUAL(p8.second, ".hello");
            TRY(p8 = split_file("abc\\def\\.hello"s));                        TEST_EQUAL(p8.first, ".hello");       TEST_EQUAL(p8.second, "");
            TRY(p8 = split_path("abc\\def\\.hello.txt"s));                    TEST_EQUAL(p8.first, "abc\\def");     TEST_EQUAL(p8.second, ".hello.txt");
            TRY(p8 = split_path("abc\\def\\.hello.txt"s, fs_fullname));       TEST_EQUAL(p8.first, "abc\\def\\");   TEST_EQUAL(p8.second, ".hello.txt");
            TRY(p8 = split_file("abc\\def\\.hello.txt"s));                    TEST_EQUAL(p8.first, ".hello");       TEST_EQUAL(p8.second, ".txt");

            TRY(p16 = split_path(u"C:\\hello.txt"s));                           TEST_EQUAL(p16.first, u"C:\\");         TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_path(u"C:\\hello.txt"s, fs_fullname));              TEST_EQUAL(p16.first, u"C:\\");         TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_file(u"C:\\hello.txt"s));                           TEST_EQUAL(p16.first, u"hello");        TEST_EQUAL(p16.second, u".txt");
            TRY(p16 = split_path(u"C:hello.txt"s));                             TEST_EQUAL(p16.first, u"C:");           TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_path(u"C:hello.txt"s, fs_fullname));                TEST_EQUAL(p16.first, u"C:");           TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_file(u"C:hello.txt"s));                             TEST_EQUAL(p16.first, u"hello");        TEST_EQUAL(p16.second, u".txt");
            TRY(p16 = split_path(u"abc\\def\\hello.txt"s));                     TEST_EQUAL(p16.first, u"abc\\def");     TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_path(u"abc\\def\\hello.txt"s, fs_fullname));        TEST_EQUAL(p16.first, u"abc\\def\\");   TEST_EQUAL(p16.second, u"hello.txt");
            TRY(p16 = split_file(u"abc\\def\\hello.txt"s));                     TEST_EQUAL(p16.first, u"hello");        TEST_EQUAL(p16.second, u".txt");
            TRY(p16 = split_path(u"abc\\def\\hello"s));                         TEST_EQUAL(p16.first, u"abc\\def");     TEST_EQUAL(p16.second, u"hello");
            TRY(p16 = split_path(u"abc\\def\\hello"s, fs_fullname));            TEST_EQUAL(p16.first, u"abc\\def\\");   TEST_EQUAL(p16.second, u"hello");
            TRY(p16 = split_file(u"abc\\def\\hello"s));                         TEST_EQUAL(p16.first, u"hello");        TEST_EQUAL(p16.second, u"");
            TRY(p16 = split_path(u"abc\\def\\hello.world.txt"s));               TEST_EQUAL(p16.first, u"abc\\def");     TEST_EQUAL(p16.second, u"hello.world.txt");
            TRY(p16 = split_path(u"abc\\def\\hello.world.txt"s, fs_fullname));  TEST_EQUAL(p16.first, u"abc\\def\\");   TEST_EQUAL(p16.second, u"hello.world.txt");
            TRY(p16 = split_file(u"abc\\def\\hello.world.txt"s));               TEST_EQUAL(p16.first, u"hello.world");  TEST_EQUAL(p16.second, u".txt");
            TRY(p16 = split_path(u"abc\\def\\.hello"s));                        TEST_EQUAL(p16.first, u"abc\\def");     TEST_EQUAL(p16.second, u".hello");
            TRY(p16 = split_path(u"abc\\def\\.hello"s, fs_fullname));           TEST_EQUAL(p16.first, u"abc\\def\\");   TEST_EQUAL(p16.second, u".hello");
            TRY(p16 = split_file(u"abc\\def\\.hello"s));                        TEST_EQUAL(p16.first, u".hello");       TEST_EQUAL(p16.second, u"");
            TRY(p16 = split_path(u"abc\\def\\.hello.txt"s));                    TEST_EQUAL(p16.first, u"abc\\def");     TEST_EQUAL(p16.second, u".hello.txt");
            TRY(p16 = split_path(u"abc\\def\\.hello.txt"s, fs_fullname));       TEST_EQUAL(p16.first, u"abc\\def\\");   TEST_EQUAL(p16.second, u".hello.txt");
            TRY(p16 = split_file(u"abc\\def\\.hello.txt"s));                    TEST_EQUAL(p16.first, u".hello");       TEST_EQUAL(p16.second, u".txt");

            TRY(p32 = split_path(U"C:\\hello.txt"s));                           TEST_EQUAL(p32.first, U"C:\\");         TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_path(U"C:\\hello.txt"s, fs_fullname));              TEST_EQUAL(p32.first, U"C:\\");         TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_file(U"C:\\hello.txt"s));                           TEST_EQUAL(p32.first, U"hello");        TEST_EQUAL(p32.second, U".txt");
            TRY(p32 = split_path(U"C:hello.txt"s));                             TEST_EQUAL(p32.first, U"C:");           TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_path(U"C:hello.txt"s, fs_fullname));                TEST_EQUAL(p32.first, U"C:");           TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_file(U"C:hello.txt"s));                             TEST_EQUAL(p32.first, U"hello");        TEST_EQUAL(p32.second, U".txt");
            TRY(p32 = split_path(U"abc\\def\\hello.txt"s));                     TEST_EQUAL(p32.first, U"abc\\def");     TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_path(U"abc\\def\\hello.txt"s, fs_fullname));        TEST_EQUAL(p32.first, U"abc\\def\\");   TEST_EQUAL(p32.second, U"hello.txt");
            TRY(p32 = split_file(U"abc\\def\\hello.txt"s));                     TEST_EQUAL(p32.first, U"hello");        TEST_EQUAL(p32.second, U".txt");
            TRY(p32 = split_path(U"abc\\def\\hello"s));                         TEST_EQUAL(p32.first, U"abc\\def");     TEST_EQUAL(p32.second, U"hello");
            TRY(p32 = split_path(U"abc\\def\\hello"s, fs_fullname));            TEST_EQUAL(p32.first, U"abc\\def\\");   TEST_EQUAL(p32.second, U"hello");
            TRY(p32 = split_file(U"abc\\def\\hello"s));                         TEST_EQUAL(p32.first, U"hello");        TEST_EQUAL(p32.second, U"");
            TRY(p32 = split_path(U"abc\\def\\hello.world.txt"s));               TEST_EQUAL(p32.first, U"abc\\def");     TEST_EQUAL(p32.second, U"hello.world.txt");
            TRY(p32 = split_path(U"abc\\def\\hello.world.txt"s, fs_fullname));  TEST_EQUAL(p32.first, U"abc\\def\\");   TEST_EQUAL(p32.second, U"hello.world.txt");
            TRY(p32 = split_file(U"abc\\def\\hello.world.txt"s));               TEST_EQUAL(p32.first, U"hello.world");  TEST_EQUAL(p32.second, U".txt");
            TRY(p32 = split_path(U"abc\\def\\.hello"s));                        TEST_EQUAL(p32.first, U"abc\\def");     TEST_EQUAL(p32.second, U".hello");
            TRY(p32 = split_path(U"abc\\def\\.hello"s, fs_fullname));           TEST_EQUAL(p32.first, U"abc\\def\\");   TEST_EQUAL(p32.second, U".hello");
            TRY(p32 = split_file(U"abc\\def\\.hello"s));                        TEST_EQUAL(p32.first, U".hello");       TEST_EQUAL(p32.second, U"");
            TRY(p32 = split_path(U"abc\\def\\.hello.txt"s));                    TEST_EQUAL(p32.first, U"abc\\def");     TEST_EQUAL(p32.second, U".hello.txt");
            TRY(p32 = split_path(U"abc\\def\\.hello.txt"s, fs_fullname));       TEST_EQUAL(p32.first, U"abc\\def\\");   TEST_EQUAL(p32.second, U".hello.txt");
            TRY(p32 = split_file(U"abc\\def\\.hello.txt"s));                    TEST_EQUAL(p32.first, U".hello");       TEST_EQUAL(p32.second, U".txt");

            TEST_EQUAL(file_path(""), "");
            TEST_EQUAL(file_path("hello.txt"), "hello.txt");
            TEST_EQUAL(file_path("abc\\def\\hello.txt"), "abc\\def\\hello.txt");
            TEST_EQUAL(file_path("", ""), "");
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

            TEST_EQUAL(file_path(u""), u"");
            TEST_EQUAL(file_path(u"hello.txt"), u"hello.txt");
            TEST_EQUAL(file_path(u"abc\\def\\hello.txt"), u"abc\\def\\hello.txt");
            TEST_EQUAL(file_path(u"", u""), u"");
            TEST_EQUAL(file_path(u"hello", u"world"), u"hello\\world");
            TEST_EQUAL(file_path(u"hello", u"world\\"), u"hello\\world\\");
            TEST_EQUAL(file_path(u"hello", u"C:\\world"), u"C:\\world");
            TEST_EQUAL(file_path(u"hello", u"C:\\world\\"), u"C:\\world\\");
            TEST_EQUAL(file_path(u"hello\\", u"world"), u"hello\\world");
            TEST_EQUAL(file_path(u"hello\\", u"world\\"), u"hello\\world\\");
            TEST_EQUAL(file_path(u"hello\\", u"C:\\world"), u"C:\\world");
            TEST_EQUAL(file_path(u"hello\\", u"C:\\world\\"), u"C:\\world\\");
            TEST_EQUAL(file_path(u"C:\\hello", u"world"), u"C:\\hello\\world");
            TEST_EQUAL(file_path(u"C:\\hello", u"world\\"), u"C:\\hello\\world\\");
            TEST_EQUAL(file_path(u"C:\\hello", u"C:\\world"), u"C:\\world");
            TEST_EQUAL(file_path(u"C:\\hello", u"C:\\world\\"), u"C:\\world\\");
            TEST_EQUAL(file_path(u"C:\\hello\\", u"world"), u"C:\\hello\\world");
            TEST_EQUAL(file_path(u"C:\\hello\\", u"world\\"), u"C:\\hello\\world\\");
            TEST_EQUAL(file_path(u"C:\\hello\\", u"C:\\world"), u"C:\\world");
            TEST_EQUAL(file_path(u"C:\\hello\\", u"C:\\world\\"), u"C:\\world\\");

            TEST_EQUAL(file_path(U""), U"");
            TEST_EQUAL(file_path(U"hello.txt"), U"hello.txt");
            TEST_EQUAL(file_path(U"abc\\def\\hello.txt"), U"abc\\def\\hello.txt");
            TEST_EQUAL(file_path(U"", U""), U"");
            TEST_EQUAL(file_path(U"hello", U"world"), U"hello\\world");
            TEST_EQUAL(file_path(U"hello", U"world\\"), U"hello\\world\\");
            TEST_EQUAL(file_path(U"hello", U"C:\\world"), U"C:\\world");
            TEST_EQUAL(file_path(U"hello", U"C:\\world\\"), U"C:\\world\\");
            TEST_EQUAL(file_path(U"hello\\", U"world"), U"hello\\world");
            TEST_EQUAL(file_path(U"hello\\", U"world\\"), U"hello\\world\\");
            TEST_EQUAL(file_path(U"hello\\", U"C:\\world"), U"C:\\world");
            TEST_EQUAL(file_path(U"hello\\", U"C:\\world\\"), U"C:\\world\\");
            TEST_EQUAL(file_path(U"C:\\hello", U"world"), U"C:\\hello\\world");
            TEST_EQUAL(file_path(U"C:\\hello", U"world\\"), U"C:\\hello\\world\\");
            TEST_EQUAL(file_path(U"C:\\hello", U"C:\\world"), U"C:\\world");
            TEST_EQUAL(file_path(U"C:\\hello", U"C:\\world\\"), U"C:\\world\\");
            TEST_EQUAL(file_path(U"C:\\hello\\", U"world"), U"C:\\hello\\world");
            TEST_EQUAL(file_path(U"C:\\hello\\", U"world\\"), U"C:\\hello\\world\\");
            TEST_EQUAL(file_path(U"C:\\hello\\", U"C:\\world"), U"C:\\world");
            TEST_EQUAL(file_path(U"C:\\hello\\", U"C:\\world\\"), U"C:\\world\\");

        #endif

    }

    void check_file_system_operations_utf8() {

        u8string d1, d2, d3, f1, f2, f3, f4;
        vector<string> vec;
        FileId id1, id2;

        TEST(file_exists("."s));
        TEST(file_exists(".."s));
        TEST(file_exists("Makefile"s));
        TEST(file_exists("unicorn"s));
        TEST(! file_exists(""s));
        TEST(! file_exists("no such file"s));

        TEST_EQUAL(file_id(""s), 0);
        TEST_EQUAL(file_id("no such file"s), 0);
        TRY(id1 = file_id("Makefile"s));
        TRY(id2 = file_id("unicorn"s));
        TEST_COMPARE(id1, >, 0);
        TEST_COMPARE(id2, >, 0);
        TEST_COMPARE(id1, !=, id2);

        TEST(file_is_directory("."s));
        TEST(file_is_directory(".."s));
        TEST(file_is_directory("unicorn"s));
        TEST(! file_is_directory("Makefile"s));
        TEST(! file_is_directory(""s));
        TEST(! file_is_directory("no such file"s));
        #if defined(PRI_TARGET_UNIX)
            TEST(file_is_directory("/"s));
        #else
            TEST(file_is_directory("C:\\"s));
        #endif

        TEST(! file_is_hidden("."s));
        TEST(! file_is_hidden(".."s));
        TEST(! file_is_hidden("unicorn"s));
        TEST(! file_is_hidden("Makefile"s));
        TEST(! file_is_hidden("no such file"s));
        #if defined(PRI_TARGET_UNIX)
            TEST(! file_is_hidden("/"s));
        #else
            TEST(! file_is_hidden("C:\\"s));
        #endif

        TEST(! file_is_symlink("."s));
        TEST(! file_is_symlink(".."s));
        TEST(! file_is_symlink("unicorn"s));
        TEST(! file_is_symlink("Makefile"s));
        TEST(! file_is_symlink("no such file"s));
        #if defined(PRI_TARGET_UNIX)
            TEST(! file_is_symlink("/"s));
        #else
            TEST(! file_is_symlink("C:\\"s));
        #endif

        TEST_EQUAL(file_size("Makefile"s), 51);
        TEST_EQUAL(file_size("Makefile"s, fs_recurse), 51);
        TEST_EQUAL(file_size("no such file"s), 0);
        TEST_EQUAL(file_size("no such file"s, fs_recurse), 0);
        TEST_COMPARE(file_size("."s, fs_recurse), >, 65000000);
        TEST_COMPARE(file_size("unicorn"s, fs_recurse), >, 4000000);

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
        TRY(copy_file(d2, d3, fs_recurse));
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
        TRY(make_directory(d2, fs_recurse));
        TEST(file_exists(d1));
        TEST(file_exists(d2));
        TEST(file_is_directory(d1));
        TEST(file_is_directory(d2));
        TRY(make_directory(d2, fs_recurse));
        TEST_THROW(remove_file(d1), std::system_error);
        TRY(remove_file(d1, fs_recurse));
        TEST(! file_exists(d1));
        TEST(! file_exists(d2));
        TRY(remove_file(d1, fs_recurse));

        d1 = "__test_dir_1";
        auto dir = directory(d1);
        f1 = file_path(d1, "hello");
        f2 = file_path(d1, "world");
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 0);
        TEST_EQUAL(Test::format_range(vec), "[]");
        TRY(make_directory(d1));
        TEST(file_exists(d1));
        TEST(file_is_directory(d1));
        TRY(dir = directory(d1));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 0);
        TEST_EQUAL(Test::format_range(vec), "[]");
        TRY(touch(f1));
        TEST(file_exists(f1));
        TRY(touch(f2));
        TEST(file_exists(f2));
        TEST_THROW(make_directory(f1), std::system_error);
        TRY(dir = directory(d1));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        std::sort(vec.begin(), vec.end());
        TEST_EQUAL(Test::format_range(vec), normalize_file("[hello,world]"s));
        TRY(dir = directory(d1, fs_dotdot));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 4);
        std::sort(vec.begin(), vec.end());
        TEST_EQUAL(Test::format_range(vec), normalize_file("[.,..,hello,world]"s));
        TRY(dir = directory(d1, fs_fullname));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        std::sort(vec.begin(), vec.end());
        TEST_EQUAL(Test::format_range(vec), normalize_file("[__test_dir_1" SLASH "hello,__test_dir_1" SLASH "world]"s));
        TEST_THROW(remove_file(d1), std::system_error);
        TEST(file_exists(d1));
        TEST(file_exists(f1));
        TEST(file_exists(f2));
        TRY(remove_file(d1, fs_recurse));
        TEST(! file_exists(d1));
        TEST(! file_exists(f1));
        TEST(! file_exists(f2));
        TRY(dir = directory(d1));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 0);
        TEST_EQUAL(Test::format_range(vec), "[]");

    }

    void check_file_system_operations_utf16() {

        u16string d1, d2, d3, f1, f2, f3, f4;
        vector<u16string> vec;
        FileId id1, id2;

        TEST(file_exists(u"."s));
        TEST(file_exists(u".."s));
        TEST(file_exists(u"Makefile"s));
        TEST(file_exists(u"unicorn"s));
        TEST(! file_exists(u""s));
        TEST(! file_exists(u"no such file"s));

        TEST_EQUAL(file_id(u""s), 0);
        TEST_EQUAL(file_id(u"no such file"s), 0);
        TRY(id1 = file_id(u"Makefile"s));
        TRY(id2 = file_id(u"unicorn"s));
        TEST_COMPARE(id1, >, 0);
        TEST_COMPARE(id2, >, 0);
        TEST_COMPARE(id1, !=, id2);

        TEST(file_is_directory(u"."s));
        TEST(file_is_directory(u".."s));
        TEST(file_is_directory(u"unicorn"s));
        TEST(! file_is_directory(u"Makefile"s));
        TEST(! file_is_directory(u""s));
        TEST(! file_is_directory(u"no such file"s));
        #if defined(PRI_TARGET_UNIX)
            TEST(file_is_directory(u"/"s));
        #else
            TEST(file_is_directory(u"C:\\"s));
        #endif

        TEST(! file_is_hidden(u"."s));
        TEST(! file_is_hidden(u".."s));
        TEST(! file_is_hidden(u"unicorn"s));
        TEST(! file_is_hidden(u"Makefile"s));
        TEST(! file_is_hidden(u"no such file"s));
        #if defined(PRI_TARGET_UNIX)
            TEST(! file_is_hidden(u"/"s));
        #else
            TEST(! file_is_hidden(u"C:\\"s));
        #endif

        TEST(! file_is_symlink(u"."s));
        TEST(! file_is_symlink(u".."s));
        TEST(! file_is_symlink(u"unicorn"s));
        TEST(! file_is_symlink(u"Makefile"s));
        TEST(! file_is_symlink(u"no such file"s));
        #if defined(PRI_TARGET_UNIX)
            TEST(! file_is_symlink(u"/"s));
        #else
            TEST(! file_is_symlink(u"C:\\"s));
        #endif

        TEST_EQUAL(file_size(u"Makefile"s), 51);
        TEST_EQUAL(file_size(u"Makefile"s, fs_recurse), 51);
        TEST_EQUAL(file_size(u"no such file"s), 0);
        TEST_EQUAL(file_size(u"no such file"s, fs_recurse), 0);
        TEST_COMPARE(file_size(u"."s, fs_recurse), >, 65000000);
        TEST_COMPARE(file_size(u"unicorn"s, fs_recurse), >, 4000000);

        f1 = u"__test_file_1";
        f2 = u"__test_file_2";
        f3 = u"__test_file_3";
        f4 = u"__test_FILE_3";
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

        d1 = u"__test_dir_1";
        d2 = u"__test_dir_2";
        d3 = u"__test_dir_3";
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
        TRY(copy_file(d2, d3, fs_recurse));
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

        d1 = u"__test_dir_1";
        d2 = u"__test_dir_1/test_dir_2";
        TEST(! file_exists(d1));
        TEST(! file_exists(d2));
        TEST_THROW(make_directory(d2), std::system_error);
        TRY(make_directory(d2, fs_recurse));
        TEST(file_exists(d1));
        TEST(file_exists(d2));
        TEST(file_is_directory(d1));
        TEST(file_is_directory(d2));
        TRY(make_directory(d2, fs_recurse));
        TEST_THROW(remove_file(d1), std::system_error);
        TRY(remove_file(d1, fs_recurse));
        TEST(! file_exists(d1));
        TEST(! file_exists(d2));
        TRY(remove_file(d1, fs_recurse));

        d1 = u"__test_dir_1";
        auto dir = directory(d1);
        f1 = file_path(d1, u"hello");
        f2 = file_path(d1, u"world");
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 0);
        TEST_EQUAL(Test::format_range(vec), "[]");
        TRY(make_directory(d1));
        TEST(file_exists(d1));
        TEST(file_is_directory(d1));
        TRY(dir = directory(d1));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 0);
        TEST_EQUAL(Test::format_range(vec), "[]");
        TRY(touch(f1));
        TEST(file_exists(f1));
        TRY(touch(f2));
        TEST(file_exists(f2));
        TEST_THROW(make_directory(f1), std::system_error);
        TRY(dir = directory(d1));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        std::sort(vec.begin(), vec.end());
        TEST_EQUAL(Test::format_range(vec), normalize_file("[hello,world]"s));
        TRY(dir = directory(d1, fs_dotdot));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 4);
        std::sort(vec.begin(), vec.end());
        TEST_EQUAL(Test::format_range(vec), normalize_file("[.,..,hello,world]"s));
        TRY(dir = directory(d1, fs_fullname));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        std::sort(vec.begin(), vec.end());
        TEST_EQUAL(Test::format_range(vec), normalize_file("[__test_dir_1" SLASH "hello,__test_dir_1" SLASH "world]"s));
        TEST_THROW(remove_file(d1), std::system_error);
        TEST(file_exists(d1));
        TEST(file_exists(f1));
        TEST(file_exists(f2));
        TRY(remove_file(d1, fs_recurse));
        TEST(! file_exists(d1));
        TEST(! file_exists(f1));
        TEST(! file_exists(f2));
        TRY(dir = directory(d1));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 0);
        TEST_EQUAL(Test::format_range(vec), "[]");

    }

    void check_file_system_operations_utf32() {

        u32string d1, d2, d3, f1, f2, f3, f4;
        vector<u32string> vec;
        FileId id1, id2;

        TEST(file_exists(U"."s));
        TEST(file_exists(U".."s));
        TEST(file_exists(U"Makefile"s));
        TEST(file_exists(U"unicorn"s));
        TEST(! file_exists(U""s));
        TEST(! file_exists(U"no such file"s));

        TEST_EQUAL(file_id(U""s), 0);
        TEST_EQUAL(file_id(U"no such file"s), 0);
        TRY(id1 = file_id(U"Makefile"s));
        TRY(id2 = file_id(U"unicorn"s));
        TEST_COMPARE(id1, >, 0);
        TEST_COMPARE(id2, >, 0);
        TEST_COMPARE(id1, !=, id2);

        TEST(file_is_directory(U"."s));
        TEST(file_is_directory(U".."s));
        TEST(file_is_directory(U"unicorn"s));
        TEST(! file_is_directory(U"Makefile"s));
        TEST(! file_is_directory(U""s));
        TEST(! file_is_directory(U"no such file"s));
        #if defined(PRI_TARGET_UNIX)
            TEST(file_is_directory(U"/"s));
        #else
            TEST(file_is_directory(U"C:\\"s));
        #endif

        TEST(! file_is_hidden(U"."s));
        TEST(! file_is_hidden(U".."s));
        TEST(! file_is_hidden(U"unicorn"s));
        TEST(! file_is_hidden(U"Makefile"s));
        TEST(! file_is_hidden(U"no such file"s));
        #if defined(PRI_TARGET_UNIX)
            TEST(! file_is_hidden(U"/"s));
        #else
            TEST(! file_is_hidden(U"C:\\"s));
        #endif

        TEST(! file_is_symlink(U"."s));
        TEST(! file_is_symlink(U".."s));
        TEST(! file_is_symlink(U"unicorn"s));
        TEST(! file_is_symlink(U"Makefile"s));
        TEST(! file_is_symlink(U"no such file"s));
        #if defined(PRI_TARGET_UNIX)
            TEST(! file_is_symlink(U"/"s));
        #else
            TEST(! file_is_symlink(U"C:\\"s));
        #endif

        TEST_EQUAL(file_size(U"Makefile"s), 51);
        TEST_EQUAL(file_size(U"Makefile"s, fs_recurse), 51);
        TEST_EQUAL(file_size(U"no such file"s), 0);
        TEST_EQUAL(file_size(U"no such file"s, fs_recurse), 0);
        TEST_COMPARE(file_size(U"."s, fs_recurse), >, 65000000);
        TEST_COMPARE(file_size(U"unicorn"s, fs_recurse), >, 4000000);

        f1 = U"__test_file_1";
        f2 = U"__test_file_2";
        f3 = U"__test_file_3";
        f4 = U"__test_FILE_3";
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

        d1 = U"__test_dir_1";
        d2 = U"__test_dir_2";
        d3 = U"__test_dir_3";
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
        TRY(copy_file(d2, d3, fs_recurse));
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

        d1 = U"__test_dir_1";
        d2 = U"__test_dir_1/test_dir_2";
        TEST(! file_exists(d1));
        TEST(! file_exists(d2));
        TEST_THROW(make_directory(d2), std::system_error);
        TRY(make_directory(d2, fs_recurse));
        TEST(file_exists(d1));
        TEST(file_exists(d2));
        TEST(file_is_directory(d1));
        TEST(file_is_directory(d2));
        TRY(make_directory(d2, fs_recurse));
        TEST_THROW(remove_file(d1), std::system_error);
        TRY(remove_file(d1, fs_recurse));
        TEST(! file_exists(d1));
        TEST(! file_exists(d2));
        TRY(remove_file(d1, fs_recurse));

        d1 = U"__test_dir_1";
        auto dir = directory(d1);
        f1 = file_path(d1, U"hello");
        f2 = file_path(d1, U"world");
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 0);
        TEST_EQUAL(Test::format_range(vec), "[]");
        TRY(make_directory(d1));
        TEST(file_exists(d1));
        TEST(file_is_directory(d1));
        TRY(dir = directory(d1));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 0);
        TEST_EQUAL(Test::format_range(vec), "[]");
        TRY(touch(f1));
        TEST(file_exists(f1));
        TRY(touch(f2));
        TEST(file_exists(f2));
        TEST_THROW(make_directory(f1), std::system_error);
        TRY(dir = directory(d1));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        std::sort(vec.begin(), vec.end());
        TEST_EQUAL(Test::format_range(vec), normalize_file("[hello,world]"s));
        TRY(dir = directory(d1, fs_dotdot));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 4);
        std::sort(vec.begin(), vec.end());
        TEST_EQUAL(Test::format_range(vec), normalize_file("[.,..,hello,world]"s));
        TRY(dir = directory(d1, fs_fullname));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 2);
        std::sort(vec.begin(), vec.end());
        TEST_EQUAL(Test::format_range(vec), normalize_file("[__test_dir_1" SLASH "hello,__test_dir_1" SLASH "world]"s));
        TEST_THROW(remove_file(d1), std::system_error);
        TEST(file_exists(d1));
        TEST(file_exists(f1));
        TEST(file_exists(f2));
        TRY(remove_file(d1, fs_recurse));
        TEST(! file_exists(d1));
        TEST(! file_exists(f1));
        TEST(! file_exists(f2));
        TRY(dir = directory(d1));
        TRY(std::copy(dir.begin(), dir.end(), overwrite(vec)));
        TEST_EQUAL(vec.size(), 0);
        TEST_EQUAL(Test::format_range(vec), "[]");

    }

}

TEST_MODULE(unicorn, file) {

    check_legal_file_names();
    check_file_name_operations();
    check_file_path_operations();
    check_file_system_operations_utf8();
    check_file_system_operations_utf16();
    check_file_system_operations_utf32();

}
