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
        opt.add("--alpha", "The most important option", Options::abbrev="-a");
        opt.add("--omega", "The least important option");
        opt.add("--number", "How many roads to walk down", Options::abbrev="-n", Options::defvalue="42", Options::integer);
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

* `class Options::`**`command_error`**`: public std::runtime_error`
    * `explicit command_error::`**`command_error`**`(const Ustring& details, const Ustring& arg = {}, const Ustring& arg2 = {})`

Thrown by `Options::parse()` during argument parsing, to report that the
command line arguments supplied by the user were not consistent with the
option specification.

* `class Options::`**`spec_error`**`: public std::runtime_error`
    * `explicit spec_error::`**`spec_error`**`(const Ustring& option)`
    * `spec_error::`**`spec_error`**`(const Ustring& details, const Ustring& option)`

Thrown by `Options::add()` during the creation of an option specification, to
report an invalid combination of properties.

* `enum class Options::`**`help`**
    * `help::`**`automatic`**
    * `help::`**`std`**

Options passed to `add()` to control the placement of help request options.

* `explicit Options::`**`Options`**`(const Ustring& info)`

Constructor to initialize an option specification. The `info` argument is a
string containing the basic description of the program, typically something
like `"Foobar 1.0 - Goes ding when there's stuff"`; this will be returned if
the user runs it with the `"--version"` option.

* `Options::`**`Options`**`() noexcept`
* `Options::`**`Options`**`(const Options& opt)`
* `Options::`**`Options`**`(Options&& opt) noexcept`
* `Options::`**`~Options`**`() noexcept`
* `Options& Options::`**`operator=`**`(const Options& opt)`
* `Options& Options::`**`operator=`**`(Options&& opt) noexcept`

Other life cycle functions. (The default constructor should not normally be
used and is supplied only to enable move initialization in some cases.)

* `template <typename... Args> Options& Options::`**`add`**`(const Ustring& name, const Ustring& info, const Args&... args)`
* `Options& Options::`**`add`**`(const Ustring& info)`
* `Options& Options::`**`add`**`(help h)`

A program will normally construct an `Options` object and use multiple calls
to `add()` to construct the option specification before calling `parse()` to
parse the actual command line arguments. Once the arguments have been parsed,
the `get()`, `get_list()`, and `has()` functions can be used to query them.

The first version of `add()` adds an option to the parser. The `name` argument
is the full name of the option, which users can invoke with `"--name"` (the
option name can be supplied to the `add()` function with or without the
leading hyphens). The `info` string is the description of the option that will
be presented to the user when help is requested. These may be followed by
optional keyword arguments, as listed below.

Keyword                    | Type           | Description
-------                    | ----           | -----------
&nbsp;                     | &nbsp;         | **Option names**
`Options::`**`abbrev`**    | `Ustring`      | A single letter abbreviation for the option (e.g. `"-x"`; the hyphen is optional in `add()`).
&nbsp;                     | &nbsp;         | **Relationships between options**
`Options::`**`group`**     | `Ustring`      | Assign the option to a mutual exclusion group; multiple options from the same group are not allowed.
`Options::`**`implies`**   | `Ustring`      | This option's presence implies another option.
`Options::`**`prereq`**    | `Ustring`      | This option requires another option to already be selected.
&nbsp;                     | &nbsp;         | **Argument types**
`Options::`**`boolean`**   | `bool`         | This option is a boolean switch and does not take arguments.
`Options::`**`enumtype`**  | _(see below)_  | The argument value must be the unqualified name of an enumeration value.
`Options::`**`file`**      | `bool`         | The argument value is expected to be a file path (this is just for documentation and does no checking).
`Options::`**`floating`**  | `bool`         | The argument value must be a floating point number (an integer is accepted).
`Options::`**`integer`**   | `bool`         | The argument value must be an integer.
`Options::`**`pattern`**   | `Ustring`      | The argument value must match this regular expression.
`Options::`**`si`**        | `bool`         | Numeric arguments can use SI abbreviations.
`Options::`**`uinteger`**  | `bool`         | The argument value must be an unsigned integer.
&nbsp;                     | &nbsp;         | **Other tags**
`Options::`**`anon`**      | `bool`         | Anonymous arguments (not claimed by any other option) will be assigned to this option.
`Options::`**`defvalue`**  | `Ustring`      | Use this default value if the option is not supplied by the user.
`Options::`**`multi`**     | `bool`         | This option may be followed by multiple arguments.
`Options::`**`required`**  | `bool`         | This option is mandatory.

Boolean options can be supplied in negated form, by giving a name starting
with `"--no-"` (or `"no-"`). This creates a boolean option whose default value
is `true`; the `"--no-whatever"` form can be used to switch it off.

Adding an option will throw `spec_error` if any of the following is true:

* A function that assumes the option list is complete (`parse()` or `help_text()`) has already been called.
* The option name has less than two characters (not counting any leading hyphens).
* The name or abbreviation contains any whitespace characters.
* The name or abbreviation has already been used by an earlier entry, unless it had a different prerequisite.
* The info string is empty (this also applies to the second version of `add()`).
* The abbreviation is longer than one character (not counting a leading hyphen).
* An option starting with `"--no-"` is not boolean or has an abbreviation.
* The `boolean` tag is combined with `anon`, `defvalue`, `enumtype`, `multi`, `pattern`, or `require`.
* The `require` tag is combined with `boolean`, `defvalue`, `group`, or `prereq`.
* More than one of the argument type options (other than `si`) is supplied.
* The `si` tag is combined with `boolean`, `enumtype`, or `file`.
* The `defvalue` and `enumtype` tags are both present, but the default value is not one of the enumeration values.
* The `defvalue` and `pattern` tags are both present, but the default value does not match the pattern.
* The target of an `implies` or `prereq` tag does not exist or is the same as the current option.
* The target of an `implies` tag is not boolean or is true by default.
* There is more than one option with both `anon` and `multi` flags (only one of these can receive leftover arguments).

The value attached to the `Options::enumtype` keyword must be a value of an
enumeration type (plain or strong) that was defined using one of the
`RS_ENUM()` or `RS_ENUM_CLASS()` macros. The actual value supplied here is not
significant; it only serves to identify the enumeration type. Normally you
would supply a default value, e.g. `Options::enumtype=MyEnum()`. Underscores
in enumeration value names are converted to hyphens in the strings expected as
arguments.

The second version of `add()` adds some information text to the option list.
This will be reproduced verbatim at the corresponding point among the options
listed in the help text.

The third version of `add()` can be used to explicitly add the help and
version options. Use this if you want to place them somewhere other than at
the end of the list of options in the help message, or if you want to set the
automatic help flag. Either value will add the `"--help"` and `"--version"`
options, with the abbreviations `"-h"` and `"-v"` if these have not already
been assigned to other options. They will be automatically added at the end of
the option list if you do not add them this way. If the `automatic` flag is
used, calling the program with no arguments will be interpreted as a request
for help (i.e. an empty argument list is equivalent to `"--help"`).

* `Ustring Options::`**`help_text`**`(uint32_t flags = 0)`
* `Ustring Options::`**`version_text`**`() const`

These are the same texts that will be presented to the user by the `"--help"`
and `"--version"` options. The help text is constructed automatically by the
`Options` object; the version text is simply the original `info` string that
was supplied to the `Options` constructor.

The only flag that has any effect on `help_text()` is `Options::colour`, which
causes the text to be colourized using terminal control sequences. By default
the text is not colourized (note that this is different to the behaviour of
`parse()` with respect to the colour flags).

* `template <typename C> bool Options::`**`parse`**`(const std::vector<std::basic_string<C>>& args, std::ostream& out = std::cout, uint32_t flags = 0)`
* `template <typename C> bool Options::`**`parse`**`(const std::basic_string<C>& args, std::ostream& out = std::cout, uint32_t flags = 0)`
* `template <typename C> bool Options::`**`parse`**`(int argc, C** argv, std::ostream& out = std::cout, uint32_t flags = 0)`

After the option specification has been constructed, call one of the `parse()`
functions to parse the actual command line arguments. The arguments can be
supplied as a vector of strings, as a single combined string that will be
split apart during parsing, or as the standard `(argc,argv)` arguments from
`main()` (or a similar source such as the UTF-16 `_wmain()` often used on
Windows). Normally the supplied argument list is assumed to start with the
command name (which will be discarded); use the `noprefix` flag to
override this.

Boolean options will be recognised in normal or negated form (e.g. `"--magic"`
vs `"--no-magic"`). Integer options will accept hexadecimal options prefixed
with `"0x"`. Integer or floating point options that were specified with the
`si` flag will accept values tagged with SI prefix abbreviations (e.g. `"50k"`
or `"2.5M"`).

If a given option added to the specification more than once with different
prerequisites, which part of the spec that option matches will be determined
by which prerequisite is present. If there is more than one matching
prerequisite, the first one found will be used. Prerequisites in this
situation should normally be part of a mutual exclusion group.

If help or version information is requested, it will be written to the given
output stream (standard output by default). The `parse()` function will return
true if all requested processing has already been handled (i.e. if help or
version information has been presented to the user); the caller should check
the return value from `parse()` and end the program if it is true.

The `flags` argument can be any combination of these:

Flag                       | Description
----                       | -----------
`Options::`**`colour`**    | Always colourize the help text
`Options::`**`nocolour`**  | Do not colourize the help text
`Options::`**`locale`**    | The argument list is in the local encoding
`Options::`**`noprefix`**  | The first argument is not the command name
`Options::`**`quoted`**    | Allow arguments to be quoted

If the output stream is standard output, and no redirection is detected (using
`isatty()`), any help text generated will be colourized using terminal control
sequences. The `colour` and `nocolour` flags override this rule, forcing the
help text to always or never be colourized (if both are present, the effect is
the same as if neither was present).

The `locale` flag is only relevant to 8 bit strings, which are assumed to be
UTF-8 by default; the flag is ignored if the `C` type is not `char`, since 16
or 32 bit strings are always assumed to be UTF-16/32.

The `parse()` functions will throw `command_error` if any of the following is
true:

* A full or abbreviated option is supplied that is not in the spec.
* The same option appears more than once, but does not have the `multi` flag.
* A required option is missing.
* More than one option from the same mutual exclusion group is supplied.
* A boolean option is implied by another option but is also explicitly negated.
* An option that has a prerequisite is present, but its prerequisite is missing.
* The argument supplied for an option does not match the pattern or data type given in the spec.
* There are unattached arguments left over after all options have been satisfied.

Behaviour is undefined if `parse()` is called more than once on the same
`Options` object.

* `template <typename T> T Options::`**`get`**`(const Ustring& name) const`
* `template <typename T> std::vector<T> Options::`**`get_list`**`(const Ustring& name) const`
* `bool Options::`**`has`**`(const Ustring& name) const`

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

All of these will throw `spec_error` if the `name` string does not match one
of the registered options. The `get()` and `get_list()` functions may also
throw exceptions generated by `from_str<T>()`.
