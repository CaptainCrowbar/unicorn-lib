# [Unicorn Library](index.html): String Formatting #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/format.hpp"`

This module provides facilities for formatting various kinds of data as
Unicode strings, in a manner similar to `printf()` in C, `str.format()` in
Python, etc. Formatters are supplied for commonly used standard types
(booleans, integers, floating point, characters, strings, and time points and
durations); user defined formatters for other types can also be supplied.

Basic formatting for range types (containers and arrays) is provided; these
will be written in comma delimited form. Pairs will be written in
`"key:value"` format. More elaborate formatting for ranges is beyond the scope
of this module and will need to be handled by the caller.

The standard flags use to control formatting are members of the `Format`
class, but are documented with the individual functions they control for
convenience. User defined flags can also be used, as described below under the
`format_type()` function.

## Contents ##

[TOC]

## Basic formatting functions ##

* `template <typename T> Ustring` **`format_type`**`(const T& t, uint64_t flags, int prec)`

This is the core formatting function. The details of how the flags and
precision are interpreted depend on the type being formatted. This function
can be overloaded for user defined types to provide formatting for them; the
other functions in this library will find any user defined overloads through
argument dependent lookup. The library defines specializations for the types
listed here; their behaviour is described in more detail below.

* <!-- TEXT --> `bool`
* `char`, `char16_t`, `char32_t`, `wchar_t`
* `signed char`, `unsigned char`, `short`, `unsigned short`, `int`, `unsigned`, `long`, `unsigned long`, `long long`, `unsigned long long`
* `float`, `double`, `long double`
* `[const] char*`, `[const] char16_t*`, `[const] char32_t*`, `[const] wchar_t*`
* `std::string`, `std::u16string`, `std::u32string`, `std::wstring`
* `std::string_view`, `std::u16string_view`, `std::u32string_view`, `std::wstring_view`
* `std::chrono::duration<R, P>`, `std::chrono::system_clock::time_point`

* <!-- DEFN --> `template <typename T> Ustring` **`format_str`**`(const T& t, uint64_t flags = 0, int prec = -1, size_t width = 0, char32_t pad = U' ')`
* `template <typename T> Ustring` **`format_str`**`(const T& t, const Ustring& flags)`
* `template <typename T> Ustring` **`format_str`**`(const T& t, const char* flags)`

These call `format_type()` to format their first argument, with additional
global options. The first version accepts the same arguments as
`format_type()`, along with a width and padding character; the formatted
string will be padded to the specified width (if it is not already longer than
that).

The other two versions of `format_str()` take a second string argument that
carries the same information as the format control arguments to
`format_type()` and the first version of `format_str()`. The flag descriptions
below indicate which letter corresponds to which flag; when defining
`format_type()` overloads for user defined types, you can use the predefined
flags or use `RS::letter_to_mask()` to define new ones.

Example:

    constexpr uint64_t alpha_format = letter_to_mask('A');
    constexpr uint64_t omega_format = letter_to_mask('Z');

The following global flags are common to all types:

<!-- DEFN -->

Flag                        | Letter  | Description
----                        | ------  | -----------
`Format::`**`left`**        |`<`      | Left align (default)
`Format::`**`right`**       |`>`      | Right align
`Format::`**`centre`**      |`=`      | Centre align
`Format::`**`upper`**       |`U`      | Convert to upper case
`Format::`**`lower`**       |`L`      | Convert to lower case
`Format::`**`title`**       |`T`      | Convert to title case
`Length::`**`characters`**  |`C`      | Measure string in characters (default)
`Length::`**`graphemes`**   |`G`      | Measure string in grapheme clusters
`Length::`**`narrow`**      |`N`      | East Asian width, defaulting to narrow
`Length::`**`wide`**        |`W`      | East Asian width, defaulting to wide

See [`unicorn/string`](string.html) for details of the four string measurement
flags. The alignment flags can be followed by a decimal number, which will be
interpreted as the fully padded width; optionally, a padding character can be
inserted between the alignment flag and the width:

    format_str(42, ">*5") == "***42"

If the flag string includes a number (other than one associated with the
alignment), it will be interpreted as a precision:

    format_str(123.0, "d6") == "123.000"

## Formatter class ##

* `class` **`Format`**
    * `Format::`**`Format`**`()`
    * `explicit Format::`**`Format`**`(const Ustring& format)`
    * `Format::`**`Format`**`(const Format& f)`
    * `Format::`**`Format`**`(Format&& f) noexcept`
    * `Format::`**`~Format`**`() noexcept`
    * `Format& Format::`**`operator=`**`(const Format& f)`
    * `Format& Format::`**`operator=`**`(Format&& f) noexcept`
    * `template <typename... Args> Ustring Format::`**`operator()`**`(const Args&... args) const`
    * `bool Format::`**`empty`**`() const noexcept`
    * `size_t Format::`**`fields`**`() const`
    * `Ustring Format::`**`format`**`() const`

This class is constructed from a formatting string, which contains
placeholders into which arguments will be substituted when the formatting
object's function call operator is called.

A placeholder takes the form `$n` or `${n}`, where `n` is the 1-based index of
the corresponding argument. Optionally the field number can be followed by a
flag string, using the same syntax as `format_str()` above. The braces are
only needed if the field number or flag string is immediately followed by a
letter or digit that would be read as part of the placeholder in the absence
of braces.

Example:

    Format f("Hello $1, your number is $2f3");
    Ustring a = f("Alice", 42); // "Hello Alice, your number is 42.000"
    Ustring b = f("Bob", 1.23); // "Hello Bob, your number is 1.230"

Use `"$$"` to insert a literal dollar sign in the format string.

* `namespace RS::Unicorn::Literals`
    * `Format` **`operator"" _fmt`**`(const char* ptr, size_t len)`

Formatting object literal.

## Formatting for specific types ##

### Boolean formatting ###

<!-- DEFN -->

Flag                    | Letter  | Description
----                    | ------  | -----------
`Format::`**`tf`**      |`t`      | Format as `"true"` or `"false"` (default)
`Format::`**`binary`**  |`b`      | Format as `"1"` or `"0"`
`Format::`**`yesno`**   |`y`      | Format as `"yes"` or `"no"`

### Integer formatting ###

<!-- DEFN -->

These flags apply to all integer types other than the four standard character
types.

Flag                     | Letter  | Description
----                     | ------  | -----------
`Format::`**`decimal`**  |`n`      | Format as a decimal number (default)
`Format::`**`binary`**   |`b`      | Format as a binary number
`Format::`**`hex`**      |`x`      | Format as a hexadecimal number
`Format::`**`roman`**    |`r`      | Format as a Roman numeral; empty string if less than 1
`Format::`**`sign`**     |`s`      | Always show a sign
`Format::`**`signz`**    |`S`      | Always show a sign unless the value is zero

### Floating point formatting ###

<!-- DEFN -->

Flag                     | Letter  | Description
----                     | ------  | -----------
`Format::`**`digits`**   |`d`      | Format with a fixed number of significant figures
`Format::`**`exp`**      |`e`      | Format in scientific notation
`Format::`**`fixed`**    |`f`      | Format with a fixed number of decimal places
`Format::`**`general`**  |`g`      | Use the shorter of `digits` or `exp` (default)
`Format::`**`sign`**     |`s`      | Always show a sign
`Format::`**`signz`**    |`S`      | Always show a sign unless the value is zero
`Format::`**`stripz`**   |`z`      | Strip trailing zeros after the decimal point

### Character and string formatting ###

These flags apply to all character, character pointer, and standard string
types.

<!-- DEFN -->

Flag                      | Letter  | Description
----                      | ------  | -----------
`Format::`**`escape`**    |`e`      | Escape C0 and C1 control characters
`Format::`**`ascii`**     |`a`      | Escape all characters except printable ASCII
`Format::`**`quote`**     |`q`      | Quote the string, and escape C0 and C1 controls
`Format::`**`ascquote`**  |`Q`      | Quote the string, and escape non-ASCII characters
`Format::`**`decimal`**   |`n`      | Format characters as decimal numbers
`Format::`**`hex`**       |`x`      | Format characters as hexadecimal numbers
`Format::`**`hex8`**      |`u`      | Format characters as hex UTF-8 code units
`Format::`**`hex16`**     |`v`      | Format characters as hex UTF-16 code units

### Time and date formatting ###

<!-- DEFN -->

Formatting for `std::chrono::duration` calls `RS::format_time()`, with the
specified precision if one was supplied. Formatting for
`std::chrono::system_clock::time_point` uses ISO 8601 format by default
(`"yyyy-mm-dd hh:mm:ss"`), and respects the following flags:

Flag                    | Letter  | Description
----                    | ------  | -----------
`Format::`**`iso`**     |`t`      | Use strict ISO 8601 format with T delimiter
`Format::`**`common`**  |`c`      | Use the locale's standard format
`Format::`**`local`**   |`l`      | Use the local time zone instead of UTC
