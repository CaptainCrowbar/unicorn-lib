Title: Unicorn Library: String Operations
CSS: style.css

# [Unicorn Library](index.html): String Operations #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/string.hpp"` ####

This module contains assorted functions related to strings that don't belong
in other modules with a more specific focus.

## Contents ##

* [Introduction][]
* [String size functions][]
* [Other string properties][]
* [String algorithms][]
* [String manipulation functions][]
* [Case mapping functions][]
* [Type conversion functions][]

## Introduction ##

Not all of these functions are directly related to Unicode; some, such as the
`str_starts_with()` and `str_ends_with()` tests or the `str_replace()`
function, simply operate on the string as a sequence of code units, without
needing to know anything about how it is interpreted. Functions that do need
to be aware of how Unicode encoding works usually operate on, or return, UTF
iterators (see [`unicorn/utf`](utf.html)); many of these can accept either an
actual string or a UTF iterator range as their subject string.

All of the functions here are intended to be used only with known valid
Unicode strings. They all use `ignore_errors` mode internally, and do not
provide any other error handling options.

Many of the string manipulation functions in this module come in two versions,
on that takes the subject string by `const` reference and returns the modified
string, and one that takes it by non-`const` reference and modifies it in
place. Usually the in-place version has a name ending with `_in` to
distinguish them, since in many cases they have identical argument lists apart
from the `const` vs non-`const` argument, and therefore would not be reliably
distinguished by overload resolution if they had the same name.

In some cases the in-place version of the function takes a non-`const`
reference to the subject string accompanied by one or more UTF iterators (see
[`unicorn/utf`](utf.html)) to indicate positions in the string, whereas the
return-value version of the function does not need to be passed the string
explicitly since it can obtain a `const` reference to it from the iterators.
In the in-place function, behaviour is undefined if the iterators do not point
to the same string passed to the reference argument. (In all functions that
take iterators, behaviour is undefined if a pair of iterators that are
expected to mark the beginning and end of a substring do not point to the same
string or are in the wrong order.)

Any in-place function that might modify its subject string invalidates any
iterators (plain or UTF) passed to it that were pointing into that string.
Note that the iterators should still be considered to be invalidated even if
the string turns out not to be actually modified in a particular case.

Any function that implicitly compares strings uses a simple literal
comparison, making no attempt to handle Unicode's concepts of canonical or
compatibility equivalence; if your code needs to be aware of such things, you
will need to normalise your strings first (see
[`unicorn/normal`](normal.html)).

When a function can take a string argument as either a string object or a
character pointer, a null pointer is always treated as equivalent to an empty
string. (When it takes a pointer and size, the size is ignored if the pointer
is null.)

## String size functions ##

Bitmask            | Letter  | Description
-------            | ------  | -----------
`character_units`  | `C`     | Count the number of Unicode characters in the string (this is normally the default)
`grapheme_units`   | `G`     | Count the number of grapheme clusters (user-perceived characters) in the string
`narrow_context`   | `N`     | Calculate the East Asian width; ambiguous characters default to narrow
`wide_context`     | `W`     | Calculate the East Asian width; ambiguous characters default to wide

Most functions that need to calculate the size of a string, in this module and
others, accept one of these flags to indicate which definition of "size" the
caller wants. The various methods of measurement are implemented in the
`str_length()` function, described below; anything else that needs a string
size will normally obtain it by calling `str_length()` (or a related function
such as `str_find_index()`).

By default, a string's length is a count of characters (Unicode scalar
values); you can also select a count of grapheme clusters (user-perceived
characters; see [`unicorn/segment`](segment.html)), or calculate the East
Asian width. The two options for East Asian width determine how ambiguous
width characters are handled, defaulting to narrow (one unit) or wide (two
units). The `grapheme_units` flag can be combined with either of the East
Asian width options, giving a size based on the width of the base character of
each grapheme cluster.

* `template <typename C> size_t str_length(const basic_string<C>& str, Flagset flags = {})`
* `template <typename C> size_t str_length(const Irange<UtfIterator<C>>& range, Flagset flags = {})`
* `template <typename C> size_t str_length(const UtfIterator<C>& begin, const UtfIterator<C>& end, Flagset flags = {})`

Return the length of the string, measured according to the flags supplied.

* `template <typename C> UtfIterator<C> str_find_index(const basic_string<C>& str, size_t pos, Flagset flags = {})`
* `template <typename C> UtfIterator<C> str_find_index(const Irange<UtfIterator<C>>& range, size_t pos, Flagset flags = {})`
* `template <typename C> UtfIterator<C> str_find_index(const UtfIterator<C>& begin, const UtfIterator<C>& end, size_t pos, Flagset flags = {})`
* `template <typename C> size_t str_find_offset(basic_string<C>& str, size_t pos, Flagset flags = {})`

These return an iterator, or an offset in code units, pointing to the
character at a given position, measured according to the flags supplied. If
the requested position would be past the end of the string, an end iterator
will be returned (or `npos` for `str_find_offset()`). If the position can't be
adjusted to exactly the specified value (because one of the East Asian width
options was selected and wide characters are present), the first valid
position after the requested point will be returned.

## Other string properties ##

* `template <typename C> char32_t str_char_at(const basic_string<C>& str, size_t index) noexcept`

Returns the character at a specific index, or zero if the index is out of
range.

* `template <typename C> char32_t str_first_char(const basic_string<C>& str) noexcept`
* `template <typename C> char32_t str_last_char(const basic_string<C>& str) noexcept`

Return the first or last character in a string, or zero if the string is
empty.

* `template <typename C> bool str_is_east_asian(const basic_string<C>& str)`

True if the string contains any East Asian characters.

* `template <typename C> bool str_starts_with(const basic_string<C>& str, const basic_string<C>& prefix) noexcept`
* `template <typename C> bool str_starts_with(const basic_string<C>& str, const C* prefix) noexcept`
* `template <typename C> bool str_ends_with(const basic_string<C>& str, const basic_string<C>& suffix) noexcept`
* `template <typename C> bool str_ends_with(const basic_string<C>& str, const C* suffix) noexcept`

These return true if the string starts or ends with the specified substring.

## String algorithms ##

* `template <typename C> size_t str_common(const basic_string<C>& s1, const basic_string<C>& s2, size_t start = 0) noexcept`
* `template <typename C> size_t str_common_utf(const basic_string<C>& s1, const basic_string<C>& s2, size_t start = 0) noexcept`

These return the count of code units in the longest common prefix of two
strings, optionally starting at a given offset (or, equivalently, the offset,
relative to `start`, of the first difference between the strings). The
`str_common()` function simply finds the longest common prefix of code units
without regard to encoding, while `str_common_utf()` finds the longest common
prefix of whole encoded characters (the returned count is still in code
units); this means it will return a smaller value than `str_common()` if the
offset found by `str_common()` is partway through an encoded character. Both
functions will return zero if `start` is past the end of either string.

* `struct StringCompare`
    * `template <typename C> bool StringCompare::operator()(const basic_string<C>& lhs, const basic_string<C>& rhs) const noexcept`
* `constexpr StringCompare str_compare`

A function object that performs a simple less-than comparison on two strings
in any UTF encoding. The result always reflects Unicode lexicographical order,
regardless of encoding. For UTF-8 and UTF-32 this is just a trivial call to
`basic_string`'s less-than operator, but for UTF-16 it needs to be slightly
more complicated to preserve the expected order (in UTF-16, unlike UTF-8 and
32, code unit order is not the same as code point order).

* `template <typename C> int str_compare_3way(const basic_string<C>& lhs, const basic_string<C>& rhs)`

This compares strings in the same way as `string_compare()` above, but returns
1, 0, or -1 to indicate that the first string is greater than, equal to, or
less than the second one, respectively.


* `template <typename C> bool str_expect(UtfIterator<C>& i, const basic_string<C>& prefix)`
* `template <typename C> bool str_expect(UtfIterator<C>& i, const UtfIterator<C>& end, const basic_string<C>& prefix)`

If the string starting from `i` starts with `prefix`, `str_expect()` updates
`i` to point to the end of the prefix and returns `true`; otherwise, it leaves
`i` unchanged and returns `false`. Optionally an endpoint other than the end
of the string can be supplied. These will always return `false` if `prefix` is
empty.

* `template <typename C> UtfIterator<C> str_find_char(const basic_string<C>& str, char32_t c)`
* `template <typename C> UtfIterator<C> str_find_char(const UtfIterator<C>& begin, const UtfIterator<C>& end, char32_t c)`
* `template <typename C> UtfIterator<C> str_find_char(const Irange<UtfIterator<C>>& range, char32_t c)`
* `template <typename C> UtfIterator<C> str_find_last_char(const basic_string<C>& str, char32_t c)`
* `template <typename C> UtfIterator<C> str_find_last_char(const UtfIterator<C>& begin, const UtfIterator<C>& end, char32_t c)`
* `template <typename C> UtfIterator<C> str_find_last_char(const Irange<UtfIterator<C>>& range, char32_t c)`

These return an iterator pointing to the first or last occurrence of the
specified character, or an end iterator if it is not found.

* `template <typename C> UtfIterator<C> str_find_first_of(const basic_string<C>& str, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_first_of(const basic_string<C>& str, const C* target)`
* `template <typename C> UtfIterator<C> str_find_first_of(const UtfIterator<C>& begin, const UtfIterator<C>& end, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_first_of(const UtfIterator<C>& begin, const UtfIterator<C>& end, const C* target)`
* `template <typename C> UtfIterator<C> str_find_first_of(const Irange<UtfIterator<C>>& range, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_first_of(const Irange<UtfIterator<C>>& range, const C* target)`
* `template <typename C> UtfIterator<C> str_find_first_not_of(const basic_string<C>& str, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_first_not_of(const basic_string<C>& str, const C* target)`
* `template <typename C> UtfIterator<C> str_find_first_not_of(const UtfIterator<C>& begin, const UtfIterator<C>& end, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_first_not_of(const UtfIterator<C>& begin, const UtfIterator<C>& end, const C* target)`
* `template <typename C> UtfIterator<C> str_find_first_not_of(const Irange<UtfIterator<C>>& range, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_first_not_of(const Irange<UtfIterator<C>>& range, const C* target)`
* `template <typename C> UtfIterator<C> str_find_last_of(const basic_string<C>& str, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_last_of(const basic_string<C>& str, const C* target)`
* `template <typename C> UtfIterator<C> str_find_last_of(const UtfIterator<C>& begin, const UtfIterator<C>& end, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_last_of(const UtfIterator<C>& begin, const UtfIterator<C>& end, const C* target)`
* `template <typename C> UtfIterator<C> str_find_last_of(const Irange<UtfIterator<C>>& range, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_last_of(const Irange<UtfIterator<C>>& range, const C* target)`
* `template <typename C> UtfIterator<C> str_find_last_not_of(const basic_string<C>& str, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_last_not_of(const basic_string<C>& str, const C* target)`
* `template <typename C> UtfIterator<C> str_find_last_not_of(const UtfIterator<C>& begin, const UtfIterator<C>& end, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_last_not_of(const UtfIterator<C>& begin, const UtfIterator<C>& end, const C* target)`
* `template <typename C> UtfIterator<C> str_find_last_not_of(const Irange<UtfIterator<C>>& range, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_find_last_not_of(const Irange<UtfIterator<C>>& range, const C* target)`

These find the first or last character in their subject range that is in, or
not in, the target list of characters. They return an end iterator if no
matching character is found. (They are essentially the same as the similarly
named member functions in standard strings, except that they work on
characters instead of code units.)

* `template <typename C> UtfIterator<C> str_search(const basic_string<C>& str, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_search(const basic_string<C>& str, const C* target)`
* `template <typename C> UtfIterator<C> str_search(const UtfIterator<C>& begin, const UtfIterator<C>& end, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_search(const UtfIterator<C>& begin, const UtfIterator<C>& end, const C* target)`
* `template <typename C> UtfIterator<C> str_search(const Irange<UtfIterator<C>>& range, const basic_string<C>& target)`
* `template <typename C> UtfIterator<C> str_search(const Irange<UtfIterator<C>>& range, const C* target)`

Find the first occurrence of the target substring in the subject range,
returning an iterator pointing to the beginning of the located substring, or
an end iterator if it was not found.

* `template <typename C> size_t str_skipws(UtfIterator<C>& i)`
* `template <typename C> size_t str_skipws(UtfIterator<C>& i, const UtfIterator<C>& end)`

Advances `i` to point to the next non-whitespace character, or the end of the
string if no such character was found. Optionally an endpoint other than the
end of the string can be supplied. The return value is the number of
characters skipped.

## String manipulation functions ##

* `template <typename C, typename C2> void str_append(basic_string<C>& str, const basic_string<C2>& suffix)`
* `template <typename C, typename C2> void str_append(basic_string<C>& str, const Irange<UtfIterator<C2>>& suffix)`
* `template <typename C, typename C2> void str_append(basic_string<C>& str, const UtfIterator<C2>& suffix_begin, const UtfIterator<C2>& suffix_end)`
* `template <typename C, typename C2> void str_append(basic_string<C>& str, const C2* suffix)`
* `template <typename C, typename C2> void str_append(basic_string<C>& dst, const C2* ptr, size_t n)`
* `template <typename C, typename C2, typename... Chars> void str_append_char(basic_string<C>& dst, C2 c2, Chars... chars)`
* `template <typename C> void str_append_chars(basic_string<C>& dst, char32_t c, size_t n)`

These append one or more characters to a Unicode string, performing any
necessary encoding conversions.

* `template <typename C> basic_string<C> str_change(const Irange<UtfIterator<C>>& dst, const basic_string<C>& src)`
* `template <typename C> basic_string<C> str_change(const Irange<UtfIterator<C>>& dst, const Irange<UtfIterator<C>>& src)`
* `template <typename C> basic_string<C> str_change(const UtfIterator<C>& dst_begin, const UtfIterator<C>& dst_end, const basic_string<C>& src)`
* `template <typename C> basic_string<C> str_change(const UtfIterator<C>& dst_begin, const UtfIterator<C>& dst_end, const UtfIterator<C>& src_begin, const UtfIterator<C>& src_end)`
* `template <typename C> Irange<UtfIterator<C>> str_change_in(basic_string<C>& dst, const Irange<UtfIterator<C>>& range, const basic_string<C>& src)`
* `template <typename C> Irange<UtfIterator<C>> str_change_in(basic_string<C>& dst, const Irange<UtfIterator<C>>& range, const Irange<UtfIterator<C>>& src)`
* `template <typename C> Irange<UtfIterator<C>> str_change_in(basic_string<C>& dst, const UtfIterator<C>& range_begin, const UtfIterator<C>& range_end, const basic_string<C>& src)`
* `template <typename C> Irange<UtfIterator<C>> str_change_in(basic_string<C>& dst, const UtfIterator<C>& range_begin, const UtfIterator<C>& range_end, const UtfIterator<C>& src_begin, const UtfIterator<C>& src_end)`

These replace the destination substring with a copy of the source string. The
effect is the same as the `basic_string::replace()` methods, except that
positions within the string are specified by UTF iterators instead of ordinary
string iterators or offsets. The `str_change()` functions return the modified
string, obtained by taking the underlying string from the destination
iterators, and replacing the delimited substring with the source string; the
`str_change_in()` functions update the destination string in place, replacing
the substring delimited by `range` with the source string, and return a pair
of iterators delimiting the newly inserted replacement string within the
updated `dst`.

* `template <typename C> basic_string<C> str_chars(char32_t c, size_t n = 1)`

Returns a string containing `n` copies of the character, in the appropriate
encoding.

* `template <typename C, typename... Strings> basic_string<C> str_concat(const basic_string<C>& s, const Strings&... ss)`
* `template <typename C, typename... Strings> basic_string<C> str_concat(const C* s, const Strings&... ss)`
* `template <typename C, typename... Strings> basic_string<C> str_concat_with(const basic_string<C>& delim, const Strings&... ss)`
* `template <typename C, typename... Strings> basic_string<C> str_concat_with(const C* delim, const Strings&... ss)`

These concatenate one or more strings, which can be an arbitrary mixture of
different Unicode encodings. The `str_concat_with()` versions insert a
delimiter between each pair of strings. The encoding type of the returned
string matches that of the first argument (the delimiter in
`str_concat_with()`).

* `template <typename C> basic_string<C> str_drop_prefix(const basic_string<C>& str, const basic_string<C>& prefix)`
* `template <typename C> basic_string<C> str_drop_prefix(const basic_string<C>& str, const C* prefix)`
* `template <typename C> void str_drop_prefix_in(basic_string<C>& str, const basic_string<C>& prefix) noexcept`
* `template <typename C> void str_drop_prefix_in(basic_string<C>& str, const C* prefix) noexcept`
* `template <typename C> basic_string<C> str_drop_suffix(const basic_string<C>& str, const basic_string<C>& suffix)`
* `template <typename C> basic_string<C> str_drop_suffix(const basic_string<C>& str, const C* suffix)`
* `template <typename C> void str_drop_suffix_in(basic_string<C>& str, const basic_string<C>& suffix) noexcept`
* `template <typename C> void str_drop_suffix_in(basic_string<C>& str, const C* suffix) noexcept`

If the first argument string starts or ends with the given prefix or suffix,
remove it; otherwise, just return the original string unchanged.

* `template <typename C> basic_string<C> str_erase_left(const basic_string<C>& str, size_t length)`
* `template <typename C> void str_erase_left_in(basic_string<C>& str, size_t length) noexcept`
* `template <typename C> basic_string<C> str_erase_right(const basic_string<C>& str, size_t length)`
* `template <typename C> void str_erase_right_in(basic_string<C>& str, size_t length) noexcept`

Erase the specified number of Unicode characters from the beginning or end of
the string. These will return an empty string if `length` is greater than the
number of characters in `str`.

* `template <typename C> basic_string<C> str_expand_tabs(const basic_string<C>& str)`
* `template <typename C, typename IntList> basic_string<C> str_expand_tabs(const basic_string<C>& str, const IntList& tabs, Flagset flags = {})`
* `template <typename C, typename IntType> basic_string<C> str_expand_tabs(const basic_string<C>& str, initializer_list<IntType> tabs, Flagset flags = {})`
* `template <typename C> void str_expand_tabs_in(basic_string<C>& str)`
* `template <typename C, typename IntList> void str_expand_tabs_in(basic_string<C>& str, const IntList& tabs, Flagset flags = {})`
* `template <typename C, typename IntType> void str_expand_tabs_in(basic_string<C>& str, initializer_list<IntType> tabs, Flagset flags = {})`

Expand tab characters to spaces. If the input string contains multiple lines
(delimited by any of the standard Unicode line break characters), each line
will be expanded separately. The `tabs` argument is a list of tab positions,
passed either as a range of integers, or as an explicit braced initializer
list of integers. The `flags` argument indicates which units will be used to
measure horizontal position.

By default, a tab stop every 8 columns is assumed. Tab stop positions that are
less than or equal to the previous tab stop are ignored. If more tab stops
beyond the last one listed are needed, the difference between the last two tab
stops is used to increment the last one (e.g. `{5,10,20}` will be expanded to
`{5,10,20,30,40,...}`). An implicit tab stop at position zero is always
assumed.

* `template <typename C> basic_string<C> str_fix_left(const basic_string<C>& str, size_t length, char32_t c = U' ', Flagset flags = {})`
* `template <typename C> void str_fix_left_in(basic_string<C>& str, size_t length, char32_t c = U' ', Flagset flags = {})`
* `template <typename C> basic_string<C> str_fix_right(const basic_string<C>& str, size_t length, char32_t c = U' ', Flagset flags = {})`
* `template <typename C> void str_fix_right_in(basic_string<C>& str, size_t length, char32_t c = U' ', Flagset flags = {})`

Pad or truncate a string to a specific length; the character argument `c` is
used for padding (converted to the appropriate encoding). The `str_fix_left()`
function anchors the string on the left and pads or truncates on the right;
this is similar to `basic_string::resize()`, except that the flags determine
how the length of the string is measured. The `str_fix_right()` function
anchors the string on the right and pads or truncates on the left. If the
string can't be adjusted to exactly the specified size (because one of the
East Asian width options was selected and wide characters are present), the
result will be one unit longer than the requested length.

* `template <typename FwdRange> basic_string<[character type]> str_join(const FwdRange& r)`
* `template <typename FwdRange, typename C> basic_string<C> str_join(const FwdRange& r, const basic_string<C>& delim)`
* `template <typename FwdRange, typename C> basic_string<C> str_join(const FwdRange& r, const C* delim)`

These concatenate a list of strings, optionally inserting a delimiter between
each pair of strings. The character types of the string list and the delimiter
must match.

* `template <typename C> basic_string<C> str_pad_left(const basic_string<C>& str, size_t length, char32_t c = U' ', Flagset flags = {})`
* `template <typename C> void str_pad_left_in(basic_string<C>& str, size_t length, char32_t c = U' ', Flagset flags = {})`
* `template <typename C> basic_string<C> str_pad_right(const basic_string<C>& str, size_t length, char32_t c = U' ', Flagset flags = {})`
* `template <typename C> void str_pad_right_in(basic_string<C>& str, size_t length, char32_t c = U' ', Flagset flags = {})`

Pad a string on the left or right to a specified length; the character
argument `c` is used for padding (converted to the appropriate encoding). The
string will be returned unchanged if it is already equal to or longer than the
required length. If the string can't be adjusted to exactly the specified size
(because one of the East Asian width options was selected and wide characters
are present), the result will be one unit longer than the requested length.

* `template <typename C> bool str_partition(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix)`
* `template <typename C> bool str_partition_at(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix, const basic_string<C>& delim)`
* `template <typename C> bool str_partition_at(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix, const C* delim)`
* `template <typename C> bool str_partition_by(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix, const basic_string<C>& delim)`
* `template <typename C> bool str_partition_by(const basic_string<C>& str, basic_string<C>& prefix, basic_string<C>& suffix, const C* delim)`

These split a string into two parts at the first occurrence of a given
delimiter. If the delimiter is found, the two parts are written into `prefix`
and `suffix` (the delimiter itself is discarded), and the function returns
`true`; otherwise, the original string is copied into `prefix`, `suffix` is
made empty, and the function returns `false`. The `str_partition()` function
splits the string on the first contiguous sequence of whitespace characters;
`str_partition_at()` splits it at the first occurrence of the `delim` string;
and `str_partition_by()` splits it at the first contiguous sequence of
characters that are in the `delim` list. In `str_partition_at()` and
`str_partition_by()`, an empty delimiter string will be treated as never being
found.

* `template <typename C> basic_string<C> str_remove(const basic_string<C>& str, char32_t c)`
* `template <typename C> basic_string<C> str_remove(const basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> basic_string<C> str_remove(const basic_string<C>& str, const C* chars)`
* `template <typename C, typename Pred> basic_string<C> str_remove_if(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> basic_string<C> str_remove_if_not(const basic_string<C>& str, Pred p)`
* `template <typename C> void str_remove_in(basic_string<C>& str, char32_t c)`
* `template <typename C> void str_remove_in(basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> void str_remove_in(basic_string<C>& str, const C* chars)`
* `template <typename C, typename Pred> void str_remove_in_if(basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> void str_remove_in_if_not(basic_string<C>& str, Pred p)`

These remove a specific character, all characters in a set, or characters
matching (or not matching) a condition from the string.

* `template <typename C> basic_string<C> str_repeat(const basic_string<C>& str, size_t n)`
* `template <typename C> basic_string<C> str_repeat(const C* str, size_t n)`
* `template <typename C> void str_repeat_in(basic_string<C>& str, size_t n)`

Return a string formed by concatenating `n` copies of the original string.

* `template <typename C> basic_string<C> str_replace(const basic_string<C>& str, const basic_string<C>& target, const basic_string<C>& sub, size_t n = npos)`
* `template <typename C> basic_string<C> str_replace(const basic_string<C>& str, const C* target, const basic_string<C>& sub, size_t n = npos)`
* `template <typename C> basic_string<C> str_replace(const basic_string<C>& str, const basic_string<C>& target, const C* sub, size_t n = npos)`
* `template <typename C> basic_string<C> str_replace(const basic_string<C>& str, const C* target, const C* sub, size_t n = npos)`
* `template <typename C> void str_replace_in(basic_string<C>& str, const basic_string<C>& target, const basic_string<C>& sub, size_t n = npos)`
* `template <typename C> void str_replace_in(basic_string<C>& str, const C* target, const basic_string<C>& sub, size_t n = npos)`
* `template <typename C> void str_replace_in(basic_string<C>& str, const basic_string<C>& target, const C* sub, size_t n = npos)`
* `template <typename C> void str_replace_in(basic_string<C>& str, const C* target, const C* sub, size_t n = npos)`

These return a copy of the first argument string, with the first `n`
substrings that match `target` replaced with `sub`. By default, all matches
are replaced. The string will be returned unchanged if `target` is empty or
`n=0`.

* `template <typename C, typename OutIter> void str_split(const basic_string<C>& src, OutIter dst)`
* `template <typename C, typename OutIter> void str_split_at(const basic_string<C>& src, OutIter dst, const basic_string<C>& delim)`
* `template <typename C, typename OutIter> void str_split_at(const basic_string<C>& src, OutIter dst, const C* delim)`
* `template <typename C, typename OutIter> void str_split_by(const basic_string<C>& src, OutIter dst, const basic_string<C>& delim)`
* `template <typename C, typename OutIter> void str_split_by(const basic_string<C>& src, OutIter dst, const C* delim)`

These split a string into substrings, using the specified delimiter to mark
the substring boundaries, and copying the resulting substrings into the
destination defined by the output iterator. The `str_split()` function splits
the string on each contiguous sequence of whitespace characters;
`str_split_at()` splits it at each occurrence of the `delim` string; and
`str_split_by()` splits it at each contiguous sequence of characters that are
in the `delim` list. Nothing will be written if the original source string is
empty; if the delimiter string is empty (but the source string is not), a
single string will be written.

* `template <typename C> basic_string<C> str_squeeze(const basic_string<C>& str)`
* `template <typename C> basic_string<C> str_squeeze(const basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> basic_string<C> str_squeeze(const basic_string<C>& str, const C* chars)`
* `template <typename C> basic_string<C> str_squeeze_trim(const basic_string<C>& str)`
* `template <typename C> basic_string<C> str_squeeze_trim(const basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> basic_string<C> str_squeeze_trim(const basic_string<C>& str, const C* chars)`
* `template <typename C> void str_squeeze_in(basic_string<C>& str)`
* `template <typename C> void str_squeeze_in(basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> void str_squeeze_in(basic_string<C>& str, const C* chars)`
* `template <typename C> void str_squeeze_trim_in(basic_string<C>& str)`
* `template <typename C> void str_squeeze_trim_in(basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> void str_squeeze_trim_in(basic_string<C>& str, const C* chars)`

These replace every sequence of one or more characters from `chars` with the
first character in `chars`. By default, if `chars` is not supplied, every
sequence of whitespace characters will be replaced with a single space. The
`str_squeeze_trim()` functions do the same thing, except that leading and
trailing characters from `chars` are removed completely instead of reduced to
one character. In all cases, the original string will be left unchanged if
`chars` is empty.

* `template <typename C> basic_string<C> str_substring(const basic_string<C>& str, size_t offset, size_t count = npos)`
* `template <typename C> basic_string<C> utf_substring(const basic_string<C>& str, size_t index, size_t length = npos, Flagset flags = {})`

These return a substring of the original string. The `str_substring()` function
returns the same string as `basic_string::substr()`, except that an offset out
of bounds will yield an empty string instead of undefined behaviour;
`utf_substring()` does the same thing, except that the position and length of
the substring are measured according according to the `flags` argument instead
of by code units (the flags are the same as for `str_length()`, defaulting to
characters).

* `template <typename C> basic_string<C> str_translate(const basic_string<C>& str, const basic_string<C>& target, const basic_string<C>& sub)`
* `template <typename C> basic_string<C> str_translate(const basic_string<C>& str, const basic_string<C>& target, const C* sub)`
* `template <typename C> basic_string<C> str_translate(const basic_string<C>& str, const C* target, const basic_string<C>& sub)`
* `template <typename C> basic_string<C> str_translate(const basic_string<C>& str, const C* target, const C* sub)`
* `template <typename C> void str_translate_in(basic_string<C>& str, const basic_string<C>& target, const basic_string<C>& sub)`
* `template <typename C> void str_translate_in(basic_string<C>& str, const basic_string<C>& target, const C* sub)`
* `template <typename C> void str_translate_in(basic_string<C>& str, const C* target, const basic_string<C>& sub)`
* `template <typename C> void str_translate_in(basic_string<C>& str, const C* target, const C* sub)`

These return a copy of the first argument string, with any characters that
occur in `target` replaced with the corresponding character in `sub`. The
string will be returned unchanged if either `target` or `sub` is empty. If
`target` is longer than `sub`, `sub` will be extended to match the length of
`target` by repeating its last character; if `target` is shorter than `sub`,
the extra characters in `sub` are ignored. If the same character occurs more
than once in `target`, only the first is used. (This function is similar to
the Unix `tr` utility.)

* `template <typename C> basic_string<C> str_trim(const basic_string<C>& str)`
* `template <typename C> basic_string<C> str_trim(const basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> basic_string<C> str_trim(const basic_string<C>& str, const C* chars)`
* `template <typename C> void str_trim_in(basic_string<C>& str)`
* `template <typename C> void str_trim_in(basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> void str_trim_in(basic_string<C>& str, const C* chars)`
* `template <typename C> basic_string<C> str_trim_left(const basic_string<C>& str)`
* `template <typename C> basic_string<C> str_trim_left(const basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> basic_string<C> str_trim_left(const basic_string<C>& str, const C* chars)`
* `template <typename C> void str_trim_left_in(basic_string<C>& str)`
* `template <typename C> void str_trim_left_in(basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> void str_trim_left_in(basic_string<C>& str, const C* chars)`
* `template <typename C> basic_string<C> str_trim_right(const basic_string<C>& str)`
* `template <typename C> basic_string<C> str_trim_right(const basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> basic_string<C> str_trim_right(const basic_string<C>& str, const C* chars)`
* `template <typename C> void str_trim_right_in(basic_string<C>& str)`
* `template <typename C> void str_trim_right_in(basic_string<C>& str, const basic_string<C>& chars)`
* `template <typename C> void str_trim_right_in(basic_string<C>& str, const C* chars)`
* `template <typename C, typename Pred> basic_string<C> str_trim_if(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> basic_string<C> str_trim_if_not(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> void str_trim_in_if(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> void str_trim_in_if_not(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> basic_string<C> str_trim_left_if(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> basic_string<C> str_trim_left_if_not(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> void str_trim_left_in_if(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> void str_trim_left_in_if_not(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> basic_string<C> str_trim_right_if(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> basic_string<C> str_trim_right_if_not(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> void str_trim_right_in_if(const basic_string<C>& str, Pred p)`
* `template <typename C, typename Pred> void str_trim_right_in_if_not(const basic_string<C>& str, Pred p)`

These trim unwanted characters from one or both ends of the string. By
default, any whitespace characters (according to the Unicode property) are
stripped; alternatively, you can supply a string containing the list of
unwanted characters, or a predicate function that takes a character and
returns `true` if the character should be trimmed. (Note that the predicate
always takes a Unicode character, i.e. a `char32_t`, regardless of the code
unit type, `C`.)

* `template <typename C> basic_string<C> str_unify_lines(const basic_string<C>& str)`
* `template <typename C> basic_string<C> str_unify_lines(const basic_string<C>& str, char32_t newline)`
* `template <typename C> basic_string<C> str_unify_lines(const basic_string<C>& str, const basic_string<C>& newline)`
* `template <typename C> basic_string<C> str_unify_lines(const basic_string<C>& str, const C* newline)`
* `template <typename C> void str_unify_lines_in(basic_string<C>& str)`
* `template <typename C> void str_unify_lines_in(basic_string<C>& str, char32_t newline)`
* `template <typename C> void str_unify_lines_in(basic_string<C>& str, const basic_string<C>& newline)`
* `template <typename C> void str_unify_lines_in(basic_string<C>& str, const C* newline)`

These convert all line breaks to the same form, a single `LF` by default. Any
Unicode line or paragraph breaking character is recognised and replaced; the
`CR+LF` sequence is also treated as a single line break.

* `template <typename C> basic_string<C> str_wrap(const basic_string<C>& str, Flagset flags = {}, size_t width = 0, size_t margin1 = 0, size_t margin2 = npos)`
* `template <typename C> void str_wrap_in(basic_string<C>& str, Flagset flags = {}, size_t width = 0, size_t margin1 = 0, size_t margin2 = npos)`

Bitmask          | Letter  | Description
-------          | ------  | -----------
`wrap_crlf`      | `r`     | Use CR+LF for line breaks (default LF)
`wrap_enforce`   | `e`     | Enforce right margin strictly
`wrap_preserve`  | `p`     | Preserve layout on already indented lines

Wrap the text in a string to a given width. Wrapping is done separately for
each paragraph; paragraphs are delimited by two or more line breaks (as usual,
`CR+LF` is counted as a single line break), or a single paragraph separator
character (`U+2029`). Words are simply delimited by whitespace, which may not
be appropriate for all languages; no attempt is made at anything more
sophisticated such as hyphenation or locale-specific word breaking rules.

If the `width` argument is zero or `npos`, the width is set to two characters
less than the current terminal width, obtained from the `COLUMNS` environment
variable; the terminal width is assumed to be 80 characters if `COLUMNS` is
undefined or invalid. The `margin1` and `margin2` arguments determine the
number of spaces used to indent the first and subsequent lines, respectively,
of a paragraph (the width includes the indentation). If `margin2=npos`,
`margin1` is used for all lines. The function will throw `std::length_error`
if either margin is greater than or equal to the width.

Any line breaking already present in the input text is discarded, except for
the special behaviour described for `wrap_preserve` below.

The `flags` argument determines the details of the word wrapping behaviour. In
addition to the flags listed above, the standard flags for determining string
length are respected.

By default, a single `LF` is used to break lines; setting `wrap_crlf` causes
`CR+LF` to be used instead.

If a single word is too long to fit on one line, the default behaviour is to
allow it to violate the right margin. If the `wrap_enforce` flag is set, this
will cause the function to throw `std::length_error` instead.

If the `wrap_preserve` flag is set, any paragraphs that start with an indented
line are left in their original format.

## Case mapping functions ##

* `template <typename C> basic_string<C> str_uppercase(const basic_string<C>& str)`
* `template <typename C> void str_uppercase_in(basic_string<C>& str)`
* `template <typename C> basic_string<C> str_lowercase(const basic_string<C>& str)`
* `template <typename C> void str_lowercase_in(basic_string<C>& str)`
* `template <typename C> basic_string<C> str_titlecase(const basic_string<C>& str)`
* `template <typename C> void str_titlecase_in(basic_string<C>& str)`
* `template <typename C> basic_string<C> str_casefold(const basic_string<C>& str)`
* `template <typename C> void str_casefold_in(basic_string<C>& str)`

These convert a string to upper case, lower case, title case, or the case
folded form (the form recommended by Unicode for case insensitive string
comparison; this is similar, but not always identical, to the lower case
form). These use the full Unicode case mappings; the returned string will not
necessarily be the same length as the original string (measured either in code
units or characters). These functions only perform the default case mappings
recommended by the Unicode standard; they do not make any attempt at
localisation.

* `struct IcaseEqual`
    * `template <typename C> bool IcaseEqual::operator()(const basic_string<C>& lhs, const basic_string<C>& rhs) const noexcept`
* `struct IcaseCompare`
    * `template <typename C> bool IcaseCompare::operator()(const basic_string<C>& lhs, const basic_string<C>& rhs) const noexcept`
* `constexpr IcaseEqual str_icase_equal`
* `constexpr IcaseCompare str_icase_compare`

Function objects that perform case insensitive string comparison, with
equality or less-than semantics. These are equivalent to calling `casefold()`
on the argument strings before comparison; using these functions is usually
more efficient for a small number of comparisons, while calling `casefold()`
and saving the case folded form of the string will be more efficient if the
same string is going to be compared frequently.

## Type conversion functions ##

* `template <typename T, typename C> size_t str_to_integer(T& t, const basic_string<C>& str, size_t offset = 0)`
* `template <typename T, typename C> T str_to_integer(const basic_string<C>& str, size_t offset = 0)`
* `template <typename T, typename C> size_t hex_to_integer(T& t, const basic_string<C>& str, size_t offset = 0)`
* `template <typename T, typename C> T hex_to_integer(const basic_string<C>& str, size_t offset = 0)`
* `template <typename T, typename C> size_t str_to_real(T& t, const basic_string<C>& str, size_t offset = 0)`
* `template <typename T, typename C> T str_to_real(const basic_string<C>& str, size_t offset = 0)`

Conversions from a string to an integer (in decimal or hexadecimal) or a
floating point number. In each pair of functions, the first version writes the
result into a variable passed by reference, and returns the number of
characters read from the string; the second version returns the result, and
requires the return type to be explicitly specified at the call site.

A value out of range for the return type will be clamped to the nearest end of
its valid range. The result will be zero if the string does not contain a
valid number. Any characters after a valid number are ignored. Note that,
unlike the otherwise similar `strtol()` and related functions, these do not
skip leading whitespace.

All of the characters that might be part of a number are in the ASCII range,
so the return value of the first version of each function does not need to
distinguish between characters and code points.
