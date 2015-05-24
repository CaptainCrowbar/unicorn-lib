Title: Unicorn Library: Command Line Options
CSS: style.css

# [Unicorn Library](index.html): Command Line Options #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/options.hpp"` ####

The `Options` class defined in this module handles parsing of command line
options. It provides commonly used features such as coalescing of single
character options (e.g. `-abc` vs `-a -b -c`), long option names (e.g.
`--long-option`), and so on.

This module calls the [`unicorn/regex`](regex.html) module, which in turn
calls the PCRE library. It will only work with encodings for which the
corresponding PCRE library has been linked; see the regex module documentation
for details.

## Contents ##

* [Exceptions][]
* [Options parsing class][]

## Exceptions ##

* `class CommandLineError: public std::runtime_error`
    * `explicit CommandLineError::CommandLineError(const u8string& details, const u8string& arg1 = {}, const u8string& arg2 = {})`
* `class OptionSpecError: public std::runtime_error`
    * `explicit OptionSpecError::OptionSpecError(const u8string& details, const u8string& option = {})`

TODO

* `class HelpRequest: public std::exception`
    * `explicit HelpRequest::HelpRequest(const u8string& details)`

TODO

## Options parsing class ##

* `class Options`

TODO

* `template <typename C> explicit Options::Options(const basic_string<C>& info, const basic_string<C>& head = {}, const basic_string<C>& tail = {})`
* `template <typename C> explicit Options::Options(const C* info, const C* head = nullptr, const C* tail = nullptr)`
* `Options::Options(const Options& opt)`
* `Options::Options(Options&& opt)`
* `Options::~Options()`
* `Options& Options::operator=(const Options& opt)`
* `Options& Options::operator=(Options&& opt)`

TODO

* `template <typename C> void Options::add(const basic_string<C>& name, C abbrev, const basic_string<C>& info, Crow::Flagset flags = {}, const basic_string<C>& defval = {}, const basic_string<C>& pattern = {}, const basic_string<C>& group = {})`
* `template <typename C> void Options::add(const C* name, C abbrev, const C* info, Crow::Flagset flags = {}, const C* defval = {}, const C* pattern = {}, const C* group = {})`

TODO

Bitmask         | Letter  | Description
-------         | ------  | -----------
`opt_anon`      | `a`     | Assign anonymous arguments to this option
`opt_boolean`   | `b`     | Boolean option
`opt_float`     | `f`     | Argument must be a floating point number
`opt_integer`   | `i`     | Argument must be an integer
`opt_multiple`  | `m`     | Option may have multiple arguments
`opt_required`  | `r`     | Option is required

TODO

* `void Options::autohelp() noexcept`

TODO

* `u8string Options::help() const`
* `u8string Options::version() const`

TODO

* `template <typename C> void Options::parse(const std::vector<basic_string<C>>& args, Crow::Flagset flags = {})`
* `template <typename C> void Options::parse(const basic_string<C>& args, Crow::Flagset flags = {})`
* `template <typename C> void Options::parse(int argc, C** argv, Crow::Flagset flags = {})`

TODO

Bitmask         | Letter  | Description
-------         | ------  | -----------
`opt_locale`    | `l`     | Argument list is in local encoding
`opt_noprefix`  | `n`     | First argument is not the command name
`opt_quoted`    | `q`     | Allow arguments to be quoted

TODO

* `template <typename C> bool Options::has(const basic_string<C>& name) const`
* `template <typename C> bool Options::has(const C* name) const`

TODO

* `template <typename T, typename C> T Options::get(const basic_string<C>& name) const`
* `template <typename T, typename C> T Options::get(const C* name) const`

TODO

* `template <typename T, typename C> std::vector<T> Options::getlist(const basic_string<C>& name) const`
* `template <typename T, typename C> std::vector<T> Options::getlist(const C* name) const`

TODO
