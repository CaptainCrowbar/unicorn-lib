#pragma once

#include "prion/core.hpp"
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

namespace Unicorn {

    using namespace Prion;
    namespace Literals { using namespace Prion::Literals; }

    // Exceptions

    class InitializationError:
    public std::runtime_error {
    public:
        explicit InitializationError(const u8string& message):
            std::runtime_error(message) {}
    };

    class EncodingError:
    public std::runtime_error {
    public:
        EncodingError():
            std::runtime_error(prefix({}, 0)), enc(), ofs(0) {}
        explicit EncodingError(const u8string& encoding, size_t offset = 0, char32_t c = 0):
            std::runtime_error(prefix(encoding, offset) + hexcode(&c, 1)), enc(make_shared<u8string>(encoding)), ofs(offset) {}
        template <typename C>
            EncodingError(const u8string& encoding, size_t offset, const C* ptr, size_t n = 1):
            std::runtime_error(prefix(encoding, offset) + hexcode(ptr, n)), enc(make_shared<u8string>(encoding)), ofs(offset) {}
        const char* encoding() const noexcept { static const char c = 0; return enc ? enc->data() : &c; }
        size_t offset() const noexcept { return ofs; }
    private:
        shared_ptr<u8string> enc;
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
            s += hex(uptr[i]);
        }
        return s;
    }

    // Basic character types

    template <typename C> struct IsCharacterType { static constexpr bool value = false; };
    template <> struct IsCharacterType<char> { static constexpr bool value = true; };
    template <> struct IsCharacterType<char16_t> { static constexpr bool value = true; };
    template <> struct IsCharacterType<char32_t> { static constexpr bool value = true; };
    template <> struct IsCharacterType<wchar_t> { static constexpr bool value = true; };

    #if defined(PRI_TARGET_UNIX)
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

    // Version information

    namespace UnicornDetail {

        struct UnicodeVersionTable {
            std::map<Version, char32_t> table;
            UnicodeVersionTable();
        };

        const UnicodeVersionTable& unicode_version_table();

    }

    Version unicorn_version() noexcept;
    Version unicode_version() noexcept;

}
