#pragma once

#include "unicorn/character.hpp"
#include "unicorn/core.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace Unicorn {

    // System dependencies

    #if defined(_XOPEN_SOURCE)
        constexpr char file_delimiter = '/';
    #else
        constexpr char file_delimiter = '\\';
    #endif

    template <typename C1, typename C2>
    void recode_filename(const std::basic_string<C1>& src, std::basic_string<C2>& dst) {
        if (sizeof(C1) == sizeof(C2))
            dst.assign(reinterpret_cast<const C2*>(src.data()), src.size());
        else
            recode(src, dst, err_replace);
    }

    template <typename C2, typename C1>
    std::basic_string<C2> recode_filename(const std::basic_string<C1>& src) {
        if (sizeof(C1) == sizeof(C2))
            return std::basic_string<C2>(reinterpret_cast<const C2*>(src.data()), src.size());
        else
            return recode<C2>(src, 0, err_replace);
    }

    // Exceptions

    class FileError:
    public std::runtime_error {
    public:
        FileError():
            std::runtime_error(assemble(0, {})), names(), err(0) {}
        explicit FileError(int error):
            std::runtime_error(assemble(error, {})), names(), err(error) {}
        template <typename... More> FileError(int error, const NativeString& file, const More&... more):
            std::runtime_error(assemble(error, assemble_files(file, more...))), names{file, more...}, err(error) {}
        NativeString file() const { return names.empty() ? NativeString() : names.front(); }
        std::vector<NativeString> files() const { return names; }
        int error() const noexcept { return err; }
    private:
        std::vector<NativeString> names;
        int err;
        static u8string assemble(int error, const u8string& files);
        template <typename... Names> static u8string assemble_files(const Names&... files);
    };

    template <typename... Names>
    u8string FileError::assemble_files(const Names&... files) {
        if (sizeof...(files) == 0)
            return {};
        u8string text = "; file";
        if (sizeof...(files) > 1)
            text += 's';
        text += ' ';
        text += str_concat_with(", ", files...);
        return text;
    }

    // File name operations

    namespace UnicornDetail {

        template <typename C>
        std::basic_string<C> normalize_file(const std::basic_string<C>& file) {
            #if defined(_XOPEN_SOURCE)
                return file;
            #else
                auto nfile = file;
                std::replace(CROW_BOUNDS(nfile), C('/'), C('\\'));
                return nfile;
            #endif
        }

        inline NativeString native_file(const NativeString& file) {
            return normalize_file(file);
        }

        template <typename C>
        NativeString native_file(const std::basic_string<C>& file) {
            NativeString nfile;
            recode_filename(file, nfile);
            return normalize_file(nfile);
        }

        bool native_file_is_absolute(const u8string& file);
        bool native_file_is_root(const u8string& file);

        #if ! defined(_XOPEN_SOURCE)
            bool native_file_is_drive_absolute(const u8string& file);
            bool native_file_is_drive_relative(const u8string& file);
        #endif

    }

    template <typename C>
    bool file_is_absolute(const std::basic_string<C>& file) {
        return UnicornDetail::native_file_is_absolute(to_utf8(file));
    }

    template <typename C>
    bool file_is_drive_absolute(const std::basic_string<C>& file) {
        #if defined(_XOPEN_SOURCE)
            (void)file;
            return false;
        #else
            return UnicornDetail::native_file_is_drive_absolute(to_utf8(file));
        #endif
    }

    template <typename C>
    bool file_is_drive_relative(const std::basic_string<C>& file) {
        #if defined(_XOPEN_SOURCE)
            (void)file;
            return false;
        #else
            return UnicornDetail::native_file_is_drive_relative(to_utf8(file));
        #endif
    }

    template <typename C>
    bool file_is_relative(const std::basic_string<C>& file) {
        return ! (file_is_absolute(file) || file_is_drive_absolute(file) || file_is_drive_relative(file));
    }

    template <typename C>
    bool file_is_root(const std::basic_string<C>& file) {
        return UnicornDetail::native_file_is_root(to_utf8(file));
    }

    template <typename C>
    std::basic_string<C> file_path(const std::basic_string<C>& file) {
        return UnicornDetail::normalize_file(file);
    }

    template <typename C>
    std::basic_string<C> file_path(const C* file) {
        return UnicornDetail::normalize_file(cstr(file));
    }

    template <typename C, typename... Args>
    std::basic_string<C> file_path(const std::basic_string<C>& file1, const std::basic_string<C>& file2,
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
    std::basic_string<C> file_path(const std::basic_string<C>& file1, const C* file2, Args... args) {
        return file_path(file1, cstr(file2), args...);
    }

    template <typename C, typename... Args>
    std::basic_string<C> file_path(const C* file1, const std::basic_string<C>& file2, Args... args) {
        return file_path(cstr(file1), file2, args...);
    }

    template <typename C, typename... Args>
    std::basic_string<C> file_path(const C* file1, const C* file2, Args... args) {
        return file_path(cstr(file1), cstr(file2), args...);
    }

    template <typename C>
    std::pair<std::basic_string<C>, std::basic_string<C>> split_path(const std::basic_string<C>& file) {
        auto nfile = UnicornDetail::normalize_file(file);
        auto cut = nfile.find_last_of(static_cast<C>(file_delimiter));
        #if ! defined(_XOPEN_SOURCE)
            if (cut == 2 && char_is_ascii(file[0]) && ascii_isalpha(static_cast<char>(file[0])) && file[1] == C(':'))
                return {nfile.substr(0, 3), nfile.substr(3, npos)};
            else if (cut == npos && char_is_ascii(file[0]) && ascii_isalpha(static_cast<char>(file[0])) && file[1] == C(':'))
                return {nfile.substr(0, 2), nfile.substr(2, npos)};
        #endif
        if (cut == npos)
            return {{}, nfile};
        else if (cut == 0)
            return {nfile.substr(0, 1), nfile.substr(1, npos)};
        else
            return {nfile.substr(0, cut), nfile.substr(cut + 1, npos)};
    }

    template <typename C>
    std::pair<std::basic_string<C>, std::basic_string<C>> split_file(const std::basic_string<C>& file) {
        auto nfile = UnicornDetail::normalize_file(file);
        auto cut = nfile.find_last_of(static_cast<C>(file_delimiter));
        #if ! defined(_XOPEN_SOURCE)
            if (cut == npos && char_is_ascii(file[0]) && ascii_isalpha(static_cast<char>(file[0])) && file[1] == C(':'))
                cut = 1;
        #endif
        if (cut == npos)
            cut = 0;
        else
            ++cut;
        if (cut >= nfile.size())
            return {};
        auto dot = nfile.find_last_of(C('.'));
        if (dot > cut && dot != npos)
            return {nfile.substr(cut, dot - cut), nfile.substr(dot, npos)};
        else
            return {nfile.substr(cut, npos), {}};
    }

    // File properties

    namespace UnicornDetail {

        NativeString native_current_directory();
        bool native_file_exists(const NativeString& file) noexcept;
        bool native_file_is_directory(const NativeString& file) noexcept;
        bool native_file_is_hidden(const NativeString& file) noexcept;
        bool native_file_is_symlink(const NativeString& file) noexcept;

    }

    template <typename C>
    std::basic_string<C> current_directory() {
        std::basic_string<C> dir;
        recode_filename(UnicornDetail::native_current_directory(), dir);
        return dir;
    }

    template <typename C>
    void current_directory(std::basic_string<C>& dir) {
        recode_filename(UnicornDetail::native_current_directory(), dir);
    }

    template <typename C>
    bool file_exists(const std::basic_string<C>& file) noexcept {
        using namespace UnicornDetail;
        return native_file_exists(native_file(file));
    }

    template <typename C>
    bool file_is_directory(const std::basic_string<C>& file) noexcept {
        using namespace UnicornDetail;
        return native_file_is_directory(native_file(file));
    }

    template <typename C>
    bool file_is_hidden(const std::basic_string<C>& file) noexcept {
        using namespace UnicornDetail;
        return native_file_is_hidden(native_file(file));
    }

    template <typename C>
    bool file_is_symlink(const std::basic_string<C>& file) noexcept {
        using namespace UnicornDetail;
        return native_file_is_symlink(native_file(file));
    }

    // File system operations

    namespace UnicornDetail {

        void native_make_directory(const NativeString& dir, bool recurse);
        void native_remove_file(const NativeString& file, bool recurse);
        void native_rename_file(const NativeString& src, const NativeString& dst);

    }

    template <typename C>
    void make_directory(const std::basic_string<C>& dir, bool recurse = false) {
        using namespace UnicornDetail;
        native_make_directory(native_file(dir), recurse);
    }

    template <typename C>
    void remove_file(const std::basic_string<C>& file, bool recurse = false) {
        using namespace UnicornDetail;
        native_remove_file(native_file(file), recurse);
    }

    template <typename C>
    void rename_file(const std::basic_string<C>& src, const std::basic_string<C>& dst) {
        using namespace UnicornDetail;
        native_rename_file(native_file(src), native_file(dst));
    }

    // Directory iterators

    namespace UnicornDetail {

        class DirectoryHelper {
        public:
            void init(const NativeString& dir);
            const NativeString& file() const noexcept;
            void next();
            bool done() const noexcept { return ! impl; }
            bool equal(const DirectoryHelper& rhs) const noexcept { return impl == rhs.impl; }
        private:
            struct impl_type;
            std::shared_ptr<impl_type> impl;
        };

    }

    constexpr auto dir_dotdot    = Flagset::value('d');  // Include . and ..
    constexpr auto dir_fullname  = Flagset::value('f');  // Full file names
    constexpr auto dir_hidden    = Flagset::value('h');  // Include hidden files

    template <typename C>
    class DirectoryIterator {
    public:
        using difference_type = ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using value_type = std::basic_string<C>;
        using pointer = const value_type*;
        using reference = const value_type&;
        DirectoryIterator() = default;
        explicit DirectoryIterator(const value_type& dir, Flagset flags = {});
        const value_type& operator*() const noexcept { return current; }
        const value_type* operator->() const noexcept { return &**this; }
        DirectoryIterator& operator++();
        DirectoryIterator operator++(int) { auto i = *this; ++*this; return i; }
        friend bool operator==(const DirectoryIterator& lhs, const DirectoryIterator& rhs) noexcept
            { return lhs.impl.equal(rhs.impl); }
        friend bool operator!=(const DirectoryIterator& lhs, const DirectoryIterator& rhs) noexcept
            { return ! (lhs == rhs); }
    private:
        UnicornDetail::DirectoryHelper impl;
        value_type prefix;
        value_type current;
        Flagset fset;
    };

    template <typename C>
    DirectoryIterator<C>::DirectoryIterator(const value_type& dir, Flagset flags) {
        auto normdir = UnicornDetail::normalize_file(dir);
        NativeString natdir;
        recode_filename(normdir, natdir);
        impl.init(natdir);
        if (fset.get(dir_fullname) || ! fset.get(dir_hidden)) {
            prefix = std::move(normdir);
            if (! prefix.empty() && prefix.back() != static_cast<C>(file_delimiter))
                prefix += static_cast<C>(file_delimiter);
        }
        fset = flags;
        ++*this;
    }

    template <typename C>
    DirectoryIterator<C>& DirectoryIterator<C>::operator++() {
        static const value_type link1(1, C('.'));
        static const value_type link2(2, C('.'));
        current.clear();
        value_type path;
        for (;;) {
            impl.next();
            if (impl.done())
                break;
            recode_filename(impl.file(), current);
            if (fset.get(dir_dotdot) || (current != link1 && current != link2)) {
                if (fset.get(dir_fullname) || ! fset.get(dir_hidden))
                    path = prefix + current;
                if (fset.get(dir_hidden) || ! file_is_hidden(path)) {
                    if (fset.get(dir_fullname))
                        current = std::move(path);
                    break;
                }
            }
        }
        return *this;
    }

    template <typename C>
    Irange<DirectoryIterator<C>> directory(const std::basic_string<C>& dir, Flagset flags = {}) {
        return {DirectoryIterator<C>(dir, flags), DirectoryIterator<C>()};
    }

}
