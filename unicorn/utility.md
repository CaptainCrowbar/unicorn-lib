# [Unicorn Library](index.html): Utilities #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/utility.hpp"`

This module contains some basic utility functions and related definitions,
used by the Unicorn library but not directly related to Unicode processing.
These are in `namespace RS` (except for the metaprogramming facilities in
`RS::Meta`), not `RS::Unicorn`.

## Contents ##

[TOC]

## Basic types ##

* `using` **`Ustring`** `= std::string`
* `using` **`Uview`** `= std::string_view`

Use `Ustring` or `Uview` for strings that are expected to be in UTF-8 (or
ASCII, since any ASCII string is also valid UTF-8), while plain `std::string`
or `std::string_view` is used where the string is expected to be in some other
Unicode encoding, or where the string is being used simply as an array of
bytes rather than encoded text.

* `using` **`Strings`** `= std::vector<std::string>`

Commonly used type defined for convenience.

* `#define` **`RS_NATIVE_WCHAR`** `1` _- defined if the system API uses wide characters_
* `using` **`NativeCharacter`** `= [char on Unix, wchar_t on Windows]`
* `using` **`NativeString`** `= [std::string on Unix, std::wstring on Windows]`

These are defined to reflect the character types used in the operating
system's native API.

* `#define` **`RS_WCHAR_UTF16`** `1` _- defined if wchar_t and wstring are UTF-16_
* `#define` **`RS_WCHAR_UTF32`** `1` _- defined if wchar_t and wstring are UTF-32_
* `using` **`WcharEquivalent`** `= [char16_t or char32_t]`
* `using` **`WstringEquivalent`** `= [std::u16string or std::u32string]`

These are defined to reflect the encoding represented by `wchar_t` and
`std::wstring`. Systems where wide strings are neither UTF-16 nor UTF-32 are
not supported.

## Constants ##

* `constexpr const char*` **`ascii_whitespace`** `= "\t\n\v\f\r "`

ASCII whitespace characters.

* `constexpr bool` **`big_endian_target`**
* `constexpr bool` **`little_endian_target`**

One of these will be true and the other false, reflecting the target system's
byte order.

* `template <typename T> constexpr bool` **`dependent_false`** ` = false`

Conventional workaround for an always-false `static_assert()` in the final
`else` clause of an `if constexpr` statement.

* `constexpr size_t` **`npos`** `= std::string::npos`

Defined for convenience. Following the conventions established by the standard
library, this value is often used as a function argument to mean "as large as
possible" or "no limit", or as a return value to mean "not found".

## Algorithms ##

* `template <typename Container> [output iterator]` **`append`**`(Container& con)`
* `template <typename Container> [output iterator]` **`overwrite`**`(Container& con)`
* `template <typename Range, typename Container> const Range&` **`operator>>`**`(const Range& lhs, [append iterator] rhs)`

The `append()` and `overwrite()` functions create output iterators that will
append elements to a standard container (see `append_to()` below). The
`append()` function is similar to `std::back_inserter()` (but supports
containers without `push_back()`), while `overwrite()` will first clear the
container and then return the append iterator. There is also an operator that
can be used to copy any range into a container (e.g. `range >> append(con)`).

* `template <typename Container, typename T> void` **`append_to`**`(Container& con, const T& t)`

Appends an item to a container; used by `append()` and `overwrite()`. The
generic version calls `con.insert(con.end(), t)`; overloads (found by argument
dependent lookup) can be used for container-like types that do not have a
suitable `insert()` method.

* `template <typename Range1, typename Range2> int` **`compare_3way`**`(const Range1& r1, const Range2& r2)`
* `template <typename Range1, typename Range2, typename Compare> int` **`compare_3way`**`(const Range1& r1, const Range2& r2, Compare cmp)`

Compare two ranges, returning -1 if the first range is less than the second,
zero if they are equal, and +1 if the first range is greater.

## Arithmetic functions ##

For the bit manipulation functions (`ibits()`, `ifloor2()`, `iceil2()`,
`ilog2p1()`, `ispow2()`, `rotl()`, and `rotr()`), behaviour is undefined if
`T` is not an integer, or if `T` is signed and the argument is negative.

* `template <typename T, typename T2, typename T3> constexpr T` **`clamp`**`(const T& x, const T2& min, const T3& max) noexcept`

Clamps a value to a fixed range. This returns `min` if `t<min`, `max` if
`t>max`, otherwise `t`. `T2` and `T3` must be implicitly convertible t `T`.

* `template <typename T> constexpr std::make_signed_t<T>` **`as_signed`**`(T t) noexcept`
* `template <typename T> constexpr std::make_unsigned_t<T>` **`as_unsigned`**`(T t) noexcept`

These return their argument converted to a signed or unsigned value of the
same size (the argument is returned unchanged if `T` already had the desired
signedness). Behaviour is undefined if `T` is not an integer or enumeration
type.

* `template <typename T> constexpr int` **`ibits`**`(T t) noexcept`

Returns the number of 1 bits in the argument.

* `template <typename T> constexpr T` **`ifloor2`**`(T t) noexcept`
* `template <typename T> constexpr T` **`iceil2`**`(T t) noexcept`

Return the argument rounded down or up to a power of 2. For `iceil2()`,
behaviour is undefined if the argument is large enough that the correct answer
is not representable.

* `template <typename T> constexpr int` **`ilog2p1`**`(T t) noexcept`

Returns `floor(log2(t))+1`, equal to the number of significant bits in `t`,
or zero if `t` is zero.

* `template <typename T> constexpr bool` **`ispow2`**`(T t) noexcept`

True if the argument is an exact power of 2.

* `constexpr uint64_t` **`letter_to_mask`**`(char c) noexcept`

Converts a letter to a mask with bit 0-51 set (corresponding to `[A-Za-z]`).
Returns zero if the argument is not an ASCII letter.

* `template <typename T> constexpr T` **`rotl`**`(T t, int n) noexcept`
* `template <typename T> constexpr T` **`rotr`**`(T t, int n) noexcept`

Bitwise rotate left or right. The bit count is reduced modulo the number of
bits in `T`; a negative shift in one direction is treated as a positive shift
in the other.

## Date and time functions ##

* `constexpr uint32_t` **`utc_zone`**
* `constexpr uint32_t` **`local_zone`**

Used to indicate whether a date is expressed in UTC or the local time zone.

* `Ustring` **`format_date`**`(system_clock::time_point tp, int prec = 0, uint32_t flags = utc_zone)`
* `Ustring` **`format_date`**`(system_clock::time_point tp, Uview format, uint32_t flags = utc_zone)`

These convert a time point into a broken down date and format it. The first
version writes the date in ISO 8601 format (`"yyyy-mm-dd hh:mm:ss"`). If
`prec` is greater than zero, the specified number of decimal places will be
added to the seconds field.

The second version writes the date using the conventions of `strftime()`. This
will return an empty string if anything goes wrong (there is no way to
distinguish between a conversion error and a legitimately empty result; this
is a limitation of `strftime()`).

Both of these will throw `std::invalid_argument` if an invalid flag is passed.

For reference, the portable subset of the `strftime()` formatting codes are:

| Code  | Description                          | Code    | Description                          |
| ----  | -----------                          | ----    | -----------                          |
|       **Date elements**                      | |       **Weekday elements**                   | |
| `%Y`  | Year number                          | `%a`    | Local weekday abbreviation           |
| `%y`  | Last 2 digits of the year (`00-99`)  | `%w`    | Sunday-based weekday number (`0-6`)  |
| `%m`  | Month number (`00-12`)               | `%A`    | Local weekday name                   |
| `%B`  | Local month name                     | `%U`    | Sunday-based week number (`00-53`)   |
| `%b`  | Local month abbreviation             | `%W`    | Monday-based week number (`00-53`)   |
| `%d`  | Day of the month (`01-31`)           |         **Other elements**                     | |
|       **Time of day elements**               | | `%c`  | Local standard date/time format      |
| `%H`  | Hour on 24-hour clock (`00-23`)      | `%x`    | Local standard date format           |
| `%I`  | Hour on 12-hour clock (`01-12`)      | `%X`    | Local standard time format           |
| `%p`  | Local equivalent of a.m./p.m.        | `%j`    | Day of the year (`001-366`)          |
| `%M`  | Minute (`00-59`)                     | `%Z`    | Time zone name                       |
| `%S`  | Second (`00-60`)                     | `%z`    | Time zone offset                     |

* `template <typename R, typename P> Ustring` **`format_time`**`(const duration<R, P>& time, int prec = 0)`

Formats a time duration in days, hours, minutes, seconds, and (if `prec>0`)
fractions of a second.

* `template <typename R, typename P> void` **`from_seconds`**`(double s, duration<R, P>& d) noexcept`
* `template <typename R, typename P> double` **`to_seconds`**`(const duration<R, P>& d) noexcept`

Convenience functions to convert between a `duration` and a floating point
number of seconds.

* `system_clock::time_point` **`make_date`**`(int year, int month, int day, int hour = 0, int min = 0, double sec = 0, uint32_t flags = utc_zone)`

Converts a broken down date into a time point. Behaviour if any of the date
arguments are invalid follows the same rules as `mktime()`. This will throw
`std::invalid_argument` if an invalid flag is passed.

## Error handling ##

* `void` **`runtime_assert`**`(bool condition, std::string_view message) noexcept`

A portable, non-blockable version of `assert()`. If the condition is false,
this will print the message to standard error (followed by a line break), and
then call `abort()`.

## Keyword arguments ##

* `template <typename T, int ID = 0> struct` **`Kwarg`**
    * `constexpr ... Kwarg::`**`operator=`**`(const T& t) const noexcept`
* `template <typename T, int ID, typename... Args> constexpr bool` **`kwtest`**`(Kwarg<T, ID> key, Args... args)`
* `template <typename T, int ID, typename... Args> T` **`kwget`**`(Kwarg<T, ID> key, const T& def, Args... args)`

This provides a simple implementation of variadic keyword arguments for C++
functions.

Define a `Kwarg<T[,ID]>` object for each keyword argument, where `T` is the
argument type. The `ID` parameter is only needed to distinguish between
keywords with the same argument type. Functions that will take keyword
arguments should be declared with a variadic argument pack, possibly preceded
by ordinary positional arguments.

When calling the function, the keyword arguments should be supplied in the
form `key=value`, where `key` is a `Kwarg` object, and `value` is the argument
value. The value type must be convertible to `T`. If `T` is `bool`, the
keyword alone can be passed as an argument, with the value defaulting to
`true`.

In the function body, call `kwget()` or `kwtest()` for each possible keyword
argument, with the corresponding `Kwarg` object as the key, a default value
(for `kwget()`), and the variadic arguments from the enclosing function. The
`kwget()` function returns the value attached to the keyword, or the default
value if the keyword was not found in the argument list; `kwtest()` returns
whether or not the keyword was present. If the same keyword appears more than
once in the actual argument list, the first one found will be returned.

Example:

    class Window {
    public:
        static constexpr Kwarg<int, 1> width = {};
        static constexpr Kwarg<int, 2> height = {};
        static constexpr Kwarg<std::string> title = {};
        static constexpr Kwarg<bool> visible = {};
        template <typename... Args> explicit Window(Args... args) {
            int win_width = kwget(width, 640, args...);
            int win_height = kwget(height, 480, args...);
            string title_text = kwget(title, "New Window"s, args...);
            bool is_visible = kwget(visible, false, args...);
            // ...
        }
    };

    Window app_window(Window::title="Hello World", Window::width=1000, Window::height=750, Window::visible);

## Metaprogramming ##

* `namespace RS::Meta`
    * `template <template <typename...> typename Archetype, typename... Args> struct` **`IsDetected`**
    * `template <template <typename...> typename Archetype, typename... Args> constexpr bool` **`is_detected`**
        * _True if the archetype is a valid expression for the supplied argument types_
    * `template <typename Default, template <typename...> typename Archetype, typename... Args> struct` **`DetectedOr`**
        * _Returns the type defined by the archetype if it is valid, otherwise the default type_
    * `template <typename Result, template <typename...> typename Archetype, typename... Args> struct` **`IsDetectedExact`**
    * `template <typename Result, template <typename...> typename Archetype, typename... Args> constexpr bool` **`is_detected_exact`**
        * _True if the archetype is valid and returns a specific type_
    * `template <typename Result, template <typename...> typename Archetype, typename... Args> struct` **`IsDetectedConvertible`**
    * `template <typename Result, template <typename...> typename Archetype, typename... Args> constexpr bool` **`is_detected_convertible`**
        * _True if the archetype is valid and returns a type convertible to the specified type_

Reflection primitives, based on Walter E. Brown's
[N4502 Proposing Standard Library Support for the C++ Detection Idiom V2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf).

* `namespace RS::Meta`
    * `template <typename T> struct` **`IsIterator`**
    * `template <typename T> constexpr bool` **`is_iterator`** `= IsIterator<T>::value`
        * _True if the type is an iterator (detected by checking `iterator_traits`)_
    * `template <typename T> struct` **`IsRange`**
    * `template <typename T> constexpr bool` **`is_range`** `= IsRange<T>::value`
        * _True if the type is a range (detected by checking for `begin()` and `end()`)_

Iterator and range type detectors.

* `namespace RS::Meta`
    * `template <typename Iterator> struct` **`IteratorValue`**
    * `template <typename Range> struct` **`RangeIterator`**
    * `template <typename Range> struct` **`RangeValue`**

These return the types associated with an iterator or range. They are
extracted by checking the return type of `begin()`, and do not require a
specialization of `iterator_traits` to exist.

## Mixin types ##

<!-- DEFN --> These are convenience base classes that define members and
operators that would normally just be repetitive boilerplate (similar to the
ones in Boost). They all use the CRTP idiom; a class `T` should derive from
`Mixin<T>` to automatically generate the desired boilerplate code. The table
below shows which operations the user is required to define, and which ones
the mixin will automatically define. (Here, `t` and `u` are objects of type
`T`, `v` is an object of `T`'s value type, and `n` is an integer.)

Mixin                                       | Requires                         | Defines
-----                                       | --------                         | -------
**`EqualityComparable`**`<T>`               | `t==u`                           | `t!=u`
**`LessThanComparable`**`<T>`               | `t==u, t<u`                      | `t!=u, t>u, t<=u, t>=u`
**`InputIterator`**`<T,CV>`                 | `*t, ++t, t==u`                  | `t->, t++, t!=u`
**`OutputIterator`**`<T>`                   | `t=v`                            | `*t, ++t, t++`
**`ForwardIterator`**`<T,CV>`               | `*t, ++t, t==u`                  | `t->, t++, t!=u`
**`BidirectionalIterator`**`<T,CV>`         | `*t, ++t, --t, t==u`             | `t->, t++, t--, t!=u`
**`RandomAccessIterator`**`<T,CV>`          | `*t, t+=n, t-u`                  | `t->, t[n], ++t, t++, --t, t--, t-=n, t+n, n+t, t-n,`<br>`t==u, t!=u, t<u, t>u, t<=u, t>=u`
**`FlexibleRandomAccessIterator`**`<T,CV>`  | `*t, ++t, --t, t+=n, t-u, t==u`  | `t->, t[n], t++, t--, t-=n, t+n, n+t, t-n,`<br>`t!=u, t<u, t>u, t<=u, t>=u`

In the iterator mixins, `CV` is either `V` or `const V`, where `V` is the
iterator's value type, depending on whether a mutable or const iterator is
required.

The first version of `RandomAccessIterator` uses the minimal set of user
supplied operations to generate all of those required;
`FlexibleRandomAccessIterator` requires more user supplied operations, but
will decay safely to one of the simpler iterator types if an underlying type
does not supply all of the corresponding operations.

In addition to the operators listed in the table above, all iterator mixins
supply the standard member types:

* `using` **`difference_type`** `= ptrdiff_t`
* `using` **`iterator_category`** `= [standard iterator tag type]`
* `using` **`pointer`** `= CV*`
* `using` **`reference`** `= CV&`
* `using` **`value_type`** `= std::remove_const_t<CV>`

## Range types ##

* `template <typename Iterator> struct` **`Irange`**
    * `using Irange::`**`iterator`** `= Iterator`
    * `using Irange::`**`value_type`** `= [Iterator's value type]`
    * `Iterator Irange::`**`first`**
    * `Iterator Irange::`**`second`**
    * `constexpr Iterator Irange::`**`begin`**`() const { return first; }`
    * `constexpr Iterator Irange::`**`end`**`() const { return second; }`
    * `constexpr bool Irange::`**`empty`**`() const { return first == second; }`
    * `constexpr size_t Irange::`**`size`**`() const { return std::distance(first, second); }`
* `template <typename Iterator> constexpr Irange<Iterator>` **`irange`**`(const Iterator& i, const Iterator& j)`
* `template <typename Iterator> constexpr Irange<Iterator>` **`irange`**`(const pair<Iterator, Iterator>& p)`

A wrapper for a pair of iterators, usable as a range in standard algorithms.

* `template <typename InputRange> size_t` **`range_count`**`(const InputRange& r)`
* `template <typename InputRange> bool` **`range_empty`**`(const InputRange& r)`

Return the length of a range. The `range_count()` function is just shorthand
for `std::distance(begin(r),end(r))`, and `range_empty()` has the obvious
meaning.

## Scope guards ##

* `template <typename T> std::unique_lock<T>` **`make_lock`**`(T& t)`
* `template <typename T> std::shared_lock<T>` **`make_shared_lock`**`(T& t)`

Simple wrapper functions to create a mutex lock.

* `enum class` **`Scope`**
    * `Scope::`**`exit`**
    * `Scope::`**`fail`**
    * `Scope::`**`success`**
* `template <typename F, Scope S> class` **`BasicScopeGuard`**
    * `BasicScopeGuard::`**`BasicScopeGuard`**`() noexcept`
    * `BasicScopeGuard::`**`BasicScopeGuard`**`(F&& f)`
    * `BasicScopeGuard::`**`BasicScopeGuard`**`(BasicScopeGuard&& sg) noexcept`
    * `BasicScopeGuard::`**`~BasicScopeGuard`**`() noexcept`
    * `BasicScopeGuard& BasicScopeGuard::`**`operator=`**`(F&& f)`
    * `BasicScopeGuard& BasicScopeGuard::`**`operator=`**`(BasicScopeGuard&& sg) noexcept`
    * `void BasicScopeGuard::`**`release`**`() noexcept`
* `using` **`ScopeExit`** `= BasicScopeGuard<std::function<void()>, Scope::exit>`
* `using` **`ScopeFail`** `= BasicScopeGuard<std::function<void()>, Scope::fail>`
* `using` **`ScopeSuccess`** `= BasicScopeGuard<std::function<void()>, Scope::success>`
* `template <typename F> BasicScopeGuard<F, Scope::exit>` **`scope_exit`**`(F&& f)`
* `template <typename F> BasicScopeGuard<F, Scope::fail>` **`scope_fail`**`(F&& f)`
* `template <typename F> BasicScopeGuard<F, Scope::success>` **`scope_success`**`(F&& f)`

The scope guard class stores a function object, to be called when the guard is
destroyed. The three functions create scope guards with different execution
conditions. The three named specializations are conveniences for when a scope
guard needs to be stored as a movable object.

A **scope exit** guard calls the function unconditionally; **scope success**
calls it only on normal exit, but not when unwinding due to an exception;
**scope fail** calls it only when an exception causes stack unwinding, but not
on normal exit. If the constructor or creation function throws an exception
(this is only possible if the function object's move constructor or assignment
operator throws), **scope exit** and **scope fail** will call the function
before propagating the exception, while **scope success** will not. Any
exceptions thrown by the function call in the scope guard's destructor are
silently ignored (normally the function should be written so as not to throw
anything).

The `release()` function discards the saved function; after it is called, the
scope guard object will do nothing on destruction.

## String functions ##

* `constexpr bool` **`ascii_isalnum`**`(char c) noexcept`
* `constexpr bool` **`ascii_isalpha`**`(char c) noexcept`
* `constexpr bool` **`ascii_iscntrl`**`(char c) noexcept`
* `constexpr bool` **`ascii_isdigit`**`(char c) noexcept`
* `constexpr bool` **`ascii_isgraph`**`(char c) noexcept`
* `constexpr bool` **`ascii_islower`**`(char c) noexcept`
* `constexpr bool` **`ascii_isprint`**`(char c) noexcept`
* `constexpr bool` **`ascii_ispunct`**`(char c) noexcept`
* `constexpr bool` **`ascii_isspace`**`(char c) noexcept`
* `constexpr bool` **`ascii_isupper`**`(char c) noexcept`
* `constexpr bool` **`ascii_isxdigit`**`(char c) noexcept`
* `constexpr bool` **`is_ascii`**`(char c) noexcept`

These are simple ASCII-only versions of the standard character type functions.
All of them will always return false for bytes outside the ASCII range
(0-127).

* `constexpr bool` **`ascii_isalnum_w`**`(char c) noexcept`
* `constexpr bool` **`ascii_isalpha_w`**`(char c) noexcept`
* `constexpr bool` **`ascii_ispunct_w`**`(char c) noexcept`

These behave the same as the corresponding functions without the `"_w"`
suffix, except that the underscore character is counted as a letter instead of
a punctuation mark. (The suffix is intended to suggest the `"\w"` regex
element, which does much the same thing.)

* `constexpr char` **`ascii_tolower`**`(char c) noexcept`
* `constexpr char` **`ascii_toupper`**`(char c) noexcept`
* `std::string` **`ascii_lowercase`**`(std::string_view s)`
* `std::string` **`ascii_uppercase`**`(std::string_view s)`
* `std::string` **`ascii_titlecase`**`(std::string_view s)`
* `std::string` **`ascii_sentencecase`**`(std::string_view s)`

Simple ASCII-only case conversion functions. All non-ASCII characters are left
unchanged. The sentence case function capitalizes the first letter of every
sentence (delimited by a full stop or two consecutive line breaks), leaving
everything else alone.

* `template <typename T> Ustring` **`bin`**`(T x, size_t digits = 8 * sizeof(T))`
* `template <typename T> Ustring` **`dec`**`(T x, size_t digits = 1)`
* `template <typename T> Ustring` **`hex`**`(T x, size_t digits = 2 * sizeof(T))`

Simple number formatting functions. These convert an integer to a binary,
decimal, or hexadecimal string, generating at least the specified number of
digits.

* `unsigned long long` **`binnum`**`(std::string_view str) noexcept`
* `long long` **`decnum`**`(std::string_view str) noexcept`
* `unsigned long long` **`hexnum`**`(std::string_view str) noexcept`
* `double` **`fpnum`**`(std::string_view str) noexcept`

The `binnum()`, `decnum()`, and `hexnum()` functions convert a binary,
decimal, or hexadecimal string to a number; `fpnum()` converts a string to a
floating point number. These will ignore any trailing characters that are not
part of a number, and will return zero if the string is empty or does not
contain a valid number. Results that are out of range will be clamped to the
nearest end of the return type's range, or for `fpnum()`, to positive or
negative infinity.

* `template <typename C> basic_string<C>` **`cstr`**`(const C* ptr)`
* `template <typename C> basic_string<C>` **`cstr`**`(const C* ptr, size_t n)`

These construct a string from a pointer to a null-terminated character
sequence, or a pointer and a length. They differ from the corresponding string
constructors in that passing a null pointer will yield an empty string, or a
string of `n` null characters, instead of undefined behaviour.

* `template <typename C> size_t` **`cstr_size`**`(const C* ptr)`

Returns the length of a null-terminated string (a generalized version of
`strlen()`). This will return zero if the pointer is null.

* `template <typename Range> Ustring` **`format_list`**`(const Range& r)`
* `template <typename Range> Ustring` **`format_list`**`(const Range& r, std::string_view prefix, std::string_view delimiter, std::string_view suffix)`
* `template <typename Range> Ustring` **`format_map`**`(const Range& r)`
* `template <typename Range> Ustring` **`format_map`**`(const Range& r, std::string_view prefix, std::string_view infix, std::string_view delimiter, std::string_view suffix)`

Format a range as a delimited list. The `format_list()` function writes the
elements in sequence, with `prefix` and `suffix` at the beginning and end, and
with a `delimiter` between each pair of elements; individual elements are
formatted using `to_str()` (see below). The `format_map()` function expects
the range's value type to be a pair (or something with `first` and `second`
members); the elements of each pair are separated with the `infix` string, and
the range is otherwise formatted in the same way as `format_list()`. The
default formats are based on JSON syntax:

<!-- TEXT -->
* `format_list(r) = format_list(r, "[", ",", "]")`
* `format_map(r) = format_map(r, "{", ":", ",", "}")`

* `template <typename T> Ustring` **`fp_format`**`(T t, char mode = 'g', int prec = 6)`
* `template <typename T> Ustring` **`opt_fp_format`**`(T t, char mode = 'g', int prec = 6)`

The `fp_format()` function performs simple floating point formatting, by
calling `snprintf()`. `T` must be an arithmetic type; it will be converted to
`long double` internally. The additional format `'Z/z'` is the same as `'G/g'`
except that trailing zeros are not stripped. The `opt_fp_format()` function
calls `fp_format()` for floating point types; otherwise it calls `to_str(t)`
and ignores the other two arguments. These will throw `std::invalid_argument`
if the mode is not one of `[EFGZefgz]` (and is not ignored); they may throw
`std::system_error` under implementation defined circumstances.

* `template <typename S> [string view]` **`make_view`**`(const S& s, size_t pos = 0, size_t len = npos) noexcept`

Returns a string view over the given string. The string argument may be an
instantiation of `std:basic_string` or `std::basic_string_view`, or a pointer
to a null terminated character array. The substring bounds are range checked
and clamped to the actual size of the string.

* `std::string` **`quote`**`(std::string_view str)`
* `Ustring` **`bquote`**`(std::string_view str)`

Return a quoted string; internal quotes, backslashes, and control characters
are escaped. The `quote()` function passes non-ASCII bytes through unchanged,
while `bquote()` escapes them.

* `Ustring` **`roman`**`(int n)`

Formats a number as a Roman numeral. Numbers greater than 1000 will be written
with an arbitrarily long sequence of `"M"`. This will return an empty string
if the argument is less than 1.

* `int64_t` **`si_to_int`**`(Uview str)`
* `double` **`si_to_float`**`(Uview str)`

These parse a number from a string representation tagged with an SI multiplier
abbreviation (e.g. `"123k"`). For the integer version, only tags representing
positive powers of 1000 (starting with`"k"`) are recognised, and are case
insensitive. For the floating point version, all tags representing powers of
100 are recognised (`"u"` is used for "micro"), and are case sensitive, except
that `"K"` is equivalent to `"k"`. For both versions, a space is allowed
between the number and the tag, and any additional text after the number or
tag is ignored. These will throw `std::invalid_argument` if the string does
not start with a valid number, or `std::range_error` if the result is too big
for the return type.

* `template <typename T> std::string` **`to_str`**`(const T& t)`

Formats an object as a string. This uses the following rules for formatting
various types:

* `bool` - Written as `"true"` or `"false"`.
* Integer types (other than `char`) - Formatted using `dec()`.
* Floating point types - Formatted using `fp_format()`.
* Strings and string-like types - The string content is simply copied verbatim; a null character pointer is treated as an empty string.
* Exceptions derived from `std::exception` - Calls the exception's `what()` method.
* Arrays and vectors of bytes (`unsigned char`) - Formatted in hexadecimal.
* Ranges (other than strings and byte arrays) - Serialized in the same format as `format_list()` above, or `format_map()` if the value type is a pair.
* Pairs and tuples - Formatted as a comma delimited list, enclosed in parentheses.
* Otherwise - Call the type's output operator, or fail to compile if it does not have one.

"String-like types" are defined as `std::string`, `std::string_view`, plain
`char`, character pointers, and anything with an implicit conversion to
`std::string` or `std::string_view`.

## Version number ##

* `class` **`Version`**
    * `using Version::`**`value_type`** `= unsigned`
    * `Version::`**`Version`**`() noexcept`
    * `template <typename... Args> Version::`**`Version`**`(unsigned n, Args... args)`
    * `explicit Version::`**`Version`**`(const Ustring& s)`
    * `Version::`**`Version`**`(const Version& v)`
    * `Version::`**`Version`**`(Version&& v) noexcept`
    * `Version::`**`~Version`**`() noexcept`
    * `Version& Version::`**`operator=`**`(const Version& v)`
    * `Version& Version::`**`operator=`**`(Version&& v) noexcept`
    * `unsigned Version::`**`operator[]`**`(size_t i) const noexcept`
    * `const unsigned* Version::`**`begin`**`() const noexcept`
    * `const unsigned* Version::`**`end`**`() const noexcept`
    * `unsigned Version::`**`major`**`() const noexcept`
    * `unsigned Version::`**`minor`**`() const noexcept`
    * `unsigned Version::`**`patch`**`() const noexcept`
    * `size_t Version::`**`size`**`() const noexcept`
    * `Ustring Version::`**`str`**`(size_t min_elements = 2, char delimiter = '.') const`
    * `Ustring Version::`**`suffix`**`() const`
    * `uint32_t Version::`**`to32`**`() const noexcept`
    * `static Version Version::`**`from32`**`(uint32_t n) noexcept`
* `bool` **`operator==`**`(const Version& lhs, const Version& rhs) noexcept`
* `bool` **`operator!=`**`(const Version& lhs, const Version& rhs) noexcept`
* `bool` **`operator<`**`(const Version& lhs, const Version& rhs) noexcept;`
* `bool` **`operator>`**`(const Version& lhs, const Version& rhs) noexcept`
* `bool` **`operator<=`**`(const Version& lhs, const Version& rhs) noexcept`
* `bool` **`operator>=`**`(const Version& lhs, const Version& rhs) noexcept`
* `std::ostream&` **`operator<<`**`(std::ostream& o, const Version& v)`

A version number, represented as an array of integers optionally followed by a
trailing string (e.g. `1.2.3beta`). The default constructor sets the version
number to zero; the second constructor expects one or more integers followed
by a string; the third constructor parses a string. A string that does not
start with an integer is assumed to start with an implicit zero; if the string
contains any whitespace, anything after that is ignored. Results are
unspecified if a version number element is too big to fit in an `unsigned
int`.

The indexing operator returns the requested element; it will return zero if
the index is out of range for the stored array. The `major()`, `minor()`, and
`patch()` functions return elements 0, 1, and 2. The `suffix()` function
returns the trailing string element.

The `size()` function returns the number of numeric elements in the version
number. This will always return at least 1.

The `str()` function (and the output operator) formats the version number in
the conventional form; a minimum number of elements can be requested. The
`to32()` and `from32()` functions pack or unpack the version into a 32 bit
integer, with one byte per element (e.g. version `1.2.3` becomes
`0x01020300`); `to32()` truncates elements higher than 255 and ignores the
string element.

Following the common convention that version suffixes are used to designate
pre-release builds, versions with a suffix are ordered before the same version
without the suffix.
