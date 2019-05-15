#pragma once

#include "unicorn/character.hpp"
#include "unicorn/path.hpp"
#include "unicorn/utf.hpp"
#include "unicorn/utility.hpp"
#include <memory>
#include <string>

namespace RS::Unicorn {

    // I/O flags

    struct IO {

        // Skip the first 3 bits because these need to be compatible with the encoding error flags

        // Common reader and writer flags

        static constexpr uint32_t bom        = setbit<3>;   // Strip or insert a BOM
        static constexpr uint32_t crlf       = setbit<4>;   // Convert all line breaks to CR+LF
        static constexpr uint32_t lf         = setbit<5>;   // Convert all line breaks to LF

        // Reader flags

        static constexpr uint32_t notempty   = setbit<6>;   // Skip empty lines
        static constexpr uint32_t pretend    = setbit<7>;   // Treat nonexistent file as empty
        static constexpr uint32_t standin    = setbit<8>;   // Default to stdin
        static constexpr uint32_t striplf    = setbit<9>;   // Strip line breaks
        static constexpr uint32_t striptws   = setbit<10>;  // Strip trailing whitespace
        static constexpr uint32_t stripws    = setbit<11>;  // Strip whitespace

        // Writer flags

        static constexpr uint32_t append     = setbit<12>;  // Append to file
        static constexpr uint32_t autoline   = setbit<13>;  // Write LF if not already there
        static constexpr uint32_t linebuf    = setbit<14>;  // Line buffered output
        static constexpr uint32_t mutex      = setbit<15>;  // Hold per-file mutex while writing
        static constexpr uint32_t protect    = setbit<16>;  // Fail if the file already exists
        static constexpr uint32_t standerr   = setbit<17>;  // Default to stderr
        static constexpr uint32_t standout   = setbit<18>;  // Default to stdout
        static constexpr uint32_t unbuf      = setbit<19>;  // Unbuffered output
        static constexpr uint32_t writeline  = setbit<20>;  // Write LF after every write

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
