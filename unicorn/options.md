Title: Unicorn Library: Command Line Options
CSS: style.css

# [Unicorn Library](index.html): Command Line Options #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/options.hpp"` ####

The `Options` class defined in this module handles parsing of command line
options. It provides commonly used features such as coalescing of single
character options (e.g. `-abc` vs `-a -b -c`), long option names (e.g.
`--long-option`), and so on.

The member functions that set up the option specifications take UTF-8 strings
(mostly because making keyword arguments work with variable string types would
have complicated the interface too much). The option parsing functions,
however, will accept an argument list in any Unicode encoding.

<p class="alert">[TODO: DOCUMENTATION]</p>

## Contents ##

* [Exceptions][]
* [Class Options][]

## Exceptions ##

* `class CommandLineError: public std::runtime_error`
* `class OptionSpecError: public std::runtime_error`

TODO

## Class Options ##

Keyword         | Type        | Description
-------         | ----        | -----------
`opt_anon`      | `bool`      | Assign anonymous arguments to this option
`opt_boolean`   | `bool`      | Boolean option
`opt_float`     | `bool`      | Argument must be a floating point number
`opt_integer`   | `bool`      | Argument must be an integer
`opt_multiple`  | `bool`      | Option may have multiple arguments
`opt_required`  | `bool`      | Option is required
`opt_abbrev`    | `u8string`  | Single letter abbreviation
`opt_default`   | `u8string`  | Default value if not supplied
`opt_group`     | `u8string`  | Mutual exclusion group name
`opt_pattern`   | `u8string`  | Argument must match this regular expression

TODO

Bitmask         | Letter  | Description
-------         | ------  | -----------
`opt_locale`    | `l`     | Argument list is in local encoding (ignored if C[1] is not char)
`opt_noprefix`  | `n`     | First argument is not the command name
`opt_quoted`    | `q`     | Allow arguments to be quoted

TODO

* `explicit Options::Options(const u8string& info, const u8string& head = {}, const u8string& tail = {})`

TODO

* `Options::Options(const Options& opt)`
* `Options::Options(Options&& opt) noexcept`
* `Options::~Options() noexcept`
* `Options& Options::operator=(const Options& opt)`
* `Options& Options::operator=(Options&& opt) noexcept`

Other life cycle functions.

* `template <typename... Args> void Options::add(const u8string& name, const u8string& info, const Args&... args)`

TODO

* `void Options::autohelp() noexcept`

TODO

* `u8string Options::help() const`
* `u8string Options::version() const`

TODO

* `template <typename C1, typename C2> bool Options::parse(const std::vector<basic_string<C1>>& args, std::basic_ostream<C2>& out, Crow::Flagset flags = {})`
* `template <typename C1, typename C2> bool Options::parse(const basic_string<C1>& args, std::basic_ostream<C2>& out, Crow::Flagset flags = {})`
* `template <typename C1, typename C2> bool Options::parse(int argc, C1** argv, std::basic_ostream<C2>& out, Crow::Flagset flags = {})`
* `template <typename C> bool Options::parse(const std::vector<basic_string<C>>& args)`
* `template <typename C> bool Options::parse(const basic_string<C>& args)`
* `template <typename C> bool Options::parse(int argc, C** argv)`

TODO

* `bool Options::has(const u8string& name) const`

TODO

* `template <typename T> T Options::get(const u8string& name) const`

TODO

* `template <typename T> std::vector<T> Options::get_list(const u8string& name) const`

TODO
