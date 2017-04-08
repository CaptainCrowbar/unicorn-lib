#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/file.hpp"
#include "unicorn/utf.hpp"
#include <cstdio>
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
    constexpr uint32_t io_pretend    = 1ul << 7;   // Treat nonexistent file as empty
    constexpr uint32_t io_striplf    = 1ul << 8;   // Strip line breaks
    constexpr uint32_t io_striptws   = 1ul << 9;   // Strip trailing whitespace
    constexpr uint32_t io_stripws    = 1ul << 10;  // Strip whitespace
    constexpr uint32_t io_notempty   = 1ul << 11;  // Skip empty lines

    // Writer flags

    constexpr uint32_t io_stdout     = 1ul << 12;  // Default to stdout
    constexpr uint32_t io_stderr     = 1ul << 13;  // Default to stderr
    constexpr uint32_t io_append     = 1ul << 14;  // Append to file
    constexpr uint32_t io_protect    = 1ul << 15;  // Fail if the file already exists
    constexpr uint32_t io_linebuf    = 1ul << 16;  // Line buffered output
    constexpr uint32_t io_unbuf      = 1ul << 17;  // Unbuffered output
    constexpr uint32_t io_writeline  = 1ul << 18;  // Write LF after every write
    constexpr uint32_t io_autoline   = 1ul << 19;  // Write LF if not already there
    constexpr uint32_t io_mutex      = 1ul << 20;  // Hold per-file mutex while writing

    // File input iterator

    class FileReader:
    public InputIterator<FileReader, U8string> {
    public:
        FileReader() {}
        #if defined(UNICORN_NATIVE_WCHAR)
            explicit FileReader(const U8string& file)
                { init(to_wstring(file), {}, {}, {}); }
            FileReader(const U8string& file, uint32_t flags)
                { init(to_wstring(file), flags, {}, {}); }
            FileReader(const U8string& file, uint32_t flags, const U8string& enc)
                { init(to_wstring(file), flags, to_utf8(enc), {}); }
            FileReader(const U8string& file, uint32_t flags, uint32_t enc)
                { init(to_wstring(file), flags, dec(enc), {}); }
            FileReader(const U8string& file, uint32_t flags, const U8string& enc, const U8string& eol)
                { init(to_wstring(file), flags, to_utf8(enc), to_utf8(eol)); }
            FileReader(const U8string& file, uint32_t flags, uint32_t enc, const U8string& eol)
                { init(to_wstring(file), flags, dec(enc), to_utf8(eol)); }
            explicit FileReader(const NativeString& file)
                { init(file, {}, {}, {}); }
            FileReader(const NativeString& file, uint32_t flags)
                { init(file, flags, {}, {}); }
            FileReader(const NativeString& file, uint32_t flags, const U8string& enc)
                { init(file, flags, enc, {}); }
            FileReader(const NativeString& file, uint32_t flags, uint32_t enc)
                { init(file, flags, dec(enc), {}); }
            FileReader(const NativeString& file, uint32_t flags, const U8string& enc, const U8string& eol)
                { init(file, flags, enc, eol); }
            FileReader(const NativeString& file, uint32_t flags, uint32_t enc, const U8string& eol)
                { init(file, flags, dec(enc), eol); }
        #else
            explicit FileReader(const U8string& file)
                { init(file, {}, {}, {}); }
            FileReader(const U8string& file, uint32_t flags)
                { init(file, flags, {}, {}); }
            FileReader(const U8string& file, uint32_t flags, const U8string& enc)
                { init(file, flags, enc, {}); }
            FileReader(const U8string& file, uint32_t flags, uint32_t enc)
                { init(file, flags, dec(enc), {}); }
            FileReader(const U8string& file, uint32_t flags, const U8string& enc, const U8string& eol)
                { init(file, flags, enc, eol); }
            FileReader(const U8string& file, uint32_t flags, uint32_t enc, const U8string& eol)
                { init(file, flags, dec(enc), eol); }
        #endif
        const U8string& operator*() const noexcept;
        FileReader& operator++();
        friend bool operator==(const FileReader& lhs, const FileReader& rhs) noexcept { return lhs.impl == rhs.impl; }
        size_t line() const noexcept;
    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl;
        void init(const NativeString& file, uint32_t flags, const U8string& enc, const U8string& eol);
        void fixline();
        void getline();
        void getmore(size_t n);
    };

    inline Irange<FileReader> read_lines(const U8string& file, uint32_t flags = 0)
        { return {FileReader{file, flags}, {}}; }
    inline Irange<FileReader> read_lines(const U8string& file, uint32_t flags, const U8string& enc)
        { return {{file, flags, enc}, {}}; }
    inline Irange<FileReader> read_lines(const U8string& file, uint32_t flags, uint32_t enc)
        { return {{file, flags, enc}, {}}; }
    inline Irange<FileReader> read_lines(const U8string& file, uint32_t flags, const U8string& enc, const U8string& eol)
        { return {{file, flags, enc, eol}, {}}; }
    inline Irange<FileReader> read_lines(const U8string& file, uint32_t flags, uint32_t enc, const U8string& eol)
        { return {{file, flags, enc, eol}, {}}; }

    #if defined(UNICORN_NATIVE_WCHAR)
        inline Irange<FileReader> read_lines(const NativeString& file, uint32_t flags = 0)
            { return {FileReader{file, flags}, {}}; }
        inline Irange<FileReader> read_lines(const NativeString& file, uint32_t flags, const U8string& enc)
            { return {{file, flags, enc}, {}}; }
        inline Irange<FileReader> read_lines(const NativeString& file, uint32_t flags, uint32_t enc)
            { return {{file, flags, enc}, {}}; }
        inline Irange<FileReader> read_lines(const NativeString& file, uint32_t flags, const U8string& enc, const U8string& eol)
            { return {{file, flags, enc, eol}, {}}; }
        inline Irange<FileReader> read_lines(const NativeString& file, uint32_t flags, uint32_t enc, const U8string& eol)
            { return {{file, flags, enc, eol}, {}}; }
    #endif

    // File output iterator

    class FileWriter:
    public OutputIterator<FileWriter> {
    public:
        FileWriter() {}
        #if defined(UNICORN_NATIVE_WCHAR)
            explicit FileWriter(const U8string& file)
                { init(to_wstring(file), {}, {}); }
            FileWriter(const U8string& file, uint32_t flags)
                { init(to_wstring(file), flags, {}); }
            FileWriter(const U8string& file, uint32_t flags, const U8string& enc)
                { init(to_wstring(file), flags, enc); }
            FileWriter(const U8string& file, uint32_t flags, uint32_t enc)
                { init(to_wstring(file), flags, dec(enc)); }
            explicit FileWriter(const NativeString& file)
                { init(file, {}, {}); }
            FileWriter(const NativeString& file, uint32_t flags)
                { init(file, flags, {}); }
            FileWriter(const NativeString& file, uint32_t flags, const U8string& enc)
                { init(file, flags, enc); }
            FileWriter(const NativeString& file, uint32_t flags, uint32_t enc)
                { init(file, flags, dec(enc)); }
        #else
            explicit FileWriter(const U8string& file)
                { init(file, {}, {}); }
            FileWriter(const U8string& file, uint32_t flags)
                { init(file, flags, {}); }
            FileWriter(const U8string& file, uint32_t flags, const U8string& enc)
                { init(file, flags, enc); }
            FileWriter(const U8string& file, uint32_t flags, uint32_t enc)
                { init(file, flags, dec(enc)); }
        #endif
        FileWriter& operator=(const U8string& str) { write(str); return *this; }
        FileWriter& operator=(const char* str) { write(cstr(str)); return *this; }
        void flush();
    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl;
        void init(const NativeString& file, uint32_t flags, const U8string& enc);
        void fix_text(U8string& str) const;
        void write(U8string str);
        void write_mbcs(const std::string& str);
    };

}
