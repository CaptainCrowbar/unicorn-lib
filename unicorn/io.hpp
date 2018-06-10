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

        static constexpr uint32_t bom        = 1ul << 3;   // Strip or insert a BOM
        static constexpr uint32_t lf         = 1ul << 4;   // Convert all line breaks to LF
        static constexpr uint32_t crlf       = 1ul << 5;   // Convert all line breaks to CR+LF

        // Reader flags

        static constexpr uint32_t standin    = 1ul << 6;   // Default to stdin
        static constexpr uint32_t pretend    = 1ul << 7;   // Treat nonexistent file as empty
        static constexpr uint32_t striplf    = 1ul << 8;   // Strip line breaks
        static constexpr uint32_t striptws   = 1ul << 9;   // Strip trailing whitespace
        static constexpr uint32_t stripws    = 1ul << 10;  // Strip whitespace
        static constexpr uint32_t notempty   = 1ul << 11;  // Skip empty lines

        // Writer flags

        static constexpr uint32_t standout   = 1ul << 12;  // Default to stdout
        static constexpr uint32_t standerr   = 1ul << 13;  // Default to stderr
        static constexpr uint32_t append     = 1ul << 14;  // Append to file
        static constexpr uint32_t protect    = 1ul << 15;  // Fail if the file already exists
        static constexpr uint32_t linebuf    = 1ul << 16;  // Line buffered output
        static constexpr uint32_t unbuf      = 1ul << 17;  // Unbuffered output
        static constexpr uint32_t writeline  = 1ul << 18;  // Write LF after every write
        static constexpr uint32_t autoline   = 1ul << 19;  // Write LF if not already there
        static constexpr uint32_t mutex      = 1ul << 20;  // Hold per-file mutex while writing

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
