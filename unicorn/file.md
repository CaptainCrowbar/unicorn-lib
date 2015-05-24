Title: Unicorn Library: File System
CSS: style.css

# [Unicorn Library](index.html): File System #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/file.hpp"` ####

This module provides a very minimal set of file system operations, such as
file renaming and deletion, directory search, and so on.

## Contents ##

* [Introduction][]
* [System dependencies][]
* [Exceptions][]
* [File name operations][]
* [File properties][]
* [File system operations][]
* [Directory iterators][]

## Introduction ##

File handling in Unicode always has the problem that the actual file names on
most systems are not guaranteed to be valid Unicode. On most Unix systems, a
file name is an arbitrary sequence of bytes, with no restrictions other than
excluding the null and slash characters; while file names on systems such as
Linux are usually expected to be encoded in UTF-8 when they contain non-ASCII
characters, this is not enforced in any way. On Microsoft Windows, using the
NTFS file system, file names are nominally UTF-16 (with a short list of
disallowed characters), but not all of the Win32 wide character API functions
that deal with files check for valid encoding; it's not hard to create a file
whose name is an arbitrary sequence of 16-bit integers. The HFS+ file system
used by Apple on Mac OS X and iOS appears to be the only widely used file
system that actually does enforce valid Unicode names at the file system
level.

To make it possible to deal with this situation, this module uses the
`NativeString` type from the [`unicorn/character`](character.html) module,
which is `string` on Unix systems and `wstring` on Windows. Like most of the
rest of the Unicorn library, all of the functions and classes in this module
take a character type as a template argument. Functions that match the native
filename type can operate on, and may return, file names with invalid
encoding; the other versions of each function will convert invalid Unicode
names using the usual character replacement rule, which may result in
unexpected behaviour if there are any files on your system with non-Unicode
names. (On Windows, although `wstring` is used as the native filename type,
`u16string` names will also be passed through without sanitization.)

In short, stick to the `NativeString` versions of the functions here if you
need reliable handling of all filenames, including those containing invalid
Unicode; use other encodings only if you expect all the files you deal with to
have valid Unicode names, and you don't mind if non-Unicode names are slightly
mangled, or if you're only targeting Mac/iOS and don't need to worry about
other operating systems.

The examples in the documentation below mostly use Unix-style file names for
simplicity; the equivalent code on Windows will make the obvious substitutions
of backslashes for slashes, and drive roots such as `"C:\"` for `"/"`.

## System dependencies ##

* `constexpr char file_delimiter = ['/' on Unix, '\\' on Windows]`

The standard delimiter for directory paths.

* `template <typename C1, typename C2> void recode_filename(const basic_string<C1>& src, basic_string<C2>& dst)`
* `template <typename C2, typename C1> basic_string<C2> recode_filename(const basic_string<C1>& src)`

These convert a file name from one UTF encoding to another, using the
`recode()` functions from [`unicorn/utf`](utf.html) with the `err_replace`
option, except that, if both the source and destination strings are the
operating system's native encoding, the string will simply be copied verbatim
without checking for valid Unicode.

## Exceptions ##

* `class FileError: public std::runtime_error`
    * `FileError::FileError()`
    * `explicit FileError::FileError(int error)`
    * `template <typename... More> FileError::FileError(int error, const NativeString& file, const More&... more)`
    * `NativeString FileError::file() const`
    * `std::vector<NativeString> FileError::files() const`
    * `int FileError::error() const noexcept`

An exception thrown to report errors in file system operations. It can be
constructed with the error code from the underlying native system call, and
optionally one or more file names for reference. The `file()` method returns
the first file in the list, or an empty string if no files were supplied.

## File name operations ##

These functions operate purely on file name strings; they do not make any
contact with the actual file system, and will give the same results regardless
of whether or not a file actually exists. Where relevant, these functions are
aware of the standard double slash convention for network paths <span
class="nobr">(`"//server/path..."`),</span> and the Windows versions are aware
of UNC paths <span class="nobr">(`"\\?\path..."`).</span>

* `template <typename C> bool file_is_absolute(const basic_string<C>& file)`
* `template <typename C> bool file_is_relative(const basic_string<C>& file)`
* `template <typename C> bool file_is_drive_absolute(const basic_string<C>& file)`
* `template <typename C> bool file_is_drive_relative(const basic_string<C>& file)`

These indicate whether a file name is absolute or relative; exactly one of
them will be true for any file name (an empty string is considered to be a
relative name).

On Windows, two extra functions identify "drive relative" paths that are
relative to the current directory on a specific drive <span
class="nobr">(`"C:path..."`),</span> and "drive absolute" paths that are
absolute with respect to an unspecified current drive <span
class="nobr">(`"\path..."`).</span> Both of these functions are always false
on Unix.

* `template <typename C> bool file_is_root(const basic_string<C>& file)`

True if the file name refers to the root of a directory tree.

* `template <typename C, typename... Args> basic_string<C> file_path(const basic_string<C>& file, Args... args)`
* `template <typename C, typename... Args> basic_string<C> file_path(const C* file, Args... args)`

Assembles a directory path from a series of path elements or relative paths.
If any argument is an absolute path, this is taken as the root of the final
path, ignoring any preceding arguments. (Drive relative and drive absolute
paths on Windows are treated as absolute in this context.)

Examples:

    file_path("foo", "bar", "hello.txt") == "foo/bar/hello.txt"
    file_path("/foo", "/bar", "hello.txt") == "/bar/hello.txt"

* `template <typename C> std::pair<basic_string<C>, basic_string<C>> split_path(const basic_string<C>& file)`
* `template <typename C> std::pair<basic_string<C>, basic_string<C>> split_file(const basic_string<C>& file)`

These functions break down a file name into its constituent parts. The
`split_path()` function breaks the full name into the parent (directory path)
and leaf name (the local file name within the directory); for example, if the
original file name is `"/foo/bar/hello.txt"`, the parent is `"/foo/bar"` and
the leaf name is `"hello.txt"`. If the file name refers to the file system
root (e.g. `"/"`), the parent is the full file name and the leaf name is
empty. The delimiter between the parent and leaf names is discarded, unless
the parent is a root name that requires the delimiter suffix for correct
identification.

The `split_file()` function breaks the leaf name into a base and extension
(discarding the directory path); for example, if the original file name is
`"foo/bar/hello.txt"`, the base name is `"hello"` and the extension is
`".txt"`. The extension starts with the last dot in the leaf name, excluding
its first character; if the name does not contain a dot, or if the only dot is
at the beginning, the base name is the full leaf name and the extension is
empty.

Examples:

    split_path("/foo/bar/hello.txt") == {"/foo/bar", "hello.txt"}
    split_file("/foo/bar/hello.txt") == {"hello", ".txt"}
    split_file(".hello") == {".hello", ""}

## File properties ##

These functions perform operations that query the file system.

* `template <typename C> basic_string<C> current_directory()`
* `template <typename C> void current_directory(basic_string<C>& dir)`

These query the current working directory of the calling process. The first
version requires the character type to be explicitly specified. These may
throw `FileError` in some unusual cases of failure, usually involving access
permission problems.

* `template <typename C> bool file_exists(const basic_string<C>& file) noexcept`

Query whether a file exists. This may give a false negative if the file exists
but is not accessible to the calling process.

* `template <typename C> bool file_is_directory(const basic_string<C>& file) noexcept`

Query whether a file is a directory. This will return `false` if the file does
not exist; it may give a false negative if the directory exists but is not
accessible to the calling process.

* `template <typename C> bool file_is_hidden(const basic_string<C>& file) noexcept`

True if the file is normally hidden. This will return `false` if the file does
not exist or is not accessible to the calling process. On Unix this is based
on the file name (a file is hidden if its name starts with a dot), but on
Windows this is a metadata property.

* `template <typename C> bool file_is_symlink(const basic_string<C>& file) noexcept`

True if the file is a symbolic link.

## File system operations ##

These functions perform operations that modify the file system.

* `template <typename C> void make_directory(const basic_string<C>& dir, bool recurse = false)`

Create a directory (with default permissions). If the `recurse` flag is set,
this will recursively create any missing parent directories (like `mkdir -p`).
It will do nothing if the directory already exists. It will throw `FileError`
if the named file already exists but is not a directory, if the directory path
is illegal, if the parent directory does not exist and the `recurse` flag was
not set, or if the caller does not have permission to create the directory.

* `template <typename C> void remove_file(const basic_string<C>& file, bool recurse = false)`

Delete a file or directory. If the `recurse` flag is set, directories will be
deleted recursively (like `rm -rf`; this will not follow symbolic links). This
will do nothing if the named file does not exist to begin with. It will throw
`FileError` if the directory path is illegal, if the name refers to a nonempty
directory and the `recurse` flag was not set, or if the caller does not have
permission to remove the file.

* `template <typename C> void rename_file(const basic_string<C>& src, const basic_string<C>& dst)`

Rename a file or directory. This will throw `FileError` if either path is
illegal, or if the caller does not have permission to perform the operation.
This function is a thin wrapper around the underlying native file renaming
functions, and will share any system specific limitations and variations;
behaviour when the destination file already exists is system dependent (this
may overwrite the file or throw an exception), and on most systems the call
will fail if the source and destination are on different file systems.

## Directory iterators ##

* `template <typename C> class DirectoryIterator`
    * `using DirectoryIterator::difference_type = ptrdiff_t`
    * `using DirectoryIterator::iterator_category = std::input_iterator_tag`
    * `using DirectoryIterator::value_type = basic_string<C>`
    * `using DirectoryIterator::pointer = const value_type*`
    * `using DirectoryIterator::reference = const value_type&`
    * `DirectoryIterator::DirectoryIterator()`
    * `explicit DirectoryIterator::DirectoryIterator(const value_type& dir, Flagset flags = {})`
    * `[standard iterator operations]`
* `template <typename C> Irange<DirectoryIterator<C>> directory(const basic_string<C>& dir, Flagset flags = {})`

Bitmask         | Letter  | Description
-------         | ------  | -----------
`dir_dotdot`    | `d`     | Include . and .. entries
`dir_fullname`  | `f`     | Return full file names instead of leaf names
`dir_hidden`    | `h`     | Include hidden files

An iterator over the files in a directory. Normally you would call
`directory()` to get a range rather than construct a `DirectoryIterator`
directly. If the name passed to the constructor, or to `directory()`, refers
to a file that does not exist or is not a directory, it will simply be treated
as an empty directory.

By default, an iterator dereferences to a file's leaf name (its local name
within the directory); if the `dir_fullname` option is used, the full path is
reported instead, including the directory (this is based on the name passed to
the constructor, and will not be a fully qualified absolute path if the
original name was not).

The order in which files are returned is unspecified; do not rely on them
being returned in the same order as the `ls` or `dir` command, or on the order
remaining the same on separate iterations over the same directory. Behaviour
is also unspecified if a file is created or deleted while a
`DirectoryIterator` is scanning its parent directory; the change may or may
not be visible to the caller.
