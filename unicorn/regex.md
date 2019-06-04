# [Unicorn Library](index.html): Regular Expressions #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/regex.hpp"`

## Contents ##

[TOC]

## Introduction ##

This module provides Unicode-aware regular expressions, and related classes
and functions. It calls the widely available [PCRE2](http://www.pcre.org/)
(Perl Compatible Regular Expressions) library. The `Regex` class uses the
8-bit version of the PCRE library, which must have been built with the
optional UTF-8 support enabled (PCRE2 can also be built as 16 and 32-bit
libraries, but these are not used here).

Refer to the PCRE2 documentation for details of the regular expression syntax.

The `Regex` class supports both UTF-8 and byte oriented matching. Byte regexes
(selected with the `Regex::byte` flag) simply treat a string as a sequence of
arbitrary bytes, with no assumptions about content encoding, and will work
with non-Unicode strings. The `\xHH` escape code (where H is a hexadecimal
digit) always matches a single byte even if the value is greater than `\x7f`
(in a UTF-8 regex this would match a multibyte encoded character); the
`\x{hex}` escape code can still be used, but it will be treated as a syntax
error if the value is greater than `\x{ff}`.

### `Unicorn::Regex` vs `std::regex` ###

It would have been convenient to use standard C++11 regular expressions here,
in the same way as the standard string classes have been used instead of
creating a new custom string class. Unfortunately, this turns out to be
impractical; for several reasons, standard regular expressions are inadequate
for use with Unicode strings.

The most obvious reason is that standard C++ regexes are not actually required
to support Unicode strings at all. Unlike `std::basic_string`, for which
specializations for 8, 16, and 32 bit characters are required to exist, only
two specializations of `std::basic_regex` are mandated, for `char` (the
system's native multibyte encoding, which may or may not be UTF-8, but see
below for a caveat on this) and `wchar_t` (the system's wide character
encoding, which can reasonably be expected to be either UTF-16 or UTF-32, but
which one varies with the OS). In short, standard regexes can only be relied
on to support one of the three UTF encodings, and we don't know which one.

Strictly speaking, not even that is required; the C++ standard does not
actually require the wide character encoding to be UTF-16 or 32. It is on all
systems I know of, though, and my code explicitly does not support systems on
which it is not one of those.

An implementation is allowed to instantiate `std::basic_regex` for other
character types, but in practise most do not, and in any case even an
implementation that supplied specializations for all four character types
would still not be reliably usable with UTF-8 (since the 8-bit encoding is not
guaranteed to be UTF-8).

The second problem with standard regexes is that, by the rules of the C++
standard, they _cannot_ properly support UTF-8 strings. The regex grammar
(based on that of JavaScript/EcmaScript, with a few changes) matches on an
element by element basis; a "character", as far as regex matching is
concerned, is a single code unit, not a Unicode scalar value that may be
represented by more than one code unit. This still allows literal matching of
multi-unit UTF-8 characters (the encoding will be the same in the regex and
the subject string, so they will match unit for unit), but makes it impossible
to match multi-unit characters to non-literal regex elements; for example,
even if the system encoding is UTF-8, `std::regex(".")` will still not match
`"€"` (which is three bytes long). For the same reason, it is impossible to
specify a character range that includes multibyte characters; for example,
`std::regex("[à-ÿ]")` will not do what you probably expected, regardless of
encoding.

Finally, standard regexes don't support the `\p{...}` and `\P{...}` character
classes, which match on Unicode properties. This may be a minor obstacle
compared to either of the above showstoppers, but even by itself it would be a
serious handicap in a library dedicated to Unicode support.

For all of the above reasons, I felt I had no choice but to abandon standard
C++ regexes, and base my regular expressions on PCRE2 instead.

## Regex class ##

### Regex flags ###

<!-- DEFN -->

| Flag                            | Description                                                         |
| ----                            | -----------                                                         |
|                                 | **Pattern compilation flags**                                       |
| `Regex::`**`ascii`**            | Character classes match only ASCII                                  |
| `Regex::`**`byte`**             | Treat the pattern and subject string as bytes instead of UTF-8      |
| `Regex::`**`crlf`**             | Lines end with `CR+LF` instead of just `LF`                         |
| `Regex::`**`dollar_end`**       | `$` does not match line breaks at the end of the subject string     |
| `Regex::`**`dot_all`**          | `.` matches all characters including line breaks                    |
| `Regex::`**`extended`**         | Free-form layout, ignore whitespace and comments starting with `#`  |
| `Regex::`**`first_line`**       | The match must start in the first line of the subject string        |
| `Regex::`**`icase`**            | Matching is case insensitive                                        |
| `Regex::`**`line`**             | Only match a complete line                                          |
| `Regex::`**`multiline`**        | `^` and `$` match the beginning and end of every line               |
| `Regex::`**`no_capture`**       | Parenthesized groups are not captured (named captures still work)   |
| `Regex::`**`optimize`**         | Optimize the regex using PCRE's JIT compiler                        |
| `Regex::`**`word`**             | Only match a complete word                                          |
|                                 | **Pattern matching flags**                                          |
| `Regex::`**`anchor`**           | Anchor the match at the start of the subject string                 |
| `Regex::`**`full`**             | Match the complete subject string                                   |
| `Regex::`**`not_empty`**        | Do not match an empty string                                        |
| `Regex::`**`not_empty_start`**  | Do not match an empty string at the start of the subject string     |
| `Regex::`**`not_line`**         | Do not match `^` or `$` at the start or end of the subject string   |
| `Regex::`**`partial_hard`**     | Enable hard partial matching (prefer partial match to full match)   |
| `Regex::`**`partial_soft`**     | Enable soft partial matching (prefer full match to partial match)   |
|                                 | **Other flags**                                                     |
| `Regex::`**`global`**           | Replace all matches instead of only the first                       |
| `Regex::`**`no_utf_check`**     | Assume the pattern or subject string is valid UTF-8                 |

These are bitmasks of the `Regex::flag_type` type. Wherever a regex function
takes a `flags` argument, the value is expected to be a bitwise-or combination
of these.

The pattern compilation flags must be supplied when the `Regex` object is
constructed, while the pattern matching flags can be supplied later, in a call
to a matching or substitution function. However, any flag can be supplied at
construction time; matching flags will be saved and applied at match time.
Flags supplied at construction time cannot be overridden at match time.

Notes on specific flags:

* The `ascii` flag causes backslash character classes (`\[BbDdRSsWw]`), and
some Posix character classes, to match only ASCII characters instead of
matching Unicode properties. This has no effect when combined with the `byte`
flag, since only ASCII characters are matched in byte mode anyway.
* The `optimize` flag will enable PCRE2's JIT compiler if possible, but this
can be silently blocked under some circumstances (see the PCRE2 documentation
for details). In particular, specifying any of the partial matching flags at
match time will disable the optimizer if the same flag was not present at
compile time.
* The `global` flag causes all matches to be replaced in `Regex::replace()`
and `Regex::transform::operator()`. It has no effect in any other context.
* The `no_utf_check` flag has a slightly different effect depending on where
it is used: it skips the normal UTF-8 validity check for the pattern string
when used at compile time, for the subject string at match time, and for the
subject and replacement strings at replacement time. In all cases, behaviour
is undefined if the string is not valid UTF-8. This flag has no effect in byte
mode.

### Member types ###

* `class Regex::`**`error`**
* `class Regex::`**`match`**
* `class Regex::`**`transform`**

These are individually described below.

* `using Regex::`**`flag_type`** `= [unsigned integer type]`

The type used for bitmask flags.

* `class Regex::`**`match_iterator`**
    * _Const forward iterator_
    * _Value type is_ `Regex::match`
* `using Regex::`**`match_range`** `= Irange<match_iterator>`

An iterator over the non-overlapping matches found within a subject string for
a given regex, returned by `Regex::grep()`.

* `class Regex::`**`split_iterator`**
    * _Const forward iterator_
    * _Value type is_ `std::string_view`
* `using Regex::`**`split_range`** `= Irange<split_iterator>`

An iterator over the substrings between matches for a given regex, returned by
`Regex::split()`.

* `struct Regex::`**`partition_type`**
    * `std::string_view partition_type::`**`left`**
    * `std::string_view partition_type::`**`mid`**
    * `std::string_view partition_type::`**`right`**

The result type returned from `Regex::partition()`.

### Member functions ###

* `Regex::`**`Regex`**`()`
* `explicit Regex::`**`Regex`**`(std::string_view pattern, flag_type flags = 0)`
* `Regex::`**`Regex`**`(const Regex& r)`
* `Regex::`**`Regex`**`(Regex&& r) noexcept`
* `Regex::`**`~Regex`**`() noexcept`
* `Regex& Regex::`**`operator=`**`(const Regex& r)`
* `Regex& Regex::`**`operator=`**`(Regex&& r) noexcept`

Life cycle operations. The default constructor creates a null regex that will
never match anything. The second constructor takes the pattern string and
optionally a flags bitmask, and will throw `Regex::error` if the pattern is
invalid or an invalid combination of flags is passed.

* `std::string Regex::`**`pattern`**`() const`
* `Regex::flag_type Regex::`**`flags`**`() const noexcept`
* `std::string` **`to_str`**`(const Regex& re)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Regex& re)`

Return the pattern string or flags supplied to the constructor. The `to_str()`
function and the output operator return the same string as `pattern()`. If the
`Regex` object was default constructed, `pattern()` is empty and `flags()` is
zero, but this is not equivalent to a regex constructed from an empty string.

* `bool Regex::`**`empty`**`() const noexcept`
* `bool Regex::`**`is_null`**`() const noexcept`

The `is_null()` function is true if the regex was default constructed;
`empty()` is true if `is_null()` is true or the pattern is an empty string.

* `size_t Regex::`**`groups`**`() const noexcept`

Returns the number of capture groups in the pattern, including the complete
match (`$0`).

* `size_t Regex::`**`named`**`(std::string_view name) const`

Returns the capture group number corresponding to the given group name, or
`npos` if the name was not present.

* `Regex::match Regex::`**`search`**`(std::string_view str, size_t pos = 0, flag_type flags = 0) const`
* `Regex::match Regex::`**`search`**`(const Utf8Iterator& start, flag_type flags = 0) const`
* `Regex::match Regex::`**`operator()`**`(std::string_view str, size_t pos = 0, flag_type flags = 0) const`
* `Regex::match Regex::`**`operator()`**`(const Utf8Iterator& start, flag_type flags = 0) const`

This is the basic regex matching function (the operator is a synonym for the
`search()` function). When a non-zero offset is passed, or an iterator that
does not point to the beginning of the string, the search begins at the
specified point in the string, but the text preceding it will still be taken
into account in lookbehind assertions. Matching will always fail if the
starting position is beyond the end of the subject string (but may succeed if
it is at the end and the pattern can match an empty substring). This will
throw `Regex::error` if the flags are inconsistent, if the starting offset is
not at a character boundary in a UTF-8 string (in the absence of either the
`byte` or `no_utf_check` flags), if the UTF iterator version is called on a
byte-mode regex, or if a very complicated matching task exceeds PCRE2's
internal limits.

* `size_t Regex::`**`count`**`(std::string_view str, size_t pos = 0, flag_type flags = 0) const`
* `size_t Regex::`**`count`**`(const Utf8Iterator& start, flag_type flags = 0) const`

Returns the number of non-overlapping matches in the subject string.

* `Regex::match_range Regex::`**`grep`**`(std::string_view str, size_t pos = 0, flag_type flags = 0) const`
* `Regex::match_range Regex::`**`grep`**`(const Utf8Iterator& start, flag_type flags = 0) const`

Returns a pair of iterators over all matches in the subject string, optionally
starting at a given byte offset. It can throw the same exceptions as
`search()`.

* `Regex::partition_type Regex::`**`partition`**`(std::string_view str, size_t pos = 0, flag_type flags = 0) const`

Finds the first match in the string, and returns three string views covering
the part of the subject string before the match (including anything before the
starting offset), the match itself, and the part after the match. If there is
no match, the first field in the return value will be the full subject string,
and the other two will be empty (but will point to the end of the string
instead of being null). This can throw the same exceptions as `search()`.

* `std::string Regex::`**`replace`**`(std::string_view str, std::string_view fmt, size_t pos = 0, flag_type flags = 0) const`
* `void Regex::`**`replace_in`**`(std::string& str, std::string_view fmt, size_t pos = 0, flag_type flags = 0) const`

Replace the first match (if any) with the given format string, following
PCRE2's full set of replacement rules. The `replace()` function returns the
modified string, while `replace_in()` modifies the string in place. If the
`global` flag is used, this will replace all matches instead of only the
first. This can throw the same exceptions as `search()`.

* `Regex::split_range Regex::`**`split`**`(std::string_view str, size_t pos = 0, flag_type flags = 0) const`

Returns a pair of iterators over the parts of the string between matches. This
can throw the same exceptions as `search()`.

* `static Version Regex::`**`compile_version`**`() noexcept` _- PCRE2 version used when this library was compiled_
* `static Version Regex::`**`runtime_version`**`() noexcept` _- Current PCRE2 version available at runtime_
* `static Version Regex::`**`unicode_version`**`() noexcept` _- Latest Unicode version supported by PCRE2_

Version number queries. Because the PCRE2 library is built separately, the
Unicode version reported here will not necessarily be the same as the version
supported by the rest of my library.

* `static std::string Regex::`**`escape`**`(std::string_view str)`

Inserts escape characters (backslashes) where necessary to return a pattern
that will match the argument string literally.

### Exceptions ###

* `class Regex::`**`error`**`: public std::runtime_error`
    * `explicit error::`**`error`**`(int code)`
    * `int error::`**`code`**`() const noexcept`
    * `static std::string error::`**`translate`**`(int code)`

This is thrown from a regex constructor or matching function when the
underlying PCRE2 call reports an error. The `translate()` function converts a
PCRE2 error code to a string.

### Regex match class ###

* `class Regex::`**`match`**

The regex matching functions return a `Regex::match` object to report the
result of a matching attempt.

* `match::`**`match`**`() noexcept`
* `match::`**`match`**`(const match& m) noexcept`
* `match::`**`match`**`(match&& m) noexcept`
* `match::`**`~match`**`() noexcept`
* `match& match::`**`operator=`**`(const match& m) noexcept`
* `match& match::`**`operator=`**`(match&& m) noexcept`

Life cycle operations. The default constructor returns the equivalent of a
failed match. Normally a match object will not be constructed explicitly by
users (apart from default construction), but returned from matching functions
in the `Regex` class.

* `bool match::`**`matched`**`(size_t i = 0) const noexcept`
* `bool match::`**`full`**`() const noexcept`
* `bool match::`**`partial`**`() const noexcept`
* `explicit match::`**`operator bool`**`() const noexcept`

These report whether the match was successful. If a non-zero index is
supplied, the result indicates whether a specific capture group was matched.
The `full()` and `partial()` functions can be used to distinguish between full
and partial matches; `matched()` is equivalent to `full() || partial()`. If
neither of the partial matching flags was used, `partial()` is always false
and `matched()` is equivalent to `full()`.

* `const char* match::`**`begin`**`(size_t i = 0) const noexcept`
* `const char* match::`**`end`**`(size_t i = 0) const noexcept`
* `size_t match::`**`offset`**`(size_t i = 0) const noexcept`
* `size_t match::`**`endpos`**`(size_t i = 0) const noexcept`
* `size_t match::`**`count`**`(size_t i = 0) const noexcept`

These report the position of the match, or of a numbered capture group, in the
subject string. The `offset()` and `endpos()` functions return the start and
end positions; `count()` returns the length of the match (offsets are measured
in code units, not characters, from the start of the subject string, not the
position from which matching was started). If the match failed, or if a
specific capture group was not matched, `begin()` and `end()` return null
pointers, `offset()` and `endpos()` return `npos`, and `count()` returns zero.

* `size_t match::`**`first`**`() const noexcept`
* `size_t match::`**`last`**`() const noexcept`

These return the indices of the first and last non-empty capture group (not
counting the complete match), or `npos` if the match failed or there are no
non-empty captures.

* `std::string_view match::`**`mark`**`() const noexcept`

Returns the name of the most recent named `(*COMMIT)`, `(*MARK)`, `(*PRUNE)`,
or `(*THEN)` tag in the pattern, or an empty string if no suitable tag was
found.

* `std::string_view match::`**`str`**`(size_t i = 0) const noexcept`
* `std::string_view match::`**`operator[]`**`(size_t i) const noexcept`
* `match::`**`operator std::string_view`**`() const noexcept`

These return the matching substring, for the complete match or a specific
capture group. All of these will return an empty string view if the match
failed, or the capture group was not matched (note that an empty string can
also be the legitimate result of a successful match).

* `bool match::`**`matched`**`(std::string_view name) const`
* `const char* match::`**`begin`**`(std::string_view name) const`
* `const char* match::`**`end`**`(std::string_view name) const`
* `size_t match::`**`offset`**`(std::string_view name) const`
* `size_t match::`**`endpos`**`(std::string_view name) const`
* `size_t match::`**`count`**`(std::string_view name) const`
* `std::string_view match::`**`str`**`(std::string_view name) const`
* `std::string_view match::`**`operator[]`**`(std::string_view name) const`

These return the same result as the functions that take a numeric index
argument, except that the capture group is identified by name instead of
number. If the named group does not exist in the pattern, the result is the
same as if the match failed.

### Regex transform class ###

* `class Regex::`**`transform`**

The `Regex::transform` class contains both a regex and a format string. It
provides operations equivalent to the `Regex::replace[_in]()` functions, but
may be more efficient if the same formatting operation is going to be applied
many times.

* `transform::`**`transform`**`() noexcept`
* `transform::`**`transform`**`(const Regex& pattern, std::string_view fmt, flag_type flags = 0)`
* `transform::`**`transform`**`(std::string_view pattern, std::string_view fmt, flag_type flags = 0)`
* `transform::`**`transform`**`(const transform& t)`
* `transform::`**`transform`**`(transform&& t) noexcept`
* `transform::`**`~transform`**`() noexcept`
* `transform& transform::`**`operator=`**`(const transform& t)`
* `transform& transform::`**`operator=`**`(transform&& t) noexcept`

Life cycle functions. The object is constructed from a regex (supplied either
as a precompiled regex or a pattern string), a format string, and optionally a
flag bitmask. In the second constructor, only matching and replacement flags
can be supplied; the third constructor can also take regex compilation flags,
and can throw the same exceptions as the corresponding regex constructor.

* `std::string transform::`**`pattern`**`() const`
* `std::string transform::`**`format`**`() const`
* `Regex::flag_type transform::`**`flags`**`() const noexcept`

These return the construction parameters.

* `std::string transform::`**`replace`**`(std::string_view str, size_t pos = 0, flag_type flags = 0) const`
* `void transform::`**`replace_in`**`(std::string& str, size_t pos = 0, flag_type flags = 0) const`
* `std::string transform::`**`operator()`**`(std::string_view str, size_t pos = 0, flag_type flags = 0) const`

The replacement functions, which perform the equivalent of
`Regex::replace[_in]()`. The function call operator is equivalent to
`replace()`.
