# [Unicorn Library](index.html): String Formatting #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/format.hpp"`

This module provides facilities for formatting various kinds of data as
Unicode strings, in a manner similar to `printf()` in C, `str.format()` in
Python, etc. Formatters are supplied for commonly used standard types
(booleans, integers, floating point, characters, strings, and time points and
durations), as well as for the `Uuid` and `Version` types from my core
utilities library; user defined formatters for other types can also be
supplied.

Basic formatting for range types (containers and arrays) is provided; these
will be written in comma delimited form. Pairs will be written in
`"key:value"` format. More elaborate formatting for ranges is beyond the scope
of this module and will need to be handled by the caller.

## Contents ##

[TOC]

## Basic formatting functions ##

* `template <typename T> U8string` **`format_type`**`(const T& t, uint64_t flags, int prec)`

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
* `U8string`, `u16string`, `u32string`, `wstring`
* `std::chrono::duration<R, P>`, `std::chrono::system_clock::time_point`
* `RS::Uuid`, `RS::Version`

* <!-- DEFN --> `template <typename T> U8string` **`format_str`**`(const T& t, uint64_t flags = 0, int prec = -1, size_t width = 0, char32_t pad = U' ')`
* `template <typename T> U8string` **`format_str`**`(const T& t, const U8string& flags)`
* `template <typename T> U8string` **`format_str`**`(const T& t, const char* flags)`

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

    constexpr uint64_t fx_alpha = letter_to_mask('A');
    constexpr uint64_t fx_omega = letter_to_mask('Z');

The following global flags are common to all types:

<!-- DEFN -->

Flag                        | Letter  | Description
----                        | ------  | -----------
**`fx_left`**               |`<`      | Left align (default)
**`fx_right`**              |`>`      | Right align
**`fx_centre`**             |`=`      | Centre align
**`fx_upper`**              |`U`      | Convert to upper case
**`fx_lower`**              |`L`      | Convert to lower case
**`fx_title`**              |`T`      | Convert to title case
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
    * `explicit Format::`**`Format`**`(const U8string& format)`
    * `Format::`**`Format`**`(const Format& f)`
    * `Format::`**`Format`**`(Format&& f) noexcept`
    * `Format::`**`~Format`**`() noexcept`
    * `Format& Format::`**`operator=`**`(const Format& f)`
    * `Format& Format::`**`operator=`**`(Format&& f) noexcept`
    * `template <typename... Args> U8string Format::`**`operator()`**`(const Args&... args) const`
    * `bool Format::`**`empty`**`() const noexcept`
    * `size_t Format::`**`fields`**`() const`
    * `U8string Format::`**`format`**`() const`

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
    U8string a = f("Alice", 42); // "Hello Alice, your number is 42.000"
    U8string b = f("Bob", 1.23); // "Hello Bob, your number is 1.230"

Use `"$$"` to insert a literal dollar sign in the format string.

* `namespace RS::Unicorn::Literals`
    * `Format` **`operator"" _fmt`**`(const char* ptr, size_t len)`

Formatting object literal.

## Formatting for specific types ##

No type-specific flags are defined for `RS::Uuid` and `RS::Version`.

### Boolean formatting ###

<!-- DEFN -->

Flag             | Letter  | Description
----             | ------  | -----------
**`fx_tf`**      |`t`      | Format as `"true"` or `"false"` (default)
**`fx_binary`**  |`b`      | Format as `"1"` or `"0"`
**`fx_yesno`**   |`y`      | Format as `"yes"` or `"no"`

### Integer formatting ###

<!-- DEFN -->

These flags apply to all integer types other than the four standard character
types.

Flag              | Letter  | Description
----              | ------  | -----------
**`fx_decimal`**  |`n`      | Format as a decimal number (default)
**`fx_binary`**   |`b`      | Format as a binary number
**`fx_hex`**      |`x`      | Format as a hexadecimal number
**`fx_roman`**    |`r`      | Format as a Roman numeral
**`fx_sign`**     |`s`      | Always show a sign
**`fx_signz`**    |`S`      | Always show a sign unless the value is zero

### Floating point formatting ###

<!-- DEFN -->

Flag              | Letter  | Description
----              | ------  | -----------
**`fx_digits`**   |`d`      | Format with a fixed number of significant figures
**`fx_exp`**      |`e`      | Format in scientific notation
**`fx_fixed`**    |`f`      | Format with a fixed number of decimal places
**`fx_general`**  |`g`      | Use the shorter of `fx_digits` or `fx_exp` (default)
**`fx_sign`**     |`s`      | Always show a sign
**`fx_signz`**    |`S`      | Always show a sign unless the value is zero
**`fx_stripz`**   |`z`      | Strip trailing zeros after the decimal point

### Character and string formatting ###

These flags apply to all character, character pointer, and standard string
types.

<!-- DEFN -->

Flag               | Letter  | Description
----               | ------  | -----------
**`fx_escape`**    |`e`      | Escape C0 and C1 control characters
**`fx_ascii`**     |`a`      | Escape all characters except printable ASCII
**`fx_quote`**     |`q`      | Quote the string, and escape C0 and C1 controls
**`fx_ascquote`**  |`Q`      | Quote the string, and escape non-ASCII characters
**`fx_decimal`**   |`n`      | Format characters as decimal numbers
**`fx_hex`**       |`x`      | Format characters as hexadecimal numbers
**`fx_hex8`**      |`u`      | Format characters as hex UTF-8 code units
**`fx_hex16`**     |`v`      | Format characters as hex UTF-16 code units

### Time and date formatting ###

<!-- DEFN -->

Formatting for `std::chrono::duration` calls `RS::format_time()`, with the
specified precision if one was supplied. Formatting for
`std::chrono::system_clock::time_point` uses ISO 8601 format by default
(`"yyyy-mm-dd hh:mm:ss"`), and respects the following flags:

Flag             | Letter  | Description
----             | ------  | -----------
**`fx_iso`**     |`t`      | Use strict ISO 8601 format with T delimiter
**`fx_common`**  |`c`      | Use the locale's standard format
**`fx_local`**   |`l`      | Use the local time zone instead of UTC
