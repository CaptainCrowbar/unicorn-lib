# [Unicorn Library](index.html): String Formatting #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/format.hpp"`

This module provides facilities for formatting various kinds of data as
Unicode strings, in a manner similar to `printf()` in C, `str.format()` in
Python, etc. Formatters are supplied for commonly used standard types
(booleans, integers, floating point, characters, strings, and time points and
durations), as well as for the `Uuid` and `Version` types from the Prion
library; user defined formatters for other types can also be supplied.

Basic formatting for range types (containers and arrays) is provided; these
will be written in comma delimited form. Pairs will be written in
`"key:value"` format. More elaborate formatting for ranges is beyond the scope
of this module and will need to be handled by the caller.

This module calls the [`unicorn/regex`](regex.html) module, which in turn
calls the PCRE library. It will only work with encodings for which the
corresponding PCRE library has been linked; see the regex module documentation
for details.

## Contents ##

[TOC]

## Basic formatting functions ##

* `template <typename T> u8string` **`format_type`**`(const T& t, uint64_t flags, int prec)`

This is the core formatting function. The details of how the flags and
precision are interpreted depend on the type being formatted. This function
can be overloaded for user defined types to provide formatting for them; the
other functions in this library will find any user defined overloads through
argument dependent lookup. The library defines specializations for the types
listed here; their behaviour is described in more detail below.

* <!-- TEXT --> `bool`
* `char`, `char16_t`, `char32_t`, `wchar_t`
* `signed char`, `unsigned char`, `short`, `unsigned short`, `int`, `unsigned`, `long`, `unsigned long`, `long long`, `unsigned long long`, `int128_t`, `uint128_t`
* `float`, `double`, `long double`
* `C*`, `const C*`, `basic_string<C>` _for the four standard character types_
* `std::chrono::duration<R, P>`, `std::chrono::system_clock::time_point`
* `Prion::Uuid`
* `Prion::Version`

* <!-- DEFN--> `template <typename C, typename T> basic_string<C>` **`format_as`**`(const T& t, uint64_t flags = 0, int prec = -1, size_t width = 0, char32_t pad = U' ')`
* `template <typename C, typename T> basic_string<C>` **`format_as`**`(const T& t, const basic_string<C>& flags)`
* `template <typename C, typename T> basic_string<C>` **`format_as`**`(const T& t, const C* flags)`

These call `format_type()` to format their first argument, converting the
resulting string to the requested type. The first version accepts the same
arguments as `format_type()`, along with a width and padding character; the
formatted string will be padded to the specified width (if it is not already
longer than that).

The other two versions of `format_as()` take a second string argument that
carries the same information as the format control arguments to
`format_type()` and the first version of `format_as()`. The flag descriptions
below indicate which letter corresponds to which flag; when defining
`format_type()` overloads for user defined types, you can use the predefined
flags or use `Prion::letter_to_mask()` to define new ones.

Example:

    constexpr uint64_t fx_alpha = letter_to_mask('A');
    constexpr uint64_t fx_omega = letter_to_mask('Z');

The following global flags are common to all types:

<!-- DEFN -->

Flag                   | Letter  | Description
----                   | ------  | -----------
**`fx_left`**          |`<`      | Left align (default)
**`fx_right`**         |`>`      | Right align
**`fx_centre`**        |`=`      | Centre align
**`fx_upper`**         |`U`      | Convert to upper case
**`fx_lower`**         |`L`      | Convert to lower case
**`fx_title`**         |`T`      | Convert to title case
**`character_units`**  |`C`      | Measure string in characters (default)
**`grapheme_units`**   |`G`      | Measure string in grapheme clusters
**`narrow_context`**   |`N`      | East Asian width, defaulting to narrow
**`wide_context`**     |`W`      | East Asian width, defaulting to wide

See [`unicorn/string`](string.html) for details of the four string measurement
flags. The alignment flags can be followed by a decimal number, which will be
interpreted as the fully padded width; optionally, a padding character can be
inserted between the alignment flag and the width:

    format_as(42, ">*5") == "***42"

If the flag string includes a number (other than one associated with the
alignment), it will be interpreted as a precision:

    `format_as(123.0, "d6") == "123.000"

## Formatter class ##

* `template <typename C> class` **`BasicFormat`**
    * `using BasicFormat::`**`char_type`** `= C`
    * `using BasicFormat::`**`string_type`** `= basic_string<C>`
    * `BasicFormat::`**`BasicFormat`**`()`
    * `explicit BasicFormat::`**`BasicFormat`**`(const string_type& format)`
    * `BasicFormat::`**`BasicFormat`**`(const BasicFormat& f)`
    * `BasicFormat::`**`BasicFormat`**`(BasicFormat&& f) noexcept`
    * `BasicFormat::`**`~BasicFormat`**`() noexcept`
    * `BasicFormat& BasicFormat::`**`operator=`**`(const BasicFormat& f)`
    * `BasicFormat& BasicFormat::`**`operator=`**`(BasicFormat&& f) noexcept`
    * `template <typename... Args> BasicFormat::string_type BasicFormat::`**`operator()`**`(const Args&... args) const`
    * `bool BasicFormat::`**`empty`**`() const noexcept`
    * `size_t BasicFormat::`**`fields`**`() const`
    * `BasicFormat::`**`string_type`** `BasicFormat::`**`format`**`() const`
* `using` **`Format`** `= BasicFormat<char>`
* `using` **`Format16`** `= BasicFormat<char16_t>`
* `using` **`Format32`** `= BasicFormat<char32_t>`
* `using` **`WideFormat`** `= BasicFormat<wchar_t>`

This class is constructed from a formatting string, which contains
placeholders into which arguments will be substituted when the formatting
object's function call operator is called.

A placeholder takes the form `$n` or `${n}`, where `n` is the 1-based index of
the corresponding argument. Optionally the field number can be followed by a
flag string, using the same syntax as `format_as()` above. The braces are only
needed if the field number or flag string is immediately followed by a letter
or digit that would be read as part of the placeholder in the absence of
braces.

Example:

    Format form("Hello $1, your number is $2f3");
    u8string a = form("Alice", 42); // a = "Hello Alice, your number is 42.000"
    u8string b = form("Bob", 1.23); // b = "Hello Bob, your number is 1.230"

Use `"$$"` to insert a literal dollar sign in the format string.

* `template <typename C> BasicFormat<C>` **`format`**`(const basic_string<C>& fmt)`
* `template <typename C> BasicFormat<C>` **`format`**`(const C* fmt)`
* `namespace Unicorn::Literals`
    * `Format` **`operator"" _fmt`**`(const char* ptr, size_t len)`
    * `Format16` **`operator"" _fmt`**`(const char16_t* ptr, size_t len)`
    * `Format32` **`operator"" _fmt`**`(const char32_t* ptr, size_t len)`
    * `WideFormat` **`operator"" _fmt`**`(const wchar_t* ptr, size_t len)`

Convenience functions and literals for construction formatting objects.

## Formatting for specific types ##

No type-specific flags are defined for `Prion::Uuid` and `Prion::Version`.

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
**`fx_roman`**    |`r`      | Format as a roman numeral
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
**`fx_prob`**     |`p`      | Use probability format (see below)
**`fx_sign`**     |`s`      | Always show a sign
**`fx_signz`**    |`S`      | Always show a sign unless the value is zero
**`fx_stripz`**   |`z`      | Strip trailing zeros after the decimal point

Probability format is similar to `fx_digits` format, but is intended for
formatting probability values between 0 and 1 (values outside this range will
be clamped to the nearest end of the range). Leading nines after the decimal
point are treated as non-significant, in the same way as leading zeros; this
means that `p` and `1-p` will be expressed to the same number of decimal
places.

    format_as(0.5, "p3") == "0.500"
    format_as(0.05, "p3") == "0.0500"
    format_as(0.95, "p3") == "0.9500"
    format_as(0.005, "p3") == "0.00500"
    format_as(0.995, "p3") == "0.99500"

### Character and string formatting ###

These flags apply to all character, character pointer, and standard string
types.

<!-- DEFN -->

Flag               | Letter  | Description
----               | ------  | -----------
**`fx_ascii`**     |`a`      | Escape all characters except printable ASCII
**`fx_escape`**    |`c`      | Escape C0 and C1 control characters
**`fx_quote`**     |`q`      | Quote the string, and escape C0 and C1 controls
**`fx_ascquote`**  |`o`      | Quote the string, and escape non-ASCII characters
**`fx_decimal`**   |`n`      | Format characters as decimal numbers
**`fx_hex`**       |`x`      | Format characters as hexadecimal numbers
**`fx_hex8`**      |`u`      | Format characters as hex UTF-8 code units
**`fx_hex16`**     |`v`      | Format characters as hex UTF-16 code units

### Time and date formatting ###

<!-- DEFN -->

Formatting for `std::chrono::duration` calls `Prion::format_time()`, with the
specified precision if one was supplied. Formatting for
`std::chrono::system_clock::time_point` uses ISO 8601 format by default
(`"yyyy-mm-dd hh:mm:ss"`), and respects the following flags:

Flag             | Letter  | Description
----             | ------  | -----------
**`fx_iso`**     |`t`      | Use ISO 8601 format with T delimiter
**`fx_common`**  |`c`      | Use the locale's standard format
**`fx_local`**   |`l`      | Use the local time zone instead of UTC
