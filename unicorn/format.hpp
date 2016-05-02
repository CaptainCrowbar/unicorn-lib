#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include <chrono>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

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
    constexpr uint64_t fx_ascii     = letter_to_mask('a');  // Escape if not printable ASCII   --   --    --   --     char  string  --
    constexpr uint64_t fx_escape    = letter_to_mask('c');  // Escape if C0/C1 control         --   --    --   --     char  string  --
    constexpr uint64_t fx_quote     = letter_to_mask('q');  // Quote string, escape C0/C1      --   --    --   --     char  string  --
    constexpr uint64_t fx_ascquote  = letter_to_mask('o');  // Quote string, escape non-ASCII  --   --    --   --     char  string  --
    constexpr uint64_t fx_hex8      = letter_to_mask('u');  // Hex UTF-8 bytes                 --   --    --   --     char  string  --
    constexpr uint64_t fx_hex16     = letter_to_mask('v');  // Hex UTF-16 code units           --   --    --   --     char  string  --
    constexpr uint64_t fx_iso       = letter_to_mask('t');  // ISO 8601 with T delimiter       --   --    --   --     --    --      date
    constexpr uint64_t fx_common    = letter_to_mask('c');  // Local standard format           --   --    --   --     --    --      date
    constexpr uint64_t fx_local     = letter_to_mask('l');  // Local time zone                 --   --    --   --     --    --      date

    namespace UnicornDetail {

        // Reserved global flags

        constexpr uint64_t fx_length_flags = character_units | grapheme_units | narrow_context | wide_context;
        constexpr uint64_t fx_toplevel_flags = fx_length_flags | fx_left | fx_centre | fx_right | fx_lower | fx_title | fx_upper;
        constexpr uint64_t fx_global_flags = fx_length_flags | fx_toplevel_flags;

        // Formatting for specific types

        void translate_flags(const u8string& str, uint64_t& flags, int& prec, size_t& width, char32_t& pad);
        u8string format_float(long double t, uint64_t flags, int prec);
        u8string format_roman(uint32_t n);

        template <typename T>
        u8string format_radix(T t, int base, int prec) {
            // Argument will never be negative
            u8string s;
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
        u8string format_int(T t, uint64_t flags, int prec) {
            static constexpr auto float_flags = fx_digits | fx_exp | fx_fixed | fx_general | fx_stripz;
            static constexpr auto int_flags = fx_binary | fx_decimal | fx_hex | fx_roman;
            static constexpr auto sign_flags = fx_sign | fx_signz;
            if ((flags & float_flags) && ! (flags & int_flags))
                return format_float(t, flags, prec);
            if (bits_set(flags & int_flags) > 1 || bits_set(flags & sign_flags) > 1)
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
            u8string s;
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

        template <typename C>
        basic_string<C> format_align(basic_string<C> src, uint64_t flags, size_t width, char32_t pad) {
            if (bits_set(flags & (fx_left | fx_centre | fx_right)) > 1)
                throw std::invalid_argument("Inconsistent formatting alignment flags");
            if (bits_set(flags & (fx_lower | fx_title | fx_upper)) > 1)
                throw std::invalid_argument("Inconsistent formatting case conversion flags");
            if (flags & fx_lower)
                str_lowercase_in(src);
            else if (flags & fx_title)
                str_titlecase_in(src);
            else if (flags & fx_upper)
                str_uppercase_in(src);
            size_t len = str_length(src, flags & fx_length_flags);
            if (width <= len)
                return src;
            size_t extra = width - len;
            basic_string<C> dst;
            if (flags & fx_right)
                str_append_chars(dst, extra, pad);
            else if (flags & fx_centre)
                str_append_chars(dst, extra / 2, pad);
            dst += src;
            if (flags & fx_left)
                str_append_chars(dst, extra, pad);
            else if (flags & fx_centre)
                str_append_chars(dst, (extra + 1) / 2, pad);
            return dst;
        }

    }

    // Basic formattng functions

    template <typename T> u8string format_type(const T& t, uint64_t flags, int prec);

    u8string format_type(bool t, uint64_t flags, int /*prec*/);
    u8string format_type(const u8string& t, uint64_t flags, int prec);
    u8string format_type(std::chrono::system_clock::time_point t, uint64_t flags, int prec);

    template <typename C> inline u8string format_type(const basic_string<C>& t, uint64_t flags, int prec)
        { return format_type(to_utf8(t), flags, prec); }
    template <typename C> inline u8string format_type(C* t, uint64_t flags, int prec)
        { return format_type(cstr(t), flags, prec); }
    template <typename C> inline u8string format_type(const C* t, uint64_t flags, int prec)
        { return format_type(cstr(t), flags, prec); }
    template <typename R, typename P> inline u8string format_type(std::chrono::duration<R, P> t, uint64_t /*flags*/, int prec)
        { return format_time(t, prec); }

    inline u8string format_type(char t, uint64_t flags, int prec)                { return format_type(str_char<char>(t), flags, prec); }
    inline u8string format_type(char16_t t, uint64_t flags, int prec)            { return format_type(str_char<char>(t), flags, prec); }
    inline u8string format_type(char32_t t, uint64_t flags, int prec)            { return format_type(str_char<char>(t), flags, prec); }
    inline u8string format_type(wchar_t t, uint64_t flags, int prec)             { return format_type(str_char<char>(t), flags, prec); }
    inline u8string format_type(signed char t, uint64_t flags, int prec)         { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(unsigned char t, uint64_t flags, int prec)       { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(short t, uint64_t flags, int prec)               { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(unsigned short t, uint64_t flags, int prec)      { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(int t, uint64_t flags, int prec)                 { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(unsigned t, uint64_t flags, int prec)            { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(long t, uint64_t flags, int prec)                { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(unsigned long t, uint64_t flags, int prec)       { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(long long t, uint64_t flags, int prec)           { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(unsigned long long t, uint64_t flags, int prec)  { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(int128_t t, uint64_t flags, int prec)            { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(uint128_t t, uint64_t flags, int prec)           { return UnicornDetail::format_int(t, flags, prec); }
    inline u8string format_type(float t, uint64_t flags, int prec)               { return UnicornDetail::format_float(t, flags, prec); }
    inline u8string format_type(double t, uint64_t flags, int prec)              { return UnicornDetail::format_float(t, flags, prec); }
    inline u8string format_type(long double t, uint64_t flags, int prec)         { return UnicornDetail::format_float(t, flags, prec); }
    inline u8string format_type(Uuid t, uint64_t /*flags*/, int /*prec*/)        { return t.str(); }
    inline u8string format_type(Version t, uint64_t /*flags*/, int prec)         { return prec >= 0 ? t.str(prec) : t.str(); }

    namespace UnicornDetail {

        template <typename R, typename I = decltype(std::begin(std::declval<R>())),
            typename V = typename std::iterator_traits<I>::value_type>
        struct FormatRange {
            u8string operator()(const R& r, uint64_t flags, int prec) const {
                u8string s;
                for (auto& x: r)
                    s += format_type(x, flags, prec) + ',';
                if (! s.empty())
                    s.pop_back();
                return s;
            }
        };

        template <typename T, bool = PrionDetail::IsRangeType<T>::value>
        struct FormatObject {
            u8string operator()(T t, uint64_t /*flags*/, int /*prec*/) const {
                std::ostringstream out;
                out << t;
                return sanitize(out.str());
            }
        };

        template <typename T> struct FormatObject<T, true>: FormatRange<T> {};

        template <typename T1, typename T2>
        struct FormatObject<pair<T1, T2>, false> {
            u8string operator()(const pair<T1, T2>& p, uint64_t flags, int prec) const {
                return format_type(p.first, flags, prec) + ':' + format_type(p.second, flags, prec);
            }
        };

    }

    template <typename T>
    inline u8string format_type(const T& t, uint64_t flags, int prec) {
        return UnicornDetail::FormatObject<std::decay_t<T>>()(t, flags, prec);
    }

    template <typename C, typename T>
    basic_string<C> format_as(const T& t, uint64_t flags = 0, int prec = -1, size_t width = 0, char32_t pad = U' ') {
        using namespace UnicornDetail;
        auto s = format_type(t, flags, prec);
        basic_string<C> dst;
        return format_align(recode<C>(s), flags & fx_global_flags, width, pad);
    }

    template <typename C, typename T>
    basic_string<C> format_as(const T& t, const basic_string<C>& flags) {
        using namespace UnicornDetail;
        uint64_t f = 0;
        int prec = 0;
        size_t width = 0;
        char32_t pad = U' ';
        translate_flags(to_utf8(flags), f, prec, width, pad);
        return format_as<C, T>(t, f, prec, width, pad);
    }

    template <typename C, typename T>
    basic_string<C> format_as(const T& t, const C* flags) {
        return format_as<C, T>(t, cstr(flags));
    }

    // Formatter class

    template <typename C>
    class BasicFormat {
    public:
        using char_type = C;
        using string_type = basic_string<C>;
        BasicFormat() = default;
        explicit BasicFormat(const string_type& format);
        template <typename... Args> string_type operator()(const Args&... args) const;
        bool empty() const noexcept { return fmt.empty(); }
        size_t fields() const { return num; }
        string_type format() const { return fmt; }
    private:
        struct element {
            int index;         // 0 = literal text, 1+ = field reference
            string_type text;  // Literal text
            uint64_t flags;    // Field formatting flags
            int prec;          // Field precision
            size_t width;      // Field width
            char32_t pad;      // Padding character
        };
        using sequence = vector<element>;
        using string_list = vector<string_type>;
        string_type fmt {};
        size_t num = 0;
        sequence seq {};
        void add_index(unsigned index, const string_type& flags = {});
        void add_literal(const string_type& text);
        void apply(string_list&, int) const {}
        template <typename T, typename... Args> void apply(string_list& list, int index, const T& t, const Args&... args) const;
    };

    template <typename C>
    BasicFormat<C>::BasicFormat(const string_type& format):
    fmt(format), seq() {
        auto i = utf_begin(format), end = utf_end(format);
        while (i != end) {
            auto j = std::find(i, end, U'$');
            add_literal(u_str(i, j));
            i = std::next(j);
            if (i == end)
                break;
            string_type prefix, suffix;
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

    template <typename C>
    template <typename... Args>
    basic_string<C> BasicFormat<C>::operator()(const Args&... args) const {
        string_list list(seq.size());
        for (size_t i = 0; i < seq.size(); ++i)
            if (seq[i].index == 0)
                list[i] = seq[i].text;
        apply(list, 1, args...);
        return str_join(list, string_type());
    }

    template <typename C>
    void BasicFormat<C>::add_index(unsigned index, const string_type& flags) {
        element elem;
        elem.index = index;
        UnicornDetail::translate_flags(to_utf8(flags), elem.flags, elem.prec, elem.width, elem.pad);
        seq.push_back(elem);
        if (index > 0 && index > num)
            num = index;
    }

    template <typename C>
    void BasicFormat<C>::add_literal(const string_type& text) {
        if (! text.empty()) {
            if (seq.empty() || seq.back().index != 0)
                seq.push_back({0, text, 0, 0, 0, 0});
            else
                seq.back().text += text;
        }
    }

    template <typename C>
    template <typename T, typename... Args>
    void BasicFormat<C>::apply(string_list& list, int index, const T& t, const Args&... args) const {
        using namespace UnicornDetail;
        for (size_t i = 0; i < seq.size(); ++i) {
            if (seq[i].index == index) {
                auto s = format_type(t, seq[i].flags, seq[i].prec);
                list[i] = format_align(recode<C>(s), seq[i].flags & fx_global_flags, seq[i].width, seq[i].pad);
            }
        }
        apply(list, index + 1, args...);
    }

    using Format = BasicFormat<char>;
    using Format16 = BasicFormat<char16_t>;
    using Format32 = BasicFormat<char32_t>;
    using WideFormat = BasicFormat<wchar_t>;

    template <typename C> BasicFormat<C> format(const basic_string<C>& fmt) { return BasicFormat<C>(fmt); }
    template <typename C> BasicFormat<C> format(const C* fmt) { return BasicFormat<C>(cstr(fmt)); }

    // Formatter literals

    namespace Literals {

        inline auto operator"" _fmt(const char* ptr, size_t len) { return Format(cstr(ptr, len)); }
        inline auto operator"" _fmt(const char16_t* ptr, size_t len) { return Format16(cstr(ptr, len)); }
        inline auto operator"" _fmt(const char32_t* ptr, size_t len) { return Format32(cstr(ptr, len)); }
        inline auto operator"" _fmt(const wchar_t* ptr, size_t len) { return WideFormat(cstr(ptr, len)); }

    }

}
