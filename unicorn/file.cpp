#include "unicorn/file.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include "unicorn/regex.hpp"
#include <cerrno>
#include <cstdio>

#if defined(_WIN32)
    #include <windows.h>
#endif

#if defined(_XOPEN_SOURCE)
    #include <dirent.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

using namespace std::literals;

namespace Unicorn {

    // Exceptions

    u8string FileError::assemble(int error, const u8string& files) {
        u8string s = "File system error" + files;
        if (error) {
            s += "; error ";
            s += dec(error);
            std::string details;
            #if defined(_XOPEN_SOURCE)
                import_string(system_message(error), details);
            #else
                recode(windows_message(error), details);
            #endif
            if (! details.empty()) {
                s += "; ";
                s += details;
            }
        }
        return s;
    }

    namespace UnicornDetail {

        #if defined(_XOPEN_SOURCE)

            // File name operations

            bool native_file_is_absolute(const u8string& file) {
                return file[0] == '/';
            }

            bool native_file_is_root(const u8string& file) {
                static const Regex pattern(R"(/{2,}[^/]+/?|/+)"s, rx_caseless);
                return pattern.match(file).matched();
            }

            // File properties

            std::string native_current_directory() {
                std::string name(256, '\0');
                for (;;) {
                    if (getcwd(&name[0], name.size()))
                        break;
                    if (errno != ERANGE)
                        throw FileError(errno);
                    name.resize(2 * name.size());
                }
                return name;
            }

            bool native_file_exists(const std::string& file) noexcept {
                struct stat s;
                return stat(file.data(), &s) == 0;
            }

            bool native_file_is_directory(const std::string& file) noexcept {
                struct stat s;
                return stat(file.data(), &s) == 0 && S_ISDIR(s.st_mode);
            }

            bool native_file_is_hidden(const std::string& file) noexcept {
                auto leaf = split_path(file).second;
                return leaf[0] == '.' && leaf != "." && leaf != "..";
            }

            bool native_file_is_symlink(const std::string& file) noexcept {
                struct stat s;
                return lstat(file.data(), &s) == 0 && S_ISLNK(s.st_mode);
            }

            // File system operations

            namespace {

                bool remove_file_helper(const std::string& file, bool dir_check) {
                    int rc = remove(file.data());
                    int err = errno;
                    if (rc == 0 || err == ENOENT)
                        return true;
                    else if (dir_check && (err == EEXIST || err == ENOTEMPTY))
                        return false;
                    else
                        throw FileError(err, file);
                }

            }

            void native_make_directory(const std::string& dir, bool recurse) {
                if (mkdir(dir.data(), 0777) == 0)
                    return;
                auto error = errno;
                if (error == EEXIST && native_file_is_directory(dir))
                    return;
                if (! recurse || error != ENOENT || dir.empty())
                    throw FileError(error, dir);
                auto parent = split_path(dir).first;
                if (parent == dir)
                    throw FileError(error, dir);
                native_make_directory(parent, recurse);
                if (mkdir(dir.data(), 0777) != 0)
                    throw FileError(errno, dir);
            }

            void native_rename_file(const std::string& src, const std::string& dst) {
                if (rename(src.data(), dst.data()))
                    throw FileError(errno, src, dst);
            }

            // Directory iterators

            struct DirectoryHelper::impl_type {
                std::string name;
                dirent entry;
                char padding[NAME_MAX + 1];
                DIR* dp;
                ~impl_type() { if (dp) closedir(dp); }
            };

            void DirectoryHelper::init(const std::string& dir) {
                impl = std::make_shared<impl_type>();
                memset(&impl->entry, 0, sizeof(impl->entry));
                memset(impl->padding, 0, sizeof(impl->padding));
                if (dir.empty())
                    impl->dp = opendir(".");
                else
                    impl->dp = opendir(dir.data());
                if (! impl->dp)
                    impl.reset();
            }

            const std::string& DirectoryHelper::file() const noexcept {
                static const std::string dummy {};
                return impl ? impl->name : dummy;
            }

            void DirectoryHelper::next() {
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
                static const Regex pattern(R"((\\\\\?\\)*([A-Z]:\\|\\{2,}[^?\\]))"s, rx_caseless);
                return pattern.anchor(file).matched();
            }

            bool native_file_is_drive_absolute(const u8string& file) {
                return ! file.empty() && file[0] == '\\' && file[1] != '\\';
            }

            bool native_file_is_drive_relative(const u8string& file) {
                return file.size() >= 2 && ascii_isalpha(file[0]) && file[1] == ':' && file[2] != '\\';
            }

            bool native_file_is_root(const u8string& file) {
                static const Regex pattern(R"((\\\\\?\\)*([A-Z]:\\|\\{2,}[^?\\]+\\?|\\+))"s, rx_caseless);
                return pattern.match(file).matched();
            }

            // File properties

            namespace {

                uint32_t get_attributes(const std::wstring& file) {
                    auto rc = GetFileAttributes(file.data());
                    return rc == INVALID_FILE_ATTRIBUTES ? 0 : rc;
                }

                bool is_drive(const std::wstring& file) {
                    auto rc = GetDriveType(file.data());
                    return rc != DRIVE_NO_ROOT_DIR && rc != DRIVE_UNKNOWN;
                }

            }

            std::wstring native_current_directory() {
                std::wstring name(256, L'\0');
                for (;;) {
                    auto rc = GetCurrentDirectoryW(name.size(), &name[0]);
                    if (rc == 0)
                        throw FileError(GetLastError());
                    if (rc < name.size())
                        break;
                    name.resize(2 * name.size());
                }
                return name;
            }

            bool native_file_exists(const std::wstring& file) noexcept {
                return file == L"." || file == L".." || get_attributes(file) || is_drive(file);
            }

            bool native_file_is_directory(const std::wstring& file) noexcept {
                return file == L"." || file == L".."
                    || (get_attributes(file) & FILE_ATTRIBUTE_DIRECTORY)
                    || is_drive(file);
            }

            bool native_file_is_hidden(const std::wstring& file) noexcept {
                return ! file_is_root(file)
                    && get_attributes(file) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
            }

            bool native_file_is_symlink(const std::wstring& file) noexcept {
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

                bool remove_file_helper(const std::wstring& file, bool dir_check) {
                    uint32_t error = 0;
                    if (file_is_directory(file)) {
                        if (RemoveDirectoryW(file.data()))
                            return true;
                        error = GetLastError();
                        if (dir_check && error == ERROR_DIR_NOT_EMPTY)
                            return false;
                    } else {
                        if (DeleteFileW(file.data()))
                            return true;
                        error = GetLastError();
                    }
                    if (error == ERROR_FILE_NOT_FOUND)
                        return true;
                    else
                        throw FileError(error, file);
                }

            }

            void native_make_directory(const std::wstring& dir, bool recurse) {
                if (CreateDirectoryW(dir.c_str(), nullptr))
                    return;
                auto error = GetLastError();
                if (error == ERROR_ALREADY_EXISTS && native_file_is_directory(dir))
                    return;
                if (! recurse || error != ERROR_PATH_NOT_FOUND || dir.empty())
                    throw FileError(error, dir);
                auto parent = split_path(dir).first;
                if (parent == dir)
                    throw FileError(error, dir);
                native_make_directory(parent, recurse);
                if (! CreateDirectoryW(dir.c_str(), nullptr))
                    throw FileError(errno, dir);

            }

            void native_rename_file(const std::wstring& src, const std::wstring& dst) {
                if (! MoveFileW(src.c_str(), dst.c_str()))
                    throw FileError(GetLastError(), src, dst);
            }

            // Directory iterators

            // On Windows we need to check first that the supplied file name
            // refers to a directory, because FindFirstFile() gives a false
            // positive for 'file\*' when the file is not a directory. There's
            // a race condition here but there doesn't seem to be anything we
            // can do about it.

            struct DirectoryHelper::impl_type {
                std::wstring name;
                HANDLE handle;
                WIN32_FIND_DATAW info;
                bool first;
                ~impl_type() { if (handle) FindClose(handle); }
            };

            void DirectoryHelper::init(const std::wstring& dir) {
                if (! file_is_directory(dir))
                    return;
                impl = std::make_shared<impl_type>();
                memset(&impl->info, 0, sizeof(impl->info));
                impl->first = true;
                auto glob = dir + L"\\*";
                impl->handle = FindFirstFileW(glob.data(), &impl->info);
                if (! impl->handle)
                    impl.reset();
            }

            const std::wstring& DirectoryHelper::file() const noexcept {
                static const std::wstring dummy {};
                return impl ? impl->name : dummy;
            }

            void DirectoryHelper::next() {
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
            if (recurse && ! remove_file_helper(file, true)
                        && native_file_is_directory(file)
                        && ! native_file_is_symlink(file))
                for (auto& child: directory(file, dir_fullname | dir_hidden))
                    native_remove_file(child, true);
            remove_file_helper(file, false);
        }

    }

}
