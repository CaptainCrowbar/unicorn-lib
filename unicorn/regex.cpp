#include "unicorn/regex.hpp"
#include <new>
#include <pcre.h>

using namespace std::literals;

namespace Unicorn {

    namespace {

        // PCRE error messages

        const char* const error_table[] {
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

        // PCRE functions for each character type

        template <typename T> struct PcreTraits;

        template <> struct PcreTraits<char> {
            using base_type = pcre;
            using extra_type = pcre_extra;
            using ccptr_type = const char*;
            using char_type = char;
            using string_type = string;
            static constexpr auto compile2 = &pcre_compile2;
            static constexpr auto study = &pcre_study;
            static constexpr auto free_study = &pcre_free_study;
            static constexpr auto exec = &pcre_exec;
            static constexpr auto dfa_exec = &pcre_dfa_exec;
            static constexpr auto get_stringnumber = &pcre_get_stringnumber;
            static constexpr auto fullinfo = &pcre_fullinfo;
            static constexpr auto refcount = &pcre_refcount;
            static void free(void* p) { pcre_free(p); }
        };

        #if defined(UNICORN_PCRE16)
            template <> struct PcreTraits<char16_t> {
                using base_type = pcre16;
                using extra_type = pcre16_extra;
                using ccptr_type = PCRE_SPTR16;
                using char_type = char16_t;
                using string_type = u16string;
                static constexpr auto compile2 = &pcre16_compile2;
                static constexpr auto study = &pcre16_study;
                static constexpr auto free_study = &pcre16_free_study;
                static constexpr auto exec = &pcre16_exec;
                static constexpr auto dfa_exec = &pcre16_dfa_exec;
                static constexpr auto get_stringnumber = &pcre16_get_stringnumber;
                static constexpr auto fullinfo = &pcre16_fullinfo;
                static constexpr auto refcount = &pcre16_refcount;
                static void free(void* p) { pcre16_free(p); }
            };
        #endif

        #if defined(UNICORN_PCRE32)
            template <> struct PcreTraits<char32_t> {
                using base_type = pcre32;
                using extra_type = pcre32_extra;
                using ccptr_type = PCRE_SPTR32;
                using char_type = char32_t;
                using string_type = u32string;
                static constexpr auto compile2 = &pcre32_compile2;
                static constexpr auto study = &pcre32_study;
                static constexpr auto free_study = &pcre32_free_study;
                static constexpr auto exec = &pcre32_exec;
                static constexpr auto dfa_exec = &pcre32_dfa_exec;
                static constexpr auto get_stringnumber = &pcre32_get_stringnumber;
                static constexpr auto fullinfo = &pcre32_fullinfo;
                static constexpr auto refcount = &pcre32_refcount;
                static void free(void* p) { pcre32_free(p); }
            };
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            template <> struct PcreTraits<wchar_t>:
            public PcreTraits<WcharEquivalent> {
                using char_type = wchar_t;
                using string_type = wstring;
            };
        #endif

        template <typename P, typename T>
        typename P::ccptr_type make_ccptr(const T* t) {
            auto p = reinterpret_cast<typename P::ccptr_type>(t);
            CROW_STATIC_ASSERT(sizeof(*p) == sizeof(T));
            return p;
        }

    }

    namespace UnicornDetail {

        namespace {

            // Implementation of PCRE reference counting

            template <typename C>
            void inc_pcre_impl(void* p) {
                using P = PcreTraits<C>;
                auto pcre = static_cast<typename P::base_type*>(p);
                if (pcre)
                    P::refcount(pcre, 1);
            }

            template <typename C>
            void dec_pcre_impl(void* p, void* ex) {
                using P = PcreTraits<C>;
                auto pcre = static_cast<typename P::base_type*>(p);
                auto extra = static_cast<typename P::extra_type*>(ex);
                if (pcre && P::refcount(pcre, -1) == 0) {
                    if (extra)
                        P::free_study(extra);
                    (*P::free)(pcre);
                }
            }

            template <typename C>
            size_t count_groups_impl(void* p) noexcept {
                using P = PcreTraits<C>;
                if (! p)
                    return 0;
                auto pcre = static_cast<typename P::base_type*>(p);
                int n = 0;
                P::fullinfo(pcre, nullptr, PCRE_INFO_CAPTURECOUNT, &n);
                return n + 1;
            }

            template <typename C>
            size_t named_group_impl(void* p, const basic_string<C>& name) noexcept {
                using P = PcreTraits<C>;
                if (! p)
                    return npos;
                auto pcre = static_cast<typename P::base_type*>(p);
                auto rc = P::get_stringnumber(pcre, make_ccptr<P>(name.data()));
                return rc >= 0 ? rc : npos;
            }

            // Implementations of regex algorithms

            template <typename C>
            void init_regex_impl(RegexInfo<C>& r, const typename PcreTraits<C>::string_type& pattern,
                    Crow::Flagset flags, bool unicode) {
                using pcre_traits = PcreTraits<C>;
                flags.allow(rx_caseless | rx_dfa | rx_dollarnewline | rx_dotinline | rx_extended
                    | rx_firstline | rx_multiline | rx_newlineanycrlf | rx_newlinecr | rx_newlinecrlf
                    | rx_newlinelf | rx_noautocapture | rx_nostartoptimize | rx_notbol | rx_notempty
                    | rx_notemptyatstart | rx_noteol | rx_noutfcheck | rx_optimize | rx_partialhard
                    | rx_partialsoft | rx_prefershort | rx_ucp, "regex");
                flags.exclusive(rx_newlineanycrlf | rx_newlinecr | rx_newlinecrlf | rx_newlinelf, "regex");
                flags.exclusive(rx_notempty | rx_notemptyatstart, "regex");
                flags.exclusive(rx_partialhard | rx_partialsoft, "regex");
                if (! unicode && flags.get(rx_noutfcheck | rx_ucp))
                    throw Crow::FlagError(flags, "byte regex");
                r.pat = pattern;
                r.fset = flags;
                int cflags = PCRE_EXTRA, sflags = 0;
                if (unicode)
                    cflags |= PCRE_UTF8;
                if (flags.get(rx_newlineanycrlf))
                    cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_ANYCRLF;
                else if (flags.get(rx_newlinecr))
                    cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_CR;
                else if (flags.get(rx_newlinecrlf))
                    cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_CRLF;
                else if (flags.get(rx_newlinelf))
                    cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_LF;
                else
                    cflags |= PCRE_BSR_ANYCRLF | PCRE_NEWLINE_ANYCRLF;
                if (flags.get(rx_optimize)) {
                    sflags |= PCRE_STUDY_JIT_COMPILE;
                    if (flags.get(rx_partialhard))
                        sflags |= PCRE_STUDY_JIT_PARTIAL_HARD_COMPILE;
                    else if (flags.get(rx_partialsoft))
                        sflags |= PCRE_STUDY_JIT_PARTIAL_SOFT_COMPILE;
                }
                if (flags.get(rx_caseless))
                    cflags |= PCRE_CASELESS;
                if (! flags.get(rx_dollarnewline))
                    cflags |= PCRE_DOLLAR_ENDONLY;
                if (! flags.get(rx_dotinline))
                    cflags |= PCRE_DOTALL;
                if (flags.get(rx_extended))
                    cflags |= PCRE_EXTENDED;
                if (flags.get(rx_firstline))
                    cflags |= PCRE_FIRSTLINE;
                if (flags.get(rx_multiline))
                    cflags |= PCRE_MULTILINE;
                if (flags.get(rx_noautocapture))
                    cflags |= PCRE_NO_AUTO_CAPTURE;
                if (flags.get(rx_nostartoptimize))
                    cflags |= PCRE_NO_START_OPTIMIZE;
                if (flags.get(rx_noutfcheck))
                    cflags |= PCRE_NO_UTF8_CHECK;
                if (flags.get(rx_prefershort))
                    cflags |= PCRE_UNGREEDY;
                if (flags.get(rx_ucp))
                    cflags |= PCRE_UCP;
                int error = 0, errpos = 0;
                const char* errptr = nullptr;
                auto pc = pcre_traits::compile2(make_ccptr<pcre_traits>(pattern.data()),
                    cflags, &error, &errptr, &errpos, nullptr);
                if (! pc) {
                    if (error == 21)
                        throw std::bad_alloc();
                    else if (unicode)
                        throw RegexError(to_utf8(pattern), error, Crow::cstr(errptr));
                }
                auto ex = pcre_traits::study(pc, sflags, &errptr);
                r.ref = {pc, ex};
            }

            template <typename C>
            void init_match_impl(MatchInfo<C>& m, const RegexInfo<C>& r,
                    const typename MatchInfo<C>::string_type& text) {
                m.ofs.clear();
                m.fset = r.fset;
                m.ref = r.ref;
                m.status = -1;
                m.text = &text;
            }

            int match_flags(Crow::Flagset fset) {
                int mask = 0;
                if (fset.get(rx_partialhard))
                    mask |= PCRE_PARTIAL_HARD;
                if (fset.get(rx_partialsoft))
                    mask |= PCRE_PARTIAL_SOFT;
                if (fset.get(rx_notbol))
                    mask |= PCRE_NOTBOL;
                if (fset.get(rx_notempty))
                    mask |= PCRE_NOTEMPTY;
                if (fset.get(rx_notemptyatstart))
                    mask |= PCRE_NOTEMPTY_ATSTART;
                if (fset.get(rx_noteol))
                    mask |= PCRE_NOTEOL;
                if (fset.get(rx_nostartoptimize))
                    mask |= PCRE_NO_START_OPTIMIZE;
                return mask;
            }

            template <typename C>
            void next_match_impl(MatchInfo<C>& m, const string& pattern, size_t start, int anchors) {
                using pcre_traits = PcreTraits<C>;
                m.status = PCRE_ERROR_NOMATCH;
                if (! m.ref || start > m.text->size())
                    return;
                auto pc = static_cast<typename pcre_traits::base_type*>(m.ref.pcre());
                auto ex = static_cast<typename pcre_traits::extra_type*>(m.ref.extra());
                int xflags = 0;
                if (anchors > 0)
                    xflags |= PCRE_ANCHORED;
                if (m.fset.get(rx_dfa)) {
                    if (m.fset.get(rx_prefershort))
                        xflags |= PCRE_DFA_SHORTEST;
                    if (m.ofs.size() < 40)
                        m.ofs.resize(40); // ovector + workspace
                    for (;;) {
                        auto half = static_cast<int>(m.ofs.size() / 2);
                        m.status = pcre_traits::dfa_exec(pc, ex, make_ccptr<pcre_traits>(m.text->data()),
                            static_cast<int>(m.text->size()), static_cast<int>(start), match_flags(m.fset) | xflags,
                            m.ofs.data(), half, m.ofs.data() + half, half);
                        if (m.status != 0 && m.status != PCRE_ERROR_DFA_WSSIZE)
                            break;
                        m.ofs.resize(m.ofs.size() * 2);
                    }
                } else {
                    size_t minsize = 3 * count_groups(m.ref);
                    if (m.ofs.size() < minsize)
                        m.ofs.resize(minsize);
                    m.status = pcre_traits::exec(pc, ex, make_ccptr<pcre_traits>(m.text->data()),
                        static_cast<int>(m.text->size()), static_cast<int>(start), match_flags(m.fset) | xflags,
                        m.ofs.data(), static_cast<int>(m.ofs.size()));
                }
                if (m.status >= 0 && anchors == 2 && match_size(m, 0) < m.text->size() - start)
                    m.status = PCRE_ERROR_NOMATCH;
                if (m.status == PCRE_ERROR_NOMEMORY)
                    throw std::bad_alloc();
                if (m.status < 0 && m.status != PCRE_ERROR_NOMATCH && m.status != PCRE_ERROR_PARTIAL)
                    throw RegexError(pattern, m.status, {});
            }

        }

        // Type-specific implementation wrapper functions

        void inc_pcre(void* pc, char) noexcept { inc_pcre_impl<char>(pc); }
        void dec_pcre(void* pc, void* ex, char) noexcept { dec_pcre_impl<char>(pc, ex); }
        size_t count_groups(const PcreRef<char>& p) noexcept
            { return count_groups_impl<char>(p.pcre()); }
        size_t named_group(const PcreRef<char>& p, const string& name) noexcept
            { return named_group_impl(p.pcre(), name); }
        void init_regex(RegexInfo<char>& r, const string& pattern, Crow::Flagset flags, bool unicode)
            { init_regex_impl(r, pattern, flags, unicode); }
        void init_match(MatchInfo<char>& m, const RegexInfo<char>& r, const string& text)
            { init_match_impl(m, r, text); }
        void next_match(MatchInfo<char>& m, const string& pattern, size_t start, int anchors)
            { next_match_impl(m, pattern, start, anchors); }

        #if defined(UNICORN_PCRE16)
            void inc_pcre(void* pc, char16_t) noexcept { inc_pcre_impl<char16_t>(pc); }
            void dec_pcre(void* pc, void* ex, char16_t) noexcept { dec_pcre_impl<char16_t>(pc, ex); }
            size_t count_groups(const PcreRef<char16_t>& p) noexcept
                { return count_groups_impl<char16_t>(p.pcre()); }
            size_t named_group(const PcreRef<char16_t>& p, const u16string& name) noexcept
                { return named_group_impl(p.pcre(), name); }
            void init_regex(RegexInfo<char16_t>& r, const u16string& pattern, Crow::Flagset flags, bool unicode)
                { init_regex_impl(r, pattern, flags, unicode); }
            void init_match(MatchInfo<char16_t>& m, const RegexInfo<char16_t>& r, const u16string& text)
                { init_match_impl(m, r, text); }
            void next_match(MatchInfo<char16_t>& m, const string& pattern, size_t start, int anchors)
                { next_match_impl(m, pattern, start, anchors); }
        #endif

        #if defined(UNICORN_PCRE32)
            void inc_pcre(void* pc, char32_t) noexcept { inc_pcre_impl<char32_t>(pc); }
            void dec_pcre(void* pc, void* ex, char32_t) noexcept { dec_pcre_impl<char32_t>(pc, ex); }
            size_t count_groups(const PcreRef<char32_t>& p) noexcept
                { return count_groups_impl<char32_t>(p.pcre()); }
            size_t named_group(const PcreRef<char32_t>& p, const u32string& name) noexcept
                { return named_group_impl(p.pcre(), name); }
            void init_regex(RegexInfo<char32_t>& r, const u32string& pattern, Crow::Flagset flags, bool unicode)
                { init_regex_impl(r, pattern, flags, unicode); }
            void init_match(MatchInfo<char32_t>& m, const RegexInfo<char32_t>& r, const u32string& text)
                { init_match_impl(m, r, text); }
            void next_match(MatchInfo<char32_t>& m, const string& pattern, size_t start, int anchors)
                { next_match_impl(m, pattern, start, anchors); }
        #endif

        #if defined(UNICORN_PCRE_WCHAR)
            void inc_pcre(void* pc, wchar_t) noexcept { inc_pcre_impl<wchar_t>(pc); }
            void dec_pcre(void* pc, void* ex, wchar_t) noexcept { dec_pcre_impl<wchar_t>(pc, ex); }
            size_t count_groups(const PcreRef<wchar_t>& p) noexcept
                { return count_groups_impl<wchar_t>(p.pcre()); }
            size_t named_group(const PcreRef<wchar_t>& p, const wstring& name) noexcept
                { return named_group_impl(p.pcre(), name); }
            void init_regex(RegexInfo<wchar_t>& r, const wstring& pattern, Crow::Flagset flags, bool unicode)
                { init_regex_impl(r, pattern, flags, unicode); }
            void init_match(MatchInfo<wchar_t>& m, const RegexInfo<wchar_t>& r, const wstring& text)
                { init_match_impl(m, r, text); }
            void next_match(MatchInfo<wchar_t>& m, const string& pattern, size_t start, int anchors)
                { next_match_impl(m, pattern, start, anchors); }
        #endif

    }

    // Exceptions

    u8string RegexError::assemble(const u8string& pattern, int error, u8string message) {
        u8string s = "Regex error ";
        s += Crow::dec(error);
        if (message.empty())
            message = translate(error);
        if (! message.empty()) {
            s += " (";
            s += message;
            s += ")";
        }
        if (! pattern.empty()) {
            s += ", pattern: ";
            s += pattern;
        }
        return s;
    }

    u8string RegexError::translate(int error) {
        if (error >= 0)
            return {};
        int index = - error - 1;
        if (index < static_cast<int>(Crow::range_count(error_table)))
            return error_table[index];
        else
            return {};
    }

    // Version information

    Crow::Version regex_version() noexcept {
        return {PCRE_MAJOR, PCRE_MINOR, 0};
    }

}
