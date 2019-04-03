#include "unicorn/regex.hpp"
#include <algorithm>
#include <new>
#include <utility>

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

    Regex::Regex(std::string_view pattern, flag_type flags) {
        if (flags & ~ all_flags)
            throw error(PCRE2_ERROR_BADOPTION);
        re_pattern = pattern;
        re_flags = flags;
        uint32_t compile_options = translate_compile_flags(flags);
        if (pattern.find("(*") != npos)
            compile_options |= PCRE2_NO_DOTSTAR_ANCHOR | PCRE2_NO_START_OPTIMIZE;
        auto context_ptr = pcre2_compile_context_create(nullptr);
        if (! context_ptr)
            throw std::bad_alloc();
        pc_context.reset(context_ptr, pcre2_compile_context_free);
        pcre2_set_bsr(context_ptr, flags & byte ? PCRE2_BSR_ANYCRLF : PCRE2_BSR_UNICODE);
        pcre2_set_newline(context_ptr, flags & crlf ? PCRE2_NEWLINE_CRLF : PCRE2_NEWLINE_LF);
        #if RS_PCRE_VERSION >= 1030
            uint32_t extra = 0;
            if (flags & Regex::line)
                extra |= PCRE2_EXTRA_MATCH_LINE;
            if (flags & Regex::word)
                extra |= PCRE2_EXTRA_MATCH_WORD;
            pcre2_set_compile_extra_options(context_ptr, extra);
        #else
            std::string new_pattern;
            if (flags & Regex::line)
                pattern = new_pattern = "^(?:" + std::string(pattern) + ")$";
            if (flags & Regex::word)
                pattern = new_pattern = "\\b(?:" + std::string(pattern) + ")\\b";
        #endif
        int error_code = 0;
        size_t error_pos = 0;
        auto code_ptr = pcre2_compile(byte_ptr(pattern), pattern.size(), compile_options, &error_code, &error_pos, context_ptr);
        if (! code_ptr)
            handle_error(error_code);
        pc_code.reset(code_ptr, pcre2_code_free);
        if (flags & optimize) {
            uint32_t jit_options = PCRE2_JIT_COMPLETE;
            if (flags & partial_hard)
                jit_options |= PCRE2_PARTIAL_HARD;
            else if (flags & partial_soft)
                jit_options |= PCRE2_PARTIAL_SOFT;
            pcre2_jit_compile(code_ptr, jit_options);
        }
    }

    size_t Regex::groups() const noexcept {
        if (is_null())
            return 0;
        uint32_t captures = 0;
        auto code_ptr = static_cast<pcre2_code*>(pc_code.get());
        pcre2_pattern_info(code_ptr, PCRE2_INFO_CAPTURECOUNT, &captures);
        return captures + 1;
    }

    size_t Regex::named(std::string_view name) const {
        if (is_null())
            return npos;
        auto code_ptr = static_cast<pcre2_code*>(pc_code.get());
        std::string name_str(name);
        int rc = pcre2_substring_number_from_name(code_ptr, byte_ptr(name_str));
        return rc == PCRE2_ERROR_NOSUBSTRING || rc == PCRE2_ERROR_NOUNIQUESUBSTRING ? npos : size_t(rc);
    }

    Regex::match Regex::search(std::string_view str, size_t pos, flag_type flags) const {
        match m(*this, str, flags);
        m.next(pos);
        return m;
    }

    Regex::match Regex::search(const Utf8Iterator& start, flag_type flags) const {
        if (re_flags & byte)
            throw error(PCRE2_ERROR_BADOPTION);
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
        if (re_flags & byte)
            throw error(PCRE2_ERROR_BADOPTION);
        return {{*this, start.source(), start.offset(), flags}, {}};
    }

    Regex::partition_type Regex::partition(std::string_view str, size_t pos, flag_type flags) const {
        match m = search(str, pos, flags);
        if (m)
            return {{str.data(), m.offset()}, m, {str.data() + m.endpos(), str.size() - m.endpos()}};
        else
            return {str, {str.data() + str.size(), 0}, {str.data() + str.size(), 0}};
    }

    void Regex::do_replace(std::string_view src, std::string& dst, std::string_view fmt, size_t pos, flag_type flags) const {
        static constexpr uint32_t default_options =
            PCRE2_SUBSTITUTE_EXTENDED | PCRE2_SUBSTITUTE_OVERFLOW_LENGTH | PCRE2_SUBSTITUTE_UNKNOWN_UNSET | PCRE2_SUBSTITUTE_UNSET_EMPTY;
        if (is_null()) {
            dst = src;
            return;
        }
        if (flags & ~ runtime_mask)
            throw error(PCRE2_ERROR_BADOPTION);
        flags |= re_flags;
        uint32_t replace_options = default_options | translate_match_flags(flags);
        if (flags & global)
            replace_options |= PCRE2_SUBSTITUTE_GLOBAL;
        auto code_ptr = static_cast<pcre2_code*>(pc_code.get());
        dst.assign(src.size() + fmt.size() + 100, '\0');
        int rc = -1;
        while (rc < 0) {
            size_t dst_size = dst.size();
            rc = pcre2_substitute(code_ptr, byte_ptr(src), src.size(), pos, replace_options, nullptr, nullptr,
                byte_ptr(fmt), fmt.size(), byte_ptr(dst), &dst_size);
            if (rc < 0 && rc != PCRE2_ERROR_NOMATCH && rc != PCRE2_ERROR_NOMEMORY && rc != PCRE2_ERROR_PARTIAL)
                handle_error(rc);
            dst.resize(dst_size);
        }
    }

    std::string Regex::replace(std::string_view str, std::string_view fmt, size_t pos, flag_type flags) const {
        std::string dst;
        do_replace(str, dst, fmt, pos, flags);
        return dst;
    }

    void Regex::replace_in(std::string& str, std::string_view fmt, size_t pos, flag_type flags) const {
        std::string dst;
        do_replace(str, dst, fmt, pos, flags);
        str = std::move(dst);
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

    Regex::match::operator bool() const noexcept {
        return match_result >= 0 || match_result == PCRE2_ERROR_PARTIAL;
    }

    bool Regex::match::full() const noexcept {
        return match_result >= 0;
    }

    bool Regex::match::partial() const noexcept {
        return match_result == PCRE2_ERROR_PARTIAL;
    }

    bool Regex::match::matched(size_t i) const noexcept {
        if (match_result < 0 && match_result != PCRE2_ERROR_PARTIAL)
            return false;
        else if (i == 0)
            return true;
        else if (i < offset_count)
            return offset_vector[2 * i] != PCRE2_UNSET;
        else
            return false;
    }

    const char* Regex::match::begin(size_t i) const noexcept {
        size_t ofs = offset(i);
        return ofs == npos ? nullptr : subject_view.data() + ofs;
    }

    const char* Regex::match::end(size_t i) const noexcept {
        size_t ofs = endpos(i);
        return ofs == npos ? nullptr : subject_view.data() + ofs;
    }

    size_t Regex::match::offset(size_t i) const noexcept {
        if (i >= offset_count)
            return npos;
        size_t ofs = offset_vector[2 * i];
        return ofs == PCRE2_UNSET ? npos : ofs;
    }

    size_t Regex::match::endpos(size_t i) const noexcept {
        if (i >= offset_count)
            return npos;
        size_t ofs = offset_vector[2 * i + 1];
        return ofs == PCRE2_UNSET ? npos : ofs;
    }

    size_t Regex::match::count(size_t i) const noexcept {
        if (i >= offset_count)
            return 0;
        size_t start = offset_vector[2 * i], stop = offset_vector[2 * i + 1];
        return start == PCRE2_UNSET || stop < start ? 0 : stop - start;
    }

    std::string_view Regex::match::str(size_t i) const noexcept {
        if (i >= offset_count)
            return {};
        size_t start = offset_vector[2 * i], stop = offset_vector[2 * i + 1];
        if (start == PCRE2_UNSET)
            return {};
        else if (start <= stop)
            return std::string_view(subject_view.data() + start, stop - start);
        else
            return std::string_view(subject_view.data() + start, 0);
    }

    size_t Regex::match::first() const noexcept {
        for (size_t i = 1; i < offset_count; ++i) {
            size_t start = offset_vector[2 * i], stop = offset_vector[2 * i + 1];
            if (start != PCRE2_UNSET && start < stop)
                return i;
        }
        return npos;
    }

    size_t Regex::match::last() const noexcept {
        if (offset_count > 1) {
            for (size_t i = offset_count - 1; i > 0; --i) {
                size_t start = offset_vector[2 * i], stop = offset_vector[2 * i + 1];
                if (start != PCRE2_UNSET && start < stop)
                    return i;
            }
        }
        return npos;
    }

    std::string_view Regex::match::mark() const noexcept {
        if (! match_data)
            return {};
        auto match_ptr = static_cast<pcre2_match_data*>(match_data.get());
        auto ptr = pcre2_get_mark(match_ptr);
        if (! ptr)
            return {};
        auto mark_ptr = reinterpret_cast<const char*>(ptr);
        size_t count = ptr[-1];
        return {mark_ptr, count};
    }

    Regex::match::match(const Regex& re, std::string_view str, flag_type flags) {
        if (flags & ~ runtime_mask)
            throw error(PCRE2_ERROR_BADOPTION);
        if (re.is_null() || ! str.data())
            return;
        subject_view = str;
        regex_ptr = &re;
        match_flags = flags | re.flags();
        match_options = translate_match_flags(match_flags);
    }

    size_t Regex::match::index_by_name(std::string_view name) const {
        return regex_ptr ? regex_ptr->named(name) : npos;
    }

    void Regex::match::next(size_t pos) {
        if (! regex_ptr)
            return;
        if (pos > subject_view.size())
            return;
        auto code_ptr = static_cast<pcre2_code*>(regex_ptr->pc_code.get());
        if (match_data.use_count() != 1) {
            auto new_data = pcre2_match_data_create_from_pattern(code_ptr, nullptr);
            if (! new_data)
                throw std::bad_alloc();
            match_data.reset(new_data, pcre2_match_data_free);
        }
        match_result = PCRE2_ERROR_NOMATCH;
        offset_count = 0;
        offset_vector = nullptr;
        auto match_ptr = static_cast<pcre2_match_data*>(match_data.get());
        match_result = pcre2_match(code_ptr, byte_ptr(subject_view), subject_view.size(), pos, match_options, match_ptr, nullptr);
        if (match_result == PCRE2_ERROR_NOMATCH)
            return;
        else if (match_result < 0 && match_result != PCRE2_ERROR_PARTIAL)
            handle_error(match_result);
        offset_vector = pcre2_get_ovector_pointer(match_ptr);
        #if RS_PCRE_VERSION < 1030
            if ((match_flags & Regex::full) && offset_vector[1] < subject_view.size()) {
                match_result = PCRE2_ERROR_NOMATCH;
                return;
            }
        #endif
        offset_count = pcre2_get_ovector_count(match_ptr);
    }

    // Class Regex::split_iterator

    Regex::split_iterator& Regex::split_iterator::operator++() {
        if (after) {
            auto start = after.end();
            after.next();
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

    std::string Regex::transform::replace(std::string_view str, size_t pos, flag_type flags) const {
        return re.replace(str, sub_format, pos, flags | sub_flags);
    }

    void Regex::transform::replace_in(std::string& str, size_t pos, flag_type flags) const {
        re.replace_in(str, sub_format, pos, flags | sub_flags);
    }

}
