#include "unicorn/file.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include "unicorn/regex.hpp"
#include <cerrno>
#include <cstdio>
#include <system_error>

#if defined(PRI_TARGET_UNIX)
    #include <dirent.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#else
    #include <windows.h>
#endif

using namespace std::literals;
using namespace Unicorn::Literals;

namespace Unicorn {

    namespace {

        template <typename C> u8string quote_file(const basic_string<C>& name) { return quote(to_utf8(name), true); }

    }

    namespace UnicornDetail {

        #if defined(PRI_TARGET_UNIX)

            // File name operations

            bool native_file_is_absolute(const u8string& file) {
                return file[0] == '/';
            }

            bool native_file_is_root(const u8string& file) {
                static const auto pattern = "/{2,}[^/]+/?|/+"_re_i;
                return pattern.match(file).matched();
            }

            // File properties

            string native_current_directory() {
                string name(256, '\0');
                for (;;) {
                    if (getcwd(&name[0], name.size()))
                        break;
                    if (errno != ERANGE)
                        throw std::system_error(errno, std::generic_category());
                    name.resize(2 * name.size());
                }
                return name;
            }

            bool native_file_exists(const string& file) noexcept {
                struct stat s;
                return stat(file.data(), &s) == 0;
            }

            bool native_file_is_directory(const string& file) noexcept {
                struct stat s;
                return stat(file.data(), &s) == 0 && S_ISDIR(s.st_mode);
            }

            bool native_file_is_hidden(const string& file) noexcept {
                auto leaf = split_path(file).second;
                return leaf[0] == '.' && leaf != "." && leaf != "..";
            }

            bool native_file_is_symlink(const string& file) noexcept {
                struct stat s;
                return lstat(file.data(), &s) == 0 && S_ISLNK(s.st_mode);
            }

            // File system operations

            namespace {

                void remove_file_helper(const string& file) {
                    int rc = std::remove(file.data());
                    int error = errno;
                    if (rc != 0 && error != ENOENT)
                        throw std::system_error(error, std::generic_category(), quote_file(file));
                }

            }

            void native_make_directory(const string& dir, bool recurse) {
                if (mkdir(dir.data(), 0777) == 0)
                    return;
                int error = errno;
                if (error == EEXIST && native_file_is_directory(dir))
                    return;
                if (! recurse || error != ENOENT || dir.empty())
                    throw std::system_error(error, std::generic_category(), quote_file(dir));
                auto parent = split_path(dir).first;
                if (parent == dir)
                    throw std::system_error(error, std::generic_category(), quote_file(dir));
                native_make_directory(parent, recurse);
                if (mkdir(dir.data(), 0777) != 0) {
                    int error = errno;
                    throw std::system_error(error, std::generic_category(), quote_file(dir));
                }
            }

            void native_rename_file(const string& src, const string& dst) {
                if (rename(src.data(), dst.data())) {
                    int error = errno;
                    u8string note = quote_file(src) + " => " + quote_file(dst);
                    throw std::system_error(error, std::generic_category(), note);
                }
            }

            // Directory iterators

            struct DirectoryStage1::impl_type {
                string name;
                dirent entry;
                char padding[NAME_MAX + 1];
                DIR* dp;
                ~impl_type() { if (dp) closedir(dp); }
            };

            const string& DirectoryStage1::leaf() const noexcept {
                static const string dummy {};
                return impl ? impl->name : dummy;
            }

            void DirectoryStage1::init1(const string& dir) {
                impl = make_shared<impl_type>();
                memset(&impl->entry, 0, sizeof(impl->entry));
                memset(impl->padding, 0, sizeof(impl->padding));
                if (dir.empty())
                    impl->dp = opendir(".");
                else
                    impl->dp = opendir(dir.data());
                if (! impl->dp)
                    impl.reset();
            }

            void DirectoryStage1::next1() {
                if (! impl)
                    return;
                dirent* ptr = nullptr;
                int rc = readdir_r(impl->dp, &impl->entry, &ptr);
                if (! rc && ptr)
                    impl->name = impl->entry.d_name;
                else
                    impl.reset();
            }

        #else

            // File name operations

            bool native_file_is_absolute(const u8string& file) {
                static const auto pattern = R"((\\\\\?\\)*([A-Z]:\\|\\{2,}[^?\\]))"_re_i;
                return pattern.anchor(file).matched();
            }

            bool native_file_is_drive_absolute(const u8string& file) {
                return ! file.empty() && file[0] == '\\' && file[1] != '\\';
            }

            bool native_file_is_drive_relative(const u8string& file) {
                return file.size() >= 2 && ascii_isalpha(file[0]) && file[1] == ':' && file[2] != '\\';
            }

            bool native_file_is_root(const u8string& file) {
                static const auto pattern = R"((\\\\\?\\)*([A-Z]:\\|\\{2,}[^?\\]+\\?|\\+))"_re_i;
                return pattern.match(file).matched();
            }

            // File properties

            namespace {

                uint32_t get_attributes(const wstring& file) {
                    auto rc = GetFileAttributes(file.data());
                    return rc == INVALID_FILE_ATTRIBUTES ? 0 : rc;
                }

                bool is_drive(const wstring& file) {
                    auto rc = GetDriveType(file.data());
                    return rc != DRIVE_NO_ROOT_DIR && rc != DRIVE_UNKNOWN;
                }

            }

            wstring native_current_directory() {
                wstring name(256, L'\0');
                for (;;) {
                    auto rc = GetCurrentDirectoryW(name.size(), &name[0]);
                    if (rc == 0)
                        throw std::system_error(GetLastError(), windows_category());
                    if (rc < name.size())
                        break;
                    name.resize(2 * name.size());
                }
                return name;
            }

            bool native_file_exists(const wstring& file) noexcept {
                return file == L"." || file == L".." || get_attributes(file) || is_drive(file);
            }

            bool native_file_is_directory(const wstring& file) noexcept {
                return file == L"." || file == L".."
                    || (get_attributes(file) & FILE_ATTRIBUTE_DIRECTORY)
                    || is_drive(file);
            }

            bool native_file_is_hidden(const wstring& file) noexcept {
                return ! file_is_root(file)
                    && get_attributes(file) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
            }

            bool native_file_is_symlink(const wstring& file) noexcept {
                if (! (get_attributes(file) & FILE_ATTRIBUTE_REPARSE_POINT))
                    return false;
                WIN32_FIND_DATAW info;
                memset(&info, 0, sizeof(info));
                auto handle = FindFirstFileW(file.data(), &info);
                if (! handle)
                    return false;
                bool sym = (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
                    && info.dwReserved0 == IO_REPARSE_TAG_SYMLINK;
                FindClose(handle);
                return sym;
            }

            // File system operations

            namespace {

                void remove_file_helper(const wstring& file) {
                    bool dir = file_is_directory(file), ok;
                    if (dir)
                        ok = RemoveDirectoryW(file.data());
                    else
                        ok = DeleteFileW(file.data());
                    auto error = GetLastError();
                    if (! ok && error != ERROR_FILE_NOT_FOUND)
                        throw std::system_error(error, std::generic_category(), quote_file(file));
                }

            }

            void native_make_directory(const wstring& dir, bool recurse) {
                if (CreateDirectoryW(dir.c_str(), nullptr))
                    return;
                auto error = GetLastError();
                if (error == ERROR_ALREADY_EXISTS && native_file_is_directory(dir))
                    return;
                if (! recurse || error != ERROR_PATH_NOT_FOUND || dir.empty())
                    throw std::system_error(error, windows_category(), quote_file(dir));
                auto parent = split_path(dir).first;
                if (parent == dir)
                    throw std::system_error(error, windows_category(), quote_file(dir));
                native_make_directory(parent, recurse);
                if (! CreateDirectoryW(dir.c_str(), nullptr)) {
                    error = GetLastError();
                    throw std::system_error(error, windows_category(), quote_file(dir));
                }

            }

            void native_rename_file(const wstring& src, const wstring& dst) {
                if (! MoveFileW(src.c_str(), dst.c_str())) {
                    auto error = GetLastError();
                    u8string note = quote_file(src) + " => " + quote_file(dst);
                    throw std::system_error(error, windows_category(), note);
                }
            }

            // Directory iterators

            // On Windows we need to check first that the supplied file name
            // refers to a directory, because FindFirstFile() gives a false
            // positive for "file\*" when "file" exists but is not a
            // directory. There's a race condition here but there doesn't seem
            // to be anything we can do about it.

            struct DirectoryStage1::impl_type {
                wstring name;
                HANDLE handle;
                WIN32_FIND_DATAW info;
                bool first;
                ~impl_type() { if (handle) FindClose(handle); }
            };

            const wstring& DirectoryStage1::leaf() const noexcept {
                static const wstring dummy {};
                return impl ? impl->name : dummy;
            }

            void DirectoryStage1::init1(const wstring& dir) {
                if (! file_is_directory(dir))
                    return;
                impl = make_shared<impl_type>();
                memset(&impl->info, 0, sizeof(impl->info));
                impl->first = true;
                auto glob = dir + L"\\*";
                impl->handle = FindFirstFileW(glob.data(), &impl->info);
                if (! impl->handle)
                    impl.reset();
            }

            void DirectoryStage1::next1() {
                if (! impl)
                    return;
                if (! impl->first && ! FindNextFile(impl->handle, &impl->info)) {
                    impl.reset();
                    return;
                }
                impl->first = false;
                impl->name = impl->info.cFileName;
            }

        #endif

        // File system operations

        void native_remove_file(const NativeString& file, bool recurse) {
            if (recurse && native_file_is_directory(file) && ! native_file_is_symlink(file))
                for (auto child: directory(file, dir_fullname | dir_hidden))
                    native_remove_file(child, true);
            remove_file_helper(file);
        }

        // Directory iterators

        void DirectoryStage2::init2(const NativeString& dir, uint32_t flags) {
            init1(dir);
            fset = flags;
            if ((fset & dir_fullname) || ! (fset & dir_hidden)) {
                prefix = dir;
                if (! prefix.empty() && prefix.back() != native_file_delimiter)
                    prefix += native_file_delimiter;
            }
            next2();
        }

        void DirectoryStage2::next2() {
            static const NativeString link1(1, PRI_CHAR('.', NativeCharacter));
            static const NativeString link2(2, PRI_CHAR('.', NativeCharacter));
            current.clear();
            for (;;) {
                next1();
                if (done())
                    break;
                if ((fset & dir_unicode) && ! valid_string(leaf()))
                    continue;
                if (! (fset & dir_dotdot) && (leaf() == link1 || leaf() == link2))
                    continue;
                current = prefix + leaf();
                if (! (fset & dir_hidden) && file_is_hidden(current))
                    continue;
                if (! (fset & dir_fullname))
                    current = leaf();
                break;
            }
        }

    }

}
