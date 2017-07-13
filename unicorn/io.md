# [Unicorn Library](index.html): Input and Output #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/io.hpp"`

The functions and classes in this module provided line oriented input and
output, with automatic conversion between Unicode and external legacy
encodings, as well as other conveniences such as normalization of line breaks.

File names can be supplied either as UTF-8 strings or as native character
strings (these are different types only on Windows; on Unix the `U8string` and
`NativeString` versions are the same function). Constructors that take a
`NativeString` can be used with non-Unicode file names; see
[`unicorn/file`](file.html) for the details of how file name encodings are
handled.

## Contents ##

[TOC]

## File input iterator ##

* `class` **`FileReader`**
    * `using FileReader::`**`difference_type`** `= ptrdiff_t`
    * `using FileReader::`**`iterator_category`** `= std::input_iterator_tag`
    * `using FileReader::`**`pointer`** `= const U8string*`
    * `using FileReader::`**`reference`** `= const U8string&`
    * `using FileReader::`**`value_type`** `= U8string`
    * `FileReader::`**`FileReader`**`()`
    * `explicit FileReader::`**`FileReader`**`(const U8string& file)`
    * `FileReader::`**`FileReader`**`(const U8string& file, uint32_t flags)`
    * `FileReader::`**`FileReader`**`(const U8string& file, uint32_t flags, const U8string& enc)`
    * `FileReader::`**`FileReader`**`(const U8string& file, uint32_t flags, uint32_t enc)`
    * `FileReader::`**`FileReader`**`(const U8string& file, uint32_t flags, const U8string& enc, const U8string& eol)`
    * `FileReader::`**`FileReader`**`(const U8string& file, uint32_t flags, uint32_t enc, const U8string& eol)`
    * `explicit FileReader::`**`FileReader`**`(const NativeString& file)`
    * `FileReader::`**`FileReader`**`(const NativeString& file, uint32_t flags)`
    * `FileReader::`**`FileReader`**`(const NativeString& file, uint32_t flags, const U8string& enc)`
    * `FileReader::`**`FileReader`**`(const NativeString& file, uint32_t flags, uint32_t enc)`
    * `FileReader::`**`FileReader`**`(const NativeString& file, uint32_t flags, const U8string& enc, const U8string& eol)`
    * `FileReader::`**`FileReader`**`(const NativeString& file, uint32_t flags, uint32_t enc, const U8string& eol)`
    * `size_t FileReader::`**`line`**`() const noexcept`
    * _[standard input iterator operations]_

An iterator over the lines in a text file. You should normally obtain a pair
of these by calling one of the `read_lines()` functions described below,
rather than constructing the iterator type explicitly.

The constructors open the file for input. The `flags` argument contains a
combination of flags controlling the  iterator's behaviour (described below).
The `enc` argument is an optional encoding name or number, indicating what
encoding is expected to be found in the file (see [`unicorn/mbcs`](mbcs.html)
for the details of how these work); if no encoding is supplied, it will assume
UTF-8. The `eol` argument optionally supplies an end-of-line marker; if no EOL
marker is supplied by the user, any of `CR`, `LF`, `FF`, or `CR+LF` will be
recognised as a line delimiter.

The constructor will throw `std::invalid_argument` if an inconsistent
combination of flags is supplied, `UnknownEncoding` if the encoding is not
recognised, or `std::system_error` if anything goes wrong while opening the
file. If the `Utf::throws` option was selected, the constructor or increment
operator may throw `EncodingError` if the file contains invalid text.

A dereferenced iterator yields one line of text from the file, optionally
modified by the flags below.

Flag                  | Description
----                  | -----------
`IO::`**`stdin`**     | Read from standard input if the file name is `"-"` or an empty string
`IO::`**`pretend`**   | Treat a nonexistent file as empty instead of throwing an exception
`IO::`**`bom`**       | Strip a leading byte order mark if one is found
`IO::`**`lf`**        | Convert all line breaks to `LF`
`IO::`**`crlf`**      | Convert all line breaks to `CR+LF`
`IO::`**`striplf`**   | Strip trailing line breaks from each line
`IO::`**`striptws`**  | Strip trailing whitespace from each line (implies `striplf`)
`IO::`**`stripws`**   | Strip leading and trailing whitespace from each line (implies `striplf`)
`IO::`**`notempty`**  | Skip empty lines (after any whitespace stripping)
`Utf::`**`replace`**  | Replace invalid encoding with `U+FFFD` (default)
`Utf::`**`throws`**   | Throw `EncodingError` if invalid encoding is encountered

* `Irange<FileReader>` **`read_lines`**`(const U8string& file, uint32_t flags = 0)`
* `Irange<FileReader>` **`read_lines`**`(const U8string& file, uint32_t flags, const U8string& enc)`
* `Irange<FileReader>` **`read_lines`**`(const U8string& file, uint32_t flags, uint32_t enc)`
* `Irange<FileReader>` **`read_lines`**`(const U8string& file, uint32_t flags, const U8string& enc, const U8string& eol)`
* `Irange<FileReader>` **`read_lines`**`(const U8string& file, uint32_t flags, uint32_t enc, const U8string& eol)`
* `Irange<FileReader>` **`read_lines`**`(const NativeString& file, uint32_t flags = 0)`
* `Irange<FileReader>` **`read_lines`**`(const NativeString& file, uint32_t flags, const U8string& enc)`
* `Irange<FileReader>` **`read_lines`**`(const NativeString& file, uint32_t flags, uint32_t enc)`
* `Irange<FileReader>` **`read_lines`**`(const NativeString& file, uint32_t flags, const U8string& enc, const U8string& eol)`
* `Irange<FileReader>` **`read_lines`**`(const NativeString& file, uint32_t flags, uint32_t enc, const U8string& eol)`

These construct a pair of iterators, from which the lines in a file can be
read. The arguments are interpreted as described above.

## File output iterator ##

* `class` **`FileWriter`**
    * `using FileWriter::`**`difference_type`** `= void`
    * `using FileWriter::`**`iterator_category`** `= std::output_iterator_tag`
    * `using FileWriter::`**`pointer`** `= void`
    * `using FileWriter::`**`reference`** `= void`
    * `using FileWriter::`**`value_type`** `= void`
    * `FileWriter::`**`FileWriter`**`()`
    * `explicit FileWriter::`**`FileWriter`**`(const U8string& file)`
    * `FileWriter::`**`FileWriter`**`(const U8string& file, uint32_t flags)`
    * `FileWriter::`**`FileWriter`**`(const U8string& file, uint32_t flags, const U8string& enc)`
    * `FileWriter::`**`FileWriter`**`(const U8string& file, uint32_t flags, uint32_t enc)`
    * `explicit FileWriter::`**`FileWriter`**`(const NativeString& file)`
    * `FileWriter::`**`FileWriter`**`(const NativeString& file, uint32_t flags)`
    * `FileWriter::`**`FileWriter`**`(const NativeString& file, uint32_t flags, const U8string& enc)`
    * `FileWriter::`**`FileWriter`**`(const NativeString& file, uint32_t flags, uint32_t enc)`
    * `void FileWriter::`**`flush`**`()`
    * _[standard output iterator operations]_

An output iterator that can be used to write to a file. The constructor
arguments have the same meaning as for `FileReader` above, except that the
`flags` argument is interpreted as described below, and will throw exceptions
under the same circumstances.

The default behaviour, if none of the flags below are used, is to simply write
any string assigned to the iterator into the file, converted if necessary to
the encoding specified in the constructor (UTF-8 by default). Text need not be
written one line at a time; a single output string can contain multiple lines.

See the [`unicorn/mbcs`](mbcs.html) documentation for the behaviour of the
error handling flags on output.

Flag                   | Description
----                   | -----------
`IO::`**`standout`**   | Write to standard output if the file name is `"-"` or an empty string
`IO::`**`standerr`**   | Write to standard error if the file name is `"-"` or an empty string
`IO::`**`append`**     | Open an existing file for appending instead of overwriting it
`IO::`**`protect`**    | Throw `system_error(file_exists)` if the file already exists
`IO::`**`bom`**        | Insert a leading byte order mark if the first output string does not contain one
`IO::`**`lf`**         | Convert all line breaks to `LF`
`IO::`**`crlf`**       | Convert all line breaks to `CR+LF`
`IO::`**`writeline`**  | Write a line feed after every write
`IO::`**`autoline`**   | Write a line feed after every write that does not already end with one
`IO::`**`linebuf`**    | Line buffered output
`IO::`**`unbuf`**      | Unbuffered output
`IO::`**`mutex`**      | Hold a per-file mutex while writing, to allow coherent multithreaded output
`Utf::`**`replace`**   | Replace encoding errors (default)
`Utf::`**`throws`**    | Throw `EncodingError` on encoding errors
