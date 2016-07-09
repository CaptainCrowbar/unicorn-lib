#include "unicorn/regex.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include <new>
#include <pcre.h>

using namespace std::literals;

namespace Unicorn {

    namespace {

        const char* const error_table[] {
            "",
            "No match",                                  // PCRE_ERROR_NOMATCH         = -1
            "Null pointer",                              // PCRE_ERROR_NULL            = -2
            "Bad option",                                // PCRE_ERROR_BADOPTION       = -3
            "Bad magic number",                          // PCRE_ERROR_BADMAGIC        = -4
            "Unknown opcode",                            // PCRE_ERROR_UNKNOWN_OPCODE  = -5
            "No memory",                                 // PCRE_ERROR_NOMEMORY        = -6
            "No such substring",                         // PCRE_ERROR_NOSUBSTRING     = -7
            "Match backtracking limit reached",          // PCRE_ERROR_MATCHLIMIT      = -8
            "Callout error",                             // PCRE_ERROR_CALLOUT         = -9
            "Bad UTF string",                            // PCRE_ERROR_BADUTF8         = -10
            "Bad UTF offset",                            // PCRE_ERROR_BADUTF8_OFFSET  = -11
            "Partial match",                             // PCRE_ERROR_PARTIAL         = -12
            "Partial match not supported",               // PCRE_ERROR_BADPARTIAL      = -13
            "Internal error",                            // PCRE_ERROR_INTERNAL        = -14
            "Bad vector size",                           // PCRE_ERROR_BADCOUNT        = -15
            "DFA unsupported item",                      // PCRE_ERROR_DFA_UITEM       = -16
            "DFA unsupported condition",                 // PCRE_ERROR_DFA_UCOND       = -17
            "DFA unsupported match limit",               // PCRE_ERROR_DFA_UMLIMIT     = -18
            "DFA workspace size reached",                // PCRE_ERROR_DFA_WSSIZE      = -19
            "DFA recursion limit reached",               // PCRE_ERROR_DFA_RECURSE     = -20
            "Recursion limit reached",                   // PCRE_ERROR_RECURSIONLIMIT  = -21
            "Null workspace limit",                      // PCRE_ERROR_NULLWSLIMIT     = -22 [obsolete]
            "Bad newline options",                       // PCRE_ERROR_BADNEWLINE      = -23
            "Offset is out of bounds",                   // PCRE_ERROR_BADOFFSET       = -24
            "Truncated UTF sequence",                    // PCRE_ERROR_SHORTUTF8       = -25
            "Recursion loop in pattern",                 // PCRE_ERROR_RECURSELOOP     = -26
            "JIT stack limit exceeded",                  // PCRE_ERROR_JIT_STACKLIMIT  = -27
            "Pattern has wrong UTF mode",                // PCRE_ERROR_BADMODE         = -28
            "Pattern has wrong endianness",              // PCRE_ERROR_BADENDIANNESS   = -29
            "DFA invalid restart workspace",             // PCRE_ERROR_DFA_BADRESTART  = -30
            "JIT matching mode does not match compile",  // PCRE_ERROR_JIT_BADOPTION   = -31
            "Negative string length",                    // PCRE_ERROR_BADLENGTH       = -32
            "Requested field is not set",                // PCRE_ERROR_UNSET           = -33
        };

        Version check_unicode_version() noexcept {
            static const Regex unassigned("\\p{Cn}");
            Version v;
            auto& table = UnicornDetail::unicode_version_table().table;
            for (auto& entry: table) {
                if (unassigned.match(str_char(entry.second)))
                    break;
                v = entry.first;
            }
            return v;
        }

        int translate_match_flags(uint32_t fset) {
            int mask = 0;
            if (fset & rx_partialhard)
                mask |= PCRE_PARTIAL_HARD;
            if (fset & rx_partialsoft)
                mask |= PCRE_PARTIAL_SOFT;
            if (fset & rx_notbol)
                mask |= PCRE_NOTBOL;
            if (fset & rx_notempty)
                mask |= PCRE_NOTEMPTY;
            if (fset & rx_notemptyatstart)
                mask |= PCRE_NOTEMPTY_ATSTART;
            if (fset & rx_noteol)
                mask |= PCRE_NOTEOL;
            if (fset & rx_nostartoptimize)
                mask |= PCRE_NO_START_OPTIMIZE;
            return mask;
        }

    }

    namespace UnicornDetail {

        // Reference counted PCRE handle

        auto get_pcre(const PcreRef& ref) noexcept {
            return static_cast<pcre*>(ref.get_pc_ptr());
        }

        auto get_extra(const PcreRef& ref) noexcept {
            return static_cast<pcre_extra*>(ref.get_ex_ptr());
        }

        size_t PcreRef::count_groups() const noexcept {
            if (! pc)
                return 0;
            int n = 0;
            pcre_fullinfo(get_pcre(*this), nullptr, PCRE_INFO_CAPTURECOUNT, &n);
            return n + 1;
        }

        size_t PcreRef::named_group(const u8string& name) const noexcept {
            if (! pc)
                return npos;
            auto rc = pcre_get_stringnumber(get_pcre(*this), name.data());
            return rc >= 0 ? rc : npos;
        }

        void PcreRef::inc() noexcept {
            if (get_pcre(*this))
                pcre_refcount(get_pcre(*this), 1);
        }

        void PcreRef::dec() noexcept {
            if (get_pcre(*this) && pcre_refcount(get_pcre(*this), -1) == 0) {
                if (get_extra(*this))
                    pcre_free_study(get_extra(*this));
                pcre_free(get_pcre(*this));
            }
        }

    }

    // Exceptions

    RegexError::RegexError(int error, const u8string& pattern, const u8string& message):
        std::runtime_error(assemble(error, pattern, message)),
        err(error),
        pat(make_shared<u8string>(pattern)) {}

    u8string RegexError::assemble(int error, const u8string& pattern, const u8string& message) {
        u8string text = "Regex error " + dec(error);
        u8string errmsg = message.empty() ? translate(error) : message;
        if (! errmsg.empty())
            text += ": " + errmsg;
        text += "; pattern: " + uquote(pattern);
        return text;
    }

    u8string RegexError::translate(int error) {
        if (error < 0 && error > - int(range_count(error_table)))
            return error_table[- error];
        else
            return {};
    }

    // Regex match class

    u8string Match::first() const {
        if (! matched() || ! text)
            return {};
        size_t n = groups();
        for (size_t i = 1; i < n; ++i)
            if (is_group(i) && ofs[2 * i + 1] > ofs[2 * i])
                return text->substr(ofs[2 * i], ofs[2 * i + 1] - ofs[2 * i]);
        return {};
    }

    u8string Match::last() const {
        if (! matched() || ! text)
            return {};
        size_t n = groups();
        for (size_t i = n - 1; i > 0; --i)
            if (is_group(i) && ofs[2 * i + 1] > ofs[2 * i])
                return text->substr(ofs[2 * i], ofs[2 * i + 1] - ofs[2 * i]);
        return {};
    }

    u8string::const_iterator Match::s_begin(size_t i) const noexcept {
        if (text && is_group(i))
            return text->begin() + offset(i);
        else
            return {};
    }

    u8string::const_iterator Match::s_end(size_t i) const noexcept {
        if (text && is_group(i))
            return text->begin() + endpos(i);
        else
            return {};
    }

    void Match::swap(Match& m) noexcept {
        ofs.swap(m.ofs);
        std::swap(fset, m.fset);
        std::swap(ref, m.ref);
        std::swap(status, m.status);
        std::swap(text, m.text);
    }

    u8string Match::str(size_t i) const {
        if (text && is_group(i))
            return text->substr(ofs[2 * i],
                ofs[2 * i + 1] - ofs[2 * i]);
        else
            return {};
    }

    Utf8Iterator Match::u_begin(size_t i) const noexcept {
        if (text && is_group(i))
            return Utf8Iterator(*text, offset(i));
        else
            return {};
    }

    Utf8Iterator Match::u_end(size_t i) const noexcept {
        if (text && is_group(i))
            return Utf8Iterator(*text, endpos(i));
        else
            return {};
    }

    void Match::init(const Regex& r, const u8string& s) {
        ofs.clear();
        fset = r.fset;
        ref = r.ref;
        status = -1;
        text = &s;
    }

    void Match::next(const u8string& pattern, size_t start, int anchors) {
        status = PCRE_ERROR_NOMATCH;
        if (! ref || start > text->size())
            return;
        int xflags = 0;
        if (anchors > 0)
            xflags |= PCRE_ANCHORED;
        if (fset & rx_dfa) {
            if (fset & rx_prefershort)
                xflags |= PCRE_DFA_SHORTEST;
            if (ofs.size() < 40)
                ofs.resize(40); // ovector + workspace
            for (;;) {
                auto half = int(ofs.size() / 2);
                status = pcre_dfa_exec(get_pcre(ref), get_extra(ref), text->data(), int(text->size()), int(start),
                    translate_match_flags(fset) | xflags, ofs.data(), half, ofs.data() + half, half);
                if (status != 0 && status != PCRE_ERROR_DFA_WSSIZE)
                    break;
                ofs.resize(ofs.size() * 2);
            }
        } else {
            size_t minsize = 3 * ref.count_groups();
            if (ofs.size() < minsize)
                ofs.resize(minsize);
            status = pcre_exec(get_pcre(ref), get_extra(ref), text->data(), int(text->size()), int(start),
                translate_match_flags(fset) | xflags, ofs.data(), int(ofs.size()));
        }
        if (status >= 0 && anchors == 2 && count(0) < text->size() - start)
            status = PCRE_ERROR_NOMATCH;
        if (status == PCRE_ERROR_NOMEMORY)
            throw std::bad_alloc();
        if (status < 0 && status != PCRE_ERROR_NOMATCH && status != PCRE_ERROR_PARTIAL)
            throw RegexError(status, to_utf8(pattern));
    }

    // Regular expression class

    Regex::Regex(const u8string& pattern, uint32_t flags) {
        if (bits_set(flags & (rx_newlineanycrlf | rx_newlinecr | rx_newlinecrlf | rx_newlinelf)) > 1
                || bits_set(flags & (rx_notempty | rx_notemptyatstart)) > 1
                || bits_set(flags & (rx_partialhard | rx_partialsoft)) > 1)
            throw std::invalid_argument("Inconsistent regex flags");
        pat = pattern;
        fset = flags;
        int cflags = PCRE_EXTRA, sflags = 0;
        if (! (flags & rx_byte))
            cflags |= PCRE_UTF8;
        if (flags & rx_newlineanycrlf)
            cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_ANYCRLF;
        else if (flags & rx_newlinecr)
            cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_CR;
        else if (flags & rx_newlinecrlf)
            cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_CRLF;
        else if (flags & rx_newlinelf)
            cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_LF;
        else
            cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_ANYCRLF;
        if (flags & rx_optimize) {
            sflags |= PCRE_STUDY_JIT_COMPILE;
            if (flags & rx_partialhard)
                sflags |= PCRE_STUDY_JIT_PARTIAL_HARD_COMPILE;
            else if (flags & rx_partialsoft)
                sflags |= PCRE_STUDY_JIT_PARTIAL_SOFT_COMPILE;
        }
        if (flags & rx_caseless)
            cflags |= PCRE_CASELESS;
        if (! (flags & rx_dollarnewline))
            cflags |= PCRE_DOLLAR_ENDONLY;
        if (! (flags & rx_dotinline))
            cflags |= PCRE_DOTALL;
        if (flags & rx_extended)
            cflags |= PCRE_EXTENDED;
        if (flags & rx_firstline)
            cflags |= PCRE_FIRSTLINE;
        if (flags & rx_multiline)
            cflags |= PCRE_MULTILINE;
        if (flags & rx_noautocapture)
            cflags |= PCRE_NO_AUTO_CAPTURE;
        if (flags & rx_nostartoptimize)
            cflags |= PCRE_NO_START_OPTIMIZE;
        if ((flags & rx_noutfcheck) && ! (flags & rx_byte))
            cflags |= PCRE_NO_UTF8_CHECK;
        if (flags & rx_prefershort)
            cflags |= PCRE_UNGREEDY;
        if (flags & rx_ucp)
            cflags |= PCRE_UCP;
        int error = 0, errpos = 0;
        const char* errptr = nullptr;
        auto pc = pcre_compile2(pattern.data(), cflags, &error, &errptr, &errpos, nullptr);
        if (! pc) {
            if (error == 21)
                throw std::bad_alloc();
            else
                throw RegexError(error, pattern, cstr(errptr));
        }
        auto ex = pcre_study(pc, sflags, &errptr);
        ref = {pc, ex};
    }

    size_t Regex::count(const u8string& text) const {
        Match m;
        m.init(*this, text);
        size_t n = 0, ofs = 0;
        for (;;) {
            m.next(pattern(), ofs, 0);
            if (! m)
                break;
            ++n;
            ofs = std::max(m.endpos(), ofs + 1);
        }
        return n;
    }

    u8string Regex::extract(const u8string& fmt, const u8string& text, size_t n) const {
        return RegexFormat(*this, fmt).extract(text, n);
    }

    u8string Regex::format(const u8string& fmt, const u8string& text, size_t n) const {
        return RegexFormat(*this, fmt).format(text, n);
    }

    MatchRange Regex::grep(const u8string& text) const {
        return {{*this, text}, {}};
    }

    SplitRange Regex::split(const u8string& text) const {
        return {{*this, text}, {}};
    }

    void Regex::swap(Regex& r) noexcept {
        pat.swap(r.pat);
        std::swap(fset, r.fset);
        ref.swap(r.ref);
    }

    u8string Regex::escape(const u8string& str) {
        static constexpr const char* hexdigits = "0123456789abcdef";
        u8string dst;
        for (auto c: str) {
            auto u = uint8_t(c);
            switch (c) {
                case 0:
                    dst += "\\0";
                    break;
                case '\t':
                    dst += "\\t";
                    break;
                case '\n':
                    dst += "\\n";
                    break;
                case '\f':
                    dst += "\\f";
                    break;
                case '\r':
                    dst += "\\r";
                    break;
                case '\"': case '$': case '\'': case '(': case ')':
                case '*': case '+': case '.': case '?': case '[':
                case '\\': case ']': case '^': case '{': case '|':
                case '}':
                    dst += '\\';
                    dst += c;
                    break;
                default:
                    if (u <= 31 || u == 127) {
                        dst += "\\x";
                        dst += hexdigits[(u >> 4) & 15];
                        dst += hexdigits[u & 15];
                    } else {
                        dst += c;
                    }
                    break;
            }
        }
        return dst;
    }

    Version Regex::pcre_version() noexcept {
        return {PCRE_MAJOR, PCRE_MINOR, 0};
    }

    Version Regex::unicode_version() noexcept {
        static const Version v = check_unicode_version();
        return v;
    }

    void Regex::do_transform(const u8string& src, u8string& dst, string_transform f, size_t n) const {
        auto matches = grep(src);
        auto m = matches.begin(), end = matches.end();
        size_t prev = 0;
        for (size_t i = 0; i < n && m != end; ++i, ++m) {
            dst.append(src, prev, m->offset() - prev);
            dst += f(m->str());
            prev = m->endpos();
        }
        dst.append(src, prev, npos);
    }

    Match Regex::exec(const u8string& text, size_t offset, int anchors) const {
        Match m;
        m.init(*this, text);
        m.next(pattern(), offset, anchors);
        return m;
    }

    bool operator==(const Regex& lhs, const Regex& rhs) noexcept {
        return lhs.pat == rhs.pat && lhs.fset == rhs.fset;
    }

    bool operator<(const Regex& lhs, const Regex& rhs) noexcept {
        return lhs.pat == rhs.pat ? lhs.fset < rhs.fset : lhs.pat < rhs.pat;
    }

    // Regex formatting class

    RegexFormat::RegexFormat(const Regex& pattern, const u8string& format):
        fmt(format),
        reg(pattern),
        seq() {
            parse();
        }

    RegexFormat::RegexFormat(const u8string& pattern, const u8string& format, uint32_t flags):
        RegexFormat(Regex(pattern, flags), format) {}

    void RegexFormat::swap(RegexFormat& r) noexcept {
        fmt.swap(r.fmt);
        reg.swap(r.reg);
        seq.swap(r.seq);
    }

    void RegexFormat::add_escape(char c) {
        char32_t u = 0;
        switch (c) {
            case '0':  u = U'\0'; break;
            case 'a':  u = U'\a'; break;
            case 'b':  u = U'\b'; break;
            case 't':  u = U'\t'; break;
            case 'n':  u = U'\n'; break;
            case 'v':  u = U'\v'; break;
            case 'f':  u = U'\f'; break;
            case 'r':  u = U'\r'; break;
            case 'e':  u = 0x1b; break;
            default:   break;
        }
        add_literal(u);
    }

    void RegexFormat::add_literal(const u8string& text) {
        if (! text.empty()) {
            if (seq.empty() || seq.back().index != literal)
                seq.push_back({literal, text});
            else
                seq.back().text += text;
        }
    }

    void RegexFormat::add_literal(const u8string& text, size_t offset, size_t count) {
        if (offset < text.size() && count > 0)
            add_literal(text.substr(offset, count));
    }

    void RegexFormat::add_literal(char32_t u) {
        if (seq.empty() || seq.back().index != literal)
            seq.push_back({literal, {}});
        if (reg.flags() & rx_byte) {
            if (u > 0xff)
                throw EncodingError("byte", 0, u);
            seq.back().text += char(u);
        } else {
            if (! char_is_unicode(u))
                throw EncodingError("Unicode", 0, u);
            str_append_char(seq.back().text, u);
        }
    }

    u8string RegexFormat::apply(const u8string& text, size_t n, bool full) const {
        using namespace UnicornDetail;
        u8string block, dst;
        u8string* current = &dst;
        char block_flag = 0, char_flag = 0;
        bool ascii = (reg.flags() & rx_byte) != 0;
        auto end_block = [&] {
            if (current == &dst)
                return;
            switch (block_flag) {
                case 'L':  if (ascii) block = ascii_lowercase(block); else str_lowercase_in(block); break;
                case 'T':  if (ascii) block = ascii_titlecase(block); else str_titlecase_in(block); break;
                case 'U':  if (ascii) block = ascii_uppercase(block); else str_uppercase_in(block); break;
                default:   break;
            }
            dst += block;
            block.clear();
            block_flag = 0;
            current = &dst;
        };
        vector<Match> matches;
        for (auto& m: reg.grep(text)) {
            matches.push_back(m);
            if (matches.size() >= n)
                break;
        }
        for (size_t i = 0, nm = matches.size(); i < nm; ++i) {
            const auto& m = matches[i];
            size_t prev = i == 0 ? 0 : matches[i - 1].endpos();
            size_t next = i == nm - 1 ? text.size() : matches[i + 1].offset();
            if (full)
                dst.append(text, prev, m.offset() - prev);
            block_flag = char_flag = 0;
            for (auto& elem: seq) {
                u8string fragment;
                if (elem.index >= 0) {
                    fragment = m.str(elem.index);
                } else if (elem.index == literal) {
                    fragment = elem.text;
                } else if (elem.index == named) {
                    fragment = m.named(elem.text);
                } else {
                    auto flag = char(- elem.index);
                    switch (flag) {
                        case 'E':                      end_block(); break;
                        case 'L': case 'T': case 'U':  end_block(); block_flag = flag; current = &block; break;
                        case 'l': case 'u':            char_flag = flag; break;
                        case '-':                      fragment = m.first(); break;
                        case '+':                      fragment = m.last(); break;
                        case '<':                      fragment = text.substr(prev, m.offset() - prev); break;
                        case '>':                      fragment = text.substr(m.endpos(), next - m.endpos()); break;
                        case '[':                      fragment = text.substr(0, m.offset()); break;
                        case ']':                      fragment = text.substr(m.endpos(), npos); break;
                        case '_':                      fragment = text; break;
                        default:                       break;
                    }
                }
                if (fragment.empty())
                    continue;
                if (char_flag) {
                    auto it = utf_begin(fragment);
                    char32_t buf[max_case_decomposition];
                    if (ascii) {
                        *current += char_flag == 'l' ? ascii_tolower(*it) : ascii_toupper(*it);
                    } else {
                        size_t nbuf(char_flag == 'l' ? char_to_full_lowercase(*it, buf) : char_to_full_uppercase(*it, buf));
                        str_append(*current, buf, nbuf);
                    }
                    ++it;
                    current->append(fragment, it.offset(), npos);
                    char_flag = 0;
                } else {
                    *current += fragment;
                }
            }
            end_block();
        }
        if (full) {
            size_t prev = matches.empty() ? 0 : matches.back().endpos();
            dst.append(text, prev, npos);
        }
        return dst;
    }

    void RegexFormat::parse() {
        using namespace UnicornDetail;
        static const Regex pattern(
            R"((\$[0&]))"                                    // (1) Match ($0,$&)
            R"(|(?:\$(\d+)))"                                // (2) Numbered capture ($number)
            R"(|(?:\\([1-9])))"                              // (3) Numbered capture (\digit)
            R"(|(?:\$\{(\d+)\}))"                            // (4) Numbered capture (${number})
            R"(|(?:\$([A-Za-z][0-9A-Za-z_]*)))"              // (5) Named capture ($name)
            R"(|(?:\$\{([^{}]+)\}))"                         // (6) Named capture (${name})
            R"(|(?:\$([-+<>\[\]_])))"                        // (7) Substring ($-,$+,$<,$>,$[,$],$_)
            R"(|(?:\\x([[:xdigit:]]{2})))"                   // (8) Escape code (\xHH)
            R"(|(?:\\x\{([[:xdigit:]]+)\}))"                 // (9) Escape code (\x{HHH...})
            R"(|(?:\\([0abtnvfre])))"                        // (10) Escape code
            R"(|(?:\\([ELTUlu])))"                           // (11) Escape code
            R"(|(?:\\Q((?:[^\\]|\\(?:[^E]|$))*)(?:\\E|$)))"  // (12) Literal text
            R"(|(?:[\\$](.)))",                              // (13) Unknown
            rx_optimize);
        size_t prev = 0;
        for (;;) {
            auto match = pattern.search(fmt, prev);
            if (! match) {
                add_literal(fmt, prev, npos);
                break;
            }
            if (match.offset() > prev)
                add_literal(fmt, prev, match.offset() - prev);
            if (match.count(1))
                add_tag(0);
            else if (match.count(2))
                add_tag(decnum(to_utf8(match[2])));
            else if (match.count(3))
                add_tag(decnum(to_utf8(match[3])));
            else if (match.count(4))
                add_tag(decnum(to_utf8(match[4])));
            else if (match.count(5))
                add_named(match[5]);
            else if (match.count(6))
                add_named(match[6]);
            else if (match.count(7))
                add_tag(- int(match[7][0]));
            else if (match.count(8))
                add_literal(hexnum(to_utf8(match[8])));
            else if (match.count(9))
                add_literal(hexnum(to_utf8(match[9])));
            else if (match.count(10))
                add_escape(match[10][0]);
            else if (match.count(11))
                add_tag(- int(match[11][0]));
            else if (match.count(12))
                add_literal(match[12]);
            else
                add_literal(match[13]);
            prev = match.endpos();
        }
    }

    // Iterator over regex matches

    MatchIterator::MatchIterator(const Regex& re, const u8string& text):
        mat(re.search(text)),
        pat(re.pattern()) {}

    MatchIterator& MatchIterator::operator++() {
        if (mat)
            mat.next(pat, mat.offset() + std::max(mat.count(), size_t(1)), 0);
        return *this;
    }

    bool operator==(const MatchIterator& lhs, const MatchIterator& rhs) noexcept {
        return bool(lhs.mat) == bool(rhs.mat) && (! lhs.mat || lhs.mat.offset() == rhs.mat.offset());
    }

    // Iterator over substrings between matches

    SplitIterator::SplitIterator(const Regex& re, const u8string& text):
        iter(re, text), start(0),
        value() {
            update();
        }

    SplitIterator& SplitIterator::operator++() {
        start = *iter ? iter->endpos() : npos;
        ++iter;
        update();
        return *this;
    }

    void SplitIterator::update() {
        if (! iter->text || start >= iter->text->size())
            value.clear();
        else if (*iter)
            value.assign(*iter->text, start, iter->offset() - start);
        else
            value.assign(*iter->text, start, npos);
    }

    bool operator==(const SplitIterator& lhs, const SplitIterator& rhs) noexcept {
        return lhs.iter == rhs.iter && lhs.start == rhs.start;
    }

}
