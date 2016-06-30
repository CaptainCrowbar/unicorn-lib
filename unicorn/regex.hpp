#pragma once

#include "unicorn/core.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace Unicorn {

    // Forward declarations

    class Match;
    class MatchIterator;
    class Regex;
    class RegexFormat;
    class SplitIterator;
    using MatchRange = Irange<MatchIterator>;
    using SplitRange = Irange<SplitIterator>;

    namespace UnicornDetail {

        // Reference counted PCRE handle

        class PcreRef {
        public:
            PcreRef() noexcept: pc(nullptr), ex(nullptr) {}
            PcreRef(void* p, void* x) noexcept: pc(p), ex(x) { inc(); }
            PcreRef(const PcreRef& p) noexcept: pc(p.pc), ex(p.ex) { inc(); }
            PcreRef(PcreRef&& p) noexcept: pc(p.pc), ex(p.ex) { p.pc = p.ex = nullptr; }
            ~PcreRef() noexcept { dec(); }
            PcreRef& operator=(const PcreRef& p) noexcept { auto q(p); swap(q); return *this; }
            PcreRef& operator=(PcreRef&& p) noexcept { auto q(p); swap(q); return *this; }
            size_t count_groups() const noexcept;
            void* get_pc_ptr() const noexcept { return pc; }
            void* get_ex_ptr() const noexcept { return ex; }
            size_t named_group(const u8string& name) const noexcept;
            void swap(PcreRef& p) noexcept { std::swap(pc, p.pc); std::swap(ex, p.ex); }
            explicit operator bool() const noexcept { return pc; }
        private:
            void* pc;
            void* ex;
            void inc() noexcept;
            void dec() noexcept;
        };

    }

    // Regex options

    constexpr uint32_t rx_byte             = 1ul << 0;   // Byte mode matching                               ~PCRE_UTF8
    constexpr uint32_t rx_caseless         = 1ul << 1;   // Case insensitive matching                        PCRE_CASELESS
    constexpr uint32_t rx_dfa              = 1ul << 2;   // Use the alternative DFA algorithm                pcre_dfa_exec()
    constexpr uint32_t rx_dollarnewline    = 1ul << 3;   // $ may match line break at end                    ~PCRE_DOLLAR_ENDONLY
    constexpr uint32_t rx_dotinline        = 1ul << 4;   // . does not match newlines                        ~PCRE_DOTALL
    constexpr uint32_t rx_extended         = 1ul << 5;   // Free-form mode (ignore whitespace and comments)  PCRE_EXTENDED
    constexpr uint32_t rx_firstline        = 1ul << 6;   // Must match in first line                         PCRE_FIRSTLINE
    constexpr uint32_t rx_multiline        = 1ul << 7;   // Multiline mode (^ and $ match BOL/EOL)           PCRE_MULTILINE
    constexpr uint32_t rx_newlineanycrlf   = 1ul << 8;   // Line break is any of CR, LF, CRLF                PCRE_NEWLINE_ANYCRLF
    constexpr uint32_t rx_newlinecr        = 1ul << 9;   // Line break is CR only                            PCRE_NEWLINE_CR
    constexpr uint32_t rx_newlinecrlf      = 1ul << 10;  // Line break is CRLF only                          PCRE_NEWLINE_CRLF
    constexpr uint32_t rx_newlinelf        = 1ul << 11;  // Line break is LF only                            PCRE_NEWLINE_LF
    constexpr uint32_t rx_noautocapture    = 1ul << 12;  // No automatic captures                            PCRE_NO_AUTO_CAPTURE
    constexpr uint32_t rx_nostartoptimize  = 1ul << 13;  // No startup optimization                          PCRE_NO_START_OPTIMIZE
    constexpr uint32_t rx_notbol           = 1ul << 14;  // Start of text is not a line break                PCRE_NOTBOL
    constexpr uint32_t rx_notempty         = 1ul << 15;  // Do not match an empty u8string                   PCRE_NOTEMPTY
    constexpr uint32_t rx_notemptyatstart  = 1ul << 16;  // Match an empty u8string only at the start        PCRE_NOTEMPTY_ATSTART
    constexpr uint32_t rx_noteol           = 1ul << 17;  // End of text is not a line break                  PCRE_NOTEOL
    constexpr uint32_t rx_noutfcheck       = 1ul << 18;  // Skip UTF validity checks                         PCRE_NO_UTF8_CHECK
    constexpr uint32_t rx_optimize         = 1ul << 19;  // Take extra effort to optimize the regex          PCRE_STUDY_JIT_COMPILE
    constexpr uint32_t rx_partialhard      = 1ul << 20;  // Hard partial matching (prefer over full match)   PCRE_PARTIAL_HARD
    constexpr uint32_t rx_partialsoft      = 1ul << 21;  // Soft partial matching (only if no full match)    PCRE_PARTIAL_SOFT
    constexpr uint32_t rx_prefershort      = 1ul << 22;  // Non-greedy quantifiers, or shorter DFA matches   PCRE_UNGREEDY,PCRE_DFA_SHORTEST
    constexpr uint32_t rx_ucp              = 1ul << 23;  // Use Unicode properties in escape charsets        PCRE_UCP

    // Exceptions

    class RegexError:
    public std::runtime_error {
    public:
        RegexError(int error, const u8string& pattern, const u8string& message = {});
        int error() const noexcept { return err; }
        const char* pattern() const noexcept { return pat->data(); }
    private:
        int err;
        shared_ptr<u8string> pat;
        static u8string assemble(int error, const u8string& pattern, const u8string& message);
        static u8string translate(int error);
    };

    // Regex match class

    class Match {
    public:
        u8string operator[](size_t i) const { return str(i); }
        operator u8string() const { return str(); }
        explicit operator bool() const noexcept { return matched(); }
        bool operator!() const noexcept { return ! matched(); }
        size_t count(size_t i = 0) const noexcept { return is_group(i) ? ofs[2 * i + 1] - ofs[2 * i] : 0; }
        bool empty() const noexcept { return ! *this || ofs[0] == ofs[1]; }
        size_t endpos(size_t i = 0) const noexcept { return is_group(i) ? ofs[2 * i + 1] : npos; }
        u8string first() const;
        u8string last() const;
        bool full_or_partial() const noexcept { return matched() || partial(); }
        size_t groups() const noexcept { return std::max(status, 0); }
        bool matched(size_t i = 0) const noexcept { return status >= 0 && (i == 0 || is_group(i)); }
        u8string named(const u8string& name) const { return ref ? str(ref.named_group(name)) : u8string(); }
        size_t offset(size_t i = 0) const noexcept { return is_group(i) ? ofs[2 * i] : npos; }
        bool partial() const noexcept { return status == -12; } // PCRE_ERROR_PARTIAL
        u8string::const_iterator s_begin(size_t i = 0) const noexcept;
        u8string::const_iterator s_end(size_t i = 0) const noexcept;
        Irange<u8string::const_iterator> s_range(size_t i = 0) const noexcept { return {s_begin(i), s_end(i)}; }
        u8string str(size_t i = 0) const;
        void swap(Match& m) noexcept;
        Utf8Iterator u_begin(size_t i = 0) const noexcept;
        Utf8Iterator u_end(size_t i = 0) const noexcept;
        Irange<Utf8Iterator> u_range(size_t i = 0) const noexcept { return {u_begin(i), u_end(i)}; }
    private:
        friend class MatchIterator;
        friend class Regex;
        friend class SplitIterator;
        vector<int> ofs;
        uint32_t fset = 0;
        UnicornDetail::PcreRef ref;
        int status = -1;
        const u8string* text = nullptr;
        void init(const Regex& r, const u8string& s);
        void next(const u8string& pattern, size_t start, int anchors);
        bool is_group(size_t i) const noexcept { return i < groups() && ofs[2 * i] >= 0 && ofs[2 * i + 1] >= 0; }
    };

    inline void swap(Match& lhs, Match& rhs) noexcept { lhs.swap(rhs); }

    // Regular expression class

    class Regex:
    public LessThanComparable<Regex> {
    public:
        Regex(): Regex({}, 0) {}
        explicit Regex(const u8string& pattern, uint32_t flags = 0);
        Match operator()(const u8string& text, size_t offset = 0) const { return search(text, offset); }
        Match operator()(const Utf8Iterator& start) const { return search(start.source(), start.offset()); }
        Match anchor(const u8string& text, size_t offset = 0) const { return exec(text, offset, 1); }
        Match anchor(const Utf8Iterator& start) const { return anchor(start.source(), start.offset()); }
        Match match(const u8string& text, size_t offset = 0) const { return exec(text, offset, 2); }
        Match match(const Utf8Iterator& start) const { return match(start.source(), start.offset()); }
        Match search(const u8string& text, size_t offset = 0) const { return exec(text, offset, 0); }
        Match search(const Utf8Iterator& start) const { return search(start.source(), start.offset()); }
        size_t count(const u8string& text) const;
        bool empty() const noexcept { return pat.empty(); }
        u8string extract(const u8string& fmt, const u8string& text, size_t n = npos) const;
        u8string format(const u8string& fmt, const u8string& text, size_t n = npos) const;
        MatchRange grep(const u8string& text) const;
        size_t groups() const noexcept { return ref.count_groups(); }
        size_t named(const u8string& name) const noexcept { return ref.named_group(name); }
        u8string pattern() const { return pat; }
        uint32_t flags() const noexcept { return fset; }
        SplitRange split(const u8string& text) const;
        void swap(Regex& r) noexcept;
        template <typename F> u8string transform(const u8string& text, F f, size_t n = npos) const;
        template <typename F> void transform_in(u8string& text, F f, size_t n = npos) const;
        static u8string escape(const u8string& str);
        static Version pcre_version() noexcept;
        static Version unicode_version() noexcept;
        friend bool operator==(const Regex& lhs, const Regex& rhs) noexcept;
        friend bool operator<(const Regex& lhs, const Regex& rhs) noexcept;
    private:
        friend class MatchIterator;
        friend class Match;
        using string_transform = function<u8string(const u8string&)>;
        u8string pat;
        uint32_t fset = 0;
        UnicornDetail::PcreRef ref;
        void do_transform(const u8string& src, u8string& dst, string_transform f, size_t n) const;
        Match exec(const u8string& text, size_t offset, int anchors) const;
    };

    template <typename F>
    u8string Regex::transform(const u8string& text, F f, size_t n) const {
        u8string dst;
        do_transform(text, dst, string_transform(f), n);
        return dst;
    }

    template <typename F>
    void Regex::transform_in(u8string& text, F f, size_t n) const {
        u8string dst;
        do_transform(text, dst, string_transform(f), n);
        text = move(dst);
    }

    inline void swap(Regex& lhs, Regex& rhs) noexcept { lhs.swap(rhs); }

    namespace Literals {

        inline Regex operator"" _re(const char* ptr, size_t len) { return Regex(cstr(ptr, len)); }
        inline Regex operator"" _re_b(const char* ptr, size_t len) { return Regex(cstr(ptr, len), rx_byte); }
        inline Regex operator"" _re_i(const char* ptr, size_t len) { return Regex(cstr(ptr, len), rx_caseless); }

    }

    // Regex formatting class

    class RegexFormat {
    public:
        RegexFormat() = default;
        RegexFormat(const Regex& pattern, const u8string& format);
        RegexFormat(const u8string& pattern, const u8string& format, uint32_t flags = 0);
        u8string operator()(const u8string& text, size_t n = npos) const { return format(text, n); }
        uint32_t flags() const noexcept { return reg.flags(); }
        u8string extract(const u8string& text, size_t n = npos) const { return apply(text, n, false); }
        u8string format() const { return fmt; }
        u8string format(const u8string& text, size_t n = npos) const { return apply(text, n, true); }
        u8string pattern() const { return reg.pattern(); }
        Regex regex() const { return reg; }
        void swap(RegexFormat& r) noexcept;
    private:
        // Index field in element record indicates what to substitute
        // >= 0  => numbered capture group
        // -1    => literal text
        // -2    => named capture group
        // < -2  => escape code
        static constexpr int literal = -1, named = -2;
        struct element {
            int index;
            u8string text;
        };
        using sequence = vector<element>;
        u8string fmt;
        Regex reg;
        sequence seq;
        void add_escape(char c);
        void add_literal(const u8string& text);
        void add_literal(const u8string& text, size_t offset, size_t count);
        void add_literal(char32_t u);
        void add_named(const u8string& name) { seq.push_back({named, name}); }
        void add_tag(int tag) { seq.push_back({tag, {}}); }
        u8string apply(const u8string& text, size_t n, bool full) const;
        void parse();
    };

    inline void swap(RegexFormat& lhs, RegexFormat& rhs) noexcept { lhs.swap(rhs); }

    // Iterator over regex matches

    class MatchIterator:
    public ForwardIterator<MatchIterator, const Match> {
    public:
        MatchIterator() = default;
        MatchIterator(const Regex& re, const u8string& text);
        const Match& operator*() const noexcept { return mat; }
        MatchIterator& operator++();
        friend bool operator==(const MatchIterator& lhs, const MatchIterator& rhs) noexcept;
    private:
        Match mat;
        u8string pat;
    };

    // Iterator over substrings between matches

    class SplitIterator:
    public ForwardIterator<SplitIterator, const u8string> {
    public:
        SplitIterator() = default;
        SplitIterator(const Regex& re, const u8string& text);
        const u8string& operator*() const noexcept { return value; }
        SplitIterator& operator++();
        friend bool operator==(const SplitIterator& lhs, const SplitIterator& rhs) noexcept;
    private:
        MatchIterator iter;
        size_t start = npos;
        u8string value;
        void update();
    };

}
