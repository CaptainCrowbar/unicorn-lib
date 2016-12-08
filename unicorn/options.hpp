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

    namespace UnicornDetail {

        template <typename T, bool I = std::is_integral<T>::value,
            bool F = std::is_floating_point<T>::value>
        struct ArgConv;

        template <typename T>
        struct ArgConv<T, false, false> {
            T operator()(const u8string& s) const {
                return s.empty() ? T() : static_cast<T>(s);
            }
        };

        template <typename T>
        struct ArgConv<T, true, false> {
            T operator()(const u8string& s) const {
                if (s.empty())
                    return T(0);
                else if (s.size() >= 3 && s[0] == '0' && (s[1] == 'X' || s[1] == 'x'))
                    return hex_to_int<T>(utf_iterator(s, 2));
                else
                    return T(si_to_int(s));
            }
        };

        template <typename T>
        struct ArgConv<T, false, true> {
            T operator()(const u8string& s) const {
                return s.empty() ? T(0) : T(si_to_float(s));
            }
        };

        template <typename C>
        struct ArgConv<basic_string<C>, false, false> {
            basic_string<C> operator()(const u8string& s) const {
                return recode<C>(s);
            }
        };

    }

    constexpr uint32_t opt_locale = 1;    // Argument list is in local encoding
    constexpr uint32_t opt_noprefix = 2;  // First argument is not the command name
    constexpr uint32_t opt_quoted = 4;    // Allow arguments to be quoted

    constexpr Kwarg<bool>
        opt_anon,     // Assign anonymous arguments to this option
        opt_bool,     // Boolean option
        opt_int,      // Argument must be an integer
        opt_uint,     // Argument must be an unsigned integer
        opt_float,    // Argument must be a floating point number
        opt_multi,    // Option may have multiple arguments
        opt_require;  // Option is required
    constexpr Kwarg<u8string>
        opt_abbrev,   // Single letter abbreviation
        opt_default,  // Default value if not supplied
        opt_group,    // Mutual exclusion group name
        opt_pattern;  // Argument must match this regular expression

    class Options {
    public:
        class CommandError: public std::runtime_error {
        public:
            explicit CommandError(const u8string& details, const u8string& arg = {}, const u8string& arg2 = {});
        };
        class SpecError: public std::runtime_error {
        public:
            explicit SpecError(const u8string& option);
            SpecError(const u8string& details, const u8string& option);
        };
        Options() = default;
        explicit Options(const u8string& info): app_info(str_trim(info)) {}
        Options& add(const u8string& info);
        template <typename... Args> Options& add(const u8string& name, const u8string& info, const Args&... args);
        void autohelp() noexcept { help_auto = true; }
        u8string help() const;
        u8string version() const { return app_info; }
        template <typename C> bool parse(const vector<basic_string<C>>& args, std::ostream& out = cout, uint32_t flags = 0);
        template <typename C> bool parse(const basic_string<C>& args, std::ostream& out = cout, uint32_t flags = 0);
        template <typename C> bool parse(int argc, C** argv, std::ostream& out = cout, uint32_t flags = 0);
        template <typename T> T get(const u8string& name) const
            { return UnicornDetail::ArgConv<T>()(str_join(find_values(name), " ")); }
        template <typename T> vector<T> get_list(const u8string& name) const;
        bool has(const u8string& name) const;
    private:
        using string_list = vector<u8string>;
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
            bool is_integer = false;
            bool is_uinteger = false;
            bool is_float = false;
            bool is_multiple = false;
            bool is_required = false;
        };
        using option_list = vector<option_type>;
        u8string app_info;
        bool help_auto = false;
        option_list opts;
        void add_option(option_type opt);
        size_t find_index(u8string name, bool require = false) const;
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
        void send_help(std::ostream& out, help_mode mode) const;
        template <typename C> static u8string arg_convert(const basic_string<C>& str, uint32_t /*flags*/)
            { return to_utf8(str); }
        static u8string arg_convert(const string& str, uint32_t flags);
        static void unquote(const u8string& src, string_list& dst);
    };

    template <typename... Args>
    Options& Options::add(const u8string& name, const u8string& info, const Args&... args) {
        option_type opt;
        u8string pat;
        opt.name = name;
        opt.info = info;
        kwget(opt_anon, opt.is_anon, args...);
        kwget(opt_bool, opt.is_boolean, args...);
        kwget(opt_int, opt.is_integer, args...);
        kwget(opt_uint, opt.is_uinteger, args...);
        kwget(opt_float, opt.is_float, args...);
        kwget(opt_multi, opt.is_multiple, args...);
        kwget(opt_require, opt.is_required, args...);
        kwget(opt_abbrev, opt.abbrev, args...);
        kwget(opt_default, opt.defval, args...);
        kwget(opt_group, opt.group, args...);
        kwget(opt_pattern, pat, args...);
        opt.pattern = Regex(pat);
        add_option(opt);
        return *this;
    }

    template <typename T>
    vector<T> Options::get_list(const u8string& name) const {
        string_list svec = find_values(name);
        vector<T> tvec;
        std::transform(svec.begin(), svec.end(), append(tvec), UnicornDetail::ArgConv<T>());
        return tvec;
    }

    template <typename C>
    bool Options::parse(const vector<basic_string<C>>& args, std::ostream& out, uint32_t flags) {
        string_list u8vec;
        std::transform(args.begin(), args.end(), append(u8vec),
            [=] (const basic_string<C>& s) { return arg_convert(s, flags); });
        auto help_wanted = parse_args(u8vec, flags);
        send_help(out, help_wanted);
        return help_wanted != help_mode::none;
    }

    template <typename C>
    bool Options::parse(const basic_string<C>& args, std::ostream& out, uint32_t flags) {
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

    template <typename C>
    bool Options::parse(int argc, C** argv, std::ostream& out, uint32_t flags) {
        vector<basic_string<C>> args(argv, argv + argc);
        if (flags & opt_quoted)
            return parse(str_join(args, basic_string<C>{C(' ')}), out, flags);
        else
            return parse(args, out, flags);
    }

}
