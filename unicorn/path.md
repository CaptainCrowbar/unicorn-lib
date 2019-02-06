# [Unicorn Library](index.html): File Path #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/path.hpp"`

## Contents ##

[TOC]

## Introduction ##

The `Path` class holds a file system path as a string. Internally, the file
name is stored as a `std::string` on Unix, or a `std::wstring` on Windows, but
can be converted from a different string type on construction, and retrieved
either in its native format or as a UTF-8 string. The class provides a set of
file system operations, such as file renaming and deletion, directory search,
and so on.

File handling in Unicode always has the problem that the actual file names on
most systems are not guaranteed to be valid Unicode. On most Unix systems, a
file name is an arbitrary sequence of bytes, with no restrictions other than
excluding the null and slash characters; while file names on systems such as
Linux are usually expected to be encoded in UTF-8 when they contain non-ASCII
characters, this is not enforced in any way. On Microsoft Windows, using the
NTFS file system, file names are nominally UTF-16 (with a short list of
disallowed characters), but not all of the wide character API functions that
deal with files check for valid encoding; it's not hard to create a file whose
name is an arbitrary sequence of 16-bit integers. The HFS+ file system
formerly used by Apple on macOS and iOS appears to be the only widely used
file system that actually does enforce valid Unicode names at the file system
level, although it then proceeds to complicate matters by using a proprietary
normalization scheme that does not match any of the four standard ones, and in
any case the new APFS file system does not appear to enforce any kind of
encoding.

To deal with this situation, the default behaviour of `Path` when constructed
from a native file name string (`std::string` on Unix, `std::wstring` on
Windows) is to copy the string verbatim, without checking for or correcting
any UTF encoding. When converting from any other string type, however, the
default is to enforce valid UTF encoding in the converted string, which may
result in loss of information if the original string was not valid UTF.
Optionally, conversion behaviour can be controlled by explicitly supplying one
of the standard conversion mode flags from the Unicorn library:

<!-- TEXT -->
* `Utf::`**`ignore`**   _- Assume valid UTF input_
* `Utf::`**`replace`**  _- Replace invalid UTF with `U+FFFD`_
* `Utf::`**`throws`**   _- Throw `EncodingError` on invalid UTF_

The examples in the documentation below mostly use Unix-style file names for
simplicity; the equivalent code on Windows will make the obvious substitutions
of backslashes for slashes, and drive roots such as `"C:\"` for `"/"`.

The term "leaf name" is used here to mean the name of an individual file
within a directory, with no directory path prefix (for example, the leaf name
of `"/foo/bar/hello.txt"`, or `"C:\foo\bar\hello.txt"`, is `"hello.txt"`).

### Exceptions ###

<!-- TEXT -->
* Any function that accepts a `flags` argument will throw `std::invalid_argument` if an invalid combination of flags is supplied.
* Any function that accepts UTF conversion flags will throw `EncodingError` if the `Utf::throws` flag is given and the input string is invalid.
* Any function not marked `noexcept` may throw `std::bad_alloc`.
* Exception behaviour is only documented for individual functions where it varies from the above rules.

## Class Path ##

### Member types ###

* `using Path::`**`character_type`** `= [char on Unix, wchar_t on Windows]`
* `using Path::`**`flag_type`** `= [unsigned integer type]`
* `using Path::`**`host_string_type`** `= [std::wstring on Cygwin, otherwise the same as string_type]`
* `using Path::`**`id_type`** `= std::pair<uint64_t, uint64_t>`
* `using Path::`**`string_type`** `= [std::string on Unix, std::wstring on Windows]`
* `using Path::`**`string_view_type`** `= [std::string_view on Unix, std::wstring_view on Windows]`

Type aliases.

* `enum class Path::`**`cmp`**
    * `Path::cmp::`**`cased`**
    * `Path::cmp::`**`icase`**
    * `Path::cmp::`**`native`**
* `class Path::`**`equal`**
    * `Path::equal::`**`equal`**`() noexcept`
    * `explicit Path::equal::`**`equal`**`(Path::cmp mode) noexcept`
    * `bool Path::equal::`**`operator()`**`(const Path& lhs, const Path& rhs) const`
* `class Path::`**`less`**
    * `Path::less::`**`less`**`() noexcept`
    * `explicit Path::less::`**`less`**`(Path::cmp mode) noexcept`
    * `bool Path::less::`**`operator()`**`(const Path& lhs, const Path& rhs) const`

Comparison objects. These can perform case sensitive or insensitive
comparisons, defaulting to the native mode for the operating system. Case
insensitive comparison of non-Unicode filenames may be unpredictable on some
systems.

* `class Path::`**`deep_search_iterator`**
    * _Const input iterator_
    * _Value type is_ `Path`
* `using Path::`**`deep_search_range`** `= Irange<deep_search_iterator>`
* `class Path::`**`directory_iterator`**
    * _Const input iterator_
    * _Value type is_ `Path`
* `using Path::`**`directory_range`** `= Irange<directory_iterator>`

Iterators over the files in a directory.

* `enum class Path::`**`form`**
    * `Path::form::`**`empty`**
    * `Path::form::`**`absolute`**
    * `Path::form::`**`drive_absolute`**
    * `Path::form::`**`drive_relative`**
    * `Path::form::`**`relative`**

An enumeration of the possible formats for a path.

### Member constants ###

* `static constexpr char Path::`**`delimiter`** `= ['/' on Unix, '\' on Windows]`
* `static constexpr character_type Path::`**`native_delimiter`** `= ['/' on Unix, L'\' on Windows]`
* `static constexpr bool Path::`**`native_case`** `= [false on Windows or Mac, true on other Unix systems]`

Information about the native path format. The `native_case` flag indicates
whether or not the operating system normally expects file names to be case
sensitive, although mounted non-native file systems may exhibit different
behaviour.

* `static constexpr Path::flag_type Path::`**`append`**       _- If the file already exists, append to it instead of overwriting_
* `static constexpr Path::flag_type Path::`**`bottom_up`**    _- Search a directory tree in bottom up order instead of top down_
* `static constexpr Path::flag_type Path::`**`legal_name`**   _- Fail if the file name is illegal for the operating system_
* `static constexpr Path::flag_type Path::`**`may_copy`**     _- Fall back on copying files if the original operation is not possible_
* `static constexpr Path::flag_type Path::`**`may_fail`**     _- Return a default value instead of throwing if the operation fails_
* `static constexpr Path::flag_type Path::`**`no_follow`**    _- Don't follow symlinks_
* `static constexpr Path::flag_type Path::`**`no_hidden`**    _- Ignore hidden files_
* `static constexpr Path::flag_type Path::`**`overwrite`**    _- Replace the file if it already exists_
* `static constexpr Path::flag_type Path::`**`recurse`**      _- Perform directory operations recursively_
* `static constexpr Path::flag_type Path::`**`std_default`**  _- If the path is empty or `"-"`, use standard input or output instead_
* `static constexpr Path::flag_type Path::`**`unicode`**      _- Ignore files whose names are not valid UTF_

Flags controlling the behaviour of some of the path functions. Further details
of the resulting behaviour are given with the relevant functions. Where
documented, the UTF conversion flags (`Utf::ignore/replace/throws`)
can also be combined with some of these.

### Life cycle functions ###

* `Path::`**`Path`**`() noexcept`

Default constructor, equivalent to constructing from an empty string.

* `template <typename C> Path::`**`Path`**`(const std::basic_string<C>& file, flag_type flags = 0)`
* `template <typename C> Path::`**`Path`**`(const std::basic_string_view<C>& file, flag_type flags = 0)`
* `template <typename C> Path::`**`Path`**`(const C* file, flag_type flags = 0)`

Constructors (and implicit conversions) from a string. Any standard string
type can be passed as a file name; if no conversion mode flag is explicitly
supplied, the name will simply be copied verbatim with no encoding checks if
the input string's character type is the same size as the native encoding (one
byte on Unix, two on Windows); otherwise, the conversion mode defaults to
`Utf::replace`. Besides an optional conversion mode, the `legal_name` flag can
also be supplied, in which case the constructor will throw
`std::invalid_argument` if a file name whose format is illegal for the
operating system is supplied.

On construction, paths are brought into a canonical form according to the
following rules;

<!-- TEXT -->
* On Windows, replace any slash delimiters with backslashes
* Trim redundant leading slashes
* Replace `/./` with `/` throughout
* Replace redundant multiple slashes with one
* Trim trailing `/` and `/.`
* On Windows, ensure a trailing slash on network paths
* On Windows, convert the drive letter to upper case

If required, UTF validation is done before canonicalization, OS specific
legality checks after canonicalization.

* `Path::`**`~Path`**`() noexcept`
* `Path::`**`Path`**`(const Path& p)`
* `Path::`**`Path`**`(Path&& p) noexcept`
* `Path& Path::`**`operator=`**`(const Path& p)`
* `Path& Path::`**`operator=`**`(Path&& p) noexcept`

Other life cycle operations.

### Path name functions ###

These operate on the path purely as a string, making no contact with the
actual file system.

* `Ustring Path::`**`name`**`(flag_type flags = Utf::replace) const`
* `Ustring` **`to_str`**`(const Path& p)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Path& p)`

These return the full path as UTF-8, possibly involving an encoding conversion
step.

* `string_type Path::`**`os_name`**`() const`
* `string_view_type Path::`**`os_view`**`() const noexcept`
* `const character_type* Path::`**`c_name`**`() const noexcept`

These return the full path in its native OS form, with no conversion.

* `host_string_type Path::`**`native_name`**`() const`

On Cygwin, this returns the underlying NTFS file name as a `std::wstring`, and
may throw `system_error` if there is a problem retrieving this. On any other
host, this is a synonym for `os_name()`.

* `Ustring Path::`**`as_url`**`(flag_type flags = Utf::replace) const`

Returns the file name as a UTF-8 `"file://..."` URL. This will throw
`std::invalid_argument` if the path is not absolute.

* `std::vector<Ustring> Path::`**`breakdown`**`() const`
* `std::vector<string_type> Path::`**`os_breakdown`**`() const`

Break the path down into its directory and file elements. If the path is
absolute, the first element will be the root path.

* `Path Path::`**`change_ext`**`(Uview new_ext) const`

Replace the file extension with a new one. The new extension can be supplied
with or without a leading dot, or can be an empty string to remove the
extension. This will throw `std::invalid_argument` if the path is empty or is
a root path.

* `bool Path::`**`empty`**`() const noexcept`
* `bool Path::`**`is_absolute`**`() const noexcept`
* `bool Path::`**`is_drive_absolute`**`() const noexcept`
* `bool Path::`**`is_drive_relative`**`() const noexcept`
* `bool Path::`**`is_relative`**`() const noexcept`
* `Path::form Path::`**`path_form`**`() const noexcept`

Query which syntactic form the path has. Of the first five functions, exactly
one will always be true for any path; `path_form()` returns the type as an
enumeration value. The concepts of a "drive absolute" (e.g. `"\foo"`) or
"drive relative" (e.g. `"C:foo"`) path are specific to Windows; on Unix,
`is_drive_absolute()` and `is_drive_relative()` are always false, and
`path_form()` will never return one of those values.

* `bool Path::`**`is_leaf`**`() const noexcept`

True if the path is a leaf name (one with no directory prefix).

* `bool Path::`**`is_legal`**`() const noexcept`

True if the path is legal for this operating system.

* `bool Path::`**`is_root`**`() const noexcept`

True if the path is the root of a logical file system. This is a purely
syntactic check and does not attempt to check against actual mount points. On
Unix this is true only for `"/"` (except that Windows-style network paths are
recognized on Cygwin); on Windows, this checks for a drive letter (e.g.
`"C:\"`) or network path (e.g. `"\\server\"`).

* `bool Path::`**`is_unicode`**`() const noexcept`

True if the name is a valid UTF encoding.

* `size_t Path::`**`hash`**`() const noexcept`
* `struct std::`**`hash`**`<Path>`

Hash function. This simply returns the standard hash of the internal string.

* `Path Path::`**`relative_to`**`(const Path& base) const`

Returns a relative path locating the current path relative to the given base
path. The current and base paths must be either both absolute or both
relative; if they are relative, they are assumed to be relative to the same
origin. On Windows, if the two paths are absolute and have different drive
prefixes, the original absolute path will be returned unchanged. This will
throw `std::invalid_argument` if the current path and base path are not both
absolute or both relative.

* `std::pair<Ustring, Ustring> Path::`**`split_leaf`**`() const`
* `std::pair<string_type, string_type> Path::`**`split_os_leaf`**`() const`

Splits the path's leaf name into a base and extension; the extension begins
with the last dot, provided this is not the first or last character of the
leaf name. The part of the path before the leaf name is not included in the
return value. If the path is a root path, both strings will be empty. For
example, `"/foo/bar/hello.txt"` will return `{"hello",".txt"}`.

* `std::pair<Path, Path> Path::`**`split_path`**`() const`

Splits the path into a directory path and a leaf name. If the path is a root
path, the leaf name will be empty. For example, `"/foo/bar/hello.txt"` will
return `{"/foo/bar","hello.txt"}`.

* `std::pair<Path, Path> Path::`**`split_root`**`() const`

Splits the path into a root path and a path relative to the root. For example,
`"/foo/bar/hello.txt"` will return `{"/","foo/bar/hello.txt"}`.

* `static Path Path::`**`common`**`(const Path& lhs, const Path& rhs)`
* `template <typename Range> static Path Path::`**`common`**`(const Range& files)`

Return the longest common prefix of two or more paths. The return value will
be a complete path (i.e. it will not split a path element in half if two path
elements happen to share a substring). The returned path will be empty if the
argument paths are a mixture of absolute and relative paths, or (on Windows)
if they are absolute but do not all have the same root. For example,
`common("/a/b/c/d/e/f","/a/b/c/x/y/z")` will return `"/a/b/c"`.

* `static Path Path::`**`join`**`(const Path& lhs, const Path& rhs)`
* `template <typename Range> static Path Path::`**`join`**`(const Range& files)`
* `Path` **`operator/`**`(const Path& lhs, const Path& rhs)`
* `Path&` **`operator/=`**`(Path& lhs, const Path& rhs)`

Join two paths. This will discard the LHS and return the RHS unchanged if the
LHS is empty or the RHS is absolute; otherwise, the resulting path is obtained
by assuming the RHS is relative to the LHS.

* `bool` **`operator==`**`(const Path& lhs, const Path& rhs) noexcept`
* `bool` **`operator!=`**`(const Path& lhs, const Path& rhs) noexcept`
* `bool` **`operator<`**`(const Path& lhs, const Path& rhs) noexcept`
* `bool` **`operator>`**`(const Path& lhs, const Path& rhs) noexcept`
* `bool` **`operator<=`**`(const Path& lhs, const Path& rhs) noexcept`
* `bool` **`operator>=`**`(const Path& lhs, const Path& rhs) noexcept`

Comparison operators. These perform lexicographical comparison by code unit,
not by code point, because the path string may not be a valid encoding; on
Windows this means that paths that contain characters outside the BMP will not
necessarily sort in Unicode order.

### File system query functions ###

These require read-only access to the file system. All functions not marked
`noexcept` can throw `std::system_error` if the underlying system API fails,
in addition to any explicitly documented exceptions.

* `system_clock::time_point Path::`**`access_time`**`(flag_type flags = 0) const noexcept`
* `system_clock::time_point Path::`**`create_time`**`(flag_type flags = 0) const noexcept`
* `system_clock::time_point Path::`**`modify_time`**`(flag_type flags = 0) const noexcept`
* `system_clock::time_point Path::`**`status_time`**`(flag_type flags = 0) const noexcept`

Query the file's time metadata; see also the corresponding update functions
below. The four possible time properties are detailed below. The query
functions will return the epoch if the file does not exist or is not
accessible. If the `no_follow` flag is set, and the path refers to a symlink,
these will operate (if possible) on the symlink rather than the target file.

Property     | Interpretation                                              | Apple       | Other Unix     | Windows
--------     | --------------                                              | -----       | ----------     | -------
Access time  | When the file's content was last read (Posix `atime`)       | Read/write  | Read/write     | Read/write
Create time  | When the file was created                                   | Read only   | Not supported  | Read/write
Modify time  | When the file's content was last modified (Posix `mtime`)   | Read/write  | Read/write     | Read/write
Status time  | When the file's metadata was last modified (Posix `ctime`)  | Read only   | Read only      | Not supported

* `directory_range Path::`**`directory`**`(flag_type flags = 0) const`
* `deep_search_range Path::`**`deep_search`**`(flag_type flags = 0) const`

Return an iterator range over the files within a directory, either the
immediate directory members (`directory()`) or a recursive search of all child
directories (`deep_search()`).

The range will be empty if the path does not exist or is not a directory. The
order in which files are returned is unspecified. If the directory's contents
are changed while iteration is in progress, it is unspecified whether the
changes will be visible to the caller. If the `no_hidden` flag is set, hidden
files will not be included. If the `unicode` flag is set, files whose names
are not valid UTF will not be included (this test is applied to the whole
path, so a directory whose own name is invalid will appear to be empty).

By default, `deep_search()` visits files in top down order (a directory is
seen before its contents); if the `bottom_up` flag is set, bottom up order is
used instead (a directory's contents are seen before the directory itself).
Deep search iterators otherwise take the same flags, and follow the same
rules, as directory iterator.

* `bool Path::`**`exists`**`(flag_type flags = 0) const noexcept`

Query whether a file exists. This may give a false negative if the file exists
but is not accessible to the calling process. The `no_follow` flag prevents
this function from following symbolic links.

* `id_type Path::`**`id`**`(flag_type flags = 0) const noexcept`

Returns a unique file identifier, intended to identify the file even if it is
referred to by different paths. It will return a null ID if the file does not
exist or the caller does not have permission to query its properties. This is
based on the file's device and inode numbers, but completely reliable file
identification cannot be guaranteed in the presence of parallel remote mounts
or similar trickery. The `no_follow` flag prevents this function from
following symbolic links.

* `bool Path::`**`is_directory`**`(flag_type flags = 0) const noexcept`
* `bool Path::`**`is_file`**`(flag_type flags = 0) const noexcept`
* `bool Path::`**`is_special`**`(flag_type flags = 0) const noexcept`

Query the file type. Exactly one of these will be true if `exists()` is true.
These may give a false negative if the file exists but is not accessible to
the calling process. The `no_follow` flag prevents this function from
following symbolic links.

* `bool Path::`**`is_hidden`**`() const noexcept`

True if the file is normally hidden. On Unix this is based on the file name (a
file is hidden if its name starts with a dot), and the file's existence is not
checked; on Windows this is a metadata property, and will return false if the
file does not exist or is not accessible to the caller.

* `bool Path::`**`is_symlink`**`() const noexcept`

True if the file is a symbolic link. I haven't tried to support the
complicated symlink-like concepts on NTFS; this simply always returns false on
Windows.

* `Path Path::`**`resolve`**`() const`

This resolves a relative path to an absolute one. If the path is empty, this
will return the current directory. On Unix, if the path is absolute, it will
be returned unchanged, but on Windows any short filenames in the original path
will be resolved to long names even if the path was absolute.

* `Path Path::`**`resolve_symlink`**`() const`

Returns the file pointed to by a symlink. This will just return the original
path unchanged if it is not a symlink.

* `uint64_t Path::`**`size`**`(flag_type flags = 0) const`

Returns the size of a file in bytes. If the `no_follow` flag is set, and the
path refers to a symlink, this will return the size of the link instead of the
target file. If the `recurse` flag is set, and the path refers to a directory,
this will recursively determine the total size of the directory and everything
in it (symlinks below the outermost directory will not be followed).

### File system update functions ###

These require write access to the file system. All of these can throw
`std::system_error` if the underlying system API fails, in addition to any
explicitly documented exceptions.

* `void Path::`**`copy_to`**`(const Path& dst, flag_type flags = 0) const`

Copy a file from the current path to the destination path. If the `overwrite`
flag is set, this will replace any existing file at the destination. If the
`recurse` flag is set, and the path refers to a directory, it will be copied
recursively. This will fail if the source file does not exist, if the source
and destination are the same file, if the destination already exists and
`overwrite` was not set, or if the source is a directory and `recurse` was not
set.

* `void Path::`**`create`**`() const`

If the file does not exist, an empty file with default permissions is created.

* `void Path::`**`make_directory`**`(flag_type flags = 0) const`

Create a directory, with default permissions, at the current path. If the
`overwrite` flag is set, and the path exists but is not a directory, it will
be replaced. If the `recurse` flag is set, any missing parent directories will
also be created (like `mkdir -p`). This will do nothing if the directory
already exists; it will fail if the path exists but is not a directory, and
the `overwrite` flag was not set.

* `void Path::`**`make_symlink`**`(const Path& linkname, flag_type flags = 0) const`

Create a symlink at `linkname`, pointing to the current path. If the
`may_copy` flag is set, this will copy the file (recursively) if a symlink
can't be created. If the `overwrite` flag is set, any existing file at
`linkname` will be replaced. This will do nothing if the link already exists
and points to the correct file. The existence of the target path is not
checked. This will fail if a file already exists at `linkname`, it is not a
symlink already pointing to the target path, and the `overwrite` flag was not
set. On Windows this will always fail unless the `may_copy` flag is set.

* `void Path::`**`move_to`**`(const Path& dst, flag_type flags = 0) const`

Move a file from the current path to the destination path. If the `may_copy`
flag is set, this will attempt to copy the file and then delete the original
if the move fails (this may leave two copies of a file if the operation fails
partway through). If the `overwrite` flag is set, this will replace any
existing file at the destination. This will do nothing if the source and
destination are the same file. This will fail if the source file does not
exist, or if the destination already exists and `overwrite` was not set.

* `void Path::`**`remove`**`(flag_type flags = 0) const`

Delete a file. This will do nothing if the file already does not exist. If the
`recurse` flag is set, this will delete directories recursively (like `rm
-rf`). This will fail if the path refers to a non-empty directory and the
`recurse` flag is not used, if the caller does not have permission to remove
the file, or in some circumstances, if the file is in use by another process.

* `void Path::`**`set_access_time`**`(flag_type flags = 0) const`
* `void Path::`**`set_access_time`**`(system_clock::time_point t, flag_type flags = 0) const`
* `void Path::`**`set_create_time`**`(flag_type flags = 0) const`
* `void Path::`**`set_create_time`**`(system_clock::time_point t, flag_type flags = 0) const`
* `void Path::`**`set_modify_time`**`(flag_type flags = 0) const`
* `void Path::`**`set_modify_time`**`(system_clock::time_point t, flag_type flags = 0) const`
* `void Path::`**`set_status_time`**`(flag_type flags = 0) const`
* `void Path::`**`set_status_time`**`(system_clock::time_point t, flag_type flags = 0) const`

Modify the file's time metadata. Refer to the corresponding query functions
(above) for details of the interpretation of the four time properties.

The functions that do not take a time argument will set it to the current
time. If the `no_follow` flag is set, and the path refers to a symlink, these
will operate (if possible) on the symlink rather than the target file. The
setter functions will throw `std::system_error` if the file does not exist, if
the calling process does not have permission to modify the requested time
field, or if that field is not defined, or not modifiable, on this operating
system.

Query and update functions are included for all four times for symmetry and
futureproofing, but no known operating system supports explicitly setting the
file metadata modification time, and the current implementation of
`set_status_time()` will always fail.

### I/O functions ###

All of these functions may throw `std::ios::failure` if anything goes wrong.

* `void Path::`**`load`**`(std::string& dst, size_t maxlen = npos, flag_type flags = 0) const`

Read the contents of a file into a string (erasing its former contents).
Optionally, a maximum number of bytes can be specified. If the `may_fail` flag
is set, this will return an empty string if the file does not exist or a read
error occurs. If the `std_default` flag is set, this will read from standard
input if the path is an empty string or `"-"`.

* `void Path::`**`save`**`(std::string_view src, flag_type flags = 0) const`

Writes the contents of a string to a file. If the `append` flag is set and the
file already exists, the new data will be appended to it; otherwise, it will
be overwritten. If the `std_default` flag is set, this will write to standard
output if the path is an empty string or `"-"`.

### Process state functions ###

All of these functions may throw `std::system_error` if the underlying system
API fails.

* `void Path::`**`change_directory`**`() const`
* `static Path Path::`**`current_directory`**`()`

Query or set the calling process's current working directory. Remember that
this is not thread safe.
