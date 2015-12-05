Title: Unicorn Library: Regular Expressions
CSS: style.css

# [Unicorn Library](index.html): Regular Expressions #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/regex.hpp"` ####

This module provides Unicode-aware regular expressions, and related classes
and functions. It calls the widely available [PCRE](http://www.pcre.org/)
(Perl Compatible Regular Expressions) library. Refer to the PCRE documentation
for details of the regular expression syntax.

## Contents ##

* [Introduction][]
* [Unicorn::Regex vs std::regex][]
* [Regex options][]
* [Formatting syntax][]
* [Supporting types][]
* [Regular expression class][]
* [Regex match class][]
* [Regex formatting class][]
* [Regex iterator classes][]
* [Utility functions][]
* [Version information][]

## Introduction ##

The PCRE library can be built in three different forms: `libpcre`,
`libpcre16`, and `libpcre32`, supporting UTF-8, UTF-16, and UTF-32 strings and
regular expressions respectively. Ideally, you should have all three versions
available, and link with all of them, to make Unicorn regular expressions work
with all three Unicode encodings. You will always need the 8-bit PCRE library;
depending on which of the other two you have or want to use, define
`UNICORN_PCRE16` and/or `UNICORN_PCRE32` when building Unicorn, to indicate
which ones are available (these are only needed when building Unicorn, not
when building code that uses it, as long as you are careful not to try to use
the missing regex types). Wide character (`wstring`) regexes are built if the
corresponding UTF build of PCRE is available (16 or 32 bits, depending on the
size of `wchar_t`).

Some other modules in the Unicorn library ([`unicorn/format`](format.html) and
[`unicorn/lexer`](lexer.html)) call the regex library to handle pattern
matching in different UTF encodings, and will only work with encodings for
which the corresponding PCRE library has been linked. (A few other modules
also use regexes internally; these require only UTF-8 support, which is always
available.)

In addition to the four UTF-based regex classes, this module also supports
byte oriented regexes, which simply treat a `std::`**`string`** as a sequence of
arbitrary bytes, with no assumptions about content encoding. Byte regexes work
the same as UTF-8 regexes as far as possible, except that characters in the
regex are matched against individual bytes instead of encoded characters. The
`\xHH` escape code (where H is a hexadecimal digit) always matches a single
byte even if the value is greater than `\x7f` (in a UTF-8 regex this would
match a multibyte encoded character); the `\x{hex}` escape code can still be
used, but it will be treated as a syntax error if the value is greater than
`\x{ff}`.

## Unicorn::Regex vs std::regex ##

It would have been convenient to use standard C++11 regular expressions in
Unicorn, in the same way as the standard string classes have been used instead
of creating a new custom string class specific to Unicorn. Unfortunately, this
turns out to be impractical; for several reasons, standard regular expressions
are inadequate for use with generalized Unicode strings.

The most obvious reason is that standard C++ regexes are not actually required
to support Unicode strings at all. Unlike `std::`**`basic_string`**, for which
specializations for 8, 16, and 32 bit characters are required to exist, only
two specializations of `std::`**`basic_regex`** are mandated, for `char` (the
system's native multibyte encoding, which may or may not be UTF-8, but see
below for a caveat on this) and `wchar_t` (the system's wide character
encoding, which can reasonably be expected to be either UTF-16 or UTF-32, but
which one varies with the OS). In short, standard regexes can only be relied
on to support one of the three UTF encodings, and we don't know which one.

(Strictly speaking, not even that is required; the C++ standard does not
actually require the wide character encoding to be UTF-16 or 32. It is on all
systems I know of, though, and the Unicorn library explicitly does not support
systems on which it is not one of those.)

An implementation is allowed to instantiate `std::`**`basic_regex`** for other
character types, but in practise most do not, and in any case even an
implementation that supplied specializations for all four character types
would still not be reliably usable with UTF-8 (since the plain `char` encoding
is not guaranteed to be UTF-8).

The second problem with standard regexes is that, by the rules of the C++
standard, they _cannot_ properly support UTF-8 or 16 strings. The regex
grammar (based on that of JavaScript/EcmaScript, with a few changes) matches
on an element by element basis; a "character", as far as regex matching is
concerned, is a single code unit, not a Unicode scalar value (which may be
represented by more than one code unit in UTF-8/16). This still allows literal
matching of multi-unit UTF-8/16 characters (the encoding will be the same in
the regex and the subject string, so they will match unit for unit), but makes
it impossible to match multi-unit characters to non-literal regex elements;
for example, `std::`**`regex`**`(".")` will not match `u8"€"` (even if the system
encoding is UTF-8). For the same reason, it is impossible to specify a
character range that includes multibyte characters (e.g.
`std::`**`regex`**`(u8"[À-ÿ]")` will not do what you probably expected).

Finally, standard regexes don't support the `\p{...}` and `\P{...}` character
classes, which match on Unicode properties. This may be a minor obstacle
compared to either of the above showstoppers, but even by itself it would be a
serious handicap in a library dedicated to Unicode support.

For all of the above reasons, I felt I had no choice but to abandon standard
C++ regexes, and base Unicorn's regular expressions on the widely used PCRE
library instead.

## Regex options ##

Flag                  | Description                                                                    | PCRE equivalent
----                  | -----------                                                                    | ---------------
`rx_caseless`         | Matching is case insensitive                                                   | `PCRE_CASELESS`
`rx_dfa`              | Use the alternative DFA matching algorithm                                     | `pcre_dfa_exec()`
`rx_dollarnewline`    | `$` may match line breaks preceding the end of the string                      | `~PCRE_DOLLAR_ENDONLY`
`rx_dotinline`        | `.` does not match line breaks                                                 | `~PCRE_DOTALL`
`rx_extended`         | Free-form mode; ignore whitespace and comments marked with `#`                 | `PCRE_EXTENDED`
`rx_firstline`        | Any match must start in the first line of the subject string                   | `PCRE_FIRSTLINE`
`rx_multiline`        | Multiline mode; `^` and `$` match the beginning and end of each line           | `PCRE_MULTILINE`
`rx_newlineanycrlf`   | Any of CR, LF, or CR+LF is recognised as a line break                          | `PCRE_NEWLINE_ANYCRLF`
`rx_newlinecr`        | Only CR is recognised as a line break                                          | `PCRE_NEWLINE_CR`
`rx_newlinecrlf`      | Only CR+LF is recognised as a line break                                       | `PCRE_NEWLINE_CRLF`
`rx_newlinelf`        | Only LF is recognised as a line break                                          | `PCRE_NEWLINE_LF`
`rx_noautocapture`    | Parentheses do not automatically capture; only named captures are recorded     | `PCRE_NO_AUTO_CAPTURE`
`rx_nostartoptimize`  | Disable some optimizations that affect `(*COMMIT)` and `(*MARK)` handling      | `PCRE_NO_START_OPTIMIZE`
`rx_notbol`           | Do not match `^` at the start of the subject string                            | `PCRE_NOTBOL`
`rx_notempty`         | Do not match an empty string                                                   | `PCRE_NOTEMPTY`
`rx_notemptyatstart`  | Do not match an empty string at the start of the subject string                | `PCRE_NOTEMPTY_ATSTART`
`rx_noteol`           | Do not match `$` at the end of the subject string                              | `PCRE_NOTEOL`
`rx_noutfcheck`       | Skip UTF validity checks                                                       | `PCRE_NO_UTF{8,16,32}_CHECK`
`rx_optimize`         | Optimize the regex using PCRE's JIT compiler                                   | `PCRE_STUDY_JIT_COMPILE`
`rx_partialhard`      | Hard partial matching; prefer a partial match to a full match                  | `PCRE_PARTIAL_HARD`
`rx_partialsoft`      | Soft partial matching; prefer a full match to a partial match                  | `PCRE_PARTIAL_SOFT`
`rx_prefershort`      | Quantifiers are non-greedy in NFA mode; prefer shorter matches in DFA mode     | `PCRE_UNGREEDY,PCRE_DFA_SHORTEST`
`rx_ucp`              | Backslash-escape character sets use Unicode properties, instead of just ASCII  | `PCRE_UCP`

Flags controlling regular expression matching behaviour. Most of these
correspond directly to PCRE flags, but note that all flags must be specified
when the regex is constructed (unlike PCRE, where some flags can be set at
execution time).

Note that two of the flags (`rx_dollarnewline` and `rx_dotinline`) have the
reverse sense to the corresponding PCRE flags (`PCRE_DOLLAR_ENDONLY` and
`PCRE_DOTALL`, respectively). This is simply because I felt that the reversed
state was the more natural default in these cases.

The four line breaking flags (`rx_newlineanycrlf`, `rx_newlinecr`,
`rx_newlinecrlf`, and `rx_newlinelf`) also affect the behaviour of the `\R`
escape code, which matches any of CR, LF, or CR+LF if any of these flags are
set (this corresponds to the `PCRE_BSR_ANYCRLF` flag). The default behaviour,
if none of these is set, recognises any Unicode line break (LF, VT, FF, CR,
CR+LF, `U+0085 NEXT LINE`, `U+2028 LINE SEPARATOR`, and `U+2029 PARAGRAPH
SEPARATOR`; the last three are not recognised in byte mode), corresponding to
the `PCRE_NEWLINE_ANY` and `PCRE_BSR_UNICODE` flags.

All regex constructors, and any functions that take a pattern and flags and
implicitly construct a regex, will throw `std::`**`invalid_argument`** if the flags
supplied are inconsistent:

* At most one of `rx_newlineanycrlf`, `rx_newlinecr`, `rx_newlinecrlf`, and `rx_newlinelf` may be used.
* `rx_notempty` and `rx_notemptyatstart` may not be combined.
* `rx_partialhard` and `rx_partialsoft` may not be combined.
* `rx_noutfcheck` and `rx_ucp` may not be used with a byte mode regex.

If you use the `rx_noutfcheck` flag, be careful about sanitizing your strings:
**behaviour is undefined** if this flag is present and any regex pattern,
subject string, or format string is not valid Unicode.

## Formatting syntax ##

Formatting strings are used in the `format()` and `extract()` methods of
`BasicRegex` and `BasicRegexFormat`, to generate a modified string by using a
regex to match substrings in the original subject string, and then replacing
each matching substring with a new one generated from the format string.

Most characters in a format string are taken literally. If a format string
does not contain any `$` or `\` escape characters, each match will simply be
replaced by the format string without further processing.

The following escape codes are recognised in a format string:

Code                              | Description
----                              | -----------
`$0`, `$&`                        | The complete match
`$number`, `${number}`, `\digit`  | Capture group, identified by number
`$name`, `${name}`                | Capture group, identified by name
`$-`                              | The first non-empty capture group
`$+`                              | The last non-empty capture group
`$<`                              | The text between the previous match and this one
`$>`                              | The text between this match and the next one
`$[`, `$`\`                       | The text before the current match
`$]`, `$'`                        | The text after the current match
`$_`                              | The complete subject string
`\xHH`, `\x{HHH...}`              | Unicode character, identified by hexadecimal code point
`\0`                              | Null character (`\x00`)
`\e`                              | Escape character (`\x1b`)
`\l`                              | Convert the next character to lower case
`\u`                              | Convert the next character to upper case
`\L...\E`                         | Convert the delimited text to lower case
`\T...\E`                         | Convert the delimited text to title case
`\U...\E`                         | Convert the delimited text to upper case
`\Q...\E`                         | Copy the delimited text literally, ignoring all escape codes except `\E`
`$$`, `\$`                        | Literal dollar sign
`$\`, `\\`                        | Literal backslash

Braces are only needed around a capture group number or name prefixed with `$`
if it is immediately followed by a literal digit or letter that would
otherwise be interpreted as part of the group number or name, or, for named
groups, if the name contains characters that are not alphanumeric. In the
`\digit` form, the group number must be a single digit from 1 to 9. The `$-`
and `$+` codes will be replaced with empty strings if there are no non-empty
captures.

The `$<`, `$>`, `$[`, `$]`, and `$_` codes are mostly useful with the
`extract()` method rather than `format()`, since `format()` copies the
unmatched parts of the subject string anyway. If this is the first match in
the subject string, `$<` starts at the beginning of the string; if this is the
last match, `$>` runs to the end of the string. If there is only one match,
`$<` and `$>` are the same as `$[` and `$]`.

If the number of matches is limited (by setting the `n` argument in the
`extract()` or `format()` functions), unhandled matches are not counted; `$>`
runs from the end of the last handled match to the end of the subject string,
regardless of whether or not there would have been any more matches if `n` had
been larger.

If a `$` or `\` escape prefix is followed by an unrecognised second character,
the escape character is discarded and the second character is copied literally
into the output string.

When the case conversion codes (`\l`, `\u`, `\L`, `\T`, and `\U`) are used
with byte mode regexes, only ASCII characters will be converted.

If the format string contains `\x{HHH...}` escape codes, an `EncodingError`
exception will be thrown if the hexadecimal number is not a valid Unicode
scalar value or, for byte mode regexes, if it is greater than `0xff`.

## Supporting types ##

* `class` **`RegexError`**`: public std::`**`runtime_error`**
    * `RegexError::`**`RegexError`**`(const u8string& pattern, int error, const u8string& message)`
    * `const char* RegexError::`**`pattern`**`() const noexcept`
    * `int RegexError::`**`error`**`() const noexcept`
    * `const char* RegexError::`**`message`**`() const noexcept`

This is thrown from a regex constructor or matching function when the
underlying PCRE call reports an error.

## Regular expression class ##

* `template <typename CX> class` **`BasicRegex`**
* `using` **`Regex`** `= BasicRegex<char>`
* `using` **`Regex16`** `= BasicRegex<char16_t>`
* `using` **`Regex32`** `= BasicRegex<char32_t>`
* `using` **`WideRegex`** `= BasicRegex<wchar_t>`
* `using` **`ByteRegex`** `= BasicRegex<void>`

The generic regular expression class, and aliases for each of the possible
instantiations. In this and all other templates in this module, the template
argument `CX` can be any of the four standard character types, or `void`. When
`void` is used, the resulting regex works on single byte strings (i.e the
character and string types are `char` and `std::`**`string`**, the same as if the
template argument had been `char`), but matches using PCRE's byte mode instead
of UTF-8 mode.

* `using BasicRegex::`**`char_type`**
    `= [char if CX is void, otherwise CX]`
* `using BasicRegex::`**`cx_type`**
    `= CX`
* `using BasicRegex::`**`string_type`**
    `= basic_string<char_type>`
* `using BasicRegex::`**`match_type`**
    `= BasicMatch<CX>`
* `using BasicRegex::`**`match_iterator`**
    `= BasicMatchIterator<CX>`
* `using BasicRegex::`**`match_range`**
    `= Irange<match_iterator>`
* `using BasicRegex::`**`split_iterator`**
    `= BasicSplitIterator<CX>`
* `using BasicRegex::`**`split_range`**
    `= Irange<split_iterator>`
* `using BasicRegex::`**`utf_iterator`**
    `= UtfIterator<CX> [not defined if CX is void]`

Member types.

* `BasicRegex::`**`BasicRegex`**`()`
* `explicit BasicRegex::`**`BasicRegex`**`(const string_type& pattern, uint32_t flags = 0)`
* `BasicRegex::`**`BasicRegex`**`(const BasicRegex& r)`
* `BasicRegex::`**`BasicRegex`**`(BasicRegex&& r) noexcept`
* `BasicRegex::`**`~BasicRegex`**`() noexcept`
* `BasicRegex& BasicRegex::`**`operator=`**`(const BasicRegex& r)`
* `BasicRegex& BasicRegex::`**`operator=`**`(BasicRegex&& r) noexcept`

Life cycle functions. The default constructor is equivalent to construction
from an empty pattern. The second constructor will throw
`std::invalid_argument` if an invalid combination of flags is passed, or
`RegexError` if the pattern is invalid. See above for full details of how the
flags are interpreted.

* `BasicRegex::match_type BasicRegex::`**`anchor`**`(const string_type& text, size_t offset = 0) const`
* `BasicRegex::match_type BasicRegex::`**`anchor`**`(const utf_iterator& start) const [not defined if CX is void]`
* `BasicRegex::match_type BasicRegex::`**`match`**`(const string_type& text, size_t offset = 0) const`
* `BasicRegex::match_type BasicRegex::`**`match`**`(const utf_iterator& start) const [not defined if CX is void]`
* `BasicRegex::match_type BasicRegex::`**`search`**`(const string_type& text, size_t offset = 0) const`
* `BasicRegex::match_type BasicRegex::`**`search`**`(const utf_iterator& start) const [not defined if CX is void]`
* `BasicRegex::match_type BasicRegex::`**`operator()`**`(const string_type& text, size_t offset = 0) const`
* `BasicRegex::match_type BasicRegex::`**`operator()`**`(const utf_iterator& start) const [not defined if CX is void]`

These are the regex matching functions. The `search()` functions return a
successful match if the pattern matches anywhere in the subject string;
`anchor()` matches only at the start of the subject string; `match()` is
successful only if the pattern matches the complete string. The function call
operators are equivalent to `search()`.

These functions can accept a starting offset into the subject string as either
an integer (interpreted as an offset in code units), or a UTF iterator for
Unicode regexes (these overloads are not defined for byte regexes). The
subject string itself is not explicitly required in the second version, since
it can be obtained from the iterator. When a nonzero offset is passed, or an
iterator that does not point to the beginning of the string, the search begins
at the specified point in the string, but the text preceding it will still be
taken into account in lookbehind assertions.

All of these will throw `RegexError` if anything goes wrong (this will be rare
in practise since most errors will have been caught when the regex was
constructed, but a few kinds of regex error are not detected by PCRE until
execution time).

* `size_t BasicRegex::`**`count`**`(const string_type& text) const`

Return the number of non-overlapping matches found in the text.

* `bool BasicRegex::`**`empty`**`() const noexcept`

True if the pattern is empty.

* `BasicRegex::string_type BasicRegex::`**`extract`**`(const string_type& fmt, const string_type& text, size_t n = npos) const`
* `BasicRegex::string_type BasicRegex::`**`format`**`(const string_type& fmt, const string_type& text, size_t n = npos) const`

The `format()` function uses the formatting string to transform the text,
replacing the first `n` matching substrings (all of them by default) with the
corresponding reformatted text, and returning the resulting string. The
`extract()` function also copies the first `n` matching substrings, applying
formatting in the same way as `format()`, but discards the unmatched text
between matches.

* `BasicRegex::match_range BasicRegex::`**`grep`**`(const string_type& text) const`

This returns a range object that can be used to iterate over all matches
within the subject string. Refer to the `BasicMatchIterator` class (below) for
further details.

* `size_t BasicRegex::`**`groups`**`() const noexcept`

Returns the number of groups in the regex (the number of parenthesized
captures, plus one for the complete match).

* `size_t BasicRegex::`**`named`**`(const string_type& name) const noexcept`

If the regex includes any named captures, this returns the group index
(1-based) corresponding to the given name. It will return zero if there is no
capture by that name (or if the regex does not use named captures).

* `BasicRegex::string_type BasicRegex::`**`pattern`**`() const`
* `uint32_t BasicRegex::`**`flags`**`() const noexcept`

These return the construction arguments.

* `BasicRegex::split_range BasicRegex::`**`split`**`(const string_type& text) const`

This returns a range object that can be used to iterate over the substrings
delimited by matches within the subject string, effectively splitting the
string using regex matches as delimiters. Refer to the `BasicSplitIterator`
class (below) for further details.

* `void BasicRegex::`**`swap`**`(BasicRegex& r) noexcept`
* `template <typename CX> void` **`swap`**`(BasicRegex<CX>& lhs, BasicRegex<CX>& rhs) noexcept`

Swap two regex objects.

* `bool` **`operator==`**`(const BasicRegex& lhs, const BasicRegex& rhs) noexcept`
* `bool` **`operator!=`**`(const BasicRegex& lhs, const BasicRegex& rhs) noexcept`
* `bool` **`operator<`**`(const BasicRegex& lhs, const BasicRegex& rhs) noexcept`
* `bool` **`operator>`**`(const BasicRegex& lhs, const BasicRegex& rhs) noexcept`
* `bool` **`operator<=`**`(const BasicRegex& lhs, const BasicRegex& rhs) noexcept`
* `bool` **`operator>=`**`(const BasicRegex& lhs, const BasicRegex& rhs) noexcept`

Comparison operators. The order is approximately based on the pattern text,
but should be treated as an arbitrary order; the flags are also taken into
account. If two regexes are semantically the same (i.e. always match or fail
to match the same text) despite differing slightly in spelling, it is
unspecified whether or not they will compare equal.

* `template <typename C> BasicRegex<C>` **`regex`**`(const basic_string<C>& pattern, uint32_t flags = 0)`
* `template <typename C> BasicRegex<C>` **`regex`**`(const C* pattern, uint32_t flags = 0)`

Convenience functions to construct a regex object.

* `namespace` **`Literals`**
    * `Regex` **`operator"" _re`**
        `(const char* ptr, size_t len)`
    * `Regex` **`operator"" _re_i`**
        `(const char* ptr, size_t len)`
    * `Regex16` **`operator"" _re`**
        `(const char16_t* ptr, size_t len)`
    * `Regex16` **`operator"" _re_i`**
        `(const char16_t* ptr, size_t len)`
    * `Regex32` **`operator"" _re`**
        `(const char32_t* ptr, size_t len)`
    * `Regex32` **`operator"" _re_i`**
        `(const char32_t* ptr, size_t len)`
    * `WideRegex` **`operator"" _re`**
        `(const wchar_t* ptr, size_t len)`
    * `WideRegex` **`operator"" _re_i`**
        `(const wchar_t* ptr, size_t len)`
    * `ByteRegex` **`operator"" _re_b`**
        `(const char* ptr, size_t len)`
    * `ByteRegex` **`operator"" _re_bi`**
        `(const char* ptr, size_t len)`

Regex literals. The versions with an `"_i"` suffix are case insensitive; apart
from that, no options are supported.

## Regex match class ##

* `template <typename CX> class` **`BasicMatch`**
* `using` **`Match`** `= BasicMatch<char>`
* `using` **`Match16`** `= BasicMatch<char16_t>`
* `using` **`Match32`** `= BasicMatch<char32_t>`
* `using` **`WideMatch`** `= BasicMatch<wchar_t>`
* `using` **`ByteMatch`** `= BasicMatch<void>`

This template class is returned by regex matching functions, reporting the
result of the matching attempt.

* `using BasicMatch::`**`char_type`**
    `= [char if CX is void, otherwise CX]`
* `using BasicMatch::`**`cx_type`**
    `= CX`
* `using BasicMatch::`**`string_type`**
    `= basic_string<char_type>`
* `using BasicMatch::`**`regex_type`**
    `= BasicRegex<CX>`
* `using BasicMatch::`**`const_iterator`**
    `= const char_type*`
* `using BasicMatch::`**`const_reference`**
    `= const char_type&`
* `using BasicMatch::`**`difference_type`**
    `= ptrdiff_t`
* `using BasicMatch::`**`iterator`**
    `= [string::const_iterator if CX is void, otherwise UtfIterator<CX>]`
* `using BasicMatch::`**`reference`**
    `= const value_type&`
* `using BasicMatch::`**`value_type`**
    `= [char if CX is void, otherwise char32_t]`

Member types.

* `BasicMatch::`**`BasicMatch`**`()`
* `BasicMatch::`**`BasicMatch`**`(const BasicMatch& m)`
* `BasicMatch::`**`BasicMatch`**`(BasicMatch&& m) noexcept`
* `BasicMatch::`**`~BasicMatch`**`() noexcept`
* `BasicMatch& BasicMatch::`**`operator=`**`(const BasicMatch& m)`
* `BasicMatch& BasicMatch::`**`operator=`**`(BasicMatch&& m) noexcept`

Life cycle functions. Normally a match object will be returned by a regex
matching function rather than directly constructed by the user.

* `BasicMatch::iterator BasicMatch::`**`begin`**`(size_t i = 0) const noexcept`
* `BasicMatch::iterator BasicMatch::`**`end`**`(size_t i = 0) const noexcept`

These return UTF iterators over the characters within a match. The default
versions return iterators bracketing the complete match; if the index argument
is positive, the iterators mark the corresponding numbered capture group. If
the index corresponds to a group that was not matched, or if the match itself
was unsuccessful, `begin()` and `end()` will return the same iterator (its
value is otherwise unspecified).

* `bool BasicMatch::`**`empty`**`() const noexcept`

True if the match failed or matched an empty string.

* `BasicMatch::string_type BasicMatch::`**`first`**`() const`
* `BasicMatch::string_type BasicMatch::`**`last`**`() const`

These return the first and last non-empty capture groups (not counting the
complete match), or empty strings if there are no such groups.

* `bool BasicMatch::`**`full_or_partial`**`() const noexcept`
* `bool BasicMatch::`**`partial`**`() const noexcept`

The `partial()` function is true if a partial match was detected, while
`full_or_partial()` is true if either a full or partial match was detected.
These are only meaningful if one of the `rx_partialhard` or `rx_partialsoft`
options was selected when the original regex was compiled; otherwise,
`partial()` is always false and `full_or_partial()` is equivalent to
`matched()`.

* `size_t BasicMatch::`**`groups`**`() const noexcept`

The number of groups in the match (the number of captures, plus one for the
complete match).

* `bool BasicMatch::`**`matched`**`(size_t i = 0) const noexcept`
* `explicit BasicMatch::`**`operator bool`**`() const noexcept`
* `bool BasicMatch::`**`operator!`**`() const noexcept`

The `matched()` function indicates whether a capture group, identified by
number, was matched; by default, it indicates whether the match as a whole was
successful. The boolean conversion is equivalent to `matched(0)`.

* `size_t BasicMatch::`**`offset`**`(size_t i = 0) const noexcept`
* `size_t BasicMatch::`**`endpos`**`(size_t i = 0) const noexcept`
* `size_t BasicMatch::`**`count`**`(size_t i = 0) const noexcept`

These return the starting position, end position, and size of the match, or of
a specific capture group. These are measured in code units (not characters)
from the start of the subject string. If the match was unsuccessful, or if the
index refers to a group that does not exist in the regex or was not included
in the match, the two offsets will both be `npos` and the size will be zero.

* `BasicMatch::string_type BasicMatch::`**`str`**`(size_t i = 0) const`
* `BasicMatch::string_type BasicMatch::`**`named`**`(const string_type& name) const`
* `BasicMatch::string_type BasicMatch::`**`operator[]`**`(size_t i) const`
* `BasicMatch::`**`operator string_type`**`() const`

The `str()` and `named()` functions return a copy of the substring matched by
a numbered or named group, or an empty string if the group does not exist or
was not matched (note that an empty string can also be the result of a
legitimate match). The index operator is equivalent to `str(i)`; the string
conversion operator is equivalent to `str(0)`, which returns the complete
match.

* `void BasicMatch::`**`swap`**`(BasicMatch& m) noexcept`
* `template <typename CX> void` **`swap`**`(BasicMatch<CX>& lhs, BasicMatch<CX>& rhs) noexcept`

Swap two match objects.

## Regex formatting class ##

* `template <typename CX> class` **`BasicRegexFormat`**
* `using` **`RegexFormat`** `= BasicRegexFormat<char>`
* `using` **`RegexFormat16`** `= BasicRegexFormat<char16_t>`
* `using` **`RegexFormat32`** `= BasicRegexFormat<char32_t>`
* `using` **`WideRegexFormat`** `= BasicRegexFormat<wchar_t>`
* `using` **`ByteRegexFormat`** `= BasicRegexFormat<void>`

The regex format class contains both a regex and a format string. It provides
operations equivalent to the `BasicRegex::`**`format`**`()` function, but compiling
the format string only once by constructing a regex format object will be more
efficient if the same formatting operation is going to be applied many times.

* `using BasicRegexFormat::`**`char_type`** `= [char if CX is void, otherwise CX]`
* `using BasicRegexFormat::`**`cx_type`** `= CX`
* `using BasicRegexFormat::`**`string_type`**
    `= basic_string<char_type>`
* `using BasicRegexFormat::`**`regex_type`** `= BasicRegex<CX>`
* `using BasicRegexFormat::`**`match_type`** `= BasicMatch<CX>`

Member types.

* `BasicRegexFormat::`**`BasicRegexFormat`**`()`
* `BasicRegexFormat::`**`BasicRegexFormat`**`(const regex_type& pattern, const string_type& format)`
* `BasicRegexFormat::`**`BasicRegexFormat`**`(const string_type& pattern, const string_type& format, uint32_t flags = 0)`
* `BasicRegexFormat::`**`BasicRegexFormat`**`(const BasicRegexFormat& f)`
* `BasicRegexFormat::`**`BasicRegexFormat`**`(BasicRegexFormat&& f) noexcept`
* `BasicRegexFormat::`**`~BasicRegexFormat`**`() noexcept`
* `BasicRegexFormat& BasicRegexFormat::`**`operator=`**`(const BasicRegexFormat& f)`
* `BasicRegexFormat& BasicRegexFormat::`**`operator=`**`(BasicRegexFormat&& f) noexcept`

Life cycle functions. The object is constructed from a regex (supplied either
as a precompiled regex or a pattern and flag set) and a format string. The
third constructor can throw the same exceptions as the corresponding regex
constructor.

* `BasicRegexFormat::`**`string_type`** `BasicRegexFormat::`**`format`**`(const string_type& text, size_t n = npos) const`
* `BasicRegexFormat::`**`string_type`** `BasicRegexFormat::`**`extract`**`(const string_type& text, size_t n = npos) const`
* `BasicRegexFormat::`**`string_type`** `BasicRegexFormat::`**`operator()`**`(const string_type& text, size_t n = npos) const`

The `format()` function (and the equivalent function call operator) uses the
formatting string to transform the text, replacing the first `n` matching
substrings (all of them by default) with the corresponding reformatted text,
and returning the resulting string. The `extract()` function copies only the
first `n` matches, discarding the unmatched text between them.
`RegexFormat(regex,fmt).format(text)` is equivalent to
`regex.format(fmt,text)`, and similarly for `extract()`.

* `BasicRegexFormat::ex_type BasicRegexFormat::`**`regex`**`() const`
* `BasicRegexFormat::ing_type BasicRegexFormat::`**`format`**`() const`
* `BasicRegexFormat::ing_type BasicRegexFormat::`**`pattern`**`() const`
* `uint32_t BasicRegexFormat::`**`flags`**`() const noexcept`

These functions query the construction parameters. The `pattern()` and
`flags()` functions are equivalent to `regex().pattern()` and
`regex().flags()`.

* `void BasicRegexFormat::`**`swap`**`(BasicRegexFormat& r) noexcept`
* `template <typename CX> void` **`swap`**`(BasicRegexFormat<CX>& lhs, BasicRegexFormat<CX>& rhs) noexcept`

Swap two objects.

* `template <typename C> BasicRegexFormat<C>` **`regex_format`**
    `(const basic_string<C>& pattern, const basic_string<C>& format, uint32_t flags = 0)`
* `template <typename C> BasicRegexFormat<C>` **`regex_format`**
    `(const basic_string<C>& pattern, const C* format, uint32_t flags = 0)`
* `template <typename C> BasicRegexFormat<C>` **`regex_format`**
    `(const C* pattern, const basic_string<C>& format, uint32_t flags = 0)`
* `template <typename C> BasicRegexFormat<C>` **`regex_format`**
    `(const C* pattern, const C* format, uint32_t flags = 0)`

Convenience functions to construct a regex format object.

## Regex iterator classes ##

* `template <typename CX> class` **`BasicMatchIterator`**
    * `using BasicMatchIterator::`**`char_type`**
        `= [char if CX is void, otherwise CX]`
    * `using BasicMatchIterator::`**`cx_type`**
        `= CX`
    * `using BasicMatchIterator::`**`string_type`**
        `= basic_string<char_type>`
    * `using BasicMatchIterator::`**`regex_type`**
        `= BasicRegex<CX>`
    * `using BasicMatchIterator::`**`match_type`**
        `= BasicMatch<CX>`
    * `using BasicMatchIterator::`**`difference_type`**
        `= ptrdiff_t`
    * `using BasicMatchIterator::`**`iterator_category`**
        `= std::`**`forward_iterator_tag`**
    * `using BasicMatchIterator::`**`pointer`**
        `= const match_type*`
    * `using BasicMatchIterator::`**`reference`**
        `= const match_type&`
    * `using BasicMatchIterator::`**`value_type`**
        `= match_type`
    * `BasicMatchIterator::`**`BasicMatchIterator`**`()`
    * `BasicMatchIterator::`**`BasicMatchIterator`**`(const regex_type& re, const string_type& text)`
    * _[standard iterator operations]_
* `using` **`MatchIterator`** `= BasicMatchIterator<char>`
* `using` **`MatchIterator16`** `= BasicMatchIterator<char16_t>`
* `using` **`MatchIterator32`** `= BasicMatchIterator<char32_t>`
* `using` **`WideMatchIterator`** `= BasicMatchIterator<wchar_t>`
* `using` **`ByteMatchIterator`** `= BasicMatchIterator<void>`

An iterator over the (non-overlapping) matches found within a subject string
for a given regex. These are normally returned by `BasicRegex::`**`grep`**`()` rather
than constructed directly by the user.

* `template <typename CX> class` **`BasicSplitIterator`**
    * `using BasicSplitIterator::`**`char_type`**
        `= [char if CX is void, otherwise CX]`
    * `using BasicSplitIterator::`**`cx_type`**
        `= CX`
    * `using BasicSplitIterator::`**`string_type`**
        `= basic_string<char_type>`
    * `using BasicSplitIterator::`**`regex_type`**
        `= BasicRegex<CX>`
    * `using BasicSplitIterator::`**`match_type`**
        `= BasicMatch<CX>`
    * `using BasicSplitIterator::`**`match_iterator`**
        `= BasicMatchIterator<CX>`
    * `using BasicSplitIterator::`**`difference_type`**
        `= ptrdiff_t`
    * `using BasicSplitIterator::`**`iterator_category`**
        `= std::`**`forward_iterator_tag`**
    * `using BasicSplitIterator::`**`pointer`**
        `= const string_type*`
    * `using BasicSplitIterator::`**`reference`**
        `= const string_type&`
    * `using BasicSplitIterator::`**`value_type`**
        `= string_type`
    * `BasicSplitIterator::`**`BasicSplitIterator`**`()`
    * `BasicSplitIterator::`**`BasicSplitIterator`**`(const regex_type& re, const string_type& text)`
    * _[standard iterator operations]_
* `using` **`SplitIterator`** `= BasicSplitIterator<char>`
* `using` **`SplitIterator16`** `= BasicSplitIterator<char16_t>`
* `using` **`SplitIterator32`** `= BasicSplitIterator<char32_t>`
* `using` **`WideSplitIterator`** `= BasicSplitIterator<wchar_t>`
* `using` **`ByteSplitIterator`** `= BasicSplitIterator<void>`

An iterator over the substrings between matches for a given regex. These are
normally returned by `BasicRegex::`**`split`**`()` rather than constructed directly by
the user.

## Utility functions ##

* `template <typename C> basic_string<C>` **`regex_escape`**
    `(const basic_string<C>& str)`
* `template <typename C> basic_string<C>` **`regex_escape`**
    `(const C* str)`

These return a copy of the argument string, modified by inserting escape
characters where necessary to produce a pattern that will exactly match the
original string and nothing else. (You can get the same effect by enclosing
the text in `"\Q...\E"` delimiters, provided the text does not contain
`"\E"`.)

## Version information ##

* `Version` **`regex_version`**`() noexcept`

Returns the version of PCRE used to build this library.

* `Version` **`regex_unicode_version`**`() noexcept`

Returns the PCRE library's version of Unicode. Because the PCRE library is
built separately, this is not guaranteed to be the same as the version used by
the Unicorn library.
