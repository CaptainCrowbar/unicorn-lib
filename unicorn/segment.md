Title: Unicorn Library: Text Segmentation
CSS: style.css

# [Unicorn Library](index.html): Text Segmentation #

#### Unicode library for C++ by Ross Smith ####

#### `#include "unicorn/segment.hpp"` ####

This module contains classes and functions for breaking text up into
characters, words, sentences, lines, and paragraphs. Most of the rules
followed here are defined in [Unicode Standard Annex 29: Unicode Text
Segmentation](http://www.unicode.org/reports/tr29/).

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

TODO

* `template <typename C> Crow::Irange<GraphemeIterator<C>> grapheme_range(const UtfIterator<C>& i, const UtfIterator<C>& j)`
* `template <typename C> Crow::Irange<GraphemeIterator<C>> grapheme_range(const Crow::Irange<UtfIterator<C>>& source)`
* `template <typename C> Crow::Irange<GraphemeIterator<C>> grapheme_range(const basic_string<C>& source)`

TODO

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

TODO

* `template <typename C> Crow::Irange<WordIterator<C>> word_range(const UtfIterator<C>& i, const UtfIterator<C>& j, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<WordIterator<C>> word_range(const Crow::Irange<UtfIterator<C>>& source, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<WordIterator<C>> word_range(const basic_string<C>& source, Crow::Flagset flags = {})`

TODO

Bitmask          | Letter  | Description
-------          | ------  | -----------
`unicode_words`  | `u`     | Report all UAX29 words (default)
`alpha_words`    | `a`     | Report only words with alphanumerics

TODO

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

TODO

* `template <typename C> Crow::Irange<SentenceIterator<C>> sentence_range(const UtfIterator<C>& i, const UtfIterator<C>& j)`
* `template <typename C> Crow::Irange<SentenceIterator<C>> sentence_range(const Crow::Irange<UtfIterator<C>>& source)`
* `template <typename C> Crow::Irange<SentenceIterator<C>> sentence_range(const basic_string<C>& source)`

TODO

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

TODO

* `template <typename C> Crow::Irange<LineIterator<C>> line_range(const UtfIterator<C>& i, const UtfIterator<C>& j, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<LineIterator<C>> line_range(const Crow::Irange<UtfIterator<C>>& source, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<LineIterator<C>> line_range(const basic_string<C>& source, Crow::Flagset flags = {})`

TODO

Bitmask         | Letter  | Description
-------         | ------  | -----------
`keep_breaks`   | `k`     | Include line terminators in returned strings (default)
`strip_breaks`  | `s`     | Do not include line terminators

TODO

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

TODO

* `template <typename C> Crow::Irange<ParagraphIterator<C>> paragraph_range(const UtfIterator<C>& i, const UtfIterator<C>& j, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<ParagraphIterator<C>> paragraph_range(const Crow::Irange<UtfIterator<C>>& source, Crow::Flagset flags = {})`
* `template <typename C> Crow::Irange<ParagraphIterator<C>> paragraph_range(const basic_string<C>& source, Crow::Flagset flags = {})`

TODO

Bitmask            | Letter  | Description
-------            | ------  | -----------
`keep_breaks`      | `k`     | Include paragraph terminators in returned strings (default)
`strip_breaks`     | `s`     | Do not include paragraph terminators
`multiline_paras`  | `m`     | Divide into paragraphs using multiple line breaks (default)
`line_paras`       | `l`     | Divide into paragraphs using any line break
`unicode_paras`    | `u`     | Divide into paragraphs using only PS

TODO
