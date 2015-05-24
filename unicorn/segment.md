Title: Unicorn Library: Text Segmentation
CSS: style.css

# [Unicorn Library](index.html): Text Segmentation #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/segment.hpp"` ####

This module contains classes and functions for breaking text up into
characters, words, sentences, lines, and paragraphs. Most of the rules
followed here are defined in [Unicode Standard Annex 29: Unicode Text
Segmentation](http://www.unicode.org/reports/tr29/).

All of the iterators defined here dereference to a substring represented by a
pair of [UTF iterators](unicorn/utf.html), bracketing the text segment of
interest. As usual, the `u_str()` function can be used to copy the actual
substring if this is needed.

## Contents ##

* [Grapheme cluster boundaries][]
* [Word boundaries][]
* [Sentence boundaries][]
* [Line boundaries][]
* [Paragraph boundaries][]

## Grapheme cluster boundaries ##

* `template <typename C> class GraphemeIterator`
    * `using GraphemeIterator::utf_iterator = UtfIterator<C>`
    * `using GraphemeIterator::difference_type = ptrdiff_t`
    * `using GraphemeIterator::iterator_category = std::forward_iterator_tag`
    * `using GraphemeIterator::value_type = Crow::Irange<utf_iterator>`
    * `using GraphemeIterator::pointer = const value_type*`
    * `using GraphemeIterator::reference = const value_type&`
    * `GraphemeIterator::GraphemeIterator()`
    * `[standard iterator operations]`
* `template <typename C> Crow::Irange<GraphemeIterator<C>> grapheme_range(const UtfIterator<C>& i, const UtfIterator<C>& j)`
* `template <typename C> Crow::Irange<GraphemeIterator<C>> grapheme_range(const Crow::Irange<UtfIterator<C>>& source)`
* `template <typename C> Crow::Irange<GraphemeIterator<C>> grapheme_range(const basic_string<C>& source)`

A forward iterator over the grapheme clusters (user-perceived characters) in a
Unicode string.

## Word boundaries ##

* `template <typename C> class WordIterator`
    * `using WordIterator::utf_iterator = UtfIterator<C>`
    * `using WordIterator::difference_type = ptrdiff_t`
    * `using WordIterator::iterator_category = std::forward_iterator_tag`
    * `using WordIterator::value_type = Crow::Irange<utf_iterator>`
    * `using WordIterator::pointer = const value_type*`
    * `using WordIterator::reference = const value_type&`
    * `WordIterator::WordIterator()`
    * `[standard iterator operations]`
* `template <typename C> Crow::Irange<WordIterator<C>> word_range(const UtfIterator<C>& i, const UtfIterator<C>& j, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<WordIterator<C>> word_range(const Crow::Irange<UtfIterator<C>>& source, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<WordIterator<C>> word_range(const basic_string<C>& source, Crow::Flagset flags = {})`

A forward iterator over the words in a Unicode string. By default, all
segments identified as "words" by the UAX29 algorithm are returned; this will
include whitespace between words, punctuation marks, etc. Flags can be used to
select only words containing at least one non-whitespace character, or only
words containing at least one alphanumeric character.

Bitmask          | Letter  | Description
-------          | ------  | -----------
`unicode_words`  | `u`     | Report all UAX29 words (default)
`graphic_words`  | `g`     | Report only words containing a non-whitespace character
`alpha_words`    | `a`     | Report only words containing an alphanumeric character

## Sentence boundaries ##

* `template <typename C> class SentenceIterator`
    * `using SentenceIterator::utf_iterator = UtfIterator<C>`
    * `using SentenceIterator::difference_type = ptrdiff_t`
    * `using SentenceIterator::iterator_category = std::forward_iterator_tag`
    * `using SentenceIterator::value_type = Crow::Irange<utf_iterator>`
    * `using SentenceIterator::pointer = const value_type*`
    * `using SentenceIterator::reference = const value_type&`
    * `SentenceIterator::SentenceIterator()`
    * `[standard iterator operations]`
* `template <typename C> Crow::Irange<SentenceIterator<C>> sentence_range(const UtfIterator<C>& i, const UtfIterator<C>& j)`
* `template <typename C> Crow::Irange<SentenceIterator<C>> sentence_range(const Crow::Irange<UtfIterator<C>>& source)`
* `template <typename C> Crow::Irange<SentenceIterator<C>> sentence_range(const basic_string<C>& source)`

A forward iterator over the sentences in a Unicode string (as defined by
UAX29).

## Line boundaries ##

* `template <typename C> class LineIterator`
    * `using LineIterator::utf_iterator = UtfIterator<C>`
    * `using LineIterator::difference_type = ptrdiff_t`
    * `using LineIterator::iterator_category = std::forward_iterator_tag`
    * `using LineIterator::value_type = Crow::Irange<utf_iterator>`
    * `using LineIterator::pointer = const value_type*`
    * `using LineIterator::reference = const value_type&`
    * `LineIterator::LineIterator()`
    * `[standard iterator operations]`
* `template <typename C> Crow::Irange<LineIterator<C>> line_range(const UtfIterator<C>& i, const UtfIterator<C>& j, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<LineIterator<C>> line_range(const Crow::Irange<UtfIterator<C>>& source, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<LineIterator<C>> line_range(const basic_string<C>& source, Crow::Flagset flags = {})`

A forward iterator over the lines in a Unicode string. Lines are ended by any
character with the line break property. Multiple consecutive line break
characters are treated as separate lines; except that `CR+LF` is treated as a
single line break. By default, the segment identified by the dereferenced
iterator includes the terminating line break; if the `strip_breaks` flag is
set, the line break is excluded from the segment.

Bitmask         | Letter  | Description
-------         | ------  | -----------
`keep_breaks`   | `k`     | Include line terminators in reported segments (default)
`strip_breaks`  | `s`     | Do not include line terminators

## Paragraph boundaries ##

* `template <typename C> class ParagraphIterator`
    * `using ParagraphIterator::utf_iterator = UtfIterator<C>`
    * `using ParagraphIterator::difference_type = ptrdiff_t`
    * `using ParagraphIterator::iterator_category = std::forward_iterator_tag`
    * `using ParagraphIterator::value_type = Crow::Irange<utf_iterator>`
    * `using ParagraphIterator::pointer = const value_type*`
    * `using ParagraphIterator::reference = const value_type&`
    * `ParagraphIterator::ParagraphIterator()`
    * `[standard iterator operations]`
* `template <typename C> Crow::Irange<ParagraphIterator<C>> paragraph_range(const UtfIterator<C>& i, const UtfIterator<C>& j, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<ParagraphIterator<C>> paragraph_range(const Crow::Irange<UtfIterator<C>>& source, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<ParagraphIterator<C>> paragraph_range(const basic_string<C>& source, Crow::Flagset flags = {})`

A forward iterator over the paragraphs in a Unicode string. The flags passed
to the constructor determine how paragraphs are identified. By default, any
sequence of two or more line breaks ends a paragraph (as usual, `CR+LF` counts
as a single line break); the `line_paras` flag causes every line break to be
interpreted as a paragraph break, while `unicode_paras` restricts paragraph
breaks to the Unicode paragraph separator character (`U+2029`). The
`strip_breaks` flag works the same way as in `LineIterator`, skipping the
paragraph delimiters.

Bitmask            | Letter  | Description
-------            | ------  | -----------
`multiline_paras`  | `m`     | Divide into paragraphs using multiple line breaks (default)
`line_paras`       | `l`     | Divide into paragraphs using any line break
`unicode_paras`    | `u`     | Divide into paragraphs using only Paragraph Separator
`keep_breaks`      | `k`     | Include paragraph terminators in reported segments (default)
`strip_breaks`     | `s`     | Do not include paragraph terminators
