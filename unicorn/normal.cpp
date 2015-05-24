/*

Unicode Normalization Algorithm:

References:
Unicode Standard 7.0, chapter 3.11, "Normalization Forms"
Unicode Standard Annex #15, "Unicode Normalization Forms"

Step 1: Decompose any decomposable characters. For NFC and NFD, apply only
canonical decompositions; for NFKC and NFKD, also apply compatibility
decompositions. Decompose characters recursively until no more decompositions
are possible.

Step 2: For each contiguous subsequence of non-starter characters, sort the
characters into ascending order by canonical combining class. Characters with
the same combining class are not reordered (i.e. stable sort).

Step 3 (only for NFC and NFKC): For each pair of characters that form a
canonical composing pair, replace the first with the composed character and
delete the second. A pair need not be adjacent to be composable, if the
characters between them are all non-starters with a lower combining class than
the second of the pair. Compose characters recursively until no more
compositions are possible.

*/

#include "unicorn/normal.hpp"
#include <algorithm>

using namespace std::literals;

namespace Unicorn {

    namespace UnicornDetail {

        void apply_ordering(std::u32string& str) {
            auto i = std::begin(str), j = i, e = std::end(str);
            while (j != e) {
                i = std::find_if(j, e, combining_class);
                if (i == e)
                    break;
                j = std::find_if_not(i, e, combining_class);
                std::stable_sort(i, j, [] (char32_t a, char32_t b)
                    { return combining_class(a) < combining_class(b); });
            }
        }

        void apply_composition(std::u32string& str) {
            if (str.size() < 2)
                return;
            auto i = std::begin(str), e = std::end(str);
            while (i != e) {
                i = std::find_if_not(i, e, combining_class);
                if (i == e)
                    break;
                bool combined = false;
                int prev_cc = 0;
                for (auto j = i + 1; j != e; ++j) {
                    int cc = combining_class(*j);
                    if (prev_cc == 0 || prev_cc < cc) {
                        char32_t c = canonical_composition(*i, *j);
                        if (c) {
                            *i = c;
                            str.erase(j);
                            combined = true;
                            break;
                        }
                    }
                    if (cc == 0)
                        break;
                    prev_cc = cc;
                }
                if (! combined)
                    ++i;
            }
        }

    }

}
