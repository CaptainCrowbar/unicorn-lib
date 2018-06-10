#include "unicorn/options.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include <utility>

using namespace RS::Unicorn::Literals;
using namespace std::literals;

namespace RS::Unicorn {

    namespace {

        const Regex match_float("[+-]?(\\d+(\\.\\d*)?|\\.\\d+)(e[+-]?\\d+)?( ?[kmgtpezy]\\w*)?", Regex::full | Regex::icase);
        const Regex match_integer("0x[[:xdigit:]]+|[+-]?\\d+((\\.\\d+)? ?[kmgtpezy]\\w*)?", Regex::full | Regex::icase);
        const Regex match_unsigned("0x[[:xdigit:]]+|\\d+((\\.\\d+)? ?[kmgtpezy]\\w*)?", Regex::full | Regex::icase);

        enum ArgType {
            is_argument = 'a',
            is_long_option = 'l',
            is_short_option = 's',
        };

        ArgType arg_type(const Ustring& arg) {
            if (arg.size() < 2 || arg[0] != '-' || ascii_isdigit(arg[1]))
                return is_argument;
            else if (arg[1] != '-' && arg[1] != '=')
                return is_short_option;
            else if (arg.size() >= 3 && arg[1] == '-' && arg[2] != '-' && arg[2] != '=')
                return is_long_option;
            else
                throw Options::command_error("Argument not recognised", arg);
        }

        Ustring cmd_error(const Ustring& details, const Ustring& arg, const Ustring& arg2) {
            Ustring msg = details;
            if (! arg.empty() || ! arg2.empty())
                msg += ": ";
            if (! arg.empty())
                msg += "$1q"_fmt(arg);
            if (! arg.empty() && ! arg2.empty())
                msg += ", ";
            if (! arg2.empty())
                msg += "$1q"_fmt(arg2);
            return msg;
        }

    }

    Options::command_error::command_error(const Ustring& details, const Ustring& arg, const Ustring& arg2):
    std::runtime_error(cmd_error(details, arg, arg2)) {}

    Options::spec_error::spec_error(const Ustring& option):
    std::runtime_error("Invalid option spec: $1q"_fmt(option)) {}

    Options::spec_error::spec_error(const Ustring& details, const Ustring& option):
    std::runtime_error("$1: $2q"_fmt(details, option)) {}

    Options& Options::add(const Ustring& info) {
        if (app_info.empty())
            app_info = str_trim(info);
        else
            opts.push_back(option_type(info));
        return *this;
    }

    Options& Options::add(special_options flag) {
        if (flag == help || flag == autohelp) {
            if (help_flag != -1)
                throw spec_error("Multiple help options");
            help_flag = flag == autohelp;
            option_type opt;
            opt.is_boolean = true;
            opt.opt_name = "help";
            opt.opt_info = "Show usage information";
            if (find_index("h") == npos)
                opt.opt_abbrev = "h";
            opts.push_back(opt);
            opt = {};
            opt.is_boolean = true;
            opt.opt_name = "version";
            opt.opt_info = "Show version information";
            if (find_index("v") == npos)
                opt.opt_abbrev = "v";
            opts.push_back(opt);
        } else {
            throw spec_error("Unknown flag");
        }
        return *this;
    }

    Ustring Options::help_text() const {
        static constexpr auto length_flags = Length::graphemes | Length::narrow;
        Ustring text = "\n" + app_info + "\n";
        Strings prefixes, suffixes;
        std::vector<size_t> lengths;
        for (auto& opt: opts) {
            Ustring prefix, suffix;
            size_t length = 0;
            if (! opt.opt_name.empty()) {
                if (opt.is_anon)
                    prefix += "[";
                prefix += "--";
                if (opt.is_boolean && opt.opt_defvalue == "1")
                    prefix += "no-";
                prefix += opt.opt_name;
                if (! opt.opt_abbrev.empty())
                    prefix += ", -"s + opt.opt_abbrev;
                if (opt.is_anon)
                    prefix += "]";
                if (! opt.is_boolean) {
                    prefix += " <";
                    if (opt.is_integer || opt.is_uinteger || opt.is_floating)
                        prefix += "num";
                    else
                        prefix += "arg";
                    prefix += ">";
                    if (opt.is_multi)
                        prefix += " ...";
                }
                length = str_length(prefix, length_flags);
                Ustring extra;
                if (opt.is_required) {
                    extra = "required";
                } else if (! opt.opt_defvalue.empty() && ! opt.is_boolean && opt.opt_info.find("default") == npos) {
                    extra = "default ";
                    if (match_integer(opt.opt_defvalue) || match_float(opt.opt_defvalue))
                        extra += opt.opt_defvalue;
                    else
                        extra += "$1q"_fmt(opt.opt_defvalue);
                }
                suffix = opt.opt_info;
                if (! extra.empty()) {
                    if (suffix.back() == ')') {
                        suffix.pop_back();
                        suffix += "; ";
                    } else {
                        suffix += " (";
                    }
                    suffix += extra + ")";
                }
            }
            prefixes.push_back(prefix);
            suffixes.push_back(suffix);
            lengths.push_back(length);
        }
        size_t maxlen = 0;
        if (! opts.empty())
            maxlen = *std::max_element(lengths.begin(), lengths.end());
        bool opthdr = false, was_info = true;
        for (size_t i = 0; i < opts.size(); ++i) {
            bool is_info = opts[i].opt_name.empty();
            if (is_info || was_info)
                text += "\n";
            if (is_info) {
                text += opts[i].opt_info + "\n";
            } else {
                if (! opthdr) {
                    text += "Options:\n\n";
                    opthdr = true;
                }
                Ustring prefix = str_pad_right(prefixes[i], maxlen, U' ', length_flags);
                text += "    $1  = $2\n"_fmt(prefix, suffixes[i]);
            }
            was_info = is_info;
        }
        if (! opthdr) {
            if (was_info)
                text += "\n";
            text += "Options:\n\n    None\n";
        }
        text += "\n";
        return text;
    }

    bool Options::has(const Ustring& name) const {
        size_t i = find_index(name, true);
        return i != npos && opts[i].is_found;
    }

    void Options::add_option(option_type opt) {
        str_trim_in(opt.opt_name, "-"s + ascii_whitespace);
        Ustring tag = opt.opt_name;
        if (str_length(opt.opt_name) < 2)
            throw spec_error(tag);
        bool neg = opt.opt_name.substr(0, 3) == "no-";
        str_trim_in(opt.opt_info);
        if (opt.opt_info.empty())
            throw spec_error(tag);
        str_trim_in(opt.opt_abbrev, "-");
        if (str_length(opt.opt_abbrev) > 1
                || (! opt.opt_abbrev.empty() && ! char_is_alphanumeric(*utf_begin(opt.opt_abbrev)))
                || (opt.is_boolean && (opt.is_anon || opt.is_multi || opt.is_required || ! opt.opt_pattern.empty()))
                || ((opt.is_boolean || opt.is_required) && ! opt.opt_defvalue.empty())
                || (neg && (! opt.is_boolean || ! opt.opt_abbrev.empty()))
                || (opt.is_required && ! opt.opt_group.empty())
                || (int(opt.is_integer) + int(opt.is_uinteger) + int(opt.is_floating) + int(! opt.opt_pattern.empty()) > 1))
            throw spec_error(tag);
        if (opt.is_integer)
            opt.opt_pattern = match_integer;
        else if (opt.is_uinteger)
            opt.opt_pattern = match_unsigned;
        else if (opt.is_floating)
            opt.opt_pattern = match_float;
        if (! opt.opt_defvalue.empty() && ! opt.opt_pattern.empty() && ! opt.opt_pattern(opt.opt_defvalue))
            throw spec_error(tag);
        if (neg) {
            opt.opt_name.erase(0, 3);
            opt.opt_defvalue = "1";
        }
        if (find_index(opt.opt_name) != npos || find_index(opt.opt_abbrev) != npos)
            throw spec_error("Duplicate option spec", tag);
        opts.push_back(opt);
    }

    size_t Options::find_index(Ustring name, bool require) const {
        str_trim_in(name, "-");
        if (name.substr(0, 3) == "no-")
            name.erase(0, 3);
        if (name.empty())
            return npos;
        auto i = std::find_if(opts.begin(), opts.end(),
            [=] (const auto& o) { return o.opt_name == name || o.opt_abbrev == name; });
        if (i != opts.end())
            return i - opts.begin();
        else if (require)
            throw spec_error("--" + name);
        else
            return npos;
    }

    Strings Options::find_values(const Ustring& name) const {
        size_t i = find_index(name, true);
        return i != npos ? opts[i].values : Strings();
    }

    Options::help_mode Options::parse_args(Strings args, uint32_t flags) {
        if (help_flag == -1)
            add(Options::help);
        clean_up_arguments(args, flags);
        if (help_flag && args.empty())
            return help_mode::usage;
        auto is_anon = parse_forced_anonymous(args);
        parse_attached_arguments(args);
        expand_abbreviations(args);
        extract_named_options(args);
        parse_remaining_anonymous(args, is_anon);
        if (has("help"))
            return help_mode::usage;
        if (has("version"))
            return help_mode::version;
        check_required();
        supply_defaults();
        return help_mode::none;
    }

    void Options::clean_up_arguments(Strings& args, uint32_t flags) {
        if (! (flags & noprefix) && ! args.empty())
            args.erase(args.begin());
        if (flags & quoted)
            for (auto& arg: args)
                if (arg.size() >= 2 && arg.front() == '\"' && arg.back() == '\"')
                    arg = arg.substr(1, arg.size() - 1);
    }

    Strings Options::parse_forced_anonymous(Strings& args) {
        Strings anon_args;
        auto i = std::find(args.begin(), args.end(), "--");
        if (i != args.end()) {
            anon_args.assign(i + 1, args.end());
            args.erase(i, args.end());
        }
        return anon_args;
    }

    void Options::parse_attached_arguments(Strings& args) {
        size_t i = 0;
        while (i < args.size()) {
            if (arg_type(args[i]) != is_argument) {
                Ustring key, value;
                bool paired = str_partition_at(args[i], key, value, "=");
                if (paired) {
                    args[i] = key;
                    args.insert(args.begin() + i + 1, value);
                    ++i;
                }
            }
            ++i;
        }
    }

    void Options::expand_abbreviations(Strings& args) {
        size_t i = 0;
        while (i < args.size()) {
            if (arg_type(args[i]) == is_short_option) {
                auto arg = args[i];
                args.erase(args.begin() + i);
                size_t j = 0;
                for (auto u = std::next(utf_begin(arg)), uend = utf_end(arg); u != uend; ++u, ++j) {
                    size_t o = find_index(arg.substr(u.offset(), u.count()));
                    if (o == npos)
                        throw command_error("Unknown option", arg);
                    args.insert(args.begin() + i + j, "--" + opts[o].opt_name);
                }
                i += j;
            } else {
                ++i;
            }
        }
    }

    void Options::extract_named_options(Strings& args) {
        size_t a = 0;
        while (a < args.size()) {
            if (arg_type(args[a]) != is_long_option) {
                ++a;
                continue;
            }
            size_t o = find_index(args[a]);
            if (o == npos)
                throw command_error("Unknown option", args[a]);
            auto& opt(opts[o]);
            if (opt.is_found && ! opt.is_multi)
                throw command_error("Duplicate option", args[a]);
            if (! opt.opt_group.empty())
                for (auto& opt2: opts)
                    if (&opt2 != &opt && opt2.opt_group == opt.opt_group && opt2.is_found)
                        throw command_error("Incompatible options", "--" + opt2.opt_name, args[a]);
            opt.is_found = true;
            if (opt.is_boolean) {
                if (args[a].substr(0, 5) == "--no-")
                    add_arg_to_opt("0", opt);
                else
                    add_arg_to_opt("1", opt);
                args.erase(args.begin() + a);
            } else {
                size_t n = 1, max_n = 1;
                if (opt.is_multi)
                    max_n = args.size() - a;
                else
                    max_n = std::min(size_t(2), args.size() - a);
                for (; n < max_n && arg_type(args[a + n]) == is_argument; ++n)
                    add_arg_to_opt(args[a + n], opt);
                args.erase(args.begin() + a, args.begin() + a + n);
            }
        }
    }

    void Options::parse_remaining_anonymous(Strings& args, const Strings& anon_args) {
        args.insert(args.end(), anon_args.begin(), anon_args.end());
        for (auto& opt: opts) {
            if (args.empty())
                break;
            if (! opt.is_anon)
                continue;
            opt.is_found = true;
            size_t n = opt.is_multi ? args.size() : 1;
            for (size_t i = 0; i < n; ++i)
                add_arg_to_opt(args[i], opt);
            args.erase(args.begin(), args.begin() + n);
        }
        if (! args.empty())
            throw command_error("Unexpected argument", args[0]);
    }

    void Options::check_required() {
        for (auto& opt: opts)
            if (opt.is_required && ! opt.is_found)
                throw command_error("Required option is missing", "--" + opt.opt_name);
    }

    void Options::supply_defaults() {
        for (auto& opt: opts)
            if (opt.values.empty() && ! opt.opt_defvalue.empty())
                add_arg_to_opt(opt.opt_defvalue, opt);
    }

    void Options::send_help(std::ostream& out, help_mode mode) const {
        Ustring message;
        switch (mode) {
            case help_mode::version:  message = app_info + '\n'; break;
            case help_mode::usage:    message = help_text(); break;
            default:                  break;
        }
        if (! message.empty())
            out << message;
    }

    Ustring Options::arg_convert(const std::string& str, uint32_t flags) {
        if (! (flags & locale))
            return str;
        Ustring utf8;
        import_string(str, utf8, local_encoding());
        return utf8;
    }

    void Options::add_arg_to_opt(const Ustring& arg, option_type& opt) {
        if (! opt.opt_pattern.empty() && (opt.is_required || ! arg.empty()) && ! opt.opt_pattern(arg))
            throw command_error("Invalid argument to option", "--" + opt.opt_name, arg);
        opt.values.push_back(arg);
    }

    void Options::unquote(const Ustring& src, Strings& dst) {
        auto i = utf_begin(src), j = i, e = utf_end(src);
        while (i != e) {
            str_skipws(i, e);
            if (i == e)
                break;
            if (*i == U'\"') {
                for (j = ++i; j != e;) {
                    j = std::find(j, e, U'\"');
                    if (j == e)
                        break;
                    ++j;
                    if (j == e || *j != U'\"') {
                        --j;
                        break;
                    }
                    ++j;
                }
                dst.push_back(str_replace(u_str(i, j), "\"\"", "\""));
                if (j == e)
                    break;
                ++j;
            } else {
                j = std::find_if(i, e, char_is_white_space);
                dst.push_back(u_str(i, j));
            }
            i = j;
        }
    }

}
