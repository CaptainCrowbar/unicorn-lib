#pragma once

#include "crow/core.hpp"
#include <map>
#include <stdexcept>
#include <string>

CROW_LDLIB(unicorn)

namespace Unicorn {

    namespace UnicornDetail {

        struct UnicodeVersionTable {
            std::map<Crow::Version, char32_t> table;
            UnicodeVersionTable();
        };

        const UnicodeVersionTable& unicode_version_table();

    }

    // Imports

    using Crow::basic_string;      // std::basic_string
    using Crow::string;            // std::string
    using Crow::u8string;          // std::string
    using Crow::u16string;         // std::u16string
    using Crow::u32string;         // std::u32string
    using Crow::wstring;           // std::wstring
    using Crow::ascii_whitespace;  // "\t\n\v\f\r "
    using Crow::npos;              // std::string::npos

    // Exceptions

    class EncodingError:
    public std::runtime_error {
    public:
        EncodingError():
            std::runtime_error(prefix({}, 0)), enc(), ofs(0) {}
        explicit EncodingError(const u8string& encoding, size_t offset = 0):
            std::runtime_error(prefix(encoding, offset)), enc(encoding), ofs(offset) {}
        template <typename C>
            EncodingError(const u8string& encoding, size_t offset, const C* ptr, size_t n = 1):
            std::runtime_error(prefix(encoding, offset) + hexcode(ptr, n)), enc(encoding), ofs(offset) {}
        u8string encoding() const { return enc; }
        size_t offset() const noexcept { return ofs; }
    private:
        u8string enc;
        size_t ofs;
        static u8string prefix(const u8string& encoding, size_t offset);
        template <typename C> static u8string hexcode(const C* ptr, size_t n);
    };

    template <typename C>
    u8string EncodingError::hexcode(const C* ptr, size_t n) {
        using utype = std::make_unsigned_t<C>;
        if (! ptr || ! n)
            return {};
        u8string s = "; hex";
        auto uptr = reinterpret_cast<const utype*>(ptr);
        for (size_t i = 0; i < n; ++i) {
            s += ' ';
            s += Crow::hex(uptr[i]);
        }
        return s;
    }

    // Basic character types

    #if defined(_XOPEN_SOURCE)
        using NativeCharacter = char;
    #else
        #define UNICORN_NATIVE_WCHAR 1
        using NativeCharacter = wchar_t;
    #endif

    #if WCHAR_MAX < 0x7fffffff
        #define UNICORN_WCHAR_UTF16 1
        using WcharEquivalent = char16_t;
    #else
        #define UNICORN_WCHAR_UTF32 1
        using WcharEquivalent = char32_t;
    #endif

    using NativeString = basic_string<NativeCharacter>;
    using WstringEquivalent = basic_string<WcharEquivalent>;

    namespace Literals {

        NativeString operator"" _nat(const char* s, size_t n);

    }

    // Version information

    Crow::Version unicorn_version() noexcept;
    Crow::Version unicode_version() noexcept;

}
