# [Unicorn Library](index.html): Command Line Options #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/options.hpp"`

The `Options` class defined in this module handles parsing of command line
options, and automatic generation of help messages for the user. It provides
some commonly expected command line features:

* Long option names prefixed with two hyphens, e.g. `"--long-option"`.
* Single letter abbreviations, e.g. `"-x"`.
* Combinations of abbreviated options, e.g. `"-abc" = "-a -b -c"`.
* Options can be followed by arguments, delimited by a space or an equals sign, e.g. `"--name value"` or `"--name=value"`.
* Options can have default arguments.
* Options may take multiple arguments, e.g. `"--option arg1 arg2 arg3"`.
* A option's arguments can be checked against a regular expression.
* Unattached arguments can be implicitly assigned to options.
* Sets of mutually exclusive options can be specified.
* An argument consisting only of two hyphens (`"--"`) marks the end of explicitly named options;
    any text following it is read as unattached arguments, even if it looks like an option.

Example:

    int main(int argc, char** argv) {
        Options opt("My Program 1.0");
        opt.add("--alpha", "The most important option", opt_abbrev="-a");
        opt.add("--omega", "The least important option");
        opt.add("--number", "How many roads to walk down", opt_abbrev="-n", opt_default="42", opt_int);
        if (opt.parse(argc, argv))
            return 0;
        // ... main program code goes here ...
    }

If this program is invoked with the `--help` option (or `-h`), it will display
the following information on the standard output:

    My Program 1.0

    Options:
        --alpha, -a <arg>   = The most important option
        --omega <arg>       = The least important option
        --number, -n <num>  = How many roads to walk down (default 42)
        --help, -h          = Show usage information
        --version, -v       = Show version information

## Options class ##

* `class Options::`**`CommandError`**`: public std::runtime_error`
    * `explicit CommandError::`**`CommandError`**`(const u8string& details, const u8string& arg = {}, const u8string& arg2 = {})`

Thrown by `Options::parse()` during argument parsing, to report that the
command line arguments supplied by the user were not consistent with the
option specification.

* `class Options::`**`SpecError`**`: public std::runtime_error`
    * `explicit SpecError::`**`SpecError`**`(const u8string& option)`
    * `SpecError::`**`SpecError`**`(const u8string& details, const u8string& option)`

Thrown by `Options::add()` during the creation of an option specification, to
report an invalid combination of properties.

* `explicit Options::`**`Options`**`(const u8string& info)`

Constructor to initialize an option specification. The `info` argument is a
string containing the basic description of the program, typically something
like `"Foobar 1.0 - Goes ding when there's stuff"`; this will be returned if
the user calls it with the `"--version"` option.

* `Options::`**`Options`**`() noexcept`
* `Options::`**`Options`**`(const Options& opt)`
* `Options::`**`Options`**`(Options&& opt) noexcept`
* `Options::`**`~Options`**`() noexcept`
* `Options& Options::`**`operator=`**`(const Options& opt)`
* `Options& Options::`**`operator=`**`(Options&& opt) noexcept`

Other life cycle functions. (The default constructor should not normally be
used and is supplied only to enable move initialization in certain scenarios.)

* `Options& Options::`**`add`**`(const u8string& info)`
* `template <typename... Args> Options& Options::`**`add`**`(const u8string& name, const u8string& info, const Args&... args)`

The first version adds some information text to the option list. This will be
reproduced verbatim at the corresponding point among the options listed in the
help text.

The second version adds an option to the parser. The `name` argument is the
full name of the option, which users can invoke with `"--name"` (the option
name can be supplied to the `add()` function with or without the leading
hyphens). The `info` string is the description of the option that will be
presented to the user when help is requested. These may be followed by
optional keyword arguments, as listed below.

Keyword            | Type        | Description
-------            | ----        | -----------
**`opt_abbrev`**   | `u8string`  | A single letter abbreviation for the option (e.g. `"-x"`; the hyphen is optional).
**`opt_anon`**     | `bool`      | Anonymous arguments (not claimed by any other option) will be assigned to this option.
**`opt_bool`**     | `bool`      | This option is a boolean switch and does not take arguments.
**`opt_default`**  | `u8string`  | Use this default value if the option is not supplied by the user.
**`opt_float`**    | `bool`      | The argument value must be a floating point number.
**`opt_group`**    | `u8string`  | Assign the option to a mutual exclusion group; at most one option from a group is allowed.
**`opt_int`**      | `bool`      | The argument value must be an integer.
**`opt_multi`**    | `bool`      | This option may be followed by multiple arguments.
**`opt_pattern`**  | `u8string`  | The argument value must match this regular expression.
**`opt_require`**  | `bool`      | This option is mandatory.
**`opt_uint`**     | `bool`      | The argument value must be an unsigned integer.

Boolean options can be supplied in negated form, by giving a name starting
with `"--no-"` (or `"no-"`). This creates a boolean option whose default value
is `true`; the `"--no-whatever"` form can be used to switch it off.

Adding an option will throw `SpecError` if any of the following is true:

* The option name has less than two characters (not counting any leading hyphens).
* The name or abbreviation has already been used by an earlier entry.
* The info string is empty (this also applies to the first version of `add()`).
* An abbreviation is supplied that is longer than one character (not counting a leading hyphen), or is not alphanumeric.
* An option starting with `"--no-"` is not boolean or has an abbreviation.
* The `opt_bool` tag is combined with `opt_anon`, `opt_default`, `opt_multi`, `opt_pattern`, or `opt_require`.
* The `opt_require` tag is combined with `opt_bool`, `opt_default`, or `opt_group`.
* More than one of `opt_float`, `opt_int`, `opt_pattern`, and `opt_uint` is supplied.
* The `opt_default` and `opt_pattern` tags are both present, but the default value does not match the pattern.

Do not explicitly add the standard `"--help"` and `"--version"` boolean
options; these will be added automatically. They will be given the
abbreviations `"-h"` and `"-v"` if these have not been claimed by other
options.

A program will normally construct an `Options` object and use multiple calls
to `add()` to construct the option specification before calling `parse()` to
parse the actual command line arguments. Once the arguments have been parsed,
the `get()`, `get_list()`, and `has()` functions can be used to query them.

* `void Options::`**`autohelp`**`() noexcept`

If this is set, calling the program with no arguments will be interpreted as a
request for help (i.e. an empty argument list is equivalent to `"--help"`).

* `u8string Options::`**`help`**`() const`
* `u8string Options::`**`version`**`() const`

These are the same texts that will be presented to the user by the `"--help"`
and `"--version"` options. The `help()` text is constructed automatically by
the `Options` object; the `version()` text is simply the original `info`
string that was supplied to the `Options` constructor.

* `template <typename C> bool Options::`**`parse`**`(const vector<basic_string<C>>& args, std::ostream& out = cout, uint32_t flags = 0)`
* `template <typename C> bool Options::`**`parse`**`(const basic_string<C>& args, std::ostream& out = cout, uint32_t flags = 0)`
* `template <typename C> bool Options::`**`parse`**`(int argc, C** argv, std::ostream& out = cout, uint32_t flags = 0)`

After the option specification has been constructed, call one of the `parse()`
functions to parse the actual command line arguments. The arguments can be
supplied as a vector of strings, as a single combined string that will be
split apart during parsing, or as the standard `(argc,argv)` arguments from
`main()` (or a similar source such as the UTF-16 `_wmain()` often used on
Windows). Normally the supplied argument list is assumed to start with the
command name (which will be discarded); use the `opt_noprefix` flag to
override this.

Boolean options will be recognised in normal or negated form (e.g. `"--magic"`
vs `"--no-magic"`). Integer options will accept hexadecimal options prefixed
with `"0x"`; integer or float options will accept values tagged with SI prefix
abbreviations (e.g. `"50k"` or `"2.5M"`).

If help or version information is requested, it will be written to the given
output stream (standard output by default). The `parse()` function will return
true if all requested processing has already been handled (i.e. if help or
version information has been presented to the user); the caller should check
the return value from `parse()` and end the program if it is true.

The `flags` argument can be any combination of these:

Flag                | Description
----                | -----------
**`opt_locale`**    | The argument list is in the local encoding
**`opt_noprefix`**  | The first argument is not the command name
**`opt_quoted`**    | Allow arguments to be quoted

The `opt_locale` flag is only relevant to 8 bit strings, which are assumed to
be UTF-8 by default; the flag is ignored if the `C` type is not `char`, since
16 or 32 bit strings are always assumed to be UTF-16/32.

The `parse()` functions will throw `CommandError` if any of the following is
true:

* A full or abbreviated option is supplied that is not in the spec.
* The same option appears more than once, but does not have the `opt_multi` flag.
* Multiple options from the same mutual exclusion group are supplied.
* The argument supplied for an option does not match the pattern given in the spec.
* A required option is missing.
* There are unattached arguments left over after all options have been satisfied.

Behaviour is unspecified if `parse()` is called more than once on the same
`Options` object.

* `template <typename T> T Options::`**`get`**`(const u8string& name) const`
* `template <typename T> vector<T> Options::`**`get_list`**`(const u8string& name) const`
* `bool Options::`**`has`**`(const u8string& name) const`

These return information about the options and arguments found in the command
line. The option name can be supplied with or without leading hyphens. Only
the full name is recognized here, not an abbreviation.

The `get()` function returns the argument attached to an option, converted to
the given type. If the return type is an arithmetic type, the argument string
will be converted; it may have a leading `"0x"` for hexadecimal, or a trailing
SI multiplier (e.g. `"42k"` is interpreted as 42000); any other trailing
characters that are not part of a number are ignored. If multiple arguments
were supplied for the option, they are concatenated into a space delimited
list first. If the option was not present on the command line, its default
value is used.

The `get_list()` function returns multiple arguments as a vector. Its
behaviour is otherwise the same as `get()`.

The `has()` function simply indicates whether an option was present on the
command line.

All of these will throw `SpecError` if the `name` string does not match one of
the registered options.
