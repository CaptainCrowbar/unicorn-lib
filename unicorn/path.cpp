#include "unicorn/path.hpp"
#include "unicorn/string.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <ios>
#include <stdexcept>
#include <system_error>

#ifdef __APPLE__
    #include <Availability.h>
#endif

#ifdef _XOPEN_SOURCE
    #include <dirent.h>
    #include <fcntl.h>
    #include <pwd.h>
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>
    #define CX(c) c
    #define FX(f) f
#else
    #include <regex>
    #include <windows.h>
    #define CX(c) L##c
    #define FX(f) _w##f
#endif

using namespace std::chrono;
using namespace std::literals;

namespace RS::Unicorn {

    namespace {

        #ifdef _XOPEN_SOURCE

            struct StatResult {
                struct stat st;
                bool ok;
            };

            StatResult get_stat(const std::string& file, Path::flag_type flags) noexcept {
                StatResult result;
                result.ok = (flags & Path::no_follow ? lstat : stat)(file.data(), &result.st) == 0;
                if (! result.ok)
                    memset(&result.st, 0, sizeof(result.st));
                return result;
            }

            Path get_user_home(std::string user) {
                const char* envptr = nullptr;
                if (user.empty()) {
                    envptr = getenv("HOME");
                    if (envptr && *envptr)
                        return Path(envptr);
                }
                std::string workbuf(1024, '\0');
                passwd pwdbuf;
                passwd* pwdptr = nullptr;
                int rc = 0;
                for (;;) {
                    if (user.empty())
                        rc = getpwuid_r(geteuid(), &pwdbuf, workbuf.data(), workbuf.size(), &pwdptr);
                    else
                        rc = getpwnam_r(user.data(), &pwdbuf, workbuf.data(), workbuf.size(), &pwdptr);
                    if (rc != ERANGE)
                        break;
                    workbuf.resize(2 * workbuf.size());
                }
                if (pwdptr && pwdptr->pw_dir && *pwdptr->pw_dir)
                    return Path(pwdptr->pw_dir);
                if (user.empty()) {
                    envptr = getenv("USER");
                    if (envptr && *envptr)
                        user = envptr;
                    else if (pwdptr && pwdptr->pw_name && *pwdptr->pw_name)
                        user = pwdptr->pw_name;
                    else
                        throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory));
                }
                #ifdef __APPLE__
                    static const Path dir("/Users");
                #else
                    static const Path dir("/home");
                #endif
                return dir / user;
            }

            system_clock::time_point timespec_to_timepoint(const timespec& ts) noexcept {
                using D = system_clock::duration;
                return system_clock::time_point() + duration_cast<D>(seconds(ts.tv_sec)) + duration_cast<D>(nanoseconds(ts.tv_nsec));
            }

            #if defined(__APPLE__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 101300

                timeval timepoint_to_timeval(const system_clock::time_point& tp) noexcept {
                    static constexpr int64_t M = 1'000'000;
                    auto d = tp - system_clock::time_point();
                    int64_t usec = duration_cast<microseconds>(d).count();
                    return {time_t(usec / M), suseconds_t(usec % M)};
                }

            #else

                timespec timepoint_to_timespec(const system_clock::time_point& tp) noexcept {
                    static constexpr int64_t G = 1'000'000'000ll;
                    auto d = tp - system_clock::time_point();
                    int64_t nsec = duration_cast<nanoseconds>(d).count();
                    return {time_t(nsec / G), long(nsec % G)};
                }

            #endif

        #else

            uint32_t get_attributes(const std::wstring& file) noexcept {
                auto rc = GetFileAttributesW(file.data());
                return rc == INVALID_FILE_ATTRIBUTES ? 0 : rc;
            }

            WIN32_FILE_ATTRIBUTE_DATA get_attributes_ex(const std::wstring& file) noexcept {
                WIN32_FILE_ATTRIBUTE_DATA info;
                memset(&info, 0, sizeof(info));
                GetFileAttributesExW(file.data(), GetFileExInfoStandard, &info);
                return info;
            }

            constexpr int64_t filetime_freq = 10'000'000;        // FILETIME ticks (100 ns) per second
            constexpr int64_t windows_epoch = 11'644'473'600ll;  // Windows epoch (1601) to Unix epoch (1970) in seconds

            system_clock::time_point filetime_to_timepoint(const FILETIME& ft) noexcept {
                int64_t ticks = (int64_t(ft.dwHighDateTime) << 32) + int64_t(ft.dwLowDateTime);
                int64_t sec = ticks / filetime_freq - windows_epoch;
                int64_t nsec = 100ll * (ticks % filetime_freq);
                return system_clock::from_time_t(time_t(sec)) + duration_cast<system_clock::duration>(nanoseconds(nsec));
            }

            FILETIME timepoint_to_filetime(const system_clock::time_point& tp) noexcept {
                auto unix_time = tp - system_clock::from_time_t(0);
                uint64_t nsec = duration_cast<nanoseconds>(unix_time).count();
                uint64_t ticks = nsec / 100ll + filetime_freq * windows_epoch;
                return {uint32_t(ticks & 0xfffffffful), uint32_t(ticks >> 32)};
            }

        #endif

        Path get_current_directory(bool checked) {
            #ifdef _XOPEN_SOURCE
                std::string buf(256, '\0');
                for (;;) {
                    auto rc = getcwd(buf.data(), buf.size());
                    int err = errno;
                    if (rc)
                        return rc;
                    else if (err == ERANGE)
                        buf.resize(2 * buf.size());
                    else if (checked)
                        throw std::system_error(err, std::generic_category());
                    else
                        return {};
                }
            #else
                auto size = GetCurrentDirectoryW(0, nullptr);
                std::wstring buf(size, L'\0');
                auto rc = GetCurrentDirectoryW(size, buf.data());
                auto err = GetLastError();
                if (rc > 0)
                    return buf.data();
                else if (checked)
                    throw std::system_error(err, std::system_category());
                else
                    return {};
            #endif
        }

    }

    // Path name functions

    Ustring Path::as_url(flag_type flags) const {
        if (! is_absolute())
            throw std::invalid_argument("File URL must be an absolute path");
        Ustring prefix = "file://", suffix = name(flags);
        #ifndef _XOPEN_SOURCE
            std::replace(suffix.begin(), suffix.end(), '\\', '/');
        #endif
        if (suffix[0] != '/')
            prefix += '/';
        return prefix + suffix;
    }

    Strings Path::breakdown() const {
        auto os_parts = os_breakdown();
        Strings u8_parts(os_parts.size());
        std::transform(os_parts.begin(), os_parts.end(), u8_parts.begin(), [=] (auto& s) { return to_utf8(s, Utf::replace); });
        return u8_parts;
    }

    std::vector<Path::string_type> Path::os_breakdown() const {
        std::vector<string_type> parts;
        if (empty())
            return parts;
        auto root = find_root(true);
        if (! root.empty())
            parts.push_back(string_type(root));
        size_t p = root.size(), q = 0, size = filename.size();
        while (p < size) {
            q = filename.find(native_delimiter, p);
            if (q == npos)
                q = size;
            parts.push_back(filename.substr(p, q - p));
            p = q + 1;
        }
        return parts;
    }

    Path Path::change_ext(Uview new_ext) const {
        if (empty() || find_root(true).size() == filename.size())
            throw std::invalid_argument("Can't change file extension: " + quote(name()));
        string_type prefix = filename;
        prefix.resize(prefix.size() - find_base_ext().second.size());
        auto suffix = recode<character_type>(Ustring(new_ext), Utf::ignore);
        if (! suffix.empty() && suffix[0] != CX('.'))
            prefix += CX('.');
        return prefix + suffix;
    }

    bool Path::is_absolute() const noexcept {
        #ifdef _XOPEN_SOURCE
            return filename[0] == '/';
        #else
            static const std::wregex pattern(LR"(^(\\\\\?\\)?([A-Z]:\\|\\{2,}(?=[^?\\])))", std::regex_constants::nosubs | std::regex_constants::optimize);
            return std::regex_search(filename, pattern);
        #endif
    }

    bool Path::is_drive_absolute() const noexcept {
        #ifdef _XOPEN_SOURCE
            return false;
        #else
            return ! filename.empty() && filename[0] == L'\\' && filename[1] != L'\\';
        #endif
    }

    bool Path::is_drive_relative() const noexcept {
        #ifdef _XOPEN_SOURCE
            return false;
        #else
            return filename.size() >= 2 && filename[0] >= L'A' && filename[0] <= L'Z' && filename[1] == L':' && filename[2] != L'\\';
        #endif
    }

    bool Path::is_relative() const noexcept {
        return ! empty() && ! is_absolute() && ! is_drive_absolute() && ! is_drive_relative();
    }

    bool Path::is_leaf() const noexcept {
        if (filename.empty())
            return false;
        #ifdef _XOPEN_SOURCE
            return filename.find('/') == npos;
        #else
            return filename.find_first_of(L":\\") == npos;
        #endif
    }

    bool Path::is_legal() const noexcept {
        if (filename.find(character_type(0)) != npos)
            return false;
        #ifndef _XOPEN_SOURCE
            auto root = find_root(true);
            if (root.empty() && filename[0] == L'\\')
                return false;
            if (filename.find_first_of(L"\"*:<>?|", root.size()) != npos)
                return false;
        #endif
        return true;
    }

    bool Path::is_root() const noexcept {
        return ! filename.empty() && find_root(false).size() == filename.size();
    }

    Path::form Path::path_form() const noexcept {
        if (filename.empty())          return form::empty;
        else if (is_drive_absolute())  return form::drive_absolute;
        else if (is_drive_relative())  return form::drive_relative;
        else if (is_absolute())        return form::absolute;
        else                           return form::relative;
    }

    Path Path::relative_to(const Path& base) const {
        auto base_form = base.path_form(), this_form = path_form();
        if (! (base_form == form::absolute && this_form == form::absolute)
                && ! (base_form == form::relative && this_form == form::relative))
            throw std::invalid_argument("Invalid combination of arguments to Path::relative_to()");
        auto base_vec = base.os_breakdown(), this_vec = os_breakdown();
        if (this_form == form::absolute && base_vec[0] != this_vec[0])
            return *this;
        auto cuts = std::mismatch(base_vec.begin(), base_vec.end(), this_vec.begin(), this_vec.end());
        Path prefix;
        if (base_vec.end() > cuts.first) {
            size_t len = 3 * (base_vec.end() - cuts.first) - 1;
            prefix.filename.assign(len, CX('.'));
            for (size_t i = 2; i < len; i += 3)
                prefix.filename[i] = native_delimiter;
        }
        Path suffix = join(irange(cuts.second, this_vec.end()));
        Path rel_path = prefix / suffix;
        if (rel_path.empty())
            rel_path = ".";
        return rel_path;
    }

    std::pair<Ustring, Ustring> Path::split_leaf() const {
        auto os_parts = split_os_leaf();
        std::pair<Ustring, Ustring> parts;
        parts.first = to_utf8(os_parts.first, Utf::replace);
        parts.second = to_utf8(os_parts.second, Utf::replace);
        return parts;
    }

    std::pair<Path::string_type, Path::string_type> Path::split_os_leaf() const {
        auto base_ext = find_base_ext();
        return {string_type(base_ext.first), string_type(base_ext.second)};
    }

    std::pair<Path, Path> Path::split_path() const {
        auto leaf = find_leaf();
        auto dir = make_view(filename, 0, filename.size() - leaf.size());
        return {dir, leaf};
    }

    std::pair<Path, Path> Path::split_root() const {
        auto root = find_root(true);
        auto tail = make_view(filename, root.size());
        return {root, tail};
    }

    Path Path::common(const Path& lhs, const Path& rhs) {
        auto root1 = lhs.find_root(true);
        auto root2 = rhs.find_root(true);
        if (root1 != root2)
            return {};
        size_t cut = std::mismatch(lhs.filename.begin(), lhs.filename.end(), rhs.filename.begin(), rhs.filename.end()).first - lhs.filename.begin();
        if (cut == root1.size())
            return lhs.filename.substr(0, cut);
        else if ((cut == lhs.filename.size() || lhs.filename[cut] == native_delimiter) && (cut == rhs.filename.size() || rhs.filename[cut] == native_delimiter))
            return lhs.filename.substr(0, cut);
        do --cut;
            while (cut > root1.size() && lhs.filename[cut] != native_delimiter);
        return lhs.filename.substr(0, cut);
    }

    Path Path::join(const Path& lhs, const Path& rhs) {
        if (lhs.empty() || rhs.is_absolute())
            return rhs;
        auto result = lhs.os_name();
        auto root = lhs.find_root(true);
        if (root.size() < result.size() && result.back() != native_delimiter && ! rhs.empty())
            result += native_delimiter;
        result += rhs.os_view();
        return result;
    }

    Ustring to_str(Path::form f) {
        switch (f) {
            case Path::form::empty:           return "empty";
            case Path::form::absolute:        return "absolute";
            case Path::form::relative:        return "relative";
            case Path::form::drive_absolute:  return "drive_absolute";
            case Path::form::drive_relative:  return "drive_relative";
            default:                          return std::to_string(int(f));
        }
    }

    // File system query functions

    system_clock::time_point Path::access_time(flag_type flags) const noexcept {
        #if defined(__APPLE__)
            auto st = get_stat(filename, flags).st;
            return timespec_to_timepoint(st.st_atimespec);
        #elif defined(_XOPEN_SOURCE)
            auto st = get_stat(filename, flags).st;
            return timespec_to_timepoint(st.st_atim);
        #else
            (void)flags;
            return get_file_time(1);
        #endif
    }

    system_clock::time_point Path::create_time(flag_type flags) const noexcept {
        #if defined(__APPLE__)
            auto st = get_stat(filename, flags).st;
            return timespec_to_timepoint(st.st_birthtimespec);
        #elif defined(_XOPEN_SOURCE)
            (void)flags;
            return {};
        #else
            (void)flags;
            return get_file_time(0);
        #endif
    }

    system_clock::time_point Path::modify_time(flag_type flags) const noexcept {
        #if defined(__APPLE__)
            auto st = get_stat(filename, flags).st;
            return timespec_to_timepoint(st.st_mtimespec);
        #elif defined(_XOPEN_SOURCE)
            auto st = get_stat(filename, flags).st;
            return timespec_to_timepoint(st.st_mtim);
        #else
            (void)flags;
            return get_file_time(2);
        #endif
    }

    system_clock::time_point Path::status_time(flag_type flags) const noexcept {
        #if defined(__APPLE__)
            auto st = get_stat(filename, flags).st;
            return timespec_to_timepoint(st.st_ctimespec);
        #elif defined(_XOPEN_SOURCE)
            auto st = get_stat(filename, flags).st;
            return timespec_to_timepoint(st.st_ctim);
        #else
            (void)flags;
            return {};
        #endif
    }

    Path::directory_range Path::directory(flag_type flags) const {
        directory_iterator it(*this, flags);
        return {it, {}};
    }

    Path::deep_search_range Path::deep_search(flag_type flags) const {
        deep_search_iterator it(*this, flags);
        return {it, {}};
    }

    bool Path::exists(flag_type flags) const noexcept {
        #ifdef _XOPEN_SOURCE
            struct stat st;
            if (flags & no_follow)
                return lstat(c_name(), &st) == 0;
            else
                return stat(c_name(), &st) == 0;
        #else
            (void)flags;
            return get_attributes(filename);
        #endif
    }

    Path::id_type Path::id(flag_type flags) const noexcept {
        uint64_t device = 0, file = 0;
        #ifdef _XOPEN_SOURCE
            auto st = get_stat(filename, flags).st;
            device = uint64_t(st.st_dev);
            file = uint64_t(st.st_ino);
        #else
            (void)flags;
            auto handle = CreateFileW(c_name(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS , nullptr);
            if (handle != INVALID_HANDLE_VALUE) {
                BY_HANDLE_FILE_INFORMATION info;
                memset(&info, 0, sizeof(info));
                if (GetFileInformationByHandle(handle, &info)) {
                    device = uint64_t(info.dwVolumeSerialNumber);
                    file = (uint64_t(info.nFileIndexHigh) << 32) + uint64_t(info.nFileIndexLow);
                }
                CloseHandle(handle);
            }
        #endif
        return {device, file};
    }

    bool Path::is_directory(flag_type flags) const noexcept {
        #ifdef _XOPEN_SOURCE
            auto st = get_stat(filename, flags).st;
            return S_ISDIR(st.st_mode);
        #else
            (void)flags;
            return get_attributes(filename) & FILE_ATTRIBUTE_DIRECTORY;
        #endif
    }

    bool Path::is_file(flag_type flags) const noexcept {
        #ifdef _XOPEN_SOURCE
            auto st = get_stat(filename, flags).st;
            return S_ISREG(st.st_mode);
        #else
            (void)flags;
            auto attr = get_attributes(filename);
            return attr && ! (attr & (FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_DIRECTORY));
        #endif
    }

    bool Path::is_special(flag_type flags) const noexcept {
        #ifdef _XOPEN_SOURCE
            auto rc = get_stat(filename, flags);
            return rc.ok && ! S_ISDIR(rc.st.st_mode) && ! S_ISREG(rc.st.st_mode);
        #else
            (void)flags;
            return get_attributes(filename) & FILE_ATTRIBUTE_DEVICE;
        #endif
    }

    bool Path::is_hidden() const noexcept {
        if (filename.empty() || filename == CX(".") || filename == CX(".."))
            return false;
        #ifdef _XOPEN_SOURCE
            auto leaf = find_leaf();
            return ! leaf.empty() && leaf[0] == '.';
        #else
            static constexpr uint32_t hidden_attr = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM
                #ifdef FILE_ATTRIBUTE_INTEGRITY_STREAM
                    | FILE_ATTRIBUTE_INTEGRITY_STREAM
                #endif
                #ifdef FILE_ATTRIBUTE_NO_SCRUB_DATA
                    | FILE_ATTRIBUTE_NO_SCRUB_DATA
                #endif
                ;
            return ! is_root() && (get_attributes(filename) & hidden_attr);
        #endif
    }

    bool Path::is_symlink() const noexcept {
        #ifdef _XOPEN_SOURCE
            auto st = get_stat(filename, no_follow).st;
            return S_ISLNK(st.st_mode);
        #else
            return false;
        #endif
    }

    Path Path::resolve() const {
        Path result;
        if (empty()) {
            result = get_current_directory(false);
        } else {
            #ifdef _XOPEN_SOURCE
                if (filename[0] == '~') {
                    size_t pos = filename.find('/');
                    std::string tail, user = filename.substr(1, pos - 1);
                    if (pos != npos)
                        tail = filename.substr(pos + 1, npos);
                    Path home = get_user_home(user);
                    if (! home.empty())
                        result = home / tail;
                }
                if (result.empty() && is_relative()) {
                    Path cwd = get_current_directory(false);
                    if (! cwd.empty())
                        result = cwd / *this;
                }
            #else
                std::wstring buf(1024, L'\0'), resolved(filename);
                for (;;) {
                    auto rc = GetLongPathNameW(resolved.data(), &buf[0], uint32_t(buf.size()));
                    if (rc == 0)
                        break;
                    if (rc < buf.size()) {
                        resolved.assign(buf.data(), rc);
                        break;
                    }
                    buf.resize(2 * buf.size());
                }
                for (;;) {
                    auto rc = GetFullPathNameW(resolved.data(), uint32_t(buf.size()), &buf[0], nullptr);
                    if (rc == 0)
                        break;
                    if (rc < buf.size()) {
                        resolved.assign(buf.data(), rc);
                        break;
                    }
                    buf.resize(2 * buf.size());
                }
                result = resolved;
            #endif
        }
        if (result.empty())
            result = *this;
        if (! result.is_absolute())
            throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), name());
        return result;
    }

    Path Path::resolve_symlink() const {
        #ifdef _XOPEN_SOURCE
            std::string buf(256, '\0');
            for (;;) {
                auto rc = readlink(c_name(), &buf[0], buf.size());
                if (rc < 0) {
                    int err = errno;
                    if (err == EINVAL || err == ENOENT || err == ENOTDIR)
                        return *this;
                    else
                        throw std::system_error(err, std::generic_category(), name());
                }
                if (size_t(rc) <= buf.size() - 2) {
                    buf.resize(rc);
                    return buf;
                }
                buf.resize(2 * buf.size());
            }
        #else
            return *this;
        #endif
    }

    uint64_t Path::size(flag_type flags) const {
        #ifdef _XOPEN_SOURCE
            auto st = get_stat(filename, flags).st;
            uint64_t bytes = st.st_size;
        #else
            auto info = get_attributes_ex(filename);
            uint64_t bytes = (uint64_t(info.nFileSizeHigh) << 32) + uint64_t(info.nFileSizeLow);
        #endif
        if (flags & recurse)
            for (auto& child: directory())
                bytes += child.size(no_follow | recurse);
        return bytes;
    }

    // File system update functions

    void Path::copy_to(const Path& dst, flag_type flags) const {
        static constexpr size_t block_size = 16384;
        if (! exists())
            throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), name());
        if (*this == dst || id() == dst.id())
            throw std::system_error(std::make_error_code(std::errc::file_exists), dst.name());
        if (is_directory() && ! (flags & recurse))
            throw std::system_error(std::make_error_code(std::errc::is_a_directory), name());
        if (dst.exists()) {
            if (! (flags & overwrite))
                throw std::system_error(std::make_error_code(std::errc::file_exists), dst.name());
            dst.remove(recurse);
        }
        if (is_symlink()) {
            resolve_symlink().make_symlink(dst);
        } else if (is_directory()) {
            dst.make_directory();
            for (auto& child: directory())
                child.copy_to(dst / child.split_path().second, recurse);
        } else {
            auto in = FX(fopen)(c_name(), CX("rb"));
            int err = errno;
            if (! in)
                throw std::system_error(err, std::generic_category(), name());
            auto guard_in = scope_exit([=] { fclose(in); });
            auto out = FX(fopen)(dst.c_name(), CX("wb"));
            err = errno;
            if (! out)
                throw std::system_error(err, std::generic_category(), dst.name());
            auto guard_out = scope_exit([=] { fclose(out); });
            std::string buf(block_size, '\0');
            while (! feof(in)) {
                errno = 0;
                size_t n = fread(&buf[0], 1, buf.size(), in);
                err = errno;
                if (err)
                    throw std::system_error(err, std::generic_category(), name());
                if (n) {
                    errno = 0;
                    fwrite(buf.data(), 1, n, out);
                    err = errno;
                    if (err)
                        throw std::system_error(err, std::generic_category(), dst.name());
                }
            }
        }
    }

    void Path::create() const {
        if (! exists())
            save(std::string{});
    }

    void Path::make_directory(flag_type flags) const {
        static const auto mkdir_call = [] (const Path& dir) {
            #ifdef _XOPEN_SOURCE
                return mkdir(dir.c_name(), 0777);
            #else
                return _wmkdir(dir.c_name());
            #endif
        };
        if (mkdir_call(*this) == 0)
            return;
        int err = errno;
        if (err == EEXIST) {
            if (is_directory())
                return;
            if (! (flags & overwrite))
                throw std::system_error(err, std::generic_category(), name());
            remove();
        } else if (err == ENOENT && (flags & recurse) && ! empty()) {
            Path parent = split_path().first;
            if (parent == *this)
                throw std::system_error(err, std::generic_category(), name());
            parent.make_directory(flags);
        }
        if (mkdir_call(*this) != 0) {
            err = errno;
            throw std::system_error(err, std::generic_category(), name());
        }
    }

    void Path::make_symlink(const Path& linkname, flag_type flags) const {
        #ifdef _XOPEN_SOURCE
            if (linkname.is_symlink()) {
                try {
                    if (linkname.resolve_symlink() == *this)
                        return;
                }
                catch (const std::system_error&) {}
            }
            if ((flags & overwrite) && linkname.exists())
                linkname.remove(flags);
            if (symlink(c_name(), linkname.c_name()) == 0)
                return;
            if (! (flags & may_copy)) {
                int err = errno;
                throw std::system_error(err, std::generic_category(), linkname.name() + " -> " + name());
            }
            copy_to(linkname, recurse);
        #else
            if (flags & may_copy)
                copy_to(linkname, recurse);
            else
                throw std::system_error(std::make_error_code(std::errc::operation_not_supported), "Symbolic links are not supported on Windows");
        #endif
    }

    void Path::move_to(const Path& dst, flag_type flags) const {
        if (! exists())
            throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), name());
        if (*this == dst)
            return;
        if (dst.exists() && id() != dst.id()) {
            if (! (flags & overwrite))
                throw std::system_error(std::make_error_code(std::errc::file_exists), dst.name());
            dst.remove(recurse);
        }
        if (FX(rename)(c_name(), dst.c_name()) == 0)
            return;
        int err = errno;
        if (err != EXDEV || ! (flags & may_copy))
            throw std::system_error(err, std::generic_category(), name() + " -> " + dst.name());
        copy_to(dst, recurse);
        remove(recurse);
    }

    void Path::remove(flag_type flags) const {
        if ((flags & recurse) && is_directory() && ! is_symlink())
            for (auto child: directory())
                child.remove(recurse);
        #ifdef _XOPEN_SOURCE
            int rc = std::remove(c_name());
            int err = errno;
            if (rc != 0 && err != ENOENT)
                throw std::system_error(err, std::generic_category(), name());
        #else
            bool ok;
            if (is_directory())
                ok = RemoveDirectoryW(c_name());
            else
                ok = DeleteFileW(c_name());
            int err = GetLastError();
            if (! ok && err != ERROR_FILE_NOT_FOUND)
                throw std::system_error(err, std::system_category(), name());
        #endif
    }

    void Path::set_access_time(system_clock::time_point t, flag_type flags) const {
        #ifdef _XOPEN_SOURCE
            set_file_times(t, modify_time(), flags);
        #else
            (void)flags;
            set_file_time(t, 1);
        #endif
    }

    void Path::set_create_time(system_clock::time_point t, flag_type /*flags*/) const {
        #ifdef _XOPEN_SOURCE
            (void)t;
            throw std::system_error(std::make_error_code(std::errc::operation_not_supported), "The operating system does not support modifying file creation time");
        #else
            set_file_time(t, 0);
        #endif
    }

    void Path::set_modify_time(system_clock::time_point t, flag_type flags) const {
        #ifdef _XOPEN_SOURCE
            set_file_times(access_time(), t, flags);
        #else
            (void)flags;
            set_file_time(t, 2);
        #endif
    }

    void Path::set_status_time(system_clock::time_point /*t*/, flag_type /*flags*/) const {
        throw std::system_error(std::make_error_code(std::errc::operation_not_supported), "The operating system does not support modifying file status time");
    }

    // I/O functions

    void Path::load(std::string& dst, size_t maxlen, flag_type flags) const {
        static constexpr size_t block_size = 16384;
        FILE* in = nullptr;
        auto guard = scope_exit([&] { if (in && in != stdin) fclose(in); });
        if ((flags & std_default) && (filename.empty() || filename == CX("-"))) {
            in = stdin;
        } else {
            in = FX(fopen)(c_name(), CX("rb"));
            if (! in) {
                if (! (flags & may_fail))
                    throw std::ios::failure("Read error: " + name());
                dst.clear();
                return;
            }
        }
        dst.clear();
        while (dst.size() < maxlen) {
            size_t ofs = dst.size(), n = std::min(maxlen - ofs, block_size);
            dst.append(n, '\0');
            size_t rc = ::fread(&dst[0] + ofs, 1, n, in);
            dst.resize(ofs + rc);
            if (rc < n)
                break;
        }
    }

    void Path::save(std::string_view src, flag_type flags) const {
        FILE* out = nullptr;
        auto guard = scope_exit([&] { if (out && out != stdout) fclose(out); });
        if ((flags & std_default) && (filename.empty() || filename == CX("-"))) {
            out = stdout;
        } else {
            out = FX(fopen)(c_name(), flags & append ? CX("ab") : CX("wb"));
            if (! out)
                throw std::ios::failure("Write error: " + name());
        }
        size_t pos = 0;
        while (pos < src.size()) {
            errno = 0;
            pos += ::fwrite(src.data() + pos, 1, src.size() - pos, out);
            if (errno)
                throw std::ios::failure("Write error: " + name());
        }
    }

    // Process state functions

    void Path::change_directory() const {
        if (FX(chdir)(c_name()) == -1) {
            int err = errno;
            throw std::system_error(err, std::generic_category(), name());
        }
    }

    Path Path::current_directory() {
        return get_current_directory(true);
    }

    // Implementation details

    std::pair<Path::view_type, Path::view_type> Path::find_base_ext() const noexcept {
        if (filename.empty())
            return {filename, filename};
        size_t start = filename.find_last_of(native_delimiter);
        if (start != npos)
            ++start;
        #ifndef _XOPEN_SOURCE
            else if (filename.size() >= 2 && filename[1] == L':')
                start = 2;
        #endif
        else
            start = 0;
        size_t dot = npos;
        if (start < filename.size()) {
            dot = filename.find_last_of(CX('.'));
            if (dot <= start || dot >= filename.size() - 1)
                dot = npos;
        }
        return {make_view(filename, start, dot - start), make_view(filename, dot)};
    }

    Path::view_type Path::find_leaf() const noexcept {
        if (filename.empty())
            return filename;
        size_t start = filename.find_last_of(native_delimiter);
        if (start != npos)
            ++start;
        #ifndef _XOPEN_SOURCE
            else if (filename.size() >= 2 && filename[1] == L':')
                start = 2;
        #endif
        else
            start = 0;
        return make_view(filename, start);
    }

    Path::view_type Path::find_root(bool allow_drive_special) const noexcept {
        size_t pos = 0;
        if (! filename.empty()) {
            #if defined(__CYGWIN__)
                // Cygwin: /path or //server/path
                (void)allow_drive_special;
                if (filename[0] == '/') {
                    pos = 1;
                    if (filename[1] == '/' && filename.size() >= 4) {
                        size_t next = filename.find('/', 2);
                        if (next > 2 && next < npos)
                            pos = next + 1;
                    }
                }
            #elif defined(_XOPEN_SOURCE)
                // Posix: /path
                (void)allow_drive_special;
                pos = filename.find_first_not_of('/');
            #else
                // Windows: [\\?\]drive:\path or [\\?\]\\server\path
                // if allow_drive_special: [\\?\]\path or [\\?\]drive:path
                static const std::wregex pattern1(LR"(^(\\\\\?\\)?([A-Za-z]:\\|\\{2,}[^?\\]+\\?))", std::regex_constants::nosubs | std::regex_constants::optimize);
                static const std::wregex pattern2(LR"(^(\\\\\?\\)?([A-Za-z]:\\?|\\{2,}[^?\\]+\\?|\\+))", std::regex_constants::nosubs | std::regex_constants::optimize);
                auto& pattern = allow_drive_special ? pattern2 : pattern1;
                std::wsmatch match;
                if (std::regex_search(filename, match, pattern))
                    pos = match.length();
            #endif
        }
        return make_view(filename, 0, pos);
    }

    void Path::make_canonical(flag_type flags) {
        static const string_type ss = {native_delimiter, native_delimiter};
        static const string_type sds = {native_delimiter, CX('.'), native_delimiter};
        #ifndef _XOPEN_SOURCE
            // Replace slash delimiters
            std::replace(filename.begin(), filename.end(), L'/', L'\\');
        #endif
        // Trim redundant leading slashes
        size_t p = filename.find_first_not_of(native_delimiter);
        if (p == npos)
            p = filename.size();
        #if defined(__CYGWIN__)
            if (p > 2)
                filename.erase(0, p - 1);
        #elif defined(_XOPEN_SOURCE)
            if (p > 1)
                filename.erase(0, p - 1);
        #else
            if (p > 2)
                filename.erase(0, p - 2);
        #endif
        // Replace /./ with /
        p = 0;
        for (;;) {
            p = filename.find(sds, p);
            if (p == npos)
                break;
            filename.erase(p, 2);
        }
        // Replace multiple slashes with one
        #ifdef _XOPEN_SOURCE
            p = filename.find_first_not_of('/');
        #else
            p = filename.find_first_not_of(L"?\\");
        #endif
        while (p < filename.size()) {
            p = filename.find(ss, p);
            if (p == npos)
                break;
            size_t q = filename.find_first_not_of(native_delimiter, p + 2);
            if (q == npos)
                q = filename.size();
            filename.erase(p + 1, q - p - 1);
            ++p;
        }
        // Trim trailing / and /.
        size_t root_size = find_root(true).size();
        size_t min_root = std::max(root_size, size_t(1));
        while (filename.size() > min_root && (filename.back() == native_delimiter || (filename.back() == CX('.') && filename.end()[-2] == native_delimiter)))
            filename.pop_back();
        #ifndef _XOPEN_SOURCE
            // Ensure a trailing slash on network paths
            if (filename.size() >= 3 && filename[0] == L'\\' && filename[1] == L'\\' && filename.find(L'\\', 2) == npos)
                filename += L'\\';
            // Set the drive letter to upper case
            size_t drive = npos;
            if (root_size >= 3 && filename[root_size - 1] == L'\\')
                drive = root_size - 3;
            else if (root_size >= 2 && filename[root_size - 1] == L':')
                drive = root_size - 2;
            if (drive != npos && filename[drive] >= L'a' && filename[drive] <= L'z' && filename[drive + 1] == L':')
                filename[drive] -= 0x20;
        #endif
        // Validate if requested
        if ((flags & legal_name) && ! is_legal())
            throw std::invalid_argument("Invalid file name: " + quote(to_utf8(filename, Utf::replace)));
    }

    #if defined(__APPLE__) && __MAC_OS_X_VERSION_MAX_ALLOWED < 101300

        void Path::set_file_times(system_clock::time_point atime, system_clock::time_point mtime, flag_type /*flags*/) const {
            timeval times[2];
            times[0] = timepoint_to_timeval(atime);
            times[1] = timepoint_to_timeval(mtime);
            errno = 0;
            int rc = utimes(c_name(), times);
            int err = errno;
            if (rc == -1)
                throw std::system_error(err, std::generic_category(), name());
        }

    #elif defined(_XOPEN_SOURCE)

        void Path::set_file_times(system_clock::time_point atime, system_clock::time_point mtime, flag_type flags) const {
            timespec times[2];
            times[0] = timepoint_to_timespec(atime);
            times[1] = timepoint_to_timespec(mtime);
            int utflags = flags & no_follow ? AT_SYMLINK_NOFOLLOW : 0;
            errno = 0;
            int rc = utimensat(AT_FDCWD, c_name(), times, utflags);
            int err = errno;
            if (rc == -1)
                throw std::system_error(err, std::generic_category(), name());
        }

    #else

        system_clock::time_point Path::get_file_time(int index) const noexcept {
            auto fh = CreateFileW(c_name(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (! fh)
                return {};
            auto guard = scope_exit([=] { CloseHandle(fh); });
            FILETIME ft[3];
            if (GetFileTime(fh, ft, ft + 1, ft + 2))
                return filetime_to_timepoint(ft[index]);
            else
                return {};
        }

        void Path::set_file_time(system_clock::time_point t, int index) const {
            SetLastError(0);
            auto fh = CreateFileW(c_name(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            auto err = GetLastError();
            if (! fh)
                throw std::system_error(err, std::system_category(), name());
            auto guard = scope_exit([=] { CloseHandle(fh); });
            auto ft = timepoint_to_filetime(t);
            FILETIME* fts[3];
            std::memset(fts, 0, sizeof(fts));
            fts[index] = &ft;
            auto rc = SetFileTime(fh, fts[0], fts[1], fts[2]);
            err = GetLastError();
            if (! rc)
                throw std::system_error(err, std::system_category(), name());
        }

    #endif

    // Comparison objects

    bool Path::equal::operator()(const Path& lhs, const Path& rhs) const {
        if (cmode == cmp::icase)
            return StringCompare<Strcmp::equal | Strcmp::icase>()(lhs.name(), rhs.name());
        else
            return lhs == rhs;
    }

    bool Path::less::operator()(const Path& lhs, const Path& rhs) const {
        if (cmode == cmp::icase)
            return StringCompare<Strcmp::less | Strcmp::icase>()(lhs.name(), rhs.name());
        else
            return lhs < rhs;
    }

    // Directory iterator

    struct Path::directory_iterator::impl_type {
        Path current;
        Path prefix;
        string_type leaf;
        flag_type flags = 0;
        #ifdef _XOPEN_SOURCE
            DIR* dirptr = nullptr;
            dirent entry;
            char padding[NAME_MAX + 1];
            ~impl_type() { if (dirptr) closedir(dirptr); }
        #else
            HANDLE handle = nullptr;
            WIN32_FIND_DATAW info;
            bool first;
            ~impl_type() { if (handle) FindClose(handle); }
        #endif
    };

    Path::directory_iterator::directory_iterator(const Path& dir, flag_type flags) {
        if ((flags & unicode) && ! dir.is_unicode())
            return;
        #ifdef _XOPEN_SOURCE
            impl = std::make_shared<impl_type>();
            memset(&impl->entry, 0, sizeof(impl->entry));
            memset(impl->padding, 0, sizeof(impl->padding));
            if (dir.empty())
                impl->dirptr = opendir(".");
            else
                impl->dirptr = opendir(dir.c_name());
            if (! impl->dirptr)
                impl.reset();
        #else
            // We need to check first that the supplied file name refers to a
            // directory, because FindFirstFile() gives a false positive for
            // "file/*" when the file exists but is not a directory. There's a
            // race condition here but there doesn't seem to be anything we
            // can do about it.
            if (! dir.empty() && ! dir.is_root() && ! dir.is_directory())
                return;
            impl = std::make_shared<impl_type>();
            memset(&impl->info, 0, sizeof(impl->info));
            impl->first = true;
            std::wstring glob = (dir / L"*").os_name();
            impl->handle = FindFirstFileW(glob.data(), &impl->info);
            if (! impl->handle)
                impl.reset();
        #endif
        if (! impl)
            return;
        impl->prefix = dir;
        impl->flags = flags;
        ++*this;
    }

    const Path& Path::directory_iterator::operator*() const noexcept {
        return impl->current;
    }

    Path::directory_iterator& Path::directory_iterator::operator++() {
        static const string_type dot1(1, CX('.'));
        static const string_type dot2(2, CX('.'));
        const bool skip_hidden = impl->flags & no_hidden;
        while (impl) {
            #ifdef _XOPEN_SOURCE
                dirent* entptr = nullptr;
                int rc = readdir_r(impl->dirptr, &impl->entry, &entptr);
                bool ok = rc == 0 && entptr;
                if (ok)
                    impl->leaf = impl->entry.d_name;
            #else
                bool ok = impl->first || FindNextFile(impl->handle, &impl->info);
                impl->first = false;
                if (ok)
                    impl->leaf = impl->info.cFileName;
            #endif
            if (! ok) {
                impl.reset();
                break;
            }
            if (impl->leaf != dot1 && impl->leaf != dot2 && (! (impl->flags & unicode) || valid_string(impl->leaf))) {
                impl->current = impl->prefix / impl->leaf;
                if (! skip_hidden || ! impl->current.is_hidden())
                    break;
            }
        }
        return *this;
    }

    // Deep search iterator

    struct Path::deep_search_iterator::impl_type {
        std::vector<directory_range> stack;
        flag_type flagset = 0;
        bool revisit = false;
    };

    Path::deep_search_iterator::deep_search_iterator(const Path& dir, flag_type flags) {
        if (! dir.is_directory(flags))
            return;
        auto range = dir.directory(flags);
        if (range.empty())
            return;
        impl = std::make_shared<impl_type>();
        impl->stack.push_back(range);
        impl->flagset = flags;
        if (impl->flagset & bottom_up) {
            for (;;) {
                range = (**this).directory(impl->flagset);
                if (range.empty())
                    break;
                impl->stack.push_back(range);
            }
        }
        impl->revisit = (impl->flagset & bottom_up) && ! impl->stack.empty() && (**this).is_directory();
        if (impl->stack.empty())
            impl.reset();
    }

    const Path& Path::deep_search_iterator::operator*() const noexcept {
        return *impl->stack.back().first;
    }

    Path::deep_search_iterator& Path::deep_search_iterator::operator++() {
        do {
            if ((**this).is_directory(impl->flagset) && ! impl->revisit) {
                auto range = (**this).directory(impl->flagset);
                impl->revisit = range.empty();
                if (! impl->revisit)
                    impl->stack.push_back(range);
            } else {
                ++impl->stack.back().first;
                impl->revisit = impl->stack.back().empty();
                if (impl->revisit)
                    impl->stack.pop_back();
            }
        } while (! impl->stack.empty() && (**this).is_directory(impl->flagset) && impl->revisit != bool(impl->flagset & bottom_up));
        if (impl->stack.empty())
            impl.reset();
        return *this;
    }

}
