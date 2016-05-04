#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(PRI_TARGET_UNIX)
    #include <sys/stat.h>
#endif

namespace Unicorn {

    // Constants

    constexpr uint32_t fs_dotdot     = 1ul << 0;  // Include . and ..
    constexpr uint32_t fs_follow     = 1ul << 1;  // Include . and ..
    constexpr uint32_t fs_fullname   = 1ul << 2;  // Return full file names
    constexpr uint32_t fs_hidden     = 1ul << 3;  // Include hidden files
    constexpr uint32_t fs_overwrite  = 1ul << 4;  // Delete existing file if necessary
    constexpr uint32_t fs_recurse    = 1ul << 5;  // Recursive directory operations
    constexpr uint32_t fs_unicode    = 1ul << 6;  // Skip files with non-Unicode names

    // Types

    using FileId =
        #if defined(PRI_TARGET_UNIX)
            std::conditional_t<(sizeof(dev_t) + sizeof(ino_t) > 8), uint128_t, uint64_t>;
        #else
            uint64_t;
        #endif

    // System dependencies

    #if defined(PRI_TARGET_UNIX)
        constexpr char file_delimiter = '/';
        constexpr char native_file_delimiter = '/';
    #else
        constexpr char file_delimiter = '\\';
        constexpr wchar_t native_file_delimiter = L'\\';
    #endif

    template <typename C1, typename C2>
    void recode_filename(const basic_string<C1>& src, basic_string<C2>& dst) {
        if (sizeof(C1) == sizeof(C2))
            dst.assign(reinterpret_cast<const C2*>(src.data()), src.size());
        else
            recode(src, dst, err_replace);
    }

    template <typename C2, typename C1>
    basic_string<C2> recode_filename(const basic_string<C1>& src) {
        if (sizeof(C1) == sizeof(C2))
            return basic_string<C2>(reinterpret_cast<const C2*>(src.data()), src.size());
        else
            return recode<C2>(src, 0, err_replace);
    }

    // File name operations

    namespace UnicornDetail {

        template <typename C>
        basic_string<C> normalize_file(const basic_string<C>& file) {
            #if defined(PRI_TARGET_UNIX)
                return file;
            #else
                auto nfile = file;
                std::replace(nfile.begin(), nfile.end(), PRI_CHAR('/', C), PRI_CHAR('\\', C));
                return nfile;
            #endif
        }

        inline NativeString native_file(const NativeString& file) {
            return normalize_file(file);
        }

        template <typename C>
        NativeString native_file(const basic_string<C>& file) {
            NativeString nfile;
            recode_filename(file, nfile);
            return normalize_file(nfile);
        }

        // The Windows file name checking functions use UTF-8 instead of
        // UTF-16 so we can use regexes in the implementation without having
        // to link with the 16-bit PCRE library. This won't make any
        // difference to the legality of a name since all the characters
        // relevant to the check are in ASCII.

        bool legal_posix_leaf(const string& file);
        bool legal_posix_path(const string& file);
        bool legal_windows_leaf(const string& file);
        bool legal_windows_path(const string& file);
        bool posix_file_is_absolute(const string& file);
        bool posix_file_is_root(const string& file);
        bool windows_file_is_absolute(const string& file);
        bool windows_file_is_root(const string& file);
        bool windows_file_is_drive_absolute(const string& file);
        bool windows_file_is_drive_relative(const string& file);

    }

    #if defined(PRI_TARGET_UNIX)

        template <typename C>
        bool file_is_absolute(const basic_string<C>& file) {
            using namespace UnicornDetail;
            return posix_file_is_absolute(recode_filename<char>(file));
        }

        template <typename C>
        bool file_is_root(const basic_string<C>& file) {
            using namespace UnicornDetail;
            return posix_file_is_root(recode_filename<char>(file));
        }

        template <typename C>
        bool file_is_drive_absolute(const basic_string<C>& /*file*/) {
            return false;
        }

        template <typename C>
        bool file_is_drive_relative(const basic_string<C>& /*file*/) {
            return false;
        }

    #else

        template <typename C>
        bool file_is_absolute(const basic_string<C>& file) {
            using namespace UnicornDetail;
            return windows_file_is_absolute(recode_filename<char>(file));
        }

        template <typename C>
        bool file_is_root(const basic_string<C>& file) {
            using namespace UnicornDetail;
            return windows_file_is_root(recode_filename<char>(file));
        }

        template <typename C>
        bool file_is_drive_absolute(const basic_string<C>& file) {
            using namespace UnicornDetail;
            return windows_file_is_drive_absolute(recode_filename<char>(file));
        }

        template <typename C>
        bool file_is_drive_relative(const basic_string<C>& file) {
            using namespace UnicornDetail;
            return windows_file_is_drive_relative(recode_filename<char>(file));
        }

    #endif

    template <typename C>
    bool file_is_relative(const basic_string<C>& file) {
        return ! (file.empty() || file_is_absolute(file) || file_is_drive_absolute(file) || file_is_drive_relative(file));
    }

    template <typename C>
    basic_string<C> file_path(const basic_string<C>& file) {
        return UnicornDetail::normalize_file(file);
    }

    template <typename C>
    basic_string<C> file_path(const C* file) {
        return UnicornDetail::normalize_file(cstr(file));
    }

    template <typename C, typename... Args>
    basic_string<C> file_path(const basic_string<C>& file1, const basic_string<C>& file2,
            Args... args) {
        using namespace UnicornDetail;
        auto prefix = normalize_file(file1), suffix = normalize_file(file2);
        if (prefix.empty() || ! file_is_relative(suffix))
            return file_path(suffix, args...);
        if (suffix.empty())
            return file_path(prefix, args...);
        if (str_last_char(prefix) != file_delimiter)
            prefix += static_cast<C>(file_delimiter);
        return file_path(prefix + suffix, args...);
    }

    template <typename C, typename... Args>
    basic_string<C> file_path(const basic_string<C>& file1, const C* file2, Args... args) {
        return file_path(file1, cstr(file2), args...);
    }

    template <typename C, typename... Args>
    basic_string<C> file_path(const C* file1, const basic_string<C>& file2, Args... args) {
        return file_path(cstr(file1), file2, args...);
    }

    template <typename C, typename... Args>
    basic_string<C> file_path(const C* file1, const C* file2, Args... args) {
        return file_path(cstr(file1), cstr(file2), args...);
    }

    template <typename C>
    bool is_legal_mac_leaf_name(const basic_string<C>& file) {
        using namespace UnicornDetail;
        return ! file.empty() && valid_string(file) && legal_posix_leaf(to_utf8(file));
    }

    template <typename C>
    bool is_legal_mac_path_name(const basic_string<C>& file) {
        using namespace UnicornDetail;
        return ! file.empty() && valid_string(file) && legal_posix_path(to_utf8(file));
    }

    template <typename C>
    bool is_legal_posix_leaf_name(const basic_string<C>& file) {
        using namespace UnicornDetail;
        return ! file.empty() && (sizeof(C) == 1 || valid_string(file)) && legal_posix_leaf(to_utf8(file, err_replace));
    }

    template <typename C>
    bool is_legal_posix_path_name(const basic_string<C>& file) {
        using namespace UnicornDetail;
        return ! file.empty() && (sizeof(C) == 1 || valid_string(file)) && legal_posix_path(to_utf8(file, err_replace));
    }

    template <typename C>
    bool is_legal_windows_leaf_name(const basic_string<C>& file) {
        using namespace UnicornDetail;
        return ! file.empty() && (sizeof(C) == 2 || valid_string(file)) && legal_windows_leaf(to_utf8(file, err_replace));
    }

    template <typename C>
    bool is_legal_windows_path_name(const basic_string<C>& file) {
        using namespace UnicornDetail;
        return ! file.empty() && (sizeof(C) == 2 || valid_string(file)) && legal_windows_path(to_utf8(file, err_replace));
    }

    template <typename C>
    bool is_legal_leaf_name(const basic_string<C>& file) {
        #if defined(PRI_TARGET_APPLE)
            return is_legal_mac_leaf_name(file);
        #elif defined(PRI_TARGET_WINDOWS)
            return is_legal_windows_leaf_name(file);
        #else
            return is_legal_posix_leaf_name(file);
        #endif
    }

    template <typename C>
    bool is_legal_path_name(const basic_string<C>& file) {
        #if defined(PRI_TARGET_APPLE)
            return is_legal_mac_path_name(file);
        #elif defined(PRI_TARGET_WINDOWS)
            return is_legal_windows_path_name(file);
        #else
            return is_legal_posix_path_name(file);
        #endif
    }

    template <typename C>
    pair<basic_string<C>, basic_string<C>> split_path(const basic_string<C>& file, uint32_t flags = 0) {
        auto nfile = UnicornDetail::normalize_file(file);
        auto cut = nfile.find_last_of(static_cast<C>(file_delimiter));
        #if defined(PRI_TARGET_WINDOWS)
            if (cut == 2 && file[1] == PRI_CHAR(':', C)
                    && char_is_ascii(file[0]) && ascii_isalpha(char(file[0])))
                return {nfile.substr(0, 3), nfile.substr(3, npos)};
            else if (cut == npos && file[1] == PRI_CHAR(':', C)
                    && char_is_ascii(file[0]) && ascii_isalpha(char(file[0])))
                return {nfile.substr(0, 2), nfile.substr(2, npos)};
        #endif
        if (cut == npos) {
            return {{}, nfile};
        } else if (cut == 0) {
            return {nfile.substr(0, 1), nfile.substr(1, npos)};
        } else {
            size_t cut1 = flags & fs_fullname ? cut + 1 : cut;
            return {nfile.substr(0, cut1), nfile.substr(cut + 1, npos)};
        }
    }

    template <typename C>
    pair<basic_string<C>, basic_string<C>> split_file(const basic_string<C>& file) {
        auto nfile = UnicornDetail::normalize_file(file);
        auto cut = nfile.find_last_of(static_cast<C>(file_delimiter));
        #if defined(PRI_TARGET_WINDOWS)
            if (cut == npos && char_is_ascii(file[0]) && ascii_isalpha(char(file[0]))
                    && file[1] == PRI_CHAR(':', C))
                cut = 1;
        #endif
        if (cut == npos)
            cut = 0;
        else
            ++cut;
        if (cut >= nfile.size())
            return {};
        auto dot = nfile.find_last_of(PRI_CHAR('.', C));
        if (dot > cut && dot != npos)
            return {nfile.substr(cut, dot - cut), nfile.substr(dot, npos)};
        else
            return {nfile.substr(cut, npos), {}};
    }

    // File system query functions

    namespace UnicornDetail {

        NativeString native_current_directory();
        bool native_file_exists(const NativeString& file) noexcept;
        FileId native_file_id(const NativeString& file, uint32_t flags) noexcept;
        bool native_file_is_directory(const NativeString& file) noexcept;
        bool native_file_is_hidden(const NativeString& file) noexcept;
        bool native_file_is_symlink(const NativeString& file) noexcept;
        uint64_t native_file_size(const NativeString& file, uint32_t flags) noexcept;
        NativeString native_resolve_symlink(const NativeString& file);

    }

    template <typename C>
    basic_string<C> current_directory() {
        basic_string<C> dir;
        recode_filename(UnicornDetail::native_current_directory(), dir);
        return dir;
    }

    template <typename C>
    void current_directory(basic_string<C>& dir) {
        recode_filename(UnicornDetail::native_current_directory(), dir);
    }

    template <typename C>
    bool file_exists(const basic_string<C>& file) noexcept {
        using namespace UnicornDetail;
        return native_file_exists(native_file(file));
    }

    template <typename C>
    FileId file_id(const basic_string<C>& file, uint32_t flags = 0) noexcept {
        using namespace UnicornDetail;
        return native_file_id(native_file(file), flags);
    }

    template <typename C>
    bool file_is_directory(const basic_string<C>& file) noexcept {
        using namespace UnicornDetail;
        return native_file_is_directory(native_file(file));
    }

    template <typename C>
    bool file_is_hidden(const basic_string<C>& file) noexcept {
        using namespace UnicornDetail;
        return native_file_is_hidden(native_file(file));
    }

    template <typename C>
    bool file_is_symlink(const basic_string<C>& file) noexcept {
        using namespace UnicornDetail;
        return native_file_is_symlink(native_file(file));
    }

    template <typename C>
    uint64_t file_size(const basic_string<C>& file, uint32_t flags = 0) noexcept {
        using namespace UnicornDetail;
        return native_file_size(native_file(file), flags);
    }

    template <typename C>
    basic_string<C> resolve_symlink(const basic_string<C>& file) {
        using namespace UnicornDetail;
        return native_resolve_symlink(native_file(file));
    }

    // File system modifying functions

    namespace UnicornDetail {

        void native_copy_file(const NativeString& src, const NativeString& dst, uint32_t flags);
        void native_make_directory(const NativeString& dir, uint32_t flags);
        void native_make_symlink(const NativeString& file, const NativeString& link, uint32_t flags);
        void native_move_file(const NativeString& src, const NativeString& dst, uint32_t flags);
        void native_remove_file(const NativeString& file, uint32_t flags);

    }

    template <typename C>
    void copy_file(const basic_string<C>& src, const basic_string<C>& dst, uint32_t flags = 0) {
        using namespace UnicornDetail;
        native_copy_file(native_file(src), native_file(dst), flags);
    }

    template <typename C>
    void make_directory(const basic_string<C>& dir, uint32_t flags = 0) {
        using namespace UnicornDetail;
        native_make_directory(native_file(dir), flags);
    }

    template <typename C>
    void make_symlink(const basic_string<C>& file, const basic_string<C>& link, uint32_t flags = 0) {
        using namespace UnicornDetail;
        native_make_symlink(native_file(file), native_file(link), flags);
    }

    template <typename C>
    void move_file(const basic_string<C>& src, const basic_string<C>& dst, uint32_t flags = 0) {
        using namespace UnicornDetail;
        native_move_file(native_file(src), native_file(dst), flags);
    }

    template <typename C>
    void remove_file(const basic_string<C>& file, uint32_t flags = 0) {
        using namespace UnicornDetail;
        native_remove_file(native_file(file), flags);
    }

    // Directory iterators

    namespace UnicornDetail {

        // Directory iterator implementation is split into three stages:

        // Stage 1: The system specific part that calls the operating system's
        // directory walking API; this requires separate implementations for
        // each OS.

        // Stage 2: The part that works only with native file names, but is
        // not system specific apart from the choice of string type, so it can
        // be written as a single implementation.

        // Stage 3: The part that translates the file name to the caller's
        // preferred encoding; this is the template based interface exported
        // to the caller.

        class DirectoryStage1 {
        protected:
            const NativeString& leaf() const noexcept;
            void init1(const NativeString& dir);
            void next1();
            bool done() const noexcept { return ! impl; }
            bool equal(const DirectoryStage1& rhs) const noexcept { return impl == rhs.impl; }
        private:
            struct impl_type;
            shared_ptr<impl_type> impl;
        };

        class DirectoryStage2:
        public DirectoryStage1 {
        protected:
            const NativeString& filename() const noexcept { return current; }
            void init2(const NativeString& dir, uint32_t flags);
            void next2();
        private:
            NativeString prefix;
            NativeString current;
            uint32_t fset;
        };

    }

    template <typename C>
    class DirectoryIterator:
    public UnicornDetail::DirectoryStage2,
    public InputIterator<DirectoryIterator<C>, const basic_string<C>> {
    public:
        DirectoryIterator() = default;
        explicit DirectoryIterator(const basic_string<C>& dir, uint32_t flags = 0) {
            if ((flags & fs_unicode) && ! valid_string(dir))
                return;
            auto normdir = UnicornDetail::normalize_file(dir);
            NativeString natdir;
            recode_filename(normdir, natdir);
            init2(natdir, flags);
            recode_filename(filename(), file);
        }
        const basic_string<C>& operator*() const noexcept { return file; }
        DirectoryIterator& operator++() {
            next2();
            recode_filename(filename(), file);
            return *this;
        }
        friend bool operator==(const DirectoryIterator& lhs, const DirectoryIterator& rhs) noexcept { return lhs.equal(rhs); }
    private:
        basic_string<C> file;
    };

    template <typename C> Irange<DirectoryIterator<C>> directory(const basic_string<C>& dir, uint32_t flags = 0)
        { return {DirectoryIterator<C>(dir, flags), DirectoryIterator<C>()}; }
    template <typename C> Irange<DirectoryIterator<C>> directory(const C* dir, uint32_t flags = 0)
        { return {DirectoryIterator<C>(cstr(dir), flags), DirectoryIterator<C>()}; }

}
