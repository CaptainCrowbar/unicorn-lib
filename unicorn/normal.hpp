#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include <string>

namespace Unicorn {

    // Normalization functions

    namespace UnicornDetail {

        template <typename C>
        void apply_decomposition(const basic_string<C>& src, u32string& dst, bool k) {
            auto decompose = k ? compatibility_decomposition : canonical_decomposition;
            size_t max_decompose = k ? max_compatibility_decomposition : max_canonical_decomposition;
            char32_t buf[max_decompose];
            dst.reserve(src.size());
            size_t pos = 0;
            for (char32_t c: utf_range(src)) {
                dst.resize(pos + max_decompose);
                size_t len = decompose(c, &dst[pos]);
                if (len == 0) {
                    dst.resize(++pos);
                    dst.back() = c;
                } else {
                    dst.resize(pos + len);
                    while (pos < dst.size()) {
                        len = decompose(dst[pos], buf);
                        if (len == 0)
                            ++pos;
                        else
                            dst.replace(pos, 1, buf, len);
                    }
                }
            }
        }

        void apply_ordering(u32string& str);
        void apply_composition(u32string& str);

    }

    enum NormalizationForm {
        NFC, NFD, NFKC, NFKD
    };

    template <typename C>
    basic_string<C> normalize(const basic_string<C>& src, NormalizationForm form) {
        using namespace UnicornDetail;
        u32string utf32;
        apply_decomposition(src, utf32, form == NFKC || form == NFKD);
        apply_ordering(utf32);
        if (form == NFC || form == NFKC)
            apply_composition(utf32);
        return recode<C>(utf32);
    }

    template <typename C>
    void normalize_in(basic_string<C>& src, NormalizationForm form) {
        using namespace UnicornDetail;
        u32string utf32;
        apply_decomposition(src, utf32, form == NFKC || form == NFKD);
        apply_ordering(utf32);
        if (form == NFC || form == NFKC)
            apply_composition(utf32);
        recode(utf32, src);
    }

}
