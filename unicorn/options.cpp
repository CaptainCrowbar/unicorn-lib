#include "unicorn/options.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include <iterator>

using namespace std::literals;
using namespace Unicorn::Literals;

namespace Unicorn {

    namespace {

        const auto match_integer = "0x[[:xdigit:]]+|[+-]?\\d+((\\.\\d+)? ?[kmgtpezy]\\w*)?"_re_i;
        const auto match_uinteger = "0x[[:xdigit:]]+|\\d+((\\.\\d+)? ?[kmgtpezy]\\w*)?"_re_i;
        const auto match_float = "[+-]?(\\d+(\\.\\d*)?|\\.\\d+)(e[+-]?\\d+)?( ?[kmgtpezy]\\w*)?"_re_i;

        enum ArgType {
            is_argument = 'a',
            is_long_option = 'l',
            is_short_option = 's',
        };

        ArgType arg_type(const u8string& arg) {
            if (arg.size() < 2 || arg[0] != '-' || ascii_isdigit(arg[1]))
                return is_argument;
            else if (arg[1] != '-' && arg[1] != '=')
                return is_short_option;
            else if (arg.size() >= 3 && arg[1] == '-' && arg[2] != '-' && arg[2] != '=')
                return is_long_option;
            else
                throw Options::CommandError("Argument not recognised", arg);
        }

        u8string cmd_error(const u8string& details, const u8string& arg, const u8string& arg2) {
            u8string msg = details;
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

    Options::CommandError::CommandError(const u8string& details, const u8string& arg, const u8string& arg2):
    std::runtime_error(cmd_error(details, arg, arg2)) {}

    Options::SpecError::SpecError(const u8string& option):
    std::runtime_error("Invalid option spec: $1q"_fmt(option)) {}

    Options::SpecError::SpecError(const u8string& details, const u8string& option):
    std::runtime_error("$1: $2q"_fmt(details, option)) {}

    Options& Options::add(const u8string& info) {
        if (info.empty())
            throw SpecError("Empty string");
        option_type opt;
        opt.info = str_trim_right(info);
        opts.push_back(opt);
        return *this;
    }

    u8string Options::help() const {
        static constexpr auto length_flags = grapheme_units | narrow_context;
        u8string text = "\n" + app_info + "\n";
        string_list prefixes, suffixes;
        vector<size_t> lengths;
        for (auto& opt: opts) {
            u8string prefix, suffix;
            size_t length = 0;
            if (! opt.name.empty()) {
                if (opt.is_anon)
                    prefix += "[";
                prefix += "--";
                if (opt.is_boolean && opt.defval == "1")
                    prefix += "no-";
                prefix += opt.name;
                if (! opt.abbrev.empty())
                    prefix += ", -"s + opt.abbrev;
                if (opt.is_anon)
                    prefix += "]";
                if (! opt.is_boolean) {
                    prefix += " <";
                    if (opt.is_integer || opt.is_uinteger || opt.is_float)
                        prefix += "num";
                    else
                        prefix += "arg";
                    prefix += ">";
                    if (opt.is_multiple)
                        prefix += " ...";
                }
                length = str_length(prefix, length_flags);
                u8string extra;
                if (opt.is_required) {
                    extra = "required";
                } else if (! opt.defval.empty() && ! opt.is_boolean && opt.info.find("default") == npos) {
                    extra = "default ";
                    if (match_integer.match(opt.defval) || match_float.match(opt.defval))
                        extra += opt.defval;
                    else
                        extra += "$1q"_fmt(opt.defval);
                }
                suffix = opt.info;
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
            bool is_info = opts[i].name.empty();
            if (is_info || was_info)
                text += "\n";
            if (is_info) {
                text += opts[i].info + "\n";
            } else {
                if (! opthdr) {
                    text += "Options:\n";
                    opthdr = true;
                }
                u8string prefix = str_pad_right(prefixes[i], maxlen, U' ', length_flags);
                text += "    $1  = $2\n"_fmt(prefix, suffixes[i]);
            }
            was_info = is_info;
        }
        if (! opthdr) {
            if (was_info)
                text += "\n";
            text += "Options:\n    None\n";
        }
        text += "\n";
        return text;
    }

    bool Options::has(const u8string& name) const {
        size_t i = find_index(name, true);
        return i != npos && opts[i].found;
    }

    void Options::add_option(option_type opt) {
        u8string tag = opt.name;
        str_trim_in(opt.name, "-");
        if (str_length(opt.name) < 2)
            throw SpecError(tag);
        bool neg = opt.name.substr(0, 3) == "no-";
        str_trim_in(opt.info);
        if (opt.info.empty())
            throw SpecError(tag);
        str_trim_in(opt.abbrev, "-");
        if (str_length(opt.abbrev) > 1
                || (! opt.abbrev.empty() && ! char_is_alphanumeric(*utf_begin(opt.abbrev)))
                || (opt.is_boolean && (opt.is_anon || opt.is_multiple || opt.is_required || ! opt.pattern.empty()))
                || ((opt.is_boolean || opt.is_required) && ! opt.defval.empty())
                || (neg && (! opt.is_boolean || ! opt.abbrev.empty()))
                || (opt.is_required && ! opt.group.empty())
                || (int(opt.is_integer) + int(opt.is_uinteger) + int(opt.is_float) + int(! opt.pattern.empty()) > 1))
            throw SpecError(tag);
        if (opt.is_integer)
            opt.pattern = match_integer;
        else if (opt.is_uinteger)
            opt.pattern = match_uinteger;
        else if (opt.is_float)
            opt.pattern = match_float;
        if (! opt.defval.empty() && ! opt.pattern.empty() && ! opt.pattern.match(opt.defval))
            throw SpecError(tag);
        if (neg) {
            opt.name.erase(0, 3);
            opt.defval = "1";
        }
        if (find_index(opt.name) != npos || find_index(opt.abbrev) != npos)
            throw SpecError("Duplicate option spec", tag);
        opts.push_back(opt);
    }

    size_t Options::find_index(u8string name, bool require) const {
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
            throw SpecError("--" + name);
        else
            return npos;
    }

    Options::string_list Options::find_values(const u8string& name) const {
        size_t i = find_index(name, true);
        return i != npos ? opts[i].values : string_list();
    }

    Options::help_mode Options::parse_args(string_list args, uint32_t flags) {
        add_help_version();
        clean_up_arguments(args, flags);
        if (help_auto && args.empty())
            return help_mode::usage;
        auto anon = parse_forced_anonymous(args);
        parse_attached_arguments(args);
        expand_abbreviations(args);
        extract_named_options(args);
        parse_remaining_anonymous(args, anon);
        if (has("help"))
            return help_mode::usage;
        if (has("version"))
            return help_mode::version;
        check_required();
        supply_defaults();
        return help_mode::none;
    }

    void Options::add_help_version() {
        if (find_index("help") == npos) {
            option_type opt;
            opt.name = "help";
            opt.info = "Show usage information";
            opt.is_boolean = true;
            if (find_index("h") == npos)
                opt.abbrev = "h";
            opts.push_back(opt);
        }
        if (find_index("version") == npos) {
            option_type opt;
            opt.name = "version";
            opt.info = "Show version information";
            opt.is_boolean = true;
            if (find_index("v") == npos)
                opt.abbrev = "v";
            opts.push_back(opt);
        }
    }

    void Options::clean_up_arguments(string_list& args, uint32_t flags) {
        if (! (flags & opt_noprefix) && ! args.empty())
            args.erase(args.begin());
        if (flags & opt_quoted)
            for (auto& arg: args)
                if (arg.size() >= 2 && arg.front() == '\"' && arg.back() == '\"')
                    arg = arg.substr(1, arg.size() - 1);
    }

    Options::string_list Options::parse_forced_anonymous(string_list& args) {
        string_list anon;
        auto i = std::find(args.begin(), args.end(), "--");
        if (i != args.end()) {
            anon.assign(i + 1, args.end());
            args.erase(i, args.end());
        }
        return anon;
    }

    void Options::parse_attached_arguments(string_list& args) {
        size_t i = 0;
        while (i < args.size()) {
            if (arg_type(args[i]) != is_argument) {
                u8string key, value;
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

    void Options::expand_abbreviations(string_list& args) {
        size_t i = 0;
        while (i < args.size()) {
            if (arg_type(args[i]) == is_short_option) {
                auto arg = args[i];
                args.erase(args.begin() + i);
                size_t j = 0;
                for (auto u = std::next(utf_begin(arg)), uend = utf_end(arg); u != uend; ++u, ++j) {
                    size_t o = find_index(arg.substr(u.offset(), u.count()));
                    if (o == npos)
                        throw CommandError("Unknown option", arg);
                    args.insert(args.begin() + i + j, "--" + opts[o].name);
                }
                i += j;
            } else {
                ++i;
            }
        }
    }

    void Options::extract_named_options(string_list& args) {
        size_t a = 0;
        while (a < args.size()) {
            if (arg_type(args[a]) != is_long_option) {
                ++a;
                continue;
            }
            size_t o = find_index(args[a]);
            if (o == npos)
                throw CommandError("Unknown option", args[a]);
            auto& opt(opts[o]);
            if (opt.found && ! opt.is_multiple)
                throw CommandError("Duplicate option", args[a]);
            if (! opt.group.empty())
                for (auto& opt2: opts)
                    if (&opt2 != &opt && opt2.group == opt.group && opt2.found)
                        throw CommandError("Incompatible options", "--" + opt2.name, args[a]);
            opt.found = true;
            if (opt.is_boolean) {
                if (args[a].substr(0, 5) == "--no-")
                    opt.values.push_back("0");
                else
                    opt.values.push_back("1");
                args.erase(args.begin() + a);
            } else {
                size_t n = 1, max_n = 1;
                if (opt.is_multiple)
                    max_n = args.size() - a;
                else
                    max_n = std::min(size_t(2), args.size() - a);
                for (; n < max_n && arg_type(args[a + n]) == is_argument; ++n) {
                    auto& arg(args[a + n]);
                    if (! opt.pattern.empty() && (opt.is_required || ! arg.empty()) && ! opt.pattern.match(arg))
                        throw CommandError("Invalid argument to option", args[a], arg);
                    opt.values.push_back(arg);
                }
                args.erase(args.begin() + a, args.begin() + a + n);
            }
        }
    }

    void Options::parse_remaining_anonymous(string_list& args, const string_list& anon) {
        args.insert(args.end(), anon.begin(), anon.end());
        for (auto& opt: opts) {
            if (args.empty())
                break;
            if (opt.is_anon) {
                opt.found = true;
                if (opt.is_multiple) {
                    opt.values.insert(opt.values.end(), args.begin(), args.end());
                    args.clear();
                } else {
                    opt.values.push_back(args[0]);
                    args.erase(args.begin());
                }
            }
        }
        if (! args.empty())
            throw CommandError("Unexpected argument", args[0]);
    }

    void Options::check_required() {
        for (auto& opt: opts)
            if (opt.is_required && ! opt.found)
                throw CommandError("Required option is missing", "--" + opt.name);
    }

    void Options::supply_defaults() {
        for (auto& opt: opts)
            if (opt.values.empty() && ! opt.defval.empty())
                opt.values.push_back(opt.defval);
    }

    void Options::send_help(std::ostream& out, help_mode mode) const {
        u8string message;
        switch (mode) {
            case help_mode::version:  message = app_info + '\n'; break;
            case help_mode::usage:    message = help(); break;
            default:                  break;
        }
        if (! message.empty())
            out << message;
    }

    u8string Options::arg_convert(const string& str, uint32_t flags) {
        if (! (flags & opt_locale))
            return str;
        u8string utf8;
        import_string(str, utf8, local_encoding());
        return utf8;
    }

    void Options::unquote(const u8string& src, string_list& dst) {
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
