#pragma once

#include "unicorn/character.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utility.hpp"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace RS::Unicorn {

    class Options {
    private:
        struct enum_wrapper {
            std::set<Ustring> values;
            enum_wrapper() = default;
            template <typename T> enum_wrapper(T);
            bool check(const Ustring& str) const { return values.count(str) != 0; }
        };
    public:
        class command_error: public std::runtime_error {
        public:
            explicit command_error(const Ustring& details, const Ustring& arg = {}, const Ustring& arg2 = {});
        };
        class spec_error: public std::runtime_error {
        public:
            explicit spec_error(const Ustring& option);
            spec_error(const Ustring& details, const Ustring& option);
        };
        enum special_options { help, autohelp };
        static constexpr uint32_t locale = 1;                // Argument list is in local encoding
        static constexpr uint32_t noprefix = 2;              // First argument is not the command name
        static constexpr uint32_t quoted = 4;                // Allow arguments to be quoted
        static constexpr Kwarg<bool, 'a'> anon = {};         // Assign anonymous arguments to this option
        static constexpr Kwarg<bool, 'b'> boolean = {};      // Boolean option
        static constexpr Kwarg<bool, 'f'> floating = {};     // Argument must be a floating point number
        static constexpr Kwarg<bool, 'i'> integer = {};      // Argument must be an integer
        static constexpr Kwarg<bool, 'm'> multi = {};        // Option may have multiple arguments
        static constexpr Kwarg<bool, 'p'> file = {};         // Argument is expected to be a file path
        static constexpr Kwarg<bool, 'r'> required = {};     // Option is required
        static constexpr Kwarg<bool, 'u'> uinteger = {};     // Argument must be an unsigned integer
        static constexpr Kwarg<Ustring, 'a'> abbrev = {};    // Single letter abbreviation
        static constexpr Kwarg<Ustring, 'd'> defvalue = {};  // Default value if not supplied
        static constexpr Kwarg<Ustring, 'g'> group = {};     // Mutual exclusion group name
        static constexpr Kwarg<Ustring, 'p'> pattern = {};   // Argument must match this regular expression
        static constexpr Kwarg<enum_wrapper> enumtype = {};  // Argument must be one of the enumeration values
        Options() = default;
        explicit Options(const Ustring& info): app_info(str_trim(info)) {}
        template <typename... Args> Options& add(const Ustring& name, const Ustring& info, Args... args);
        Options& add(const Ustring& info);
        Options& add(special_options flag);
        void add_help(bool automatic = false);
        Ustring help_text() const;
        Ustring version_text() const { return app_info; }
        template <typename C> bool parse(const std::vector<std::basic_string<C>>& args, std::ostream& out = std::cout, uint32_t flags = 0);
        template <typename C> bool parse(const std::basic_string<C>& args, std::ostream& out = std::cout, uint32_t flags = 0);
        template <typename C> bool parse(int argc, C** argv, std::ostream& out = std::cout, uint32_t flags = 0);
        template <typename T> T get(const Ustring& name) const { return get_converted<T>(str_join(find_values(name), " ")); }
        template <typename T> std::vector<T> get_list(const Ustring& name) const;
        bool has(const Ustring& name) const;
    private:
        enum class help_mode { none, version, usage };
        struct option_type {
            option_type() = default;
            template <typename... Args> option_type(const Ustring& name, const Ustring& info, Args... args);
            option_type(const Ustring& info);
            option_type(const char* info): option_type(cstr(info)) {}
            Ustring opt_abbrev;
            Ustring opt_defvalue;
            Ustring opt_group;
            Ustring opt_info;
            Ustring opt_name;
            enum_wrapper opt_enum;
            Regex opt_pattern;
            Strings values;
            bool is_anon = false;
            bool is_boolean = false;
            bool is_enumtype = false;
            bool is_file = false;
            bool is_floating = false;
            bool is_found = false;
            bool is_integer = false;
            bool is_multi = false;
            bool is_required = false;
            bool is_uinteger = false;
        };
        using option_list = std::vector<option_type>;
        Ustring app_info;
        int help_flag = -1;
        option_list opts;
        void add_option(option_type opt);
        size_t find_index(Ustring name, bool require = false) const;
        Strings find_values(const Ustring& name) const;
        help_mode parse_args(Strings args, uint32_t flags);
        void clean_up_arguments(Strings& args, uint32_t flags);
        Strings parse_forced_anonymous(Strings& args);
        void parse_attached_arguments(Strings& args);
        void expand_abbreviations(Strings& args);
        void extract_named_options(Strings& args);
        void parse_remaining_anonymous(Strings& args, const Strings& anon_args);
        void check_required();
        void supply_defaults();
        void send_help(std::ostream& out, help_mode mode) const;
        template <typename C> static Ustring arg_convert(const std::basic_string<C>& str, uint32_t /*flags*/) { return to_utf8(str); }
        static Ustring arg_convert(const std::string& str, uint32_t flags);
        static void add_arg_to_opt(const Ustring& arg, option_type& opt);
        template <typename T> static T get_converted(const Ustring& str);
        static void unquote(const Ustring& src, Strings& dst);
    };

    template <typename... Args>
    Options& Options::add(const Ustring& name, const Ustring& info, Args... args) {
        add_option(option_type(name, info, args...));
        return *this;
    }

    template <typename T>
    std::vector<T> Options::get_list(const Ustring& name) const {
        Strings svec = find_values(name);
        std::vector<T> tvec;
        std::transform(svec.begin(), svec.end(), append(tvec), get_converted<T>);
        return tvec;
    }

    template <typename C>
    bool Options::parse(const std::vector<std::basic_string<C>>& args, std::ostream& out, uint32_t flags) {
        Strings u8vec;
        std::transform(args.begin(), args.end(), append(u8vec),
            [=] (const std::basic_string<C>& s) { return arg_convert(s, flags); });
        auto help_wanted = parse_args(u8vec, flags);
        send_help(out, help_wanted);
        return help_wanted != help_mode::none;
    }

    template <typename C>
    bool Options::parse(const std::basic_string<C>& args, std::ostream& out, uint32_t flags) {
        auto u8args = arg_convert(args, flags);
        Strings vec;
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

    template <typename T>
    T Options::get_converted(const Ustring& str) {
        if (str.empty())
            return T();
        if constexpr (std::is_integral_v<T>) {
            if (str.size() >= 3 && str[0] == '0' && (str[1] == 'X' || str[1] == 'x'))
                return hex_to_int<T>(utf_iterator(str, 2));
            else
                return static_cast<T>(si_to_int(str));
        } else if constexpr (std::is_floating_point_v<T>) {
            return static_cast<T>(si_to_float(str));
        } else if constexpr (std::is_enum_v<T>) {
            T t = T();
            str_to_enum(str, t); // already checked
            return t;
        } else {
            return static_cast<T>(str);
        }
    }

    template <typename T>
    Options::enum_wrapper::enum_wrapper(T) {
        for (T t: enum_values<T>()) {
            Ustring s = unqualify(to_str(t));
            std::replace(s.begin(), s.end(), '_', '-');
            values.insert(s);
        }
    }

    template <typename... Args>
    Options::option_type::option_type(const Ustring& name, const Ustring& info, Args... args) {
        Ustring patstr;
        opt_name = name;
        opt_info = info;
        is_anon = kwget(anon, false, args...);
        is_boolean = kwget(boolean, false, args...);
        is_file = kwget(file, false, args...);
        is_floating = kwget(floating, false, args...);
        is_integer = kwget(integer, false, args...);
        is_multi = kwget(multi, false, args...);
        is_required = kwget(required, false, args...);
        is_uinteger = kwget(uinteger, false, args...);
        opt_abbrev = kwget(abbrev, Ustring(), args...);
        opt_defvalue = kwget(defvalue, Ustring(), args...);
        opt_group = kwget(group, Ustring(), args...);
        opt_enum = kwget(enumtype, enum_wrapper(), args...);
        patstr = kwget(pattern, Ustring(), args...);
        if (! patstr.empty())
            opt_pattern = Regex(patstr, Regex::full);
    }

    inline Options::option_type::option_type(const Ustring& info) {
        opt_info = str_trim_right(info);
        if (opt_info.empty())
            throw spec_error("Empty information string");
    }

}
