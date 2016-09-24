# Unicorn Library #

_Unicode library for C++ by Ross Smith_

The Unicorn library is intended to make [Unicode](http://www.unicode.org/)
text manipulation easy for programmers using modern C++.
It implements most of the main requirements of the
[Unicode standard](http://www.unicode.org/versions/latest/),
currently supporting Unicode version 8.0.

The library is divided into the modules listed below; you can import only the
ones you need, or import the whole API using `#include "unicorn/library.hpp"`.
Everything in the Unicorn library is in `namespace Unicorn` (or a subordinate
namespace inside `Unicorn`). You can either qualify all calls with the
`Unicorn::` prefix, or use `using namespace Unicorn`.

The Unicorn library depends on the utilities and unit testing framework in my
[Prion library](https://github.com/CaptainCrowbar/prion-lib).

* <!-- TEXT --> **Documentation**
    * [Introduction to the Unicorn Library](intro.html)
* **Core definitions**
    * [`"unicorn/core.hpp"`](core.html) -- Core definitions.
* **Character and string encoding**
    * [`"unicorn/character.hpp"`](character.html) -- Unicode characters and their basic properties.
    * [`"unicorn/utf.hpp"`](utf.html) -- The standard UTF encodings, and conversions between them.
    * [`"unicorn/mbcs.hpp"`](mbcs.html) -- Conversion between UTF and non-Unicode encodings.
* **Operations on strings**
    * [`"unicorn/string.hpp"`](string.html) -- A collection of generic string manipulation functions.
    * [`"unicorn/regex.hpp"`](regex.html) -- Unicode regular expressions.
    * [`"unicorn/normal.hpp"`](normal.html) -- The standard Unicode normalization forms.
* **Text formatting and parsing**
    * [`"unicorn/segment.hpp"`](segment.html) -- Breaking text up into characters, words, sentences, lines, and paragraphs.
    * [`"unicorn/format.hpp"`](format.html) -- Formatting various kinds of data as Unicode strings.
    * [`"unicorn/lexer.hpp"`](lexer.html) -- Breaking text up into tokens by user-defined rules.
* **Interfacing with the outside world**
    * [`"unicorn/environment.hpp"`](environment.html) -- Environment variables.
    * [`"unicorn/file.hpp"`](file.html) -- Basic operations on the file system.
    * [`"unicorn/io.hpp"`](io.html) -- File input and output.
    * [`"unicorn/options.hpp"`](options.html) -- Parsing command line options.
