#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include "unicorn/string.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if (defined(UNICORN_WCHAR_UTF16) && defined(UNICORN_PCRE16)) \
        || (defined(UNICORN_WCHAR_UTF32) && defined(UNICORN_PCRE32))
    #define UNICORN_PCRE_WCHAR 1
#endif

namespace Unicorn {

    // Class forward declarations and aliases

    template <typename CX> class BasicMatch;
    template <typename CX> class BasicMatchIterator;
    template <typename CX> class BasicRegex;
    template <typename CX> class BasicRegexFormat;
    template <typename CX> class BasicSplitIterator;

    using Match = BasicMatch<char>;
    using MatchIterator = BasicMatchIterator<char>;
    using Regex = BasicRegex<char>;
    using RegexFormat = BasicRegexFormat<char>;
    using SplitIterator = BasicSplitIterator<char>;
    using ByteMatch = BasicMatch<void>;
    using ByteMatchIterator = BasicMatchIterator<void>;
    using ByteRegex = BasicRegex<void>;
    using ByteRegexFormat = BasicRegexFormat<void>;
    using ByteSplitIterator = BasicSplitIterator<void>;

    #if defined(UNICORN_PCRE16)
        using Match16 = BasicMatch<char16_t>;
        using MatchIterator16 = BasicMatchIterator<char16_t>;
        using Regex16 = BasicRegex<char16_t>;
        using RegexFormat16 = BasicRegexFormat<char16_t>;
        using SplitIterator16 = BasicSplitIterator<char16_t>;
    #endif

    #if defined(UNICORN_PCRE32)
        using Match32 = BasicMatch<char32_t>;
        using MatchIterator32 = BasicMatchIterator<char32_t>;
        using Regex32 = BasicRegex<char32_t>;
        using RegexFormat32 = BasicRegexFormat<char32_t>;
        using SplitIterator32 = BasicSplitIterator<char32_t>;
    #endif

    #if defined(UNICORN_PCRE_WCHAR)
        using WideMatch = BasicMatch<wchar_t>;
        using WideMatchIterator = BasicMatchIterator<wchar_t>;
        using WideRegex = BasicRegex<wchar_t>;
        using WideRegexFormat = BasicRegexFormat<wchar_t>;
        using WideSplitIterator = BasicSplitIterator<wchar_t>;
    #endif

    namespace UnicornDetail {

        // Reference counted PCRE handle

        void inc_pcre(void* pc, char) noexcept;
        void dec_pcre(void* pc, void* ex, char) noexcept;

        #if defined(UNICORN_PCRE16)
            void inc_pcre(void* pc, char16_t) noexcept;
            void dec_pcre(void* pc, void* ex, char16_t) noexcept;
        #endif

        #if defined(UNICORN_PCRE32)
            void inc_pcre(void* pc, char32_t) noexcept;
            void dec_pcre(void* pc, void* ex, char32_t) noexcept;
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            void inc_pcre(void* pc, wchar_t) noexcept;
            void dec_pcre(void* pc, void* ex, wchar_t) noexcept;
        #endif

        template <typename C>
        class PcreRef {
        public:
            PcreRef() noexcept: pc(nullptr), ex(nullptr) {}
            PcreRef(void* p, void* x) noexcept: pc(p), ex(x) { inc(); }
            PcreRef(const PcreRef& p) noexcept: pc(p.pc), ex(p.ex) { inc(); }
            PcreRef(PcreRef&& p) noexcept: pc(p.pc), ex(p.ex) { p.pc = p.ex = nullptr; }
            ~PcreRef() noexcept { dec(); }
            PcreRef& operator=(const PcreRef& p) noexcept { auto q(p); swap(q); return *this; }
            PcreRef& operator=(PcreRef&& p) noexcept { auto q(p); swap(q); return *this; }
            void* pcre() const noexcept { return pc; }
            void* extra() const noexcept { return ex; }
            void swap(PcreRef& p) noexcept { std::swap(pc, p.pc); std::swap(ex, p.ex); }
            explicit operator bool() const noexcept { return pc; }
        private:
            void* pc;
            void* ex;
            void inc() noexcept { inc_pcre(pc, C()); }
            void dec() noexcept { dec_pcre(pc, ex, C()); }
        };

        // Internal data structures for regex and match classes

        template <typename C>
        struct RegexInfo {
            using string_type = basic_string<C>;
            using pcre_ref = PcreRef<C>;
            string_type pat {};
            uint32_t fset {};
            pcre_ref ref {};
        };

        template <typename C>
        void swap_info(RegexInfo<C>& r1, RegexInfo<C>& r2) noexcept {
            r1.pat.swap(r2.pat);
            std::swap(r1.fset, r2.fset);
            r1.ref.swap(r2.ref);
        }

        template <typename C>
        struct MatchInfo {
            using string_type = basic_string<C>;
            using pcre_ref = PcreRef<C>;
            vector<int> ofs {};
            uint32_t fset {};
            pcre_ref ref {};
            int status {-1};
            const string_type* text = nullptr;
        };

        template <typename C>
        size_t count_groups(const MatchInfo<C>& m) noexcept {
            return std::max(m.status, 0);
        }

        template <typename C>
        bool is_group(const MatchInfo<C>& m, size_t i) noexcept {
            return i < count_groups(m) && m.ofs[2 * i] >= 0 && m.ofs[2 * i + 1] >= 0;
        }

        template <typename C>
        size_t match_size(const MatchInfo<C>& m, size_t i) noexcept {
            return is_group(m, i) ? m.ofs[2 * i + 1] - m.ofs[2 * i] : 0;
        }

        template <typename C>
        void swap_info(MatchInfo<C>& m1, MatchInfo<C>& m2) noexcept {
            m1.ofs.swap(m2.ofs);
            std::swap(m1.fset, m2.fset);
            std::swap(m1.ref, m2.ref);
            std::swap(m1.status, m2.status);
            std::swap(m1.text, m2.text);
        }

        template <typename CX>
        struct SubjectIterator {
            using iterator = UtfIterator<CX>;
            static iterator make_iterator(const basic_string<CX>& text, size_t offset) noexcept { return utf_iterator(text, offset); }
        };

        template <>
        struct SubjectIterator<void> {
            using iterator = typename string::const_iterator;
            static iterator make_iterator(const string& text, size_t offset) noexcept { return text.begin() + offset; }
        };

        // Type-specific implementation wrapper functions

        size_t count_groups(const PcreRef<char>& p) noexcept;
        size_t named_group(const PcreRef<char>& p, const string& name) noexcept;
        void init_regex(RegexInfo<char>& r, const string& pattern, uint32_t flags, bool unicode);
        void init_match(MatchInfo<char>& m, const RegexInfo<char>& r, const string& text);
        void next_match(MatchInfo<char>& m, const string& pattern, size_t start, int anchors);

        #if defined(UNICORN_PCRE16)
            size_t count_groups(const PcreRef<char16_t>& p) noexcept;
            size_t named_group(const PcreRef<char16_t>& p, const u16string& name) noexcept;
            void init_regex(RegexInfo<char16_t>& r, const u16string& pattern, uint32_t flags, bool unicode);
            void init_match(MatchInfo<char16_t>& m, const RegexInfo<char16_t>& r, const u16string& text);
            void next_match(MatchInfo<char16_t>& m, const string& pattern, size_t start, int anchors);
        #endif

        #if defined(UNICORN_PCRE32)
            size_t count_groups(const PcreRef<char32_t>& p) noexcept;
            size_t named_group(const PcreRef<char32_t>& p, const u32string& name) noexcept;
            void init_regex(RegexInfo<char32_t>& r, const u32string& pattern, uint32_t flags, bool unicode);
            void init_match(MatchInfo<char32_t>& m, const RegexInfo<char32_t>& r, const u32string& text);
            void next_match(MatchInfo<char32_t>& m, const string& pattern, size_t start, int anchors);
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            size_t count_groups(const PcreRef<wchar_t>& p) noexcept;
            size_t named_group(const PcreRef<wchar_t>& p, const wstring& name) noexcept;
            void init_regex(RegexInfo<wchar_t>& r, const wstring& pattern, uint32_t flags, bool unicode);
            void init_match(MatchInfo<wchar_t>& m, const RegexInfo<wchar_t>& r, const wstring& text);
            void next_match(MatchInfo<wchar_t>& m, const string& pattern, size_t start, int anchors);
        #endif

        // Helper functions

        template <typename CX>
        struct CharType {
            using type = CX;
            static constexpr bool is_utf = true;
            static constexpr auto make_lower = &str_lowercase<CX>;
            static constexpr auto make_title = &str_titlecase<CX>;
            static constexpr auto make_upper = &str_uppercase<CX>;
        };

        template <>
        struct CharType<void> {
            using type = char;
            static constexpr bool is_utf = false;
            static constexpr auto make_lower = &ascii_lowercase;
            static constexpr auto make_title = &ascii_titlecase;
            static constexpr auto make_upper = &ascii_uppercase;
        };

        template <typename CX>
        struct MakeUtf8 {
            using string_type = basic_string<CX>;
            u8string operator()(const string_type& pattern) const { return to_utf8(pattern); }
        };

        template <>
        struct MakeUtf8<void> {
            using string_type = string;
            u8string operator()(const string& pattern) const {
                u32string p32;
                std::transform(PRI_BOUNDS(pattern), append(p32), [] (char c) { return uint8_t(c); });
                return to_utf8(p32);
            }
        };

        template <typename C>
        char32_t decode_escape(C c) {
            switch (c) {
                case PRI_CHAR('0', C):  return U'\0';
                case PRI_CHAR('e', C):  return 0x1b;
                default:                return char_to_uint(c);
            }
        }

        // Regex search function helpers

        template <typename Regex, typename Match, typename CX>
        struct RegexHelper {
            using utf_iterator = UtfIterator<CX>;
            Match anchor(const basic_string<CX>& text, size_t offset = 0) const
                { return static_cast<const Regex*>(this)->exec(text, offset, 1); }
            Match anchor(const utf_iterator& start) const
                { return anchor(start.source(), start.offset()); }
            Match match(const basic_string<CX>& text, size_t offset = 0) const
                { return static_cast<const Regex*>(this)->exec(text, offset, 2); }
            Match match(const utf_iterator& start) const
                { return match(start.source(), start.offset()); }
            Match search(const basic_string<CX>& text, size_t offset = 0) const
                { return static_cast<const Regex*>(this)->exec(text, offset, 0); }
            Match search(const utf_iterator& start) const
                { return search(start.source(), start.offset()); }
            Match operator()(const basic_string<CX>& text, size_t offset = 0) const
                { return search(text, offset); }
            Match operator()(const utf_iterator& start) const
                { return search(start.source(), start.offset()); }
        };

        template <typename Regex, typename Match>
        struct RegexHelper<Regex, Match, void> {
            Match anchor(const string& text, size_t offset = 0) const
                { return static_cast<const Regex*>(this)->exec(text, offset, 1); }
            Match match(const string& text, size_t offset = 0) const
                { return static_cast<const Regex*>(this)->exec(text, offset, 2); }
            Match search(const string& text, size_t offset = 0) const
                { return static_cast<const Regex*>(this)->exec(text, offset, 0); }
            Match operator()(const string& text, size_t offset = 0) const
                { return search(text, offset); }
        };

    }

    // Regex options

    UNICORN_DEFINE_FLAG(regex, rx_caseless, 0);          // Case insensitive matching                        PCRE_CASELESS
    UNICORN_DEFINE_FLAG(regex, rx_dfa, 1);               // Use the alternative DFA algorithm                pcre_dfa_exec()
    UNICORN_DEFINE_FLAG(regex, rx_dollarnewline, 2);     // $ may match line break at end                    ~PCRE_DOLLAR_ENDONLY
    UNICORN_DEFINE_FLAG(regex, rx_dotinline, 3);         // . does not match newlines                        ~PCRE_DOTALL
    UNICORN_DEFINE_FLAG(regex, rx_extended, 4);          // Free-form mode (ignore whitespace and comments)  PCRE_EXTENDED
    UNICORN_DEFINE_FLAG(regex, rx_firstline, 5);         // Must match in first line                         PCRE_FIRSTLINE
    UNICORN_DEFINE_FLAG(regex, rx_multiline, 6);         // Multiline mode (^ and $ match BOL/EOL)           PCRE_MULTILINE
    UNICORN_DEFINE_FLAG(regex, rx_newlineanycrlf, 7);    // Line break is any of CR, LF, CRLF                PCRE_NEWLINE_ANYCRLF
    UNICORN_DEFINE_FLAG(regex, rx_newlinecr, 8);         // Line break is CR only                            PCRE_NEWLINE_CR
    UNICORN_DEFINE_FLAG(regex, rx_newlinecrlf, 9);       // Line break is CRLF only                          PCRE_NEWLINE_CRLF
    UNICORN_DEFINE_FLAG(regex, rx_newlinelf, 10);        // Line break is LF only                            PCRE_NEWLINE_LF
    UNICORN_DEFINE_FLAG(regex, rx_noautocapture, 11);    // No automatic captures                            PCRE_NO_AUTO_CAPTURE
    UNICORN_DEFINE_FLAG(regex, rx_nostartoptimize, 12);  // No startup optimization                          PCRE_NO_START_OPTIMIZE
    UNICORN_DEFINE_FLAG(regex, rx_notbol, 13);           // Start of text is not a line break                PCRE_NOTBOL
    UNICORN_DEFINE_FLAG(regex, rx_notempty, 14);         // Do not match an empty string                     PCRE_NOTEMPTY
    UNICORN_DEFINE_FLAG(regex, rx_notemptyatstart, 15);  // Match an empty string only at the start          PCRE_NOTEMPTY_ATSTART
    UNICORN_DEFINE_FLAG(regex, rx_noteol, 16);           // End of text is not a line break                  PCRE_NOTEOL
    UNICORN_DEFINE_FLAG(regex, rx_noutfcheck, 17);       // Skip UTF validity checks                         PCRE_NO_UTF{8,16,32}_CHECK
    UNICORN_DEFINE_FLAG(regex, rx_optimize, 18);         // Take extra effort to optimize the regex          PCRE_STUDY_JIT_COMPILE
    UNICORN_DEFINE_FLAG(regex, rx_partialhard, 19);      // Hard partial matching (prefer over full match)   PCRE_PARTIAL_HARD
    UNICORN_DEFINE_FLAG(regex, rx_partialsoft, 20);      // Soft partial matching (only if no full match)    PCRE_PARTIAL_SOFT
    UNICORN_DEFINE_FLAG(regex, rx_prefershort, 21);      // Non-greedy quantifiers, or shorter DFA matches   PCRE_UNGREEDY,PCRE_DFA_SHORTEST
    UNICORN_DEFINE_FLAG(regex, rx_ucp, 22);              // Use Unicode properties in escape charsets        PCRE_UCP

    // Exceptions

    class RegexError:
    public std::runtime_error {
    public:
        RegexError(const u8string& pattern, int error, const u8string& message):
            std::runtime_error(assemble(pattern, error, message)), pat(make_shared<u8string>(pattern)),
            err(error), msg(make_shared<u8string>(message)) {}
        const char* pattern() const noexcept { return pat->data(); }
        int error() const noexcept { return err; }
        const char* message() const noexcept { return msg->data(); }
    private:
        shared_ptr<u8string> pat;
        int err;
        shared_ptr<u8string> msg;
        static u8string assemble(const u8string& pattern, int error, u8string message);
        static u8string translate(int error);
    };

    // Regex match class

    template <typename CX>
    class BasicMatch:
    private UnicornDetail::MatchInfo<typename UnicornDetail::CharType<CX>::type> {
    public:
        using char_type = typename UnicornDetail::CharType<CX>::type;
        using cx_type = CX;
        using string_type = basic_string<char_type>;
        using regex_type = BasicRegex<CX>;
        using const_iterator = const char_type*;
        using const_reference = const char_type&;
        using difference_type = ptrdiff_t;
        using iterator = typename UnicornDetail::SubjectIterator<CX>::iterator;
        using reference = typename std::iterator_traits<iterator>::reference;
        using value_type = typename std::iterator_traits<iterator>::value_type;
        string_type operator[](size_t i) const { return str(i); }
        operator string_type() const { return str(); }
        explicit operator bool() const noexcept { return matched(); }
        bool operator!() const noexcept { return ! matched(); }
        iterator begin(size_t i = 0) const noexcept;
        size_t count(size_t i = 0) const noexcept { return match_size(*this, i); }
        bool empty() const noexcept { return ! *this || this->ofs[0] == this->ofs[1]; }
        iterator end(size_t i = 0) const noexcept;
        size_t endpos(size_t i = 0) const noexcept { return is_group(*this, i) ? this->ofs[2 * i + 1] : npos; }
        string_type first() const;
        bool full_or_partial() const noexcept { return matched() || partial(); }
        size_t groups() const noexcept { return count_groups(*this); }
        string_type last() const;
        bool matched(size_t i = 0) const noexcept { return this->status >= 0 && (i == 0 || is_group(*this, i)); }
        string_type named(const string_type& name) const { return this->ref ? str(named_group(this->ref, name)) : string_type(); }
        size_t offset(size_t i = 0) const noexcept { return is_group(*this, i) ? this->ofs[2 * i] : npos; }
        bool partial() const noexcept { return this->status == -12; } // PCRE_ERROR_PARTIAL
        string_type str(size_t i = 0) const;
        void swap(BasicMatch& m) noexcept { swap_info(m); }
    private:
        friend class BasicMatchIterator<CX>;
        friend class BasicRegex<CX>;
        friend class BasicSplitIterator<CX>;
    };

    template <typename CX>
    typename BasicMatch<CX>::iterator BasicMatch<CX>::begin(size_t i) const noexcept {
        if (this->text && is_group(*this, i))
            return UnicornDetail::SubjectIterator<CX>::make_iterator(*this->text, offset(i));
        else
            return {};
    }

    template <typename CX>
    typename BasicMatch<CX>::iterator BasicMatch<CX>::end(size_t i) const noexcept {
        if (this->text && is_group(*this, i))
            return UnicornDetail::SubjectIterator<CX>::make_iterator(*this->text, endpos(i));
        else
            return {};
    }

    template <typename CX>
    typename BasicMatch<CX>::string_type BasicMatch<CX>::first() const {
        if (! matched() || ! this->text)
            return {};
        size_t n = groups();
        for (size_t i = 1; i < n; ++i)
            if (is_group(*this, i) && this->ofs[2 * i + 1] > this->ofs[2 * i])
                return this->text->substr(this->ofs[2 * i], this->ofs[2 * i + 1] - this->ofs[2 * i]);
        return {};
    }

    template <typename CX>
    typename BasicMatch<CX>::string_type BasicMatch<CX>::last() const {
        if (! matched() || ! this->text)
            return {};
        size_t n = groups();
        for (size_t i = n - 1; i > 0; --i)
            if (is_group(*this, i) && this->ofs[2 * i + 1] > this->ofs[2 * i])
                return this->text->substr(this->ofs[2 * i], this->ofs[2 * i + 1] - this->ofs[2 * i]);
        return {};
    }

    template <typename CX>
    typename BasicMatch<CX>::string_type BasicMatch<CX>::str(size_t i) const {
        if (this->text && is_group(*this, i))
            return this->text->substr(this->ofs[2 * i],
                this->ofs[2 * i + 1] - this->ofs[2 * i]);
        else
            return {};
    }

    template <typename CX>
    void swap(BasicMatch<CX>& lhs, BasicMatch<CX>& rhs) noexcept {
        lhs.swap(rhs);
    }

    // Regular expression class

    template <typename CX>
    class BasicRegex:
    public UnicornDetail::RegexHelper<BasicRegex<CX>, BasicMatch<CX>, CX>,
    public LessThanComparable<BasicRegex<CX>>,
    private UnicornDetail::RegexInfo<typename UnicornDetail::CharType<CX>::type> {
    private:
        using subject_iterator = UnicornDetail::SubjectIterator<CX>;
    public:
        using char_type = typename UnicornDetail::CharType<CX>::type;
        using cx_type = CX;
        using string_type = basic_string<char_type>;
        using match_type = BasicMatch<CX>;
        using match_iterator = BasicMatchIterator<CX>;
        using match_range = Irange<match_iterator>;
        using split_iterator = BasicSplitIterator<CX>;
        using split_range = Irange<split_iterator>;
        BasicRegex() { init_regex(*this, {}, {}, is_utf); }
        explicit BasicRegex(const string_type& pattern, uint32_t flags = 0) { init_regex(*this, pattern, flags, is_utf); }
        size_t count(const string_type& text) const;
        bool empty() const noexcept { return this->pat.empty(); }
        string_type extract(const string_type& fmt, const string_type& text, size_t n = npos) const
            { return BasicRegexFormat<CX>(*this, fmt).extract(text, n); }
        string_type format(const string_type& fmt, const string_type& text, size_t n = npos) const
            { return BasicRegexFormat<CX>(*this, fmt).format(text, n); }
        match_range grep(const string_type& text) const { return {{*this, text}, {}}; }
        size_t groups() const noexcept { return UnicornDetail::count_groups(this->ref); }
        size_t named(const string_type& name) const noexcept { return UnicornDetail::named_group(this->ref, name); }
        string_type pattern() const { return this->pat; }
        uint32_t flags() const noexcept { return this->fset; }
        split_range split(const string_type& text) const { return {{*this, text}, {}}; }
        void swap(BasicRegex& r) noexcept { swap_info(r); }
        friend bool operator==(const BasicRegex& lhs, const BasicRegex& rhs) noexcept
            { return lhs.pat == rhs.pat && lhs.fset == rhs.fset; }
        friend bool operator<(const BasicRegex& lhs, const BasicRegex& rhs) noexcept
            { return lhs.pat == rhs.pat ? lhs.fset < rhs.fset : lhs.pat < rhs.pat; }
    private:
        friend class BasicMatchIterator<CX>;
        friend class BasicMatch<CX>;
        friend struct UnicornDetail::RegexHelper<BasicRegex, match_type, CX>;
        static constexpr bool is_utf = UnicornDetail::CharType<CX>::is_utf;
        match_type exec(const string_type& text, size_t offset, int anchors) const;
        u8string utf8_pattern() const { return UnicornDetail::MakeUtf8<CX>()(this->pat); }
    };

    template <typename CX>
    size_t BasicRegex<CX>::count(const string_type& text) const {
        match_type m;
        init_match(m, *this, text);
        size_t n = 0, ofs = 0;
        for (;;) {
            next_match(m, utf8_pattern(), ofs, 0);
            if (! m)
                break;
            ++n;
            ofs = std::max(m.endpos(), ofs + 1);
        }
        return n;
    }

    template <typename CX>
    BasicMatch<CX> BasicRegex<CX>::exec(const string_type& text, size_t offset, int anchors) const {
        match_type m;
        init_match(m, *this, text);
        next_match(m, utf8_pattern(), offset, anchors);
        return m;
    }

    template <typename C>
    BasicRegex<C> regex(const basic_string<C>& pattern, uint32_t flags = 0) {
        return BasicRegex<C>(pattern, flags);
    }

    template <typename C>
    BasicRegex<C> regex(const C* pattern, uint32_t flags = 0) {
        return BasicRegex<C>(cstr(pattern), flags);
    }

    template <typename CX>
    void swap(BasicRegex<CX>& lhs, BasicRegex<CX>& rhs) noexcept {
        lhs.swap(rhs);
    }

    namespace Literals {

        inline auto operator"" _re(const char* ptr, size_t len) { return Regex(cstr(ptr, len)); }
        inline auto operator"" _re_i(const char* ptr, size_t len) { return Regex(cstr(ptr, len), rx_caseless); }
        inline auto operator"" _re(const char16_t* ptr, size_t len) { return Regex16(cstr(ptr, len)); }
        inline auto operator"" _re_i(const char16_t* ptr, size_t len) { return Regex16(cstr(ptr, len), rx_caseless); }
        inline auto operator"" _re(const char32_t* ptr, size_t len) { return Regex32(cstr(ptr, len)); }
        inline auto operator"" _re_i(const char32_t* ptr, size_t len) { return Regex32(cstr(ptr, len), rx_caseless); }
        inline auto operator"" _re(const wchar_t* ptr, size_t len) { return WideRegex(cstr(ptr, len)); }
        inline auto operator"" _re_i(const wchar_t* ptr, size_t len) { return WideRegex(cstr(ptr, len), rx_caseless); }
        inline auto operator"" _re_b(const char* ptr, size_t len) { return ByteRegex(cstr(ptr, len)); }
        inline auto operator"" _re_bi(const char* ptr, size_t len) { return ByteRegex(cstr(ptr, len), rx_caseless); }

    }

    // Regex formatting class

    template <typename CX>
    class BasicRegexFormat {
    public:
        using char_type = typename UnicornDetail::CharType<CX>::type;
        using cx_type = CX;
        using string_type = basic_string<char_type>;
        using regex_type = BasicRegex<CX>;
        using match_type = BasicMatch<CX>;
        BasicRegexFormat() = default;
        BasicRegexFormat(const regex_type& pattern, const string_type& format): fmt(format), reg(pattern), seq() { parse(); }
        BasicRegexFormat(const string_type& pattern, const string_type& format, uint32_t flags = 0):
            BasicRegexFormat(regex_type(pattern, flags), format) {}
        string_type operator()(const string_type& text, size_t n = npos) const { return format(text, n); }
        uint32_t flags() const noexcept { return reg.flags(); }
        string_type extract(const string_type& text, size_t n = npos) const { return apply(text, n, false); }
        string_type format() const { return fmt; }
        string_type format(const string_type& text, size_t n = npos) const { return apply(text, n, true); }
        string_type pattern() const { return reg.pattern(); }
        regex_type regex() const { return reg; }
        void swap(BasicRegexFormat& r) noexcept;
    private:
        static constexpr bool is_utf = UnicornDetail::CharType<CX>::is_utf;
        // Index field in element record indicates what to substitute
        // If index>=0, this is a numbered capture group
        enum tag_type {
            literal   = -1,           // Literal text
            named     = -2,           // Nameed capture group
            reset     = - int('E'),   // \E = End of delimited text
            lower     = - int('L'),   // \L = Convert delimited text to lower case
            title     = - int('T'),   // \T = Convert delimited text to title case
            upper     = - int('U'),   // \U = Convert delimited text to upper case
            lower1    = - int('l'),   // \l = Convert next character to lower case
            upper1    = - int('u'),   // \u = Convert next character to upper case
            first     = - int('-'),   // $- = First non-empty capture group
            last      = - int('+'),   // $+ = Last non-empty capture group
            prefix    = - int('<'),   // $< = Text between previous match and this one
            suffix    = - int('>'),   // $> = Text between this match and the next one
            before1   = - int('['),   // $[ = Text before this match
            after1    = - int(']'),   // $] = Text after this match
            before2   = - int('`'),   // $` = Text before this match
            after2    = - int('\''),  // $' = Text after this match
            complete  = - int('_'),   // $_ = Complete subject string
        };
        struct element {
            int index;
            string_type text;
        };
        using sequence = vector<element>;
        string_type fmt;
        regex_type reg;
        sequence seq;
        void add_literal(const string_type& text);
        void add_literal(const string_type& text, size_t offset, size_t count);
        void add_literal(char32_t u);
        void add_named(const string_type& name) { seq.push_back({named, name}); }
        void add_tag(int tag) { seq.push_back({tag, {}}); }
        string_type apply(const string_type& text, size_t n, bool full) const;
        void parse();
    };

    template <typename CX>
    void BasicRegexFormat<CX>::swap(BasicRegexFormat<CX>& r) noexcept {
        fmt.swap(r.fmt);
        reg.swap(r.reg);
        seq.swap(r.seq);
    }

    template <typename CX>
    void BasicRegexFormat<CX>::add_literal(const string_type& text) {
        if (! text.empty()) {
            if (seq.empty() || seq.back().index != literal)
                seq.push_back({literal, text});
            else
                seq.back().text += text;
        }
    }

    template <typename CX>
    void BasicRegexFormat<CX>::add_literal(const string_type& text, size_t offset, size_t count) {
        if (offset < text.size() && count > 0)
            add_literal(text.substr(offset, count));
    }

    template <typename CX>
    void BasicRegexFormat<CX>::add_literal(char32_t u) {
        if (seq.empty() || seq.back().index != literal)
            seq.push_back({literal, {}});
        if (is_utf)
            str_append_char(seq.back().text, u);
        else
            seq.back().text += char_type(u);
    }

    template <typename CX>
    typename BasicRegexFormat<CX>::string_type
    BasicRegexFormat<CX>::apply(const string_type& text, size_t n, bool full) const {
        using namespace UnicornDetail;
        string_type block, dst;
        string_type* current = &dst;
        int block_flag = 0, char_flag = 0;
        auto end_block = [&] {
            if (current == &dst)
                return;
            switch (block_flag) {
                case lower: dst += CharType<CX>::make_lower(block); break;
                case title: dst += CharType<CX>::make_title(block); break;
                case upper: dst += CharType<CX>::make_upper(block); break;
                default: dst += block; break;
            }
            block.clear();
            block_flag = 0;
            current = &dst;
        };
        auto new_block = [&] (int tag) {
            end_block();
            block_flag = tag;
            current = &block;
        };
        vector<match_type> matches;
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
                string_type fragment;
                switch (elem.index) {
                    case literal:                        fragment = elem.text; break;
                    case named:                          fragment = m.named(elem.text); break;
                    case reset:                          end_block(); break;
                    case lower: case title: case upper:  new_block(elem.index); break;
                    case lower1: case upper1:            char_flag = elem.index; break;
                    case prefix:                         fragment = text.substr(prev, m.offset() - prev); break;
                    case suffix:                         fragment = text.substr(m.endpos(), next - m.endpos()); break;
                    case before1: case before2:          fragment = text.substr(0, m.offset()); break;
                    case after1: case after2:            fragment = text.substr(m.endpos(), npos); break;
                    case first:                          fragment = m.first(); break;
                    case last:                           fragment = m.last(); break;
                    case complete:                       fragment = text; break;
                    default:                             fragment = m.str(elem.index); break;
                }
                if (fragment.empty())
                    continue;
                if (char_flag) {
                    auto it = utf_begin(fragment);
                    char32_t buf[max_case_decomposition];
                    if (is_utf) {
                        size_t nbuf(char_flag == lower1 ? char_to_full_lowercase(*it, buf) : char_to_full_uppercase(*it, buf));
                        str_append(*current, buf, nbuf);
                    } else {
                        *current += char_flag == lower1 ? ascii_tolower(*it) : ascii_toupper(*it);
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

    template <typename CX>
    void BasicRegexFormat<CX>::parse() {
        using namespace UnicornDetail;
        static const u8string pattern_ascii =
            R"((\$[0&]))"                                    // (1) Match ($0,$&)
            R"(|(?:\$(\d+)))"                                // (2) Numbered capture ($number)
            R"(|(?:\\([1-9])))"                              // (3) Numbered capture (\digit)
            R"(|(?:\$\{(\d+)\}))"                            // (4) Numbered capture (${number})
            R"(|(?:\$([A-Za-z][0-9A-Za-z_]*)))"              // (5) Named capture ($name)
            R"(|(?:\$\{([^{}]+)\}))"                         // (6) Named capture (${name})
            R"(|(?:\$([-+<>\[\]`'_])))"                      // (7) Substring ($-,$+,$<,$>,$[,$],$`,$',$_)
            R"(|(?:\\x([[:xdigit:]]{2})))"                   // (8) Escape code (\xHH)
            R"(|(?:\\x\{([[:xdigit:]]+)\}))"                 // (9) Escape code (\x{HHH...})
            R"(|(?:\\([0e])))"                               // (10) Escape code
            R"(|(?:\\([ELTUlu])))"                           // (11) Escape code
            R"(|(?:\\Q((?:[^\\]|\\(?:[^E]|$))*)(?:\\E|$)))"  // (12) Literal text
            R"(|(?:[\\$](.)))";                              // (13) Unknown
        static const regex_type pattern(recode<char_type>(pattern_ascii), rx_optimize);
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
                add_literal(decode_escape(match[10][0]));
            else if (match.count(11))
                add_tag(- int(match[11][0]));
            else if (match.count(12))
                add_literal(match[12]);
            else
                add_literal(match[13]);
            prev = match.endpos();
        }
    }

    template <typename C>
    BasicRegexFormat<C> regex_format(const basic_string<C>& pattern, const basic_string<C>& format, uint32_t flags = 0) {
        return {pattern, format, flags};
    }

    template <typename C>
    BasicRegexFormat<C> regex_format(const basic_string<C>& pattern, const C* format, uint32_t flags = 0) {
        return {pattern, cstr(format), flags};
    }

    template <typename C>
    BasicRegexFormat<C> regex_format(const C* pattern, const basic_string<C>& format, uint32_t flags = 0) {
        return {cstr(pattern), format, flags};
    }

    template <typename C>
    BasicRegexFormat<C> regex_format(const C* pattern, const C* format, uint32_t flags = 0) {
        return {cstr(pattern), cstr(format), flags};
    }

    template <typename CX>
    void swap(BasicRegexFormat<CX>& lhs, BasicRegexFormat<CX>& rhs) noexcept {
        lhs.swap(rhs);
    }

    // Iterator over regex matches

    template <typename CX>
    class BasicMatchIterator:
    public ForwardIterator<BasicMatchIterator<CX>, const BasicMatch<CX>> {
    public:
        using char_type = typename UnicornDetail::CharType<CX>::type;
        using cx_type = CX;
        using string_type = basic_string<char_type>;
        using regex_type = BasicRegex<CX>;
        using match_type = BasicMatch<CX>;
        BasicMatchIterator() = default;
        BasicMatchIterator(const regex_type& re, const string_type& text): mat(re.search(text)), pat(re.utf8_pattern()) {}
        const match_type& operator*() const noexcept { return mat; }
        BasicMatchIterator& operator++();
        friend bool operator==(const BasicMatchIterator& lhs, const BasicMatchIterator& rhs) noexcept
            { return bool(lhs.mat) == bool(rhs.mat) && (! lhs.mat || lhs.mat.offset() == rhs.mat.offset()); }
    private:
        match_type mat;
        string pat;
    };

    template <typename CX>
    BasicMatchIterator<CX>& BasicMatchIterator<CX>::operator++() {
        if (mat)
            next_match(mat, pat, mat.offset() + std::max(mat.count(), size_t(1)), 0);
        return *this;
    }

    // Iterator over substrings between matches

    template <typename CX>
    class BasicSplitIterator:
    public ForwardIterator<BasicSplitIterator<CX>, const basic_string<typename UnicornDetail::CharType<CX>::type>> {
    public:
        using char_type = typename UnicornDetail::CharType<CX>::type;
        using cx_type = CX;
        using string_type = basic_string<char_type>;
        using regex_type = BasicRegex<CX>;
        using match_type = BasicMatch<CX>;
        using match_iterator = BasicMatchIterator<CX>;
        BasicSplitIterator() = default;
        BasicSplitIterator(const regex_type& re, const string_type& text): iter(re, text), start(0), value() { update(); }
        const string_type& operator*() const noexcept { return value; }
        BasicSplitIterator& operator++();
        friend bool operator==(const BasicSplitIterator& lhs, const BasicSplitIterator& rhs) noexcept
            { return lhs.iter == rhs.iter && lhs.start == rhs.start; }
    private:
        match_iterator iter;
        size_t start = npos;
        string_type value;
        void update();
    };

    template <typename CX>
    BasicSplitIterator<CX>& BasicSplitIterator<CX>::operator++() {
        start = *iter ? iter->endpos() : npos;
        ++iter;
        update();
        return *this;
    }

    template <typename CX>
    void BasicSplitIterator<CX>::update() {
        if (! iter->text || start >= iter->text->size())
            value.clear();
        else if (*iter)
            value.assign(*iter->text, start, iter->offset() - start);
        else
            value.assign(*iter->text, start, npos);
    }

    // Utility functions

    namespace UnicornDetail {

        template <typename C>
        void escape_char(C c, basic_string<C>& dst) {
            static constexpr const char* hexdigits = "0123456789abcdef";
            switch (c) {
                case 0:
                    dst += PRI_CHAR('\\', C);
                    dst += PRI_CHAR('0', C);
                    break;
                case PRI_CHAR('\t', C):
                    dst += PRI_CHAR('\\', C);
                    dst += PRI_CHAR('t', C);
                    break;
                case PRI_CHAR('\n', C):
                    dst += PRI_CHAR('\\', C);
                    dst += PRI_CHAR('n', C);
                    break;
                case PRI_CHAR('\f', C):
                    dst += PRI_CHAR('\\', C);
                    dst += PRI_CHAR('f', C);
                    break;
                case PRI_CHAR('\r', C):
                    dst += PRI_CHAR('\\', C);
                    dst += PRI_CHAR('r', C);
                    break;
                case PRI_CHAR('\"', C): case PRI_CHAR('$', C): case PRI_CHAR('\'', C): case PRI_CHAR('(', C): case PRI_CHAR(')', C):
                case PRI_CHAR('*', C): case PRI_CHAR('+', C): case PRI_CHAR('.', C): case PRI_CHAR('?', C): case PRI_CHAR('[', C):
                case PRI_CHAR('\\', C): case PRI_CHAR(']', C): case PRI_CHAR('^', C): case PRI_CHAR('{', C): case PRI_CHAR('|', C):
                case PRI_CHAR('}', C):
                    dst += PRI_CHAR('\\', C);
                    dst += c;
                    break;
                default:
                    if (char_to_uint(c) <= 31 || c == 127) {
                        dst += PRI_CHAR('\\', C);
                        dst += PRI_CHAR('x', C);
                        dst += static_cast<C>(hexdigits[(c >> 4) & 15]);
                        dst += static_cast<C>(hexdigits[c & 15]);
                    } else {
                        dst += c;
                    }
                    break;
            }
        }

    }

    template <typename C>
    basic_string<C> regex_escape(const basic_string<C>& str) {
        basic_string<C> dst;
        for (auto c: str)
            UnicornDetail::escape_char(c, dst);
        return dst;
    }

    template <typename C>
    basic_string<C> regex_escape(const C* str) {
        basic_string<C> dst;
        if (str)
            while (*str)
                UnicornDetail::escape_char(*str++, dst);
        return dst;
    }

    // Version information

    Version regex_version() noexcept;
    Version regex_unicode_version() noexcept;

}
