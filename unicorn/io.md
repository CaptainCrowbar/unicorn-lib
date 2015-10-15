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

* [File input iterator][]
* [File output iterator][]

## File input iterator ##

* `class FileReader`
    * `using FileReader::difference_type = ptrdiff_t`
    * `using FileReader::iterator_category = std::input_iterator_tag`
    * `using FileReader::pointer = const u8string*`
    * `using FileReader::reference = const u8string&`
    * `using FileReader::value_type = u8string`
    * `FileReader::FileReader()`
    * `template <typename C> explicit FileReader::FileReader(const basic_string<C>& file)`
    * `template <typename C> FileReader::FileReader(const basic_string<C>& file, uint32_t flags)`
    * `template <typename C1, typename C2> FileReader::FileReader(const basic_string<C1>& file, uint32_t flags, const basic_string<C2>& enc)`
    * `template <typename C> FileReader::FileReader(const basic_string<C>& file, uint32_t flags, uint32_t enc)`
    * `template <typename C1, typename C2, typename C3> FileReader::FileReader(const basic_string<C1>& file, uint32_t flags, const basic_string<C2>& enc, const basic_string<C3>& eol)`
    * `template <typename C1, typename C2> FileReader::FileReader(const basic_string<C1>& file, uint32_t flags, uint32_t enc, const basic_string<C2>& eol)`
    * `size_t FileReader::line() const noexcept`
    * `[standard input iterator operations]`

TODO

Flag           | Description
----           | -----------
`io_stdin`     | Default to stdin
`io_nofail`    | Treat nonexistent file as empty
`io_bom`       | Strip leading BOM
`io_lf`        | Convert all line breaks to LF
`io_crlf`      | Convert all line breaks to CR+LF
`io_striplf`   | Strip line breaks
`io_striptws`  | Strip trailing whitespace
`io_stripws`   | Strip whitespace
`io_notempty`  | Skip empty lines

(May be combined with err_replace/throw)

TODO

* `template <typename C> Irange<FileReader> read_lines(const basic_string<C>& file, uint32_t flags = 0)`
* `template <typename C1, typename C2> Irange<FileReader> read_lines(const basic_string<C1>& file, uint32_t flags, const basic_string<C2>& enc)`
* `template <typename C> Irange<FileReader> read_lines(const basic_string<C>& file, uint32_t flags, uint32_t enc)`
* `template <typename C1, typename C2, typename C3> Irange<FileReader> read_lines(const basic_string<C1>& file, uint32_t flags, const basic_string<C2>& enc, const basic_string<C3>& eol)`
* `template <typename C1, typename C2> Irange<FileReader> read_lines(const basic_string<C1>& file, uint32_t flags, uint32_t enc, const basic_string<C2>& eol)`

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
    * `template <typename C> FileWriter::FileWriter(const basic_string<C>& file, uint32_t flags)`
    * `template <typename C1, typename C2> FileWriter::FileWriter(const basic_string<C1>& file, uint32_t flags, const basic_string<C2>& enc)`
    * `template <typename C> FileWriter::FileWriter(const basic_string<C>& file, uint32_t flags, uint32_t enc)`
    * `void FileWriter::flush()`
    * `[standard output iterator operations]`

TODO

Flag            | Description
----            | -----------
`io_stdout`     | Default to stdout
`io_stderr`     | Default to stderr
`io_append`     | Append to file
`io_bom`        | Insert leading BOM if not already there
`io_lf`         | Convert all line breaks to LF
`io_crlf`       | Convert all line breaks to CR+LF
`io_writeline`  | Write LF after every write
`io_autoline`   | Write LF if not already there
`io_linebuf`    | Line buffered output
`io_unbuf`      | Unbuffered output
`io_mutex`      | Hold per-file mutex while writing

(May be combined with err_replace/throw)

TODO
