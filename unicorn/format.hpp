#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/regex.hpp"
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

    // Forward declarations

    template <typename T> class FormatType;

    template <typename T, typename C>
        void format_type(const T& t, basic_string<C>& dst, uint64_t flags = 0, int prec = -1, size_t width = 0, char32_t pad = U' ');
    template <typename T, typename C> void format_type(const T& t, basic_string<C>& dst, const basic_string<C>& flags);
    template <typename T, typename C> void format_type(const T& t, basic_string<C>& dst, const C* flags);
    template <typename C, typename T>
        basic_string<C> format_as(const T& t, uint64_t flags = 0, int prec = -1, size_t width = 0, char32_t pad = U' ');
    template <typename C, typename T> basic_string<C> format_as(const T& t, const basic_string<C>& flags);
    template <typename C, typename T> basic_string<C> format_as(const T& t, const C* flags);

    // Formatting behaviour flags

    UNICORN_DEFINE_FLAG_64(formatting,      fx_left,      52);   // (<) Left align                  all  --    --   --     --    --      --
    UNICORN_DEFINE_FLAG_64(formatting,      fx_centre,    53);   // (=) Centre align                all  --    --   --     --    --      --
    UNICORN_DEFINE_FLAG_64(formatting,      fx_right,     54);   // (>) Right align                 all  --    --   --     --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_upper,     'U');  // Convert to upper case           all  --    --   --     --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_lower,     'L');  // Convert to lower case           all  --    --   --     --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_title,     'T');  // Convert to title case           all  --    --   --     --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_tf,        't');  // Write as true/false             --   bool  --   --     --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_yesno,     'y');  // Write as yes/no                 --   bool  --   --     --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_binary,    'b');  // Binary number                   --   bool  int  --     --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_decimal,   'n');  // Decimal number                  --   --    int  --     char  string  --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_hex,       'x');  // Hexadecimal number              --   --    int  --     char  string  --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_roman,     'r');  // Roman numerals                  --   --    int  --     --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_sign,      's');  // Always show a sign              --   --    int  float  --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_signz,     'S');  // Always show a sign unless zero  --   --    int  float  --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_digits,    'd');  // Fixed significant figures       --   --    --   float  --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_exp,       'e');  // Scientific notation             --   --    --   float  --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_fixed,     'f');  // Fixed point notation            --   --    --   float  --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_general,   'g');  // Use the shorter of d or e       --   --    --   float  --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_prob,      'p');  // Probability format              --   --    --   float  --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_stripz,    'z');  // Strip trailing zeros            --   --    --   float  --    --      --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_ascii,     'a');  // Escape if not printable ASCII   --   --    --   --     char  string  --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_escape,    'c');  // Escape if C0/C1 control         --   --    --   --     char  string  --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_quote,     'q');  // Quote string, escape C0/C1      --   --    --   --     char  string  --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_ascquote,  'o');  // Quote string, escape non-ASCII  --   --    --   --     char  string  --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_hex8,      'u');  // Hex UTF-8 bytes                 --   --    --   --     char  string  --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_hex16,     'v');  // Hex UTF-16 code units           --   --    --   --     char  string  --
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_iso,       't');  // ISO 8601 with T delimiter       --   --    --   --     --    --      date
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_common,    'c');  // Local standard format           --   --    --   --     --    --      date
    UNICORN_DEFINE_FLAG_LETTER(formatting,  fx_local,     'l');  // Local time zone                 --   --    --   --     --    --      date

    namespace UnicornDetail {

        // Reserved global flags

        constexpr uint64_t fx_length_flags = character_units | grapheme_units | narrow_context | wide_context;
        constexpr uint64_t fx_toplevel_flags = fx_length_flags | fx_left | fx_centre | fx_right | fx_lower | fx_title | fx_upper;
        constexpr uint64_t fx_global_flags = fx_length_flags | fx_toplevel_flags;

        // Alignment and padding

        template <typename C>
        void format_align(basic_string<C> src, basic_string<C>& dst, uint64_t flags, size_t width, char32_t pad) {
            exclusive_flags(flags, fx_left | fx_centre | fx_right, "formatting");
            exclusive_flags(flags, fx_lower | fx_title | fx_upper, "formatting");
            if (flags & fx_lower)
                str_lowercase_in(src);
            else if (flags & fx_title)
                str_titlecase_in(src);
            else if (flags & fx_upper)
                str_uppercase_in(src);
            size_t len = str_length(src, flags & fx_length_flags);
            if (width <= len) {
                dst += src;
                return;
            }
            size_t extra = width - len;
            if (flags & fx_right)
                str_append_chars(dst, pad, extra);
            else if (flags & fx_centre)
                str_append_chars(dst, pad, extra / 2);
            dst += src;
            if (flags & fx_left)
                str_append_chars(dst, pad, extra);
            else if (flags & fx_centre)
                str_append_chars(dst, pad, (extra + 1) / 2);
        }

        void translate_flags(const u8string& str, uint64_t& flags, int& prec, size_t& width, char32_t& pad);

        // Formatting for specific types

        u8string format_boolean(bool t, uint64_t flags);
        u8string format_integer(uint128_t t, uint64_t flags, int prec);
        u8string format_integer_roman(uint32_t n);
        u8string format_floating(long double t, uint64_t flags, int prec);
        u8string format_string(const u8string& t, uint64_t flags, int prec);
        u8string format_timepoint(std::chrono::system_clock::time_point t, uint64_t flags, int prec);

        template <typename T>
        u8string format_integer_radix(T t, int base, int prec) {
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

        struct FormatBoolean {
            u8string operator()(bool t, uint64_t flags, int /*prec*/) const { return format_boolean(t, flags); }
        };

        template <typename T>
        struct FormatInteger {
            u8string operator()(T t, uint64_t flags, int prec) const {
                if ((flags & (fx_digits | fx_exp | fx_fixed | fx_general | fx_prob | fx_stripz))
                        && ! (flags & (fx_binary | fx_decimal | fx_hex | fx_roman)))
                    return format_floating(t, flags, prec);
                allow_flags(flags, fx_global_flags | fx_binary | fx_decimal | fx_hex | fx_roman | fx_sign | fx_signz, "formatting");
                exclusive_flags(flags, fx_binary | fx_decimal | fx_hex | fx_roman, "formatting");
                exclusive_flags(flags, fx_sign | fx_signz, "formatting");
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
                    s = format_integer_radix(t, 2, prec);
                else if (flags & fx_roman)
                    s = format_integer_roman(uint32_t(t));
                else if (flags & fx_hex)
                    s = format_integer_radix(t, 16, prec);
                else
                    s = format_integer_radix(t, 10, prec);
                if (sign)
                    s.insert(s.begin(), sign);
                return s;
            }
        };

        template <typename T>
        struct FormatFloatingPoint {
            u8string operator()(T t, uint64_t flags, int prec) const {
                return format_floating(t, flags, prec);
            }
        };

        template <typename C>
        struct FormatString {
            u8string operator()(const basic_string<C>& t, uint64_t flags, int prec) const {
                return format_string(to_utf8(t), flags, prec);
            }
        };

        template <typename C>
        struct FormatCharacterPointer {
            u8string operator()(const C* t, uint64_t flags, int prec) const {
                return format_string(to_utf8(cstr(t)), flags, prec);
            }
        };

        template <typename C>
        struct FormatCharacter {
            u8string operator()(C t, uint64_t flags, int prec) const {
                return format_string(str_chars<char>(t), flags, prec);
            }
        };

        struct FormatDate {
            u8string operator()(std::chrono::system_clock::time_point tp, uint64_t flags, int prec) const {
                return format_timepoint(tp, flags, prec);
            }
        };

        template <typename R, typename P>
        struct FormatTime {
            u8string operator()(std::chrono::duration<R, P> t, uint64_t /*flags*/, int prec) const {
                return format_time(t, prec);
            }
        };

        struct FormatUuid {
            u8string operator()(const Uuid& t, uint64_t flags, int /*prec*/) const {
                allow_flags(flags, fx_global_flags, "formatting");
                return t.str();
            }
        };

        struct FormatVersion {
            u8string operator()(const Version& t, uint64_t flags, int /*prec*/) const {
                allow_flags(flags, fx_global_flags, "formatting");
                return t.str();
            }
        };

        // Formatting for range types

        template <typename R, typename I = decltype(std::begin(std::declval<R>())),
            typename V = typename std::iterator_traits<I>::value_type>
        struct FormatRange {
            u8string operator()(const R& r, uint64_t flags, int prec) const {
                u8string s;
                for (auto& v: r) {
                    s += format_as<char>(v, flags, prec);
                    s += ',';
                }
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
                auto s = out.str();
                sanitize(s);
                return s;
            }
        };

        template <typename T>
        struct FormatObject<T, true>:
        FormatRange<T> {};

        template <typename T1, typename T2>
        struct FormatObject<std::pair<T1, T2>, false> {
            u8string operator()(const std::pair<T1, T2>& p, uint64_t flags, int prec) const {
                u8string s = format_as<char>(p.first, flags, prec);
                s += ':';
                s += format_as<char>(p.second, flags, prec);
                return s;
            }
        };

    }

    // Generic formatting function object

    template <typename T> class FormatType: public UnicornDetail::FormatObject<T> {};
    template <> class FormatType<bool>: public UnicornDetail::FormatBoolean {};
    template <> class FormatType<signed char>: public UnicornDetail::FormatInteger<signed char> {};
    template <> class FormatType<unsigned char>: public UnicornDetail::FormatInteger<unsigned char> {};
    template <> class FormatType<short>: public UnicornDetail::FormatInteger<short> {};
    template <> class FormatType<unsigned short>: public UnicornDetail::FormatInteger<unsigned short> {};
    template <> class FormatType<int>: public UnicornDetail::FormatInteger<int> {};
    template <> class FormatType<unsigned>: public UnicornDetail::FormatInteger<unsigned> {};
    template <> class FormatType<long>: public UnicornDetail::FormatInteger<long> {};
    template <> class FormatType<unsigned long>: public UnicornDetail::FormatInteger<unsigned long> {};
    template <> class FormatType<long long>: public UnicornDetail::FormatInteger<long long> {};
    template <> class FormatType<unsigned long long>: public UnicornDetail::FormatInteger<unsigned long long> {};
    template <> class FormatType<int128_t>: public UnicornDetail::FormatInteger<int128_t> {};
    template <> class FormatType<uint128_t>: public UnicornDetail::FormatInteger<uint128_t> {};
    template <> class FormatType<float>: public UnicornDetail::FormatFloatingPoint<float> {};
    template <> class FormatType<double>: public UnicornDetail::FormatFloatingPoint<double> {};
    template <> class FormatType<long double>: public UnicornDetail::FormatFloatingPoint<long double> {};
    template <typename C> class FormatType<basic_string<C>>: public UnicornDetail::FormatString<C> {};
    template <> class FormatType<char*>: public UnicornDetail::FormatCharacterPointer<char> {};
    template <> class FormatType<char16_t*>: public UnicornDetail::FormatCharacterPointer<char16_t> {};
    template <> class FormatType<char32_t*>: public UnicornDetail::FormatCharacterPointer<char32_t> {};
    template <> class FormatType<wchar_t*>: public UnicornDetail::FormatCharacterPointer<wchar_t> {};
    template <> class FormatType<const char*>: public UnicornDetail::FormatCharacterPointer<char> {};
    template <> class FormatType<const char16_t*>: public UnicornDetail::FormatCharacterPointer<char16_t> {};
    template <> class FormatType<const char32_t*>: public UnicornDetail::FormatCharacterPointer<char32_t> {};
    template <> class FormatType<const wchar_t*>: public UnicornDetail::FormatCharacterPointer<wchar_t> {};
    template <> class FormatType<char>: public UnicornDetail::FormatCharacter<char> {};
    template <> class FormatType<char16_t>: public UnicornDetail::FormatCharacter<char16_t> {};
    template <> class FormatType<char32_t>: public UnicornDetail::FormatCharacter<char32_t> {};
    template <> class FormatType<wchar_t>: public UnicornDetail::FormatCharacter<wchar_t> {};
    template <> class FormatType<std::chrono::system_clock::time_point>: public UnicornDetail::FormatDate {};
    template <typename R, typename P> class FormatType<std::chrono::duration<R, P>>: public UnicornDetail::FormatTime<R, P> {};
    template <> class FormatType<Uuid>: public UnicornDetail::FormatUuid {};
    template <> class FormatType<Version>: public UnicornDetail::FormatVersion {};

    // Basic formatting functions

    template <typename T, typename C>
    void format_type(const T& t, basic_string<C>& dst, uint64_t flags, int prec, size_t width, char32_t pad) {
        using namespace UnicornDetail;
        auto s = FormatType<std::decay_t<T>>()(t, flags & ~ fx_toplevel_flags, prec);
        format_align(recode<C>(s), dst, flags & fx_global_flags, width, pad);
    }

    template <typename T, typename C>
    void format_type(const T& t, basic_string<C>& dst, const basic_string<C>& flags) {
        using namespace UnicornDetail;
        uint64_t f = 0;
        int prec = 0;
        size_t width = 0;
        char32_t pad = U' ';
        translate_flags(to_utf8(flags), f, prec, width, pad);
        format_type(t, dst, f, prec, width, pad);
    }

    template <typename T, typename C>
    void format_type(const T& t, basic_string<C>& dst, const C* flags) {
        format_type(t, dst, cstr(flags));
    }

    template <typename C, typename T>
    basic_string<C> format_as(const T& t, uint64_t flags, int prec, size_t width, char32_t pad) {
        basic_string<C> dst;
        format_type(t, dst, flags, prec, width, pad);
        return dst;
    }

    template <typename C, typename T>
    basic_string<C> format_as(const T& t, const basic_string<C>& flags) {
        basic_string<C> dst;
        format_type(t, dst, flags);
        return dst;
    }

    template <typename C, typename T>
    basic_string<C> format_as(const T& t, const C* flags) {
        basic_string<C> dst;
        format_type(t, dst, flags);
        return dst;
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
        using regex_type = BasicRegex<C>;
        static const u8string pattern_ascii =
            "(?:\\$(\\d+)(\\w*))"
            "|(?:\\$\\{(\\d+)([^{}]*)\\})"
            "|(?:\\$(.?))";
        static const regex_type pattern(recode<C>(pattern_ascii));
        size_t prev = 0;
        for (;;) {
            auto match = pattern.search(format, prev);
            if (! match) {
                add_literal(format.substr(prev, npos));
                break;
            }
            if (match.offset() > prev)
                add_literal(format.substr(prev, match.offset() - prev));
            if (match.count(1))
                add_index(str_to_integer<unsigned>(match[1]), match[2]);
            else if (match.count(3))
                add_index(str_to_integer<unsigned>(match[3]), match[4]);
            else
                add_literal(match[5]);
            prev = match.endpos();
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
        for (size_t i = 0; i < seq.size(); ++i)
            if (seq[i].index == index)
                format_type(t, list[i], seq[i].flags, seq[i].prec, seq[i].width, seq[i].pad);
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
