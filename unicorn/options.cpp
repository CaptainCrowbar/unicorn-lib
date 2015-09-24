#include "unicorn/options.hpp"
#include "unicorn/format.hpp"
#include "unicorn/mbcs.hpp"
#include <iterator>

using namespace std::literals;
using namespace Unicorn::Literals;

namespace Unicorn {

    namespace {

        const auto match_integer = "[+-]?\\d+"_re;
        const auto match_float = "[+-]?(\\d+(\\.\\d*)?|\\.\\d+)(e[+-]?\\d+)?"_re_i;

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
                throw CommandLineError("Argument not recognised", arg);
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

    // Exceptions

    CommandLineError::CommandLineError(const u8string& details, const u8string& arg, const u8string& arg2):
    std::runtime_error(cmd_error(details, arg, arg2)) {}

    OptionSpecError::OptionSpecError(const u8string& option):
    std::runtime_error("Invalid option spec: $1q"_fmt(option)) {}

    OptionSpecError::OptionSpecError(const u8string& details, const u8string& option):
    std::runtime_error("$1: $2q"_fmt(details, option)) {}

    // Options class

    constexpr Kwarg<bool> Options::anon;
    constexpr Kwarg<bool> Options::boolean;
    constexpr Kwarg<bool> Options::floating;
    constexpr Kwarg<bool> Options::integer;
    constexpr Kwarg<bool> Options::multiple;
    constexpr Kwarg<bool> Options::required;
    constexpr Kwarg<u8string> Options::abbrev;
    constexpr Kwarg<u8string> Options::defval;
    constexpr Kwarg<u8string> Options::group;
    constexpr Kwarg<u8string> Options::pattern;

    u8string Options::help() const {
        u8string text = "\n$1\n\n"_fmt(app_info);
        if (! help_head.empty())
            text += "$1\n\n"_fmt(help_head);
        text += "Options:\n";
        string_list prefixes, suffixes;
        std::vector<size_t> lengths;
        for (auto& opt: opts) {
            u8string prefix;
            if (opt.is_anon)
                prefix += "[";
            prefix += "--$1"_fmt(opt.name);
            if (! opt.abbrev.empty())
                prefix += ", -$1"_fmt(opt.abbrev);
            if (opt.is_anon)
                prefix += "]";
            if (! opt.is_boolean) {
                prefix += " <";
                if (opt.is_float || opt.is_integer)
                    prefix += "num";
                else
                    prefix += "arg";
                prefix += ">";
                if (opt.is_multiple)
                    prefix += " ...";
            }
            u8string suffix = opt.info, extra;
            if (opt.is_required) {
                extra = "required";
            } else if (! opt.defval.empty() && opt.info.find("default") == npos) {
                extra = "default ";
                if (match_integer.match(opt.defval) || match_float.match(opt.defval))
                    extra += opt.defval;
                else
                    extra += "$1q"_fmt(opt.defval);
            }
            if (! extra.empty()) {
                if (suffix.back() == ')') {
                    suffix.pop_back();
                    suffix += "; ";
                } else {
                    suffix += " (";
                }
                suffix += extra + ")";
            }
            prefixes.push_back(prefix);
            suffixes.push_back(suffix);
            lengths.push_back(str_length(prefix, grapheme_units));
        }
        size_t maxlen = 0;
        if (! opts.empty())
            maxlen = *std::max_element(PRI_BOUNDS(lengths));
        for (size_t i = 0; i < opts.size(); ++i)
            text += "    $1  = $2\n"_fmt(str_pad_right(prefixes[i], maxlen, U' ', grapheme_units), suffixes[i]);
        if (opts.empty())
            text += "    None\n";
        text += "\n";
        if (! help_tail.empty())
            text += "$1\n\n"_fmt(help_tail);
        return text;
    }

    void Options::add_option(option_type opt) {
        u8string tag = opt.name;
        str_trim_in(opt.name, "-");
        if (str_length(opt.name) < 2)
            throw OptionSpecError(tag);
        str_trim_in(opt.info);
        if (opt.info.empty())
            throw OptionSpecError(tag);
        str_trim_in(opt.abbrev, "-");
        if (str_length(opt.abbrev) > 1
                || (! opt.abbrev.empty() && ! char_is_alphanumeric(*utf_begin(opt.abbrev)))
                || (opt.is_boolean && (opt.is_anon || opt.is_multiple || opt.is_required || ! opt.pattern.empty()))
                || ((opt.is_boolean || opt.is_required) && ! opt.defval.empty())
                || (opt.is_required && ! opt.group.empty())
                || (opt.is_float && opt.is_integer)
                || ((opt.is_float || opt.is_integer) > 0 && ! opt.pattern.empty()))
            throw OptionSpecError(tag);
        if (opt.is_integer)
            opt.pattern = match_integer;
        else if (opt.is_float)
            opt.pattern = match_float;
        if (! opt.defval.empty() && ! opt.pattern.empty() && ! opt.pattern.match(opt.defval))
            throw OptionSpecError(tag);
        if (find_index(opt.name) != npos || find_index(opt.abbrev) != npos)
            throw OptionSpecError("Duplicate option spec", tag);
        opts.push_back(opt);
    }

    size_t Options::find_index(u8string name, bool found) const {
        str_trim_in(name, "-");
        if (name.empty())
            return npos;
        auto i = std::find_if(PRI_BOUNDS(opts),
            [=] (const auto& o) { return o.name == name || o.abbrev == name; });
        if (i == opts.end() || (found && ! i->found))
            return npos;
        else
            return i - opts.begin();
    }

    Options::string_list Options::find_values(const u8string& name) const {
        size_t i = find_index(name);
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
            args.erase(std::begin(args));
        if (flags & opt_quoted)
            for (auto& arg: args)
                if (arg.size() >= 2 && arg.front() == '\"' && arg.back() == '\"')
                    arg = arg.substr(1, arg.size() - 1);
    }

    Options::string_list Options::parse_forced_anonymous(string_list& args) {
        string_list anon;
        auto i = std::find(PRI_BOUNDS(args), "--");
        if (i != std::end(args)) {
            anon.assign(i + 1, std::end(args));
            args.erase(i, std::end(args));
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
                    args.insert(std::begin(args) + i + 1, value);
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
                args.erase(std::begin(args) + i);
                size_t j = 0;
                for (auto u = std::next(utf_begin(arg)), uend = utf_end(arg); u != uend; ++u, ++j) {
                    size_t o = find_index(arg.substr(u.offset(), u.count()));
                    if (o == npos)
                        throw CommandLineError("Unknown option", arg);
                    args.insert(std::begin(args) + i + j, "--" + opts[o].name);
                }
                i += j;
            } else {
                ++i;
            }
        }
    }

    void Options::extract_named_options(string_list& args) {
        size_t i = 0;
        while (i < args.size()) {
            if (arg_type(args[i]) != is_long_option) {
                ++i;
                continue;
            }
            size_t o = find_index(args[i]);
            if (o == npos)
                throw CommandLineError("Unknown option", args[i]);
            auto& opt(opts[o]);
            if (opt.found && ! opt.is_multiple)
                throw CommandLineError("Duplicate option", args[i]);
            if (! opt.group.empty())
                for (auto& opt2: opts)
                    if (&opt2 != &opt && opt2.group == opt.group && opt2.found)
                        throw CommandLineError("Incompatible options", "--" + opt2.name, args[i]);
            opt.found = true;
            size_t n = 1, max_n = 1;
            if (opt.is_multiple)
                max_n = args.size() - i;
            else if (! opt.is_boolean)
                max_n = std::min(size_t(2), args.size() - i);
            for (; n < max_n && arg_type(args[i + n]) == is_argument; ++n) {
                auto& arg(args[i + n]);
                if (! opt.pattern.empty() && (opt.is_required || ! arg.empty()) && ! opt.pattern.match(arg))
                    throw CommandLineError("Invalid argument to option", args[i], arg);
                opt.values.push_back(arg);
            }
            args.erase(std::begin(args) + i, std::begin(args) + i + n);
        }
    }

    void Options::parse_remaining_anonymous(string_list& args, const string_list& anon) {
        args.insert(std::end(args), PRI_BOUNDS(anon));
        for (auto& opt: opts) {
            if (args.empty())
                break;
            if (opt.is_anon) {
                opt.found = true;
                if (opt.is_multiple) {
                    opt.values.insert(std::end(opt.values), PRI_BOUNDS(args));
                    args.clear();
                } else {
                    opt.values.push_back(args[0]);
                    args.erase(std::begin(args));
                }
            }
        }
        if (! args.empty())
            throw CommandLineError("Unexpected argument", args[0]);
    }

    void Options::check_required() {
        for (auto& opt: opts)
            if (opt.is_required && ! opt.found)
                throw CommandLineError("Required option is missing", "--" + opt.name);
    }

    void Options::supply_defaults() {
        for (auto& opt: opts) {
            if (opt.found && opt.is_boolean)
                opt.values.push_back("1");
            if (opt.values.empty() && ! opt.defval.empty())
                opt.values.push_back(opt.defval);
        }
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
