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
    tab << "Name" << "Number" << "Hex" << "Float" << Table::endl;
    tab.div();
    tab.format("", "", "0x$1x4", "$1fs4");
    tab << "Patrick McGoohan" << 6 << 6 << 6.0 << Table::endl;
    tab << "James Bond" << 007 << 007 << 007.0 << Table::endl;
    tab << "Douglas Adams" << 42 << 42 << 42.0 << Table::endl;
    tab << "Maxwell Smart" << 86 << 86 << 86.0 << Table::endl;
    std::cout << tab;

Output:

    Name              Number  Hex     Float
    ----------------  ------  ------  --------
    Patrick McGoohan  6       0x0006  +6.0000
    James Bond        7       0x0007  +7.0000
    Douglas Adams     42      0x002a  +42.0000
    Maxwell Smart     86      0x0056  +86.0000

## Table class ##

* `static constexpr [private type] Table::endl`

This is passed to a table through `operator<<` to mark the end of a row.

* `Table::Table()`
* `Table::Table(const Table& t)`
* `Table::Table(Table&& t) noexcept`
* `Table::~Table() noexcept`
* `Table& Table::operator=(const Table& t)`
* `Table& Table::operator=(Table&& t) noexcept`

Life cycle functions.

* `void Table::clear_all() noexcept`
* `void Table::clear_data() noexcept`
* `void Table::clear_formats() noexcept`

Clear the cell data, formatting settings, or both.

* `template <typename... FS> void Table::format(const u8string& f, const FS&... fs)`

This sets up formatting for the table's columns, using the formatting codes
defined in `[unicorn/format](format.html)`. Each formatting element should be
of the form `"${1...}"`, with flags appropriate to the type of data expected
in that column. Columns for which no format has been set, or whose format is
an empty string, will be written using `to_str()` (from Prion, but called
without namespace qualification, so user defined overloads will be found).

* `template <typename T> Table& Table::operator<<(const T& t)`

Adds a data cell to the table. The cell will be formatted according to the
current column's formatting code, if one has been set.

* `void Table::div(char c = '-')`

Writes a horizontal rule across the table.

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
`Table::spacing`  | `size_t`       | `4`               | Number of spaces between columns
`Table::unfill`   | `bool`         | `false`           | Show repeated cells with a ditto mark
`Table::ditto`    | `u8string`     | `"''"`            | Symbol used for repeated cells in unfill mode
`Table::empty`    | `u8string`     | `"--"`            | Symbol used for empty cells
