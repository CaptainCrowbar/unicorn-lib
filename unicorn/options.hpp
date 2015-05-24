#pragma once

#include "crow/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/format.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>

namespace Unicorn {

    // Exceptions

    class CommandLineError:
    public std::runtime_error {
    public:
        explicit CommandLineError(const u8string& details, const u8string& arg1 = {}, const u8string& arg2 = {}):
            std::runtime_error(assemble(details, arg1, arg2)) {}
    private:
        static u8string assemble(const u8string& details, const u8string& arg1, const u8string& arg2);
    };

    class OptionSpecError:
    public std::runtime_error {
    public:
        explicit OptionSpecError(const u8string& details, const u8string& option = {}):
            std::runtime_error(assemble(details, option)) {}
    private:
        static u8string assemble(const u8string& details, const u8string& option);
    };

    class HelpRequest:
    public std::exception {
    public:
        explicit HelpRequest(const u8string& details): std::exception(), msg(details) {}
        virtual const char* what() const noexcept { return msg.data(); }
    private:
        u8string msg;
    };

    // Constants

    constexpr auto opt_anon      = Flagset::value('a');  // Assign anonymous arguments to this option  add()
    constexpr auto opt_boolean   = Flagset::value('b');  // Boolean option                             add()
    constexpr auto opt_float     = Flagset::value('f');  // Argument must be a floating point number   add()
    constexpr auto opt_integer   = Flagset::value('i');  // Argument must be an integer                add()
    constexpr auto opt_multiple  = Flagset::value('m');  // Option may have multiple arguments         add()
    constexpr auto opt_required  = Flagset::value('r');  // Option is required                         add()
    constexpr auto opt_locale    = Flagset::value('l');  // Argument list is in local encoding         parse()
    constexpr auto opt_noprefix  = Flagset::value('n');  // First argument is not the command name     parse()
    constexpr auto opt_quoted    = Flagset::value('q');  // Allow arguments to be quoted               parse()

    // Options parsing class

    namespace UnicornDetail {

        template <typename T, bool FP = std::is_floating_point<T>::value>
        struct ArgConv {
            T operator()(const u8string& s) const {
                return static_cast<T>(s);
            }
        };

        template <typename T>
        struct ArgConv<T, true> {
            T operator()(const u8string& s) const {
                return str_to_real<T>(s);
            }
        };

        template <typename T>
        struct ArgConv<T, false> {
            T operator()(const u8string& s) const {
                if (s.size() >= 3 && s[0] == '0' && (s[1] == 'X' || s[1] == 'x'))
                    return hex_to_integer<T>(s, 2);
                else
                    return str_to_integer<T>(s);
            }
        };

        template <typename C>
        struct ArgConv<std::basic_string<C>, false> {
            std::basic_string<C> operator()(const u8string& s) const {
                return recode<C>(s);
            }
        };

    }

    class Options {
    public:
        template <typename C> explicit Options(const std::basic_string<C>& info,
            const std::basic_string<C>& head = {}, const std::basic_string<C>& tail = {});
        template <typename C> explicit Options(const C* info,
            const C* head = nullptr, const C* tail = nullptr):
            Options(cstr(info), cstr(head), cstr(tail)) {}
        template <typename C> void add(const std::basic_string<C>& name, C abbrev,
            const std::basic_string<C>& info, Flagset flags = {},
            const std::basic_string<C>& defval = {}, const std::basic_string<C>& pattern = {},
            const std::basic_string<C>& group = {}) {
                add_option(to_utf8(name), to_utf8(std::basic_string<C>{abbrev}), to_utf8(info), flags,
                    to_utf8(defval), to_utf8(pattern), to_utf8(group));
            }
        template <typename C> void add(const C* name, C abbrev, const C* info, Flagset flags = {},
            const C* defval = {}, const C* pattern = {}, const C* group = {}) {
                add_option(to_utf8(cstr(name)), to_utf8(std::basic_string<C>{abbrev}), to_utf8(cstr(info)), flags,
                    to_utf8(cstr(defval)), to_utf8(cstr(pattern)), to_utf8(cstr(group)));
            }
        void autohelp() noexcept { help_auto = true; }
        u8string help() const;
        u8string version() const { return app_info; }
        template <typename C> void parse(const std::vector<std::basic_string<C>>& args, Flagset flags = {});
        template <typename C> void parse(const std::basic_string<C>& args, Flagset flags = {});
        template <typename C> void parse(int argc, C** argv, Flagset flags = {});
        template <typename C> bool has(const std::basic_string<C>& name) const
            { return opt_index(to_utf8(name), true) != npos; }
        template <typename C> bool has(const C* name) const
            { return opt_index(to_utf8(cstr(name)), true) != npos; }
        template <typename T, typename C> T get(const std::basic_string<C>& name) const
            { return UnicornDetail::ArgConv<T>()(str_join(opt_values(to_utf8(name)), " ")); }
        template <typename T, typename C> T get(const C* name) const { return get<T>(cstr(name)); }
        template <typename T, typename C> std::vector<T> getlist(const std::basic_string<C>& name) const;
        template <typename T, typename C> std::vector<T> getlist(const C* name) const
            { return getlist<T>(cstr(name)); }
    private:
        struct option_type {
            u8string name {};
            u8string abbrev {};
            u8string defval {};
            u8string info {};
            u8string group {};
            u8string argtype {};
            Regex pattern {};
            std::vector<u8string> values {};
            Flagset flags {};
            bool found {false};
        };
        using option_list = std::vector<option_type>;
        u8string app_info;
        bool help_auto;
        u8string help_head;
        u8string help_tail;
        option_list opts;
        void add_option(const u8string& name, const u8string& abbrev, const u8string& info,
            Flagset flags, const u8string& defval, const u8string& pattern, const u8string& group);
        size_t opt_index(const u8string& name, bool found = false) const;
        std::vector<u8string> opt_values(const u8string& name) const;
        void parse_args(std::vector<u8string> args, Flagset flags);
        template <typename C> static u8string arg_convert(const std::basic_string<C>& str, Flagset /*flags*/)
            { return to_utf8(str); }
        static u8string arg_convert(const std::string& str, Flagset flags);
        static void check_flags(Flagset flags)
            { flags.allow(opt_locale | opt_noprefix | opt_quoted, "command line option"); }
        static void unquote(const u8string& src, std::vector<u8string>& dst);
    };

    template <typename C>
    Options::Options(const std::basic_string<C>& info,
            const std::basic_string<C>& head, const std::basic_string<C>& tail):
        app_info(to_utf8(str_trim(info))),
        help_auto(false),
        help_head(to_utf8(str_trim(head))),
        help_tail(to_utf8(str_trim(tail))),
        opts() {}

    template <typename T, typename C>
    std::vector<T> Options::getlist(const std::basic_string<C>& name) const {
        std::vector<u8string> svec = opt_values(to_utf8(name));
        std::vector<T> tvec;
        std::transform(CROW_BOUNDS(svec), append(tvec), UnicornDetail::ArgConv<T>());
        return tvec;
    }

    template <typename C>
    void Options::parse(const std::vector<std::basic_string<C>>& args, Flagset flags) {
        check_flags(flags);
        std::vector<u8string> u8vec;
        std::transform(CROW_BOUNDS(args), append(u8vec),
            [=] (const std::basic_string<C>& s) { return arg_convert(s, flags); });
        parse_args(u8vec, flags);
    }

    template <typename C>
    void Options::parse(const std::basic_string<C>& args, Flagset flags) {
        check_flags(flags);
        auto u8args = arg_convert(args, flags);
        std::vector<u8string> vec;
        if (flags.get(opt_quoted)) {
            unquote(u8args, vec);
            flags.set(opt_quoted, false);
        } else {
            str_split(u8args, append(vec));
        }
        parse_args(vec, flags);
    }

    template <typename C>
    void Options::parse(int argc, C** argv, Flagset flags) {
        check_flags(flags);
        std::vector<std::basic_string<C>> args(argv, argv + argc);
        if (flags.get(opt_quoted))
            parse(str_join(args, str_chars<C>(U' ')), flags);
        else
            parse(args, flags);
    }

}
