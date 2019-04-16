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

        Ustring make_command_error(const Ustring& details, const Ustring& arg, const Ustring& arg2) {
            Ustring msg = details;
            if (! arg.empty() || ! arg2.empty())
                msg += ": ";
            if (! arg.empty())
                msg += str_quote(arg);
            if (! arg.empty() && ! arg2.empty())
                msg += ", ";
            if (! arg2.empty())
                msg += str_quote(arg2);
            return msg;
        }

        Ustring make_spec_error(const Ustring& details, const Ustring& option) {
            Ustring msg = details;
            if (! option.empty())
                msg += ": --" + option;
            return msg;
        }

    }

    Options::command_error::command_error(const Ustring& details, const Ustring& arg, const Ustring& arg2):
    std::runtime_error(make_command_error(details, arg, arg2)) {}

    Options::spec_error::spec_error(const Ustring& details, const Ustring& option):
    std::runtime_error(make_spec_error(details, option)) {}

    Options& Options::add(const Ustring& info) {
        if (checked)
            throw spec_error("Option list is already complete");
        if (app_info.empty())
            app_info = str_trim(info);
        else
            opts.push_back(option_type(info));
        return *this;
    }

    Options& Options::add(help h) {
        if (checked)
            throw spec_error("Option list is already complete");
        if (help_flag != help::unset)
            throw spec_error("Multiple help options");
        help_flag = h;
        option_type opt;
        opt.is_boolean = true;
        opt.name = "help";
        opt.info = "Show usage information";
        if (find_index("h") == npos)
            opt.abbrev = "h";
        opts.push_back(opt);
        opt = {};
        opt.is_boolean = true;
        opt.name = "version";
        opt.info = "Show version information";
        if (find_index("v") == npos)
            opt.abbrev = "v";
        opts.push_back(opt);
        return *this;
    }

    Ustring Options::help_text() {
        static constexpr auto length_flags = Length::graphemes | Length::narrow;
        final_check();
        Ustring text = "\n" + app_info + "\n";
        Strings prefixes, suffixes;
        std::vector<size_t> lengths;
        for (auto& opt: opts) {
            Ustring prefix, suffix;
            size_t length = 0;
            if (! opt.name.empty()) {
                if (opt.is_anon)
                    prefix += "[";
                prefix += "--";
                if (opt.is_boolean && opt.defvalue == "1")
                    prefix += "no-";
                prefix += opt.name;
                if (! opt.abbrev.empty())
                    prefix += ", -"s + opt.abbrev;
                if (opt.is_anon)
                    prefix += "]";
                if (! opt.is_boolean) {
                    prefix += " <";
                    if (opt.is_file)           prefix += "file";
                    else if (opt.is_floating)  prefix += "float";
                    else if (opt.is_integer)   prefix += "int";
                    else if (opt.is_uinteger)  prefix += "uint";
                    else                       prefix += "arg";
                    prefix += ">";
                    if (opt.is_multi)
                        prefix += " ...";
                }
                length = str_length(prefix, length_flags);
                Strings extras;
                if (! opt.defvalue.empty() && ! opt.is_boolean && opt.info.find("default") == npos) {
                    Ustring defstr = "default ";
                    if (match_integer(opt.defvalue) || match_float(opt.defvalue))
                        defstr += opt.defvalue;
                    else
                        defstr += str_quote(opt.defvalue);
                    extras.push_back(defstr);
                }
                if (! opt.implies.empty())
                    extras.push_back("implies --" + opt.implies);
                if (opt.is_required)
                    extras.push_back("required");
                suffix = opt.info;
                if (! extras.empty()) {
                    if (suffix.back() == ')') {
                        suffix.pop_back();
                        suffix += "; ";
                    } else {
                        suffix += " (";
                    }
                    suffix += str_join(extras, "; ") + ")";
                }
            }
            if (! opt.enums.values.empty())
                suffix += "\n(" + str_join(opt.enums.values, ", ") + ")";
            prefixes.push_back(prefix);
            suffixes.push_back(suffix);
            lengths.push_back(length);
        }
        size_t maxlen = 0;
        if (! opts.empty())
            maxlen = *std::max_element(lengths.begin(), lengths.end());
        bool opthdr = false, was_info = true;
        for (size_t i = 0; i < opts.size(); ++i) {
            bool is_info = opts[i].name.empty();
            if (is_info || was_info)
                text += "\n";
            if (is_info) {
                text += opts[i].info + "\n";
            } else {
                if (! opthdr) {
                    text += "Options:\n\n";
                    opthdr = true;
                }
                Ustring prefix = str_pad_right(prefixes[i], maxlen, U' ', length_flags);
                auto lines = str_splitv_lines(suffixes[i]);
                text += "    " + prefix + "  = " + lines[0] + "\n";
                for (size_t j = 1; j < lines.size(); ++j)
                    text += Ustring(maxlen + 8, ' ') + lines[j] + "\n";
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
        str_trim_in(opt.name, "-"s + ascii_whitespace);
        Ustring tag = opt.name;
        if (str_length(opt.name) < 2 || std::any_of(utf_begin(opt.name), utf_end(opt.name), char_is_white_space))
            throw spec_error("Invalid option name", tag);
        bool neg = opt.name.substr(0, 3) == "no-";
        str_trim_in(opt.info);
        if (opt.info.empty())
            throw spec_error("No help message", tag);
        str_trim_in(opt.abbrev, "-");
        if (str_length(opt.abbrev) > 1)
            throw spec_error("Invalid abbreviation", tag);
        if (std::any_of(utf_begin(opt.abbrev), utf_end(opt.abbrev), char_is_white_space))
            throw spec_error("Invalid abbreviation", tag);
        if (opt.is_boolean && (opt.is_anon || opt.is_multi || opt.is_required
                || ! opt.enums.values.empty() || ! opt.pattern.empty()))
            throw spec_error("Incompatible option flags", tag);
        if ((opt.is_boolean || opt.is_required) && ! opt.defvalue.empty())
            throw spec_error("Default value is not allowed here", tag);
        if (neg && (! opt.is_boolean || ! opt.abbrev.empty()))
            throw spec_error("Incompatible option flags", tag);
        if (opt.is_required && ! opt.group.empty())
            throw spec_error("Incompatible option flags", tag);
        if (int(opt.is_boolean) + int(opt.is_file) + int(opt.is_floating) + int(opt.is_integer) + int(opt.is_uinteger)
                + int(! opt.enums.values.empty()) + int(! opt.pattern.empty()) > 1)
            throw spec_error("Incompatible option flags", tag);
        if (opt.is_floating)
            opt.pattern = match_float;
        else if (opt.is_integer)
            opt.pattern = match_integer;
        else if (opt.is_uinteger)
            opt.pattern = match_unsigned;
        if (! opt.defvalue.empty() && ! opt.pattern.empty() && ! opt.pattern(opt.defvalue))
            throw spec_error("Default value does not match pattern", tag);
        if (! opt.defvalue.empty() && ! opt.enums.values.empty() && ! opt.enums.check(opt.defvalue))
            throw spec_error("Default is not an enumeration value", tag);
        if (opt.is_anon && opt.is_multi && ++tail_opts > 1)
            throw spec_error("More than one option is claiming trailing arguments", tag);
        str_trim_in(opt.implies, "-"s + ascii_whitespace);
        if (opt.implies == opt.name)
            throw spec_error("Option implies itself", tag);
        if (neg) {
            opt.name.erase(0, 3);
            opt.defvalue = "1";
        }
        if (find_index(opt.name) != npos || find_index(opt.abbrev) != npos)
            throw spec_error("Duplicate option spec", tag);
        opts.push_back(opt);
    }

    void Options::final_check() {
        for (auto& opt: opts) {
            if (! opt.implies.empty()) {
                size_t i = find_index(opt.implies);
                if (i == npos)
                    throw spec_error("--$1 implies nonexistent option --$2"_fmt(opt.name, opt.implies));
                if (! opts[i].is_boolean || opts[i].defvalue == "1")
                    throw spec_error("Inconsistent linked options: --$1 cannot imply --$2"_fmt(opt.name, opt.implies));
            }
        }
        if (help_flag == help::unset)
            add(Options::help::std);
        checked = true;
    }

    size_t Options::find_index(Ustring name, bool require) const {
        str_trim_in(name, "-");
        if (name.substr(0, 3) == "no-")
            name.erase(0, 3);
        if (name.empty())
            return npos;
        auto i = std::find_if(opts.begin(), opts.end(),
            [=] (const auto& o) { return o.name == name || o.abbrev == name; });
        if (i != opts.end())
            return i - opts.begin();
        else if (require)
            throw spec_error("No such option", name);
        else
            return npos;
    }

    Strings Options::find_values(const Ustring& name) const {
        size_t i = find_index(name, true);
        return i != npos ? opts[i].values : Strings();
    }

    Options::parse_result Options::parse_args(Strings args, uint32_t flags) {
        final_check();
        clean_up_arguments(args, flags);
        if (args.empty() && help_flag == help::automatic)
            return parse_result::help;
        auto is_anon = parse_forced_anonymous(args);
        parse_attached_arguments(args);
        expand_abbreviations(args);
        extract_named_options(args);
        parse_remaining_anonymous(args, is_anon);
        if (has("help"))
            return parse_result::help;
        if (has("version"))
            return parse_result::version;
        check_conditions();
        supply_defaults();
        return parse_result::ok;
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
                        throw command_error("Unknown option: -" + arg);
                    args.insert(args.begin() + i + j, "--" + opts[o].name);
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
                throw command_error("Unknown option: --" + args[a]);
            auto& opt(opts[o]);
            if (opt.is_found && ! opt.is_multi)
                throw command_error("Duplicate option: --" + args[a]);
            if (! opt.group.empty())
                for (auto& opt2: opts)
                    if (&opt2 != &opt && opt2.group == opt.group && opt2.is_found)
                        throw command_error("Incompatible options: --$1, --$2"_fmt(opt2.name, opt.name));
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
            throw command_error("Unexpected argument: " + str_quote(args[0]));
    }

    void Options::check_conditions() {
        for (auto& opt: opts) {
            if (opt.is_found) {
                if (! opt.implies.empty()) {
                    size_t i = find_index(opt.implies);
                    if (opts[i].is_found && ! get_converted<bool>(str_join(opts[i].values, " ")))
                        throw command_error("Inconsistent options: --$1, --$2"_fmt(opt.name, opts[i].name));
                    opts[i].values.push_back("1");
                }
            } else {
                if (opt.is_required)
                    throw command_error("Required option is missing: --" + opt.name);
            }
        }
    }

    void Options::supply_defaults() {
        for (auto& opt: opts)
            if (opt.values.empty() && ! opt.defvalue.empty())
                add_arg_to_opt(opt.defvalue, opt);
    }

    void Options::send_help(std::ostream& out, parse_result mode) {
        Ustring message;
        switch (mode) {
            case parse_result::help:     message = help_text(); break;
            case parse_result::version:  message = app_info + '\n'; break;
            default:                     break;
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
        if ((opt.is_required || ! arg.empty())) {
            if (! opt.pattern.empty() && ! opt.pattern(arg))
                throw command_error("Invalid argument to --$1: $2q"_fmt(opt.name, arg));
            if (! opt.enums.values.empty() && ! opt.enums.check(arg))
                throw command_error("Invalid argument to --$1: $2q"_fmt(opt.name, arg));
        }
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
