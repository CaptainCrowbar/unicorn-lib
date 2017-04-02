# [Unicorn Library](index.html): File System #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/file.hpp"`

This module provides a very minimal set of file system operations, such as
file renaming and deletion, directory search, and so on.

## Contents ##

[TOC]

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
used by Apple on macOS and iOS appears to be the only widely used file system
that actually does enforce valid Unicode names at the file system level,
although it then proceeds to complicate matters by using a proprietary
normalization scheme that does not match any of the four standard ones (and
even on a Mac you need to be careful in the presence of remote file systems
that may be on a different OS).

To make it possible to deal with this situation, this module uses the
`NativeString` type from the [`unicorn/core`](core.html) module, which is
`string` on Unix systems and `wstring` on Windows. Most of the functions in
this module come in `U8string` and `NativeString` versions; on Windows these
are separate overloads, while on Unix `U8string` and `NativeString` are really
the same type (`std::string`), and only one version of each function is
needed.

Functions that take a `NativeString` can operate on, and may return, file
names with invalid encoding. If the `U8string` function is a separate
overload, it will convert invalid Unicode names using the usual character
replacement rule, which may result in unexpected behaviour if there are any
files on your system with non-Unicode names.

In short, use the `NativeString` versions of the functions here if you need
reliable handling of all filenames, including those containing invalid
Unicode; use the `U8string` versions if you expect all the files you deal with
to have valid Unicode names, and you don't mind if non-Unicode names are
slightly mangled, or if you're only targeting Mac/iOS and don't need to worry
about other operating systems.

The examples in the documentation below mostly use Unix-style file names for
simplicity; the equivalent code on Windows will make the obvious substitutions
of backslashes for slashes, and drive roots such as `"C:\"` for `"/"`.

The term "leaf name" is used here to mean the name of an individual file
within a directory, with no directory path prefix (for example, the leaf name
of `"/foo/bar/hello.txt"` is `"hello.txt"`).

## Constants ##

Flag                | Description
----                | -----------
**`fs_dotdot`**     | Include . and ..
**`fs_follow`**     | Follow symlinks
**`fs_fullname`**   | Return full file names
**`fs_hidden`**     | Include hidden files
**`fs_overwrite`**  | Delete existing file if necessary
**`fs_recurse`**    | Recursive directory operations
**`fs_unicode`**    | Skip files with non-Unicode names

Flags recognised by some of the functions in this module.

## Types ##

* `typename` **`FileId`**

The file identifier returned by the `file_id()` function. On some systems this
will be an integer type; on others it will be a class type. It will support
all comparison operators, I/O using `<<` and `>>` operators, and `std::hash`.

## System dependencies ##

* `constexpr char` **`file_delimiter`** `= ['/' on Unix, '\\' on Windows]`
* `constexpr NativeCharacter` **`native_file_delimiter`** `= ['/' on Unix, L'\\' on Windows]`

The standard delimiter for directory paths.

## File name functions ##

These functions operate purely on file names as strings; they do not make any
contact with the actual file system, and will give the same results regardless
of whether or not a file actually exists, or the user has permission to access
it.

The `file_is_*()` functions do not test the complete file name for legality;
if a file name would fail `is_legal_path_name()`, the results of the
`file_is_*()` functions is unspecified, except as noted below for the
individual functions.

Where relevant, these functions are aware of the standard double slash
convention for network paths <span class="nobr">(`"//server/path..."`),</span>
and the Windows versions are aware of UNC paths <span
class="nobr">(`"\\?\path..."`).</span>

* `bool` **`file_is_absolute`**`(const U8string& file)`
* `bool` **`file_is_absolute`**`(const NativeString& file)`
* `bool` **`file_is_relative`**`(const U8string& file)`
* `bool` **`file_is_relative`**`(const NativeString& file)`
* `bool` **`file_is_drive_absolute`**`(const U8string& file)`
* `bool` **`file_is_drive_absolute`**`(const NativeString& file)`
* `bool` **`file_is_drive_relative`**`(const U8string& file)`
* `bool` **`file_is_drive_relative`**`(const NativeString& file)`

These indicate whether a file name is absolute or relative; exactly one of
them will be true for any non-empty file name (including illegal ones).

On Windows, two extra functions identify "drive relative" paths that are
relative to the current directory on a specific drive <span
class="nobr">(`"C:path..."`),</span> and "drive absolute" paths that are
absolute with respect to an unspecified current drive <span
class="nobr">(`"\path..."`).</span> Both of these functions are always false
on Unix.

* `bool` **`file_is_root`**`(const U8string& file)`
* `bool` **`file_is_root`**`(const NativeString& file)`

True if the file name refers to the root of a directory tree (note that this
is a purely syntactic operation on the file name, and is not the same as
identifying the root of a physical file system).

* `template <typename... Args> U8string` **`file_path`**`(const U8string& file, Args... args)`
* `template <typename... Args> NativeString` **`file_path`**`(const NativeString& file, Args... args)`

Assembles a directory path from a series of path elements or relative paths.
If any argument is an absolute path, this is taken as the root of the final
path, ignoring any preceding arguments. (Drive relative and drive absolute
paths on Windows are treated as absolute in this context.)

Examples:

    file_path("foo", "bar", "hello.txt") == "foo/bar/hello.txt"
    file_path("/foo", "/bar", "hello.txt") == "/bar/hello.txt"

* `bool` **`is_legal_leaf_name`**`(const U8string& file)`
* `bool` **`is_legal_leaf_name`**`(const NativeString& file)`
* `bool` **`is_legal_path_name`**`(const U8string& file)`
* `bool` **`is_legal_path_name`**`(const NativeString& file)`
* `bool` **`is_legal_mac_leaf_name`**`(const U8string& file)`
* `bool` **`is_legal_mac_path_name`**`(const U8string& file)`
* `bool` **`is_legal_posix_leaf_name`**`(const U8string& file)`
* `bool` **`is_legal_posix_path_name`**`(const U8string& file)`
* `bool` **`is_legal_windows_leaf_name`**`(const U8string& file)`
* `bool` **`is_legal_windows_leaf_name`**`(const wstring& file)`
* `bool` **`is_legal_windows_path_name`**`(const U8string& file)`
* `bool` **`is_legal_windows_path_name`**`(const wstring& file)`

These indicate whether a string is a legal file name. The first two functions
call the corresponding system specific function for the host operating system.

The `*_leaf_name()` functions test for a valid path component, and will always
return false for any name that contains a slash (or backslash on Windows); the
`*_path_name()` functions test for a valid relative or absolute qualified file
name. Any name that passes `*_leaf_name()` will also pass the corresponding
`*_path_name()` test. All of these will return false for an empty string.

These test for the usual file name rules on the relevant operating systems;
keep in mind that remote mounted file systems may be physically located on a
different system and therefore will not necessarily follow exactly the same
rules. There are no separate functions for Linux because as far as I know it
does not impose any file name restrictions beyond the standard Posix rules.

* `pair<U8string, U8string>` **`split_path`**`(const U8string& file, uint32_t flags = 0)`
* `pair<NativeString, NativeString>` **`split_path`**`(const NativeString& file, uint32_t flags = 0)`
* `pair<U8string, U8string>` **`split_file`**`(const U8string& file)`
* `pair<NativeString, NativeString>` **`split_file`**`(const NativeString& file)`

These functions break down a file name into its constituent parts. The
`split_path()` function breaks the full name into directory and leaf names;
for example, if the original file name is `"/foo/bar/hello.txt"`, the
directory name is `"/foo/bar"` and the leaf name is `"hello.txt"`. If the file
name refers to the file system root (e.g. `"/"`), the directory is the full
file name and the leaf name is empty.

By default, the delimiter between the directory and leaf names is discarded,
unless the directory is a root name that requires the delimiter suffix for
correct identification; if the `fs_fullname` flag is set, the delimiter (if
any) will always be kept at the end of the directory part. This is the only
flag recognised.

The `split_file()` function breaks the leaf name into a base and extension
(discarding the directory part); for example, if the original file name is
`"foo/bar/hello.txt"`, the base name is `"hello"` and the extension is
`".txt"`. The extension starts with the last dot in the leaf name, excluding
its first character; if the name does not contain a dot, or if the only dot is
at the beginning, the base name is the full leaf name and the extension is
empty.

Examples:

    split_path("/foo/bar/hello.txt") == {"/foo/bar", "hello.txt"}
    split_file("/foo/bar/hello.txt") == {"hello", ".txt"}
    split_file(".hello") == {".hello", ""}

## File system query functions ##

These functions perform read-only operations on the file system.

* `U8string` **`current_directory`**`()`
* `NativeString` **`native_current_directory`**`()`

Query the current working directory of the calling process. These may throw
`std::system_error` in some unusual cases of failure, usually involving access
permission problems.

* `bool` **`file_exists`**`(const U8string& file)`
* `bool` **`file_exists`**`(const NativeString& file)`

Query whether a file exists. This may give a false negative if the file exists
but is not accessible to the calling process.

* `FileId` **`file_id`**`(const U8string& file, uint32_t flags = 0)`
* `FileId` **`file_id`**`(const NativeString& file, uint32_t flags = 0)`

Returns a unique file identifier, intended to identify the file even if it is
referred to by different paths. It will return zero if the file does not exist
or the caller does not have permission to query its properties. The only flag
recognised is `fs_follow`, which causes the function to return the ID of the
linked file instead of that of the symlink; it has no effect if the file named
is not a symlink.

This is based on the device and inode numbers on Unix, or the file index on
Windows. Completely reliable file identification cannot be guaranteed in the
presence of parallel remote mounts or similar trickery. On some systems it may
throw `std::system_error` in unusual cases.

* `bool` **`file_is_directory`**`(const U8string& file)`
* `bool` **`file_is_directory`**`(const NativeString& file)`

Query whether a file is a directory. This will return `false` if the file does
not exist; it may give a false negative if the directory exists but is not
accessible to the calling process.

* `bool` **`file_is_hidden`**`(const U8string& file)`
* `bool` **`file_is_hidden`**`(const NativeString& file)`

True if the file is normally hidden. This will return `false` if the file does
not exist or is not accessible to the calling process. On Unix this is based
on the file name (a file is hidden if its name starts with a dot), but on
Windows this is a metadata property.

* `bool` **`file_is_symlink`**`(const U8string& file)`
* `bool` **`file_is_symlink`**`(const NativeString& file)`

True if the file is a symbolic link.

* `uint64_t` **`file_size`**`(const U8string& file, uint32_t flags = 0)`
* `uint64_t` **`file_size`**`(const NativeString& file, uint32_t flags = 0)`

Returns the size of the file in bytes. This will return zero if the file does
not exist, or if it can't be accessed for any other reason. If the file is a
directory, by default only the size of the directory entry itself (which may
be zero on some systems) is returned; if the `fs_recurse` flag is supplied,
the directory's contents will be recursively scanned (symbolic links are not
followed).

* `U8string` **`resolve_path`**`(const U8string& file)`
* `NativeString` **`resolve_path`**`(const NativeString& file)`

Resolves a relative path to an absolute one. On Unix this includes resolving
the `"~"` home directory convention. This will not resolve symlinks in the
path. It will remove redundant path components such as a trailing `"/."`, and
on Windows it will expand short file names to long ones, so a path that was
already absolute will not necessarily be returned unchanged.

* `U8string` **`resolve_symlink`**`(const U8string& file)`
* `NativeString` **`resolve_symlink`**`(const NativeString& file)`

Returns the file pointed to by a symlink. If the argument names a file that
does not exist or is not a symlink, this will simply return the argument
unchanged. This will throw `std::system_error` if anything goes wrong, such as
a permission failure or a symlink loop. On Windows, resolving a symlink
requires opening the target file, so this will fail if the target file does
not exist (presumably the symlink still contains a path but there does not
appear to be any way to retrieve it).

## File system modifying functions ##

These functions perform operations that require write access to the file
system.

* `void` **`copy_file`**`(const U8string& src, const U8string& dst, uint32_t flags = 0)`
* `void` **`copy_file`**`(const NativeString& src, const NativeString& dst, uint32_t flags = 0)`

Copy a file. If the `fs_recurse` flag is used, this will copy a directory
recursively; otherwise, it will fail if the source file is a directory. If the
`fs_overwrite` flag is used, an existing file of the same name will be deleted
if possible; otherwise, the copy will fail. If the existing destination file
is a directory, it will only be replaced if the `fs_recurse` flag is also
present (regardless of whether the source is a directory). Regardless of
flags, it will always fail if the source and destination are the same.
Symbolic links will be copied as links; the linked file will not be copied.

This will throw `std::system_error` if anything goes wrong. This is
necessarily a non-atomic operation; there is always the possibility that an
interruption or a race condition between threads will leave a partially copied
file or directory.

* `void` **`make_directory`**`(const U8string& dir, uint32_t flags = 0)`
* `void` **`make_directory`**`(const NativeString& dir, uint32_t flags = 0)`

Create a directory (with default permissions). It will do nothing if the
directory already exists. If the `fs_recurse` flag is set, this will
recursively create any missing parent directories (similar to `mkdir -p`). If
the `fs_overwrite` flag is set, and a file of the same name exists but is not
a directory, the existing file will be replaced.

This will throw `std::system_error` if the named file already exists but is
not a directory, and either the `fs_overwrite` flag is not used or an attempt
to delete the existing file fails, if the directory path is not a legal
filename, if the parent directory does not exist and the `recurse` flag was
not set, or if the caller does not have permission to create the directory.

* `void` **`make_symlink`**`(const U8string& file, const U8string& link, uint32_t flags = 0)`
* `void` **`make_symlink`**`(const NativeString& file, const NativeString& link, uint32_t flags = 0)`

Creates a symlink named `link` pointing to `file`. This will do nothing if the
symlink already exists. If the `fs_overwrite` flag is set, this will delete
any existing file with the same name as the new link; if the existing file is
a non-empty directory, it will only be replaced if the `fs_recurse` flag is
also set. This will throw `std::system_error` if the symlink cannot be
created, or if a file or directory of the same name as `link` already exists
but the necessary flags were not supplied.

* `void` **`move_file`**`(const U8string& src, const U8string& dst, uint32_t flags = 0)`
* `void` **`move_file`**`(const NativeString& src, const NativeString& dst, uint32_t flags = 0)`

Rename a file or directory. This will attempt to use the operating system's
native `rename()` call if possible, but will attempt a copy-and-delete
operation if this fails. This follows the same rules, and respects the same
flags, as `copy_file()`, except that moving a file to itself always succeeds,
and directories are always moved recursively (the `fs_recurse` flag is only
needed if an existing directory is to be replaced). This will throw
`std::system_error` if anything goes wrong.

* `void` **`remove_file`**`(const U8string& file, uint32_t flags = 0)`
* `void` **`remove_file`**`(const NativeString& file, uint32_t flags = 0)`

Delete a file or directory. If the `fs_recurse` flag is set, directories will
be deleted recursively (like `rm -rf`; this will not follow symbolic links);
the flag is not needed to delete an empty directory. This will do nothing if
the named file does not exist to begin with. It will throw `std::system_error`
if the name is not a legal filename, if the name refers to a non-empty
directory and the `fs_recurse` flag was not set, or if the caller does not
have permission to delete the file.

## Directory iterators ##

* `class` **`[Native]DirectoryIterator`**
    * `using [Native]DirectoryIterator::`**`difference_type`** `= ptrdiff_t`
    * `using [Native]DirectoryIterator::`**`iterator_category`** `= std::input_iterator_tag`
    * `using [Native]DirectoryIterator::`**`value_type`** `= [NativeString or U8string]`
    * `using [Native]DirectoryIterator::`**`pointer`** `= const value_type*`
    * `using [Native]DirectoryIterator::`**`reference`** `= const value_type&`
    * `[Native]DirectoryIterator::`**`[Native]DirectoryIterator`**`()`
    * `explicit [Native]DirectoryIterator::`**`[Native]DirectoryIterator`**`(const value_type& dir, uint32_t flags = 0)`
    * _[standard iterator operations]_
* `Irange<DirectoryIterator>` **`directory`**`(const U8string& dir, uint32_t flags = 0)`
* `Irange<NativeDirectoryIterator>` **`directory`**`(const NativeString& dir, uint32_t flags = 0)`

An iterator over the files in a directory. Normally you should call the
`directory()` function to get an iterator range, rather than explicitly
constructing a `[Native]DirectoryIterator`. Note that this is an input (i.e.
single pass) iterator (a restriction imposed by the way the underlying system
APIs work).

On systems (Windows) where the native string type is not 8-bit, there are two
directory iterator types: `NativeDirectoryIterator` has a value type of
`NativeString`, and returns the actual file names even if they are not valid
Unicode (unless the `fs_unicode` option is used); `DirectoryIterator` has a
value type of `U8string`, and converts file names using the usual substitution
convention if the original name is invalid.

On Unix, `DirectoryIterator` and `NativeDirectoryIterator` are the same type.
Although the value type is nominally `U8string`, file names that are not valid
UTF-8 (on systems where this is possible) are passed through unchanged.

The following flags are recognised:

Flag               | Description
----               | -----------
**`fs_dotdot`**    | Include . and ..
**`fs_fullname`**  | Return full file names
**`fs_hidden`**    | Include hidden files
**`fs_unicode`**   | Skip files with non-Unicode names

If the name passed to the constructor, or to the `directory()` function,
refers to a file that does not exist or is not a directory, it will simply be
treated as an empty directory.

By default, an iterator dereferences to a file's leaf name; if the
`fs_fullname` option is used, the full path is reported instead, including the
directory (this is based on the name passed to the constructor, and will not
be a fully qualified absolute path if the original directory name was not).

If the `fs_unicode` flag is supplied, and the original directory name was
invalid UTF, no files will be returned.

The order in which files are returned is unspecified; do not rely on them
being returned in the same order as the `ls` or `dir` command, or on the order
remaining the same on separate iterations over the same directory. Behaviour
is also unspecified if a directory's contents change while a directory
iterator is scanning it; the change may or may not be visible to the caller.
