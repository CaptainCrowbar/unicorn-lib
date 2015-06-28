Title: Unicorn Library: Core Definitions
CSS: style.css

# [Unicorn Library](index.html): Core Definitions #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/core.hpp"` ####

This module provides some common definitions used throughout the library.

## Contents ##

* [Imports][]
* [Exceptions][]
* [Basic character types][]
* [Version information][]

## Imports ##

* `using Crow::basic_string      [= std::basic_string]`
* `using Crow::string            [= std::string]`
* `using Crow::u8string          [= std::string]`
* `using Crow::u16string         [= std::u16string]`
* `using Crow::u32string         [= std::u32string]`
* `using Crow::wstring           [= std::wstring]`
* `using Crow::ascii_whitespace  [= "\t\n\v\f\r "]`
* `using Crow::npos              [= std::string::npos]`

Imported for convenience from the Crow library.

## Exceptions ##

* `class EncodingError: public std::runtime_error`
    * `EncodingError::EncodingError()`
    * `explicit EncodingError::EncodingError(const u8string& encoding, size_t offset = 0)`
    * `template <typename C> EncodingError::EncodingError(const u8string& encoding, size_t offset, const C* ptr, size_t n = 1)`
    * `u8string EncodingError::encoding() const`
    * `size_t EncodingError::offset() const noexcept`

An exception thrown to indicate a text encoding error encountered when
converting a string from one encoding to another, or when checking an encoded
string for validity. The offset of the offending data within the source string
(when available) can be retrieved through the `offset()` method. If possible,
a hexadecimal representation of the offending data will be included in the
error message.

## Basic character types ##

* `#define UNICORN_NATIVE_WCHAR 1`

Defined if the operating system's native API uses wide character strings (this
is currently defined only for native Windows builds).

* `using NativeCharacter = [char on Unix, wchar_t on Windows]`
* `using NativeString = [string on Unix, wstring on Windows]`

These are the character and string types used in the operating system's native
ABI.

* `namespace Literals`
    * `NativeString operator"" _nat(const char* s, size_t n)`

Literal suffix to make it easy to define native string constants. The
implementation on wide character systems assumes that `char` values can simply
be directly copied into a `wchar_t`, so this should only be used with simple
ASCII strings; text containing Unicode characters will need separate narrow
and wide character versions.

* `#define UNICORN_WCHAR_UTF16 1`
* `#define UNICORN_WCHAR_UTF32 1`

One of these is defined to indicate which UTF encoding the system's `wstring`
class uses.

* `using WcharEquivalent = [char16_t or char32_t]`
* `using WstringEquivalent = [u16string or u32string]`

These are defined to match the UTF encoding of the system's `wstring` class.

## Version information ##

* `Crow::Version unicorn_version() noexcept`
* `Crow::Version unicode_version() noexcept`

These return the version of the Unicorn library and the supported version of
the Unicode standard.
