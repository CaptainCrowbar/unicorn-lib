#include "unicorn/mbcs.hpp"
#include "unicorn/iana-character-sets.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <map>
#include <unordered_map>

#if defined(PRI_TARGET_UNIX)
    #include <iconv.h>
#else
    #include <windows.h>
#endif

using namespace std::literals;
using namespace Unicorn::Literals;

namespace Unicorn {

    namespace {

        using CharsetPtr = const UnicornDetail::CharsetInfo*;

        #if defined(PRI_TARGET_UNIX)

            constexpr const char* utf8_tag       = "utf-8";
            constexpr const char* utf16_tag      = "utf-16";
            constexpr const char* utf16swap_tag  = "utf-16-swapped";
            constexpr const char* utf32_tag      = "utf-32";
            constexpr const char* utf32swap_tag  = "utf-32-swapped";

            struct Iconv {
            public:
                iconv_t cd;
                Iconv(const u8string& from, const u8string& to) { cd = iconv_open(to.data(), from.data()); }
                ~Iconv() { if (*this) iconv_close(cd); }
                explicit operator bool() const { return cd != iconv_t(-1); }
                bool operator!() const { return cd == iconv_t(-1); }
                void reset() { if (*this) iconv(cd, nullptr, nullptr, nullptr, nullptr); }
                Iconv(const Iconv&) = delete;
                Iconv(Iconv&&) = delete;
                Iconv& operator=(const Iconv&) = delete;
                Iconv& operator=(Iconv&&) = delete;
            };

            bool valid_iconv(u8string tag) { return bool(Iconv(tag, "utf-8"s)); }

        #else

            constexpr uint32_t utf8_tag       = 65001;
            constexpr uint32_t utf16_tag      = 1200;
            constexpr uint32_t utf16swap_tag  = 1201;
            constexpr uint32_t utf32_tag      = 12000;
            constexpr uint32_t utf32swap_tag  = 12001;

            bool valid_codepage(uint32_t tag) { return MultiByteToWideChar(tag, 0, "", -1, nullptr, 0) > 0; }

        #endif

        static constexpr auto wchar_tag = sizeof(wchar_t) == 2 ? utf16_tag : utf32_tag;

        u8string smash_name(const u8string& name, bool stripz) {
            u8string result;
            size_t i = 0, size = name.size();
            while (i < size) {
                while (i < size && ! ascii_isalnum(name[i]))
                    ++i;
                if (i == size)
                    break;
                auto j = i + 1;
                if (ascii_isdigit(name[i])) {
                    while (j < size && ascii_isdigit(name[j]))
                        ++j;
                    if (stripz)
                        while (i < j - 1 && name[i] == '0')
                            ++i;
                } else {
                    while (j < size && ascii_isalpha(name[j]))
                        ++j;
                }
                result.append(name, i, j - i);
                i = j;
            }
            return ascii_lowercase(result);
        }

        class NameIterator:
        public ForwardIterator<NameIterator, const u8string> {
        public:
            explicit NameIterator(const char* ptr = nullptr): s(), p(ptr), q(ptr) { ++*this; }
            const u8string& operator*() const noexcept { return s; }
            NameIterator& operator++();
            bool operator==(const NameIterator& rhs) const noexcept { return p == rhs.p; }
        private:
            u8string s;
            const char* p;
            const char* q;
        };

        NameIterator& NameIterator::operator++() {
            if (! q || ! *q) {
                p = q = nullptr;
            } else {
                p = q;
                q = strchr(p, ',');
                if (q) {
                    s.assign(p, q);
                    ++q;
                } else {
                    s = p;
                }
            }
            return *this;
        }

        Irange<NameIterator> name_range(const char* p) { return {NameIterator(p), NameIterator()}; }

        class CharsetMap {
        public:
            CharsetMap();
            CharsetPtr operator[](uint32_t page) const {
                auto i = pages.find(page);
                return i == std::end(pages) ? nullptr : i->second;
            }
            CharsetPtr operator[](const u8string& name) const {
                auto i = names.find(name);
                return i == std::end(names) ? nullptr : i->second;
            }
        private:
            using page_map = std::unordered_map<uint32_t, CharsetPtr>;
            using name_map = std::unordered_map<u8string, CharsetPtr>;
            page_map pages;
            name_map names;
        };

        CharsetMap::CharsetMap() {
            for (auto& charset: UnicornDetail::iana_character_sets) {
                for (auto page: charset.pages)
                    if (page)
                        pages[page] = &charset;
                for (auto& name: name_range(charset.names))
                    names[smash_name(name, true)] = &charset;
            }
        }

        bool big_endian() noexcept {
            static constexpr uint16_t x = 1;
            return *reinterpret_cast<const uint8_t*>(&x) == 0;
        }

        char16_t reverse_char16(char16_t c) {
            auto p = reinterpret_cast<uint8_t*>(&c);
            std::swap(p[0], p[1]);
            return c;
        }

        char32_t reverse_char32(char32_t c) {
            auto p = reinterpret_cast<uint8_t*>(&c);
            std::swap(p[0], p[3]);
            std::swap(p[1], p[2]);
            return c;
        }

        UnicornDetail::EncodingTag find_encoding(const u8string& name) {
            using namespace UnicornDetail;
            static const CharsetMap map;
            static const auto match_codepage = "(?:cp|dos|ibm|ms|windows)-?(\\d+)"_re_i;
            static const auto match_integer = "\\d+"_re;
            static const auto match_unicode = "(?:cs|x)?(?:iso10646)?((?:ucs|utf)\\d+)(be|le|internal|swapped)?"_re;
            #if defined(PRI_TARGET_UNIX)
                static const vector<u8string> codepage_prefixes {"cp","dos","ibm","ms","windows-"};
            #endif
            // Check for UTF encodings
            auto smashed = smash_name(name, true);
            auto match = match_unicode.match(smashed);
            if (match) {
                u8string m1 = match[1], m2 = match[2];
                bool swap = false;
                if (m2 == "be")
                    swap = ! big_endian();
                else if (m2 == "le")
                    swap = big_endian();
                else if (m2 == "swapped")
                    swap = true;
                if (m1 == "utf8")
                    return utf8_tag;
                else if (m1 == "utf16")
                    return swap ? utf16swap_tag : utf16_tag;
                else if (m1 == "utf32" || m1 == "ucs4")
                    return swap ? utf32swap_tag : utf32_tag;
            }
            #if defined(PRI_TARGET_UNIX)
                // Try using the name directly in iconv()
                if (valid_iconv(name))
                    return name;
            #endif
            // Strip prefixes (x-, cs, cp, dos, ibm, ms, windows)
            auto current = name;
            if (str_starts_with(current, "cs") || str_starts_with(current, "x-"))
                current.erase(0, 2);
            match = match_codepage.match(current);
            if (match)
                current = match[1];
            CharsetPtr csp = nullptr;
            if (match || match_integer.match(current)) {
                // Name is an integer, presumably a code page
                auto page = uint32_t(decnum(current));
                #if defined(PRI_TARGET_UNIX)
                    // Look the number up in the {codepage => charset} map
                    csp = map[page];
                    if (! csp) {
                        // If not found, try variations against iconv()
                        for (auto& prefix: codepage_prefixes) {
                            auto tryname = prefix + current;
                            if (valid_iconv(tryname))
                                return tryname;
                        }
                        return EncodingTag();
                    }
                #else
                    // Try using the number as the code page
                    if (valid_codepage(page))
                        return page;
                    else
                        return EncodingTag();
                #endif
            } else {
                // Look the name up in the {normalized name => charset} map
                csp = map[smash_name(current, true)];
                if (! csp) {
                    #if defined(PRI_TARGET_UNIX)
                        // If not found, try variations against iconv()
                        if (valid_iconv(current))
                            return current;
                        auto tryname = "cs" + smash_name(current, false);
                        if (valid_iconv(tryname))
                            return tryname;
                        tryname = "x-" + current;
                        if (valid_iconv(tryname))
                            return tryname;
                    #endif
                    return EncodingTag();
                }
            }
            // If we reach here, we have identified a charset record
            #if defined(PRI_TARGET_UNIX)
                // For each name in the charset's name list, try variations against iconv()
                for (auto& nm: name_range(csp->names))
                    if (valid_iconv(nm))
                        return nm;
                for (auto& nm: name_range(csp->names)) {
                    auto tryname = "cs" + smash_name(nm, false);
                    if (valid_iconv(tryname))
                        return tryname;
                }
                for (auto& nm: name_range(csp->names)) {
                    auto tryname = "x-" + nm;
                    if (valid_iconv(tryname))
                        return tryname;
                }
            #else
                // Try each codepage in the charset's codepage list
                for (auto page: csp->pages)
                    if (valid_codepage(page))
                        return page;
            #endif
            return EncodingTag();
        }

        #if defined(PRI_TARGET_UNIX)

            void native_recode(const string& src, string& dst, const u8string& from, const u8string& to,
                    const u8string& tag, uint32_t flags) {
                Iconv conv(from, to);
                if (! conv)
                    throw UnknownEncoding(tag);
                string buf(src.size(), 0);
                size_t inpos = 0, outpos = 0;
                while (inpos < src.size()) {
                    auto inbuf = const_cast<char*>(&src[inpos]); // Posix brain damage
                    auto inbytes = src.size() - inpos;
                    auto outbuf = &buf[outpos];
                    auto outbytes = buf.size() - outpos;
                    errno = 0;
                    iconv(conv.cd, &inbuf, &inbytes, &outbuf, &outbytes);
                    inpos = src.size() - inbytes;
                    outpos = buf.size() - outbytes;
                    if (errno == 0) {
                        break;
                    } else if (errno == E2BIG) {
                        buf.resize(buf.size() + src.size());
                    } else {
                        if (flags & err_throw)
                            throw EncodingError(tag, inpos, &src[inpos]);
                        if (outbytes < 3)
                            buf.resize(buf.size() + 3);
                        memcpy(&buf[outpos], utf8_replacement, 3);
                        ++inpos;
                        outpos += 3;
                        conv.reset();
                    }
                }
                buf.resize(outpos);
                dst.swap(buf);
            }

        #endif

    }

    // Exceptions

    u8string UnknownEncoding::assemble(const u8string& encoding, const u8string& details) {
        u8string s = "Unknown encoding";
        if (! encoding.empty()) {
            s += ": ";
            s += encoding;
        }
        if (! details.empty()) {
            s += "; ";
            s += details;
        }
        return s;
    }

    // Utility functions

    u8string local_encoding(const u8string& default_encoding) {
        #if defined(PRI_TARGET_UNIX)
            static constexpr const char* locale_vars[] {"LC_ALL", "LC_CTYPE", "LANG"};
            u8string name;
            for (auto key: locale_vars) {
                string value = cstr(getenv(key));
                size_t dot = value.find('.');
                if (dot != npos) {
                    value.erase(0, dot + 1);
                    size_t cut = value.find_first_of(",;");
                    if (cut != npos)
                        value.resize(cut);
                    auto cut2 = std::find_if(PRI_BOUNDS(value),
                        [] (char c) { return uint8_t(c) > 127; });
                    value.erase(cut2, std::end(value));
                    return value;
                }
            }
        #else
            CPINFOEX info;
            if (GetCPInfoEx(CP_THREAD_ACP, 0, &info) && info.CodePage > 0)
                return dec(info.CodePage);
        #endif
        return default_encoding;
    }

    // Conversion functions

    namespace UnicornDetail {

        u8string guess_utf(const string& str) {
            constexpr size_t max_check_bytes = 100;
            if (str.empty())
                return "utf-8";
            if (str.size() >= 3 && memcmp(str.data(), utf8_bom, 3) == 0)
                return "utf-8";
            if (str.size() >= 4 && str.size() % 4 == 0) {
                if (memcmp(str.data(), "\0\0\xfe\xff", 4) == 0)
                    return "utf-32be";
                else if (memcmp(str.data(), "\xff\xfe\0\0", 4) == 0)
                    return "utf-32le";
            }
            if (str.size() >= 2 && str.size() % 2 == 0) {
                if (memcmp(str.data(), "\xfe\xff", 2) == 0)
                    return "utf-16be";
                else if (memcmp(str.data(), "\xff\xfe", 2) == 0)
                    return "utf-16le";
            }
            auto check_bytes = std::min(str.size(), max_check_bytes);
            size_t nonzero[] {0,0,0,0};
            for (size_t i = 0; i < check_bytes; ++i)
                if (str[i])
                    ++nonzero[i % 4];
            if (str.size() % 4 == 0) {
                if (nonzero[0] == 0 && 8 * nonzero[1] < check_bytes  && 8 * nonzero[3] > check_bytes)
                    return "utf-32be";
                else if (8 * nonzero[0] > check_bytes && 8 * nonzero[2] < check_bytes  && nonzero[3] == 0)
                    return "utf-32le";
            }
            if (str.size() % 2 == 0) {
                auto nz0 = 4 * (nonzero[0] + nonzero[2]);
                auto nz1 = 4 * (nonzero[1] + nonzero[3]);
                if (nz0 < check_bytes && nz1 > check_bytes)
                    return "utf-16be";
                else if (nz0 > check_bytes && nz1 < check_bytes)
                    return "utf-16le";
            }
            return "utf-8";
        }

        EncodingTag lookup_encoding(const u8string& name) {
            static std::map<u8string, EncodingTag> cache;
            static Mutex mtx;
            auto lcname = ascii_lowercase(name);
            if (lcname.empty())
                throw UnknownEncoding();
            else if (lcname == "char")
                return lookup_encoding(local_encoding());
            else if (lcname == "wchar_t")
                return wchar_tag;
            else if (lcname == "utf")
                return EncodingTag();
            {
                MutexLock lock(mtx);
                auto it = cache.find(lcname);
                if (it != std::end(cache))
                    return it->second;
            }
            auto tag = EncodingTag();
            #if defined(PRI_TARGET_NATIVE_WINDOWS)
                if (lcname.find_first_not_of("0123456789") == npos)
                    tag = lookup_encoding(uint32_t(decnum(lcname)));
            #endif
            if (tag == EncodingTag())
                tag = find_encoding(lcname);
            if (tag == EncodingTag())
                throw UnknownEncoding(name);
            MutexLock lock(mtx);
            cache[lcname] = tag;
            return tag;
        }

        EncodingTag lookup_encoding(uint32_t page) {
            static std::map<uint32_t, EncodingTag> cache;
            static Mutex mtx;
            if (page == 0)
                throw UnknownEncoding();
            {
                MutexLock lock(mtx);
                auto it = cache.find(page);
                if (it != std::end(cache))
                    return it->second;
            }
            auto tag = EncodingTag();
            #if defined(PRI_TARGET_UNIX)
                tag = lookup_encoding(dec(page));
            #else
                if (page == utf8_tag || page == utf16_tag || page == utf16swap_tag
                        || page == utf32_tag || page == utf32swap_tag || valid_codepage(page))
                    tag = page;
            #endif
            if (tag == EncodingTag())
                throw UnknownEncoding(page);
            MutexLock lock(mtx);
            cache[page] = tag;
            return tag;
        }

        void mbcs_flags(uint32_t& flags) {
            if (flags & err_ignore)
                throw std::invalid_argument("Invalid MBCS conversion flag: err_ignore");
            if (flags == 0)
                flags = err_replace;
        }

        #if defined(PRI_TARGET_UNIX)

            void native_import(const string& src, string& dst, u8string tag, uint32_t flags) {
                native_recode(src, dst, tag, "utf-8"s, tag, flags);
            }

            void native_export(const string& src, string& dst, u8string tag, uint32_t flags) {
                native_recode(src, dst, "utf-8"s, tag, tag, flags);
            }

        #else

            void native_import(const string& src, wstring& dst, uint32_t tag, uint32_t flags) {
                uint32_t wflags = 0;
                if (flags & err_throw)
                    wflags |= MB_ERR_INVALID_CHARS;
                auto rc = MultiByteToWideChar(tag, wflags, src.data(), int(src.size()), nullptr, 0);
                if (rc == 0) {
                    auto err = GetLastError();
                    if (err == ERROR_NO_UNICODE_TRANSLATION)
                        throw EncodingError(dec(tag));
                    else
                        throw UnknownEncoding(dec(tag), windows_category().message(err));
                }
                dst.resize(rc);
                MultiByteToWideChar(tag, wflags, src.data(), int(src.size()), &dst[0], int(rc));
            }

            void native_export(const wstring& src, string& dst, uint32_t tag, uint32_t flags) {
                vector<uint32_t> tryflags;
                if (flags & err_throw)
                    tryflags = {WC_NO_BEST_FIT_CHARS | WC_ERR_INVALID_CHARS, WC_ERR_INVALID_CHARS, WC_NO_BEST_FIT_CHARS, 0};
                else
                    tryflags = {WC_NO_BEST_FIT_CHARS, 0};
                int rc = 0;
                uint32_t err = 0, wflags = 0;
                for (auto f: tryflags) {
                    wflags = f;
                    rc = WideCharToMultiByte(tag, wflags, &src[0], int(src.size()),
                        nullptr, 0, nullptr, nullptr);
                    err = GetLastError();
                    if (rc > 0 || err != ERROR_INVALID_FLAGS)
                        break;
                }
                if (rc == 0) {
                    if (err == ERROR_NO_UNICODE_TRANSLATION)
                        throw EncodingError(dec(tag));
                    else
                        throw UnknownEncoding(dec(tag), windows_category().message(err));
                }
                dst.resize(rc);
                WideCharToMultiByte(tag, wflags, &src[0], int(src.size()), &dst[0], rc, nullptr, nullptr);
            }

        #endif

        bool utf_import(const string& src, NativeString& dst, EncodingTag tag, uint32_t flags) {
            if (tag == utf8_tag) {
                recode(src, dst, flags);
                return true;
            } else if (tag == utf16_tag || tag == utf16swap_tag) {
                auto extra = src.size() % 2;
                if (extra && (flags & err_throw))
                    throw EncodingError("UTF-16", src.size() - extra,
                        src.data() + src.size() - extra, extra);
                u16string src16(src.size() / 2, 0);
                memcpy(&src16[0], src.data(), src.size());
                if (tag == utf16swap_tag)
                    std::transform(PRI_BOUNDS(src16), std::begin(src16), reverse_char16);
                if (extra)
                    src16 += char16_t(replacement_char);
                recode(src16, dst, flags);
                return true;
            } else if (tag == utf32_tag || tag == utf32swap_tag) {
                auto extra = src.size() % 4;
                if (extra && (flags & err_throw))
                    throw EncodingError("UTF-32", src.size() - extra,
                        src.data() + src.size() - extra, extra);
                u32string src32(src.size() / 4, 0);
                memcpy(&src32[0], src.data(), src.size());
                if (tag == utf32swap_tag)
                    std::transform(PRI_BOUNDS(src32), std::begin(src32), reverse_char32);
                if (extra)
                    src32 += char16_t(replacement_char);
                recode(src32, dst, flags);
                return true;
            } else {
                return false;
            }
        }

        bool utf_export(const NativeString& src, string& dst, EncodingTag tag, uint32_t flags) {
            if (tag == utf8_tag) {
                recode(src, dst, flags);
                return true;
            } else if (tag == utf16_tag || tag == utf16swap_tag) {
                u16string dst16;
                recode(src, dst16, flags);
                if (tag == utf16swap_tag)
                    std::transform(PRI_BOUNDS(dst16), std::begin(dst16), reverse_char16);
                dst.resize(2 * dst16.size());
                memcpy(&dst[0], dst16.data(), dst.size());
                return true;
            } else if (tag == utf32_tag || tag == utf32swap_tag) {
                u32string dst32;
                recode(src, dst32, flags);
                if (tag == utf32swap_tag)
                    std::transform(PRI_BOUNDS(dst32), std::begin(dst32), reverse_char32);
                dst.resize(4 * dst32.size());
                memcpy(&dst[0], dst32.data(), dst.size());
                return true;
            } else {
                return false;
            }
        }

    }

}
