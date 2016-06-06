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
    constexpr uint32_t fs_follow     = 1ul << 1;  // Resolve symlinks
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

    // File name operations

    namespace UnicornDetail {

        #if defined(PRI_TARGET_UNIX)

            inline u8string normalize_path(const u8string& path) {
                return path;
            }

        #else

            inline u8string normalize_path(const u8string& path) {
                auto result = path;
                std::replace(result.begin(), result.end(), '/', '\\');
                return result;
            }

            inline wstring normalize_path(const wstring& path) {
                auto result = path;
                std::replace(result.begin(), result.end(), L'/', L'\\');
                return result;
            }

        #endif

    }

    bool is_legal_mac_leaf_name(const u8string& file);
    bool is_legal_mac_path_name(const u8string& file);
    bool is_legal_posix_leaf_name(const u8string& file);
    bool is_legal_posix_path_name(const u8string& file);
    bool is_legal_windows_leaf_name(const wstring& file);
    bool is_legal_windows_path_name(const wstring& file);
    inline bool is_legal_windows_leaf_name(const u8string& file) { return is_legal_windows_leaf_name(to_wstring(file)); }
    inline bool is_legal_windows_path_name(const u8string& file) { return is_legal_windows_path_name(to_wstring(file)); }

    #if defined(PRI_TARGET_UNIX)

        #if defined(PRI_TARGET_APPLE)

            inline bool is_legal_leaf_name(const u8string& file) { return is_legal_mac_leaf_name(file); }
            inline bool is_legal_path_name(const u8string& file) { return is_legal_mac_path_name(file); }

        #else

            inline bool is_legal_leaf_name(const u8string& file) { return is_legal_posix_leaf_name(file); }
            inline bool is_legal_path_name(const u8string& file) { return is_legal_posix_path_name(file); }

        #endif

        bool file_is_absolute(const u8string& file);
        bool file_is_relative(const u8string& file);
        bool file_is_root(const u8string& file);
        inline bool file_is_drive_absolute(const u8string& /*file*/) { return false; }
        inline bool file_is_drive_relative(const u8string& /*file*/) { return false; }

    #else

        bool file_is_absolute(const wstring& file);
        bool file_is_relative(const wstring& file);
        bool file_is_drive_absolute(const wstring& file);
        bool file_is_drive_relative(const wstring& file);
        bool file_is_root(const wstring& file);
        inline bool file_is_absolute(const u8string& file) { return file_is_absolute(to_wstring(file)); }
        inline bool file_is_relative(const u8string& file) { return file_is_relative(to_wstring(file)); }
        inline bool file_is_drive_absolute(const u8string& file) { return file_is_drive_absolute(to_wstring(file)); }
        inline bool file_is_drive_relative(const u8string& file) { return file_is_drive_relative(to_wstring(file)); }
        inline bool file_is_root(const u8string& file) { return file_is_root(to_wstring(file)); }
        inline bool is_legal_leaf_name(const wstring& file) { return is_legal_windows_leaf_name(file); }
        inline bool is_legal_leaf_name(const u8string& file) { return is_legal_windows_leaf_name(file); }
        inline bool is_legal_path_name(const wstring& file) { return is_legal_windows_path_name(file); }
        inline bool is_legal_path_name(const u8string& file) { return is_legal_windows_path_name(file); }

    #endif

    inline u8string file_path(const u8string& file) { return UnicornDetail::normalize_path(file); }

    template <typename... Args>
    u8string file_path(const u8string& file1, const u8string& file2, Args... args) {
        using namespace UnicornDetail;
        u8string prefix = normalize_path(file1), suffix = normalize_path(file2);
        if (prefix.empty() || ! file_is_relative(suffix))
            return file_path(suffix, args...);
        if (suffix.empty())
            return file_path(prefix, args...);
        if (prefix.back() != file_delimiter)
            prefix += file_delimiter;
        return file_path(prefix + suffix, args...);
    }

    pair<u8string, u8string> split_path(const u8string& file, uint32_t flags = 0);
    pair<u8string, u8string> split_file(const u8string& file);

    #if defined(PRI_TARGET_WINDOWS)

        inline wstring file_path(const wstring& file) { return UnicornDetail::normalize_path(file); }

        template <typename... Args>
        wstring file_path(const wstring& file1, const wstring& file2, Args... args) {
            using namespace UnicornDetail;
            wstring prefix = normalize_path(file1), suffix = normalize_path(file2);
            if (prefix.empty() || ! file_is_relative(suffix))
                return file_path(suffix, args...);
            if (suffix.empty())
                return file_path(prefix, args...);
            wstring delim;
            if (prefix.back() != native_file_delimiter)
                prefix += native_file_delimiter;
            return file_path(prefix + suffix, args...);
        }

        pair<wstring, wstring> split_path(const wstring& file, uint32_t flags = 0);
        pair<wstring, wstring> split_file(const wstring& file);

    #endif

    // File system query functions

    #if defined(PRI_TARGET_UNIX)

        u8string native_current_directory();
        bool file_exists(const u8string& file);
        FileId file_id(const u8string& file, uint32_t flags = 0);
        bool file_is_directory(const u8string& file);
        bool file_is_hidden(const u8string& file);
        bool file_is_symlink(const u8string& file);
        uint64_t file_size(const u8string& file, uint32_t flags = 0);
        u8string resolve_symlink(const u8string& file);
        inline u8string current_directory() { return native_current_directory(); }

    #else

        wstring native_current_directory();
        bool file_exists(const wstring& file);
        FileId file_id(const wstring& file, uint32_t flags = 0);
        bool file_is_directory(const wstring& file);
        bool file_is_hidden(const wstring& file);
        bool file_is_symlink(const wstring& file);
        uint64_t file_size(const wstring& file, uint32_t flags = 0);
        wstring resolve_symlink(const wstring& file);
        inline u8string current_directory() { return to_utf8(native_current_directory()); }
        inline bool file_exists(const u8string& file) { return file_exists(to_wstring(file)); }
        inline FileId file_id(const u8string& file, uint32_t flags = 0) { return file_id(to_wstring(file)); }
        inline bool file_is_directory(const u8string& file) { return file_is_directory(to_wstring(file)); }
        inline bool file_is_hidden(const u8string& file) { return file_is_hidden(to_wstring(file)); }
        inline bool file_is_symlink(const u8string& file) { return file_is_symlink(to_wstring(file)); }
        inline uint64_t file_size(const u8string& file, uint32_t flags = 0) { return file_size(to_wstring(file)); }
        inline u8string resolve_symlink(const u8string& file) { return to_utf8(resolve_symlink(to_wstring(file))); }

    #endif

    // File system modifying functions

    void copy_file(const NativeString& src, const NativeString& dst, uint32_t flags = 0);
    void make_directory(const NativeString& dir, uint32_t flags = 0);
    void make_symlink(const NativeString& file, const NativeString& link, uint32_t flags = 0);
    void move_file(const NativeString& src, const NativeString& dst, uint32_t flags = 0);
    void remove_file(const NativeString& file, uint32_t flags = 0);

    #if defined(PRI_TARGET_WINDOWS)

        inline void copy_file(const u8string& src, const u8string& dst, uint32_t flags = 0)
            { return copy_file(to_wstring(src), to_wstring(dst), flags = 0); }
        inline void make_directory(const u8string& dir, uint32_t flags = 0)
            { return make_directory(to_wstring(dir), flags = 0); }
        inline void make_symlink(const u8string& file, const u8string& link, uint32_t flags = 0)
            { return make_symlink(to_wstring(file), to_wstring(link), flags = 0); }
        inline void move_file(const u8string& src, const u8string& dst, uint32_t flags = 0)
            { return move_file(to_wstring(src), to_wstring(dst), flags = 0); }
        inline void remove_file(const u8string& file, uint32_t flags = 0)
            { return remove_file(to_wstring(file), flags = 0); }

    #endif

    // TODO

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
            auto normdir = UnicornDetail::normalize_path(dir);
            NativeString natdir;
            recode(normdir, natdir);
            init2(natdir, flags);
            recode(filename(), file);
        }
        const basic_string<C>& operator*() const noexcept { return file; }
        DirectoryIterator& operator++() {
            next2();
            recode(filename(), file);
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
