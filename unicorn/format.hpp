#pragma once

#include "unicorn/character.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include "unicorn/utility.hpp"
#include <algorithm>
#include <chrono>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS::Unicorn {

    // Formatter class

    // (Defined early so we can use the member constants)

    class Format {

    public:

        static constexpr uint64_t left      = setbit<52>;           // (<) Left align                  all  --    --   --     --    --      --
        static constexpr uint64_t centre    = setbit<53>;           // (=) Centre align                all  --    --   --     --    --      --
        static constexpr uint64_t right     = setbit<54>;           // (>) Right align                 all  --    --   --     --    --      --
        static constexpr uint64_t upper     = letter_to_mask('U');  // Convert to upper case           all  --    --   --     --    --      --
        static constexpr uint64_t lower     = letter_to_mask('L');  // Convert to lower case           all  --    --   --     --    --      --
        static constexpr uint64_t title     = letter_to_mask('T');  // Convert to title case           all  --    --   --     --    --      --
        static constexpr uint64_t tf        = letter_to_mask('t');  // Write as true/false             --   bool  --   --     --    --      --
        static constexpr uint64_t yesno     = letter_to_mask('y');  // Write as yes/no                 --   bool  --   --     --    --      --
        static constexpr uint64_t binary    = letter_to_mask('b');  // Binary number                   --   bool  int  --     --    --      --
        static constexpr uint64_t decimal   = letter_to_mask('n');  // Decimal number                  --   --    int  --     char  string  --
        static constexpr uint64_t hex       = letter_to_mask('x');  // Hexadecimal number              --   --    int  --     char  string  --
        static constexpr uint64_t roman     = letter_to_mask('r');  // Roman numerals                  --   --    int  --     --    --      --
        static constexpr uint64_t sign      = letter_to_mask('s');  // Always show a sign              --   --    int  float  --    --      --
        static constexpr uint64_t signz     = letter_to_mask('S');  // Always show a sign unless zero  --   --    int  float  --    --      --
        static constexpr uint64_t digits    = letter_to_mask('d');  // Fixed significant figures       --   --    --   float  --    --      --
        static constexpr uint64_t exp       = letter_to_mask('e');  // Scientific notation             --   --    --   float  --    --      --
        static constexpr uint64_t fixed     = letter_to_mask('f');  // Fixed point notation            --   --    --   float  --    --      --
        static constexpr uint64_t general   = letter_to_mask('g');  // Use the shorter of d or e       --   --    --   float  --    --      --
        static constexpr uint64_t stripz    = letter_to_mask('z');  // Strip trailing zeros            --   --    --   float  --    --      --
        static constexpr uint64_t escape    = letter_to_mask('e');  // Escape if C0/C1 control         --   --    --   --     char  string  --
        static constexpr uint64_t ascii     = letter_to_mask('a');  // Escape if not printable ASCII   --   --    --   --     char  string  --
        static constexpr uint64_t quote     = letter_to_mask('q');  // Quote string, escape C0/C1      --   --    --   --     char  string  --
        static constexpr uint64_t ascquote  = letter_to_mask('Q');  // Quote string, escape non-ASCII  --   --    --   --     char  string  --
        static constexpr uint64_t hex8      = letter_to_mask('u');  // Hex UTF-8 bytes                 --   --    --   --     char  string  --
        static constexpr uint64_t hex16     = letter_to_mask('v');  // Hex UTF-16 code units           --   --    --   --     char  string  --
        static constexpr uint64_t iso       = letter_to_mask('t');  // ISO 8601 with T delimiter       --   --    --   --     --    --      date
        static constexpr uint64_t common    = letter_to_mask('c');  // Local standard time format      --   --    --   --     --    --      date
        static constexpr uint64_t local     = letter_to_mask('l');  // Local time zone                 --   --    --   --     --    --      date

        Format() = default;
        explicit Format(const Ustring& format);
        template <typename... Args> Ustring operator()(const Args&... args) const;
        bool empty() const noexcept { return fmt.empty(); }
        size_t fields() const { return num; }
        Ustring format() const { return fmt; }

    private:

        struct element {
            int index;       // 0 = literal text, 1+ = field reference
            Ustring text;   // Literal text
            uint64_t flags;  // Field formatting flags
            int prec;        // Field precision
            size_t width;    // Field width
            char32_t pad;    // Padding character
        };
        using sequence = std::vector<element>;
        Ustring fmt;
        size_t num = 0;
        sequence seq;
        void add_index(unsigned index, const Ustring& flags = {});
        void add_literal(const Ustring& text);
        void apply_format(Strings&, int) const {}
        template <typename T, typename... Args> void apply_format(Strings& list, int index, const T& t, const Args&... args) const;

    };

    namespace UnicornDetail {

        // Reserved global flags

        constexpr uint64_t format_length_flags = Length::characters | Length::graphemes | Length::narrow | Length::wide;
        constexpr uint64_t global_format_flags = format_length_flags | Format::left | Format::centre | Format::right | Format::lower | Format::title | Format::upper;

        // Formatting for specific types

        void translate_flags(const Ustring& str, uint64_t& flags, int& prec, size_t& width, char32_t& pad);
        Ustring format_ldouble(long double t, uint64_t flags, int prec);

        template <typename T>
        Ustring format_float(T t, uint64_t flags, int prec) {
            return format_ldouble(static_cast<long double>(t), flags, prec);
        }

        template <typename T>
        Ustring format_radix(T t, int base, int prec) {
            // Argument will never be negative
            Ustring s;
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
        Ustring format_int(T t, uint64_t flags, int prec) {
            static constexpr auto float_flags = Format::digits | Format::exp | Format::fixed | Format::general | Format::stripz;
            static constexpr auto int_flags = Format::binary | Format::decimal | Format::hex | Format::roman;
            static constexpr auto sign_flags = Format::sign | Format::signz;
            if ((flags & float_flags) && ! (flags & int_flags))
                return format_float(t, flags, prec);
            if (popcount(flags & int_flags) > 1 || popcount(flags & sign_flags) > 1)
                throw std::invalid_argument("Inconsistent integer formatting flags");
            char sign = 0;
            if (t > static_cast<T>(0)) {
                if (flags & (Format::sign | Format::signz))
                    sign = '+';
            } else if (t == static_cast<T>(0)) {
                if (flags & Format::sign)
                    sign = '+';
            } else {
                t = RS_Detail::SimpleAbs<T>()(t);
                sign = '-';
            }
            Ustring s;
            if (flags & Format::binary)
                s = format_radix(t, 2, prec);
            else if (flags & Format::roman)
                s = roman(unsigned(t));
            else if (flags & Format::hex)
                s = format_radix(t, 16, prec);
            else
                s = format_radix(t, 10, prec);
            if (sign)
                s.insert(s.begin(), sign);
            return s;
        }

        // Alignment and padding

        Ustring format_align(Ustring src, uint64_t flags, size_t width, char32_t pad);

    }

    // Basic formattng functions

    template <typename T> Ustring format_type(const T& t, uint64_t flags, int prec);
    Ustring format_type(bool t, uint64_t flags, int /*prec*/);
    Ustring format_type(const Ustring& t, uint64_t flags, int prec);
    Ustring format_type(std::chrono::system_clock::time_point t, uint64_t flags, int prec);
    template <typename C> inline Ustring format_type(const std::basic_string<C>& t, uint64_t flags, int prec) { return format_type(to_utf8(t), flags, prec); }
    template <typename C> inline Ustring format_type(const std::basic_string_view<C>& t, uint64_t flags, int prec)
        { return format_type(to_utf8(std::basic_string<C>(t)), flags, prec); }
    template <typename C> inline Ustring format_type(C* t, uint64_t flags, int prec) { return format_type(cstr(t), flags, prec); }
    template <typename C> inline Ustring format_type(const C* t, uint64_t flags, int prec) { return format_type(cstr(t), flags, prec); }
    template <typename R, typename P> inline Ustring format_type(std::chrono::duration<R, P> t, uint64_t /*flags*/, int prec) { return format_time(t, prec); }

    inline Ustring format_type(char t, uint64_t flags, int prec)                { return format_type(str_char(t), flags, prec); }
    inline Ustring format_type(char16_t t, uint64_t flags, int prec)            { return format_type(str_char(t), flags, prec); }
    inline Ustring format_type(char32_t t, uint64_t flags, int prec)            { return format_type(str_char(t), flags, prec); }
    inline Ustring format_type(wchar_t t, uint64_t flags, int prec)             { return format_type(str_char(t), flags, prec); }
    inline Ustring format_type(signed char t, uint64_t flags, int prec)         { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(unsigned char t, uint64_t flags, int prec)       { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(short t, uint64_t flags, int prec)               { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(unsigned short t, uint64_t flags, int prec)      { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(int t, uint64_t flags, int prec)                 { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(unsigned t, uint64_t flags, int prec)            { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(long t, uint64_t flags, int prec)                { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(unsigned long t, uint64_t flags, int prec)       { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(long long t, uint64_t flags, int prec)           { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(unsigned long long t, uint64_t flags, int prec)  { return UnicornDetail::format_int(t, flags, prec); }
    inline Ustring format_type(float t, uint64_t flags, int prec)               { return UnicornDetail::format_float(t, flags, prec); }
    inline Ustring format_type(double t, uint64_t flags, int prec)              { return UnicornDetail::format_float(t, flags, prec); }
    inline Ustring format_type(long double t, uint64_t flags, int prec)         { return UnicornDetail::format_float(t, flags, prec); }

    namespace UnicornDetail {

        template <typename R, typename I = decltype(std::begin(std::declval<R>())), typename V = typename std::iterator_traits<I>::value_type>
        struct FormatRange {
            Ustring operator()(const R& r, uint64_t flags, int prec) const {
                Ustring s;
                for (auto& x: r)
                    s += format_type(x, flags, prec) + ',';
                if (! s.empty())
                    s.pop_back();
                return s;
            }
        };

        template <typename T, bool = Meta::is_range<T>>
        struct FormatObject {
            Ustring operator()(T t, uint64_t /*flags*/, int /*prec*/) const {
                std::ostringstream out;
                out << t;
                return sanitize(out.str());
            }
        };

        template <typename T> struct FormatObject<T, true>: FormatRange<T> {};

        template <typename T1, typename T2>
        struct FormatObject<std::pair<T1, T2>, false> {
            Ustring operator()(const std::pair<T1, T2>& p, uint64_t flags, int prec) const {
                return format_type(p.first, flags, prec) + ':' + format_type(p.second, flags, prec);
            }
        };

    }

    template <typename T>
    inline Ustring format_type(const T& t, uint64_t flags, int prec) {
        return UnicornDetail::FormatObject<std::decay_t<T>>()(t, flags, prec);
    }

    template <typename T>
    Ustring format_str(const T& t, uint64_t flags = 0, int prec = -1, size_t width = 0, char32_t pad = U' ') {
        using namespace UnicornDetail;
        auto s = format_type(t, flags, prec);
        return format_align(s, flags & global_format_flags, width, pad);
    }

    template <typename T>
    Ustring format_str(const T& t, const Ustring& flags) {
        using namespace UnicornDetail;
        uint64_t f = 0;
        int prec = 0;
        size_t width = 0;
        char32_t pad = U' ';
        translate_flags(flags, f, prec, width, pad);
        return format_str(t, f, prec, width, pad);
    }

    template <typename T>
    Ustring format_str(const T& t, const char* flags) {
        return format_str(t, cstr(flags));
    }

    // Formatter class functions

    template <typename... Args>
    Ustring Format::operator()(const Args&... args) const {
        Strings list(seq.size());
        for (size_t i = 0; i < seq.size(); ++i)
            if (seq[i].index == 0)
                list[i] = seq[i].text;
        apply_format(list, 1, args...);
        return str_join(list, Ustring());
    }

    template <typename T, typename... Args>
    void Format::apply_format(Strings& list, int index, const T& t, const Args&... args) const {
        using namespace UnicornDetail;
        for (size_t i = 0; i < seq.size(); ++i) {
            if (seq[i].index == index) {
                auto s = format_type(t, seq[i].flags, seq[i].prec);
                list[i] = format_align(s, seq[i].flags & global_format_flags, seq[i].width, seq[i].pad);
            }
        }
        apply_format(list, index + 1, args...);
    }

    // Formatter literals

    namespace Literals {

        inline auto operator"" _fmt(const char* ptr, size_t len) { return Format(cstr(ptr, len)); }

    }

}
