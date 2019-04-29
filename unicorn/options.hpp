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

        class command_error:
        public std::runtime_error {
        public:
            explicit command_error(const Ustring& details): std::runtime_error(details) {}
        };

        class spec_error:
        public std::runtime_error {
        public:
            explicit spec_error(const Ustring& details): std::runtime_error(details) {}
        };

        enum class help { unset, std, automatic };

        static constexpr uint32_t colour = 1;                // Always colourize help text
        static constexpr uint32_t nocolour = 2;              // Do not colourize help text
        static constexpr uint32_t locale = 4;                // Argument list is in local encoding
        static constexpr uint32_t noprefix = 8;              // First argument is not the command name
        static constexpr uint32_t quoted = 16;               // Allow arguments to be quoted
        static constexpr Kwarg<bool, 1> anon = {};           // Assign anonymous arguments to this option
        static constexpr Kwarg<bool, 2> boolean = {};        // Boolean option
        static constexpr Kwarg<bool, 3> file = {};           // Argument is expected to be a file path
        static constexpr Kwarg<bool, 4> floating = {};       // Argument must be a floating point number
        static constexpr Kwarg<bool, 5> integer = {};        // Argument must be an integer
        static constexpr Kwarg<bool, 6> multi = {};          // Option may have multiple arguments
        static constexpr Kwarg<bool, 7> required = {};       // Option is required
        static constexpr Kwarg<bool, 8> si = {};             // Numeric arguments can use SI abbreviations
        static constexpr Kwarg<bool, 9> uinteger = {};       // Argument must be an unsigned integer
        static constexpr Kwarg<Ustring, 1> abbrev = {};      // Single letter abbreviation
        static constexpr Kwarg<Ustring, 2> defvalue = {};    // Default value if not supplied
        static constexpr Kwarg<Ustring, 3> group = {};       // Mutual exclusion group name
        static constexpr Kwarg<Ustring, 4> implies = {};     // Argument implies another argument
        static constexpr Kwarg<Ustring, 5> pattern = {};     // Argument must match this regular expression
        static constexpr Kwarg<Ustring, 6> prereq = {};      // Option requires this prerequisite
        static constexpr Kwarg<enum_wrapper> enumtype = {};  // Argument must be one of the enumeration values

        Options() = default;
        explicit Options(const Ustring& info): app_info(str_trim(info)) {}

        template <typename... Args> Options& add(const Ustring& name, const Ustring& info, Args... args);
        Options& add(const Ustring& info);
        Options& add(help h);
        Ustring help_text(uint32_t flags = 0);
        Ustring version_text() const { return app_info; }
        template <typename C> bool parse(const std::vector<std::basic_string<C>>& args, std::ostream& out = std::cout, uint32_t flags = 0);
        template <typename C> bool parse(const std::basic_string<C>& args, std::ostream& out = std::cout, uint32_t flags = 0);
        template <typename C> bool parse(int argc, C** argv, std::ostream& out = std::cout, uint32_t flags = 0);
        template <typename T> T get(const Ustring& name) const;
        template <typename T> std::vector<T> get_list(const Ustring& name) const;
        bool has(const Ustring& name) const;

    private:

        struct option_type {
            Ustring abbrev;
            Ustring defvalue;
            Ustring group;
            Ustring implies;
            Ustring info;
            Ustring name;
            Ustring prereq;
            enum_wrapper enums;
            Regex pattern;
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
            bool is_si = false;
            bool is_uinteger = false;
            option_type() = default;
            template <typename... Args> option_type(const Ustring& nm, const Ustring& inf, Args... args);
            option_type(const Ustring& inf);
            option_type(const char* inf): option_type(cstr(inf)) {}
            void add_arg(const Ustring& arg);
        };

        using option_list = std::vector<option_type>;

        Ustring app_info;
        help help_flag = help::unset;
        option_list opts;
        int tail_opts = 0;
        bool checked = false;

        void setup_add_option(option_type opt);
        void setup_finalize();
        size_t find_first_index(const Ustring& name) const;
        bool find_if_exists(const Ustring& name) const;
        size_t find_runtime_option(const Ustring& name) const;
        bool parse_main(Strings args, std::ostream& out, uint32_t flags);
        void parse_initial_cleanup(Strings& args, uint32_t flags);
        Strings parse_explicit_anonymous(Strings& args);
        void parse_attached_arguments(Strings& args);
        void parse_named_options(Strings& args);
        void parse_remaining_anonymous(Strings& args, const Strings& anon_args);
        void parse_check_conditions();
        void parse_supply_defaults();

        template <typename C> static Ustring arg_convert(const std::basic_string<C>& str, uint32_t /*flags*/) { return to_utf8(str); }
        static Ustring arg_convert(const std::string& str, uint32_t flags);
        template <typename T> static T get_converted(const Ustring& str, bool with_si = false);
        static void unquote(const Ustring& src, Strings& dst);

    };

    template <typename... Args>
    Options& Options::add(const Ustring& name, const Ustring& info, Args... args) {
        setup_add_option(option_type(name, info, args...));
        return *this;
    }

    template <typename T>
    T Options::get(const Ustring& name) const {
        try {
            auto& opt = opts[find_runtime_option(name)];
            return get_converted<T>(str_join(opt.values, " "), opt.is_si);
        }
        catch (const command_error&) {
            return get_converted<T>(Ustring());
        }
    }

    template <typename T>
    std::vector<T> Options::get_list(const Ustring& name) const {
        try {
            auto& opt = opts[find_runtime_option(name)];
            std::vector<T> vec;
            for (auto& str: opt.values)
                vec.push_back(get_converted<T>(str, opt.is_si));
            return vec;
        }
        catch (const command_error&) {
            return {};
        }
    }

    template <typename C>
    bool Options::parse(const std::vector<std::basic_string<C>>& args, std::ostream& out, uint32_t flags) {
        Strings u8vec;
        std::transform(args.begin(), args.end(), append(u8vec), [=] (auto& s) { return arg_convert(s, flags); });
        return parse_main(u8vec, out, flags);
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
        return parse_main(vec, out, flags);
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
    T Options::get_converted(const Ustring& str, bool with_si) {
        (void)with_si;
        if (str.empty())
            return T();
        if constexpr (std::is_integral_v<T>) {
            if (str_starts_with(str, "0x") || str_starts_with(str, "0X"))
                return hex_to_int<T>(utf_iterator(str, 2));
            else if (with_si)
                return static_cast<T>(si_to_int(str));
            else
                return from_str<T>(str);
        } else if constexpr (std::is_floating_point_v<T>) {
            if (with_si)
                return static_cast<T>(si_to_float(str));
            else
                return from_str<T>(str);
        } else if constexpr (std::is_enum_v<T>) {
            T t = T();
            str_to_enum(str, t);
            return t;
        } else {
            return from_str<T>(str);
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
    Options::option_type::option_type(const Ustring& nm, const Ustring& inf, Args... args) {
        Ustring patstr;
        name = nm;
        info = inf;
        is_anon = kwget(Options::anon, false, args...);
        is_boolean = kwget(Options::boolean, false, args...);
        is_file = kwget(Options::file, false, args...);
        is_floating = kwget(Options::floating, false, args...);
        is_integer = kwget(Options::integer, false, args...);
        is_multi = kwget(Options::multi, false, args...);
        is_required = kwget(Options::required, false, args...);
        is_si = kwget(Options::si, false, args...);
        is_uinteger = kwget(Options::uinteger, false, args...);
        abbrev = kwget(Options::abbrev, Ustring(), args...);
        defvalue = kwget(Options::defvalue, Ustring(), args...);
        group = kwget(Options::group, Ustring(), args...);
        implies = kwget(Options::implies, Ustring(), args...);
        enums = kwget(Options::enumtype, enum_wrapper(), args...);
        patstr = kwget(Options::pattern, Ustring(), args...);
        prereq = kwget(Options::prereq, Ustring(), args...);
        if (! patstr.empty())
            pattern = Regex(patstr, Regex::full);
    }

}
