Title: Unicorn Library: Text Tokenization
CSS: style.css

# [Unicorn Library](index.html): Text Tokenization #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/lexer.hpp"` ####

This module defined a simple lexer or tokenizer, which can split text up into
tokens (optionally discarding unwanted tokens such as whitespace or comments)
according to rules defined by the user.

This module calls the [`unicorn/regex`](regex.html) module, which in turn
calls the PCRE library. It will only work with encodings for which the
corresponding PCRE library has been linked; see the regex module documentation
for details.

<p class="alert">[TODO: DOCUMENTATION]</p>

## Contents ##

* [Exceptions][]
* [Token structure][]
* [Token iterator][]
* [Lexer class][]

## Exceptions ##

* `class SyntaxError: public std::runtime_error`
    * `SyntaxError::SyntaxError(const u8string& text, size_t offset, const u8string& message = "Syntax error")`
    * `const char* SyntaxError::text() const noexcept`
    * `size_t SyntaxError::offset() const noxcept`

TODO

## Token structure ##

* `template <typename C> struct BasicToken`
    * `using BasicToken::char_type = C`
    * `using BasicToken::string_type = basic_string<C>`
    * `const BasicToken::string_type* text`
    * `size_t BasicToken::offset`
    * `size_t BasicToken::count`
    * `int BasicToken::tag`
    * `BasicToken::operator string_type() const`
* `using Token = BasicToken<char>`
* `using Token16 = BasicToken<char16_t>`
* `using Token32 = BasicToken<char32_t>`
* `using WideToken = BasicToken<wchar_t>`

TODO

## Token iterator ##

* `template <typename CX> class BasicTokenIterator`
    * `using BasicTokenIterator::char_type = [char if CX is void, otherwise CX]`
    * `using BasicTokenIterator::string_type = basic_string<char_type>`
    * `using BasicTokenIterator::difference_type = ptrdiff_t`
    * `using BasicTokenIterator::iterator_category = std::forward_iterator_tag`
    * `using BasicTokenIterator::pointer = const BasicToken<char_type>*`
    * `using BasicTokenIterator::reference = const BasicToken<char_type>&`
    * `using BasicTokenIterator::value_type = BasicToken<char_type>`
    * `BasicTokenIterator::BasicTokenIterator()`
    * `[standard iterator operations]`
* `using TokenIterator = BasicTokenIterator<char>`
* `using TokenIterator16 = BasicTokenIterator<char16_t>`
* `using TokenIterator32 = BasicTokenIterator<char32_t>`
* `using WideTokenIterator = BasicTokenIterator<wchar_t>`
* `using ByteTokenIterator = BasicTokenIterator<void>`

TODO

## Lexer class ##

* `template <typename CX> class BasicLexer`
* `using Lexer = BasicLexer<char>`
* `using Lexer16 = BasicLexer<char16_t>`
* `using Lexer32 = BasicLexer<char32_t>`
* `using WideLexer = BasicLexer<wchar_t>`
* `using ByteLexer = BasicLexer<void>`

TODO

* `using BasicLexer::char_type = [char if CX is void, otherwise CX]`
* `using BasicLexer::string_type = basic_string<char_type>`
* `using BasicLexer::regex_type = BasicRegex<CX>`
* `using BasicLexer::token_type = BasicToken<char_type>`
* `using BasicLexer::callback_type = std::function<size_t(const string_type&, size_t)>`
* `using BasicLexer::token_iterator = BasicTokenIterator<CX>`
* `using BasicLexer::token_range = Crow::Irange<token_iterator>`

TODO

* `BasicLexer::BasicLexer()`
* `BasicLexer::BasicLexer(const BasicLexer& lex)`
* `BasicLexer::BasicLexer(BasicLexer&& lex)`
* `BasicLexer::~BasicLexer()`
* `BasicLexer& BasicLexer::operator=(const BasicLexer& lex)`
* `BasicLexer& BasicLexer::operator=(BasicLexer&& lex)`

TODO

* `BasicLexer::token_range BasicLexer::lex(const string_type& text) const`
* `BasicLexer::token_range BasicLexer::operator()(const string_type& text) const`

TODO

* `void BasicLexer::call(int tag, const callback_type& call)`
* `void BasicLexer::exact(int tag, const string_type& pattern)`
* `void BasicLexer::exact(int tag, const char_type* pattern)`
* `void BasicLexer::match(int tag, const regex_type& pattern)`
* `void BasicLexer::match(int tag, const string_type& pattern, Crow::Flagset flags = {})`
* `void BasicLexer::match(int tag, const char_type* pattern, Crow::Flagset flags = {})`

TODO
