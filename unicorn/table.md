Title: Unicorn Library: Table Layout
CSS: style.css

# [Unicorn Library](index.html): Table Layout #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/table.hpp"` ####

`Unicorn::Table` is a utility class for simple table layout in fixed width
text, a task often useful in formatting the output from command line programs
and similar utilities. Tables are compiled in UTF-8 but can be written to
strings or output streams of any UTF encoding.

Example:

    Table tab;
    tab << "Name" << "Number" << "Hex" << "Float" << '\n'
        << '=';
    tab.format("", "", "0x$1x3", "$1fs2");
    tab << "Patrick McGoohan" << 6 << 6 << 6 << '\n'
        << "James Bond" << 007 << 007 << 007 << '\n'
        << "Douglas Adams" << 42 << 42 << 42 << '\n'
        << "Maxwell Smart" << 86 << 86 << 86 << '\n';
    std::cout << tab;

Output:

    Name              Number  Hex    Float
    ================  ======  =====  ======
    Patrick McGoohan  6       0x006  +6.00
    James Bond        7       0x007  +7.00
    Douglas Adams     42      0x02a  +42.00
    Maxwell Smart     86      0x056  +86.00

## Table class ##

* `Table::Table()`
* `Table::Table(const Table& t)`
* `Table::Table(Table&& t) noexcept`
* `Table::~Table() noexcept`
* `Table& Table::operator=(const Table& t)`
* `Table& Table::operator=(Table&& t) noexcept`

Life cycle functions.

* `void Table::clear() noexcept`

Clears all cell data and formatting settings from the table (i.e. resets the
table to its default constructed state).

* `template <typename... FS> void Table::format(const u8string& f, const FS&... fs)`

This sets up formatting for the table's columns, using the formatting codes
defined in `[unicorn/format](format.html)`. Each formatting element should be
of the form `"${1...}"`, with flags appropriate to the type of data expected
in that column. Columns for which no format has been set, or whose format is
an empty string, will be written using `to_str()` (from Prion, but called
without namespace qualification, so user defined overloads will be found).
Formatting is applied to cell data entered after a call to `format()`;
existing cells already in the table will not be reformatted.

* `template <typename T> Table& Table::operator<<(const T& t)`

Adds a data cell to the table. The cell will be formatted according to the
current column's formatting code, if one has been set.

* `Table& Table::operator<<([character type] c)`

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

If the character is a control character (not listed above), whitespace, or an
unassigned code point, the insertion operator will throw
`std::invalid_argument`. Otherwise, the character is used to write a
horizontal rule across the table, by repeating the character to match the
width of each column.

* `template <typename C, typename... Args> std::basic_string<C> Table::as_string(const Args&... args) const`
* `template <typename... Args> u8string Table::str(const Args&... args) const`
* `template <typename C, typename... Args> void Table::write(std::basic_ostream<C>& out, const Args&... args) const`

These format the table, to a string or an output stream, in the chosen UTF
encoding. The `str()` function is shorthand for `as_string<char>()`.

By default, string lengths are measured in grapheme units (user perceived
characters) in calculating fixed width table layout. Other measurement flags
(from `[unicorn/string](string.html)`) can be passed if required.

If the `unfill` flag is passed, cells that are identical to the one
immediately above them will be written using a ditto mark (two apostrophes by
default) instead of repeating their contents.

The following keyword arguments are recognised:

Keyword           | Argument type  | Default           | Description
-------           | -------------  | -------           | -----------
`Table::flags`    | `Flagset`      | `grapheme_units`  | How string lengths are measured
`Table::margin`   | `size_t`       | `0`               | Number of spaces in the left margin
`Table::spacing`  | `size_t`       | `2`               | Number of spaces between columns
`Table::unfill`   | `bool`         | `false`           | Show repeated cells with a ditto mark
`Table::ditto`    | `u8string`     | `"''"`            | Symbol used for repeated cells in unfill mode
`Table::empty`    | `u8string`     | `"--"`            | Symbol used for empty cells
