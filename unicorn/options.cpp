#include "unicorn/options.hpp"
#include "unicorn/mbcs.hpp"

using namespace std::literals;
using namespace Unicorn::Literals;

namespace Unicorn {

    namespace {

        enum ArgType {
            is_argument = 'a',
            is_long_option = 'l',
            is_short_option = 's',
        };

        ArgType arg_type(const u8string& arg) {
            if (arg.size() < 2 || arg[0] != '-' || Crow::ascii_isdigit(arg[1]))
                return is_argument;
            else if (arg[1] != '-' && arg[1] != '=')
                return is_short_option;
            else if (arg.size() >= 3 && arg[1] == '-' && arg[2] != '-' && arg[2] != '=')
                return is_long_option;
            else
                throw CommandLineError("Argument not recognised", arg);
        }

        u8string maybe_quote(const u8string& str) {
            static const auto is_alnum = gc_predicate("L,N");
            for (auto& c: utf_range(str))
                if (! is_alnum(c))
                    return "${1:q}"_fmt(str);
            return str;
        }

    }

    // Exceptions

    u8string CommandLineError::assemble(const u8string& details, const u8string& arg1, const u8string& arg2) {
        u8string s = details;
        if (! arg1.empty())
            s += "; ${1:q}"_fmt(arg1);
        if (! arg2.empty())
            s += "; ${1:q}"_fmt(arg2);
        return s;
    }

    u8string OptionSpecError::assemble(const u8string& details, const u8string& option) {
        u8string s = details;
        if (! option.empty()) {
            s += ": ";
            s += option;
        }
        return s;
    }

    // Options parsing class

    u8string Options::help() const {
        static const u8string indent(4, ' ');
        u8string text = "\n";
        text += app_info;
        text += "\n\n";
        if (! help_head.empty()) {
            text += help_head;
            text += "\n\n";
        }
        text += "Options:\n";
        std::vector<u8string> prefixes, suffixes;
        std::vector<size_t> lengths;
        for (auto& opt: opts) {
            u8string prefix;
            if (opt.flags.get(opt_anon))
                prefix += "[";
            prefix += opt.name;
            if (! opt.abbrev.empty()) {
                prefix += ", ";
                prefix += opt.abbrev;
            }
            if (opt.flags.get(opt_anon))
                prefix += "]";
            if (! opt.flags.get(opt_boolean)) {
                prefix += " <";
                if (opt.argtype.empty())
                    prefix += "arg";
                else
                    prefix += opt.argtype;
                prefix += ">";
                if (opt.flags.get(opt_multiple))
                    prefix += " ...";
            }
            u8string suffix = opt.info, extra;
            if (opt.flags.get(opt_required))
                extra = "required";
            else if (! opt.defval.empty() && opt.info.find("default") == npos)
                extra = "default " + maybe_quote(opt.defval);
            if (! extra.empty()) {
                if (*(std::end(suffix) - 1) == ')') {
                    suffix.pop_back();
                    suffix += "; ";
                } else {
                    suffix += " (";
                }
                suffix += extra;
                suffix += ")";
            }
            prefixes.push_back(prefix);
            suffixes.push_back(suffix);
            lengths.push_back(str_length(prefix, grapheme_units));
        }
        size_t maxlen = 0;
        if (! opts.empty())
            maxlen = *std::max_element(CROW_BOUNDS(lengths));
        for (size_t i = 0; i < opts.size(); ++i) {
            text += indent;
            text += prefixes[i];
            text.append(maxlen - lengths[i], ' ');
            text += "  = ";
            text += suffixes[i];
            text += "\n";
        }
        if (opts.empty()) {
            text += indent;
            text += "None\n";
        }
        text += "\n";
        if (! help_tail.empty()) {
            text += help_tail;
            text += "\n\n";
        }
        return text;
    }

    void Options::add_option(const u8string& name, const u8string& abbrev, const u8string& info, Crow::Flagset flags,
            const u8string& defval, const u8string& pattern, const u8string& group) {
        static const auto match_float = "/[+-]?(\\d+(\\.\\d*)?|\\.\\d+)(e[+-]?\\d+)?/i"_re;
        static const auto match_integer = "/[+-]?\\d+/"_re;
        flags.allow(opt_anon | opt_boolean | opt_float | opt_integer | opt_multiple | opt_required,
            "command line option");
        option_type opt;
        opt.name = "--" + str_trim(name, "-");
        if (abbrev[0])
            opt.abbrev = "-" + abbrev;
        opt.info = info;
        opt.flags = flags;
        opt.defval = defval;
        if (! pattern.empty())
            opt.pattern = Regex(pattern);
        opt.group = group;
        auto formats = static_cast<int>(opt.flags.get(opt_float)) + static_cast<int>(opt.flags.get(opt_integer));
        if (opt.name == "--"
                || (abbrev[0] && ! char_is_alphanumeric(*utf_begin(abbrev)))
                || opt.info.empty()
                || (opt.flags.get(opt_boolean)
                    && (opt.flags.get(opt_anon | opt_multiple | opt_required) || ! opt.pattern.empty()))
                || ((opt.flags.get(opt_boolean | opt_required)) && ! opt.defval.empty())
                || (opt.flags.get(opt_required) && ! opt.group.empty())
                || formats > 1
                || (formats > 0 && ! opt.pattern.empty()))
            throw OptionSpecError("Invalid option spec", opt.name);
        if (opt.flags.get(opt_float)) {
            opt.pattern = match_float;
            opt.argtype = "num";
        } else if (opt.flags.get(opt_integer)) {
            opt.pattern = match_integer;
            opt.argtype = "num";
        }
        if (! opt.defval.empty() && ! opt.pattern.empty() && ! opt.pattern.match(opt.defval))
            throw OptionSpecError("Invalid option spec", opt.name);
        if (opt_index(opt.name) != npos || opt_index(opt.abbrev) != npos)
            throw OptionSpecError("Duplicate option spec", opt.name);
        opts.push_back(opt);
    }

    size_t Options::opt_index(const u8string& name, bool found) const {
        if (name.empty())
            return npos;
        u8string optname = str_starts_with(name, "-") ? name : "--" + name;
        for (size_t i = 0; i < opts.size(); ++i)
            if (opts[i].name == optname || opts[i].abbrev == optname)
                return found && ! opts[i].found ? npos : i;
        return npos;
    }

    std::vector<u8string> Options::opt_values(const u8string& name) const {
        size_t i = opt_index(name);
        return i != npos ? opts[i].values : std::vector<u8string>();
    }

    void Options::parse_args(std::vector<u8string> args, Crow::Flagset flags) {
        // Add the help and version options
        if (opt_index("--help") == npos) {
            option_type opt;
            opt.name = "--help";
            if (opt_index("-h") == npos)
                opt.abbrev = "-h";
            opt.info = "Show usage information";
            opt.flags = opt_boolean;
            opts.push_back(opt);
        }
        if (opt_index("--version") == npos) {
            option_type opt;
            opt.name = "--version";
            if (opt_index("-v") == npos)
                opt.abbrev = "-v";
            opt.info = "Show version information";
            opt.flags = opt_boolean;
            opts.push_back(opt);
        }
        // Clean up the argument list
        if (! flags.get(opt_noprefix) && ! args.empty())
            args.erase(std::begin(args));
        if (flags.get(opt_quoted))
            for (auto& arg: args)
                if (arg.size() >= 2 && arg.front() == '\"' && arg.back() == '\"')
                    arg = arg.substr(1, arg.size() - 1);
        if (help_auto && args.empty())
            throw HelpRequest(help());
        // Split off any forced anonymous options
        std::vector<u8string> anon;
        auto it(std::find(CROW_BOUNDS(args), "--"));
        if (it != std::end(args)) {
            anon.assign(it + 1, std::end(args));
            args.erase(it, std::end(args));
        }
        // Split up any options with an argument attached
        size_t i = 0;
        while (i < args.size()) {
            if (arg_type(args[i]) != is_argument) {
                u8string key, value;
                bool paired(str_partition_at(args[i], key, value, "="));
                if (paired) {
                    args[i] = key;
                    args.insert(std::begin(args) + i + 1, value);
                    ++i;
                }
            }
            ++i;
        }
        // Replace abbreviated options with their full names
        i = 0;
        while (i < args.size()) {
            if (arg_type(args[i]) == is_short_option) {
                auto arg = args[i];
                args.erase(std::begin(args) + i);
                size_t j = 0;
                for (auto u = std::next(utf_begin(arg)), uend = utf_end(arg); u != uend; ++u, ++j) {
                    auto key = "-" + str_chars<char>(*u);
                    size_t o = opt_index(key);
                    if (o == npos)
                        throw CommandLineError("Unknown option", arg);
                    args.insert(std::begin(args) + i + j, opts[o].name);
                }
                i += j;
            } else {
                ++i;
            }
        }
        // Extract the named options
        i = 0;
        while (i < args.size()) {
            if (arg_type(args[i]) != is_long_option) {
                ++i;
                continue;
            }
            size_t o = opt_index(args[i]);
            if (o == npos)
                throw CommandLineError("Unknown option", args[i]);
            auto& opt(opts[o]);
            if (opt.found && ! opt.flags.get(opt_multiple))
                throw CommandLineError("Duplicate option", args[i]);
            if (! opt.group.empty())
                for (auto& opt2: opts)
                    if (&opt2 != &opt && opt2.group == opt.group && opt2.found)
                        throw CommandLineError("Incompatible options", opt2.name, args[i]);
            opt.found = true;
            size_t n, max_n;
            if (opt.flags.get(opt_boolean))
                max_n = 1;
            else if (opt.flags.get(opt_multiple))
                max_n = args.size() - i;
            else
                max_n = std::min(size_t(2), args.size() - i);
            bool check_regex = ! opt.pattern.empty();
            for (n = 1; n < max_n && arg_type(args[i + n]) == is_argument; ++n) {
                auto& arg(args[i + n]);
                if (check_regex && (opt.flags.get(opt_required) || ! arg.empty()) && ! opt.pattern.match(arg))
                    throw CommandLineError("Invalid argument to option", args[i], arg);
                opt.values.push_back(arg);
            }
            args.erase(std::begin(args) + i, std::begin(args) + i + n);
        }
        // Handle the remaining anonymous arguments
        args.insert(std::end(args), CROW_BOUNDS(anon));
        for (auto& opt: opts) {
            if (args.empty())
                break;
            if (opt.flags.get(opt_anon)) {
                opt.found = true;
                if (opt.flags.get(opt_multiple)) {
                    opt.values.insert(std::end(opt.values), CROW_BOUNDS(args));
                    args.clear();
                } else {
                    opt.values.push_back(args[0]);
                    args.erase(std::begin(args));
                }
            }
        }
        if (! args.empty())
            throw CommandLineError("Unexpected argument", args[0]);
        // Check for the help and version options
        if (has("help"))
            throw HelpRequest(help());
        if (has("version"))
            throw HelpRequest(app_info);
        // Check that all required options are present, and fill in default
        // values for missing options
        for (auto& opt: opts) {
            if (opt.found) {
                if (opt.flags.get(opt_boolean))
                    opt.values.push_back("1");
            } else {
                if (opt.flags.get(opt_required))
                    throw CommandLineError("Required option is missing", opt.name);
                if (! opt.defval.empty())
                    opt.values.push_back(opt.defval);
            }
        }
    }

    u8string Options::arg_convert(const string& str, Crow::Flagset flags) {
        if (! flags.get(opt_locale))
            return str;
        u8string utf8;
        import_string(str, utf8, local_encoding());
        return utf8;
    }

    void Options::unquote(const u8string& src, std::vector<u8string>& dst) {
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
