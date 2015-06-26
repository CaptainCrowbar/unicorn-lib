#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include "crow/meta.hpp"
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
        void format_type(const T& t, basic_string<C>& dst, Crow::Flagset flags = {},
            int prec = -1, size_t width = 0, char32_t pad = U' ');
    template <typename T, typename C>
        void format_type(const T& t, basic_string<C>& dst, const basic_string<C>& flags);
    template <typename T, typename C>
        void format_type(const T& t, basic_string<C>& dst, const C* flags);
    template <typename C, typename T>
        basic_string<C> format_as(const T& t, Crow::Flagset flags = {},
            int prec = -1, size_t width = 0, char32_t pad = U' ');
    template <typename C, typename T>
        basic_string<C> format_as(const T& t, const basic_string<C>& flags);
    template <typename C, typename T>
        basic_string<C> format_as(const T& t, const C* flags);

    // Formatting behaviour flags

    constexpr auto fx_left      = Crow::Flagset::value('<');  // Left align                      all  --    --   --     --    --  --
    constexpr auto fx_centre    = Crow::Flagset::value('=');  // Centre align                    all  --    --   --     --    --  --
    constexpr auto fx_right     = Crow::Flagset::value('>');  // Right align                     all  --    --   --     --    --  --
    constexpr auto fx_upper     = Crow::Flagset::value('U');  // Convert to upper case           all  --    --   --     --    --  --
    constexpr auto fx_lower     = Crow::Flagset::value('L');  // Convert to lower case           all  --    --   --     --    --  --
    constexpr auto fx_title     = Crow::Flagset::value('T');  // Convert to title case           all  --    --   --     --    --  --
    constexpr auto fx_tf        = Crow::Flagset::value('t');  // Write as true/false             --   bool  --   --     --    --  --
    constexpr auto fx_yesno     = Crow::Flagset::value('y');  // Write as yes/no                 --   bool  --   --     --    --  --
    constexpr auto fx_binary    = Crow::Flagset::value('b');  // Binary number                   --   bool  int  --     --    --  --
    constexpr auto fx_decimal   = Crow::Flagset::value('n');  // Decimal number                  --   --    int  --     char  string  --
    constexpr auto fx_hex       = Crow::Flagset::value('x');  // Hexadecimal number              --   --    int  --     char  string  --
    constexpr auto fx_roman     = Crow::Flagset::value('r');  // Roman numerals                  --   --    int  --     --    --  --
    constexpr auto fx_sign      = Crow::Flagset::value('s');  // Always show a sign              --   --    int  float  --    --  --
    constexpr auto fx_signz     = Crow::Flagset::value('i');  // Always show a sign unless zero  --   --    int  float  --    --  --
    constexpr auto fx_digits    = Crow::Flagset::value('d');  // Fixed significant figures       --   --    --   float  --    --  --
    constexpr auto fx_exp       = Crow::Flagset::value('e');  // Scientific notation             --   --    --   float  --    --  --
    constexpr auto fx_fixed     = Crow::Flagset::value('f');  // Fixed point notation            --   --    --   float  --    --  --
    constexpr auto fx_general   = Crow::Flagset::value('g');  // Use the shorter of d or e       --   --    --   float  --    --  --
    constexpr auto fx_prob      = Crow::Flagset::value('p');  // Probability format              --   --    --   float  --    --  --
    constexpr auto fx_stripz    = Crow::Flagset::value('z');  // Strip trailing zeros            --   --    --   float  --    --  --
    constexpr auto fx_ascii     = Crow::Flagset::value('a');  // Escape if not printable ASCII   --   --    --   --     char  string  --
    constexpr auto fx_escape    = Crow::Flagset::value('c');  // Escape if C0/C1 control         --   --    --   --     char  string  --
    constexpr auto fx_quote     = Crow::Flagset::value('q');  // Quote string, escape C0/C1      --   --    --   --     char  string  --
    constexpr auto fx_ascquote  = Crow::Flagset::value('o');  // Quote string, escape non-ASCII  --   --    --   --     char  string  --
    constexpr auto fx_hex8      = Crow::Flagset::value('u');  // Hex UTF-8 bytes                 --   --    --   --     char  string  --
    constexpr auto fx_hex16     = Crow::Flagset::value('v');  // Hex UTF-16 code units           --   --    --   --     char  string  --
    constexpr auto fx_iso       = Crow::Flagset::value('t');  // ISO 8601 with T delimiter       --   --    --   --     --    --      date
    constexpr auto fx_common    = Crow::Flagset::value('c');  // Local standard format           --   --    --   --     --    --      date
    constexpr auto fx_local     = Crow::Flagset::value('l');  // Local time zone                 --   --    --   --     --    --      date

    namespace UnicornDetail {

        // Reserved global flags

        constexpr uint64_t fx_length_flags = character_units | grapheme_units | narrow_context | wide_context;
        constexpr uint64_t fx_toplevel_flags = fx_length_flags | fx_left | fx_centre | fx_right | fx_lower | fx_title | fx_upper;
        constexpr uint64_t fx_global_flags = fx_length_flags | fx_toplevel_flags;

        // Alignment and padding

        template <typename C>
        void format_align(basic_string<C> src, basic_string<C>& dst,
                Crow::Flagset flags, size_t width, char32_t pad) {
            flags.exclusive(fx_left | fx_centre | fx_right, "formatting");
            flags.exclusive(fx_lower | fx_title | fx_upper, "formatting");
            if (flags.get(fx_lower))
                str_lowercase_in(src);
            else if (flags.get(fx_title))
                str_titlecase_in(src);
            else if (flags.get(fx_upper))
                str_uppercase_in(src);
            size_t len = str_length(src, flags & fx_length_flags);
            if (width <= len) {
                dst += src;
                return;
            }
            size_t extra = width - len;
            auto unit = str_chars<C>(pad);
            auto padding = str_repeat(unit, flags.get(fx_centre) ? extra / 2 : extra);
            if (flags.get(fx_centre | fx_right))
                dst += padding;
            dst += src;
            if (! flags.get(fx_right))
                dst += padding;
            if (flags.get(fx_centre) && (extra & 1))
                dst += unit;
        }

        void translate_flags(const u8string& str, Crow::Flagset& flags, int& prec, size_t& width, char32_t& pad);

        // Formatting for specific types

        u8string format_boolean(bool t, Crow::Flagset flags);
        u8string format_integer(unsigned long long t, Crow::Flagset flags, int prec);
        u8string format_floating(long double t, Crow::Flagset flags, int prec);
        u8string format_string(const u8string& t, Crow::Flagset flags, int prec);
        u8string format_timepoint(std::chrono::system_clock::time_point t, Crow::Flagset flags, int prec);

        struct FormatBoolean {
            u8string operator()(bool t, Crow::Flagset flags, int /*prec*/) const {
                return format_boolean(t, flags);
            }
        };

        template <typename T>
        struct FormatInteger {
            u8string operator()(T t, Crow::Flagset flags, int prec) const {
                unsigned long long n = 0;
                char sign = 0;
                if (t > static_cast<T>(0)) {
                    n = static_cast<unsigned long long>(t);
                    if (flags.get(fx_sign | fx_signz))
                        sign = '+';
                } else if (t == static_cast<T>(0)) {
                    if (flags.get(fx_sign))
                        sign = '+';
                } else {
                    n = static_cast<unsigned long long>(- t);
                    sign = '-';
                }
                auto s = format_integer(n, flags, prec);
                if (sign)
                    s.insert(std::begin(s), sign);
                return s;
            }
        };

        template <typename T>
        struct FormatFloatingPoint {
            u8string operator()(T t, Crow::Flagset flags, int prec) const {
                return format_floating(t, flags, prec);
            }
        };

        template <typename C>
        struct FormatString {
            u8string operator()(const basic_string<C>& t, Crow::Flagset flags, int prec) const {
                return format_string(to_utf8(t), flags, prec);
            }
        };

        template <typename C>
        struct FormatCharacterPointer {
            u8string operator()(const C* t, Crow::Flagset flags, int prec) const {
                return format_string(to_utf8(Crow::cstr(t)), flags, prec);
            }
        };

        template <typename C>
        struct FormatCharacter {
            u8string operator()(C t, Crow::Flagset flags, int prec) const {
                return format_string(str_chars<char>(t), flags, prec);
            }
        };

        struct FormatDate {
            u8string operator()(std::chrono::system_clock::time_point tp, Crow::Flagset flags, int prec) const {
                return format_timepoint(tp, flags, prec);
            }
        };

        template <typename R, typename P>
        struct FormatTime {
            u8string operator()(std::chrono::duration<R, P> t, Crow::Flagset /*flags*/, int prec) const {
                return Crow::format_time(t, prec);
            }
        };

        struct FormatUuid {
            u8string operator()(const Crow::Uuid& t, Crow::Flagset flags, int /*prec*/) const {
                flags.allow(fx_global_flags, "UUID formatting");
                return t.str();
            }
        };

        struct FormatVersion {
            u8string operator()(const Crow::Version& t, Crow::Flagset flags, int /*prec*/) const {
                flags.allow(fx_global_flags, "version formatting");
                return t.str();
            }
        };

        // Formatting for range types

        template <typename R, typename I = decltype(std::begin(std::declval<R>())),
            typename V = typename std::iterator_traits<I>::value_type>
        struct FormatRange {
            u8string operator()(const R& r, Crow::Flagset flags, int prec) const {
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

        template <typename T, bool = Crow::IsRange<T>::value>
        struct FormatObject {
            u8string operator()(T t, Crow::Flagset /*flags*/, int /*prec*/) const {
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
            u8string operator()(const std::pair<T1, T2>& p, Crow::Flagset flags, int prec) const {
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
    template <> class FormatType<Crow::Uuid>: public UnicornDetail::FormatUuid {};
    template <> class FormatType<Crow::Version>: public UnicornDetail::FormatVersion {};

    // Basic formatting functions

    template <typename T, typename C>
    void format_type(const T& t, basic_string<C>& dst, Crow::Flagset flags,
            int prec, size_t width, char32_t pad) {
        using namespace UnicornDetail;
        auto s = FormatType<std::decay_t<T>>()(t, flags & ~ fx_toplevel_flags, prec);
        format_align(recode<C>(s), dst, flags & fx_global_flags, width, pad);
    }

    template <typename T, typename C>
    void format_type(const T& t, basic_string<C>& dst, const basic_string<C>& flags) {
        using namespace UnicornDetail;
        Crow::Flagset f;
        int prec = 0;
        size_t width = 0;
        char32_t pad = U' ';
        translate_flags(to_utf8(flags), f, prec, width, pad);
        format_type(t, dst, f, prec, width, pad);
    }

    template <typename T, typename C>
    void format_type(const T& t, basic_string<C>& dst, const C* flags) {
        format_type(t, dst, Crow::cstr(flags));
    }

    template <typename C, typename T>
    basic_string<C> format_as(const T& t, Crow::Flagset flags,
            int prec, size_t width, char32_t pad) {
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
    class Format {
    public:
        using char_type = C;
        using string_type = basic_string<C>;
        Format() = default;
        explicit Format(const string_type& format);
        template <typename... Args> string_type operator()(const Args&... args) const;
        size_t fields() const { return num; }
        string_type format() const { return fmt; }
    private:
        struct element {
            int index;            // 0 = literal text, 1+ = field reference
            string_type text;     // Literal text
            Crow::Flagset flags;  // Field formatting flags
            int prec;             // Field precision
            size_t width;         // Field width
            char32_t pad;         // Padding character
        };
        using sequence = std::vector<element>;
        using string_list = std::vector<string_type>;
        string_type fmt {};
        size_t num = 0;
        sequence seq {};
        void add_index(unsigned index, const string_type& flags = {});
        void add_literal(const string_type& text);
        void apply(string_list&, int) const {}
        template <typename T, typename... Args>
            void apply(string_list& list, int index, const T& t, const Args&... args) const;
    };

    template <typename C>
    Format<C>::Format(const string_type& format):
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
    basic_string<C> Format<C>::operator()(const Args&... args) const {
        string_list list(seq.size());
        for (size_t i = 0; i < seq.size(); ++i)
            if (seq[i].index == 0)
                list[i] = seq[i].text;
        apply(list, 1, args...);
        return str_join(list, string_type());
    }

    template <typename C>
    void Format<C>::add_index(unsigned index, const string_type& flags) {
        element elem;
        elem.index = index;
        UnicornDetail::translate_flags(to_utf8(flags), elem.flags, elem.prec, elem.width, elem.pad);
        seq.push_back(elem);
        if (index > 0 && index > num)
            num = index;
    }

    template <typename C>
    void Format<C>::add_literal(const string_type& text) {
        if (! text.empty()) {
            if (seq.empty() || seq.back().index != 0)
                seq.push_back({0, text, {}, 0, 0, 0});
            else
                seq.back().text += text;
        }
    }

    template <typename C>
    template <typename T, typename... Args>
    void Format<C>::apply(string_list& list, int index, const T& t, const Args&... args) const {
        for (size_t i = 0; i < seq.size(); ++i)
            if (seq[i].index == index)
                format_type(t, list[i], seq[i].flags, seq[i].prec, seq[i].width, seq[i].pad);
        apply(list, index + 1, args...);
    }

    template <typename C>
    Format<C> format(const basic_string<C>& fmt) {
        return Format<C>(fmt);
    }

    template <typename C>
    Format<C> format(const C* fmt) {
        return Format<C>(Crow::cstr(fmt));
    }

    // Formatter literals

    namespace Literals {

        inline auto operator"" _fmt(const char* ptr, size_t len) { return Format<char>(Crow::cstr(ptr, len)); }
        inline auto operator"" _fmt(const char16_t* ptr, size_t len) { return Format<char16_t>(Crow::cstr(ptr, len)); }
        inline auto operator"" _fmt(const char32_t* ptr, size_t len) { return Format<char32_t>(Crow::cstr(ptr, len)); }
        inline auto operator"" _fmt(const wchar_t* ptr, size_t len) { return Format<wchar_t>(Crow::cstr(ptr, len)); }

    }

}
