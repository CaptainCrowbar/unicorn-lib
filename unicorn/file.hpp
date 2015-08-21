#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
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

    #if defined(CROW_TARGET_UNIX)
        constexpr char file_delimiter = '/';
    #else
        constexpr char file_delimiter = '\\';
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

    // Exceptions

    class FileError:
    public std::runtime_error {
    public:
        FileError():
            std::runtime_error(assemble(0, {})), names(), err(0) {}
        explicit FileError(int error):
            std::runtime_error(assemble(error, {})), names(), err(error) {}
        template <typename... More> FileError(int error, const NativeString& file, const More&... more);
        const NativeCharacter* file(size_t i = 0) const noexcept;
        size_t files() const noexcept { return names ? names->size() : size_t(0); }
        int error() const noexcept { return err; }
    private:
        std::shared_ptr<std::vector<NativeString>> names;
        int err;
        static u8string assemble(int error, const u8string& files);
        template <typename... Names> static u8string assemble_files(const Names&... files);
    };

    template <typename... More>
    FileError::FileError(int error, const NativeString& file, const More&... more):
    std::runtime_error(assemble(error, assemble_files(file, more...))),
    names(std::make_shared<std::vector<NativeString>>()),
    err(error) {
        *names = {file, more...};
    }

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
        basic_string<C> normalize_file(const basic_string<C>& file) {
            #if defined(CROW_TARGET_UNIX)
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
        NativeString native_file(const basic_string<C>& file) {
            NativeString nfile;
            recode_filename(file, nfile);
            return normalize_file(nfile);
        }

        bool native_file_is_absolute(const u8string& file);
        bool native_file_is_root(const u8string& file);

        #if defined(CROW_TARGET_NATIVE_WINDOWS)
            bool native_file_is_drive_absolute(const u8string& file);
            bool native_file_is_drive_relative(const u8string& file);
        #endif

    }

    template <typename C>
    bool file_is_absolute(const basic_string<C>& file) {
        return UnicornDetail::native_file_is_absolute(to_utf8(file));
    }

    template <typename C>
    bool file_is_drive_absolute(const basic_string<C>& file) {
        #if defined(CROW_TARGET_UNIX)
            (void)file;
            return false;
        #else
            return UnicornDetail::native_file_is_drive_absolute(to_utf8(file));
        #endif
    }

    template <typename C>
    bool file_is_drive_relative(const basic_string<C>& file) {
        #if defined(CROW_TARGET_UNIX)
            (void)file;
            return false;
        #else
            return UnicornDetail::native_file_is_drive_relative(to_utf8(file));
        #endif
    }

    template <typename C>
    bool file_is_relative(const basic_string<C>& file) {
        return ! (file_is_absolute(file) || file_is_drive_absolute(file) || file_is_drive_relative(file));
    }

    template <typename C>
    bool file_is_root(const basic_string<C>& file) {
        return UnicornDetail::native_file_is_root(to_utf8(file));
    }

    template <typename C>
    basic_string<C> file_path(const basic_string<C>& file) {
        return UnicornDetail::normalize_file(file);
    }

    template <typename C>
    basic_string<C> file_path(const C* file) {
        return UnicornDetail::normalize_file(Crow::cstr(file));
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
        return file_path(file1, Crow::cstr(file2), args...);
    }

    template <typename C, typename... Args>
    basic_string<C> file_path(const C* file1, const basic_string<C>& file2, Args... args) {
        return file_path(Crow::cstr(file1), file2, args...);
    }

    template <typename C, typename... Args>
    basic_string<C> file_path(const C* file1, const C* file2, Args... args) {
        return file_path(Crow::cstr(file1), Crow::cstr(file2), args...);
    }

    template <typename C>
    std::pair<basic_string<C>, basic_string<C>> split_path(const basic_string<C>& file, bool keep = false) {
        auto nfile = UnicornDetail::normalize_file(file);
        auto cut = nfile.find_last_of(static_cast<C>(file_delimiter));
        #if defined(CROW_TARGET_NATIVE_WINDOWS)
            if (cut == 2 && file[1] == C(':')
                    && char_is_ascii(file[0]) && Crow::ascii_isalpha(static_cast<char>(file[0])))
                return {nfile.substr(0, 3), nfile.substr(3, npos)};
            else if (cut == npos && file[1] == C(':')
                    && char_is_ascii(file[0]) && Crow::ascii_isalpha(static_cast<char>(file[0])))
                return {nfile.substr(0, 2), nfile.substr(2, npos)};
        #endif
        if (cut == npos)
            return {{}, nfile};
        else if (cut == 0)
            return {nfile.substr(0, 1), nfile.substr(1, npos)};
        else
            return {nfile.substr(0, cut + static_cast<size_t>(keep)), nfile.substr(cut + 1, npos)};
    }

    template <typename C>
    std::pair<basic_string<C>, basic_string<C>> split_file(const basic_string<C>& file) {
        auto nfile = UnicornDetail::normalize_file(file);
        auto cut = nfile.find_last_of(static_cast<C>(file_delimiter));
        #if defined(CROW_TARGET_NATIVE_WINDOWS)
            if (cut == npos && char_is_ascii(file[0]) && Crow::ascii_isalpha(static_cast<char>(file[0]))
                    && file[1] == C(':'))
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

    // File system operations

    namespace UnicornDetail {

        void native_make_directory(const NativeString& dir, bool recurse);
        void native_remove_file(const NativeString& file, bool recurse);
        void native_rename_file(const NativeString& src, const NativeString& dst);

    }

    template <typename C>
    void make_directory(const basic_string<C>& dir, bool recurse = false) {
        using namespace UnicornDetail;
        native_make_directory(native_file(dir), recurse);
    }

    template <typename C>
    void remove_file(const basic_string<C>& file, bool recurse = false) {
        using namespace UnicornDetail;
        native_remove_file(native_file(file), recurse);
    }

    template <typename C>
    void rename_file(const basic_string<C>& src, const basic_string<C>& dst) {
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

    constexpr auto dir_dotdot    = Crow::Flagset::value('d');  // Include . and ..
    constexpr auto dir_fullname  = Crow::Flagset::value('f');  // Full file names
    constexpr auto dir_hidden    = Crow::Flagset::value('h');  // Include hidden files
    constexpr auto dir_unicode   = Crow::Flagset::value('u');  // Valid Unicode names only

    template <typename C>
    class DirectoryIterator:
    public Crow::InputIterator<DirectoryIterator<C>, const basic_string<C>> {
    public:
        DirectoryIterator() = default;
        explicit DirectoryIterator(const basic_string<C>& dir, Crow::Flagset flags = {});
        const basic_string<C>& operator*() const noexcept { return current; }
        DirectoryIterator& operator++();
        friend bool operator==(const DirectoryIterator& lhs, const DirectoryIterator& rhs) noexcept
            { return lhs.impl.equal(rhs.impl); }
    private:
        UnicornDetail::DirectoryHelper impl;
        basic_string<C> prefix;
        basic_string<C> current;
        Crow::Flagset fset;
    };

    template <typename C>
    DirectoryIterator<C>::DirectoryIterator(const basic_string<C>& dir, Crow::Flagset flags) {
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
        static const basic_string<C> link1(1, C('.'));
        static const basic_string<C> link2(2, C('.'));
        current.clear();
        basic_string<C> path;
        for (;;) {
            impl.next();
            if (impl.done())
                break;
            if (fset.get(dir_unicode) && ! valid_string(impl.file()))
                continue;
            recode_filename(impl.file(), current);
            if (! fset.get(dir_dotdot) && (current == link1 || current == link2))
                continue;
            if (fset.get(dir_fullname) || ! fset.get(dir_hidden))
                path = prefix + current;
            if (! fset.get(dir_hidden) && file_is_hidden(path))
                continue;
            if (fset.get(dir_fullname))
                current = std::move(path);
            break;
        }
        return *this;
    }

    template <typename C>
    Crow::Irange<DirectoryIterator<C>> directory(const basic_string<C>& dir, Crow::Flagset flags = {}) {
        return {DirectoryIterator<C>(dir, flags), DirectoryIterator<C>()};
    }

}
