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

#ifdef _XOPEN_SOURCE
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

    #ifdef _XOPEN_SOURCE

        struct FileId:
        public LessThanComparable<FileId> {
            constexpr FileId() = default;
            constexpr FileId(uint64_t h, uint64_t l) noexcept: hi(h), lo(l) {}
            uint64_t hi = 0, lo = 0;
            size_t hash() const noexcept;
        };

        bool operator==(const FileId& lhs, const FileId& rhs) noexcept;
        bool operator<(const FileId& lhs, const FileId& rhs) noexcept;
        std::istream& operator>>(std::istream& in, FileId& f);
        std::ostream& operator<<(std::ostream& out, const FileId& f);

    #else

        using FileId = uint64_t;

    #endif

    // System dependencies

    #ifdef _XOPEN_SOURCE
        constexpr char file_delimiter = '/';
        constexpr char native_file_delimiter = '/';
    #else
        constexpr char file_delimiter = '\\';
        constexpr wchar_t native_file_delimiter = L'\\';
    #endif

    // File name operations

    namespace UnicornDetail {

        #ifdef _XOPEN_SOURCE

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

    #ifdef _XOPEN_SOURCE

        #ifdef __APPLE__

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
        if (prefix.empty() || (! suffix.empty() && ! file_is_relative(suffix)))
            return file_path(suffix, args...);
        if (suffix.empty())
            return file_path(prefix, args...);
        if (prefix.back() != file_delimiter)
            prefix += file_delimiter;
        return file_path(prefix + suffix, args...);
    }

    pair<u8string, u8string> split_path(const u8string& file, uint32_t flags = 0);
    pair<u8string, u8string> split_file(const u8string& file);

    #ifndef _XOPEN_SOURCE

        inline wstring file_path(const wstring& file) { return UnicornDetail::normalize_path(file); }

        template <typename... Args>
        wstring file_path(const wstring& file1, const wstring& file2, Args... args) {
            using namespace UnicornDetail;
            wstring prefix = normalize_path(file1), suffix = normalize_path(file2);
            if (prefix.empty() || (! suffix.empty() && ! file_is_relative(suffix)))
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

    #ifdef _XOPEN_SOURCE

        u8string current_directory();
        bool file_exists(const u8string& file);
        FileId file_id(const u8string& file, uint32_t flags = 0);
        bool file_is_directory(const u8string& file);
        bool file_is_hidden(const u8string& file);
        bool file_is_symlink(const u8string& file);
        uint64_t file_size(const u8string& file, uint32_t flags = 0);
        u8string resolve_path(const u8string& file);
        u8string resolve_symlink(const u8string& file);
        inline u8string native_current_directory() { return current_directory(); }

    #else

        wstring native_current_directory();
        bool file_exists(const wstring& file);
        FileId file_id(const wstring& file, uint32_t flags = 0);
        bool file_is_directory(const wstring& file);
        bool file_is_hidden(const wstring& file);
        bool file_is_symlink(const wstring& file);
        uint64_t file_size(const wstring& file, uint32_t flags = 0);
        wstring resolve_path(const wstring& file);
        wstring resolve_symlink(const wstring& file);
        inline u8string current_directory() { return to_utf8(native_current_directory()); }
        inline bool file_exists(const u8string& file) { return file_exists(to_wstring(file)); }
        inline FileId file_id(const u8string& file, uint32_t flags = 0) { return file_id(to_wstring(file), flags); }
        inline bool file_is_directory(const u8string& file) { return file_is_directory(to_wstring(file)); }
        inline bool file_is_hidden(const u8string& file) { return file_is_hidden(to_wstring(file)); }
        inline bool file_is_symlink(const u8string& file) { return file_is_symlink(to_wstring(file)); }
        inline uint64_t file_size(const u8string& file, uint32_t flags = 0) { return file_size(to_wstring(file), flags); }
        inline u8string resolve_path(const u8string& file) { return to_utf8(resolve_path(to_wstring(file))); }
        inline u8string resolve_symlink(const u8string& file) { return to_utf8(resolve_symlink(to_wstring(file))); }

    #endif

    // File system modifying functions

    void copy_file(const NativeString& src, const NativeString& dst, uint32_t flags = 0);
    void make_directory(const NativeString& dir, uint32_t flags = 0);
    void make_symlink(const NativeString& file, const NativeString& link, uint32_t flags = 0);
    void move_file(const NativeString& src, const NativeString& dst, uint32_t flags = 0);
    void remove_file(const NativeString& file, uint32_t flags = 0);

    #ifndef _XOPEN_SOURCE

        inline void copy_file(const u8string& src, const u8string& dst, uint32_t flags = 0)
            { return copy_file(to_wstring(src), to_wstring(dst), flags); }
        inline void make_directory(const u8string& dir, uint32_t flags = 0)
            { return make_directory(to_wstring(dir), flags); }
        inline void make_symlink(const u8string& file, const u8string& link, uint32_t flags = 0)
            { return make_symlink(to_wstring(file), to_wstring(link), flags); }
        inline void move_file(const u8string& src, const u8string& dst, uint32_t flags = 0)
            { return move_file(to_wstring(src), to_wstring(dst), flags); }
        inline void remove_file(const u8string& file, uint32_t flags = 0)
            { return remove_file(to_wstring(file), flags); }

    #endif

    // Directory iterators

    class NativeDirectoryIterator:
    public InputIterator<NativeDirectoryIterator, const NativeString> {
    public:
        NativeDirectoryIterator() = default;
        explicit NativeDirectoryIterator(const NativeString& dir, uint32_t flags = 0);
        const NativeString& operator*() const noexcept { return current; }
        NativeDirectoryIterator& operator++();
        bool operator==(const NativeDirectoryIterator& rhs) const noexcept { return impl == rhs.impl; }
    private:
        struct impl_type;
        shared_ptr<impl_type> impl;
        NativeString prefix;
        NativeString leaf;
        NativeString current;
        uint32_t fset;
        void do_init(const NativeString& dir);
        void do_next();
    };

    inline Irange<NativeDirectoryIterator> directory(const NativeString& dir, uint32_t flags = 0)
        { return {NativeDirectoryIterator(dir, flags), NativeDirectoryIterator()}; }

    #ifdef _XOPEN_SOURCE

        using DirectoryIterator = NativeDirectoryIterator;

    #else

        class DirectoryIterator:
        public InputIterator<DirectoryIterator, const u8string> {
        public:
            DirectoryIterator() = default;
            explicit DirectoryIterator(const u8string& dir, uint32_t flags = 0):
                nat(to_wstring(dir), flags), current(to_utf8(*nat)) {}
            const u8string& operator*() const noexcept { return current; }
            DirectoryIterator& operator++() { ++nat; current = to_utf8(*nat); return *this; }
            bool operator==(const DirectoryIterator& rhs) const noexcept { return nat == rhs.nat; }
        private:
            NativeDirectoryIterator nat;
            u8string current;
        };

        inline Irange<DirectoryIterator> directory(const u8string& dir, uint32_t flags = 0)
            { return {DirectoryIterator(dir, flags), DirectoryIterator()}; }

    #endif

}

#ifdef _XOPEN_SOURCE
    PRI_DEFINE_STD_HASH(Unicorn::FileId)
#endif
