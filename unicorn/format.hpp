#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <chrono>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS {

    namespace Unicorn {

        // Formatting behaviour flags

        constexpr uint64_t fx_left      = 1ull << 52;           // (<) Left align                  all  --    --   --     --    --      --
        constexpr uint64_t fx_centre    = 1ull << 53;           // (=) Centre align                all  --    --   --     --    --      --
        constexpr uint64_t fx_right     = 1ull << 54;           // (>) Right align                 all  --    --   --     --    --      --
        constexpr uint64_t fx_upper     = letter_to_mask('U');  // Convert to upper case           all  --    --   --     --    --      --
        constexpr uint64_t fx_lower     = letter_to_mask('L');  // Convert to lower case           all  --    --   --     --    --      --
        constexpr uint64_t fx_title     = letter_to_mask('T');  // Convert to title case           all  --    --   --     --    --      --
        constexpr uint64_t fx_tf        = letter_to_mask('t');  // Write as true/false             --   bool  --   --     --    --      --
        constexpr uint64_t fx_yesno     = letter_to_mask('y');  // Write as yes/no                 --   bool  --   --     --    --      --
        constexpr uint64_t fx_binary    = letter_to_mask('b');  // Binary number                   --   bool  int  --     --    --      --
        constexpr uint64_t fx_decimal   = letter_to_mask('n');  // Decimal number                  --   --    int  --     char  string  --
        constexpr uint64_t fx_hex       = letter_to_mask('x');  // Hexadecimal number              --   --    int  --     char  string  --
        constexpr uint64_t fx_roman     = letter_to_mask('r');  // Roman numerals                  --   --    int  --     --    --      --
        constexpr uint64_t fx_sign      = letter_to_mask('s');  // Always show a sign              --   --    int  float  --    --      --
        constexpr uint64_t fx_signz     = letter_to_mask('S');  // Always show a sign unless zero  --   --    int  float  --    --      --
        constexpr uint64_t fx_digits    = letter_to_mask('d');  // Fixed significant figures       --   --    --   float  --    --      --
        constexpr uint64_t fx_exp       = letter_to_mask('e');  // Scientific notation             --   --    --   float  --    --      --
        constexpr uint64_t fx_fixed     = letter_to_mask('f');  // Fixed point notation            --   --    --   float  --    --      --
        constexpr uint64_t fx_general   = letter_to_mask('g');  // Use the shorter of d or e       --   --    --   float  --    --      --
        constexpr uint64_t fx_stripz    = letter_to_mask('z');  // Strip trailing zeros            --   --    --   float  --    --      --
        constexpr uint64_t fx_escape    = letter_to_mask('e');  // Escape if C0/C1 control         --   --    --   --     char  string  --
        constexpr uint64_t fx_ascii     = letter_to_mask('a');  // Escape if not printable ASCII   --   --    --   --     char  string  --
        constexpr uint64_t fx_quote     = letter_to_mask('q');  // Quote string, escape C0/C1      --   --    --   --     char  string  --
        constexpr uint64_t fx_ascquote  = letter_to_mask('Q');  // Quote string, escape non-ASCII  --   --    --   --     char  string  --
        constexpr uint64_t fx_hex8      = letter_to_mask('u');  // Hex UTF-8 bytes                 --   --    --   --     char  string  --
        constexpr uint64_t fx_hex16     = letter_to_mask('v');  // Hex UTF-16 code units           --   --    --   --     char  string  --
        constexpr uint64_t fx_iso       = letter_to_mask('t');  // ISO 8601 with T delimiter       --   --    --   --     --    --      date
        constexpr uint64_t fx_common    = letter_to_mask('c');  // Local standard format           --   --    --   --     --    --      date
        constexpr uint64_t fx_local     = letter_to_mask('l');  // Local time zone                 --   --    --   --     --    --      date

        namespace UnicornDetail {

            // Reserved global flags

            constexpr uint64_t fx_length_flags = Length::characters | Length::graphemes | Length::narrow | Length::wide;
            constexpr uint64_t fx_toplevel_flags = fx_length_flags | fx_left | fx_centre | fx_right | fx_lower | fx_title | fx_upper;
            constexpr uint64_t fx_global_flags = fx_length_flags | fx_toplevel_flags;

            // Formatting for specific types

            void translate_flags(const U8string& str, uint64_t& flags, int& prec, size_t& width, char32_t& pad);
            U8string format_float(long double t, uint64_t flags, int prec);
            U8string format_roman(uint32_t n);

            template <typename T>
            U8string format_radix(T t, int base, int prec) {
                // Argument will never be negative
                U8string s;
                auto b = static_cast<T>(base);
                prec = std::max(prec, 1);
                while (t > 0 || int(s.size()) < prec) {
                    auto d = t % b;
                    s += char(d + (d <= 9 ? '0' : 'a' - 10));
                    t /= b;
                }
                std::reverse(s.begin(), s.end());
                return s;
            }

            template <typename T>
            U8string format_int(T t, uint64_t flags, int prec) {
                static constexpr auto float_flags = fx_digits | fx_exp | fx_fixed | fx_general | fx_stripz;
                static constexpr auto int_flags = fx_binary | fx_decimal | fx_hex | fx_roman;
                static constexpr auto sign_flags = fx_sign | fx_signz;
                if ((flags & float_flags) && ! (flags & int_flags))
                    return format_float(t, flags, prec);
                if (ibits(flags & int_flags) > 1 || ibits(flags & sign_flags) > 1)
                    throw std::invalid_argument("Inconsistent integer formatting flags");
                char sign = 0;
                if (t > static_cast<T>(0)) {
                    if (flags & (fx_sign | fx_signz))
                        sign = '+';
                } else if (t == static_cast<T>(0)) {
                    if (flags & fx_sign)
                        sign = '+';
                } else {
                    t = - t;
                    sign = '-';
                }
                U8string s;
                if (flags & fx_binary)
                    s = format_radix(t, 2, prec);
                else if (flags & fx_roman)
                    s = format_roman(uint32_t(t));
                else if (flags & fx_hex)
                    s = format_radix(t, 16, prec);
                else
                    s = format_radix(t, 10, prec);
                if (sign)
                    s.insert(s.begin(), sign);
                return s;
            }

            // Alignment and padding

            U8string format_align(U8string src, uint64_t flags, size_t width, char32_t pad);

        }

        // Basic formattng functions

        template <typename T> U8string format_type(const T& t, uint64_t flags, int prec);

        U8string format_type(bool t, uint64_t flags, int /*prec*/);
        U8string format_type(const U8string& t, uint64_t flags, int prec);
        U8string format_type(std::chrono::system_clock::time_point t, uint64_t flags, int prec);

        template <typename C> inline U8string format_type(const std::basic_string<C>& t, uint64_t flags, int prec)
            { return format_type(to_utf8(t), flags, prec); }
        template <typename C> inline U8string format_type(C* t, uint64_t flags, int prec)
            { return format_type(cstr(t), flags, prec); }
        template <typename C> inline U8string format_type(const C* t, uint64_t flags, int prec)
            { return format_type(cstr(t), flags, prec); }
        template <typename R, typename P> inline U8string format_type(std::chrono::duration<R, P> t, uint64_t /*flags*/, int prec)
            { return format_time(t, prec); }

        inline U8string format_type(char t, uint64_t flags, int prec)                { return format_type(str_char(t), flags, prec); }
        inline U8string format_type(char16_t t, uint64_t flags, int prec)            { return format_type(str_char(t), flags, prec); }
        inline U8string format_type(char32_t t, uint64_t flags, int prec)            { return format_type(str_char(t), flags, prec); }
        inline U8string format_type(wchar_t t, uint64_t flags, int prec)             { return format_type(str_char(t), flags, prec); }
        inline U8string format_type(signed char t, uint64_t flags, int prec)         { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(unsigned char t, uint64_t flags, int prec)       { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(short t, uint64_t flags, int prec)               { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(unsigned short t, uint64_t flags, int prec)      { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(int t, uint64_t flags, int prec)                 { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(unsigned t, uint64_t flags, int prec)            { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(long t, uint64_t flags, int prec)                { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(unsigned long t, uint64_t flags, int prec)       { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(long long t, uint64_t flags, int prec)           { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(unsigned long long t, uint64_t flags, int prec)  { return UnicornDetail::format_int(t, flags, prec); }
        inline U8string format_type(float t, uint64_t flags, int prec)               { return UnicornDetail::format_float(t, flags, prec); }
        inline U8string format_type(double t, uint64_t flags, int prec)              { return UnicornDetail::format_float(t, flags, prec); }
        inline U8string format_type(long double t, uint64_t flags, int prec)         { return UnicornDetail::format_float(t, flags, prec); }
        inline U8string format_type(Uuid t, uint64_t /*flags*/, int /*prec*/)        { return t.str(); }
        inline U8string format_type(Version t, uint64_t /*flags*/, int prec)         { return prec >= 0 ? t.str(prec) : t.str(); }

        namespace UnicornDetail {

            template <typename R, typename I = decltype(std::begin(std::declval<R>())),
                typename V = typename std::iterator_traits<I>::value_type>
            struct FormatRange {
                U8string operator()(const R& r, uint64_t flags, int prec) const {
                    U8string s;
                    for (auto& x: r)
                        s += format_type(x, flags, prec) + ',';
                    if (! s.empty())
                        s.pop_back();
                    return s;
                }
            };

            template <typename T, bool = RS_Detail::IsRangeType<T>::value>
            struct FormatObject {
                U8string operator()(T t, uint64_t /*flags*/, int /*prec*/) const {
                    std::ostringstream out;
                    out << t;
                    return sanitize(out.str());
                }
            };

            template <typename T> struct FormatObject<T, true>: FormatRange<T> {};

            template <typename T1, typename T2>
            struct FormatObject<std::pair<T1, T2>, false> {
                U8string operator()(const std::pair<T1, T2>& p, uint64_t flags, int prec) const {
                    return format_type(p.first, flags, prec) + ':' + format_type(p.second, flags, prec);
                }
            };

        }

        template <typename T>
        inline U8string format_type(const T& t, uint64_t flags, int prec) {
            return UnicornDetail::FormatObject<std::decay_t<T>>()(t, flags, prec);
        }

        template <typename T>
        U8string format_str(const T& t, uint64_t flags = 0, int prec = -1, size_t width = 0, char32_t pad = U' ') {
            using namespace UnicornDetail;
            auto s = format_type(t, flags, prec);
            return format_align(s, flags & fx_global_flags, width, pad);
        }

        template <typename T>
        U8string format_str(const T& t, const U8string& flags) {
            using namespace UnicornDetail;
            uint64_t f = 0;
            int prec = 0;
            size_t width = 0;
            char32_t pad = U' ';
            translate_flags(flags, f, prec, width, pad);
            return format_str(t, f, prec, width, pad);
        }

        template <typename T>
        U8string format_str(const T& t, const char* flags) {
            return format_str(t, cstr(flags));
        }

        // Formatter class

        class Format {
        public:
            Format() = default;
            explicit Format(const U8string& format);
            template <typename... Args> U8string operator()(const Args&... args) const;
            bool empty() const noexcept { return fmt.empty(); }
            size_t fields() const { return num; }
            U8string format() const { return fmt; }
        private:
            struct element {
                int index;       // 0 = literal text, 1+ = field reference
                U8string text;   // Literal text
                uint64_t flags;  // Field formatting flags
                int prec;        // Field precision
                size_t width;    // Field width
                char32_t pad;    // Padding character
            };
            using sequence = std::vector<element>;
            using string_list = std::vector<U8string>;
            U8string fmt;
            size_t num = 0;
            sequence seq;
            void add_index(unsigned index, const U8string& flags = {});
            void add_literal(const U8string& text);
            void apply_format(string_list&, int) const {}
            template <typename T, typename... Args> void apply_format(string_list& list, int index, const T& t, const Args&... args) const;
        };

        template <typename... Args>
        U8string Format::operator()(const Args&... args) const {
            string_list list(seq.size());
            for (size_t i = 0; i < seq.size(); ++i)
                if (seq[i].index == 0)
                    list[i] = seq[i].text;
            apply_format(list, 1, args...);
            return str_join(list, U8string());
        }

        template <typename T, typename... Args>
        void Format::apply_format(string_list& list, int index, const T& t, const Args&... args) const {
            using namespace UnicornDetail;
            for (size_t i = 0; i < seq.size(); ++i) {
                if (seq[i].index == index) {
                    auto s = format_type(t, seq[i].flags, seq[i].prec);
                    list[i] = format_align(s, seq[i].flags & fx_global_flags, seq[i].width, seq[i].pad);
                }
            }
            apply_format(list, index + 1, args...);
        }

        // Formatter literals

        namespace Literals {

            inline auto operator"" _fmt(const char* ptr, size_t len) { return Format(cstr(ptr, len)); }

        }

    }

}
