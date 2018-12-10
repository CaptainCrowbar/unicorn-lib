#pragma once

#include "unicorn/utf.hpp"
#include "unicorn/utility.hpp"
#include <chrono>
#include <functional>
#include <iterator>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace RS::Unicorn {

    // File name class

    class Path:
    public LessThanComparable<Path> {

    public:

        // Types

        #ifdef _XOPEN_SOURCE
            using character_type = char;
        #else
            using character_type = wchar_t;
        #endif

        class deep_search_iterator;
        class directory_iterator;
        using deep_search_range = Irange<deep_search_iterator>;
        using directory_range = Irange<directory_iterator>;
        using flag_type = uint32_t;
        using id_type = std::pair<uint64_t, uint64_t>;
        using string_type = std::basic_string<character_type>;
        using string_view_type = std::basic_string_view<character_type>;

        enum class form {
            empty,
            absolute,
            drive_absolute,
            drive_relative,
            relative,
        };

        // Constants

        #ifdef _XOPEN_SOURCE
            static constexpr char delimiter = '/';
            static constexpr char native_delimiter = '/';
        #else
            static constexpr char delimiter = '\\';
            static constexpr wchar_t native_delimiter = L'\\';
        #endif

        #if defined(__APPLE__) || defined(__CYGWIN__) || defined(_WIN32)
            static constexpr bool native_case = false;
        #else
            static constexpr bool native_case = true;
        #endif

        static constexpr auto append       = flag_type(1) << 3;   // Append if file exists
        static constexpr auto bottom_up    = flag_type(1) << 4;   // Bottom up order
        static constexpr auto legal_name   = flag_type(1) << 5;   // Throw on illegal file name
        static constexpr auto may_copy     = flag_type(1) << 6;   // Copy if operation not allowed
        static constexpr auto may_fail     = flag_type(1) << 7;   // Empty string if read fails
        static constexpr auto no_follow    = flag_type(1) << 8;   // Don't follow symlinks
        static constexpr auto no_hidden    = flag_type(1) << 9;   // Skip hidden files
        static constexpr auto overwrite    = flag_type(1) << 10;  // Delete existing file if necessary
        static constexpr auto recurse      = flag_type(1) << 11;  // Recursive directory operations
        static constexpr auto std_default  = flag_type(1) << 12;  // Use stdin/out if file is "" or "-"
        static constexpr auto unicode      = flag_type(1) << 13;  // Skip files with non-Unicode names

        // Comparison objects

        enum class cmp {
            cased,
            icase,
            native = native_case ? cased : icase,
        };

        class equal {
        public:
            equal() = default;
            explicit equal(cmp mode) noexcept: cmode(mode) {}
            bool operator()(const Path& lhs, const Path& rhs) const;
        private:
            cmp cmode = cmp::native;
        };

        class less {
        public:
            less() = default;
            explicit less(cmp mode) noexcept: cmode(mode) {}
            bool operator()(const Path& lhs, const Path& rhs) const;
        private:
            cmp cmode = cmp::native;
        };

        // Life cycle functions

        Path() = default;
        template <typename C> Path(const std::basic_string<C>& file, flag_type flags = 0);
        template <typename C> Path(const std::basic_string_view<C>& file, flag_type flags = 0): Path(std::basic_string<C>(file), flags) {}
        template <typename C> Path(const C* file, flag_type flags = 0): Path(cstr(file), flags) {}

        // Path name functions

        Ustring name(flag_type flags = Utf::replace) const { return to_utf8(filename, flags); }
        string_type os_name() const { return filename; }
        string_view_type os_view() const noexcept { return filename; }
        const character_type* c_name() const noexcept { return filename.data(); }
        Ustring as_url(flag_type flags = Utf::replace) const;
        Strings breakdown() const;
        std::vector<string_type> os_breakdown() const;
        Path change_ext(Uview new_ext) const;
        bool empty() const noexcept { return filename.empty(); }
        size_t hash() const noexcept { return std::hash<string_type>()(filename); }
        bool is_absolute() const noexcept;
        bool is_drive_absolute() const noexcept;
        bool is_drive_relative() const noexcept;
        bool is_relative() const noexcept;
        bool is_leaf() const noexcept;
        bool is_legal() const noexcept;
        bool is_root() const noexcept;
        bool is_unicode() const noexcept { return valid_string(filename); }
        form path_form() const noexcept;
        Path relative_to(const Path& base) const;
        std::pair<Ustring, Ustring> split_leaf() const;
        std::pair<string_type, string_type> split_os_leaf() const;
        std::pair<Path, Path> split_path() const;
        std::pair<Path, Path> split_root() const;
        static Path common(const Path& lhs, const Path& rhs);
        template <typename Range> static Path common(const Range& files);
        static Path join(const Path& lhs, const Path& rhs);
        template <typename Range> static Path join(const Range& files);
        friend Path operator/(const Path& lhs, const Path& rhs) { return Path::join(lhs, rhs); }
        friend Path& operator/=(Path& lhs, const Path& rhs) { lhs = Path::join(lhs, rhs); return lhs; }
        friend Ustring to_str(const Path& p) { return p.name(); }
        friend Ustring to_str(Path::form f);
        friend std::ostream& operator<<(std::ostream& out, const Path& p) { return out << p.name(); }
        friend std::ostream& operator<<(std::ostream& out, Path::form f) { return out << to_str(f); }
        friend bool operator==(const Path& lhs, const Path& rhs) noexcept { return lhs.filename == rhs.filename; }
        friend bool operator<(const Path& lhs, const Path& rhs) noexcept { return lhs.filename < rhs.filename; }

        // File system query functions

        std::chrono::system_clock::time_point access_time(flag_type flags = 0) const noexcept;
        std::chrono::system_clock::time_point create_time(flag_type flags = 0) const noexcept;
        std::chrono::system_clock::time_point modify_time(flag_type flags = 0) const noexcept;
        std::chrono::system_clock::time_point status_time(flag_type flags = 0) const noexcept;
        directory_range directory(flag_type flags = 0) const;
        deep_search_range deep_search(flag_type flags = 0) const;
        bool exists(flag_type flags = 0) const noexcept;
        id_type id(flag_type flags = 0) const noexcept;
        bool is_directory(flag_type flags = 0) const noexcept;
        bool is_file(flag_type flags = 0) const noexcept;
        bool is_special(flag_type flags = 0) const noexcept;
        bool is_hidden() const noexcept;
        bool is_symlink() const noexcept;
        Path resolve() const;
        Path resolve_symlink() const;
        uint64_t size(flag_type flags = 0) const;

        // File system update functions

        void copy_to(const Path& dst, flag_type flags = 0) const;
        void create() const;
        void make_directory(flag_type flags = 0) const;
        void make_symlink(const Path& linkname, flag_type flags = 0) const;
        void move_to(const Path& dst, flag_type flags = 0) const;
        void remove(flag_type flags = 0) const;
        void set_access_time(flag_type flags = 0) const { set_access_time(std::chrono::system_clock::now(), flags); }
        void set_access_time(std::chrono::system_clock::time_point t, flag_type flags = 0) const;
        void set_create_time(flag_type flags = 0) const { set_create_time(std::chrono::system_clock::now(), flags); }
        void set_create_time(std::chrono::system_clock::time_point t, flag_type flags = 0) const;
        void set_modify_time(flag_type flags = 0) const { set_modify_time(std::chrono::system_clock::now(), flags); }
        void set_modify_time(std::chrono::system_clock::time_point t, flag_type flags = 0) const;
        void set_status_time(flag_type flags = 0) const { set_status_time(std::chrono::system_clock::now(), flags); }
        void set_status_time(std::chrono::system_clock::time_point t, flag_type flags = 0) const;

        // I/O functions

        void load(std::string& dst, size_t maxlen = npos, flag_type flags = 0) const;
        void save(std::string_view src, flag_type flags = 0) const;

        // Process state functions

        void change_directory() const;
        static Path current_directory();

    private:

        using view_type = std::basic_string_view<character_type>;

        string_type filename;

        std::pair<view_type, view_type> find_base_ext() const noexcept;
        view_type find_leaf() const noexcept;
        view_type find_root(bool allow_drive_special) const noexcept;
        void make_canonical(flag_type flags);
        template <typename Range, typename BinaryFunction> static Path do_combine(const Range& range, BinaryFunction f);

        #ifdef _XOPEN_SOURCE
            void set_file_times(std::chrono::system_clock::time_point atime, std::chrono::system_clock::time_point mtime, flag_type flags) const;
        #else
            std::chrono::system_clock::time_point get_file_time(int index) const noexcept;
            void set_file_time(std::chrono::system_clock::time_point t, int index) const;
        #endif

    };

        template <typename C>
        Path::Path(const std::basic_string<C>& file, flag_type flags) {
            flag_type uflags = flags & Utf::mask;
            if (uflags == 0)
                uflags = sizeof(C) == sizeof(character_type) ? Utf::ignore : Utf::replace;
            filename = recode<character_type>(file, uflags);
            make_canonical(flags);
        }

        template <typename Range>
        Path Path::common(const Range& files) {
            return do_combine(files, [] (const Path& lhs, const Path& rhs) { return common(lhs, rhs); });
        }

        template <typename Range>
        Path Path::join(const Range& files) {
            return do_combine(files, [] (const Path& lhs, const Path& rhs) { return join(lhs, rhs); });
        }

        template <typename Range, typename BinaryFunction>
        Path Path::do_combine(const Range& range, BinaryFunction f) {
            using std::begin;
            using std::end;
            auto i = begin(range), j = end(range);
            if (i == j)
                return {};
            Path p = *i;
            for (++i; i != j; ++i)
                p = f(p, *i);
            return p;
        }

        class Path::deep_search_iterator:
        public InputIterator<deep_search_iterator, const Path> {
        public:
            deep_search_iterator() = default;
            deep_search_iterator(const Path& dir, flag_type flags);
            const Path& operator*() const noexcept;
            deep_search_iterator& operator++();
            bool operator==(const deep_search_iterator& i) const noexcept { return impl == i.impl; }
        private:
            struct impl_type;
            std::shared_ptr<impl_type> impl;
        };

        class Path::directory_iterator:
        public InputIterator<directory_iterator, const Path> {
        public:
            directory_iterator() = default;
            directory_iterator(const Path& dir, flag_type flags);
            const Path& operator*() const noexcept;
            directory_iterator& operator++();
            bool operator==(const directory_iterator& i) const noexcept { return impl == i.impl; }
        private:
            struct impl_type;
            std::shared_ptr<impl_type> impl;
        };

}

RS_DEFINE_STD_HASH(RS::Unicorn::Path);
