#pragma once

#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "unicorn/utility.hpp"
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

RS_LDLIB(pcre2-8);

namespace RS::Unicorn {

    class Regex {

    public:

        class error;
        class match;
        class match_iterator;
        class split_iterator;
        class transform;
        using flag_type = uint32_t;
        using match_range = Irange<match_iterator>;
        using split_range = Irange<split_iterator>;

        struct partition_type {
            std::string_view left;
            std::string_view mid;
            std::string_view right;
        };

        static constexpr auto ascii              = flag_type(1) << 0;   // [C--] ASCII character properties           ~PCRE2_UCP
        static constexpr auto byte               = flag_type(1) << 1;   // [C--] Match byte strings instead of UTF-8  ~(PCRE2_UCP|PCRE2_UTF)
        static constexpr auto crlf               = flag_type(1) << 2;   // [C--] Lines end with CRLF                  PCRE2_NEWLINE_CRLF
        static constexpr auto dollar_end         = flag_type(1) << 3;   // [C--] $ does not match line breaks at end  PCRE2_DOLLAR_ENDONLY
        static constexpr auto dot_all            = flag_type(1) << 4;   // [C--] . matches line breaks                PCRE2_DOTALL
        static constexpr auto extended           = flag_type(1) << 5;   // [C--] Ignore whitespace and # comments     PCRE2_EXTENDED
        static constexpr auto first_line         = flag_type(1) << 6;   // [C--] Match must start in first line       PCRE2_FIRSTLINE
        static constexpr auto icase              = flag_type(1) << 7;   // [C--] Case insensitive match               PCRE2_CASELESS
        static constexpr auto line               = flag_type(1) << 8;   // [C--] Match whole line                     PCRE2_EXTRA_MATCH_LINE
        static constexpr auto multiline          = flag_type(1) << 9;   // [C--] ^/$ match beginning/end of line      PCRE2_MULTILINE
        static constexpr auto no_capture         = flag_type(1) << 10;  // [C--] No automatic capture                 PCRE2_NO_AUTO_CAPTURE
        static constexpr auto optimize           = flag_type(1) << 11;  // [C--] Optimize using JIT compiler          --
        static constexpr auto word               = flag_type(1) << 12;  // [C--] Match whole word                     PCRE2_EXTRA_MATCH_WORD
        static constexpr auto compile_sentinel_  = word;                //                                            --
        static constexpr auto anchor             = flag_type(1) << 13;  // [CMR] Anchor match at start                PCRE2_ANCHORED
        static constexpr auto full               = flag_type(1) << 14;  // [CMR] Match complete string                PCRE2_ANCHORED|PCRE2_ENDANCHORED
        static constexpr auto global             = flag_type(1) << 15;  // [--R] Replace all matches                  --
        static constexpr auto no_utf_check       = flag_type(1) << 16;  // [CMR] Assume valid UTF-8                   PCRE2_NO_UTF_CHECK
        static constexpr auto not_empty          = flag_type(1) << 17;  // [-MR] Do not match empty string            PCRE2_NOTEMPTY
        static constexpr auto not_empty_start    = flag_type(1) << 18;  // [-MR] Do not match empty string at start   PCRE2_NOTEMPTY_ATSTART
        static constexpr auto not_line           = flag_type(1) << 19;  // [-MR] Do not match ^/$ at start/end        PCRE2_NOTBOL|PCRE2_NOTEOL
        static constexpr auto partial_hard       = flag_type(1) << 20;  // [CMR] Hard partial match (partial>full)    PCRE2_PARTIAL_HARD
        static constexpr auto partial_soft       = flag_type(1) << 21;  // [CMR] Soft partial match (full>partial)    PCRE2_PARTIAL_SOFT
        static constexpr auto runtime_sentinel_  = partial_soft;        //                                            --

        Regex() = default;
        explicit Regex(std::string_view pattern, flag_type flags = 0);
        std::string pattern() const { return re_pattern; }
        flag_type flags() const noexcept { return re_flags; }
        bool empty() const noexcept { return re_pattern.empty(); }
        bool is_null() const noexcept { return ! pc_code; }
        size_t groups() const noexcept;
        size_t named(std::string_view name) const;
        match search(std::string_view str, size_t pos = 0, flag_type flags = 0) const;
        match search(const Utf8Iterator& start, flag_type flags = 0) const;
        match operator()(std::string_view str, size_t pos = 0, flag_type flags = 0) const;
        match operator()(const Utf8Iterator& start, flag_type flags = 0) const;
        size_t count(std::string_view str, size_t pos = 0, flag_type flags = 0) const;
        size_t count(const Utf8Iterator& start, flag_type flags = 0) const;
        match_range grep(std::string_view str, size_t pos = 0, flag_type flags = 0) const;
        match_range grep(const Utf8Iterator& start, flag_type flags = 0) const;
        partition_type partition(std::string_view str, size_t pos = 0, flag_type flags = 0) const;
        std::string replace(std::string_view str, std::string_view fmt, size_t pos = 0, flag_type flags = 0) const;
        void replace_in(std::string& str, std::string_view fmt, size_t pos = 0, flag_type flags = 0) const;
        split_range split(std::string_view str, size_t pos = 0, flag_type flags = 0) const;
        static Version compile_version() noexcept;
        static Version runtime_version() noexcept;
        static Version unicode_version() noexcept;
        static std::string escape(std::string_view str);
        friend std::ostream& operator<<(std::ostream& out, const Regex& re) { return out << re.pattern(); }
        friend std::string to_str(const Regex& re) { return re.pattern(); }

    private:

        std::shared_ptr<void> pc_context; // pcre2_compile_context
        std::shared_ptr<void> pc_code; // pcre2_code
        std::string re_pattern;
        flag_type re_flags = 0;

        void do_replace(std::string_view src, std::string& dst, std::string_view fmt, size_t pos, flag_type flags) const;

    };

    class Regex::error:
    public std::runtime_error {
    public:
        explicit error(int code): std::runtime_error(translate(code)), err(code) {}
        int code() const noexcept { return err; }
        static std::string translate(int code);
    private:
        int err;
    };

    class Regex::match {
    public:
        match() = default;
        explicit operator bool() const noexcept;
        bool full() const noexcept;
        bool partial() const noexcept;
        bool matched(size_t i = 0) const noexcept;
        const char* begin(size_t i = 0) const noexcept;
        const char* end(size_t i = 0) const noexcept;
        size_t offset(size_t i = 0) const noexcept;
        size_t endpos(size_t i = 0) const noexcept;
        size_t count(size_t i = 0) const noexcept;
        std::string_view str(size_t i = 0) const noexcept;
        std::string_view operator[](size_t i) const noexcept { return str(i); }
        size_t first() const noexcept;
        size_t last() const noexcept;
        std::string_view mark() const noexcept;
        operator std::string_view() const noexcept { return str(); }
        bool matched(std::string_view name) const { return matched(index_by_name(name)); }
        const char* begin(std::string_view name) const { return begin(index_by_name(name)); }
        const char* end(std::string_view name) const { return end(index_by_name(name)); }
        size_t offset(std::string_view name) const { return offset(index_by_name(name)); }
        size_t endpos(std::string_view name) const { return endpos(index_by_name(name)); }
        size_t count(std::string_view name) const { return count(index_by_name(name)); }
        std::string_view str(std::string_view name) const { return str(index_by_name(name)); }
        std::string_view operator[](std::string_view name) const { return str(name); }
    private:
        friend class Regex;
        friend class Regex::match_iterator;
        friend class Regex::split_iterator;
        std::shared_ptr<void> match_data; // pcre2_match_data
        std::string_view subject_view;
        const Regex* regex_ptr = nullptr;
        flag_type match_flags = 0;
        uint32_t match_options = 0;
        int match_result = -1; // PCRE2_ERROR_NOMATCH
        size_t offset_count = 0;
        size_t* offset_vector = nullptr;
        match(const Regex& re, std::string_view str, flag_type flags);
        size_t index_by_name(std::string_view name) const;
        void next() { next(endpos()); }
        void next(size_t pos);
    };

    class Regex::match_iterator:
    public ForwardIterator<match_iterator, const match> {
    public:
        match_iterator() = default;
        const match& operator*() const noexcept { return current; }
        match_iterator& operator++() { current.next(); return *this; }
        bool operator==(const match_iterator& rhs) const noexcept { return current.offset() == rhs.current.offset(); }
    private:
        friend class Regex;
        match current;
        match_iterator(const Regex& re, std::string_view str, size_t pos, flag_type flags): current(re, str, flags) { current.next(pos); }
    };

    class Regex::split_iterator:
    public ForwardIterator<split_iterator, const std::string_view> {
    public:
        split_iterator() = default;
        const std::string_view& operator*() const noexcept { return span; }
        split_iterator& operator++();
        bool operator==(const split_iterator& rhs) const noexcept { return span.data() == rhs.span.data(); }
    private:
        friend class Regex;
        Regex::match after;
        const char* end = nullptr;
        std::string_view span;
        split_iterator(const Regex& re, std::string_view str, size_t pos, flag_type flags);
    };

    class Regex::transform {
    public:
        transform() = default;
        explicit transform(const Regex& pattern, std::string_view fmt, flag_type flags = 0);
        explicit transform(std::string_view pattern, std::string_view fmt, flag_type flags = 0);
        std::string pattern() const { return re.pattern(); }
        std::string format() const { return sub_format; }
        flag_type flags() const noexcept { return re.flags() | sub_flags; }
        std::string replace(std::string_view str, size_t pos = 0) const;
        void replace_in(std::string& str, size_t pos = 0) const;
        std::string operator()(std::string_view str, size_t pos = 0) const { return replace(str, pos); }
    private:
        Regex re;
        std::string sub_format;
        flag_type sub_flags = 0;
    };

}
