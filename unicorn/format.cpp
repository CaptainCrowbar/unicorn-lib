#include "unicorn/format.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>

using namespace std::chrono;
using namespace std::literals;
using namespace Unicorn::Literals;

namespace Unicorn {

    namespace {

        constexpr bool char_is_digit(char32_t c) noexcept { return c >= '0' && c <= '9'; }

    }

    namespace UnicornDetail {

        // Alignment and padding

        void translate_flags(const u8string& str, Flagset& flags, int& prec, size_t& width, char32_t& pad) {
            prec = -1;
            width = 0;
            pad = U' ';
            u8string flagstr;
            auto i = utf_begin(str), e = utf_end(str);
            while (i != e) {
                if (*i == U'<' || *i == U'=' || *i == U'>') {
                    flagstr += static_cast<char>(*i++);
                    if (i != e && ! char_is_digit(*i))
                        pad = *i++;
                    if (i != e && char_is_digit(*i)) {
                        width = str_to_integer<size_t>(str, i.offset());
                        i = std::find_if_not(i, e, char_is_digit);
                    }
                } else if (char_is_digit(*i)) {
                    prec = str_to_integer<int>(str, i.offset());
                    i = std::find_if_not(i, e, char_is_digit);
                } else {
                    if (char_is_ascii(*i))
                        flagstr += static_cast<char>(*i);
                    ++i;
                }
            }
            flags = Flagset(flagstr);
        }

        namespace {

            // Integer formatting helper functions

            template <typename C>
            std::basic_string<C> int_radix(unsigned long long n, int base, int prec) {
                using string_type = std::basic_string<C>;
                if (prec < 1)
                    prec = 1;
                string_type s;
                auto b = static_cast<unsigned long long>(base);
                while (n > 0 || s.size() < size_t(prec)) {
                    auto d = n % b;
                    s += static_cast<C>(d + (d <= 9 ? '0' : 'a' - 10));
                    n /= b;
                }
                std::reverse(CROW_BOUNDS(s));
                return s;
            }

            u8string int_roman(unsigned long long n) {
                struct entry { const char* str; unsigned num; };
                static const entry table[] {
                    { "M", 1000 },
                    { "CM", 900 }, { "D", 500 }, { "CD", 400 }, { "C", 100 },
                    { "XC", 90 }, { "L", 50 }, { "XL", 40 }, { "X", 10 },
                    { "IX", 9 }, { "V", 5 }, { "IV", 4 }, { "I", 1 },
                };
                if (n == 0)
                    return "0";
                u8string s;
                for (auto& t: table) {
                    auto q = n / t.num;
                    n %= t.num;
                    for (unsigned long long i = 0; i < q; ++i)
                        s += t.str;
                }
                return s;
            }

            // Floating point formatting helper functions
            // (These will always be called with x>=0 and prec>=0)

            u8string float_print(const char* format, long double x, int prec) {
                std::vector<char> buf(32);
                for (;;) {
                    if (snprintf(buf.data(), buf.size(), format, prec, x) < static_cast<int>(buf.size()))
                        return buf.data();
                    buf.resize(2 * buf.size());
                }
            }

            void float_strip(u8string& str) {
                static const auto pattern = regex("(.*)(\\.(?:\\d*[1-9])?)(0+)(e.*)?");
                auto match = pattern.match(str);
                if (match) {
                    u8string result = match[1];
                    if (match.count(2) != 1)
                        result += match[2];
                    result += match[4];
                    str.swap(result);
                }
            }

            u8string float_digits(long double x, int prec) {
                prec = std::max(prec, 1);
                auto result = float_print("%.*Le", x, prec - 1);
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
                    result.insert(std::end(result), exponent - prec + 1, '0');
                } else if (exponent > 0) {
                    result.erase(1, 1);
                    result.insert(exponent + 1, 1, '.');
                }
                return result;
            }

            u8string float_exp(long double x, int prec) {
                prec = std::max(prec, 1);
                auto result = float_print("%.*Le", x, prec - 1);
                auto epos = result.find_first_of("Ee");
                char esign = 0;
                if (result[epos + 1] == '-')
                    esign = '-';
                auto epos2 = result.find_first_not_of("+-0", epos + 1);
                u8string exponent;
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

            u8string float_fixed(long double x, int prec) {
                return float_print("%.*Lf", x, prec);
            }

            u8string float_general(long double x, int prec) {
                using std::floor;
                using std::log10;
                if (x == 0)
                    return float_digits(x, prec);
                auto exp = static_cast<int>(floor(log10(x)));
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

            u8string float_prob(long double x, int prec) {
                if (x <= 0)
                    return "0";
                if (x >= 1)
                    return "1";
                if (x > 0.8) {
                    prec = std::max(prec, 1);
                    auto complement = float_print("%.*Le", 1 - x, prec - 1);
                    auto epos = complement.find_first_of("Ee");
                    auto exponent = strtol(complement.data() + epos + 1, nullptr, 10);
                    prec -= exponent + 1;
                }
                return float_digits(x, prec);
            }

            // String and character formatting helper functions

            u8string string_escape(const u8string& s, Flagset mode) {
                u8string result;
                result.reserve(s.size() + 2);
                if (mode.get(fx_quote))
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
                            if (mode.get(fx_quote))
                                result += '\\';
                            result += '\"';
                            break;
                        default:
                            if (*i >= 32 && *i <= 126) {
                                result += static_cast<char>(*i);
                            } else if (*i >= 0xa0 && ! mode.get(fx_ascii)) {
                                result.append(s, i.offset(), i.count());
                            } else if (*i <= 0xff) {
                                result += "\\x";
                                result += int_radix<char>(*i, 16, 2);
                            } else {
                                result += "\\x{";
                                result += int_radix<char>(*i, 16, 1);
                                result += '}';
                            }
                            break;
                    }
                }
                if (mode.get(fx_quote))
                    result += '\"';
                return result;
            }

            template <typename Range>
            u8string string_values(const Range& s, int base, int prec, int defprec) {
                if (prec < 0)
                    prec = defprec;
                u8string result;
                for (auto c: s) {
                    result += int_radix<char>(as_uchar(c), base, prec);
                    result += ' ';
                }
                if (! result.empty())
                    result.pop_back();
                return result;
            }

        }

        // Formatting for specific types

        u8string format_boolean(bool t, Flagset flags) {
            flags.allow(fx_global_flags | fx_binary | fx_tf | fx_yesno, "boolean formatting");
            flags.exclusive(fx_binary | fx_tf | fx_yesno, "boolean formatting");
            if (flags.get(fx_binary))
                return t ? "1" : "0";
            else if (flags.get(fx_yesno))
                return t ? "yes" : "no";
            else
                return t ? "true" : "false";
        }

        u8string format_integer(unsigned long long t, Flagset flags, int prec) {
            flags.allow(fx_global_flags | fx_binary | fx_decimal | fx_hex | fx_roman | fx_sign | fx_signz,
                "integer formatting");
            flags.exclusive(fx_binary | fx_decimal | fx_hex | fx_roman, "integer formatting");
            flags.exclusive(fx_sign | fx_signz, "integer formatting");
            if (flags.get(fx_binary))
                return int_radix<char>(t, 2, prec);
            else if (flags.get(fx_roman))
                return int_roman(t);
            else if (flags.get(fx_hex))
                return int_radix<char>(t, 16, prec);
            else
                return int_radix<char>(t, 10, prec);
        }

        u8string format_floating(long double t, Flagset flags, int prec) {
            using std::fabs;
            flags.allow(fx_global_flags | fx_digits | fx_exp | fx_fixed | fx_general | fx_prob
                | fx_sign | fx_signz | fx_stripz, "floating point formatting");
            flags.exclusive(fx_digits | fx_exp | fx_fixed | fx_general | fx_prob,
                "floating point formatting");
            flags.exclusive(fx_sign | fx_signz, "floating point formatting");
            if (prec < 0)
                prec = 6;
            if (flags.get(fx_prob))
                t = t < 0 ? 0 : t > 1 ? 1 : t;
            auto mag = fabs(t);
            u8string s;
            if (flags.get(fx_digits))
                s = float_digits(mag, prec);
            else if (flags.get(fx_exp))
                s = float_exp(mag, prec);
            else if (flags.get(fx_fixed))
                s = float_fixed(mag, prec);
            else if (flags.get(fx_prob))
                s = float_prob(mag, prec);
            else
                s = float_general(mag, prec);
            if (flags.get(fx_stripz))
                float_strip(s);
            if (t < 0 || flags.get(fx_sign) || (t > 0 && flags.get(fx_signz)))
                s.insert(std::begin(s), t < 0 ? '-' : '+');
            return s;
        }

        u8string format_string(const u8string& t, Flagset flags, int prec) {
            flags.allow(fx_global_flags | fx_ascii | fx_ascquote | fx_decimal | fx_escape | fx_hex
                | fx_hex8 | fx_hex16 | fx_quote, "string formatting");
            flags.exclusive(fx_ascii | fx_ascquote  | fx_escape | fx_decimal | fx_hex
                | fx_hex8 | fx_hex16 | fx_quote, "string formatting");
            if (flags.get(fx_quote))
                return string_escape(t, fx_quote);
            else if (flags.get(fx_ascquote))
                return string_escape(t, fx_quote | fx_ascii);
            else if (flags.get(fx_escape))
                return string_escape(t, 0);
            else if (flags.get(fx_ascii))
                return string_escape(t, fx_ascii);
            else if (flags.get(fx_decimal))
                return string_values(utf_range(t), 10, prec, 1);
            else if (flags.get(fx_hex8))
                return string_values(t, 16, prec, 2);
            else if (flags.get(fx_hex16))
                return string_values(to_utf16(t), 16, prec, 4);
            else if (flags.get(fx_hex))
                return string_values(utf_range(t), 16, prec, 1);
            else
                return t;
        }

        u8string format_timepoint(system_clock::time_point t, Flagset flags, int prec) {
            flags.allow(fx_global_flags | fx_iso | fx_common | fx_local, "date formatting");
            flags.exclusive(fx_iso | fx_common, "date formatting");
            auto zone = flags.get(fx_local) ? local_date : utc_date;
            if (flags.get(fx_common))
                return format_date(t, "%c"s, zone);
            auto result = format_date(t, prec, zone);
            if (flags.get(fx_iso)) {
                auto pos = result.find(' ');
                if (pos != npos)
                    result[pos] = 'T';
            }
            return result;
        }

    }

}
