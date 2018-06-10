#include "unicorn/regex.hpp"
#include <algorithm>
#include <cstdlib>
#include <new>

#ifdef PCRE2_CODE_UNIT_WIDTH
    #undef PCRE2_CODE_UNIT_WIDTH
#endif
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#define RS_PCRE_VERSION (100 * PCRE2_MAJOR + PCRE2_MINOR)
#if RS_PCRE_VERSION < 1030
    #define PCRE2_ENDANCHORED 0
#endif

using namespace std::literals;

namespace RS::Unicorn {

    namespace {

        constexpr Regex::flag_type compile_mask  = (Regex::compile_sentinel_ << 1) - 1;
        constexpr Regex::flag_type all_flags     = (Regex::runtime_sentinel_ << 1) - 1;
        constexpr Regex::flag_type runtime_mask  = all_flags - compile_mask;

        auto byte_ptr(std::string_view v) noexcept { return reinterpret_cast<const unsigned char*>(v.data()); }
        auto byte_ptr(std::string& s) noexcept { return reinterpret_cast<unsigned char*>(&s[0]); }

        void handle_error(int rc) {
            if (rc == PCRE2_ERROR_NOMEMORY)
                throw std::bad_alloc();
            else
                throw Regex::error(rc);
        }

        Version get_version(uint32_t what) noexcept {
            char buf[24];
            int rc = pcre2_config(what, buf);
            if (rc < 0)
                return {};
            Ustring str(buf);
            size_t cut = str.find(' ');
            if (cut != npos)
                str.resize(cut);
            return Version(str);
        }

        constexpr uint32_t translate_compile_flags(Regex::flag_type flags) noexcept {
            uint32_t options = 0;
            if (! (flags & Regex::byte)) {
                options |= PCRE2_UTF;
                if (! (flags & Regex::ascii))
                    options |= PCRE2_UCP;
            }
            if (flags & Regex::anchor)        options |= PCRE2_ANCHORED;
            if (flags & Regex::dollar_end)    options |= PCRE2_DOLLAR_ENDONLY;
            if (flags & Regex::dot_all)       options |= PCRE2_DOTALL;
            if (flags & Regex::extended)      options |= PCRE2_EXTENDED;
            if (flags & Regex::first_line)    options |= PCRE2_FIRSTLINE;
            if (flags & Regex::full)          options |= PCRE2_ANCHORED | PCRE2_ENDANCHORED;
            if (flags & Regex::icase)         options |= PCRE2_CASELESS;
            if (flags & Regex::multiline)     options |= PCRE2_MULTILINE;
            if (flags & Regex::no_capture)    options |= PCRE2_NO_AUTO_CAPTURE;
            if (flags & Regex::no_utf_check)  options |= PCRE2_NO_UTF_CHECK;
            return options;
        }

        constexpr uint32_t translate_match_flags(Regex::flag_type flags) noexcept {
            uint32_t options = 0;
            if (flags & Regex::anchor)           options |= PCRE2_ANCHORED;
            if (flags & Regex::full)             options |= PCRE2_ANCHORED | PCRE2_ENDANCHORED;
            if (flags & Regex::no_utf_check)     options |= PCRE2_NO_UTF_CHECK;
            if (flags & Regex::not_empty)        options |= PCRE2_NOTEMPTY;
            if (flags & Regex::not_empty_start)  options |= PCRE2_NOTEMPTY_ATSTART;
            if (flags & Regex::not_line)         options |= PCRE2_NOTBOL | PCRE2_NOTEOL;
            if (flags & Regex::partial_hard)     options |= PCRE2_PARTIAL_HARD;
            if (flags & Regex::partial_soft)     options |= PCRE2_PARTIAL_SOFT;
            return options;
        }

    }

    // Class Regex

    struct Regex::impl_type {
        std::string pattern;
        flag_type flags = 0;
        pcre2_compile_context* context = nullptr;
        pcre2_code* code = nullptr;
        impl_type() = default;
        ~impl_type() noexcept {
            pcre2_code_free(code);
            pcre2_compile_context_free(context);
        }
        RS_NO_COPY_MOVE(impl_type);
    };

    Regex::Regex(std::string_view pattern, flag_type flags) {
        if (flags & ~ all_flags)
            throw error(PCRE2_ERROR_BADOPTION);
        impl = std::make_shared<impl_type>();
        impl->pattern = pattern;
        impl->flags = flags;
        uint32_t compile_options = translate_compile_flags(flags);
        if (pattern.find("(*COMMIT)") != npos || pattern.find("(*MARK)") != npos)
            compile_options |= PCRE2_NO_DOTSTAR_ANCHOR | PCRE2_NO_START_OPTIMIZE;
        impl->context = pcre2_compile_context_create(nullptr);
        if (! impl->context)
            throw std::bad_alloc();
        pcre2_set_bsr(impl->context, flags & byte ? PCRE2_BSR_ANYCRLF : PCRE2_BSR_UNICODE);
        pcre2_set_newline(impl->context, flags & crlf ? PCRE2_NEWLINE_CRLF : PCRE2_NEWLINE_LF);
        #if RS_PCRE_VERSION >= 1030
            uint32_t extra = 0;
            if (flags & Regex::line)
                extra |= PCRE2_EXTRA_MATCH_LINE;
            if (flags & Regex::word)
                extra |= PCRE2_EXTRA_MATCH_WORD;
            pcre2_set_compile_extra_options(impl->context, extra);
        #else
            std::string new_pattern;
            if (flags & Regex::line)
                pattern = new_pattern = "^(?:" + std::string(pattern) + ")$";
            if (flags & Regex::word)
                pattern = new_pattern = "\\b(?:" + std::string(pattern) + ")\\b";
        #endif
        int error_code = 0;
        size_t error_pos = 0;
        impl->code = pcre2_compile(byte_ptr(pattern), pattern.size(), compile_options, &error_code, &error_pos, impl->context);
        if (! impl->code)
            handle_error(error_code);
        if (flags & optimize) {
            uint32_t jit_options = PCRE2_JIT_COMPLETE;
            if (flags & partial_hard)
                jit_options |= PCRE2_PARTIAL_HARD;
            else if (flags & partial_soft)
                jit_options |= PCRE2_PARTIAL_SOFT;
            pcre2_jit_compile(impl->code, jit_options);
        }
    }

    std::string Regex::pattern() const {
        return impl ? impl->pattern : std::string();
    }

    Regex::flag_type Regex::flags() const noexcept {
        return impl ? impl->flags : 0;
    }

    bool Regex::empty() const noexcept {
        return ! impl || impl->pattern.empty();
    }

    size_t Regex::groups() const noexcept {
        if (! impl)
            return 0;
        uint32_t captures = 0;
        pcre2_pattern_info(impl->code, PCRE2_INFO_CAPTURECOUNT, &captures);
        return captures + 1;
    }

    size_t Regex::named(std::string_view name) const {
        if (! impl)
            return npos;
        std::string name_str(name);
        int rc = pcre2_substring_number_from_name(impl->code, byte_ptr(name_str));
        return rc == PCRE2_ERROR_NOSUBSTRING || rc == PCRE2_ERROR_NOUNIQUESUBSTRING ? npos : size_t(rc);
    }

    Regex::match Regex::search(std::string_view str, size_t pos, flag_type flags) const {
        match m(*this, str, flags);
        m.next(pos);
        return m;
    }

    Regex::match Regex::search(const Utf8Iterator& start, flag_type flags) const {
        utf_only();
        return search(start.source(), start.offset(), flags);
    }

    Regex::match Regex::operator()(std::string_view str, size_t pos, flag_type flags) const {
        return search(str, pos, flags);
    }

    Regex::match Regex::operator()(const Utf8Iterator& start, flag_type flags) const {
        return search(start, flags);
    }

    size_t Regex::count(std::string_view str, size_t pos, flag_type flags) const {
        return grep(str, pos, flags).size();
    }

    size_t Regex::count(const Utf8Iterator& start, flag_type flags) const {
        return grep(start, flags).size();
    }

    Regex::match_range Regex::grep(std::string_view str, size_t pos, flag_type flags) const {
        return {{*this, str, pos, flags}, {}};
    }

    Regex::match_range Regex::grep(const Utf8Iterator& start, flag_type flags) const {
        utf_only();
        return {{*this, start.source(), start.offset(), flags}, {}};
    }

    std::tuple<std::string_view, std::string_view, std::string_view> Regex::partition(std::string_view str, size_t pos, flag_type flags) const {
        match m = search(str, pos, flags);
        if (m) {
            size_t start = m.offset(), stop = m.endpos();
            return {{str.data(), start}, m, {str.data() + stop, str.size() - stop}};
        } else {
            std::string_view at_end(str.data() + str.size(), 0);
            return {str, at_end, at_end};
        }
    }

    std::string Regex::replace(std::string_view str, std::string_view fmt, size_t pos, flag_type flags) const {
        static constexpr uint32_t default_options = PCRE2_SUBSTITUTE_EXTENDED | PCRE2_SUBSTITUTE_OVERFLOW_LENGTH | PCRE2_SUBSTITUTE_UNKNOWN_UNSET | PCRE2_SUBSTITUTE_UNSET_EMPTY;
        if (! impl)
            return std::string(str);
        if (flags & ~ runtime_mask)
            throw error(PCRE2_ERROR_BADOPTION);
        flags |= impl->flags;
        uint32_t sub_options = default_options | translate_match_flags(flags);
        if (flags & global)
            sub_options |= PCRE2_SUBSTITUTE_GLOBAL;
        std::string result(str.size() + fmt.size() + 100, '\0');
        for (;;) {
            size_t result_size = result.size();
            int rc = pcre2_substitute(impl->code, byte_ptr(str), str.size(), pos, sub_options, nullptr, nullptr, byte_ptr(fmt), fmt.size(), byte_ptr(result), &result_size);
            if (rc < 0 && rc != PCRE2_ERROR_NOMATCH && rc != PCRE2_ERROR_NOMEMORY && rc != PCRE2_ERROR_PARTIAL)
                handle_error(rc);
            result.resize(result_size);
            if (rc >= 0)
                return result;
        }
    }

    Regex::split_range Regex::split(std::string_view str, size_t pos, flag_type flags) const {
        return {{*this, str, pos, flags}, {}};
    }

    Version Regex::compile_version() noexcept {
        static const Version v(PCRE2_MAJOR, PCRE2_MINOR);
        return v;
    }

    Version Regex::runtime_version() noexcept {
        static const Version v = get_version(PCRE2_CONFIG_VERSION);
        return v;
    }

    Version Regex::unicode_version() noexcept {
        static const Version v = get_version(PCRE2_CONFIG_UNICODE_VERSION);
        return v;
    }

    std::string Regex::escape(std::string_view str) {
        static const auto digit = [] (auto x) { return char(x < 10 ? '0' + x : 'a' + x - 10); };
        static const std::string table = [] {
            static const std::string meta = "$()*+.?[\\]^{|}";
            std::string tab(256, 'c');
            auto b = tab.begin();
            std::fill(b, b + 9, 'x');
            std::fill(b + 14, b + 32, 'x');
            for (char m: meta)
                tab[uint8_t(m)] = 'm';
            tab[127] = 'x';
            return tab;
        }();
        std::string esc;
        for (char c: str) {
            auto b = uint8_t(c);
            switch (table[b]) {
                case 'm':  esc += {'\\', c}; break;
                case 'x':  esc += {'\\', 'x', digit(b / 16), digit(b % 16)}; break;
                default:   esc += c; break;
            }
        }
        return esc;
    }

    void Regex::utf_only() const {
        if (flags() & byte)
            throw error(PCRE2_ERROR_BADOPTION);
    }

    // Class Regex::error

    std::string Regex::error::translate(int code) {
        std::string buf(120, '\0');
        int rc = pcre2_get_error_message(code, byte_ptr(buf), buf.size());
        if (rc > 0)
            return "PCRE2 error: "s + buf.data();
        else
            return "PCRE2 error "s + std::to_string(code);
    }

    // Class Regex::match

    struct Regex::match::impl_type {
        pcre2_match_data* data = nullptr;
        flag_type flags = 0;
        uint32_t options = 0;
        const Regex* regex = nullptr;
        std::string_view subject;
        size_t ocount = 0;
        size_t* ovector = nullptr;
        bool partial = false;
        impl_type() = default;
        ~impl_type() noexcept { pcre2_match_data_free(data); }
        RS_NO_COPY_MOVE(impl_type);
    };

    Regex::match::operator bool() const noexcept {
        return bool(impl);
    }

    bool Regex::match::full() const noexcept {
        return impl && ! impl->partial;
    }

    bool Regex::match::partial() const noexcept {
        return impl && impl->partial;
    }

    bool Regex::match::matched(size_t i) const noexcept {
        return i == 0 ? bool(impl) : offset(i) != npos;
    }

    const char* Regex::match::begin(size_t i) const noexcept {
        size_t ofs = offset(i);
        return ofs == npos ? nullptr : impl->subject.data() + ofs;
    }

    const char* Regex::match::end(size_t i) const noexcept {
        size_t ofs = endpos(i);
        return ofs == npos ? nullptr : impl->subject.data() + ofs;
    }

    size_t Regex::match::offset(size_t i) const noexcept {
        if (! index_check(i))
            return npos;
        size_t ofs = impl->ovector[2 * i];
        return ofs == PCRE2_UNSET ? npos : ofs;
    }

    size_t Regex::match::endpos(size_t i) const noexcept {
        if (! index_check(i))
            return npos;
        size_t ofs = impl->ovector[2 * i + 1];
        return ofs == PCRE2_UNSET ? npos : ofs;
    }

    size_t Regex::match::count(size_t i) const noexcept {
        if (! index_check(i))
            return 0;
        size_t start = impl->ovector[2 * i], stop = impl->ovector[2 * i + 1];
        return start == PCRE2_UNSET || stop < start ? 0 : stop - start;
    }

    std::string_view Regex::match::str(size_t i) const noexcept {
        if (! index_check(i))
            return {};
        size_t start = impl->ovector[2 * i], stop = impl->ovector[2 * i + 1];
        if (start == PCRE2_UNSET)
            return {};
        else if (start <= stop)
            return std::string_view(impl->subject.data() + start, stop - start);
        else
            return std::string_view(impl->subject.data() + start, 0);
    }

    Regex::match::match(const Regex& re, std::string_view str, flag_type flags) {
        if (flags & ~ runtime_mask)
            throw error(PCRE2_ERROR_BADOPTION);
        if (! re.impl || ! str.data())
            return;
        impl = std::make_shared<impl_type>();
        impl->flags = flags | re.flags();
        impl->options = translate_match_flags(impl->flags);
        impl->regex = &re;
        impl->subject = str;
        impl->data = pcre2_match_data_create_from_pattern(impl->regex->impl->code, nullptr);
        if (! impl->data)
            throw std::bad_alloc();
    }

    size_t Regex::match::index_by_name(std::string_view name) const {
        return impl ? impl->regex->named(name) : npos;
    }

    bool Regex::match::index_check(size_t i) const noexcept {
        return impl && i < impl->ocount;
    }

    void Regex::match::next(size_t pos) {
        if (! impl)
            return;
        auto guard = scope_exit([&] { impl.reset(); });
        if (pos > impl->subject.size())
            return;
        int rc = pcre2_match(impl->regex->impl->code, byte_ptr(impl->subject), impl->subject.size(), pos, impl->options, impl->data, nullptr);
        if (rc == PCRE2_ERROR_NOMATCH)
            return;
        else if (rc < 0 && rc != PCRE2_ERROR_PARTIAL)
            handle_error(rc);
        impl->ovector = pcre2_get_ovector_pointer(impl->data);
        #if RS_PCRE_VERSION < 1030
            if ((impl->flags & Regex::full) && impl->ovector[1] < impl->subject.size())
                return;
        #endif
        impl->ocount = pcre2_get_ovector_count(impl->data);
        impl->partial = rc == PCRE2_ERROR_PARTIAL;
        guard.release();
    }

    // Class Regex::split_iterator

    Regex::split_iterator& Regex::split_iterator::operator++() {
        if (after) {
            auto start = after.end();
            after.next(after.endpos());
            if (after)
                span = {start, size_t(after.begin() - start)};
            else
                span = {start, size_t(end - start)};
        } else {
            span = {};
        }
        return *this;
    }

    Regex::split_iterator::split_iterator(const Regex& re, std::string_view str, size_t pos, flag_type flags):
    after(re, str, flags), end(str.data() + str.size()), span(str) {
        after.next(pos);
        if (after)
            span = std::string_view(str.data() + pos, after.offset() - pos);
    }

    // Class Regex::transform

    Regex::transform::transform(const Regex& pattern, std::string_view fmt, flag_type flags):
    re(pattern), sub_format(fmt), sub_flags(flags) {}

    Regex::transform::transform(std::string_view pattern, std::string_view fmt, flag_type flags):
    re(pattern, flags), sub_format(fmt), sub_flags(flags & runtime_mask) {}

    std::string Regex::transform::replace(std::string_view str, size_t pos) const {
        return re.replace(str, sub_format, pos, sub_flags);
    }

}
