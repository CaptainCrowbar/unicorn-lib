# [Unicorn Library](index.html): Environment Variables #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/environment.hpp"`

## Contents ##

[TOC]

## Introduction ##

Operating systems store environment variables in a system dependent format,
based on the string encoding supported by the native system call API. This
means that environment variable names and values are stored as 16 bit strings
on Microsoft Windows, but as 8 bit strings on Unix systems. None of the native
environment variable APIs make any attempt to enforce Unicode encoding forms;
although normally intended to be interpreted as character data, the names and
values are really arbitrary strings of 8 or 16 bit unsigned integers.

The Unicorn environment variable API allows the caller to express variable
names and values either in UTF-8 (`Ustring`) or in the operating system's
native encoding (`NativeString`). On Unix, the native API uses 8 bit strings,
`NativeString` and `Ustring` are really the same type (plain `std::string`),
and there is only one set of functions here; although the function signatures
are nominally written in terms of UTF-8 strings, in fact any arbitrary byte
string can be passed or may be returned. On Windows the native API uses 16 bit
strings, and `NativeString` is an alias for `std::wstring`; the `Ustring`
versions of these functions will convert between UTF-8 and UTF-16, using the
standard replacement convention to handle invalid encoding.

Whether environment variable names are case sensitive is also operating system
dependent.

The native environment variable APIs are usually not threadsafe; all of the
functions in this module ensure thread safety internally by locking a common
global mutex.

## Functions ##

All of these functions can throw `std::invalid_argument` if an environment
variable name passed in is empty or contains an equals sign, or
`std::system_error` if anything goes wrong with the underlying system API
call.

* `NativeString` **`expand_env`**`(const NativeString& src, uint32_t flags = Environment::native)`
* `Ustring` **`expand_env`**`(const Ustring& src, uint32_t flags = Environment::native)`

Expand environment variable references in a string. The `flags` argument
indicates whether to follow the Posix convention (`"$VAR"` or `"${VAR}"`) or
the Windows convention (`"%VAR%"`); the flags can be combined to recognise
both. Nonexistent environment variables will be replaced with an empty string.
In the absence of braces, a Posix variable name is assumed be the longest
substring of alphanumeric characters (including the underscore) following the
dollar sign.

* `NativeString` **`get_env`**`(const NativeString& name)`
* `Ustring` **`get_env`**`(const Ustring& name)`

Query the value of an environment variable. This will return an empty string
if the variable does not exist.

* `bool` **`has_env`**`(const NativeString& name)`
* `bool` **`has_env`**`(const Ustring& name)`

Query whether an environment variable exists. Windows, unlike Unix, does not
always distinguish clearly between a variable that does not exist, and one
that exists but whose value is an empty string; if a variable has been set to
an empty value, this function may or may not report its existence on Windows.

* `void` **`set_env`**`(const NativeString& name, const NativeString& value)`
* `void` **`set_env`**`(const Ustring& name, const Ustring& value)`

Set the value of a variable in the process's environment block.

* `void` **`unset_env`**`(const NativeString& name)`
* `void` **`unset_env`**`(const Ustring& name)`

Delete a variable from the process's environment block.

## Class Environment ##

* `class` **`Environment`**

A class that holds a complete environment block.

* `class` **`Environment`**`::iterator`

A read-only iterator over the name-value pairs stored in an `Environment`
object. This is a forward iterator whose value type is
`pair<NativeString,NativeString>`.

* `constexpr uint32_t Environment::`**`posix`**
* `constexpr uint32_t Environment::`**`windows`**
* `constexpr uint32_t Environment::`**`native`**

Bitmask constants indicating how environment variable expansion is to be done
in `expand_env()` and `Environment::expand()`. The `native` constant will be
equal to either `posix` or `windows`, depending on the operating system.

* `Environment::`**`Environment`**`()`
* `explicit Environment::`**`Environment`**`(bool from_process)`
* `Environment::`**`Environment`**`(const Environment& env)`
* `Environment::`**`Environment`**`(Environment&& env) noexcept`
* `Environment::`**`~Environment`**`() noexcept`
* `Environment& Environment::`**`operator=`**`(const Environment& env)`
* `Environment& Environment::`**`operator=`**`(Environment&& env) noexcept`

Life cycle operations. If the `from_process` flag is set, the constructor will
copy the environment data from the calling process; otherwise, the newly
constructed `Environment` object will be empty.

* `NativeString Environment::`**`operator[]`**`(const NativeString& name)`
* `Ustring Environment::`**`operator[]`**`(const Ustring& name)`
* `NativeString Environment::`**`expand`**`(const NativeString& src, uint32_t flags = Environment::native)`
* `Ustring Environment::`**`expand`**`(const Ustring& src, uint32_t flags = Environment::native)`
* `NativeString Environment::`**`get`**`(const NativeString& name)`
* `Ustring Environment::`**`get`**`(const Ustring& name)`
* `bool Environment::`**`has`**`(const NativeString& name)`
* `bool Environment::`**`has`**`(const Ustring& name)`
* `void Environment::`**`set`**`(const NativeString& name, const NativeString& value)`
* `void Environment::`**`set`**`(const Ustring& name, const Ustring& value)`
* `void Environment::`**`unset`**`(const NativeString& name)`
* `void Environment::`**`unset`**`(const Ustring& name)`

These perform the same operations as the corresponding environment variable
functions described above (`operator[]` is a synonym for `get()`), and will
throw the same exceptions.

* `Environment::iterator Environment::`**`begin`**`() const`
* `Environment::iterator Environment::`**`end`**`() const`

Iterators over the name-value pairs stored in an `Environment` object.

* `void Environment::`**`clear`**`()`

Clears all data from the environment block.

* `bool Environment::`**`empty`**`() const noexcept`

True if the environment block contains no data.

* `void Environment::`**`load`**`()`

Loads the `Environment` object's data from the calling process's environment
block. Any existing data in the `Environment` object is discarded.

* `NativeCharacter** Environment::`**`ptr`**`()`

Collects the environment block into a single data block in the standard format
(suitable for passing to process spawning functions, for example), and returns
a pointer to it. The returned pointer is invalidated by any operation that may
modify the `Environment` object (any call to a non-`const` function).

* `size_t Environment::`**`size`**`() const noexcept`

Returns the number of entries (variables) in the environment block.
