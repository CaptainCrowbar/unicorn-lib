#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace RS {

    namespace Unicorn {

        namespace UnicornDetail {

            template <typename T, bool I = std::is_integral<T>::value, bool F = std::is_floating_point<T>::value>
            struct ArgConv;

            template <typename T>
            struct ArgConv<T, false, false> {
                T operator()(const U8string& s) const {
                    return s.empty() ? T() : static_cast<T>(s);
                }
            };

            template <typename T>
            struct ArgConv<T, true, false> {
                T operator()(const U8string& s) const {
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
                T operator()(const U8string& s) const {
                    return s.empty() ? T(0) : T(si_to_float(s));
                }
            };

            template <typename C>
            struct ArgConv<std::basic_string<C>, false, false> {
                std::basic_string<C> operator()(const U8string& s) const {
                    return recode<C>(s);
                }
            };

        }


        class Options {
        public:
            class command_error: public std::runtime_error {
            public:
                explicit command_error(const U8string& details, const U8string& arg = {}, const U8string& arg2 = {});
            };
            class spec_error: public std::runtime_error {
            public:
                explicit spec_error(const U8string& option);
                spec_error(const U8string& details, const U8string& option);
            };
            enum special_options { help, autohelp };
            static constexpr uint32_t locale    = 1;  // Argument list is in local encoding
            static constexpr uint32_t noprefix  = 2;  // First argument is not the command name
            static constexpr uint32_t quoted    = 4;  // Allow arguments to be quoted
            static constexpr Kwarg<bool>
                anon = {},      // Assign anonymous arguments to this option
                boolean = {},   // Boolean option
                integer = {},   // Argument must be an integer
                uinteger = {},  // Argument must be an unsigned integer
                floating = {},  // Argument must be a floating point number
                multi = {},     // Option may have multiple arguments
                required = {};  // Option is required
            static constexpr Kwarg<U8string>
                abbrev = {},    // Single letter abbreviation
                defvalue = {},  // Default value if not supplied
                group = {},     // Mutual exclusion group name
                pattern = {};   // Argument must match this regular expression
            Options() = default;
            explicit Options(const U8string& info): app_info(str_trim(info)) {}
            template <typename... Args> Options& add(const U8string& name, const U8string& info, const Args&... args)
                { add_option(option_type(name, info, args...)); return *this; }
            Options& add(const U8string& info);
            Options& add(special_options flag);
            void add_help(bool automatic = false);
            U8string help_text() const;
            U8string version_text() const { return app_info; }
            template <typename C> bool parse(const std::vector<std::basic_string<C>>& args, std::ostream& out = std::cout, uint32_t flags = 0);
            template <typename C> bool parse(const std::basic_string<C>& args, std::ostream& out = std::cout, uint32_t flags = 0);
            template <typename C> bool parse(int argc, C** argv, std::ostream& out = std::cout, uint32_t flags = 0);
            template <typename T> T get(const U8string& name) const { return UnicornDetail::ArgConv<T>()(str_join(find_values(name), " ")); }
            template <typename T> std::vector<T> get_list(const U8string& name) const;
            bool has(const U8string& name) const;
        private:
            using string_list = std::vector<U8string>;
            enum class help_mode { none, version, usage };
            struct option_type {
                option_type() = default;
                template <typename... Args> option_type(const U8string& name, const U8string& info, const Args&... args);
                option_type(const U8string& info);
                option_type(const char* info): option_type(cstr(info)) {}
                string_list values;
                U8string opt_name;
                U8string opt_info;
                U8string opt_abbrev;
                U8string opt_defvalue;
                U8string opt_group;
                Regex opt_pattern;
                bool is_found = false;
                bool is_anon = false;
                bool is_boolean = false;
                bool is_integer = false;
                bool is_uinteger = false;
                bool is_floating = false;
                bool is_multi = false;
                bool is_required = false;
            };
            using option_list = std::vector<option_type>;
            U8string app_info;
            int help_flag = -1;
            option_list opts;
            void add_option(option_type opt);
            size_t find_index(U8string name, bool require = false) const;
            string_list find_values(const U8string& name) const;
            help_mode parse_args(string_list args, uint32_t flags);
            void clean_up_arguments(string_list& args, uint32_t flags);
            string_list parse_forced_anonymous(string_list& args);
            void parse_attached_arguments(string_list& args);
            void expand_abbreviations(string_list& args);
            void extract_named_options(string_list& args);
            void parse_remaining_anonymous(string_list& args, const string_list& anon);
            void check_required();
            void supply_defaults();
            void send_help(std::ostream& out, help_mode mode) const;
            template <typename C> static U8string arg_convert(const std::basic_string<C>& str, uint32_t /*flags*/) { return to_utf8(str); }
            static U8string arg_convert(const std::string& str, uint32_t flags);
            static void add_arg_to_opt(const U8string& arg, option_type& opt);
            static void unquote(const U8string& src, string_list& dst);
        };

        template <typename T>
        std::vector<T> Options::get_list(const U8string& name) const {
            string_list svec = find_values(name);
            std::vector<T> tvec;
            std::transform(svec.begin(), svec.end(), append(tvec), UnicornDetail::ArgConv<T>());
            return tvec;
        }

        template <typename C>
        bool Options::parse(const std::vector<std::basic_string<C>>& args, std::ostream& out, uint32_t flags) {
            string_list u8vec;
            std::transform(args.begin(), args.end(), append(u8vec),
                [=] (const std::basic_string<C>& s) { return arg_convert(s, flags); });
            auto help_wanted = parse_args(u8vec, flags);
            send_help(out, help_wanted);
            return help_wanted != help_mode::none;
        }

        template <typename C>
        bool Options::parse(const std::basic_string<C>& args, std::ostream& out, uint32_t flags) {
            auto u8args = arg_convert(args, flags);
            string_list vec;
            if (flags & quoted) {
                unquote(u8args, vec);
                flags &= ~ quoted;
            } else {
                str_split(u8args, append(vec));
            }
            auto help_wanted = parse_args(vec, flags);
            send_help(out, help_wanted);
            return help_wanted != help_mode::none;
        }

        template <typename C>
        bool Options::parse(int argc, C** argv, std::ostream& out, uint32_t flags) {
            std::vector<std::basic_string<C>> args(argv, argv + argc);
            if (flags & quoted)
                return parse(str_join(args, std::basic_string<C>{C(' ')}), out, flags);
            else
                return parse(args, out, flags);
        }

        template <typename... Args>
        Options::option_type::option_type(const U8string& name, const U8string& info, const Args&... args) {
            U8string patstr;
            opt_name = name;
            opt_info = info;
            kwget(anon, is_anon, args...);
            kwget(boolean, is_boolean, args...);
            kwget(integer, is_integer, args...);
            kwget(uinteger, is_uinteger, args...);
            kwget(floating, is_floating, args...);
            kwget(multi, is_multi, args...);
            kwget(required, is_required, args...);
            kwget(abbrev, opt_abbrev, args...);
            kwget(defvalue, opt_defvalue, args...);
            kwget(group, opt_group, args...);
            kwget(pattern, patstr, args...);
            if (! patstr.empty())
                opt_pattern = Regex(patstr);
        }

        inline Options::option_type::option_type(const U8string& info) {
            opt_info = str_trim_right(info);
            if (opt_info.empty())
                throw spec_error("Empty information string");
        }

    }

}
