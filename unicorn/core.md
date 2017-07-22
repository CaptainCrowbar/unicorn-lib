# [Unicorn Library](index.html): Core Definitions #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/core.hpp"`

This module provides some common definitions used throughout the library.

## Contents ##

[TOC]

## Character types ##

* `template <typename T> constexpr bool` **`is_character_type`**

True if `T` is one of the character types recognized by the Unicorn library
(`char`, `char16_t`, `char32_t`, or `wchar_t`).

## Exceptions ##

* `class` **`InitializationError`**`: public std::runtime_error`

An abstract base class for internal errors that may happen while loading the
Unicode tables used by certain functions. Functions that can throw exceptions
derived from this are individually documented.

* `class` **`EncodingError`**`: public std::runtime_error`
    * `EncodingError::`**`EncodingError`**`()`
    * `explicit EncodingError::`**`EncodingError`**`(const U8string& encoding, size_t offset = 0, char32_t c = 0)`
    * `template <typename C> EncodingError::`**`EncodingError`**`(const U8string& encoding, size_t offset, const C* ptr, size_t n = 1)`
    * `const char* EncodingError::`**`encoding`**`() noexcept const`
    * `size_t EncodingError::`**`offset`**`() const noexcept`

An exception thrown to indicate a text encoding error encountered when
converting a string from one encoding to another, or when checking an encoded
string for validity. The offset of the offending data within the source string
(when available) can be retrieved through the `offset()` method. If possible,
a hexadecimal representation of the offending data will be included in the
error message.

## Version information ##

* `Version` **`unicorn_version`**`() noexcept`
* `Version` **`unicode_version`**`() noexcept`

These return the version of the Unicorn library and the supported version of
the Unicode standard.
