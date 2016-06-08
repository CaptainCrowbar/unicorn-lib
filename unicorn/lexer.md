# [Unicorn Library](index.html): Text Tokenization #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/lexer.hpp"`

This module defines a simple lexer or tokenizer, which can split text up into
tokens (optionally discarding unwanted tokens such as whitespace or comments)
according to rules defined by the user.

Example:

    enum TokenType { name = 1, number = 2, op = 3 };

    Lexer lex;
    lex.match(0, "\\s+");
    lex.match(0, "#[^\\n]*");
    lex.match(name, "[a-z]\\w*", rx_caseless);
    lex.match(number, "\\d+");
    lex.match(op, "[-+*/=]");

    u8string source =
        "Hello world\n"
        "# Comment\n"
        "2 + 2 = 4\n";

    auto tokens = lex(source);

    for (auto& t: tokens)
        cout << "Type " << t.tag << ": " << u8string(t) << "\n";

Output:

    Type 1: Hello
    Type 1: world
    Type 2: 2
    Type 3: +
    Type 2: 2
    Type 3: =
    Type 2: 4

## Contents ##

[TOC]

## Exceptions ##

* `class` **`SyntaxError`**`: public std::runtime_error`
    * `SyntaxError::`**`SyntaxError`**`(const u8string& text, size_t offset, const u8string& message = "Syntax error")`
    * `const u8string& SyntaxError::`**`text`**`() const noexcept`
    * `size_t SyntaxError::`**`offset`**`() const noxcept`

Thrown when the lexer detects invalid syntax in the subject string. The
`text()` function returns the erroneous text (normally a single character that
could not be parsed); `offset()` returns its position (in code units) from the
start of the original subject string.

## Token structure ##

* `struct` **`Token`**
    * `const u8string* Token::`**`text`**
    * `size_t Token::`**`offset`**
    * `size_t Token::`**`count`**
    * `int Token::`**`tag`**
    * `Token::`**`operator u8string`**`() const`
* `using` **`TokenList`** `= vector<Token>`

This contains the details of a token. The `text` member points to the original
subject string; `offset` and `count` give the position and length of the token
(in code units). The `tag` member identifies the token type (one of a list of
user supplied constants indicating the various lexical elements that you
expect to find). The string conversion operator returns a copy of the token's
substring within the subject string (i.e. `text->substr(offset,count)`, or an
empty string if the text pointer is null).

## Token iterator ##

* `class` **`TokenIterator`**
    * `using TokenIterator::`**`iterator_category`** `= std::forward_iterator_tag`
    * `using TokenIterator::`**`value_type`** `= Token`
    * `TokenIterator::`**`TokenIterator`**`()`
    * _[standard iterator operations]_

An iterator over the tokens within a string. A lexer returns a pair of these
marking the beginning and end of the token stream. The iterator holds
references to both the lexer object and the subject string; behaviour is
undefined if either of those is changed or destroyed while the token iterator
exists.

## Lexer class ##

* `class` **`Lexer`**
    * `using Lexer::`**`callback_type`** `= function<size_t(const u8string&, size_t)>`
    * `Lexer::`**`Lexer`**`()`
    * `explicit Lexer::`**`Lexer`**`(uint32_t flags)`
    * `Lexer::`**`Lexer`**`(const Lexer& lex)`
    * `Lexer::`**`Lexer`**`(Lexer&& lex) noexcept`
    * `Lexer::`**`~Lexer`**`() noexcept`
    * `Lexer& Lexer::`**`operator=`**`(const Lexer& lex)`
    * `Lexer& Lexer::`**`operator=`**`(Lexer&& lex) noexcept`
    * `void Lexer::`**`exact`**`(int tag, const u8string& pattern)`
    * `void Lexer::`**`exact`**`(int tag, const char* pattern)`
    * `void Lexer::`**`match`**`(int tag, const Regex& pattern)`
    * `void Lexer::`**`match`**`(int tag, const u8string& pattern, uint32_t flags = 0)`
    * `void Lexer::`**`match`**`(int tag, const char* pattern, uint32_t flags = 0)`
    * `void Lexer::`**`custom`**`(int tag, const callback_type& call)`
    * `Irange<TokenIterator> Lexer::`**`operator()`**`(const u8string& text) const`
    * `void Lexer::`**`operator()`**`(const u8string& text, vector<Token>& tokens) const`

The lexer class. Normally this will be used by first adding a number of user
defined lexical rules through the `exact()`, `match()`, and `custom()`
functions, then applying the lexer to a subject string through one of the
function call operators. The first version of the function call returns a pair
of iterators that can be used to step through the token stream; the second
generates the complete token list at once.

The constructor that takes a `flags` argument accepts a bitmask of regular
expression flags, and adds them to any regex pattern later supplied through
the `match()` function; this is just a convenience to avoid having to repeat
the same flags on all `match()` calls.

The `tag` argument to the functions that add lexical rules identifies the type
of token that this rule will match. If the tag is zero, the token is assumed
to be unimportant (e.g. whitespace or a comment), and will not be returned by
the token iterator. Any other tag values can be given whatever meaning you
choose.

The `exact()` functions create the simplest rule, one that just matches a
single literal string. The `match()` functions create a rule that matches a
regular expression. The `custom()` function creates a rule that calls a user
supplied function, which will be passed a subject string and a code unit
offset into it, and is expected to return the length of the token starting at
that position, or zero if there is no match.

The lexer will accept the longest token that could match any of the rules at a
given point in the subject string. If there is more than one possible longest
match, the first matching rule (in the order in which they were added to the
lexer) will be accepted.

Lexical rules will never match an empty token (unless you try to increment the
token iterator past the end of the subject string). Regular expression rules
will not match an empty substring, even if the regex would normally match an
empty string (i.e. the regex is always matched as though it had the
`rx_nonempty` flag); an exact rule whose pattern string is empty will never be
matched.
