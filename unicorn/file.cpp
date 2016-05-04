#include "unicorn/file.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include "unicorn/regex.hpp"
#include <cerrno>
#include <cstdio>
#include <system_error>

#if defined(PRI_TARGET_UNIX)
    #include <dirent.h>
    #include <sys/types.h>
    #include <unistd.h>
#else
    #include <windows.h>
#endif

using namespace Unicorn::Literals;
using namespace std::literals;

namespace Unicorn {

    namespace {

        #if defined(PRI_TARGET_WINDOWS)
            using HandleTarget = std::remove_pointer_t<HANDLE>;
        #endif

        template <typename C> u8string quote_file(const basic_string<C>& name) { return quote(to_utf8(name), true); }
        u8string file_pair(const NativeString& f1, const NativeString& f2) { return quote_file(f1) + " -> " + quote_file(f2); }

    }

    namespace UnicornDetail {

        // File name operations

        namespace {

            #define ILLEGAL_WINDOWS_CHARS "\0\"*:<>?|"s

            constexpr const char* posix_root              = R"(/{2,}[^/]+/?|/+)";
            constexpr const char* windows_absolute        = R"((\\\\\?\\)*([A-Z]:\\|\\{2,}(?=[^?\\])))";
            constexpr const char* windows_root            = R"((\\\\\?\\)*([A-Z]:\\|\\{2,}[^?\\]+\\?|\\+))";
            constexpr const char* windows_drive_absolute  = R"(\\[^\\])";
            constexpr const char* windows_drive_relative  = R"([A-Z]:(?!\\))";
            constexpr const char* windows_illegal_names   = R"((AUX|COM[1-9]|CON|LPT[1-9]|NUL|PRN)(\.[^.]*)?)";
            constexpr uint32_t match_flags                = rx_byte | rx_caseless | rx_noautocapture;

        }

        bool legal_posix_leaf(const string& file) {
            return file.find_first_of("\0/"s) == npos;
        }

        bool legal_posix_path(const string& file) {
            if (file.find('\0') != npos || (file[0] == '/' && ! posix_file_is_absolute(file)))
                return false;
            size_t i = file.find_first_not_of('/');
            return file.find("//", i) == npos;
        }

        bool legal_windows_leaf(const string& file) {
            static const string illegal_chars = ILLEGAL_WINDOWS_CHARS "/\\"s;
            static const Regex illegal_pattern(windows_illegal_names, match_flags);
            return file.find_first_of(illegal_chars) == npos && ! illegal_pattern.match(file);
        }

        bool legal_windows_path(const string& file) {
            static const string illegal_chars = ILLEGAL_WINDOWS_CHARS;
            static const Regex abs_pattern(windows_absolute, match_flags);
            static const Regex illegal_pattern(R"(([:\\]|^))"s + windows_illegal_names + R"(([:\\]|$))"s, match_flags);
            auto norm = normalize_file(file);
            auto match = abs_pattern.anchor(norm);
            if (! match && norm[0] == '\\')
                return false;
            size_t skip = match.count();
            return norm.find_first_of(illegal_chars, skip) == npos
                && norm.find("\\\\"s, skip) == npos
                && ! illegal_pattern.search(norm);
        }

        bool posix_file_is_absolute(const string& file) {
            return file[0] == '/';
        }

        bool posix_file_is_root(const string& file) {
            static const Regex root_pattern(posix_root, match_flags);
            return bool(root_pattern.match(file));
        }

        bool windows_file_is_absolute(const string& file) {
            static const Regex abs_pattern(windows_absolute, match_flags);
            return bool(abs_pattern.anchor(file));
        }

        bool windows_file_is_root(const string& file) {
            static const Regex root_pattern(windows_root, match_flags);
            return bool(root_pattern.match(file));
        }

        bool windows_file_is_drive_absolute(const string& file) {
            static const Regex drive_abs_pattern(windows_drive_absolute, match_flags);
            return bool(drive_abs_pattern.anchor(file));
        }

        bool windows_file_is_drive_relative(const string& file) {
            static const Regex drive_rel_pattern(windows_drive_relative, match_flags);
            return bool(drive_rel_pattern.anchor(file));
        }

        #if defined(PRI_TARGET_UNIX)

            namespace {

                class Cstdio {
                public:
                    Cstdio(const string& file, bool write) {
                        fp = fopen(file.data(), write ? "wb" : "rb");
                        int error = errno;
                        if (! fp)
                            throw std::system_error(error, std::generic_category(), quote_file(file));
                    }
                    ~Cstdio() noexcept { if (fp) fclose(fp); }
                    operator FILE*() const noexcept { return fp; }
                private:
                    FILE* fp;
                };

            }

            // File system query functions

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

            FileId native_file_id(const NativeString& file, uint32_t flags) noexcept {
                struct stat s;
                int rc;
                if (flags & fs_follow)
                    rc = stat(file.data(), &s);
                else
                    rc = lstat(file.data(), &s);
                if (rc != 0)
                    return 0;
                auto dev = FileId(s.st_dev);
                auto ino = FileId(s.st_ino);
                return (dev << (8 * sizeof(ino_t))) + ino;
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

            uint64_t native_file_size(const NativeString& file, uint32_t flags) noexcept {
                struct stat s;
                if (lstat(file.data(), &s) != 0)
                    return 0;
                auto bytes = uint64_t(s.st_size);
                if ((flags & fs_recurse) && S_ISDIR(s.st_mode))
                    for (auto& child: directory(file, fs_fullname | fs_hidden))
                        bytes += native_file_size(child, fs_recurse);
                return bytes;
            }

            string native_resolve_symlink(const string& file) {
                if (! native_file_is_symlink(file))
                    return file;
                vector<char> buf(256);
                for (;;) {
                    auto rc = readlink(file.data(), buf.data(), buf.size());
                    if (rc < 0) {
                        int error = errno;
                        if (error == EINVAL || error == ENOENT || error == ENOTDIR)
                            return file;
                        else
                            throw std::system_error(error, std::generic_category(), quote_file(file));
                    }
                    if (size_t(rc) <= buf.size() - 2)
                        return string(buf.data(), rc);
                    buf.resize(2 * buf.size());
                }
            }

            // File system modifying functions

            namespace {

                constexpr int fail_exists = EEXIST;
                constexpr int fail_no_parent = ENOENT;

                int mkdir_helper(const string& dir) {
                    errno = 0;
                    mkdir(dir.data(), 0777);
                    return errno;
                }

                void make_symlink_helper(const string& file, const string& link, uint32_t /*flags*/) {
                    if (symlink(file.data(), link.data()) == 0)
                        return;
                    int error = errno;
                    throw std::system_error(error, std::generic_category(), file_pair(link, file));
                }

                bool move_file_helper(const string& src, const string& dst) noexcept {
                    return rename(src.data(), dst.data()) == 0;
                }

                void remove_file_helper(const string& file) {
                    int rc = std::remove(file.data());
                    int error = errno;
                    if (rc != 0 && error != ENOENT)
                        throw std::system_error(error, std::generic_category(), quote_file(file));
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

            namespace {

                class Cstdio {
                public:
                    Cstdio(const wstring& file, bool write) {
                        fp = _wfopen(file.data(), write ? L"wb" : L"rb");
                        int error = errno;
                        if (! fp)
                            throw std::system_error(error, std::generic_category(), quote_file(file));
                    }
                    ~Cstdio() noexcept { if (fp) fclose(fp); }
                    operator FILE*() const noexcept { return fp; }
                private:
                    FILE* fp;
                };

            }

            // File system query functions

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

            FileId native_file_id(const NativeString& file, uint32_t flags) noexcept {
                NativeString f;
                try {
                    if (flags & fs_follow)
                        f = native_resolve_symlink(file);
                    else
                        f = file;
                }
                catch (const std::exception&) {
                    return 0;
                }
                shared_ptr<HandleTarget> handle(CreateFileW(f.data(), GENERIC_READ, FILE_SHARE_READ,
                    nullptr, OPEN_EXISTING, 0, nullptr), CloseHandle);
                if (! handle)
                    return 0;
                BY_HANDLE_FILE_INFORMATION info;
                memset(&info, 0, sizeof(info));
                if (! GetFileInformationByHandle(handle.get(), &info))
                    return 0;
                uint64_t hi = info.nFileIndexHigh;
                uint64_t lo = info.nFileIndexLow;
                return (hi << 32) + lo;
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

            uint64_t native_file_size(const NativeString& file, uint32_t flags) noexcept {
                WIN32_FILE_ATTRIBUTE_DATA info;
                if (! GetFileAttributesEx(file.data(), GetFileExInfoStandard, &info))
                    return 0;
                auto bytes = (uint64_t(info.nFileSizeHigh) << 32) + uint64_t(info.nFileSizeLow);
                if (flags & fs_recurse)
                    for (auto& child: directory(file, fs_fullname | fs_hidden))
                        bytes += native_file_size(child, fs_recurse);
                return bytes;
            }

            wstring native_resolve_symlink(const wstring& file) {
                if (! native_file_is_symlink(file))
                    return file;
                shared_ptr<HandleTarget> handle(CreateFileW(file.data(), GENERIC_READ, FILE_SHARE_READ,
                    nullptr, OPEN_EXISTING, 0, nullptr), CloseHandle);
                if (! handle) {
                    auto error = GetLastError();
                    throw std::system_error(error, std::generic_category(), quote_file(file));
                }
                vector<FILE_NAME_INFO> buf(100);
                for (;;) {
                    if (GetFileInformationByHandleEx(handle.get(), FileNameInfo, &buf[0], buf.size() * sizeof(FILE_NAME_INFO)))
                        return wstring(buf[0].FileName, buf[0].FileNameLength);
                    auto error = GetLastError();
                    if (error != ERROR_INSUFFICIENT_BUFFER)
                        throw std::system_error(error, std::generic_category(), quote_file(file));
                    buf.resize(2 * buf.size());
                }
            }

            // File system modifying functions

            namespace {

                constexpr int fail_exists = ERROR_ALREADY_EXISTS;
                constexpr int fail_no_parent = ERROR_PATH_NOT_FOUND;

                int mkdir_helper(const wstring& dir) {
                    SetLastError(0);
                    CreateDirectoryW(dir.c_str(), nullptr);
                    return GetLastError();
                }

                void make_symlink_helper(const wstring& file, const wstring& link, uint32_t flags) {
                    if (! (flags & fs_overwrite) && native_file_exists(link))
                        throw std::system_error(ERROR_ALREADY_EXISTS, windows_category(), quote_file(link));
                    DWORD wflags = native_file_is_directory(file) ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0;
                    if (CreateSymbolicLinkW(link.data(), file.data(), wflags))
                        return;
                    auto error = GetLastError();
                    throw std::system_error(error, windows_category(), file_pair(link, file));
                }

                bool move_file_helper(const wstring& src, const wstring& dst) noexcept {
                    return MoveFileW(src.c_str(), dst.c_str()) != 0;
                }

                void remove_file_helper(const wstring& file) {
                    bool dir = file_is_directory(file), ok;
                    if (dir)
                        ok = RemoveDirectoryW(file.data());
                    else
                        ok = DeleteFileW(file.data());
                    auto error = GetLastError();
                    if (! ok && error != ERROR_FILE_NOT_FOUND)
                        throw std::system_error(error, windows_category(), quote_file(file));
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

        // File system modifying functions

        void native_copy_file(const NativeString& src, const NativeString& dst, uint32_t flags) {
            bool overwrite = (flags & fs_overwrite) != 0, recurse = (flags & fs_recurse) != 0;
            if (! native_file_exists(src))
                throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), quote_file(src));
            if (src == dst || file_id(src) == file_id(dst))
                throw std::system_error(std::make_error_code(std::errc::file_exists), quote_file(dst));
            if (native_file_is_directory(src) && ! recurse)
                throw std::system_error(std::make_error_code(std::errc::is_a_directory), quote_file(src));
            if (native_file_exists(dst)) {
                if (! overwrite || (native_file_is_directory(dst) && ! recurse))
                    throw std::system_error(std::make_error_code(std::errc::file_exists), quote_file(dst));
                native_remove_file(dst, fs_recurse);
            }
            if (native_file_is_symlink(src)) {
                auto target = native_resolve_symlink(src);
                native_make_symlink(target, dst, 0);
            } else if (native_file_is_directory(src)) {
                native_make_directory(dst, 0);
                for (auto& child: directory(src, fs_hidden))
                    native_copy_file(file_path(src, child), file_path(dst, child), fs_recurse);
            } else {
                Cstdio in(src, false), out(dst, true);
                vector<char> buf(16384);
                while (! feof(in)) {
                    errno = 0;
                    size_t n = fread(buf.data(), 1, buf.size(), in);
                    int error = errno;
                    if (error)
                        throw std::system_error(error, std::generic_category(), quote_file(src));
                    if (n) {
                        errno = 0;
                        fwrite(buf.data(), 1, n, out);
                        int error = errno;
                        if (error)
                            throw std::system_error(error, std::generic_category(), quote_file(dst));
                    }
                }
            }
        }

        void native_make_directory(const NativeString& dir, uint32_t flags) {
            int error = mkdir_helper(dir);
            if (error == 0)
                return;
            if (error == fail_exists) {
                if (native_file_is_directory(dir))
                    return;
                if (flags & fs_overwrite) {
                    native_remove_file(dir, 0);
                    error = 0;
                }
            } else if (error == fail_no_parent && (flags & fs_recurse) && ! dir.empty()) {
                auto parent = split_path(dir).first;
                if (parent != dir) {
                    native_make_directory(parent, flags);
                    error = 0;
                }
            }
            if (error == 0)
                error = mkdir_helper(dir);
            if (error != 0)
                throw std::system_error(error, std::generic_category(), quote_file(dir));
        }

        void native_make_symlink(const NativeString& file, const NativeString& link, uint32_t flags) {
            if (native_file_is_symlink(link)) {
                try {
                    if (native_resolve_symlink(link) == file)
                        return;
                }
                catch (const std::system_error&) {}
            }
            if ((flags & fs_overwrite) && native_file_exists(link))
                native_remove_file(link, flags);
            make_symlink_helper(file, link, flags);
        }

        void native_move_file(const NativeString& src, const NativeString& dst, uint32_t flags) {
            bool overwrite = (flags & fs_overwrite) != 0, recurse = (flags & fs_recurse) != 0;
            if (! native_file_exists(src))
                throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), quote_file(src));
            if (src == dst)
                return;
            if (native_file_exists(dst) && file_id(src) != file_id(dst)) {
                if (! overwrite || (native_file_is_directory(dst) && ! recurse))
                    throw std::system_error(std::make_error_code(std::errc::file_exists), quote_file(dst));
                native_remove_file(dst, fs_recurse);
            }
            if (! move_file_helper(src, dst)) {
                native_copy_file(src, dst, fs_recurse);
                native_remove_file(src, fs_recurse);
            }
        }

        void native_remove_file(const NativeString& file, uint32_t flags) {
            if ((flags & fs_recurse) && native_file_is_directory(file) && ! native_file_is_symlink(file))
                for (auto child: directory(file, fs_fullname | fs_hidden))
                    native_remove_file(child, fs_recurse);
            remove_file_helper(file);
        }

        // Directory iterators

        void DirectoryStage2::init2(const NativeString& dir, uint32_t flags) {
            init1(dir);
            fset = flags;
            if ((fset & fs_fullname) || ! (fset & fs_hidden)) {
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
                if ((fset & fs_unicode) && ! valid_string(leaf()))
                    continue;
                if (! (fset & fs_dotdot) && (leaf() == link1 || leaf() == link2))
                    continue;
                current = prefix + leaf();
                if (! (fset & fs_hidden) && file_is_hidden(current))
                    continue;
                if (! (fset & fs_fullname))
                    current = leaf();
                break;
            }
        }

    }

}
