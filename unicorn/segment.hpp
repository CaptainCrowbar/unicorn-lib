#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <deque>
#include <iterator>
#include <string>

namespace Unicorn {

    // Constants

    constexpr auto unicode_words    = Crow::Flagset::value('u');  // Report all UAX29 words (default)
    constexpr auto alpha_words      = Crow::Flagset::value('w');  // Report only words with alphanumerics
    constexpr auto keep_breaks      = Crow::Flagset::value('k');  // Include line/para terminators in results (default)
    constexpr auto strip_breaks     = Crow::Flagset::value('s');  // Do not include line/para terminators
    constexpr auto multiline_paras  = Crow::Flagset::value('m');  // Divide into paragraphs using multiple breaks (default)
    constexpr auto line_paras       = Crow::Flagset::value('l');  // Divide into paragraphs using any line break
    constexpr auto unicode_paras    = Crow::Flagset::value('u');  // Divide into paragraphs using only PS

    // Common base template for grapheme, word, and sentence iterators

    namespace UnicornDetail {

        template <typename Property> using PropertyQuery = Property (*)(char32_t);
        template <typename Property> using SegmentFunction = size_t (*)(const std::deque<Property>&, bool);

        size_t find_grapheme_break(const std::deque<Grapheme_Cluster_Break>& buf, bool eof);
        size_t find_word_break(const std::deque<Word_Break>& buf, bool eof);
        size_t find_sentence_break(const std::deque<Sentence_Break>& buf, bool eof);

    }

    template <typename C, typename Property, UnicornDetail::PropertyQuery<Property> PQ,
        UnicornDetail::SegmentFunction<Property> SF>
    class BasicSegmentIterator {
    public:
        using utf_iterator = UtfIterator<C>;
        using difference_type = ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using value_type = Crow::Irange<utf_iterator>;
        using pointer = const value_type*;
        using reference = const value_type&;
        BasicSegmentIterator() noexcept {}
        BasicSegmentIterator(const utf_iterator& i, const utf_iterator& j, Crow::Flagset flags):
            seg{i, i}, ends(j), next(i), bufsize(initsize), mode(flags) { ++*this; }
        const value_type& operator*() const noexcept { return seg; }
        const value_type* operator->() const noexcept { return &seg; }
        BasicSegmentIterator& operator++() noexcept;
        BasicSegmentIterator operator++(int) noexcept { auto i = *this; ++*this; return i; }
        bool operator==(const BasicSegmentIterator& rhs) const noexcept { return std::begin(seg) == std::begin(rhs.seg); }
        bool operator!=(const BasicSegmentIterator& rhs) const noexcept { return ! (*this == rhs); }
    private:
        static constexpr size_t initsize = 16;
        value_type seg {{}, {}};      // Iterator pair marking current segment
        size_t len {0};               // Length of segment
        utf_iterator ends {};         // End of source string
        utf_iterator next {};         // End of buffer contents
        std::deque<Property> buf {};  // Property lookahead buffer
        size_t bufsize {0};           // Current lookahead limit
        Crow::Flagset mode {};        // Mode flags
        bool select_segment() const noexcept;
    };

    template <typename C, typename Property, UnicornDetail::PropertyQuery<Property> PQ,
        UnicornDetail::SegmentFunction<Property> SF>
    BasicSegmentIterator<C, Property, PQ, SF>&
    BasicSegmentIterator<C, Property, PQ, SF>::operator++() noexcept {
        do {
            seg.first = seg.second;
            if (seg.first == ends)
                break;
            buf.erase(std::begin(buf), std::begin(buf) + len);
            for (;;) {
                while (next != ends && buf.size() < bufsize)
                    buf.push_back(PQ(*next++));
                len = SF(buf, next == ends);
                if (len || next == ends)
                    break;
                bufsize += initsize;
            }
            if (len == 0)
                seg.second = ends;
            else
                std::advance(seg.second, len);
        } while (! select_segment());
        return *this;
    }

    template <typename C, typename Property, UnicornDetail::PropertyQuery<Property> PQ,
        UnicornDetail::SegmentFunction<Property> SF>
    bool BasicSegmentIterator<C, Property, PQ, SF>::select_segment() const noexcept {
        return ! mode.get(alpha_words) || std::find_if(CROW_BOUNDS(seg), char_is_alphanumeric) != std::end(seg);
    }

    // Grapheme cluster boundaries

    template <typename C> using GraphemeIterator
        = BasicSegmentIterator<C, Grapheme_Cluster_Break, grapheme_cluster_break, UnicornDetail::find_grapheme_break>;

    template <typename C> Crow::Irange<GraphemeIterator<C>>
    grapheme_range(const UtfIterator<C>& i, const UtfIterator<C>& j) {
        return {{i, j, {}}, {j, j, {}}};
    }

    template <typename C> Crow::Irange<GraphemeIterator<C>>
    grapheme_range(const Crow::Irange<UtfIterator<C>>& source) {
        return grapheme_range(CROW_BOUNDS(source));
    }

    template <typename C> Crow::Irange<GraphemeIterator<C>>
    grapheme_range(const basic_string<C>& source) {
        return grapheme_range(utf_range(source));
    }

    // Word boundaries

    template <typename C> using WordIterator
        = BasicSegmentIterator<C, Word_Break, word_break, UnicornDetail::find_word_break>;

    template <typename C> Crow::Irange<WordIterator<C>>
    word_range(const UtfIterator<C>& i, const UtfIterator<C>& j, Crow::Flagset flags = {}) {
        flags.allow(unicode_words | alpha_words, "word breaking");
        flags.exclusive(unicode_words | alpha_words, "word breaking");
        return {{i, j, flags}, {j, j, flags}};
    }

    template <typename C> Crow::Irange<WordIterator<C>>
    word_range(const Crow::Irange<UtfIterator<C>>& source, Crow::Flagset flags = {}) {
        return word_range(CROW_BOUNDS(source), flags);
    }

    template <typename C> Crow::Irange<WordIterator<C>>
    word_range(const basic_string<C>& source, Crow::Flagset flags = {}) {
        return word_range(utf_range(source), flags);
    }

    // Sentence boundaries

    template <typename C> using SentenceIterator
        = BasicSegmentIterator<C, Sentence_Break, sentence_break, UnicornDetail::find_sentence_break>;

    template <typename C> Crow::Irange<SentenceIterator<C>>
    sentence_range(const UtfIterator<C>& i, const UtfIterator<C>& j) {
        return {{i, j, {}}, {j, j, {}}};
    }

    template <typename C> Crow::Irange<SentenceIterator<C>>
    sentence_range(const Crow::Irange<UtfIterator<C>>& source) {
        return sentence_range(CROW_BOUNDS(source));
    }

    template <typename C> Crow::Irange<SentenceIterator<C>>
    sentence_range(const basic_string<C>& source) {
        return sentence_range(utf_range(source));
    }

    // Common base template for line and paragraph iterators

    namespace UnicornDetail {

        // This takes a pair of UTF iterators marking the current position and
        // the end of the subject string, and returns a pair delimiting the
        // end-of-block marker.

        template <typename C>
        using FindBlockFunction = Crow::Irange<UtfIterator<C>> (*)(const UtfIterator<C>&, const UtfIterator<C>&);

        inline bool is_restricted_line_break(char32_t c)
            { return c == U'\n' || c == U'\v' || c == U'\r' || c == 0x85; }

        inline bool is_basic_para_break(char32_t c)
            { return is_restricted_line_break(c) || c == paragraph_separator_char; }

        template <typename C>
        Crow::Irange<UtfIterator<C>> find_end_of_line(const UtfIterator<C>& current, const UtfIterator<C>& endstr) {
            auto i = std::find_if(current, endstr, char_is_line_break);
            auto j = i;
            if (j != endstr)
                ++j;
            if (j != endstr && *i == U'\r' && *j == U'\n')
                ++j;
            return {i, j};
        }

        template <typename C>
        Crow::Irange<UtfIterator<C>> find_basic_para(const UtfIterator<C>& current, const UtfIterator<C>& endstr) {
            auto i = std::find_if(current, endstr, is_basic_para_break);
            auto j = i;
            if (j != endstr)
                ++j;
            if (j != endstr && *i == U'\r' && *j == U'\n')
                ++j;
            return {i, j};
        }

        template <typename C>
        Crow::Irange<UtfIterator<C>> find_multiline_para(const UtfIterator<C>& current, const UtfIterator<C>& endstr) {
            auto from = current;
            UtfIterator<C> i, j;
            for (;;) {
                i = j = std::find_if(from, endstr, char_is_line_break);
                if (i == endstr)
                    break;
                if (*i == paragraph_separator_char) {
                    ++j;
                    break;
                }
                if (*i == line_separator_char || *i == U'\f') {
                    from = std::next(i);
                    continue;
                }
                int linebreaks = 0;
                do {
                    auto pre = j;
                    ++j;
                    if (j != endstr && *pre == U'\r' && *j == U'\n')
                        ++j;
                    ++linebreaks;
                } while (j != endstr && is_restricted_line_break(*j));
                if (linebreaks >= 2)
                    break;
                from = j;
            }
            return {i, j};
        }

        template <typename C>
        Crow::Irange<UtfIterator<C>> find_unicode_para(const UtfIterator<C>& current, const UtfIterator<C>& endstr) {
            auto i = std::find(current, endstr, paragraph_separator_char);
            auto j = i;
            if (j != endstr)
                ++j;
            return {i, j};
        }

    }

    template <typename C>
    class BlockSegmentIterator {
    private:
        using find_block = UnicornDetail::FindBlockFunction<C>;
    public:
        using utf_iterator = UtfIterator<C>;
        using difference_type = ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using value_type = Crow::Irange<utf_iterator>;
        using pointer = const value_type*;
        using reference = const value_type&;
        BlockSegmentIterator() = default;
        BlockSegmentIterator(const utf_iterator& i, const utf_iterator& j, Crow::Flagset flags, find_block f) noexcept:
            next(i), ends(j), mode(flags), find(f) { ++*this; }
        const value_type& operator*() const noexcept { return seg; }
        const value_type* operator->() const noexcept { return &seg; }
        BlockSegmentIterator& operator++() noexcept;
        BlockSegmentIterator operator++(int) noexcept { auto i = *this; ++*this; return i; }
        bool operator==(const BlockSegmentIterator& rhs) const noexcept { return std::begin(seg) == std::begin(rhs.seg); }
        bool operator!=(const BlockSegmentIterator& rhs) const noexcept { return ! (*this == rhs); }
    private:
        value_type seg {{}, {}};    // Iterator pair marking current block
        utf_iterator next {};       // Start of next block
        utf_iterator ends {};       // End of source string
        Crow::Flagset mode {};      // Mode flags
        find_block find {nullptr};  // Find end of block
    };

    template <typename C>
    BlockSegmentIterator<C>& BlockSegmentIterator<C>::operator++() noexcept {
        seg.first = seg.second = next;
        if (next == ends)
            return *this;
        auto endblock = find(next, ends);
        seg.first = next;
        seg.second = mode.get(strip_breaks) ? endblock.first : endblock.second;
        next = endblock.second;
        return *this;
    }

    // Line boundaries

    template <typename C> using LineIterator = BlockSegmentIterator<C>;

    template <typename C>
    Crow::Irange<BlockSegmentIterator<C>> line_range(const UtfIterator<C>& i, const UtfIterator<C>& j,
            Crow::Flagset flags = {}) {
        using namespace UnicornDetail;
        flags.allow(keep_breaks | strip_breaks, "line breaking");
        flags.exclusive(keep_breaks | strip_breaks, "line breaking");
        return {{i, j, flags, find_end_of_line}, {j, j, flags, find_end_of_line}};
    }

    template <typename C>
    Crow::Irange<BlockSegmentIterator<C>> line_range(const Crow::Irange<UtfIterator<C>>& source,
            Crow::Flagset flags = {}) {
        return line_range(CROW_BOUNDS(source), flags);
    }

    template <typename C>
    Crow::Irange<BlockSegmentIterator<C>> line_range(const basic_string<C>& source,
            Crow::Flagset flags = {}) {
        return line_range(utf_range(source), flags);
    }

    // Paragraph boundaries

    template <typename C> using ParagraphIterator = BlockSegmentIterator<C>;

    template <typename C>
    Crow::Irange<BlockSegmentIterator<C>> paragraph_range(const UtfIterator<C>& i, const UtfIterator<C>& j,
            Crow::Flagset flags = {}) {
        using namespace UnicornDetail;
        flags.allow(keep_breaks | strip_breaks | multiline_paras | line_paras | unicode_paras, "paragraph breaking");
        flags.exclusive(keep_breaks | strip_breaks, "paragraph breaking");
        flags.exclusive(multiline_paras | line_paras | unicode_paras, "paragraph breaking");
        FindBlockFunction<C> f;
        if (flags.get(unicode_paras))
            f = find_unicode_para;
        else if (flags.get(line_paras))
            f = find_basic_para;
        else
            f = find_multiline_para;
        return {{i, j, flags, f}, {j, j, flags, f}};
    }

    template <typename C>
    Crow::Irange<BlockSegmentIterator<C>> paragraph_range(const Crow::Irange<UtfIterator<C>>& source,
            Crow::Flagset flags = {}) {
        return paragraph_range(CROW_BOUNDS(source), flags);
    }

    template <typename C>
    Crow::Irange<BlockSegmentIterator<C>> paragraph_range(const basic_string<C>& source,
            Crow::Flagset flags = {}) {
        return paragraph_range(utf_range(source), flags);
    }

}
