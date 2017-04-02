# [Unicorn Library](index.html): Introduction #

_Unicode library for C++ by Ross Smith_

Please keep in mind that the Unicorn documentation does not attempt to be a
[Unicode](http://unicode.org/) tutorial; the reader is assumed to be familiar
with the basic concepts and terminology involved. I recommend reading the
Unicorn documentation with a copy of the Unicode standard on hand.

## Contents ##

[TOC]

## Features ##

Unicorn implements most of the main requirements of the Unicode standard and
related specifications. The major missing or incomplete features in the
current version are:

* The Unicode bidirectional algorithm.
* Localization support through the [CLDR](http://cldr.unicode.org/index),
* Support for IDNA/Punycode.
* User defined encoding conversions.

## Example ##

A sample program that demonstrates some of the facilities in the Unicorn
library. This program reads a text file, splits it into words, converts each
word to normalization form NFC and lower case, and writes out a list of all
words encountered more than once, in descending order of frequency.

    #include "unicorn/library.hpp"
    #include <cstdlib>
    #include <exception>
    #include <iostream>
    #include <map>
    #include <string>

    using namespace Unicorn;
    using namespace Unicorn::Literals;

    int main(int argc, char** argv) {

        try {

            const U8string description =
                "This program reads a text file, splits it into words, "
                "converts each word to normalization form NFC and lower case, "
                "and writes out a list of all words encountered more than once, "
                "in descending order of frequency.";

            // Parse the command line options

            Options opt("Unicorn Demo", str_wrap(description, 0, 75));

            opt.add("input", "Input file (default is standard input)", opt_abbrev="i", opt_default="-");
            opt.add("output", "Output file (default is standard output)", opt_abbrev="o", opt_default="-");
            opt.add("encoding", "Input encoding", opt_abbrev="e", opt_default="UTF-8");

            if (opt.parse(argc, argv))
                return 0;

            auto input_file = opt.get<U8string>("input");
            auto output_file = opt.get<U8string>("output");
            auto input_encoding = opt.get<U8string>("encoding");

            // Read all the words in the input file

            std::map<U8string, size_t> census;

            for (auto& line: read_lines(input_file, io_stdin, input_encoding))
                for (auto& word: word_range(line, alpha_words))
                    ++census[str_lowercase(normalize(u_str(word), NFC))];

            // Order them by descending frequency

            std::multimap<size_t, U8string, std::greater<>> frequencies;

            for (auto& pair: census)
                if (pair.second > 1)
                    frequencies.insert({pair.second, pair.first});

            for (auto& pair: frequencies)
                cout << "[$1] $2\n"_fmt(pair.first, pair.second);

            return 0;

        }

        catch (const std::exception& ex) {
            std::cerr << "*** " << ex.what() << "\n";
            return 1;
        }

    }

If the program is run with the `--help` (or `-h`) option, it will print the
following usage guide:

    Unicorn Demo

    This program reads a text file, splits it into words, converts each word to
    normalization form NFC and lower case, and writes out a list of all words
    encountered more than once, in descending order of frequency.

    Options:
        --input, -i <arg>     = Input file (default is standard input)
        --output, -o <arg>    = Output file (default is standard output)
        --encoding, -e <arg>  = Input encoding (default "UTF-8")
        --help, -h            = Show usage information
        --version, -v         = Show version information

## Compatibility ##

Unicorn is written in C++14; you will need an up-to-date C++ compiler,
probably a recent version of Clang or GCC. I developed it mainly on macOS,
currently using Clang 7.0. All test builds are made using strict compilation
mode (`-Wall -Wextra -Werror`).

Unicorn has also been tested with GCC 5.x and 6.x on Ubuntu GNU/Linux,
[Cygwin](http://www.cygwin.com/) on Microsoft Windows, and native Windows
using [Nuwen-Mingw](http://nuwen.net/mingw.html), although I can't promise
that the most recent checkin will have been tested with all of those.

Currently, Unicorn will not work with any existing release of Microsoft Visual
Studio. The latest release, VS 2015 update 2 probably has sufficiently good
support for C++14 and for UTF-8 source code, so I hope to be able to port
Unicorn to MSVC in the future.

## Design philosophy ##

The Unicorn library is still in a fairly early form. The code is primarily
striving to be correct, with performance a secondary consideration; aggressive
optimization can come later.

One of the basic design goals of Unicorn was to make it work well with code
written in the style of existing idiomatic C++11/14, rather than requiring
users to change their coding style to accommodate Unicorn. In particular, it
uses the existing string classes supplied by the standard C++ library, rather
than adding its own new string type. I would have preferred to also work with
standard C++ regular expressions instead of creating a new regex class, but
this turned out to be impractical (see the [`unicorn/regex`](regex.html)
documentation for the details).

UTF-8, 16, and 32 strings are represented by `U8string`, `u16string`, and
`u32string` respectively; you can also use `wstring` for UTF-16 or 32,
whichever is appropriate for your system (systems where `wstring` is not
UTF-16 or 32 are not supported). The name `U8string` is an alias for
`std::string`; the intention here is that `U8string` is used for strings
required to contain valid UTF-8 (or ASCII), while plain `string` is used for
strings in some other encoding, or for when a string is used simply as an
array of bytes with no assumptions about encoding. This is intended purely as
an aid to code clarity, and has no effect on the compiler; `U8string` is just
an alias for `string`, not a new type.

Unicorn assumes that most or all of your string processing will be done in
Unicode. Programs that need to work with text in other formats should convert
the text to Unicode at the earliest opportunity upon reading it in, do their
processing with strings known to be valid Unicode, and then convert to other
encodings, if necessary, upon output. Functions for conversion between Unicode
and other encodings can be found in the [`unicorn/mbcs`](mbcs.html) and
[`unicorn/io`](io.html) modules. Most of the other modules are intended to
work only with Unicode strings that have already been validated. The
documentation for the [`unicorn/utf`](utf.html) module explains the details of
how the handling of invalid encoding is controlled.

Earlier versions of Unicorn largely treated all four standard string types
equally; in practise, however, the world is increasingly moving toward
consistently using [UTF-8 everywhere](http://utf8everywhere.org), and
experience with using Unicorn in real projects has shown that 16 and 32 bit
strings are used rarely and only for a few specialized purposes. (Calling 16
bit APIs in Microsoft Windows is probably the most common use for non-UTF-8
strings in practise.) I concluded that supporting all three character
encodings in the entire Unicorn API, when only one is really needed, was just
complicating the code (and extending compile times) to no useful purpose.
Unicorn is now designed to handle text almost entirely in UTF-8; support for
the other encoding forms and string classes is mostly present now only in the
encoding conversion functions in the [`unicorn/utf`](utf.html) module, and in
a handful of other places (such as the string length functions in
[`unicorn/string`](string.html)) where they may be particularly useful.

## Coding conventions ##

In the context of Unicode and other text encodings, the word "character" is
often used confusingly with any number of different meanings. In the Unicorn
documentation, **character** is normally used to mean one Unicode scalar
value, represented conceptually as a single 21 bit integer, and in practise as
one to four bytes in various encodings. Following the standard Unicode
terminology, the integer types used for encoding (`char` in UTF-8, `char16_t`
in UTF-16, and `char32_t` in UTF-32) are referred to as **code units**.

A Unicode "character" is a character as seen by the programmer, rather than a
character as seen by the user; a single user-perceived character (or
**grapheme cluster** in the official Unicode terminology) may contain one or
more Unicode characters, and the same user-perceived character may be
represented in different ways in the encoded text (e.g. composed vs decomposed
characters). The Unicorn library includes functions for identifying grapheme
clusters (in the [`unicorn/segment`](segment.html) module), and for converting
text to the standard normalization forms (in the
[`unicorn/normal`](normal.html) module).

A related point of confusion is the concept of the "length" of a string. When
text may be represented in variable-length encodings such as UTF-8 and UTF-16,
we need to be clear on whether we are measuring in characters, code units, or
some other unit such as grapheme clusters. In Unicorn, I have adopted the
convention that a **count** is the size of a string in code units, while a
**length** is in characters; similarly, an **offset** is a position within a
string measured in code units from the start of the string, while an **index**
is a position measured in characters.

In some contexts a string's size may be measured in other units, such as
grapheme clusters; these variations are described in the documentation of the
relevant functions (in particular, see `str_length()` and related functions in
[`unicorn/string`](string.html)).

The term **character type** is used to mean one of the four standard types
that can be used as the code unit type in an instantiation of
`std::basic_string`, i.e. `char`, `char16_t`, `char32_t`, and `wchar_t`. Note
that this does not include `signed char` or `unsigned char`; although these
are "character types" in a sense, they can't be used as the value type of a
standard string (at least not without writing a new traits class; Unicorn is
not intended to be compatible with custom string types).

Template parameters are usually named to indicate what kind of type is
expected (the requirement may not be explicitly documented if I think the
parameter name makes it sufficiently clear). When a parameter type is named
`C` (or `C1`, `C2`, etc), it is expected to be a character type as defined
above.

The abbreviations `src` and `dst` are frequently used for source and
destination objects (i.e. input and output parameters).

## Exception safety ##

The `noexcept` qualifier is used as much as possible to indicate functions
that will never throw exceptions (in the absence of undefined behaviour).

In a library primarily concerned with string manipulation, almost any
operation might implicitly allocate one or more strings and therefore
potentially trigger an out-of-memory error. You should assume that any
function not marked `noexcept` may throw a `std::bad_alloc` exception; because
this is so ubiquitous it is not usually mentioned in the documentation for
individual functions.

Functions that take a callback function as an argument will propagate any
exceptions thrown by the callback.

Functions that accept UTF iterator arguments (see the
[`unicorn/utf`](utf.html) module) can throw `EncodingError` if invalid Unicode
is encountered and the appropriate error handling flag was selected when the
iterators were created.

If a function is documented as throwing a specific exception class, it may
actually throw a class derived from that one.

Any other exceptions that a function might throw are described in that
function's description. If a function is not marked `noexcept`, but its
documentation does not explicitly mention any exceptions, then it may throw
`std::bad_alloc`, and possibly `Unicorn::EncodingError` if it implicitly does
a checked encoding conversion, but will not throw anything else.

## Building Unicorn ##

A makefile is included, although it may not be as portable as I would like.
Building Unicorn using your preferred build tool should be simple enough. To
build the library, compile all the `.cpp` files whose names don't include the
word `test`; to build the test program, compile all the test modules and link
them with the library.

If you want to make changes to the code, you may need to rebuild the Unicode
character tables from the original data. You can do this by first running
`scripts/download-ucd` to download the original tables from the Unicode
website, then `scripts/make-tables` to generate the C++ tables used by Unicorn
(this script requires Python 3.4+). This is not necessary if you're just
building the library and not modifying it, or if your changes don't require
any tables to be rebuilt; all the precompiled tables are already included in
the source tree.

Rebuilding the documentation (`make doc`) requires Python 3.4+ and the
[Markdown module](https://pypi.python.org/pypi/Markdown). The scripts expect
Python 3 to be callable as `python3`.

Unicorn requires some other libraries to be linked with programs that use it;
all of these should be present or easily installed on most systems:

* [PCRE](http://www.pcre.org/) (`-lpcre`)
* [Zlib](http://www.zlib.net/) (`-lz`)
* Iconv for Unix targets (on some systems this is implicit, on others it requires `-liconv`)
* The system thread library (`-lpthread` on most Unix systems, `-mthreads` on Mingw)

## Using Unicorn ##

You can import the whole library using `#include "unicorn/library.hpp"`, or
include the individual modules that you need. Link with `-lunicorn`, and with
the other required libraries described above.

Everything in the Unicorn library is in `namespace Unicorn` (or a subordinate
namespace inside `Unicorn`). You can either qualify all calls with the
`Unicorn::` prefix, or use `using namespace Unicorn`.

Unicorn calls my [Prion library](https://github.com/CaptainCrowbar/prion-lib).
This consists only of a single header (and a second one if you build the unit
tests); no special build actions are required beyond making sure the compiler
can find the `"prion/core.hpp"` header. Everything in `namespace Prion` is
imported into `namespace Unicorn`.
