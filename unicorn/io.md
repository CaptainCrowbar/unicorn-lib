Title: Unicorn Library: Input and Output
CSS: style.css

# [Unicorn Library](index.html): Input and Output #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/io.hpp"` ####

The functions and classes in this module provided line oriented input and
output, with automatic conversion between Unicode and external legacy
encodings, as well as other conveniences such as normalization of line breaks.

<p class="alert">[TODO: DOCUMENTATION]</p>

## Contents ##

* [Exceptions][]
* [Simple file I/O][]
* [File input iterator][]
* [File output iterator][]

## Exceptions ##

* `class IOError: public std::runtime_error`
    * `IOError::IOError()`
    * `explicit IOError::IOError(const char* msg)`
    * `template <typename C> IOError::IOError(const char* msg, const basic_string<C>& file, int error = 0)`
    * `const NativeCharacter* IOError::file() const noexcept`
    * `int IOError::error() const noexcept`
* `class ReadError: public IOError`
    * `ReadError::ReadError()`
    * `template <typename C> explicit ReadError::ReadError(const basic_string<C>& file, int error = 0)`
* `class WriteError: public IOError`
    * `WriteError::WriteError()`
    * `template <typename C> explicit WriteError::WriteError(const basic_string<C>& file, int error = 0)`

TODO

## Simple file I/O ##

* `template <typename C> void load_file(const basic_string<C>& file, string& dst, Crow::Flagset flags = {})`

Bitmask      | Letter  | Description
-------      | ------  | -----------
`io_stdin`   | `i`     | Default to stdin
`io_nofail`  | `f`     | Treat nonexistent file as empty

TODO

* `template <typename C> void save_file(const basic_string<C>& file, const string& src, Crow::Flagset flags = {})`
* `template <typename C> void save_file(const basic_string<C>& file, const void* ptr, size_t n, Crow::Flagset flags = {})`

Bitmask      | Letter  | Description
-------      | ------  | -----------
`io_stdout`  | `o`     | Default to stdout
`io_stderr`  | `e`     | Default to stderr
`io_append`  | `a`     | Append to file

TODO

## File input iterator ##

* `class FileReader`
    * `using FileReader::difference_type = ptrdiff_t`
    * `using FileReader::iterator_category = std::input_iterator_tag`
    * `using FileReader::pointer = const u8string*`
    * `using FileReader::reference = const u8string&`
    * `using FileReader::value_type = u8string`
    * `FileReader::FileReader()`
    * `template <typename C> explicit FileReader::FileReader(const basic_string<C>& file)`
    * `template <typename C> FileReader::FileReader(const basic_string<C>& file, Crow::Flagset flags)`
    * `template <typename C1, typename C2> FileReader::FileReader(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc)`
    * `template <typename C> FileReader::FileReader(const basic_string<C>& file, Crow::Flagset flags, uint32_t enc)`
    * `template <typename C1, typename C2, typename C3> FileReader::FileReader(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc, const basic_string<C3>& eol)`
    * `template <typename C1, typename C2> FileReader::FileReader(const basic_string<C1>& file, Crow::Flagset flags, uint32_t enc, const basic_string<C2>& eol)`
    * `size_t FileReader::line() const noexcept`
    * `[standard input iterator operations]`

TODO

Bitmask        | Letter  | Description
-------        | ------  | -----------
`io_stdin`     | `i`     | Default to stdin
`io_nofail`    | `f`     | Treat nonexistent file as empty
`io_bom`       | `B`     | Strip leading BOM
`io_lf`        | `n`     | Convert all line breaks to LF
`io_crlf`      | `c`     | Convert all line breaks to CR+LF
`io_striplf`   | `s`     | Strip line breaks
`io_striptws`  | `t`     | Strip trailing whitespace
`io_stripws`   | `S`     | Strip whitespace
`io_notempty`  | `z`     | Skip empty lines

(May be combined with err_replace/throw)

TODO

* `template <typename C> Crow::Irange<FileReader> read_lines(const basic_string<C>& file, Crow::Flagset flags = {})`
* `template <typename C1, typename C2> Crow::Irange<FileReader> read_lines(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc)`
* `template <typename C> Crow::Irange<FileReader> read_lines(const basic_string<C>& file, Crow::Flagset flags, uint32_t enc)`
* `template <typename C1, typename C2, typename C3> Crow::Irange<FileReader> read_lines(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc, const basic_string<C3>& eol)`
* `template <typename C1, typename C2> Crow::Irange<FileReader> read_lines(const basic_string<C1>& file, Crow::Flagset flags, uint32_t enc, const basic_string<C2>& eol)`

TODO

## File output iterator ##

* `class FileWriter`
    * `using FileWriter::difference_type = void`
    * `using FileWriter::iterator_category = std::output_iterator_tag`
    * `using FileWriter::pointer = void`
    * `using FileWriter::reference = void`
    * `using FileWriter::value_type = void`
    * `FileWriter::FileWriter()`
    * `template <typename C> explicit FileWriter::FileWriter(const basic_string<C>& file)`
    * `template <typename C> FileWriter::FileWriter(const basic_string<C>& file, Crow::Flagset flags)`
    * `template <typename C1, typename C2> FileWriter::FileWriter(const basic_string<C1>& file, Crow::Flagset flags, const basic_string<C2>& enc)`
    * `template <typename C> FileWriter::FileWriter(const basic_string<C>& file, Crow::Flagset flags, uint32_t enc)`
    * `void FileWriter::flush()`
    * `[standard output iterator operations]`

TODO

Bitmask         | Letter  | Description
-------         | ------  | -----------
`io_stdout`     | `o`     | Default to stdout
`io_stderr`     | `e`     | Default to stderr
`io_append`     | `a`     | Append to file
`io_bom`        | `B`     | Insert leading BOM if not already there
`io_lf`         | `n`     | Convert all line breaks to LF
`io_crlf`       | `c`     | Convert all line breaks to CR+LF
`io_writeline`  | `L`     | Write LF after every write
`io_autoline`   | `A`     | Write LF if not already there
`io_linebuf`    | `l`     | Line buffered output
`io_unbuf`      | `u`     | Unbuffered output
`io_mutex`      | `m`     | Hold per-file mutex while writing

(May be combined with err_replace/throw)

TODO
