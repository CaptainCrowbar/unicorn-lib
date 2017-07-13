# [Unicorn Library](index.html): Non-Unicode Encodings #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/mbcs.hpp"`

This module defines functions for conversion between UTF-8 and non-Unicode
encodings, as well as external UTF encodings with a specific byte order.

## Contents ##

[TOC]

## Constants ##

* `constexpr uint32_t Mbcs::`**`strict`**

Constant used to control how encoding names are interpreted (see below).

## Exceptions ##

* `class` **`UnknownEncoding`**`: public std::runtime_error`
    * `UnknownEncoding::`**`UnknownEncoding`**`()`
    * `explicit UnknownEncoding::`**`UnknownEncoding`**`(const U8string& encoding, const U8string& details = {})`
    * `explicit UnknownEncoding::`**`UnknownEncoding`**`(uint32_t encoding, const U8string& details = {})`
    * `const char* UnknownEncoding::`**`encoding`**`() const noexcept`

Exception thrown to report an unknown encoding name or number.

## Conversion functions ##

* `void` **`import_string`**`(const string& src, U8string& dst, const U8string& enc = "", uint32_t flags = 0)`
* `void` **`import_string`**`(const string& src, U8string& dst, uint32_t enc, uint32_t flags = 0)`
* `void` **`export_string`**`(const U8string& src, string& dst, const U8string& enc = "", uint32_t flags = 0)`
* `void` **`export_string`**`(const U8string& src, string& dst, uint32_t enc, uint32_t flags = 0)`

These functions convert from an external multibyte encoding to UTF-8
(`import_string()`), and from UTF-8 to an external multibyte encoding
(`export_string()`). They work by calling the operating system's native code
conversion API (`iconv()` on Unix; `MultiByteToWideChar()` and
`WideCharToMultiByte()` on Windows).

Encodings can be identified either by name or by a Windows "code page" number.
Either kind of identifier can be used on any system; a built-in lookup table
is used to convert to the form required by the native API. Besides the normal
names and numbers associated with standard encodings, the following special
values can be used:

* `"char"` or an empty string = Use the current locale's default multibyte character encoding.
* `"wchar_t"` = Use the operating system's default wide character encoding.
* `"utf"` = Inspect the string and try to guess the UTF encoding.

The default local encoding is used if no encoding is specified.

An unrecognised encoding name or number will cause an `UnknownEncoding`
exception to be thrown. The code that parses encoding names allows a good deal
of leeway for variations in punctuation and capitalization, but since we are
relying on system APIs for the actual conversion, no promises can be made
about any particular encoding (other than the UTF encodings) being correctly
recognised and handled.

The `flags` argument can be `Utf::replace` (the default) or `Utf::throws`
(these are defined in [`unicorn/utf`](utf.html)). If `Utf::replace` is
selected, invalid data in the input string will be replaced with a system-
defined replacement character (not necessarily `U+FFFD`); if `Utf::throws` is
selected, invalid data will cause an `EncodingError` exception to be thrown,
if this is detectable (see below).

Ignoring invalid encoding is not allowed here; if any MBCS function that takes
a flags argument is passed the `Utf::ignore` flags, it will throw
`std::invalid_argument`.

If the constant `Mbcs::strict` is included in the flags, no attempt will be
made to translate the encoding name or number supplied into an equivalent
acceptable to the underlying API; the value supplied will simply be passed
unchanged (apart from conversion between string and integer where necessary).
This flag has no effect if no encoding is explicitly supplied.

These functions necessarily inherit some of the limitations of the underlying
native APIs. In particular, the Windows APIs do not reliably report encoding
errors; although the wrapper functions make a good faith attempt to respect
the error handling flags, in some cases the underlying conversion function
will go ahead and replace invalid data without reporting an error.

## Utility functions ##

* `U8string` **`local_encoding`**`(const U8string& default_encoding = "utf-8")`

Returns the encoding of the current default locale. The default value will be
returned if no encoding information can be obtained from the operating system.
