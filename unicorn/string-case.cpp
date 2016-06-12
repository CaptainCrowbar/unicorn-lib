#include "unicorn/string-case.hpp"
#include "unicorn/character.hpp"
#include "unicorn/segment.hpp"
#include "unicorn/string-forward.hpp"
#include "unicorn/string-property.hpp"
#include "unicorn/string-size.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <iterator>

namespace Unicorn {

    namespace {

        template <typename F>
        const u8string casemap_helper(const u8string& src, F f) {
            u8string dst;
            char32_t buf[max_case_decomposition];
            auto out = utf_writer(dst);
            for (auto c: utf_range(src)) {
                auto n = f(c, buf);
                std::copy_n(buf, n, out);
            }
            return dst;
        }

        template <typename FwdIter>
        bool next_cased(FwdIter i, FwdIter e) {
            if (i == e)
                return false;
            for (++i; i != e; ++i)
                if (! char_is_case_ignorable(*i))
                    return char_is_cased(*i);
            return false;
        }

        struct LowerChar {
            static constexpr char32_t sigma = 0x3c3;
            static constexpr char32_t final_sigma = 0x3c2;
            bool last_cased = false;
            char32_t buf[max_case_decomposition];
            template <typename FwdIter, typename OutIter> void convert(FwdIter i, FwdIter e, OutIter to) {
                auto n = char_to_full_lowercase(*i, buf);
                if (buf[0] == sigma && last_cased && ! next_cased(i, e))
                    buf[0] = final_sigma;
                std::copy_n(buf, n, to);
                if (! char_is_case_ignorable(*i))
                    last_cased = char_is_cased(*i);
            }
        };

    }

    u8string str_uppercase(const u8string& str) {
        return casemap_helper(str, char_to_full_uppercase);
    }

    u8string str_lowercase(const u8string& str) {
        u8string dst;
        LowerChar lc;
        auto range = utf_range(str);
        auto out = utf_writer(dst);
        for (auto i = range.begin(), e = range.end(); i != e; ++i)
            lc.convert(i, e, out);
        return dst;
    }

    u8string str_titlecase(const u8string& str) {
        u8string dst;
        LowerChar lc;
        auto e = utf_end(str);
        auto out = utf_writer(dst);
        for (auto& w: word_range(str)) {
            bool initial = true;
            for (auto i = w.begin(); i != w.end(); ++i) {
                if (initial && char_is_cased(*i)) {
                    auto n = char_to_full_titlecase(*i, lc.buf);
                    std::copy_n(lc.buf, n, out);
                    lc.last_cased = true;
                    initial = false;
                } else {
                    lc.convert(i, e, out);
                }
            }
        }
        return dst;
    }

    u8string str_casefold(const u8string& str) {
        return casemap_helper(str, char_to_full_casefold);
    }

    void str_uppercase_in(u8string& str) {
        auto result = str_uppercase(str);
        str.swap(result);
    }

    void str_lowercase_in(u8string& str) {
        auto result = str_lowercase(str);
        str.swap(result);
    }

    void str_titlecase_in(u8string& str) {
        auto result = str_titlecase(str);
        str.swap(result);
    }

    void str_casefold_in(u8string& str) {
        auto result = str_casefold(str);
        str.swap(result);
    }

}
