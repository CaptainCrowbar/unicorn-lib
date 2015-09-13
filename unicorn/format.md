Title: Unicorn Library: String Formatting
CSS: style.css

# [Unicorn Library](index.html): String Formatting #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/format.hpp"` ####

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

<p class="alert">[TODO: DOCUMENTATION]</p>

## Contents ##

* [Basic formatting functions][]
* [Formatter class][]
* [Formatter literals][]
* [Formatting for specific types][]
    * [Global flags][]
    * [Boolean formatting][]
    * [Integer formatting][]
    * [Floating point formatting][]
    * [Character and string formatting][]
    * [Time and date formatting][]
    * [UUID formatting][]
    * [Version number formatting][]

## Basic formatting functions ##

* `template <typename T> class FormatType`
    * `[string type] operator()(T t, Flagset flags, int prec) const`

TODO

* `template <> class FormatType<bool>`
* `template <> class FormatType<signed char>`
* `template <> class FormatType<unsigned char>`
* `template <> class FormatType<short>`
* `template <> class FormatType<unsigned short>`
* `template <> class FormatType<int>`
* `template <> class FormatType<unsigned>`
* `template <> class FormatType<long>`
* `template <> class FormatType<unsigned long>`
* `template <> class FormatType<long long>`
* `template <> class FormatType<unsigned long long>`
* `template <> class FormatType<int128_t>`
* `template <> class FormatType<uint128_t>`
* `template <> class FormatType<float>`
* `template <> class FormatType<double>`
* `template <> class FormatType<long double>`
* `template <typename C> class FormatType<basic_string<C>>`
* `template <> class FormatType<char*>`
* `template <> class FormatType<char16_t*>`
* `template <> class FormatType<char32_t*>`
* `template <> class FormatType<wchar_t*>`
* `template <> class FormatType<const char*>`
* `template <> class FormatType<const char16_t*>`
* `template <> class FormatType<const char32_t*>`
* `template <> class FormatType<const wchar_t*>`
* `template <> class FormatType<char>`
* `template <> class FormatType<char16_t>`
* `template <> class FormatType<char32_t>`
* `template <> class FormatType<wchar_t>`
* `template <> class FormatType<std::chrono::system_clock::time_point>`
* `template <typename R, typename P> class FormatType<std::chrono::duration<R, P>>`
* `template <> class FormatType<Uuid>`
* `template <> class FormatType<Version>`

TODO

* `template <typename T, typename C> void format_type(const T& t, basic_string<C>& dst, Flagset flags = {}, int prec = -1, size_t width = 0, char32_t pad = U' ')`
* `template <typename T, typename C> void format_type(const T& t, basic_string<C>& dst, const basic_string<C>& flags)`
* `template <typename T, typename C> void format_type(const T& t, basic_string<C>& dst, const C* flags)`
* `template <typename C, typename T> basic_string<C> format_as(const T& t, Flagset flags = {}, int prec = -1, size_t width = 0, char32_t pad = U' ')`
* `template <typename C, typename T> basic_string<C> format_as(const T& t, const basic_string<C>& flags)`
* `template <typename C, typename T> basic_string<C> format_as(const T& t, const C* flags)`

TODO

## Formatter class ##

* `template <typename C> class BasicFormat`
    * `using BasicFormat::char_type = C`
    * `using BasicFormat::string_type = basic_string<C>`
    * `BasicFormat::BasicFormat()`
    * `explicit BasicFormat::BasicFormat(const string_type& format)`
    * `BasicFormat::BasicFormat(const BasicFormat& f)`
    * `BasicFormat::BasicFormat(BasicFormat&& f) noexcept`
    * `BasicFormat::~BasicFormat() noexcept`
    * `BasicFormat& BasicFormat::operator=(const BasicFormat& f)`
    * `BasicFormat& BasicFormat::operator=(BasicFormat&& f) noexcept`
    * `template <typename... Args> BasicFormat::string_type BasicFormat::operator()(const Args&... args) const`
    * `bool BasicFormat::empty() const noexcept`
    * `size_t BasicFormat::fields() const`
    * `BasicFormat::string_type BasicFormat::format() const`
* `using Format = BasicFormat<char>`
* `using Format16 = BasicFormat<char16_t>`
* `using Format32 = BasicFormat<char32_t>`
* `using WideFormat = BasicFormat<wchar_t>`

TODO

* `template <typename C> BasicFormat<C> format(const basic_string<C>& fmt)`
* `template <typename C> BasicFormat<C> format(const C* fmt)`

TODO

## Formatter literals ##

* `namespace Literals`
    * `Format operator"" _fmt(const char* ptr, size_t len)`
    * `Format16 operator"" _fmt(const char16_t* ptr, size_t len)`
    * `Format32 operator"" _fmt(const char32_t* ptr, size_t len)`
    * `WideFormat operator"" _fmt(const wchar_t* ptr, size_t len)`

TODO

## Formatting for specific types ##

### Global flags ###

Bitmask            | Letter  | Description
-------            | ------  | -----------
`fx_left`          | `<`     | Left align (default)
`fx_right`         | `>`     | Right align
`fx_centre`        | `=`     | Centre align
`fx_upper`         | `U`     | Convert to upper case
`fx_lower`         | `L`     | Convert to lower case
`fx_title`         | `T`     | Convert to title case
`character_units`  | `C`     | Measure string in characters (default)
`grapheme_units`   | `G`     | Measure string in grapheme clusters
`narrow_context`   | `N`     | East Asian width, defaulting to narrow
`wide_context`     | `W`     | East Asian width, defaulting to wide

TODO

### Boolean formatting ###

Bitmask      | Letter  | Description
-------      | ------  | -----------
`fx_tf`      | `t`     | Write as true or false (default)
`fx_binary`  | `b`     | Write as 1 or 0
`fx_yesno`   | `y`     | Write as yes or no

TODO

### Integer formatting ###

Bitmask       | Letter  | Description
-------       | ------  | -----------
`fx_decimal`  | `n`     | Decimal number (default)
`fx_binary`   | `b`     | Binary number
`fx_hex`      | `x`     | Hexadecimal number
`fx_roman`    | `r`     | Roman numerals
`fx_sign`     | `s`     | Always show a sign
`fx_signz`    | `S`     | Always show a sign unless zero

TODO

### Floating point formatting ###

Bitmask       | Letter  | Description
-------       | ------  | -----------
`fx_digits`   | `d`     | Fixed significant figures
`fx_exp`      | `e`     | Scientific notation
`fx_fixed`    | `f`     | Fixed point notation
`fx_general`  | `g`     | Use the shorter of d or e (default)
`fx_prob`     | `p`     | Probability format
`fx_sign`     | `s`     | Always show a sign
`fx_signz`    | `S`     | Always show a sign unless zero
`fx_stripz`   | `z`     | Strip trailing zeros

TODO

### Character and string formatting ###

Bitmask        | Letter  | Description
-------        | ------  | -----------
`fx_ascii`     | `a`     | Escape if not printable ASCII
`fx_escape`    | `c`     | Escape C0/C1 controls
`fx_quote`     | `q`     | Quote string, escape C0/C1
`fx_ascquote`  | `o`     | Quote string, escape non-ASCII
`fx_decimal`   | `n`     | Decimal number
`fx_hex`       | `x`     | Hexadecimal number
`fx_hex8`      | `u`     | Hex UTF-8 bytes
`fx_hex16`     | `v`     | Hex UTF-16 code units

TODO

### Time and date formatting ###

Bitmask      | Letter  | Description
-------      | ------  | -----------
`fx_iso`     | `t`     | ISO 8601 with T delimiter
`fx_common`  | `c`     | Local standard format
`fx_local`   | `l`     | Local time zone

TODO

### UUID formatting ###

No type-specific flags are defined.

### Version number formatting ###

No type-specific flags are defined.
