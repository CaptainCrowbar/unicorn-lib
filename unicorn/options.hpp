#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace Unicorn {

    class CommandLineError:
    public std::runtime_error {
    public:
        CommandLineError(const u8string& details, const u8string& arg, const u8string& arg2 = {});
    };

    class OptionSpecError:
    public std::runtime_error {
    public:
        explicit OptionSpecError(const u8string& option);
        OptionSpecError(const u8string& details, const u8string& option);
    };

    UNICORN_DEFINE_FLAG(command line parsing, opt_locale, 0);    // Argument list is in local encoding
    UNICORN_DEFINE_FLAG(command line parsing, opt_noprefix, 1);  // First argument is not the command name
    UNICORN_DEFINE_FLAG(command line parsing, opt_quoted, 2);    // Allow arguments to be quoted

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
        struct ArgConv<basic_string<C>, false> {
            basic_string<C> operator()(const u8string& s) const {
                return recode<C>(s);
            }
        };

    }

    class Options {
    public:
        static constexpr Kwarg<bool>
            anon = {},      // Assign anonymous arguments to this option
            boolean = {},   // Boolean option
            floating = {},     // Argument must be a floating point number
            integer = {},   // Argument must be an integer
            multiple = {},  // Option may have multiple arguments
            required = {};  // Option is required
        static constexpr Kwarg<u8string>
            abbrev = {},    // Single letter abbreviation
            defval = {},    // Default value if not supplied
            group = {},     // Mutual exclusion group name
            pattern = {};   // Argument must match this regular expression
        explicit Options(const u8string& info, const u8string& head = {}, const u8string& tail = {}):
            app_info(str_trim(info)), help_auto(false),
            help_head(str_trim(head)), help_tail(str_trim(tail)), opts() {}
        template <typename... Args> void add(const u8string& name, const u8string& info, const Args&... args);
        void autohelp() noexcept { help_auto = true; }
        u8string help() const;
        u8string version() const { return app_info; }
        template <typename C, typename C2>
            bool parse(const std::vector<basic_string<C>>& args, std::basic_ostream<C2>& out, uint32_t flags = 0);
        template <typename C, typename C2>
            bool parse(const basic_string<C>& args, std::basic_ostream<C2>& out, uint32_t flags = 0);
        template <typename C, typename C2>
            bool parse(int argc, C** argv, std::basic_ostream<C2>& out, uint32_t flags = 0);
        template <typename C> bool parse(const std::vector<basic_string<C>>& args) { return parse(args, std::cout); }
        template <typename C> bool parse(const basic_string<C>& args) { return parse(args, std::cout); }
        template <typename C> bool parse(int argc, C** argv) { return parse(argc, argv, std::cout); }
        template <typename T> T get(const u8string& name) const
            { return UnicornDetail::ArgConv<T>()(str_join(find_values(name), " ")); }
        template <typename T> std::vector<T> get_list(const u8string& name) const;
        bool has(const u8string& name) const { return find_index(name, true) != npos; }
    private:
        using string_list = std::vector<u8string>;
        enum class help_mode { none, version, usage };
        struct option_type {
            string_list values;
            u8string abbrev;
            u8string defval;
            u8string group;
            u8string info;
            u8string name;
            Regex pattern;
            bool found = false;
            bool is_anon = false;
            bool is_boolean = false;
            bool is_float = false;
            bool is_integer = false;
            bool is_multiple = false;
            bool is_required = false;
        };
        using option_list = std::vector<option_type>;
        u8string app_info;
        bool help_auto;
        u8string help_head;
        u8string help_tail;
        option_list opts;
        void add_option(option_type opt);
        size_t find_index(u8string name, bool found = false) const;
        string_list find_values(const u8string& name) const;
        help_mode parse_args(string_list args, uint32_t flags);
        void add_help_version();
        void clean_up_arguments(string_list& args, uint32_t flags);
        string_list parse_forced_anonymous(string_list& args);
        void parse_attached_arguments(string_list& args);
        void expand_abbreviations(string_list& args);
        void extract_named_options(string_list& args);
        void parse_remaining_anonymous(string_list& args, const string_list& anon);
        void check_required();
        void supply_defaults();
        template <typename C> void send_help(std::basic_ostream<C>& out, help_mode mode) const;
        template <typename C> static u8string arg_convert(const basic_string<C>& str, uint32_t /*flags*/)
            { return to_utf8(str); }
        static u8string arg_convert(const string& str, uint32_t flags);
        static void check_flags(uint32_t flags)
            { UnicornDetail::allow_flags(flags, opt_locale | opt_noprefix | opt_quoted, "command line parsing"); }
        static void unquote(const u8string& src, string_list& dst);
    };

    template <typename... Args>
    void Options::add(const u8string& name, const u8string& info, const Args&... args) {
        option_type opt;
        u8string pat;
        opt.name = name;
        opt.info = info;
        kwget(anon, opt.is_anon, args...);
        kwget(boolean, opt.is_boolean, args...);
        kwget(floating, opt.is_float, args...);
        kwget(integer, opt.is_integer, args...);
        kwget(multiple, opt.is_multiple, args...);
        kwget(required, opt.is_required, args...);
        kwget(abbrev, opt.abbrev, args...);
        kwget(defval, opt.defval, args...);
        kwget(group, opt.group, args...);
        kwget(pattern, pat, args...);
        opt.pattern = Regex(pat);
        add_option(opt);
    }

    template <typename T>
    std::vector<T> Options::get_list(const u8string& name) const {
        string_list svec = find_values(name);
        std::vector<T> tvec;
        std::transform(PRI_BOUNDS(svec), append(tvec), UnicornDetail::ArgConv<T>());
        return tvec;
    }

    template <typename C, typename C2>
    bool Options::parse(const std::vector<basic_string<C>>& args, std::basic_ostream<C2>& out, uint32_t flags) {
        check_flags(flags);
        string_list u8vec;
        std::transform(PRI_BOUNDS(args), append(u8vec),
            [=] (const basic_string<C>& s) { return arg_convert(s, flags); });
        auto help_wanted = parse_args(u8vec, flags);
        send_help(out, help_wanted);
        return help_wanted != help_mode::none;
    }

    template <typename C, typename C2>
    bool Options::parse(const basic_string<C>& args, std::basic_ostream<C2>& out, uint32_t flags) {
        check_flags(flags);
        auto u8args = arg_convert(args, flags);
        string_list vec;
        if (flags & opt_quoted) {
            unquote(u8args, vec);
            flags &= ~ opt_quoted;
        } else {
            str_split(u8args, append(vec));
        }
        auto help_wanted = parse_args(vec, flags);
        send_help(out, help_wanted);
        return help_wanted != help_mode::none;
    }

    template <typename C, typename C2>
    bool Options::parse(int argc, C** argv, std::basic_ostream<C2>& out, uint32_t flags) {
        check_flags(flags);
        std::vector<basic_string<C>> args(argv, argv + argc);
        if (flags & opt_quoted)
            return parse(str_join(args, str_chars<C>(U' ')), out, flags);
        else
            return parse(args, out, flags);
    }

    template <typename C>
    void Options::send_help(std::basic_ostream<C>& out, help_mode mode) const {
        u8string message;
        switch (mode) {
            case help_mode::version:  message = app_info + '\n'; break;
            case help_mode::usage:    message = help(); break;
            default:                  break;
        }
        if (! message.empty())
            out << recode<C>(message);
    }

}
