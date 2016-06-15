# [Unicorn Library](index.html): Table Layout #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/text-table.hpp"`

`Unicorn::`**`TextTable`** is a utility class for simple table layout in fixed
width text, a task often useful in formatting the output from command line
programs and similar utilities.

Example:

    TextTable tab;
    tab << "Name" << "Number" << "Hex" << "Float" << '\n'
        << '=';
    tab.format("", "", "0x$1x3", "$1fs2");
    tab << "Patrick McGoohan" << 6 << 6 << 6 << '\n'
        << "James Bond" << 007 << 007 << 007 << '\n'
        << "Douglas Adams" << 42 << 42 << 42 << '\n'
        << "Maxwell Smart" << 86 << 86 << 86 << '\n';
    cout << tab;

Output:

    Name              Number  Hex    Float
    ================  ======  =====  ======
    Patrick McGoohan  6       0x006  +6.00
    James Bond        7       0x007  +7.00
    Douglas Adams     42      0x02a  +42.00
    Maxwell Smart     86      0x056  +86.00

## TextTable class ##

* `TextTable::`**`TextTable`**`()`
* `TextTable::`**`TextTable`**`(const TextTable& t)`
* `TextTable::`**`TextTable`**`(TextTable&& t) noexcept`
* `TextTable::`**`~TextTable`**`() noexcept`
* `TextTable& TextTable::`**`operator=`**`(const TextTable& t)`
* `TextTable& TextTable::`**`operator=`**`(TextTable&& t) noexcept`

Life cycle functions.

* `void TextTable::`**`clear`**`() noexcept`

Clears all cell data and formatting settings from the table (i.e. resets the
table to its default constructed state).

* `template <typename... FS> void TextTable::`**`format`**`(const u8string& f, const FS&... fs)`

This sets up formatting for the table's columns, using the formatting codes
defined in [`unicorn/format`](format.html). Each formatting element should be
of the form `"${1...}"`, with flags appropriate to the type of data expected
in that column. Columns for which no format has been set, or whose format is
an empty string, will be written using `to_str()` (from Prion, but called
without namespace qualification, so user defined overloads will be found).
Formatting is applied to cell data entered after a call to `format()`;
existing cells already in the table will not be reformatted.

* `template <typename T> TextTable& TextTable::`**`operator<<`**`(const T& t)`

Adds a data cell to the table. The cell will be formatted according to the
current column's formatting code, if one has been set.

* `TextTable& TextTable::`**`operator<<`**`(char c)`

Single character insertion is used to request various miscellaneous control operations, as
described in the table below.

Character  | Effect
---------  | ------
`\f`       | Clear all formatting codes
`\n`       | Start a new row
`\t`       | Copy the cell to the left
`\v`       | Copy the cell above

The `"\t"` and `"\v"` controls simply copy the existing cell's string content
literally; they will not reformat the original data value if the new cell has
a different formatting code. They will produce a blank cell if this is the
first column or row.

If the character is an ASCII punctuation mark, the character is used to write
a horizontal rule across the table, by repeating the character to match the
width of each column. Otherwise, the insertion operator will throw
`std::invalid_argument`.

* `template <typename... Args> u8string TextTable::`**`str`**`(const Args&... args) const`
* `template <typename... Args> void TextTable::`**`write`**`(std::ostream& out, const Args&... args) const`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const TextTable& tab)`

These format the table to a string or an output stream.

By default, string lengths are measured in grapheme units (user perceived
characters) in calculating fixed width table layout. Other measurement flags
(from [`unicorn/string`](string.html)) can be passed if required.

If the `tab_unfill` flag is passed, cells that are identical to the one
immediately above them will be written using a ditto mark (two apostrophes by
default) instead of repeating their contents.

The following keyword arguments are recognised:

Keyword            | Argument type  | Default           | Description
-------            | -------------  | -------           | -----------
**`tab_ditto`**    | `u8string`     | `"''"`            | Symbol used for repeated cells in unfill mode
**`tab_empty`**    | `u8string`     | `"--"`            | Symbol used for empty cells
**`tab_flags`**    | `uint32_t`     | `grapheme_units`  | How string lengths are measured
**`tab_margin`**   | `size_t`       | `0`               | Number of spaces in the left margin
**`tab_spacing`**  | `size_t`       | `2`               | Number of spaces between columns
**`tab_unfill`**   | `bool`         | `false`           | Show repeated cells with a ditto mark
