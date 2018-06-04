#pragma once

#include "unicorn/character.hpp"
#include "unicorn/utf.hpp"
#include "unicorn/utility.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace RS::Unicorn {

    namespace UnicornDetail {

        // These are only exposed in the header to enable testing

        using EncodingTag =
            #ifdef _XOPEN_SOURCE
                Ustring;
            #else
                uint32_t;
            #endif

        Ustring guess_utf(const std::string& str);
        EncodingTag lookup_encoding(const Ustring& name, uint32_t flags = 0);
        EncodingTag lookup_encoding(uint32_t page, uint32_t flags = 0);

    }

    // Constants

    struct Mbcs {

        static constexpr uint32_t strict = 8; // Do not look up encoding

    };

    // Exceptions

    class UnknownEncoding:
    public std::runtime_error {
    public:
        UnknownEncoding();
        explicit UnknownEncoding(const Ustring& encoding, const Ustring& details = {});
        explicit UnknownEncoding(uint32_t encoding, const Ustring& details = {});
        const char* encoding() const noexcept;
    private:
        std::shared_ptr<Ustring> enc;
        static Ustring assemble(const Ustring& encoding, const Ustring& details);
    };

    // Utility functions

    Ustring local_encoding(const Ustring& default_encoding = "utf-8");

    // Conversion functions

    void import_string(const std::string& src, Ustring& dst, const Ustring& enc = {}, uint32_t flags = 0);
    void import_string(const std::string& src, Ustring& dst, uint32_t enc, uint32_t flags = 0);
    void export_string(const Ustring& src, std::string& dst, const Ustring& enc = {}, uint32_t flags = 0);
    void export_string(const Ustring& src, std::string& dst, uint32_t enc, uint32_t flags = 0);

}
