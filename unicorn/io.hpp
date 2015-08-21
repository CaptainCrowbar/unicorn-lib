#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/file.hpp"
#include "unicorn/utf.hpp"
#include <cstdio>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

namespace Unicorn {

    // Exceptions

    class IOError:
    public std::runtime_error {
    public:
        IOError(): std::runtime_error(assemble()), name(), err(0) {}
        explicit IOError(const char* msg):
            std::runtime_error(assemble(msg)), name(), err(0) {}
        template <typename C> IOError(const char* msg, const basic_string<C>& file, int error = 0):
            std::runtime_error(assemble(msg, to_utf8(file), error)),
            name(std::make_shared<NativeString>()), err(error)
            { recode(file, *name); }
        const NativeCharacter* file() const noexcept
            { static const NativeCharacter c = 0; return name ? name->data() : &c; }
        int error() const noexcept { return err; }
    private:
        std::shared_ptr<NativeString> name;
        int err;
        static u8string assemble(const char* msg = nullptr, const u8string& file = {}, int error = 0);
    };

    class ReadError:
    public IOError {
    public:
        ReadError(): IOError("Read error") {}
        template <typename C>
            explicit ReadError(const basic_string<C>& file, int error = 0):
            IOError("Read error", file, error) {}
    };

    class WriteError:
    public IOError {
    public:
        WriteError(): IOError("Write error") {}
        template <typename C>
            explicit WriteError(const basic_string<C>& file, int error = 0):
            IOError("Write error", file, error) {}
    };

    // I/O flags

    constexpr auto io_bom        = Crow::Flagset::value('B');  // Strip or insert a BOM              FileReader  FileWriter
    constexpr auto io_lf         = Crow::Flagset::value('n');  // Convert all line breaks to LF      FileReader  FileWriter
    constexpr auto io_crlf       = Crow::Flagset::value('c');  // Convert all line breaks to CR+LF   FileReader  FileWriter
    constexpr auto io_stdin      = Crow::Flagset::value('i');  // Default to stdin                   FileReader  --
    constexpr auto io_nofail     = Crow::Flagset::value('f');  // Treat nonexistent file as empty    FileReader  --
    constexpr auto io_striplf    = Crow::Flagset::value('s');  // Strip line breaks                  FileReader  --
    constexpr auto io_striptws   = Crow::Flagset::value('t');  // Strip trailing whitespace          FileReader  --
    constexpr auto io_stripws    = Crow::Flagset::value('S');  // Strip whitespace                   FileReader  --
    constexpr auto io_notempty   = Crow::Flagset::value('z');  // Skip empty lines                   FileReader  --
    constexpr auto io_stdout     = Crow::Flagset::value('o');  // Default to stdout                  --          FileWriter
    constexpr auto io_stderr     = Crow::Flagset::value('e');  // Default to stderr                  --          FileWriter
    constexpr auto io_append     = Crow::Flagset::value('a');  // Append to file                     --          FileWriter
    constexpr auto io_linebuf    = Crow::Flagset::value('l');  // Line buffered output               --          FileWriter
    constexpr auto io_unbuf      = Crow::Flagset::value('u');  // Unbuffered output                  --          FileWriter
    constexpr auto io_writeline  = Crow::Flagset::value('L');  // Write LF after every write         --          FileWriter
    constexpr auto io_autoline   = Crow::Flagset::value('A');  // Write LF if not already there      --          FileWriter
    constexpr auto io_mutex      = Crow::Flagset::value('m');  // Hold per-file mutex while writing  --          FileWriter

    // File input iterator

    class FileReader:
    public Crow::InputIterator<FileReader, u8string> {
    public:
        FileReader() {}
        template <typename C>
            explicit FileReader(const basic_string<C>& file)
            { init(recode_filename<NativeCharacter>(file), {}, {}, {}); }
        template <typename C>
            FileReader(const basic_string<C>& file, Crow::Flagset flags)
            { init(recode_filename<NativeCharacter>(file), flags, {}, {}); }
        template <typename C1, typename C2>
            FileReader(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc)
            { init(recode_filename<NativeCharacter>(file), flags, to_utf8(enc), {}); }
        template <typename C>
            FileReader(const basic_string<C>& file, Crow::Flagset flags, uint32_t enc)
            { init(recode_filename<NativeCharacter>(file), flags, Crow::dec(enc), {}); }
        template <typename C1, typename C2, typename C3>
            FileReader(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc,
                const basic_string<C3>& eol)
            { init(recode_filename<NativeCharacter>(file), flags, to_utf8(enc), to_utf8(eol)); }
        template <typename C1, typename C2>
            FileReader(const basic_string<C1>& file, Crow::Flagset flags, uint32_t enc,
                const basic_string<C2>& eol)
            { init(recode_filename<NativeCharacter>(file), flags, Crow::dec(enc), to_utf8(eol)); }
        const u8string& operator*() const noexcept;
        FileReader& operator++();
        friend bool operator==(const FileReader& lhs, const FileReader& rhs) noexcept { return lhs.impl == rhs.impl; }
        size_t line() const noexcept;
    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl;
        void init(const NativeString& file, Crow::Flagset flags, const u8string& enc, const u8string& eol);
        void fixline();
        void getline();
        void getmore(size_t n);
    };

    template <typename C>
        Crow::Irange<FileReader> read_lines(const basic_string<C>& file, Crow::Flagset flags = {})
        { return {FileReader{file, flags}, {}}; }
    template <typename C1, typename C2>
        Crow::Irange<FileReader> read_lines(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc)
        { return {{file, flags, enc}, {}}; }
    template <typename C>
        Crow::Irange<FileReader> read_lines(const basic_string<C>& file, Crow::Flagset flags, uint32_t enc)
        { return {{file, flags, enc}, {}}; }
    template <typename C1, typename C2, typename C3>
        Crow::Irange<FileReader> read_lines(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc,
            const basic_string<C3>& eol)
        { return {{file, flags, enc, eol}, {}}; }
    template <typename C1, typename C2>
        Crow::Irange<FileReader> read_lines(const basic_string<C1>& file, Crow::Flagset flags, uint32_t enc,
            const basic_string<C2>& eol)
        { return {{file, flags, enc, eol}, {}}; }

    // File output iterator

    class FileWriter:
    public Crow::OutputIterator<FileWriter> {
    public:
        FileWriter() {}
        template <typename C>
            explicit FileWriter(const basic_string<C>& file)
            { init(recode_filename<NativeCharacter>(file), {}, {}); }
        template <typename C>
            FileWriter(const basic_string<C>& file, Crow::Flagset flags)
            { init(recode_filename<NativeCharacter>(file), flags, {}); }
        template <typename C1, typename C2>
            FileWriter(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc)
            { init(recode_filename<NativeCharacter>(file), flags, to_utf8(enc)); }
        template <typename C>
            FileWriter(const basic_string<C>& file, Crow::Flagset flags, uint32_t enc)
            { init(recode_filename<NativeCharacter>(file), flags, Crow::dec(enc)); }
        template <typename C> FileWriter& operator=(const basic_string<C>& str)
            { write(to_utf8(str)); return *this; }
        template <typename C> FileWriter& operator=(const C* str)
            { write(to_utf8(Crow::cstr(str))); return *this; }
        void flush();
    private:
        struct impl_type;
        std::shared_ptr<impl_type> impl;
        void init(const NativeString& file, Crow::Flagset flags, const u8string& enc);
        void fixtext(u8string& str) const;
        void write(u8string str);
        void writembcs(const string& str);
    };

}
