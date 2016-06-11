# [Unicorn Library](index.html): Regular Expressions #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/regex.hpp"`

## Contents ##

[TOC]

## Introduction ##

This module provides Unicode-aware regular expressions, and related classes
and functions. It calls the widely available [PCRE](http://www.pcre.org/)
(Perl Compatible Regular Expressions) library. Unicorn's regexes use the 8 bit
version of the PCRE library, which must have been built with the optional
UTF-8 support enabled (PCRE can also be built as 16 and 32 bit libraries, but
these are not used by Unicorn). The module currently uses the traditional
version of PCRE; at some point I intend to convert it to use PCRE2 instead.

Refer to the PCRE documentation for details of the regular expression syntax.

Although the regex functions in this module are specified in terms of
`u8string` arguments, Unicorn regexes support both UTF-8 and byte oriented
matching. Byte regexes (selected with the `rx_byte` flag) simply treat a
string as a sequence of arbitrary bytes, with no assumptions about content
encoding, and will work with non-Unicode strings. The `\xHH` escape code
(where H is a hexadecimal digit) always matches a single byte even if the
value is greater than `\x7f` (in a UTF-8 regex this would match a multibyte
encoded character); the `\x{hex}` escape code can still be used, but it will
be treated as a syntax error if the value is greater than `\x{ff}`.

## Unicorn::Regex vs std::regex ##

It would have been convenient to use standard C++11 regular expressions in
Unicorn, in the same way as the standard string classes have been used instead
of creating a new custom string class specific to Unicorn. Unfortunately, this
turns out to be impractical; for several reasons, standard regular expressions
are inadequate for use with generalized Unicode strings.

The most obvious reason is that standard C++ regexes are not actually required
to support Unicode strings at all. Unlike `std::basic_string`, for which
specializations for 8, 16, and 32 bit characters are required to exist, only
two specializations of `std::basic_regex` are mandated, for `char` (the
system's native multibyte encoding, which may or may not be UTF-8, but see
below for a caveat on this) and `wchar_t` (the system's wide character
encoding, which can reasonably be expected to be either UTF-16 or UTF-32, but
which one varies with the OS). In short, standard regexes can only be relied
on to support one of the three UTF encodings, and we don't know which one.

(Strictly speaking, not even that is required; the C++ standard does not
actually require the wide character encoding to be UTF-16 or 32. It is on all
systems I know of, though, and the Unicorn library explicitly does not support
systems on which it is not one of those.)

An implementation is allowed to instantiate `std::basic_regex` for other
character types, but in practise most do not, and in any case even an
implementation that supplied specializations for all four character types
would still not be reliably usable with UTF-8 (since the plain `char` encoding
is not guaranteed to be UTF-8).

The second problem with standard regexes is that, by the rules of the C++
standard, they _cannot_ properly support UTF-8 strings. The regex grammar
(based on that of JavaScript/EcmaScript, with a few changes) matches on an
element by element basis; a "character", as far as regex matching is
concerned, is a single code unit, not a Unicode scalar value (which may be
represented by more than one code unit). This still allows literal matching of
multi-unit UTF-8 characters (the encoding will be the same in the regex and
the subject string, so they will match unit for unit), but makes it impossible
to match multi-unit characters to non-literal regex elements; for example,
`std::regex(".")` will not match `u8"€"` (even if the system encoding is
UTF-8). For the same reason, it is impossible to specify a character range
that includes multibyte characters (e.g. `std::regex(u8"[À-ÿ]")` will not do
what you probably expected).

Finally, standard regexes don't support the `\p{...}` and `\P{...}` character
classes, which match on Unicode properties. This may be a minor obstacle
compared to either of the above showstoppers, but even by itself it would be a
serious handicap in a library dedicated to Unicode support.

For all of the above reasons, I felt I had no choice but to abandon standard
C++ regexes, and base Unicorn's regular expressions on the widely used PCRE
library instead.

## Regex options ##

Flag                      | Description                                                                    | PCRE equivalent
----                      | -----------                                                                    | ---------------
**`rx_byte`**             | Match in byte mode instead of UTF-8                                            | `~PCRE_UTF8`
**`rx_caseless`**         | Matching is case insensitive                                                   | `PCRE_CASELESS`
**`rx_dfa`**              | Use the alternative DFA matching algorithm                                     | `pcre_dfa_exec()`
**`rx_dollarnewline`**    | `$` may match line breaks preceding the end of the string                      | `~PCRE_DOLLAR_ENDONLY`
**`rx_dotinline`**        | `.` does not match line breaks                                                 | `~PCRE_DOTALL`
**`rx_extended`**         | Free-form mode; ignore whitespace and comments marked with `#`                 | `PCRE_EXTENDED`
**`rx_firstline`**        | Any match must start in the first line of the subject string                   | `PCRE_FIRSTLINE`
**`rx_multiline`**        | Multiline mode; `^` and `$` match the beginning and end of each line           | `PCRE_MULTILINE`
**`rx_newlineanycrlf`**   | Any of CR, LF, or CR+LF is recognised as a line break                          | `PCRE_NEWLINE_ANYCRLF`
**`rx_newlinecr`**        | Only CR is recognised as a line break                                          | `PCRE_NEWLINE_CR`
**`rx_newlinecrlf`**      | Only CR+LF is recognised as a line break                                       | `PCRE_NEWLINE_CRLF`
**`rx_newlinelf`**        | Only LF is recognised as a line break                                          | `PCRE_NEWLINE_LF`
**`rx_noautocapture`**    | Parentheses do not automatically capture; only named captures are recorded     | `PCRE_NO_AUTO_CAPTURE`
**`rx_nostartoptimize`**  | Disable some optimizations that affect `(*COMMIT)` and `(*MARK)` handling      | `PCRE_NO_START_OPTIMIZE`
**`rx_notbol`**           | Do not match `^` at the start of the subject string                            | `PCRE_NOTBOL`
**`rx_notempty`**         | Do not match an empty string                                                   | `PCRE_NOTEMPTY`
**`rx_notemptyatstart`**  | Do not match an empty string at the start of the subject string                | `PCRE_NOTEMPTY_ATSTART`
**`rx_noteol`**           | Do not match `$` at the end of the subject string                              | `PCRE_NOTEOL`
**`rx_noutfcheck`**       | Skip UTF validity checks (ignored in byte mode)                                | `PCRE_NO_UTF8_CHECK`
**`rx_optimize`**         | Optimize the regex using PCRE's JIT compiler                                   | `PCRE_STUDY_JIT_COMPILE`
**`rx_partialhard`**      | Hard partial matching; prefer a partial match to a full match                  | `PCRE_PARTIAL_HARD`
**`rx_partialsoft`**      | Soft partial matching; prefer a full match to a partial match                  | `PCRE_PARTIAL_SOFT`
**`rx_prefershort`**      | Quantifiers are non-greedy in NFA mode; prefer shorter matches in DFA mode     | `PCRE_UNGREEDY,PCRE_DFA_SHORTEST`
**`rx_ucp`**              | Backslash-escape character sets use Unicode properties, instead of just ASCII  | `PCRE_UCP`

Flags controlling regular expression matching behaviour. Most of these
correspond directly to PCRE flags, but note that all flags must be specified
when the regex is constructed (unlike PCRE, where some flags can be set at
execution time).

Note that some of the flags (`rx_byte`, `rx_dollarnewline`, and
`rx_dotinline`) have the reverse sense to the corresponding PCRE flags
(`PCRE_UTF8`, `PCRE_DOLLAR_ENDONLY`, and `PCRE_DOTALL`, respectively). This is
simply because I felt that the reversed state was the more natural default in
these cases.

The four line breaking flags (`rx_newlineanycrlf`, `rx_newlinecr`,
`rx_newlinecrlf`, and `rx_newlinelf`) also affect the behaviour of the `\R`
escape code, which matches any of CR, LF, or CR+LF if any of these flags are
set (this corresponds to the `PCRE_BSR_ANYCRLF` flag). The default behaviour,
if none of these is set, recognises any Unicode line break (LF, VT, FF, CR,
CR+LF, `U+0085 NEXT LINE`, `U+2028 LINE SEPARATOR`, and `U+2029 PARAGRAPH
SEPARATOR`; the last three are not recognised in byte mode), corresponding to
the `PCRE_NEWLINE_ANY` and `PCRE_BSR_UNICODE` flags.

All regex constructors, and any functions that take a pattern and flags and
implicitly construct a regex, will throw `std::invalid_argument` if the flags
supplied are inconsistent:

* At most one of `rx_newlineanycrlf`, `rx_newlinecr`, `rx_newlinecrlf`, and `rx_newlinelf` may be used.
* `rx_byte` can only be used with 8-bit strings.
* `rx_byte` and `rx_ucp` may not be combined.
* `rx_notempty` and `rx_notemptyatstart` may not be combined.
* `rx_partialhard` and `rx_partialsoft` may not be combined.

**Caution:** If you use the `rx_noutfcheck` flag, be careful about sanitizing
your strings; behaviour is undefined if this flag is present and any regex
pattern, subject string, or format string is not valid Unicode.

## Formatting syntax ##

Formatting strings are used in the `format()` and `extract()` methods of
`Regex` and `RegexFormat`, to generate a modified string by using a regex to
match substrings in the original subject string, and then replacing each
matching substring with a new one generated from the format string.

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
`$[`                              | The text before the current match
`$]`                              | The text after the current match
`$_`                              | The complete subject string
`\xHH`, `\x{HHH...}`              | Unicode character, identified by hexadecimal code point
`\0`                              | Null character (`\x00`)
`\a`                              | Alert character (`\x07`)
`\b`                              | Backspace character (`\x08`)
`\t`                              | Horizontal tab character (`\x09`)
`\n`                              | Line feed character (`\x0a`)
`\v`                              | Vertical tab character (`\x0b`)
`\f`                              | Form feed character (`\x0c`)
`\r`                              | Carriage return character (`\x0d`)
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

When the case conversion codes (`\[luLTU]`) are used with byte mode regexes,
only ASCII characters will be converted.

If the format string contains `\x{HHH...}` escape codes, an `EncodingError`
exception will be thrown if the hexadecimal number is not a valid Unicode
scalar value or, for byte mode regexes, if it is greater than `0xff`.

## Supporting types ##

* `class` **`RegexError`**`: public std::runtime_error`
    * `RegexError::`**`RegexError`**`(int error, const u8string& pattern, const u8string& message = "")`
    * `int RegexError::`**`error`**`() const noexcept`
    * `const char* RegexError::`**`pattern`**`() const noexcept`

This is thrown from a regex constructor or matching function when the
underlying PCRE call reports an error.

## Regular expression class ##

* `class` **`Regex`**

The regular expression class.

* `Regex::`**`Regex`**`()`
* `explicit Regex::`**`Regex`**`(const u8string& pattern, uint32_t flags = 0)`
* `Regex::`**`Regex`**`(const Regex& r)`
* `Regex::`**`Regex`**`(Regex&& r) noexcept`
* `Regex::`**`~Regex`**`() noexcept`
* `Regex& Regex::`**`operator=`**`(const Regex& r)`
* `Regex& Regex::`**`operator=`**`(Regex&& r) noexcept`

Life cycle functions. The default constructor is equivalent to construction
from an empty pattern. The second constructor will throw
`std::invalid_argument` if an invalid combination of flags is passed, or
`RegexError` if the pattern is invalid. See above for full details of how the
flags are interpreted.

* `Match Regex::`**`anchor`**`(const u8string& text, size_t offset = 0) const`
* `Match Regex::`**`anchor`**`(const Utf8Iterator& start) const`
* `Match Regex::`**`match`**`(const u8string& text, size_t offset = 0) const`
* `Match Regex::`**`match`**`(const Utf8Iterator& start) const`
* `Match Regex::`**`search`**`(const u8string& text, size_t offset = 0) const`
* `Match Regex::`**`search`**`(const Utf8Iterator& start) const`
* `Match Regex::`**`operator()`**`(const u8string& text, size_t offset = 0) const`
* `Match Regex::`**`operator()`**`(const Utf8Iterator& start) const`

These are the regex matching functions. The `search()` functions return a
successful match if the pattern matches anywhere in the subject string;
`anchor()` matches only at the start of the subject string; `match()` is
successful only if the pattern matches the complete string. The function call
operators are equivalent to `search()`.

These functions can accept a starting offset into the subject string as either
an integer (interpreted as an offset in code units), or a UTF iterator for
Unicode regexes. The subject string itself is not explicitly required in the
second version, since it can be obtained from the iterator. When a nonzero
offset is passed, or an iterator that does not point to the beginning of the
string, the search begins at the specified point in the string, but the text
preceding it will still be taken into account in lookbehind assertions.

All of these will throw `RegexError` if anything goes wrong (this will be rare
in practise since most errors will have been caught when the regex was
constructed, but a few kinds of regex error are not detected by PCRE until
execution time).

**Caution:** Behaviour is undefined if you use the UTF iterator versions of
these functions with a byte mode regex.

* `size_t Regex::`**`count`**`(const u8string& text) const`

Returns the number of non-overlapping matches found in the text.

* `bool Regex::`**`empty`**`() const noexcept`

True if the pattern is empty.

* `u8string Regex::`**`extract`**`(const u8string& fmt, const u8string& text, size_t n = npos) const`
* `u8string Regex::`**`format`**`(const u8string& fmt, const u8string& text, size_t n = npos) const`

The `format()` function uses the formatting string to transform the text,
replacing the first `n` matching substrings (all of them by default) with the
corresponding reformatted text, and returning the resulting string. The
`extract()` function also copies the first `n` matching substrings, applying
formatting in the same way as `format()`, but discards the unmatched text
between matches.

* `MatchRange Regex::`**`grep`**`(const u8string& text) const`

This returns a range object that can be used to iterate over all matches
within the subject string. Refer to the `MatchIterator` class (below) for
further details.

* `size_t Regex::`**`groups`**`() const noexcept`

Returns the number of groups in the regex (the number of parenthesized
captures, plus one for the complete match).

* `size_t Regex::`**`named`**`(const u8string& name) const noexcept`

If the regex includes any named captures, this returns the group index (1
based) corresponding to the given name. It will return zero if there is no
capture by that name (or if the regex does not use named captures).

* `u8string Regex::`**`pattern`**`() const`
* `uint32_t Regex::`**`flags`**`() const noexcept`

These return the construction arguments.

* `SplitRange Regex::`**`split`**`(const u8string& text) const`

This returns a range object that can be used to iterate over the substrings
delimited by matches within the subject string, effectively splitting the
string using regex matches as delimiters. Refer to the `SplitIterator` class
(below) for further details.

* `void Regex::`**`swap`**`(Regex& r) noexcept`
* `void` **`swap`**`(Regex& lhs, Regex& rhs) noexcept`

Swap two regex objects.

* `static u8string Regex::`**`escape`**`(const u8string& str)`

Returns a copy of the argument string, modified by inserting escape characters
where necessary to produce a pattern that will exactly match the original
string and nothing else. (You can get the same effect by enclosing the text in
`"\Q...\E"` delimiters, provided the text does not contain `"\E"`.)

* `Version Regex::`**`pcre_version`**`() noexcept`
* `Version Regex::`**`unicode_version`**`() noexcept`

These returns the version of PCRE used to build this library, and the PCRE
library's version of Unicode (because the PCRE library is built separately,
this is not guaranteed to be the same as the version used by the rest of the
Unicorn library).

* `bool` **`operator==`**`(const Regex& lhs, const Regex& rhs) noexcept`
* `bool` **`operator!=`**`(const Regex& lhs, const Regex& rhs) noexcept`
* `bool` **`operator<`**`(const Regex& lhs, const Regex& rhs) noexcept`
* `bool` **`operator>`**`(const Regex& lhs, const Regex& rhs) noexcept`
* `bool` **`operator<=`**`(const Regex& lhs, const Regex& rhs) noexcept`
* `bool` **`operator>=`**`(const Regex& lhs, const Regex& rhs) noexcept`

Comparison operators. The order is approximately based on the pattern text,
but should be treated as an arbitrary order; the flags are also taken into
account. If two regexes are semantically the same (i.e. always match or fail
to match the same text) despite differing slightly in spelling, it is
unspecified whether or not they will compare equal.

* `namespace` **`Literals`**
    * `Regex` **`operator"" _re`**`(const char* ptr, size_t len)`
    * `Regex` **`operator"" _re_b`**`(const char* ptr, size_t len)` _- Byte mode_
    * `Regex` **`operator"" _re_i`**`(const char* ptr, size_t len)` _- Case insensitive_

Regex literals.

## Regex match class ##

* `class` **`Match`**

This class is returned by regex matching functions, reporting the result of
the matching attempt.

* `Match::`**`Match`**`()`
* `Match::`**`Match`**`(const Match& m)`
* `Match::`**`Match`**`(Match&& m) noexcept`
* `Match::`**`~Match`**`() noexcept`
* `Match& Match::`**`operator=`**`(const Match& m)`
* `Match& Match::`**`operator=`**`(Match&& m) noexcept`

Life cycle functions. Normally a match object will be returned by a regex
matching function rather than directly constructed by the user.

* `bool Match::`**`empty`**`() const noexcept`

True if the match failed or matched an empty string.

* `u8string Match::`**`first`**`() const`
* `u8string Match::`**`last`**`() const`

These return the first and last non-empty capture groups (not counting the
complete match), or empty strings if there are no such groups.

* `bool Match::`**`full_or_partial`**`() const noexcept`
* `bool Match::`**`partial`**`() const noexcept`

The `partial()` function is true if a partial match was detected, while
`full_or_partial()` is true if either a full or partial match was detected.
These are only meaningful if one of the `rx_partialhard` or `rx_partialsoft`
options was selected when the original regex was compiled; otherwise,
`partial()` is always false and `full_or_partial()` is equivalent to
`matched()`.

* `size_t Match::`**`groups`**`() const noexcept`

The number of groups in the match (the number of captures, plus one for the
complete match).

* `bool Match::`**`matched`**`(size_t i = 0) const noexcept`
* `explicit Match::`**`operator bool`**`() const noexcept`
* `bool Match::`**`operator!`**`() const noexcept`

The `matched()` function indicates whether a capture group, identified by
number, was matched; by default, it indicates whether the match as a whole was
successful. The boolean conversion is equivalent to `matched(0)`.

* `size_t Match::`**`offset`**`(size_t i = 0) const noexcept`
* `size_t Match::`**`endpos`**`(size_t i = 0) const noexcept`
* `size_t Match::`**`count`**`(size_t i = 0) const noexcept`

These return the starting position, end position, and size of the match, or of
a specific capture group. These are measured in code units (not characters)
from the start of the subject string. If the match was unsuccessful, or if the
index refers to a group that does not exist in the regex or was not included
in the match, the two offsets will both be `npos` and the size will be zero.

* `u8string::const_iterator Match::`**`s_begin`**`(size_t i = 0) const noexcept`
* `u8string::const_iterator Match::`**`s_end`**`(size_t i = 0) const noexcept`
* `Irange<u8string::const_iterator> Match::`**`s_range`**`(size_t i = 0) const noexcept`
* `Utf8Iterator Match::`**`u_begin`**`(size_t i = 0) const noexcept`
* `Utf8Iterator Match::`**`u_end`**`(size_t i = 0) const noexcept`
* `Irange<Utf8Iterator> Match::`**`u_range`**`(size_t i = 0) const noexcept`

These return iterators (string or UTF) over the characters within a match. The
default versions return iterators bracketing the complete match; if the index
argument is not zero, the iterators mark the corresponding numbered capture
group. If the index corresponds to a group that was not matched, or if the
match itself was unsuccessful, `begin()` and `end()` will return the same
iterator (its value is otherwise unspecified).

**Caution:** If the match was returned by a byte-mode regex, be careful to
always use the string iterators and not the UTF iterators, which are not
meaningful when the string is not being interpreted as UTF-8. Behaviour is
undefined in this situation.

* `u8string Match::`**`str`**`(size_t i = 0) const`
* `u8string Match::`**`named`**`(const u8string& name) const`
* `u8string Match::`**`operator[]`**`(size_t i) const`
* `Match::`**`operator u8string`**`() const`

The `str()` and `named()` functions return a copy of the substring matched by
a numbered or named group, or an empty string if the group does not exist or
was not matched (note that an empty string can also be the result of a
legitimate match). The index operator is equivalent to `str(i)`; the string
conversion operator is equivalent to `str(0)`, which returns the complete
match.

* `void Match::`**`swap`**`(Match& m) noexcept`
* `void` **`swap`**`(Match& lhs, Match& rhs) noexcept`

Swap two match objects.

## Regex formatting class ##

* `class` **`RegexFormat`**

The regex format class contains both a regex and a format string. It provides
operations equivalent to the `Regex::format()` function, but compiling the
format string only once by constructing a regex format object will be more
efficient if the same formatting operation is going to be applied many times.

* `RegexFormat::`**`RegexFormat`**`()`
* `RegexFormat::`**`RegexFormat`**`(const Regex& pattern, const u8string& format)`
* `RegexFormat::`**`RegexFormat`**`(const u8string& pattern, const u8string& format, uint32_t flags = 0)`
* `RegexFormat::`**`RegexFormat`**`(const RegexFormat& f)`
* `RegexFormat::`**`RegexFormat`**`(RegexFormat&& f) noexcept`
* `RegexFormat::`**`~RegexFormat`**`() noexcept`
* `RegexFormat& RegexFormat::`**`operator=`**`(const RegexFormat& f)`
* `RegexFormat& RegexFormat::`**`operator=`**`(RegexFormat&& f) noexcept`

Life cycle functions. The object is constructed from a regex (supplied either
as a precompiled regex or a pattern and flag set) and a format string. The
third constructor can throw the same exceptions as the corresponding regex
constructor.

* `u8string RegexFormat::`**`format`**`(const u8string& text, size_t n = npos) const`
* `u8string RegexFormat::`**`extract`**`(const u8string& text, size_t n = npos) const`
* `u8string RegexFormat::`**`operator()`**`(const u8string& text, size_t n = npos) const`

The `format()` function (and the equivalent function call operator) uses the
formatting string to transform the text, replacing the first `n` matching
substrings (all of them by default) with the corresponding reformatted text,
and returning the resulting string. The `extract()` function copies only the
first `n` matches, discarding the unmatched text between them.
`RegexFormat(regex,fmt).format(text)` is equivalent to
`regex.format(fmt,text)`, and similarly for `extract()`.

* `Regex RegexFormat::`**`regex`**`() const`
* `u8string RegexFormat::`**`format`**`() const`
* `u8string RegexFormat::`**`pattern`**`() const`
* `uint32_t RegexFormat::`**`flags`**`() const noexcept`

These functions query the construction parameters. The `pattern()` and
`flags()` functions are equivalent to `regex().pattern()` and
`regex().flags()`.

* `void RegexFormat::`**`swap`**`(RegexFormat& r) noexcept`
* `void` **`swap`**`(RegexFormat& lhs, RegexFormat& rhs) noexcept`

Swap two objects.

## Regex iterator classes ##

* `class` **`MatchIterator`**
    * `using MatchIterator::`**`difference_type`** `= ptrdiff_t`
    * `using MatchIterator::`**`iterator_category`** `= std::forward_iterator_tag`
    * `using MatchIterator::`**`pointer`** `= const Match*`
    * `using MatchIterator::`**`reference`** `= const Match&`
    * `using MatchIterator::`**`value_type`** `= Match`
    * `MatchIterator::`**`MatchIterator`**`()`
    * `MatchIterator::`**`MatchIterator`**`(const Regex& re, const u8string& text)`
    * _[standard iterator operations]_
* `using` **`MatchRange`** `= Irange<MatchIterator>`

An iterator over the (non-overlapping) matches found within a subject string
for a given regex. These are normally returned by `Regex::grep()` rather than
constructed directly by the user.

* `class` **`SplitIterator`**
    * `using SplitIterator::`**`difference_type`** `= ptrdiff_t`
    * `using SplitIterator::`**`iterator_category`** `= std::forward_iterator_tag`
    * `using SplitIterator::`**`pointer`** `= const u8string*`
    * `using SplitIterator::`**`reference`** `= const u8string&`
    * `using SplitIterator::`**`value_type`** `= u8string`
    * `SplitIterator::`**`SplitIterator`**`()`
    * `SplitIterator::`**`SplitIterator`**`(const Regex& re, const u8string& text)`
    * _[standard iterator operations]_
* `using` **`SplitRange`** `= Irange<SplitIterator>`

An iterator over the substrings between matches for a given regex. These are
normally returned by `Regex::split()` rather than constructed directly by the
user.
