#include "unicorn/file.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include "unicorn/regex.hpp"
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <exception>
#include <random>
#include <system_error>
#include <type_traits>
#include <vector>

#ifdef _XOPEN_SOURCE
    #include <dirent.h>
    #include <pwd.h>
    #include <sys/types.h>
    #include <unistd.h>
#else
    #include <windows.h>
#endif

using namespace RS::Unicorn::Literals;
using namespace std::literals;

namespace RS {

    namespace Unicorn {

        namespace {

            template <typename C> U8string quote_file(const std::basic_string<C>& name) { return quote(to_utf8(name)); }
            U8string file_pair(const NativeString& f1, const NativeString& f2) { return quote_file(f1) + " -> " + quote_file(f2); }

            #ifdef _XOPEN_SOURCE

                class Cstdio {
                public:
                    Cstdio(const std::string& file, bool write) {
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

            #else

                using HandleTarget = std::remove_pointer_t<HANDLE>;

                class Cstdio {
                public:
                    Cstdio(const std::wstring& file, bool write) {
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

                bool is_ascii_letter(wchar_t c) noexcept {
                    return (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z');
                }

                uint32_t get_attributes(const std::wstring& file) {
                    auto rc = GetFileAttributes(file.data());
                    return rc == INVALID_FILE_ATTRIBUTES ? 0 : rc;
                }

                bool is_drive(const std::wstring& file) {
                    auto rc = GetDriveType(file.data());
                    return rc != DRIVE_NO_ROOT_DIR && rc != DRIVE_UNKNOWN;
                }

            #endif

        }

        // Types

        #ifdef _XOPEN_SOURCE

            size_t FileId::hash() const noexcept {
                return hash_value(hi, lo);
            }

            bool operator==(const FileId& lhs, const FileId& rhs) noexcept {
                return lhs.hi == rhs.hi && lhs.lo == rhs.lo;
            }

            bool operator<(const FileId& lhs, const FileId& rhs) noexcept {
                return lhs.hi < rhs.hi || (lhs.hi == rhs.hi && lhs.lo < rhs.lo);
            }

            std::istream& operator>>(std::istream& in, FileId& f) {
                U8string s(32, '0');
                for (char& c: s) {
                    auto x = in.get();
                    if (x == EOF)
                        break;
                    c = char(x);
                }
                f.hi = hexnum(s.substr(0, 16));
                f.lo = hexnum(s.substr(16, 16));
                return in;
            }

            std::ostream& operator<<(std::ostream& out, const FileId& f) {
                return out << hex(f.hi, 16) << hex(f.lo, 16);
            }

        #endif

        // File name operations

        namespace {

            constexpr const char* posix_root             = R"(/{2,}[^/]+/?|/+)";
            constexpr const char* windows_absolute       = R"((\\\\\?\\)*([A-Z]:\\|\\{2,}(?=[^?\\])))";
            constexpr const char* windows_illegal_names  = R"((AUX|COM[1-9]|CON|LPT[1-9]|NUL|PRN)(\.[^.]*)?)";
            constexpr uint32_t match_flags               = rx_byte | rx_caseless | rx_noautocapture;

            #ifndef _XOPEN_SOURCE
                constexpr const char* windows_root            = R"((\\\\\?\\)*([A-Z]:\\|\\{2,}[^?\\]+\\?|\\+))";
                constexpr const char* windows_drive_absolute  = R"(\\[^\\])";
                constexpr const char* windows_drive_relative  = R"([A-Z]:(?!\\))";
            #endif

        }

        bool is_legal_mac_leaf_name(const U8string& file) {
            return valid_string(file) && is_legal_posix_leaf_name(file);
        }

        bool is_legal_mac_path_name(const U8string& file) {
            return valid_string(file) && is_legal_posix_path_name(file);
        }

        bool is_legal_posix_leaf_name(const U8string& file) {
            return ! file.empty() && file.find_first_of("\0/"s) == npos;
        }

        bool is_legal_posix_path_name(const U8string& file) {
            if (file.empty() || file.find('\0') != npos)
                return false;
            size_t i = file.find_first_not_of('/');
            return file.find("//", i) == npos;
        }

        bool is_legal_windows_leaf_name(const std::wstring& file) {
            static const std::wstring illegal_chars = L"\0\"*:<>?|/\\"s;
            static const Regex illegal_pattern(windows_illegal_names, match_flags);
            return ! file.empty() && file.find_first_of(illegal_chars) == npos && ! illegal_pattern.match(to_utf8(file));
        }

        bool is_legal_windows_path_name(const std::wstring& file) {
            static const U8string illegal_chars = "\0\"*:<>?|"s;
            static const Regex abs_pattern(windows_absolute, match_flags);
            static const Regex illegal_pattern(R"(([:\\]|^))"s + windows_illegal_names + R"(([:\\]|$))"s, match_flags);
            if (file.empty())
                return false;
            auto norm = UnicornDetail::normalize_path(to_utf8(file));
            auto match = abs_pattern.anchor(norm);
            if (! match && norm[0] == '\\')
                return false;
            size_t skip = match.count();
            return norm.find_first_of(illegal_chars, skip) == npos
                && norm.find("\\\\"s, skip) == npos
                && ! illegal_pattern.search(norm);
        }

        #ifdef _XOPEN_SOURCE

            bool file_is_absolute(const U8string& file) {
                return file[0] == '/';
            }

            bool file_is_relative(const U8string& file) {
                return ! file.empty() && ! file_is_absolute(file);
            }

            bool file_is_root(const U8string& file) {
                static const Regex root_pattern(posix_root, match_flags);
                return bool(root_pattern.match(file));
            }

        #else

            bool file_is_absolute(const std::wstring& file) {
                static const Regex abs_pattern(windows_absolute, match_flags);
                return bool(abs_pattern.anchor(to_utf8(file)));
            }

            bool file_is_relative(const std::wstring& file) {
                return ! file.empty() && ! file_is_absolute(file)
                    && ! file_is_drive_absolute(file) && ! file_is_drive_relative(file);
            }

            bool file_is_drive_absolute(const std::wstring& file) {
                static const Regex drive_abs_pattern(windows_drive_absolute, match_flags);
                return bool(drive_abs_pattern.anchor(to_utf8(file)));
            }

            bool file_is_drive_relative(const std::wstring& file) {
                static const Regex drive_rel_pattern(windows_drive_relative, match_flags);
                return bool(drive_rel_pattern.anchor(to_utf8(file)));
            }

            bool file_is_root(const std::wstring& file) {
                static const Regex root_pattern(windows_root, match_flags);
                return bool(root_pattern.match(to_utf8(file)));
            }

        #endif

        std::pair<U8string, U8string> split_path(const U8string& file, uint32_t flags) {
            auto nfile = UnicornDetail::normalize_path(file);
            auto cut = nfile.find_last_of(file_delimiter);
            #ifndef _XOPEN_SOURCE
                if (cut == 2 && file[1] == ':' && ascii_isalpha(file[0]))
                    return {nfile.substr(0, 3), nfile.substr(3, npos)};
                else if (cut == npos && file[1] == ':' && ascii_isalpha(file[0]))
                    return {nfile.substr(0, 2), nfile.substr(2, npos)};
            #endif
            if (cut == npos) {
                return {{}, nfile};
            } else if (cut == 0) {
                return {nfile.substr(0, 1), nfile.substr(1, npos)};
            } else {
                size_t cut1 = flags & File::fullname ? cut + 1 : cut;
                return {nfile.substr(0, cut1), nfile.substr(cut + 1, npos)};
            }
        }

        std::pair<U8string, U8string> split_file(const U8string& file) {
            auto nfile = UnicornDetail::normalize_path(file);
            auto cut = nfile.find_last_of(file_delimiter);
            #ifndef _XOPEN_SOURCE
                if (cut == npos && ascii_isalpha(file[0]) && file[1] == ':')
                    cut = 1;
            #endif
            if (cut == npos)
                cut = 0;
            else
                ++cut;
            if (cut >= nfile.size())
                return {};
            auto dot = nfile.find_last_of('.');
            if (dot > cut && dot != npos)
                return {nfile.substr(cut, dot - cut), nfile.substr(dot, npos)};
            else
                return {nfile.substr(cut, npos), {}};
        }

        #ifndef _XOPEN_SOURCE

            std::pair<std::wstring, std::wstring> split_path(const std::wstring& file, uint32_t flags) {
                auto nfile = UnicornDetail::normalize_path(file);
                auto cut = nfile.find_last_of(native_file_delimiter);
                if (cut == 2 && is_ascii_letter(char(file[0])) && file[1] == L':') {
                    return {nfile.substr(0, 3), nfile.substr(3, npos)};
                } else if (cut == npos && is_ascii_letter(char(file[0])) && file[1] == L':') {
                    return {nfile.substr(0, 2), nfile.substr(2, npos)};
                } else if (cut == npos) {
                    return {{}, nfile};
                } else if (cut == 0) {
                    return {nfile.substr(0, 1), nfile.substr(1, npos)};
                } else {
                    size_t cut1 = flags & File::fullname ? cut + 1 : cut;
                    return {nfile.substr(0, cut1), nfile.substr(cut + 1, npos)};
                }
            }

            std::pair<std::wstring, std::wstring> split_file(const std::wstring& file) {
                auto nfile = UnicornDetail::normalize_path(file);
                auto cut = nfile.find_last_of(native_file_delimiter);
                if (cut == npos && is_ascii_letter(char(file[0])) && file[1] == L':')
                    cut = 1;
                if (cut == npos)
                    cut = 0;
                else
                    ++cut;
                if (cut >= nfile.size())
                    return {};
                auto dot = nfile.find_last_of(L'.');
                if (dot > cut && dot != npos)
                    return {nfile.substr(cut, dot - cut), nfile.substr(dot, npos)};
                else
                    return {nfile.substr(cut, npos), {}};
            }

        #endif

        // File system query functions

        namespace {

            template <typename C>
            std::basic_string<C> trim_dots(std::basic_string<C> file) {
                static const std::basic_string<C> sds = {native_file_delimiter, C('.'), native_file_delimiter};
                while (file.size() > 2 && file[0] == C('.') && file[1] == native_file_delimiter)
                    file.erase(0, 2);
                while (file.size() > 2 && file.end()[-1] == C('.') && file.end()[-2] == native_file_delimiter)
                    file.resize(file.size() - 2);
                size_t pos = 0;
                while (pos < file.size()) {
                    pos = file.find(sds, pos);
                    if (pos == npos)
                        break;
                    file.erase(pos, 2);
                }
                if (! file.empty() && ! file_is_root(file) && file.back() == native_file_delimiter)
                    file.pop_back();
                return file;
            }

        }

        #ifdef _XOPEN_SOURCE

            namespace {

                template <typename Getpw, typename T>
                U8string call_getpw(Getpw* getpw, T arg) {
                    std::vector<char> workbuf(1024);
                    passwd pwdbuf;
                    passwd* pwdptr = nullptr;
                    for (;;) {
                        int rc = getpw(arg, &pwdbuf, &workbuf[0], workbuf.size(), &pwdptr);
                        if (rc == 0)
                            return cstr(pwdptr->pw_dir);
                        if (rc != ERANGE)
                            return {};
                        workbuf.resize(2 * workbuf.size());
                    }
                }

                U8string user_home(const U8string& user) {
                    U8string home;
                    if (user.empty()) {
                        home = cstr(getenv("HOME"));
                        if (home.empty())
                            home = call_getpw(getpwuid_r, getuid());
                    } else {
                        home = call_getpw(getpwnam_r, user.data());
                    }
                    return home;
                }

            }

            U8string current_directory() {
                U8string name(256, '\0');
                for (;;) {
                    if (getcwd(&name[0], name.size()))
                        break;
                    if (errno != ERANGE)
                        throw std::system_error(errno, std::generic_category());
                    name.resize(2 * name.size());
                }
                size_t n = name.find('\0');
                if (n != npos)
                    name.resize(n);
                name.shrink_to_fit();
                return name;
            }

            bool file_exists(const U8string& file) {
                struct stat s;
                return stat(file.data(), &s) == 0;
            }

            FileId file_id(const U8string& file, uint32_t flags) {
                struct stat s;
                int rc;
                if (flags & File::follow)
                    rc = stat(file.data(), &s);
                else
                    rc = lstat(file.data(), &s);
                if (rc != 0)
                    return {};
                auto dev = uint64_t(s.st_dev);
                auto ino = uint64_t(s.st_ino);
                return {dev, ino};
            }

            bool file_is_directory(const U8string& file) {
                struct stat s;
                return stat(file.data(), &s) == 0 && S_ISDIR(s.st_mode);
            }

            bool file_is_hidden(const U8string& file) {
                auto leaf = split_path(file).second;
                return leaf[0] == '.' && leaf != "." && leaf != "..";
            }

            bool file_is_symlink(const U8string& file) {
                struct stat s;
                return lstat(file.data(), &s) == 0 && S_ISLNK(s.st_mode);
            }

            uint64_t file_size(const U8string& file, uint32_t flags) {
                struct stat s;
                if (lstat(file.data(), &s) != 0)
                    return 0;
                auto bytes = uint64_t(s.st_size);
                if ((flags & File::recurse) && S_ISDIR(s.st_mode))
                    for (auto& child: directory(file, File::fullname | File::hidden))
                        bytes += file_size(child, File::recurse);
                return bytes;
            }

            U8string resolve_path(const U8string& file) {
                U8string result = file;
                size_t pos = 0;
                // Three or more leading slashes are equivalent to one (Posix)
                if (result.size() >= 3) {
                    pos = result.find_first_not_of('/');
                    if (pos >= 3)
                        result.erase(0, pos - 1);
                }
                // Elsewhere replace all multiple slashes with one
                pos = 1;
                while (pos < result.size()) {
                    pos = result.find("//", pos);
                    if (pos == npos)
                        break;
                    result.erase(pos, 1);
                }
                // Replace ~[user] with the home directory
                if (result[0] == '~') {
                    U8string user, tail;
                    pos = result.find('/');
                    user = result.substr(1, pos - 1);
                    if (pos != npos)
                        tail = result.substr(pos + 1, npos);
                    U8string head = user_home(user);
                    if (! head.empty())
                        result = file_path(head, tail);
                }
                // Insert the current directory
                if (! result.empty() && file_is_relative(result))
                    result = file_path(current_directory(), result);
                // Strip redundant dot entries in path
                return trim_dots(result);
            }

            U8string resolve_symlink(const U8string& file) {
                if (! file_is_symlink(file))
                    return file;
                std::vector<char> buf(256);
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
                        return U8string(buf.data(), rc);
                    buf.resize(2 * buf.size());
                }
            }

        #else

            namespace {

                std::wstring get_long_path(const std::wstring& file) {
                    if (file.empty())
                        return {};
                    std::wstring buf;
                    buf.resize(1024);
                    for (;;) {
                        auto rc = GetLongPathNameW(file.data(), &buf[0], buf.size());
                        if (rc == 0)
                            return file;
                        if (rc < buf.size()) {
                            buf.resize(rc);
                            return buf;
                        }
                        buf.resize(2 * buf.size());
                    }
                }

                std::wstring get_full_path(const std::wstring& file) {
                    if (file.empty())
                        return {};
                    std::wstring buf;
                    buf.resize(1024);
                    for (;;) {
                        auto rc = GetFullPathNameW(file.data(), buf.size(), &buf[0], nullptr);
                        if (rc == 0)
                            return file;
                        if (rc < buf.size()) {
                            buf.resize(rc);
                            return buf;
                        }
                        buf.resize(2 * buf.size());
                    }
                }

            }

            std::wstring native_current_directory() {
                std::wstring name(256, L'\0');
                for (;;) {
                    auto rc = GetCurrentDirectoryW(name.size(), &name[0]);
                    if (rc == 0)
                        throw std::system_error(GetLastError(), windows_category());
                    if (rc < name.size()) {
                        name.resize(rc);
                        break;
                    }
                    name.resize(2 * name.size());
                }
                return name;
            }

            bool file_exists(const std::wstring& file) {
                return file == L"." || file == L".." || get_attributes(file) || is_drive(file);
            }

            FileId file_id(const std::wstring& file, uint32_t flags) {
                std::wstring f;
                try {
                    if (flags & File::follow)
                        f = resolve_symlink(file);
                    else
                        f = file;
                }
                catch (const std::exception&) {
                    return 0;
                }
                std::shared_ptr<HandleTarget> handle(CreateFileW(f.data(), GENERIC_READ, FILE_SHARE_READ,
                    nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS , nullptr), CloseHandle);
                if (handle.get() == INVALID_HANDLE_VALUE)
                    return 0;
                BY_HANDLE_FILE_INFORMATION info;
                memset(&info, 0, sizeof(info));
                if (! GetFileInformationByHandle(handle.get(), &info))
                    return 0;
                uint64_t hi = info.nFileIndexHigh;
                uint64_t lo = info.nFileIndexLow;
                return (hi << 32) + lo;
            }

            bool file_is_directory(const std::wstring& file) {
                return file == L"." || file == L".."
                    || (get_attributes(file) & FILE_ATTRIBUTE_DIRECTORY)
                    || is_drive(file);
            }

            bool file_is_hidden(const std::wstring& file) {
                return ! file_is_root(file)
                    && get_attributes(file) & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
            }

            bool file_is_symlink(const std::wstring& file) {
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

            uint64_t file_size(const std::wstring& file, uint32_t flags) {
                WIN32_FILE_ATTRIBUTE_DATA info;
                if (! GetFileAttributesEx(file.data(), GetFileExInfoStandard, &info))
                    return 0;
                auto bytes = (uint64_t(info.nFileSizeHigh) << 32) + uint64_t(info.nFileSizeLow);
                if (flags & File::recurse)
                    for (auto& child: directory(file, File::fullname | File::hidden))
                        bytes += file_size(child, File::recurse);
                return bytes;
            }

            std::wstring resolve_path(const std::wstring& file) {
                auto path = trim_dots(get_full_path(get_long_path(UnicornDetail::normalize_path(file))));
                path.shrink_to_fit();
                return path;
            }

            std::wstring resolve_symlink(const std::wstring& file) {
                if (! file_is_symlink(file))
                    return file;
                std::shared_ptr<HandleTarget> handle(CreateFileW(file.data(), GENERIC_READ, FILE_SHARE_READ,
                    nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS , nullptr), CloseHandle);
                if (handle.get() == INVALID_HANDLE_VALUE) {
                    auto error = GetLastError();
                    throw std::system_error(error, std::generic_category(), quote_file(file));
                }
                std::vector<FILE_NAME_INFO> buf(100);
                for (;;) {
                    if (GetFileInformationByHandleEx(handle.get(), FileNameInfo, &buf[0], buf.size() * sizeof(FILE_NAME_INFO)))
                        return std::wstring(buf[0].FileName, buf[0].FileNameLength);
                    auto error = GetLastError();
                    if (error != ERROR_INSUFFICIENT_BUFFER)
                        throw std::system_error(error, std::generic_category(), quote_file(file));
                    buf.resize(2 * buf.size());
                }
            }

        #endif

        // File system modifying functions

        namespace {

            #ifdef _XOPEN_SOURCE

                constexpr int fail_exists = EEXIST;
                constexpr int fail_not_found = ENOENT;

                int mkdir_helper(const U8string& dir) {
                    errno = 0;
                    mkdir(dir.data(), 0777);
                    return errno;
                }

                void make_symlink_helper(const U8string& file, const U8string& link, uint32_t /*flags*/) {
                    if (symlink(file.data(), link.data()) == 0)
                        return;
                    int error = errno;
                    throw std::system_error(error, std::generic_category(), file_pair(link, file));
                }

                bool move_file_helper(const U8string& src, const U8string& dst) noexcept {
                    return rename(src.data(), dst.data()) == 0;
                }

                void remove_file_helper(const U8string& file) {
                    int rc = std::remove(file.data());
                    int error = errno;
                    if (rc != 0 && error != ENOENT)
                        throw std::system_error(error, std::generic_category(), quote_file(file));
                }

            #else

                constexpr int fail_exists = ERROR_ALREADY_EXISTS;
                constexpr int fail_not_found = ERROR_PATH_NOT_FOUND;

                int mkdir_helper(const std::wstring& dir) {
                    SetLastError(0);
                    CreateDirectoryW(dir.c_str(), nullptr);
                    return GetLastError();
                }

                void make_symlink_helper(const std::wstring& file, const std::wstring& link, uint32_t flags) {
                    if (! (flags & File::overwrite) && file_exists(link))
                        throw std::system_error(ERROR_ALREADY_EXISTS, windows_category(), quote_file(link));
                    DWORD wflags = file_is_directory(file) ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0;
                    if (CreateSymbolicLinkW(link.data(), file.data(), wflags))
                        return;
                    auto error = GetLastError();
                    throw std::system_error(error, windows_category(), file_pair(link, file));
                }

                bool move_file_helper(const std::wstring& src, const std::wstring& dst) noexcept {
                    return MoveFileW(src.c_str(), dst.c_str()) != 0;
                }

                // File deletion on Windows is asynchronous; DeleteFile() and
                // RemoveDirectory() may return before the file is actually
                // deleted. Borrowing the idea from Boost here: rename the file to
                // a random name first, so the original is gone before we return.

                class RandomName {
                public:
                    RandomName():
                        rng(uint32_t(std::chrono::high_resolution_clock::now().time_since_epoch().count())) {}
                    U8string operator()() {
                        auto lock = make_lock(mtx);
                        auto uuid = ruuid(rng);
                        return uuid.str();
                    }
                private:
                    Mutex mtx;
                    std::minstd_rand rng;
                    RandomUuid ruuid;
                };

                void remove_file_helper(const std::wstring& file) {
                    static RandomName rname;
                    std::wstring parent = split_path(file).first;
                    std::wstring tempname = file_path(parent, to_wstring(rname() + ".deleted"));
                    if (MoveFileW(file.data(), tempname.data()) == 0)
                        tempname = file;
                    bool dir = file_is_directory(tempname), ok;
                    if (dir)
                        ok = RemoveDirectoryW(tempname.data());
                    else
                        ok = DeleteFileW(tempname.data());
                    auto error = GetLastError();
                    if (! ok) {
                        MoveFileW(tempname.data(), file.data());
                        if (error != ERROR_FILE_NOT_FOUND)
                            throw std::system_error(error, windows_category(), quote_file(file));
                    }
                }

            #endif

        }

        void copy_file(const NativeString& src, const NativeString& dst, uint32_t flags) {
            bool overwrite = (flags & File::overwrite) != 0, recurse = (flags & File::recurse) != 0;
            if (! file_exists(src))
                throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), quote_file(src));
            if (src == dst || file_id(src) == file_id(dst))
                throw std::system_error(std::make_error_code(std::errc::file_exists), quote_file(dst));
            if (file_is_directory(src) && ! recurse)
                throw std::system_error(std::make_error_code(std::errc::is_a_directory), quote_file(src));
            if (file_exists(dst)) {
                if (! overwrite || (file_is_directory(dst) && ! recurse))
                    throw std::system_error(std::make_error_code(std::errc::file_exists), quote_file(dst));
                remove_file(dst, File::recurse);
            }
            if (file_is_symlink(src)) {
                auto target = resolve_symlink(src);
                make_symlink(target, dst, 0);
            } else if (file_is_directory(src)) {
                make_directory(dst, 0);
                for (auto& child: directory(src, File::hidden))
                    copy_file(file_path(src, child), file_path(dst, child), File::recurse);
            } else {
                Cstdio in(src, false), out(dst, true);
                std::vector<char> buf(16384);
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

        void make_directory(const NativeString& dir, uint32_t flags) {
            int error = mkdir_helper(dir);
            if (error == 0)
                return;
            if (error == fail_exists) {
                if (file_is_directory(dir))
                    return;
                if (flags & File::overwrite) {
                    remove_file(dir, 0);
                    error = 0;
                }
            } else if (error == fail_not_found && (flags & File::recurse) && ! dir.empty()) {
                auto parent = split_path(dir).first;
                if (parent != dir) {
                    make_directory(parent, flags);
                    error = 0;
                }
            }
            if (error == 0)
                error = mkdir_helper(dir);
            if (error != 0)
                throw std::system_error(error, std::generic_category(), quote_file(dir));
        }

        void make_symlink(const NativeString& file, const NativeString& link, uint32_t flags) {
            if (file_is_symlink(link)) {
                try {
                    if (resolve_symlink(link) == file)
                        return;
                }
                catch (const std::system_error&) {}
            }
            if ((flags & File::overwrite) && file_exists(link))
                remove_file(link, flags);
            make_symlink_helper(file, link, flags);
        }

        void move_file(const NativeString& src, const NativeString& dst, uint32_t flags) {
            bool overwrite = (flags & File::overwrite) != 0, recurse = (flags & File::recurse) != 0;
            if (! file_exists(src))
                throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), quote_file(src));
            if (src == dst)
                return;
            if (file_exists(dst) && file_id(src) != file_id(dst)) {
                if (! overwrite || (file_is_directory(dst) && ! recurse))
                    throw std::system_error(std::make_error_code(std::errc::file_exists), quote_file(dst));
                remove_file(dst, File::recurse);
            }
            if (! move_file_helper(src, dst)) {
                copy_file(src, dst, File::recurse);
                remove_file(src, File::recurse);
            }
        }

        void remove_file(const NativeString& file, uint32_t flags) {
            if ((flags & File::recurse) && file_is_directory(file) && ! file_is_symlink(file))
                for (auto child: directory(file, File::fullname | File::hidden))
                    remove_file(child, File::recurse);
            remove_file_helper(file);
        }

        // Directory iterators

        #ifdef _XOPEN_SOURCE

            struct NativeDirectoryIterator::impl_type {
                dirent entry;
                char padding[NAME_MAX + 1];
                DIR* dp;
                ~impl_type() { if (dp) closedir(dp); }
            };

            void NativeDirectoryIterator::do_init(const std::string& dir) {
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

            void NativeDirectoryIterator::do_next() {
                if (! impl)
                    return;
                dirent* ptr = nullptr;
                int rc = readdir_r(impl->dp, &impl->entry, &ptr);
                if (! rc && ptr)
                    leaf = impl->entry.d_name;
                else
                    impl.reset();
            }

        #else

            // On Windows we need to check first that the supplied file name
            // refers to a directory, because FindFirstFile() gives a false
            // positive for "file\*" when "file" exists but is not a directory.
            // There's a race condition here but there doesn't seem to be anything
            // we can do about it.

            struct NativeDirectoryIterator::impl_type {
                HANDLE handle;
                WIN32_FIND_DATAW info;
                bool first;
                ~impl_type() { if (handle) FindClose(handle); }
            };

            void NativeDirectoryIterator::do_init(const std::wstring& dir) {
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

            void NativeDirectoryIterator::do_next() {
                if (! impl)
                    return;
                if (! impl->first && ! FindNextFile(impl->handle, &impl->info)) {
                    impl.reset();
                    return;
                }
                impl->first = false;
                leaf = impl->info.cFileName;
            }

        #endif

        NativeDirectoryIterator::NativeDirectoryIterator(const NativeString& dir, uint32_t flags) {
            if ((flags & File::unicode) && ! valid_string(dir))
                return;
            auto normdir = UnicornDetail::normalize_path(dir);
            do_init(normdir);
            fset = flags;
            if ((fset & File::fullname) || ! (fset & File::hidden)) {
                prefix = normdir;
                if (! prefix.empty() && prefix.back() != native_file_delimiter)
                    prefix += native_file_delimiter;
            }
            ++*this;
        }

        NativeDirectoryIterator& NativeDirectoryIterator::operator++() {
            static const NativeString link1(1, NativeCharacter('.'));
            static const NativeString link2(2, NativeCharacter('.'));
            current.clear();
            for (;;) {
                do_next();
                if (! impl)
                    break;
                if ((fset & File::unicode) && ! valid_string(leaf))
                    continue;
                if (! (fset & File::dotdot) && (leaf == link1 || leaf == link2))
                    continue;
                current = prefix + leaf;
                if (! (fset & File::hidden) && file_is_hidden(current))
                    continue;
                if (! (fset & File::fullname))
                    current = leaf;
                break;
            }
            return *this;
        }

    }

}
