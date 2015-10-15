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

    // Skip the first 3 bits because these need to be compatible with the
    // encoding error flags.

    // Common reader and writer flags

    UNICORN_DEFINE_FLAG(file io, io_bom,   3);  // Strip or insert a BOM
    UNICORN_DEFINE_FLAG(file io, io_lf,    4);  // Convert all line breaks to LF
    UNICORN_DEFINE_FLAG(file io, io_crlf,  5);  // Convert all line breaks to CR+LF

    // Reader flags

    UNICORN_DEFINE_FLAG(file io, io_stdin,     6);   // Default to stdin
    UNICORN_DEFINE_FLAG(file io, io_nofail,    7);   // Treat nonexistent file as empty
    UNICORN_DEFINE_FLAG(file io, io_striplf,   8);   // Strip line breaks
    UNICORN_DEFINE_FLAG(file io, io_striptws,  9);   // Strip trailing whitespace
    UNICORN_DEFINE_FLAG(file io, io_stripws,   10);  // Strip whitespace
    UNICORN_DEFINE_FLAG(file io, io_notempty,  11);  // Skip empty lines

    // Writer flags

    UNICORN_DEFINE_FLAG(file io, io_stdout,     12);  // Default to stdout
    UNICORN_DEFINE_FLAG(file io, io_stderr,     13);  // Default to stderr
    UNICORN_DEFINE_FLAG(file io, io_append,     14);  // Append to file
    UNICORN_DEFINE_FLAG(file io, io_linebuf,    15);  // Line buffered output
    UNICORN_DEFINE_FLAG(file io, io_unbuf,      16);  // Unbuffered output
    UNICORN_DEFINE_FLAG(file io, io_writeline,  17);  // Write LF after every write
    UNICORN_DEFINE_FLAG(file io, io_autoline,   18);  // Write LF if not already there
    UNICORN_DEFINE_FLAG(file io, io_mutex,      19);  // Hold per-file mutex while writing

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
