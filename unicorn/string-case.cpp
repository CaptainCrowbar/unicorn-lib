#include "unicorn/string.hpp"

namespace Unicorn {

    namespace {

        template <typename F>
        const U8string casemap_helper(const U8string& src, F f) {
            U8string dst;
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

    U8string str_uppercase(const U8string& str) {
        return casemap_helper(str, char_to_full_uppercase);
    }

    U8string str_lowercase(const U8string& str) {
        U8string dst;
        LowerChar lc;
        auto range = utf_range(str);
        auto out = utf_writer(dst);
        for (auto i = range.begin(), e = range.end(); i != e; ++i)
            lc.convert(i, e, out);
        return dst;
    }

    U8string str_titlecase(const U8string& str) {
        U8string dst;
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

    U8string str_casefold(const U8string& str) {
        return casemap_helper(str, char_to_full_casefold);
    }

    void str_uppercase_in(U8string& str) {
        auto result = str_uppercase(str);
        str.swap(result);
    }

    void str_lowercase_in(U8string& str) {
        auto result = str_lowercase(str);
        str.swap(result);
    }

    void str_titlecase_in(U8string& str) {
        auto result = str_titlecase(str);
        str.swap(result);
    }

    void str_casefold_in(U8string& str) {
        auto result = str_casefold(str);
        str.swap(result);
    }

}
