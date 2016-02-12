#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/file.hpp"
#include "unicorn/utf.hpp"
#include <cstdio>
#include <iterator>
#include <memory>
#include <string>

namespace Unicorn {

    // I/O flags

    // Skip the first 3 bits because these need to be compatible with the encoding error flags

    // Common reader and writer flags

    constexpr uint32_t io_bom        = 1ul << 3;   // Strip or insert a BOM
    constexpr uint32_t io_lf         = 1ul << 4;   // Convert all line breaks to LF
    constexpr uint32_t io_crlf       = 1ul << 5;   // Convert all line breaks to CR+LF

    // Reader flags

    constexpr uint32_t io_stdin      = 1ul << 6;   // Default to stdin
    constexpr uint32_t io_nofail     = 1ul << 7;   // Treat nonexistent file as empty
    constexpr uint32_t io_striplf    = 1ul << 8;   // Strip line breaks
    constexpr uint32_t io_striptws   = 1ul << 9;   // Strip trailing whitespace
    constexpr uint32_t io_stripws    = 1ul << 10;  // Strip whitespace
    constexpr uint32_t io_notempty   = 1ul << 11;  // Skip empty lines

    // Writer flags

    constexpr uint32_t io_stdout     = 1ul << 12;  // Default to stdout
    constexpr uint32_t io_stderr     = 1ul << 13;  // Default to stderr
    constexpr uint32_t io_append     = 1ul << 14;  // Append to file
    constexpr uint32_t io_linebuf    = 1ul << 15;  // Line buffered output
    constexpr uint32_t io_unbuf      = 1ul << 16;  // Unbuffered output
    constexpr uint32_t io_writeline  = 1ul << 17;  // Write LF after every write
    constexpr uint32_t io_autoline   = 1ul << 18;  // Write LF if not already there
    constexpr uint32_t io_mutex      = 1ul << 19;  // Hold per-file mutex while writing

    // File input iterator

    class FileReader:
    public InputIterator<FileReader, u8string> {
    public:
        FileReader() {}
        template <typename C> explicit FileReader(const basic_string<C>& file)
            { init(recode_filename<NativeCharacter>(file), {}, {}, {}); }
        template <typename C> FileReader(const basic_string<C>& file, uint32_t flags)
            { init(recode_filename<NativeCharacter>(file), flags, {}, {}); }
        template <typename C1, typename C2> FileReader(const basic_string<C1>& file, uint32_t flags, const basic_string<C2>& enc)
            { init(recode_filename<NativeCharacter>(file), flags, to_utf8(enc), {}); }
        template <typename C> FileReader(const basic_string<C>& file, uint32_t flags, uint32_t enc)
            { init(recode_filename<NativeCharacter>(file), flags, dec(enc), {}); }
        template <typename C1, typename C2, typename C3> FileReader(const basic_string<C1>& file, uint32_t flags,
            const basic_string<C2>& enc, const basic_string<C3>& eol)
            { init(recode_filename<NativeCharacter>(file), flags, to_utf8(enc), to_utf8(eol)); }
        template <typename C1, typename C2> FileReader(const basic_string<C1>& file, uint32_t flags, uint32_t enc,
            const basic_string<C2>& eol)
            { init(recode_filename<NativeCharacter>(file), flags, dec(enc), to_utf8(eol)); }
        const u8string& operator*() const noexcept;
        FileReader& operator++();
        friend bool operator==(const FileReader& lhs, const FileReader& rhs) noexcept { return lhs.impl == rhs.impl; }
        size_t line() const noexcept;
    private:
        struct impl_type;
        shared_ptr<impl_type> impl;
        void init(const NativeString& file, uint32_t flags, const u8string& enc, const u8string& eol);
        void fixline();
        void getline();
        void getmore(size_t n);
    };

    template <typename C>
        Irange<FileReader> read_lines(const basic_string<C>& file, uint32_t flags = 0)
        { return {FileReader{file, flags}, {}}; }
    template <typename C1, typename C2>
        Irange<FileReader> read_lines(const basic_string<C1>& file, uint32_t flags, const basic_string<C2>& enc)
        { return {{file, flags, enc}, {}}; }
    template <typename C>
        Irange<FileReader> read_lines(const basic_string<C>& file, uint32_t flags, uint32_t enc)
        { return {{file, flags, enc}, {}}; }
    template <typename C1, typename C2, typename C3>
        Irange<FileReader> read_lines(const basic_string<C1>& file, uint32_t flags, const basic_string<C2>& enc, const basic_string<C3>& eol)
        { return {{file, flags, enc, eol}, {}}; }
    template <typename C1, typename C2>
        Irange<FileReader> read_lines(const basic_string<C1>& file, uint32_t flags, uint32_t enc, const basic_string<C2>& eol)
        { return {{file, flags, enc, eol}, {}}; }

    // File output iterator

    class FileWriter:
    public OutputIterator<FileWriter> {
    public:
        FileWriter() {}
        template <typename C> explicit FileWriter(const basic_string<C>& file)
            { init(recode_filename<NativeCharacter>(file), {}, {}); }
        template <typename C> FileWriter(const basic_string<C>& file, uint32_t flags)
            { init(recode_filename<NativeCharacter>(file), flags, {}); }
        template <typename C1, typename C2> FileWriter(const basic_string<C1>& file, uint32_t flags, const basic_string<C2>& enc)
            { init(recode_filename<NativeCharacter>(file), flags, to_utf8(enc)); }
        template <typename C> FileWriter(const basic_string<C>& file, uint32_t flags, uint32_t enc)
            { init(recode_filename<NativeCharacter>(file), flags, dec(enc)); }
        template <typename C> FileWriter& operator=(const basic_string<C>& str) { write(to_utf8(str)); return *this; }
        template <typename C> FileWriter& operator=(const C* str) { write(to_utf8(cstr(str))); return *this; }
        void flush();
    private:
        struct impl_type;
        shared_ptr<impl_type> impl;
        void init(const NativeString& file, uint32_t flags, const u8string& enc);
        void fixtext(u8string& str) const;
        void write(u8string str);
        void writembcs(const string& str);
    };

}
