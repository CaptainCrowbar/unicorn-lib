#include "unicorn/format.hpp"
#include <cmath>
#include <cstdio>

using namespace RS::Unicorn::Literals;
using namespace std::chrono;
using namespace std::literals;

namespace RS::Unicorn {

    namespace UnicornDetail {

        namespace {

            // These will always be called with x>=0 and prec>=0

            Ustring float_print(long double x, int prec, bool exp) {
                std::vector<char> buf(32);
                int len = 0;
                for (;;) {
                    if (exp)
                        len = snprintf(buf.data(), buf.size(), "%.*Le", prec, x);
                    else
                        len = snprintf(buf.data(), buf.size(), "%.*Lf", prec, x);
                    if (len < int(buf.size()))
                        return buf.data();
                    buf.resize(2 * buf.size());
                }
            }

            void float_strip(Ustring& str) {
                static const Regex pattern("(.*)(\\.(?:\\d*[1-9])?)(0+)(e.*)?", Regex::full);
                auto match = pattern(str);
                if (match) {
                    Ustring result(match[1]);
                    if (match.count(2) != 1)
                        result += match[2];
                    result += match[4];
                    str.swap(result);
                }
            }

            Ustring float_digits(long double x, int prec) {
                prec = std::max(prec, 1);
                auto result = float_print(x, prec - 1, true);
                auto epos = result.find_first_of("Ee");
                auto exponent = strtol(result.data() + epos + 1, nullptr, 10);
                result.resize(epos);
                if (exponent < 0) {
                    if (prec > 1)
                        result.erase(1, 1);
                    result.insert(0, 1 - exponent, '0');
                    result[1] = '.';
                } else if (exponent >= prec - 1) {
                    if (prec > 1)
                        result.erase(1, 1);
                    result.insert(result.end(), exponent - prec + 1, '0');
                } else if (exponent > 0) {
                    result.erase(1, 1);
                    result.insert(exponent + 1, 1, '.');
                }
                return result;
            }

            Ustring float_exp(long double x, int prec) {
                prec = std::max(prec, 1);
                auto result = float_print(x, prec - 1, true);
                auto epos = result.find_first_of("Ee");
                char esign = 0;
                if (result[epos + 1] == '-')
                    esign = '-';
                auto epos2 = result.find_first_not_of("+-0", epos + 1);
                Ustring exponent;
                if (epos2 < result.size())
                    exponent = result.substr(epos2);
                result.resize(epos);
                result += 'e';
                if (esign)
                    result += esign;
                if (exponent.empty())
                    result += '0';
                else
                    result += exponent;
                return result;
            }

            Ustring float_fixed(long double x, int prec) {
                return float_print(x, prec, false);
            }

            Ustring float_general(long double x, int prec) {
                using std::floor;
                using std::log10;
                if (x == 0)
                    return float_digits(x, prec);
                auto exp = int(floor(log10(x)));
                auto d_estimate = exp < 0 ? prec + 1 - exp : exp < prec - 1 ? prec + 1 : exp + 1;
                auto e_estimate = exp < 0 ? prec + 4 : prec + 3;
                auto e_vs_d = e_estimate - d_estimate;
                if (e_vs_d <= -2)
                    return float_exp(x, prec);
                if (e_vs_d >= 2)
                    return float_digits(x, prec);
                auto dform = float_digits(x, prec);
                auto eform = float_exp(x, prec);
                return dform.size() <= eform.size() ? dform : eform;
            }

            Ustring string_escape(const Ustring& s, uint64_t mode) {
                Ustring result;
                result.reserve(s.size() + 2);
                if (mode & Format::quote)
                    result += '\"';
                for (auto i = utf_begin(s), e = utf_end(s); i != e; ++i) {
                    switch (*i) {
                        case U'\0': result += "\\0"; break;
                        case U'\t': result += "\\t"; break;
                        case U'\n': result += "\\n"; break;
                        case U'\f': result += "\\f"; break;
                        case U'\r': result += "\\r"; break;
                        case U'\\': result += "\\\\"; break;
                        case U'\"':
                            if (mode & Format::quote)
                                result += '\\';
                            result += '\"';
                            break;
                        default:
                            if (*i >= 32 && *i <= 126) {
                                result += char(*i);
                            } else if (*i >= 0xa0 && ! (mode & Format::ascii)) {
                                result.append(s, i.offset(), i.count());
                            } else if (*i <= 0xff) {
                                result += "\\x";
                                result += format_radix(*i, 16, 2);
                            } else {
                                result += "\\x{";
                                result += format_radix(*i, 16, 1);
                                result += '}';
                            }
                            break;
                    }
                }
                if (mode & Format::quote)
                    result += '\"';
                return result;
            }

            template <typename Range>
            Ustring string_values(const Range& s, int base, int prec, int defprec) {
                if (prec < 0)
                    prec = defprec;
                Ustring result;
                for (auto c: s) {
                    result += format_radix(char_to_uint(c), base, prec);
                    result += ' ';
                }
                if (! result.empty())
                    result.pop_back();
                return result;
            }

        }

        // Formatting for specific types

        void translate_flags(const Ustring& str, uint64_t& flags, int& prec, size_t& width, char32_t& pad) {
            flags = 0;
            prec = -1;
            width = 0;
            pad = U' ';
            auto i = utf_begin(str), end = utf_end(str);
            while (i != end) {
                if (*i == U'<' || *i == U'=' || *i == U'>') {
                    if (*i == U'<')
                        flags |= Format::left;
                    else if (*i == U'=')
                        flags |= Format::centre;
                    else
                        flags |= Format::right;
                    ++i;
                    if (i != end && ! char_is_digit(*i))
                        pad = *i++;
                    if (i != end && char_is_digit(*i))
                        i = str_to_int<size_t>(width, i);
                } else if (char_is_ascii(*i) && ascii_isalpha(char(*i))) {
                    flags |= letter_to_mask(char(*i++));
                } else if (char_is_digit(*i)) {
                    i = str_to_int<int>(prec, i);
                } else {
                    ++i;
                }
            }
        }

        Ustring format_ldouble(long double t, uint64_t flags, int prec) {
            using std::fabs;
            static constexpr auto format_flags = Format::digits | Format::exp | Format::fixed | Format::general;
            static constexpr auto sign_flags = Format::sign | Format::signz;
            if (popcount(flags & format_flags) > 1 || popcount(flags & sign_flags) > 1)
                throw std::invalid_argument("Inconsistent formatting flags");
            if (prec < 0)
                prec = 6;
            auto mag = fabs(t);
            Ustring s;
            if (flags & Format::digits)
                s = float_digits(mag, prec);
            else if (flags & Format::exp)
                s = float_exp(mag, prec);
            else if (flags & Format::fixed)
                s = float_fixed(mag, prec);
            else
                s = float_general(mag, prec);
            if (flags & Format::stripz)
                float_strip(s);
            if (t < 0 || (flags & Format::sign) || (t > 0 && (flags & Format::signz)))
                s.insert(s.begin(), t < 0 ? '-' : '+');
            return s;
        }

        // Alignment and padding

        Ustring format_align(Ustring src, uint64_t flags, size_t width, char32_t pad) {
            if (popcount(flags & (Format::left | Format::centre | Format::right)) > 1)
                throw std::invalid_argument("Inconsistent formatting alignment flags");
            if (popcount(flags & (Format::lower | Format::title | Format::upper)) > 1)
                throw std::invalid_argument("Inconsistent formatting case conversion flags");
            if (flags & Format::lower)
                str_lowercase_in(src);
            else if (flags & Format::title)
                str_titlecase_in(src);
            else if (flags & Format::upper)
                str_uppercase_in(src);
            size_t len = str_length(src, flags & format_length_flags);
            if (width <= len)
                return src;
            size_t extra = width - len;
            Ustring dst;
            if (flags & Format::right)
                str_append_chars(dst, extra, pad);
            else if (flags & Format::centre)
                str_append_chars(dst, extra / 2, pad);
            dst += src;
            if (flags & Format::left)
                str_append_chars(dst, extra, pad);
            else if (flags & Format::centre)
                str_append_chars(dst, (extra + 1) / 2, pad);
            return dst;
        }

    }

    // Basic formattng functions

    Ustring format_type(bool t, uint64_t flags, int /*prec*/) {
        static constexpr auto format_flags = Format::binary | Format::tf | Format::yesno;
        if (popcount(flags & format_flags) > 1)
            throw std::invalid_argument("Inconsistent formatting flags");
        if (flags & Format::binary)
            return t ? "1" : "0";
        else if (flags & Format::yesno)
            return t ? "yes" : "no";
        else
            return t ? "true" : "false";
    }

    Ustring format_type(const Ustring& t, uint64_t flags, int prec) {
        using namespace UnicornDetail;
        static constexpr auto format_flags = Format::ascii | Format::ascquote | Format::escape | Format::decimal | Format::hex | Format::hex8 | Format::hex16 | Format::quote;
        if (popcount(flags & format_flags) > 1)
            throw std::invalid_argument("Inconsistent formatting flags");
        if (flags & Format::quote)
            return string_escape(t, Format::quote);
        else if (flags & Format::ascquote)
            return string_escape(t, Format::quote | Format::ascii);
        else if (flags & Format::escape)
            return string_escape(t, 0);
        else if (flags & Format::ascii)
            return string_escape(t, Format::ascii);
        else if (flags & Format::decimal)
            return string_values(utf_range(t), 10, prec, 1);
        else if (flags & Format::hex8)
            return string_values(t, 16, prec, 2);
        else if (flags & Format::hex16)
            return string_values(to_utf16(t), 16, prec, 4);
        else if (flags & Format::hex)
            return string_values(utf_range(t), 16, prec, 1);
        else
            return t;
    }

    Ustring format_type(system_clock::time_point t, uint64_t flags, int prec) {
        static constexpr auto format_flags = Format::iso | Format::common;
        if (popcount(flags & format_flags) > 1)
            throw std::invalid_argument("Inconsistent formatting flags");
        auto zone = flags & Format::local ? local_zone : utc_zone;
        if (flags & Format::common)
            return format_date(t, "%c"s, zone);
        auto result = format_date(t, prec, zone);
        if (flags & Format::iso) {
            auto pos = result.find(' ');
            if (pos != npos)
                result[pos] = 'T';
        }
        return result;
    }

    // Formatter class

    Format::Format(const Ustring& format):
    fmt(format), seq() {
        auto i = utf_begin(format), end = utf_end(format);
        while (i != end) {
            auto j = std::find(i, end, U'$');
            add_literal(u_str(i, j));
            i = std::next(j);
            if (i == end)
                break;
            Ustring prefix, suffix;
            if (char_is_digit(*i)) {
                auto k = std::find_if_not(i, end, char_is_digit);
                j = std::find_if_not(k, end, char_is_alphanumeric);
                prefix = u_str(i, k);
                suffix = u_str(k, j);
            } else if (*i == U'{') {
                auto k = std::next(i);
                if (char_is_digit(*k)) {
                    auto l = std::find_if_not(k, end, char_is_digit);
                    j = std::find(l, end, U'}');
                    if (j != end) {
                        prefix = u_str(k, l);
                        suffix = u_str(l, j);
                        ++j;
                    }
                }
            }
            if (prefix.empty()) {
                add_literal(i.str());
                ++i;
            } else {
                add_index(str_to_int<unsigned>(prefix), suffix);
                i = j;
            }
        }
    }

    void Format::add_index(unsigned index, const Ustring& flags) {
        using namespace UnicornDetail;
        element elem;
        elem.index = index;
        translate_flags(to_utf8(flags), elem.flags, elem.prec, elem.width, elem.pad);
        seq.push_back(elem);
        if (index > 0 && index > num)
            num = index;
    }

    void Format::add_literal(const Ustring& text) {
        if (! text.empty()) {
            if (seq.empty() || seq.back().index != 0)
                seq.push_back({0, text, 0, 0, 0, 0});
            else
                seq.back().text += text;
        }
    }

}
