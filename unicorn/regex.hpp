#pragma once

#include "unicorn/core.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

RS_LDLIB(pcre);

namespace RS::Unicorn {

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
            size_t named_group(const Ustring& name) const noexcept;
            void swap(PcreRef& p) noexcept { std::swap(pc, p.pc); std::swap(ex, p.ex); }
            explicit operator bool() const noexcept { return pc; }
        private:
            void* pc;
            void* ex;
            void inc() noexcept;
            void dec() noexcept;
        };

    }

    // Exceptions

    class RegexError:
    public std::runtime_error {
    public:
        RegexError(int error, const Ustring& pattern, const Ustring& message = {});
        int error() const noexcept { return err; }
        const char* pattern() const noexcept { return pat->data(); }
    private:
        int err;
        std::shared_ptr<Ustring> pat;
        static Ustring assemble(int error, const Ustring& pattern, const Ustring& message);
        static Ustring translate(int error);
    };

    // Regex match class

    class Match {
    public:
        Ustring operator[](size_t i) const { return str(i); }
        operator Ustring() const { return str(); }
        explicit operator bool() const noexcept { return matched(); }
        bool operator!() const noexcept { return ! matched(); }
        size_t count(size_t i = 0) const noexcept { return is_group(i) ? ofs[2 * i + 1] - ofs[2 * i] : 0; }
        bool empty() const noexcept { return ! *this || ofs[0] == ofs[1]; }
        size_t endpos(size_t i = 0) const noexcept { return is_group(i) ? ofs[2 * i + 1] : npos; }
        Ustring first() const;
        Ustring last() const;
        bool full_or_partial() const noexcept { return matched() || partial(); }
        size_t groups() const noexcept { return std::max(status, 0); }
        bool matched(size_t i = 0) const noexcept { return status >= 0 && (i == 0 || is_group(i)); }
        Ustring named(const Ustring& name) const { return ref ? str(ref.named_group(name)) : Ustring(); }
        size_t offset(size_t i = 0) const noexcept { return is_group(i) ? ofs[2 * i] : npos; }
        bool partial() const noexcept { return status == -12; } // PCRE_ERROR_PARTIAL
        Ustring::const_iterator s_begin(size_t i = 0) const noexcept;
        Ustring::const_iterator s_end(size_t i = 0) const noexcept;
        Irange<Ustring::const_iterator> s_range(size_t i = 0) const noexcept { return {s_begin(i), s_end(i)}; }
        Ustring str(size_t i = 0) const;
        void swap(Match& m) noexcept;
        Utf8Iterator u_begin(size_t i = 0) const noexcept;
        Utf8Iterator u_end(size_t i = 0) const noexcept;
        Irange<Utf8Iterator> u_range(size_t i = 0) const noexcept { return {u_begin(i), u_end(i)}; }
    private:
        friend class MatchIterator;
        friend class Regex;
        friend class SplitIterator;
        std::vector<int> ofs;
        uint32_t fset = 0;
        UnicornDetail::PcreRef ref;
        int status = -1;
        const Ustring* text = nullptr;
        void init(const Regex& r, const Ustring& s);
        void next(const Ustring& pattern, size_t start, int anchors);
        bool is_group(size_t i) const noexcept { return i < groups() && ofs[2 * i] >= 0 && ofs[2 * i + 1] >= 0; }
    };

    inline void swap(Match& lhs, Match& rhs) noexcept { lhs.swap(rhs); }

    // Regular expression class

    class Regex:
    public LessThanComparable<Regex> {

    public:

        static constexpr uint32_t byte             = 1ul << 0;   // Byte mode matching                               ~PCRE_UTF8
        static constexpr uint32_t caseless         = 1ul << 1;   // Case insensitive matching                        PCRE_CASELESS
        static constexpr uint32_t dfa              = 1ul << 2;   // Use the alternative DFA algorithm                pcre_dfa_exec()
        static constexpr uint32_t dollarnewline    = 1ul << 3;   // $ may match line break at end                    ~PCRE_DOLLAR_ENDONLY
        static constexpr uint32_t dotinline        = 1ul << 4;   // . does not match newlines                        ~PCRE_DOTALL
        static constexpr uint32_t extended         = 1ul << 5;   // Free-form mode (ignore whitespace and comments)  PCRE_EXTENDED
        static constexpr uint32_t firstline        = 1ul << 6;   // Must match in first line                         PCRE_FIRSTLINE
        static constexpr uint32_t multiline        = 1ul << 7;   // Multiline mode (^ and $ match BOL/EOL)           PCRE_MULTILINE
        static constexpr uint32_t newlineanycrlf   = 1ul << 8;   // Line break is any of CR, LF, CRLF                PCRE_NEWLINE_ANYCRLF
        static constexpr uint32_t newlinecr        = 1ul << 9;   // Line break is CR only                            PCRE_NEWLINE_CR
        static constexpr uint32_t newlinecrlf      = 1ul << 10;  // Line break is CRLF only                          PCRE_NEWLINE_CRLF
        static constexpr uint32_t newlinelf        = 1ul << 11;  // Line break is LF only                            PCRE_NEWLINE_LF
        static constexpr uint32_t noautocapture    = 1ul << 12;  // No automatic captures                            PCRE_NO_AUTO_CAPTURE
        static constexpr uint32_t nostartoptimize  = 1ul << 13;  // No startup optimization                          PCRE_NO_START_OPTIMIZE
        static constexpr uint32_t notbol           = 1ul << 14;  // Start of text is not a line break                PCRE_NOTBOL
        static constexpr uint32_t notempty         = 1ul << 15;  // Do not match an empty Ustring                   PCRE_NOTEMPTY
        static constexpr uint32_t notemptyatstart  = 1ul << 16;  // Match an empty Ustring only at the start        PCRE_NOTEMPTY_ATSTART
        static constexpr uint32_t noteol           = 1ul << 17;  // End of text is not a line break                  PCRE_NOTEOL
        static constexpr uint32_t noutfcheck       = 1ul << 18;  // Skip UTF validity checks                         PCRE_NO_UTF8_CHECK
        static constexpr uint32_t optimize         = 1ul << 19;  // Take extra effort to optimize the regex          PCRE_STUDY_JIT_COMPILE
        static constexpr uint32_t partialhard      = 1ul << 20;  // Hard partial matching (prefer over full match)   PCRE_PARTIAL_HARD
        static constexpr uint32_t partialsoft      = 1ul << 21;  // Soft partial matching (only if no full match)    PCRE_PARTIAL_SOFT
        static constexpr uint32_t prefershort      = 1ul << 22;  // Non-greedy quantifiers, or shorter DFA matches   PCRE_UNGREEDY,PCRE_DFA_SHORTEST
        static constexpr uint32_t ucp              = 1ul << 23;  // Use Unicode properties in escape charsets        PCRE_UCP

        Regex(): Regex({}, 0) {}
        explicit Regex(const Ustring& pattern, uint32_t flags = 0);
        Match operator()(const Ustring& text, size_t offset = 0) const { return search(text, offset); }
        Match operator()(const Utf8Iterator& start) const { return search(start.source(), start.offset()); }
        Match anchor(const Ustring& text, size_t offset = 0) const { return exec(text, offset, 1); }
        Match anchor(const Utf8Iterator& start) const { return anchor(start.source(), start.offset()); }
        Match match(const Ustring& text, size_t offset = 0) const { return exec(text, offset, 2); }
        Match match(const Utf8Iterator& start) const { return match(start.source(), start.offset()); }
        Match search(const Ustring& text, size_t offset = 0) const { return exec(text, offset, 0); }
        Match search(const Utf8Iterator& start) const { return search(start.source(), start.offset()); }
        size_t count(const Ustring& text) const;
        bool empty() const noexcept { return pat.empty(); }
        Ustring extract(const Ustring& fmt, const Ustring& text, size_t n = npos) const;
        Ustring format(const Ustring& fmt, const Ustring& text, size_t n = npos) const;
        MatchRange grep(const Ustring& text) const;
        size_t groups() const noexcept { return ref.count_groups(); }
        size_t named(const Ustring& name) const noexcept { return ref.named_group(name); }
        Ustring pattern() const { return pat; }
        uint32_t flags() const noexcept { return fset; }
        SplitRange split(const Ustring& text) const;
        void swap(Regex& r) noexcept;
        template <typename F> Ustring transform(const Ustring& text, F f, size_t n = npos) const;
        template <typename F> void transform_in(Ustring& text, F f, size_t n = npos) const;
        static Ustring escape(const Ustring& str);
        static Version pcre_version() noexcept;
        static Version unicode_version() noexcept;
        friend bool operator==(const Regex& lhs, const Regex& rhs) noexcept;
        friend bool operator<(const Regex& lhs, const Regex& rhs) noexcept;

    private:

        friend class MatchIterator;
        friend class Match;
        Ustring pat;
        uint32_t fset = 0;
        UnicornDetail::PcreRef ref;
        Match exec(const Ustring& text, size_t offset, int anchors) const;

    };

    namespace UnicornDetail {

        void regex_match_transform(const Regex& re, const Ustring& src, Ustring& dst, std::function<Ustring(const Match&)> f, size_t n);
        void regex_string_transform(const Regex& re, const Ustring& src, Ustring& dst, std::function<Ustring(const Ustring&)> f, size_t n);

        template <typename F, bool S = std::is_convertible<F, std::function<Ustring(const Ustring&)>>::value>
        struct RegexTransform ;

        template <typename F>
        struct RegexTransform<F, false> {
            void operator()(const Regex& re, const Ustring& src, Ustring& dst, F f, size_t n) const {
                regex_match_transform(re, src, dst, f, n);
            }
        };

        template <typename F>
        struct RegexTransform<F, true> {
            void operator()(const Regex& re, const Ustring& src, Ustring& dst, F f, size_t n) const {
                regex_string_transform(re, src, dst, f, n);
            }
        };

    }

    template <typename F>
    Ustring Regex::transform(const Ustring& text, F f, size_t n) const {
        Ustring dst;
        UnicornDetail::RegexTransform<F>()(*this, text, dst, f, n);
        return dst;
    }

    template <typename F>
    void Regex::transform_in(Ustring& text, F f, size_t n) const {
        Ustring dst;
        UnicornDetail::RegexTransform<F>()(*this, text, dst, f, n);
        text = move(dst);
    }

    inline void swap(Regex& lhs, Regex& rhs) noexcept { lhs.swap(rhs); }

    namespace Literals {

        inline Regex operator"" _re(const char* ptr, size_t len) { return Regex(cstr(ptr, len)); }
        inline Regex operator"" _re_b(const char* ptr, size_t len) { return Regex(cstr(ptr, len), Regex::byte); }
        inline Regex operator"" _re_i(const char* ptr, size_t len) { return Regex(cstr(ptr, len), Regex::caseless); }

    }

    // Regex formatting class

    class RegexFormat {
    public:
        RegexFormat() = default;
        RegexFormat(const Regex& pattern, const Ustring& format);
        RegexFormat(const Ustring& pattern, const Ustring& format, uint32_t flags = 0);
        Ustring operator()(const Ustring& text, size_t n = npos) const { return format(text, n); }
        uint32_t flags() const noexcept { return reg.flags(); }
        Ustring extract(const Ustring& text, size_t n = npos) const { return apply(text, n, false); }
        Ustring format() const { return fmt; }
        Ustring format(const Ustring& text, size_t n = npos) const { return apply(text, n, true); }
        Ustring pattern() const { return reg.pattern(); }
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
            Ustring text;
        };
        using sequence = std::vector<element>;
        Ustring fmt;
        Regex reg;
        sequence seq;
        void add_escape(char c);
        void add_literal(const Ustring& text);
        void add_literal(const Ustring& text, size_t offset, size_t count);
        void add_literal(char32_t u);
        void add_named(const Ustring& name) { seq.push_back({named, name}); }
        void add_tag(int tag) { seq.push_back({tag, {}}); }
        Ustring apply(const Ustring& text, size_t n, bool full) const;
        void parse();
    };

    inline void swap(RegexFormat& lhs, RegexFormat& rhs) noexcept { lhs.swap(rhs); }

    // Iterator over regex matches

    class MatchIterator:
    public ForwardIterator<MatchIterator, const Match> {
    public:
        MatchIterator() = default;
        MatchIterator(const Regex& re, const Ustring& text);
        const Match& operator*() const noexcept { return mat; }
        MatchIterator& operator++();
        friend bool operator==(const MatchIterator& lhs, const MatchIterator& rhs) noexcept;
    private:
        Match mat;
        Ustring pat;
    };

    // Iterator over substrings between matches

    class SplitIterator:
    public ForwardIterator<SplitIterator, const Ustring> {
    public:
        SplitIterator() = default;
        SplitIterator(const Regex& re, const Ustring& text);
        const Ustring& operator*() const noexcept { return value; }
        SplitIterator& operator++();
        friend bool operator==(const SplitIterator& lhs, const SplitIterator& rhs) noexcept;
    private:
        MatchIterator iter;
        size_t start = npos;
        Ustring value;
        void update();
    };

}
