#pragma once

#include "unicorn/core.hpp"
#include "unicorn/character.hpp"
#include <algorithm>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>

namespace Unicorn {

    // Constants

    // Remember that any other set of flags that might be combined with these
    // needs to skip the bits that are already spoken for.

    UNICORN_DEFINE_FLAG(encoding, err_ignore, 0);   // Assume valid UTF input
    UNICORN_DEFINE_FLAG(encoding, err_replace, 1);  // Replace invalid UTF with U+FFFD
    UNICORN_DEFINE_FLAG(encoding, err_throw, 2);    // Throw EncodingError on invalid UTF

    constexpr auto err_flags = err_ignore | err_replace | err_throw;

    namespace UnicornDetail {

        // UtfEncoding::decode() reads one encoded character from src, writes
        // it to dst, and returns the number of code units consumed. n is the
        // maximum number of code units available. On failure, it sets dst to
        // an invalid character, and returns the minimum number of code units
        // to skip (at least 1). It can assume src!=nullptr and n>0.

        // UtfEncoding::decode_fast() does the same as decode(), but without
        // checking for valid Unicode.

        // UtfEncoding::decode_prev() reads the encoded character immediately
        // preceding src+pos, writes it to dst, and returns the number of code
        // units stepped back. On failure, it sets dst to an invalid character,
        // and returns the start of the invalid code sequence. It can assume
        // src!=nullptr and pos>0.

        // UtfEncoding::encode() encodes the character in src and writes it to
        // dst. It can assume char_is_unicode(src), dst!=nullptr, and enough
        // room for max_units.

        template <typename C> struct UtfEncoding;

        template <>
        struct UtfEncoding<char> {
            static constexpr size_t max_units = 4;
            static size_t decode(const char* src, size_t n, char32_t& dst) noexcept;
            static size_t decode_fast(const char* src, size_t n, char32_t& dst) noexcept;
            static size_t decode_prev(const char* src, size_t pos, char32_t& dst) noexcept;
            static size_t encode(char32_t src, char* dst) noexcept;
            static const char* name() noexcept { return "UTF-8"; }
        };

        inline size_t UtfEncoding<char>::decode_fast(const char* src, size_t n, char32_t& dst) noexcept {
            auto code = reinterpret_cast<const uint8_t*>(src);
            if (code[0] <= 0xc1 || n < 2) {
                dst = code[0];
                return 1;
            } else if (code[0] <= 0xdf || n < 3) {
                dst = (char32_t(code[0] & 0x1f) << 6)
                    | char32_t(code[1] & 0x3f);
                return 2;
            } else if (code[0] <= 0xef || n < 4) {
                dst = (char32_t(code[0] & 0x0f) << 12)
                    | (char32_t(code[1] & 0x3f) << 6)
                    | char32_t(code[2] & 0x3f);
                return 3;
            } else {
                dst = (char32_t(code[0] & 0x07) << 18)
                    | (char32_t(code[1] & 0x3f) << 12)
                    | (char32_t(code[2] & 0x3f) << 6)
                    | char32_t(code[3] & 0x3f);
                return 4;
            }
        }


        template <>
        struct UtfEncoding<char16_t> {
            static constexpr size_t max_units = 2;
            static size_t decode(const char16_t* src, size_t n, char32_t& dst) noexcept;
            static size_t decode_fast(const char16_t* src, size_t n, char32_t& dst) noexcept;
            static size_t decode_prev(const char16_t* src, size_t pos, char32_t& dst) noexcept;
            static size_t encode(char32_t src, char16_t* dst) noexcept;
            static const char* name() noexcept { return "UTF-16"; }
        };

        inline size_t UtfEncoding<char16_t>::decode_fast(const char16_t* src, size_t n, char32_t& dst) noexcept {
            if (n >= 2 && char_is_surrogate(src[0])) {
                dst = 0x10000 + ((char32_t(src[0]) & 0x3ff) << 10)
                    + (char32_t(src[1]) & 0x3ff);
                return 2;
            } else {
                dst = src[0];
                return 1;
            }
        }

        template <>
        struct UtfEncoding<char32_t> {
            static constexpr size_t max_units = 1;
            static size_t decode(const char32_t* src, size_t /*n*/, char32_t& dst) noexcept
                { dst = *src; return 1; }
            static size_t decode_fast(const char32_t* src, size_t /*n*/, char32_t& dst) noexcept
                { dst = *src; return 1; }
            static size_t decode_prev(const char32_t* src, size_t pos, char32_t& dst) noexcept
                { dst = src[pos - 1]; return 1; }
            static size_t encode(char32_t src, char32_t* dst) noexcept
                { *dst = src; return 1; }
            static const char* name() noexcept { return "UTF-32"; }
        };

        template <>
        struct UtfEncoding<wchar_t> {
            using utf = UtfEncoding<WcharEquivalent>;
            static constexpr size_t max_units = utf::max_units;
            static size_t decode(const wchar_t* src, size_t n, char32_t& dst) noexcept
                { return utf::decode(reinterpret_cast<const WcharEquivalent*>(src), n, dst); }
            static size_t decode_fast(const wchar_t* src, size_t n, char32_t& dst) noexcept
                { return utf::decode_fast(reinterpret_cast<const WcharEquivalent*>(src), n, dst); }
            static size_t decode_prev(const wchar_t* src, size_t pos, char32_t& dst) noexcept
                { return utf::decode_prev(reinterpret_cast<const WcharEquivalent*>(src), pos, dst); }
            static size_t encode(char32_t src, wchar_t* dst) noexcept
                { return utf::encode(src, reinterpret_cast<WcharEquivalent*>(dst)); }
            static const char* name() noexcept { return "wchar_t"; }
        };

        template <typename C> inline void append_error(basic_string<C>& str)
            { str += static_cast<C>(replacement_char); }
        inline void append_error(u8string& str) { str += utf8_replacement; }

    }

    // Utility functions

    template <typename C>
    unsigned code_units(char32_t c) {
        switch (sizeof(C)) {
            case 1: return c <= 0x7f ? 1 : c <= 0x7ff ? 2 : c <= 0xffff ? 3 : 4;
            case 2: return c <= 0xffff ? 1 : 2;
            default: return 1;
        }
    }

    template <typename C>
    bool is_single_unit(C c) {
        return sizeof(C) == 1 ? uint8_t(c) <= 0x7f : char_is_unicode(c);
    }

    template <typename C>
    bool is_start_unit(C c) {
        return (sizeof(C) == 1 && uint8_t(c) >= 0xc2 && uint8_t(c) <= 0xf4)
            || (sizeof(C) == 2 && char_is_high_surrogate(c));
    }

    template <typename C>
    bool is_following_unit(C c) {
        return (sizeof(C) == 1 && uint8_t(c) >= 0x80 && uint8_t(c) <= 0xbf)
            || (sizeof(C) == 2 && char_is_low_surrogate(c));
    }

    template <typename C>
    bool is_invalid_unit(C c) {
        return (sizeof(C) == 1 && ((uint8_t(c) >= 0xc0 && uint8_t(c) <= 0xc1) || uint8_t(c) >= 0xf5))
            || (sizeof(C) == 4 && ! char_is_unicode(c));
    }

    template <typename C>
    bool is_initial_unit(C c) {
        return is_single_unit(c) || is_start_unit(c);
    }

    // UTF decoding iterator

    template <typename C>
    class UtfIterator:
    public BidirectionalIterator<UtfIterator<C>, const char32_t> {
    public:
        using code_unit = C;
        using string_type = basic_string<C>;
        UtfIterator() noexcept { static const string_type dummy; sptr = &dummy; }
        explicit UtfIterator(const string_type& src): sptr(&src)
            { UnicornDetail::default_flags(fset, err_flags, err_ignore); ++*this; }
        UtfIterator(const string_type& src, size_t offset, uint32_t flags = 0):
            sptr(&src), ofs(std::min(offset, src.size())), fset(flags)
            { UnicornDetail::default_flags(fset, err_flags, err_ignore); ++*this; }
        const char32_t& operator*() const noexcept { return u; }
        UtfIterator& operator++();
        UtfIterator& operator--();
        const string_type& source() const noexcept { return *sptr; }
        size_t offset() const noexcept { return ofs; }
        size_t count() const noexcept { return units; }
        Irange<const C*> range() const noexcept;
        string_type str() const { return sptr ? sptr->substr(ofs, units) : string_type(); }
        bool valid() const noexcept { return ok; }
        friend bool operator==(const UtfIterator& lhs, const UtfIterator& rhs) noexcept
            { return lhs.ofs == rhs.ofs; }
    private:
        const string_type* sptr = nullptr;  // Source string
        size_t ofs = 0;                     // Offset of current character in source
        size_t units = 0;                   // Code units in current character
        char32_t u = 0;                     // Current decoded character
        uint32_t fset = err_ignore;         // Error handling flag
        bool ok = false;                    // Current character is valid
    };

    template <typename C>
    UtfIterator<C>& UtfIterator<C>::operator++() {
        using namespace UnicornDetail;
        ofs = std::min(ofs + units, sptr->size());
        units = 0;
        u = 0;
        ok = false;
        if (ofs == sptr->size())
            return *this;
        if (fset & err_ignore) {
            units = UtfEncoding<C>::decode_fast(sptr->data() + ofs, sptr->size() - ofs, u);
            ok = true;
        } else {
            units = UtfEncoding<C>::decode(sptr->data() + ofs, sptr->size() - ofs, u);
            ok = char_is_unicode(u);
            if (! ok) {
                u = replacement_char;
                if (fset & err_throw)
                    throw EncodingError(UtfEncoding<C>::name(), ofs, sptr->data() + ofs, units);
            }
        }
        return *this;
    }

    template <typename C>
    UtfIterator<C>& UtfIterator<C>::operator--() {
        using namespace UnicornDetail;
        units = 0;
        u = 0;
        ok = false;
        if (ofs == 0)
            return *this;
        units = UtfEncoding<C>::decode_prev(sptr->data(), ofs, u);
        ofs -= units;
        ok = (fset & err_ignore) || char_is_unicode(u);
        if (! ok) {
            u = replacement_char;
            if (fset & err_throw)
                throw EncodingError(UtfEncoding<C>::name(), ofs, sptr->data() + ofs, units);
        }
        return *this;
    }

    template <typename C>
    Irange<const C*> UtfIterator<C>::range() const noexcept {
        if (sptr)
            return {sptr->data() + ofs, sptr->data() + ofs + units};
        else
            return {nullptr, nullptr};
    }

    using Utf8Iterator = UtfIterator<char>;
    using Utf8Range = Irange<Utf8Iterator>;
    using Utf16Iterator = UtfIterator<char16_t>;
    using Utf16Range = Irange<Utf16Iterator>;
    using Utf32Iterator = UtfIterator<char32_t>;
    using Utf32Range = Irange<Utf32Iterator>;
    using WcharIterator = UtfIterator<wchar_t>;
    using WcharRange = Irange<WcharIterator>;

    template <typename C>
    UtfIterator<C> utf_begin(const basic_string<C>& src, uint32_t flags = 0) {
        return {src, 0, flags};
    }

    template <typename C>
    UtfIterator<C> utf_end(const basic_string<C>& src, uint32_t flags = 0) {
        return {src, src.size(), flags};
    }

    template <typename C>
    UtfIterator<C> utf_iterator(const basic_string<C>& src, size_t offset, uint32_t flags = 0) {
        return {src, offset, flags};
    }

    template <typename C>
    Irange<UtfIterator<C>> utf_range(const basic_string<C>& src, uint32_t flags = 0) {
        return {utf_begin(src, flags), utf_end(src, flags)};
    }

    template <typename C>
    basic_string<C> u_str(const UtfIterator<C>& i, const UtfIterator<C>& j) {
        return i.source().substr(i.offset(), j.offset() - i.offset());
    }

    template <typename C>
    basic_string<C> u_str(const Irange<UtfIterator<C>>& range) {
        return u_str(range.begin(), range.end());
    }

    // UTF encoding iterator

    template <typename C>
    class UtfWriter:
    public OutputIterator<UtfWriter<C>> {
    public:
        using code_unit = C;
        using string_type = basic_string<C>;
        UtfWriter() noexcept {}
        explicit UtfWriter(string_type& dst) noexcept:
            sptr(&dst) { UnicornDetail::default_flags(fset, err_flags, err_ignore); }
        UtfWriter(string_type& dst, uint32_t flags) noexcept:
            sptr(&dst), fset(flags) { UnicornDetail::default_flags(fset, err_flags, err_ignore); }
        UtfWriter& operator=(char32_t u);
        bool valid() const noexcept { return ok; }
    private:
        string_type* sptr = nullptr;  // Destination string
        uint32_t fset = err_ignore;   // Error handling flag
        bool ok = false;              // Most recent character is valid
    };

    template <typename C>
    UtfWriter<C>& UtfWriter<C>::operator=(char32_t u) {
        using namespace UnicornDetail;
        if (! sptr)
            return *this;
        bool fast = fset & err_ignore;
        auto pos = sptr->size();
        sptr->resize(pos + UtfEncoding<C>::max_units);
        size_t rc = 0;
        if (fast || char_is_unicode(u))
            rc = UtfEncoding<C>::encode(u, &(*sptr)[pos]);
        sptr->resize(pos + rc);
        if (fast) {
            ok = true;
        } else {
            ok = rc > 0;
            if (! ok) {
                if (fset & err_throw)
                    throw EncodingError(UtfEncoding<C>::name(), pos, &u);
                else
                    append_error(*sptr);
            }
        }
        return *this;
    }

    using Utf8Writer = UtfWriter<char>;
    using Utf16Writer = UtfWriter<char16_t>;
    using Utf32Writer = UtfWriter<char32_t>;
    using WcharWriter = UtfWriter<wchar_t>;

    template <typename C>
    UtfWriter<C> utf_writer(basic_string<C>& dst, uint32_t flags = 0) noexcept {
        return {dst, flags};
    }

    // UTF conversion functions

    namespace UnicornDetail {

        template <typename C1, typename C2>
        struct Recode {
            void operator()(const C1* src, size_t n, basic_string<C2>& dst, uint32_t flags) const {
                UnicornDetail::default_flags(flags, err_flags, err_ignore);
                if (! src)
                    return;
                size_t pos = 0;
                char32_t u = 0;
                C2 buf[UtfEncoding<C2>::max_units];
                while (pos < n) {
                    auto rc = UtfEncoding<C1>::decode(src + pos, n - pos, u);
                    if (! (flags & err_ignore) && ! char_is_unicode(u)) {
                        if (flags & err_throw)
                            throw EncodingError(UtfEncoding<C1>::name(), pos, src + pos, rc);
                        u = replacement_char;
                    }
                    pos += rc;
                    rc = UtfEncoding<C2>::encode(u, buf);
                    dst.append(buf, rc);
                }
            }
        };

        template <typename C1>
        struct Recode<C1, char32_t> {
            void operator()(const C1* src, size_t n, u32string& dst, uint32_t flags) const {
                UnicornDetail::default_flags(flags, err_flags, err_ignore);
                if (! src)
                    return;
                size_t pos = 0;
                char32_t u = 0;
                if (flags & err_ignore) {
                    while (pos < n) {
                        pos += UtfEncoding<C1>::decode(src + pos, n - pos, u);
                        dst += u;
                    }
                } else {
                    while (pos < n) {
                        auto rc = UtfEncoding<C1>::decode(src + pos, n - pos, u);
                        if (char_is_unicode(u))
                            dst += u;
                        else if (flags & err_throw)
                            throw EncodingError(UtfEncoding<C1>::name(), pos, src + pos, rc);
                        else
                            dst += replacement_char;
                        pos += rc;
                    }
                }
            }
        };

        template <typename C2>
        struct Recode<char32_t, C2> {
            void operator()(const char32_t* src, size_t n, basic_string<C2>& dst, uint32_t flags) const {
                UnicornDetail::default_flags(flags, err_flags, err_ignore);
                if (! src)
                    return;
                char32_t u = 0;
                C2 buf[UtfEncoding<C2>::max_units];
                for (size_t pos = 0; pos < n; ++pos) {
                    if (n == npos && src[pos] == 0)
                        break;
                    else if ((flags & err_ignore) || char_is_unicode(src[pos]))
                        u = src[pos];
                    else if (flags & err_throw)
                        throw EncodingError(UtfEncoding<char32_t>::name(), pos, src + pos);
                    else
                        u = replacement_char;
                    auto rc = UtfEncoding<C2>::encode(u, buf);
                    dst.append(buf, rc);
                }
            }
        };

        template <typename C>
        struct Recode<C, C> {
            void operator()(const C* src, size_t n, basic_string<C>& dst, uint32_t flags) const {
                UnicornDetail::default_flags(flags, err_flags, err_ignore);
                if (! src)
                    return;
                size_t pos = 0;
                char32_t u = 0;
                C buf[UtfEncoding<C>::max_units];
                if (flags & err_ignore) {
                    while (pos < n) {
                        auto rc = UtfEncoding<C>::decode(src + pos, n - pos, u);
                        dst.append(src + pos, rc);
                        pos += rc;
                    }
                } else {
                    while (pos < n) {
                        auto rc = UtfEncoding<C>::decode(src + pos, n - pos, u);
                        if (char_is_unicode(u)) {
                            dst.append(src + pos, rc);
                        } else if (flags & err_throw) {
                            throw EncodingError(UtfEncoding<C>::name(), pos, src + pos, rc);
                        } else {
                            auto rc2 = UtfEncoding<C>::encode(replacement_char, buf);
                            dst.append(buf, rc2);
                        }
                        pos += rc;
                    }
                }
            }
        };

        template <>
        struct Recode<char32_t, char32_t> {
            void operator()(const char32_t* src, size_t n, u32string& dst, uint32_t flags) const {
                UnicornDetail::default_flags(flags, err_flags, err_ignore);
                if (! src)
                    return;
                for (size_t pos = 0; pos < n; ++pos) {
                    if (n == npos && src[pos] == 0)
                        break;
                    else if ((flags & err_throw) || char_is_unicode(src[pos]))
                        dst += src[pos];
                    else if (flags & err_throw)
                        throw EncodingError(UtfEncoding<char32_t>::name(), pos, src + pos);
                    else
                        dst += replacement_char;
                }
            }
        };

    };

    template <typename C1, typename C2>
    void recode(const basic_string<C1>& src, basic_string<C2>& dst,
            uint32_t flags = 0) {
        basic_string<C2> result;
        UnicornDetail::Recode<C1, C2>()(src.data(), src.size(), result, flags);
        dst.swap(result);
    }

    template <typename C1, typename C2>
    void recode(const basic_string<C1>& src, size_t offset, basic_string<C2>& dst,
            uint32_t flags = 0) {
        basic_string<C2> result;
        if (offset < src.size())
            UnicornDetail::Recode<C1, C2>()(src.data() + offset, src.size() - offset, result, flags);
        dst.swap(result);
    }

    template <typename C1, typename C2>
    void recode(const C1* src, size_t count, basic_string<C2>& dst, uint32_t flags = 0) {
        basic_string<C2> result;
        UnicornDetail::Recode<C1, C2>()(src, count, result, flags);
        dst.swap(result);
    }

    template <typename C2, typename C1>
    basic_string<C2> recode(const basic_string<C1>& src, uint32_t flags = 0) {
        basic_string<C2> result;
        UnicornDetail::Recode<C1, C2>()(src.data(), src.size(), result, flags);
        return result;
    }

    template <typename C2, typename C1>
    basic_string<C2> recode(const basic_string<C1>& src, size_t offset, uint32_t flags) {
        basic_string<C2> result;
        if (offset < src.size())
            UnicornDetail::Recode<C1, C2>()(src.data() + offset, src.size() - offset, result, flags);
        return result;
    }

    template <typename C>
    u8string to_utf8(const basic_string<C>& src, uint32_t flags = 0) {
        return recode<char>(src, flags);
    }

    template <typename C>
    u16string to_utf16(const basic_string<C>& src, uint32_t flags = 0) {
        return recode<char16_t>(src, flags);
    }

    template <typename C>
    u32string to_utf32(const basic_string<C>& src, uint32_t flags = 0) {
        return recode<char32_t>(src, flags);
    }

    template <typename C>
    wstring to_wstring(const basic_string<C>& src, uint32_t flags = 0) {
        return recode<wchar_t>(src, flags);
    }

    template <typename C>
    NativeString to_native(const basic_string<C>& src, uint32_t flags = 0) {
        return recode<NativeCharacter>(src, flags);
    }

    // UTF validation functions

    template <typename C>
    void sanitize(basic_string<C>& str) {
        basic_string<C> result;
        recode(str, result, err_replace);
        str.swap(result);
    }

    template <typename C>
    bool valid_string(const basic_string<C>& str) {
        using namespace UnicornDetail;
        auto data = str.data();
        size_t pos = 0, size = str.size();
        char32_t u = 0;
        while (pos < size) {
            auto rc = UtfEncoding<C>::decode(data + pos, size - pos, u);
            if (char_is_unicode(u))
                pos += rc;
            else
                return false;
        }
        return true;
    }

    template <typename C>
    void check_string(const basic_string<C>& str) {
        using namespace UnicornDetail;
        auto data = str.data();
        size_t pos = 0, size = str.size();
        char32_t u = 0;
        while (pos < size) {
            auto rc = UtfEncoding<C>::decode(data + pos, size - pos, u);
            if (! char_is_unicode(u))
                throw EncodingError(UtfEncoding<C>::name(), pos, data + pos, rc);
            pos += rc;
        }
    }

    template <typename C>
    size_t valid_count(const basic_string<C>& str) noexcept {
        using namespace UnicornDetail;
        auto data = str.data();
        size_t pos = 0, size = str.size();
        char32_t u = 0;
        while (pos < size) {
            auto rc = UtfEncoding<C>::decode(data + pos, size - pos, u);
            if (! char_is_unicode(u))
                return pos;
            pos += rc;
        }
        return npos;
    }

}
