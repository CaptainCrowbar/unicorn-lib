// Internal to the library, do not include this directly

#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/segment.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <iterator>
#include <string>
#include <utility>

namespace Unicorn {

    // Remember that any other set of flags that might be combined with these
    // needs to skip the bits that are already spoken for. The string length
    // flags are linked to upper case letters for use with unicorn/format, so
    // they have bit positions in the 0-25 range.

    constexpr uint32_t character_units  = letter_to_mask('C');  // Measure string in characters (default)
    constexpr uint32_t grapheme_units   = letter_to_mask('G');  // Measure string in grapheme clusters
    constexpr uint32_t narrow_context   = letter_to_mask('N');  // East Asian width, defaulting to narrow
    constexpr uint32_t wide_context     = letter_to_mask('W');  // East Asian width, defaulting to wide

    namespace UnicornDetail {

        constexpr auto east_asian_flags = narrow_context | wide_context;
        constexpr auto all_length_flags = character_units | grapheme_units | east_asian_flags;

        inline void check_length_flags(uint32_t& flags) {
            if (bits_set(flags & (character_units | grapheme_units)) > 1
                    || bits_set(flags & (character_units | east_asian_flags)) > 1)
                throw std::invalid_argument("Inconsistent string length flags");
            if (bits_set(flags & all_length_flags) == 0)
                flags |= character_units;
        }

        class EastAsianCount {
        public:
            explicit EastAsianCount(uint32_t flags) noexcept: count(), fset(flags) { memset(count, 0, sizeof(count)); }
            void add(char32_t c) noexcept { ++count[unsigned(east_asian_width(c))]; }
            size_t get() const noexcept {
                size_t default_width = fset & wide_context ? 2 : 1;
                return count[neut] + count[half] + count[narr] + 2 * count[full] + 2 * count[wide]
                    + default_width * count[ambi];
            }
        private:
            static constexpr auto neut = int(East_Asian_Width::N);
            static constexpr auto ambi = int(East_Asian_Width::A);
            static constexpr auto full = int(East_Asian_Width::F);
            static constexpr auto half = int(East_Asian_Width::H);
            static constexpr auto narr = int(East_Asian_Width::Na);
            static constexpr auto wide = int(East_Asian_Width::W);
            size_t count[6];
            uint32_t fset;
        };

        template <typename C>
        std::pair<UtfIterator<C>, bool> find_position(const Irange<UtfIterator<C>>& range, size_t pos, uint32_t flags = 0) {
            check_length_flags(flags);
            if (flags & character_units) {
                auto i = range.begin();
                size_t len = 0;
                while (i != range.end() && len < pos) {
                    ++i;
                    ++len;
                }
                return {i, len == pos};
            } else if (flags & east_asian_flags) {
                EastAsianCount eac(flags);
                if (flags & grapheme_units) {
                    auto graph = grapheme_range(range);
                    auto g = graph.begin();
                    while (g != graph.end() && eac.get() < pos) {
                        eac.add(*g->begin());
                        ++g;
                    }
                    return {g->begin(), eac.get() >= pos};
                } else {
                    auto i = range.begin();
                    while (i != range.end() && eac.get() < pos) {
                        eac.add(*i);
                        ++i;
                    }
                    return {i, eac.get() >= pos};
                }
            } else {
                auto graph = grapheme_range(range);
                auto g = graph.begin();
                size_t len = 0;
                while (g != graph.end() && len < pos) {
                    ++g;
                    ++len;
                }
                return {g->begin(), len == pos};
            }
        }

    }

    template <typename C>
    size_t str_length(const Irange<UtfIterator<C>>& range, uint32_t flags = 0) {
        using namespace UnicornDetail;
        check_length_flags(flags);
        if (flags & character_units) {
            return range_count(range);
        } else if (flags & east_asian_flags) {
            EastAsianCount eac(flags);
            if (flags & grapheme_units) {
                for (auto g: grapheme_range(range))
                    eac.add(*g.begin());
            } else {
                for (auto c: range)
                    eac.add(c);
            }
            return eac.get();
        } else {
            return range_count(grapheme_range(range));
        }
    }

    template <typename C>
    size_t str_length(const UtfIterator<C>& b, const UtfIterator<C>& e, uint32_t flags = 0) {
        return str_length(irange(b, e), flags);
    }

    template <typename C>
    size_t str_length(const basic_string<C>& str, uint32_t flags = 0) {
        return str_length(utf_range(str), flags);
    }

    template <typename C>
    UtfIterator<C> str_find_index(const Irange<UtfIterator<C>>& range, size_t pos, uint32_t flags = 0) {
        return UnicornDetail::find_position(range, pos, flags).first;
    }

    template <typename C>
    UtfIterator<C> str_find_index(const UtfIterator<C>& b, const UtfIterator<C>& e, size_t pos, uint32_t flags = 0) {
        return str_find_index(irange(b, e), pos, flags);
    }

    template <typename C>
    UtfIterator<C> str_find_index(const basic_string<C>& str, size_t pos, uint32_t flags = 0) {
        return str_find_index(utf_range(str), pos, flags);
    }

    template <typename C>
    size_t str_find_offset(const basic_string<C>& str, size_t pos, uint32_t flags = 0) {
        auto rc = UnicornDetail::find_position(utf_range(str), pos, flags);
        return rc.second ? rc.first.offset() : npos;
    }

}
