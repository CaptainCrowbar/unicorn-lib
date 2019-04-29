#include "unicorn/options.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include <iterator>

#ifdef _XOPEN_SOURCE
    #include <unistd.h>
#else
    #include <io.h>
    #define isatty _isatty
#endif

using namespace RS::Unicorn::Literals;
using namespace std::literals;

namespace RS::Unicorn {

    namespace {

        const Ustring integer_pattern   = "0[Xx][[:xdigit:]]+|[+-]?\\d+";
        const Ustring unsigned_pattern  = "0[Xx][[:xdigit:]]+|\\d+";
        const Ustring float_pattern     = "[+-]?(\\d+(\\.\\d*)?|\\.\\d+)([Ee][+-]?\\d+)?";
        const Ustring integer_si_unit   = "( ?[KkMGTPEZY]\\w*)?";
        const Ustring float_si_unit     = "( ?[KkMGTPEZYmunpfazy]\\w*)?";

        const Regex match_integer(integer_pattern, Regex::full | Regex::optimize);
        const Regex match_integer_si(integer_pattern + integer_si_unit, Regex::full | Regex::optimize);
        const Regex match_unsigned(unsigned_pattern, Regex::full | Regex::optimize);
        const Regex match_unsigned_si(unsigned_pattern + integer_si_unit, Regex::full | Regex::optimize);
        const Regex match_float(float_pattern, Regex::full | Regex::optimize);
        const Regex match_float_si(float_pattern + float_si_unit, Regex::full | Regex::optimize);

        const Ustring xt_reset   = "\x1b[0m";
        const Ustring xt_bold    = "\x1b[1m";
        const Ustring xt_unbold  = "\x1b[22m";

        enum class ArgType { argument, long_option, short_option, multiple_short_options };

        ArgType arg_type(const Ustring& arg) {
            size_t length = str_length(arg);
            if (length < 2 || arg[0] != '-' || ascii_isdigit(arg[1]))
                return ArgType::argument;
            else if (length == 2 && arg[1] != '-')
                return ArgType::short_option;
            else if (arg[1] == '-')
                return ArgType::long_option;
            else
                return ArgType::multiple_short_options;
        }

        Ustring xt_colour(int r, int g, int b) {
            int n = 36 * r + 6 * g + b + 16;
            return "\x1b[38;5;" + std::to_string(n) + "m";
        }

    }

    Options& Options::add(const Ustring& info) {
        if (checked)
            throw spec_error("Option list is already complete");
        if (app_info.empty())
            app_info = str_trim(info);
        else if (opts.empty())
            app_info += "\n\n" + str_trim(info);
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
        if (! find_if_exists("h"))
            opt.abbrev = "h";
        opts.push_back(opt);
        opt = {};
        opt.is_boolean = true;
        opt.name = "version";
        opt.info = "Show version information";
        if (! find_if_exists("v"))
            opt.abbrev = "v";
        opts.push_back(opt);
        return *this;
    }

    Ustring Options::help_text(uint32_t flags) {
        static constexpr auto length_flags = Length::graphemes | Length::narrow;
        setup_finalize();
        Ustring bold, unbold, reset, head, body, optc, argc, desc;
        if (flags & colour) {
            bold = xt_bold;
            unbold = xt_unbold;
            reset = xt_reset;
            head = xt_colour(5,5,1);
            body = xt_colour(5,5,3);
            optc = xt_colour(1,5,1);
            argc = xt_colour(3,5,4);
            desc = xt_colour(2,4,5);
        }
        Ustring text = "\n" + head + bold + app_info + reset + "\n";
        auto para = text.find("\n\n");
        if (para != npos)
            text.insert(para + 2, reset + body);
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
                if (! opt.prereq.empty())
                    extras.push_back("requires --" + opt.prereq);
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
        bool opt_hdr = str_ends_with(app_info, ":");
        bool was_info = true;
        for (size_t i = 0; i < opts.size(); ++i) {
            bool is_info = opts[i].name.empty();
            if (is_info || was_info)
                text += "\n";
            if (is_info) {
                text += body + opts[i].info + reset + "\n";
            } else {
                if (! opt_hdr) {
                    text += body + "Options:" + reset + "\n\n";
                    opt_hdr = true;
                }
                Ustring prefix = str_pad_right(prefixes[i], maxlen, U' ', length_flags);
                size_t lt = prefix.find('<');
                if (lt == npos)
                    prefix += unbold;
                else
                    prefix.insert(lt, unbold + argc);
                auto lines = str_splitv_lines(suffixes[i]);
                text += "    " + optc + bold + prefix + desc + "  = " + lines[0] + reset + "\n";
                for (size_t j = 1; j < lines.size(); ++j)
                    text += Ustring(maxlen + 8, ' ') + desc + lines[j] + reset + "\n";
            }
            was_info = is_info;
        }
        if (! opt_hdr) {
            if (was_info)
                text += "\n";
            text += body + "Options:\n\n    None" + reset + "\n";
        }
        text += "\n";
        return text;
    }

    bool Options::has(const Ustring& name) const {
        return ! name.empty() && opts[find_runtime_option(name)].is_found;
    }

    void Options::setup_add_option(option_type opt) {
        static const Ustring trim_chars = "-"s + ascii_whitespace;
        str_trim_in(opt.name, trim_chars);
        str_trim_in(opt.info);
        str_trim_in(opt.abbrev, trim_chars);
        str_trim_in(opt.implies, trim_chars);
        str_trim_in(opt.prereq, trim_chars);
        Ustring tag = opt.name;
        if (str_length(opt.name) < 2 || std::any_of(utf_begin(opt.name), utf_end(opt.name), char_is_white_space))
            throw spec_error("Invalid option name: --" + tag);
        bool neg = opt.name.substr(0, 3) == "no-";
        if (opt.info.empty())
            throw spec_error("No help message: --" + tag);
        if (str_length(opt.abbrev) > 1)
            throw spec_error("Invalid abbreviation: -" + opt.abbrev);
        if (std::any_of(utf_begin(opt.abbrev), utf_end(opt.abbrev), char_is_white_space))
            throw spec_error("Invalid abbreviation: -" + opt.abbrev);
        if (opt.is_boolean && (opt.is_anon || opt.is_multi || opt.is_required
                || ! opt.enums.values.empty() || ! opt.pattern.empty()))
            throw spec_error("Incompatible option flags: --" + tag);
        if ((opt.is_boolean || opt.is_required) && ! opt.defvalue.empty())
            throw spec_error("Default value is not allowed here: --" + tag);
        if (neg && (! opt.is_boolean || ! opt.abbrev.empty()))
            throw spec_error("Incompatible option flags: --" + tag);
        if (opt.is_required && (! opt.group.empty() || ! opt.prereq.empty()))
            throw spec_error("Incompatible option flags: --" + tag);
        if (int(opt.is_boolean) + int(opt.is_file) + int(opt.is_floating) + int(opt.is_integer) + int(opt.is_uinteger)
                + int(! opt.enums.values.empty()) + int(! opt.pattern.empty()) > 1)
            throw spec_error("Incompatible option flags: --" + tag);
        if (opt.is_si && (opt.is_boolean || opt.is_file || ! opt.enums.values.empty()))
            throw spec_error("Incompatible option flags: --" + tag);
        if (opt.is_floating)
            opt.pattern = opt.is_si ? match_float_si : match_float;
        else if (opt.is_integer)
            opt.pattern = opt.is_si ? match_integer_si : match_integer;
        else if (opt.is_uinteger)
            opt.pattern = opt.is_si ? match_unsigned_si : match_unsigned;
        if (! opt.defvalue.empty() && ! opt.pattern.empty() && ! opt.pattern(opt.defvalue))
            throw spec_error("Default value does not match pattern: --" + tag);
        if (! opt.defvalue.empty() && ! opt.enums.values.empty() && ! opt.enums.check(opt.defvalue))
            throw spec_error("Default is not an enumeration value: --" + tag);
        if (opt.is_anon && opt.is_multi && ++tail_opts > 1)
            throw spec_error("More than one option is claiming trailing arguments: --" + tag);
        if (opt.implies == opt.name)
            throw spec_error("Option implies itself: --" + tag);
        if (opt.prereq == opt.name)
            throw spec_error("Option requires itself: --" + tag);
        if (neg) {
            opt.name.erase(0, 3);
            opt.defvalue = "1";
        }
        for (auto& other: opts)
            if (opt.name == other.name || (! opt.abbrev.empty() && opt.abbrev == other.abbrev))
                if (opt.prereq.empty() || other.prereq.empty() || opt.prereq == other.prereq)
                    throw spec_error("Duplicate option: --" + tag);
        opts.push_back(opt);
    }

    void Options::setup_finalize() {
        for (auto& opt: opts) {
            if (! opt.implies.empty()) {
                size_t i = find_first_index(opt.implies);
                if (i == npos)
                    throw spec_error("--$1 implies nonexistent option --$2"_fmt(opt.name, opt.implies));
                if (! opts[i].is_boolean || opts[i].defvalue == "1")
                    throw spec_error("Inconsistent linked options: --$1 cannot imply --$2"_fmt(opt.name, opt.implies));
            }
            if (! opt.prereq.empty() && ! find_if_exists(opt.prereq))
                throw spec_error("Prerequisite of --$1 does not exist: --$2"_fmt(opt.name, opt.prereq));
        }
        if (help_flag == help::unset)
            add(Options::help::std);
        checked = true;
    }

    size_t Options::find_first_index(const Ustring& name) const {
        auto key = str_trim(name, "-");
        str_drop_prefix_in(key, "no-");
        if (key.empty())
            return npos;
        auto i = std::find_if(opts.begin(), opts.end(),
            [=] (const auto& o) { return o.name == key || o.abbrev == key; });
        if (i == opts.end())
            return npos;
        else
            return i - opts.begin();
    }

    bool Options::find_if_exists(const Ustring& name) const {
        return find_first_index(name) != npos;
    }

    size_t Options::find_runtime_option(const Ustring& name) const {
        auto key = str_trim(name, "-");
        str_drop_prefix_in(key, "no-");
        auto begin = opts.begin(), end = opts.end(), i = begin;
        Strings prereqs;
        for (;;) {
            i = std::find_if(i, end, [=] (const auto& o) { return o.name == key || o.abbrev == key; });
            if (i == end)
                break;
            auto& pre = i->prereq;
            if (pre.empty() || has(pre))
                return i - begin;
            prereqs.push_back(pre);
            ++i;
        }
        if (str_length(key) == 1)
            key.insert(0, 1, '-');
        else
            key.insert(0, 2, '-');
        if (prereqs.empty())
            throw spec_error("Unknown option: " + key);
        else
            throw command_error("Option $1 requires --$2"_fmt(key, str_join(prereqs, ", --")));
    }

    bool Options::parse_main(Strings args, std::ostream& out, uint32_t flags) {
        uint32_t cflags = flags & (colour | nocolour), hflags = 0;
        if (cflags == colour || (cflags != nocolour && &out == &std::cout && isatty(1) != 0))
            hflags = colour;
        setup_finalize();
        parse_initial_cleanup(args, flags);
        if (args.empty() && help_flag == help::automatic) {
            out << help_text(hflags);
            return true;
        }
        auto anon_args = parse_explicit_anonymous(args);
        parse_attached_arguments(args);
        parse_named_options(args);
        parse_remaining_anonymous(args, anon_args);
        if (get<bool>("help")) {
            out << help_text(hflags);
            return true;
        } else if (get<bool>("version")) {
            out << app_info << '\n';
            return true;
        } else {
            parse_check_conditions();
            parse_supply_defaults();
            return false;
        }
    }

    void Options::parse_initial_cleanup(Strings& args, uint32_t flags) {
        if (! (flags & noprefix) && ! args.empty())
            args.erase(args.begin());
        if (flags & quoted)
            for (auto& arg: args)
                if (arg.size() >= 2 && arg.front() == '\"' && arg.back() == '\"')
                    arg = arg.substr(1, arg.size() - 1);
    }

    Strings Options::parse_explicit_anonymous(Strings& args) {
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
            if (arg_type(args[i]) != ArgType::argument) {
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

    void Options::parse_named_options(Strings& args) {
        size_t i = 0;
        while (i < args.size()) {
            auto type = arg_type(args[i]);
            if (type == ArgType::multiple_short_options) {
                auto arg = args[i];
                args.erase(args.begin() + i);
                size_t j = 0;
                for (auto u = std::next(utf_begin(arg)), uend = utf_end(arg); u != uend; ++u, ++j)
                    args.insert(args.begin() + i + j, "-" + u.str());
                continue;
            }
            if (type == ArgType::argument) {
                ++i;
                continue;
            }
            auto& opt = opts[find_runtime_option(args[i])];
            if (opt.is_found && ! opt.is_multi)
                throw command_error("Duplicate option: --" + args[i]);
            if (! opt.group.empty())
                for (auto& opt2: opts)
                    if (&opt2 != &opt && opt2.group == opt.group && opt2.is_found)
                        throw command_error("Incompatible options: --$1, --$2"_fmt(opt2.name, opt.name));
            opt.is_found = true;
            if (opt.is_boolean) {
                if (args[i].substr(0, 5) == "--no-")
                    opt.add_arg("0");
                else
                    opt.add_arg("1");
                args.erase(args.begin() + i);
            } else {
                size_t n = 1, max_n = 1;
                if (opt.is_multi)
                    max_n = args.size() - i;
                else
                    max_n = std::min(size_t(2), args.size() - i);
                for (; n < max_n && arg_type(args[i + n]) == ArgType::argument; ++n)
                    opt.add_arg(args[i + n]);
                args.erase(args.begin() + i, args.begin() + i + n);
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
                opt.add_arg(args[i]);
            args.erase(args.begin(), args.begin() + n);
        }
        if (! args.empty())
            throw command_error("Unexpected argument: " + str_quote(args[0]));
    }

    void Options::parse_check_conditions() {
        for (auto& opt: opts) {
            if (opt.is_found) {
                if (! opt.implies.empty()) {
                    auto& target = opts[find_runtime_option(opt.implies)];
                    if (target.is_found && ! from_str<bool>(target.values[0]))
                        throw command_error("Inconsistent options: --$1, --no-$2"_fmt(opt.name, opt.implies));
                    target.values.push_back("1");
                }
            } else {
                if (opt.is_required)
                    throw command_error("Required option is missing: --" + opt.name);
            }
        }
    }

    void Options::parse_supply_defaults() {
        for (auto& opt: opts)
            if (opt.values.empty() && ! opt.defvalue.empty())
                opt.add_arg(opt.defvalue);
    }

    Ustring Options::arg_convert(const std::string& str, uint32_t flags) {
        if (! (flags & locale))
            return str;
        Ustring utf8;
        import_string(str, utf8, local_encoding());
        return utf8;
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

    Options::option_type::option_type(const Ustring& inf) {
        info = str_trim_right(inf);
        if (info.empty())
            throw spec_error("Empty information string");
    }

    void Options::option_type::add_arg(const Ustring& arg) {
        if ((is_required || ! arg.empty())) {
            if (! pattern.empty() && ! pattern(arg))
                throw command_error("Invalid argument to --$1: $2q"_fmt(name, arg));
            if (! enums.values.empty() && ! enums.check(arg))
                throw command_error("Invalid argument to --$1: $2q"_fmt(name, arg));
        }
        values.push_back(arg);
    }

}
