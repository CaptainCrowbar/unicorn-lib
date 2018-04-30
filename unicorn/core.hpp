#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

RS_LDLIB(unicorn);

namespace RS::Unicorn {

    // Character types

    template <typename T> constexpr bool is_character_type =
        std::is_same<T, char>::value || std::is_same<T, char16_t>::value
        || std::is_same<T, char32_t>::value || std::is_same<T, wchar_t>::value;

    // Exceptions

    class InitializationError:
    public std::runtime_error {
    public:
        explicit InitializationError(const Ustring& message):
            std::runtime_error(message) {}
    };

    class EncodingError:
    public std::runtime_error {
    public:
        EncodingError():
            std::runtime_error(prefix({}, 0)), enc(), ofs(0) {}
        explicit EncodingError(const Ustring& encoding, size_t offset = 0, char32_t c = 0):
            std::runtime_error(prefix(encoding, offset) + hexcode(&c, 1)), enc(std::make_shared<Ustring>(encoding)), ofs(offset) {}
        template <typename C>
            EncodingError(const Ustring& encoding, size_t offset, const C* ptr, size_t n = 1):
            std::runtime_error(prefix(encoding, offset) + hexcode(ptr, n)), enc(std::make_shared<Ustring>(encoding)), ofs(offset) {}
        const char* encoding() const noexcept { static const char c = 0; return enc ? enc->data() : &c; }
        size_t offset() const noexcept { return ofs; }
    private:
        std::shared_ptr<Ustring> enc;
        size_t ofs;
        static Ustring prefix(const Ustring& encoding, size_t offset);
        template <typename C> static Ustring hexcode(const C* ptr, size_t n);
    };

    template <typename C>
    Ustring EncodingError::hexcode(const C* ptr, size_t n) {
        using utype = std::make_unsigned_t<C>;
        if (! ptr || ! n)
            return {};
        Ustring s = "; hex";
        auto uptr = reinterpret_cast<const utype*>(ptr);
        for (size_t i = 0; i < n; ++i) {
            s += ' ';
            s += hex(uptr[i]);
        }
        return s;
    }

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
