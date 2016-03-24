# [Unicorn Library](index.html): Text Tokenization #

_Unicode library for C++ by Ross Smith_

* `#include "unicorn/lexer.hpp"`

This module defined a simple lexer or tokenizer, which can split text up into
tokens (optionally discarding unwanted tokens such as whitespace or comments)
according to rules defined by the user.

This module calls the [`unicorn/regex`](regex.html) module, which in turn
calls the PCRE library. It will only work with encodings for which the
corresponding PCRE library has been linked; see the regex module documentation
for details.

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

* `template <typename C> struct` **`BasicToken`**
    * `using BasicToken::`**`char_type`** `= C`
    * `using BasicToken::`**`string_type`** `= basic_string<C>`
    * `const string_type* BasicToken::`**`text`**
    * `size_t BasicToken::`**`offset`**
    * `size_t BasicToken::`**`count`**
    * `int BasicToken::`**`tag`**
    * `BasicToken::`**`operator string_type`**`() const`
* `using` **`Token`** `= BasicToken<char>`
* `using` **`Token16`** `= BasicToken<char16_t>`
* `using` **`Token32`** `= BasicToken<char32_t>`
* `using` **`WideToken`** `= BasicToken<wchar_t>`

This contains the details of a token. The `text` member points to the original
subject string; `offset` and `count` give the position and length of the token
(in code units). The `tag` member identifies the token type (one of a list of
user supplied constants indicating the various lexical elements that you
expect to find). The string conversion operator returns a copy of the token's
substring within the subject string (i.e. `text->substr(offset,count)`, or an
empty string if the text pointer is null).

## Token iterator ##

* `template <typename C> class` **`BasicTokenIterator`**
    * `using BasicTokenIterator::`**`char_type`** `= C`
    * `using BasicTokenIterator::`**`difference_type`** `= ptrdiff_t`
    * `using BasicTokenIterator::`**`iterator_category`** `= std::forward_iterator_tag`
    * `using BasicTokenIterator::`**`pointer`** `= const BasicToken<C>*`
    * `using BasicTokenIterator::`**`reference`** `= const BasicToken<C>&`
    * `using BasicTokenIterator::`**`string_type`** `= basic_string<C>`
    * `using BasicTokenIterator::`**`value_type`** `= BasicToken<C>`
    * `BasicTokenIterator::`**`BasicTokenIterator`**`()`
    * _[standard iterator operations]_
* `using` **`TokenIterator`** `= BasicTokenIterator<char>`
* `using` **`TokenIterator16`** `= BasicTokenIterator<char16_t>`
* `using` **`TokenIterator32`** `= BasicTokenIterator<char32_t>`
* `using` **`WideTokenIterator`** `= BasicTokenIterator<wchar_t>`

An iterator over the tokens within a string. A lexer returns a pair of these
marking the beginning and end of the token stream. The iterator holds
references to both the lexer object and the subject string; behaviour is
undefined if either of those is changed or destroyed while the token iterator
exists.

## Lexer class ##

* `template <typename C> class` **`BasicLexer`**
    * `using BasicLexer::`**`callback_type`** `= function<size_t(const string_type&, size_t)>`
    * `using BasicLexer::`**`char_type`** `= C`
    * `using BasicLexer::`**`regex_type`** `= BasicRegex<C>`
    * `using BasicLexer::`**`string_type`** `= basic_string<C>`
    * `using BasicLexer::`**`token_iterator`** `= BasicTokenIterator<C>`
    * `using BasicLexer::`**`token_range`** `= Irange<token_iterator>`
    * `using BasicLexer::`**`token_type`** `= BasicToken<C>`
    * `BasicLexer::`**`BasicLexer`**`()`
    * `explicit BasicLexer::`**`BasicLexer`**`(uint32_t flags)`
    * `BasicLexer::`**`BasicLexer`**`(const BasicLexer& lex)`
    * `BasicLexer::`**`BasicLexer`**`(BasicLexer&& lex) noexcept`
    * `BasicLexer::`**`~BasicLexer`**`() noexcept`
    * `BasicLexer& BasicLexer::`**`operator=`**`(const BasicLexer& lex)`
    * `BasicLexer& BasicLexer::`**`operator=`**`(BasicLexer&& lex) noexcept`
    * `void BasicLexer::`**`exact`**`(int tag, const string_type& pattern)`
    * `void BasicLexer::`**`exact`**`(int tag, const C* pattern)`
    * `void BasicLexer::`**`match`**`(int tag, const regex_type& pattern)`
    * `void BasicLexer::`**`match`**`(int tag, const string_type& pattern, uint32_t flags = 0)`
    * `void BasicLexer::`**`match`**`(int tag, const C* pattern, uint32_t flags = 0)`
    * `void BasicLexer::`**`custom`**`(int tag, const callback_type& call)`
    * `BasicLexer::token_range BasicLexer::`**`operator()`**`(const string_type& text) const`
* `using` **`Lexer`** `= BasicLexer<char>`
* `using` **`Lexer16`** `= BasicLexer<char16_t>`
* `using` **`Lexer32`** `= BasicLexer<char32_t>`
* `using` **`WideLexer`** `= BasicLexer<wchar_t>`

The lexer class. Normally this will be used by first adding a number of user
defined lexical rules through the `exact()`, `match()`, and `custom()`
functions, then applying the lexer to a subject string through the function
call operator.

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
