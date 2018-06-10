#pragma once

#include "unicorn/character.hpp"
#include "unicorn/path.hpp"
#include "unicorn/utf.hpp"
#include "unicorn/utility.hpp"
#include <cstdio>
#include <memory>
#include <string>

namespace RS::Unicorn {

    // I/O flags

    struct IO {

        // Skip the first 3 bits because these need to be compatible with the encoding error flags

        // Common reader and writer flags

        static constexpr auto bom        = uint32_t(1) << 3;   // Strip or insert a BOM
        static constexpr auto crlf       = uint32_t(1) << 4;   // Convert all line breaks to CR+LF
        static constexpr auto lf         = uint32_t(1) << 5;   // Convert all line breaks to LF

        // Reader flags

        static constexpr auto notempty   = uint32_t(1) << 6;   // Skip empty lines
        static constexpr auto pretend    = uint32_t(1) << 7;   // Treat nonexistent file as empty
        static constexpr auto standin    = uint32_t(1) << 8;   // Default to stdin
        static constexpr auto striplf    = uint32_t(1) << 9;   // Strip line breaks
        static constexpr auto striptws   = uint32_t(1) << 10;  // Strip trailing whitespace
        static constexpr auto stripws    = uint32_t(1) << 11;  // Strip whitespace

        // Writer flags

        static constexpr auto append     = uint32_t(1) << 12;  // Append to file
        static constexpr auto autoline   = uint32_t(1) << 13;  // Write LF if not already there
        static constexpr auto linebuf    = uint32_t(1) << 14;  // Line buffered output
        static constexpr auto mutex      = uint32_t(1) << 15;  // Hold per-file mutex while writing
        static constexpr auto protect    = uint32_t(1) << 16;  // Fail if the file already exists
        static constexpr auto standerr   = uint32_t(1) << 17;  // Default to stderr
        static constexpr auto standout   = uint32_t(1) << 18;  // Default to stdout
        static constexpr auto unbuf      = uint32_t(1) << 19;  // Unbuffered output
        static constexpr auto writeline  = uint32_t(1) << 20;  // Write LF after every write

    };

    // File input iterator

    class FileReader:
    public InputIterator<FileReader, Ustring> {
    public:
        FileReader() {}
        explicit FileReader(const Path& file) { init(file, {}, {}, {}); }
        FileReader(const Path& file, uint32_t flags) { init(file, flags, {}, {}); }
        FileReader(const Path& file, uint32_t flags, const Ustring& enc) { init(file, flags, enc, {}); }
        FileReader(const Path& file, uint32_t flags, uint32_t enc) { init(file, flags, std::to_string(enc), {}); }
        FileReader(const Path& file, uint32_t flags, const Ustring& enc, const Ustring& eol) { init(file, flags, enc, eol); }
        FileReader(const Path& file, uint32_t flags, uint32_t enc, const Ustring& eol) { init(file, flags, std::to_string(enc), eol); }
        const Ustring& operator*() const noexcept;
        FileReader& operator++();
        friend bool operator==(const FileReader& lhs, const FileReader& rhs) noexcept { return lhs.impl == rhs.impl; }
        size_t line() const noexcept;
    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl;
        void init(const Path& file, uint32_t flags, const Ustring& enc, const Ustring& eol);
        void fixline();
        void getline();
        void getmore(size_t n);
    };

    inline Irange<FileReader> read_lines(const Path& file, uint32_t flags = 0) { return {FileReader{file, flags}, {}}; }
    inline Irange<FileReader> read_lines(const Path& file, uint32_t flags, const Ustring& enc) { return {{file, flags, enc}, {}}; }
    inline Irange<FileReader> read_lines(const Path& file, uint32_t flags, uint32_t enc) { return {{file, flags, enc}, {}}; }
    inline Irange<FileReader> read_lines(const Path& file, uint32_t flags, const Ustring& enc, const Ustring& eol) { return {{file, flags, enc, eol}, {}}; }
    inline Irange<FileReader> read_lines(const Path& file, uint32_t flags, uint32_t enc, const Ustring& eol) { return {{file, flags, enc, eol}, {}}; }

    // File output iterator

    class FileWriter:
    public OutputIterator<FileWriter> {
    public:
        FileWriter() {}
        explicit FileWriter(const Path& file) { init(file, {}, {}); }
        FileWriter(const Path& file, uint32_t flags) { init(file, flags, {}); }
        FileWriter(const Path& file, uint32_t flags, const Ustring& enc) { init(file, flags, enc); }
        FileWriter(const Path& file, uint32_t flags, uint32_t enc) { init(file, flags, std::to_string(enc)); }
        FileWriter& operator=(const Ustring& str) { write(str); return *this; }
        FileWriter& operator=(const char* str) { write(cstr(str)); return *this; }
        void flush();
    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl;
        void init(const Path& file, uint32_t flags, const Ustring& enc);
        void fix_text(Ustring& str) const;
        void write(Ustring str);
        void write_mbcs(const std::string& str);
    };

}
