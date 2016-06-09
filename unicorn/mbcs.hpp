#pragma once

#include "unicorn/core.hpp"
#include "unicorn/utf.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace Unicorn {

    namespace UnicornDetail {

        // These are only exposed in the header to enable testing

        using EncodingTag =
            #if defined(PRI_TARGET_UNIX)
                u8string;
            #else
                uint32_t;
            #endif

        u8string guess_utf(const string& str);
        EncodingTag lookup_encoding(const u8string& name, uint32_t flags = 0);
        EncodingTag lookup_encoding(uint32_t page, uint32_t flags = 0);

    }

    // Constants

    constexpr uint32_t mb_strict = 8; // Do not look up encoding

    // Exceptions

    class UnknownEncoding:
    public std::runtime_error {
    public:
        UnknownEncoding();
        explicit UnknownEncoding(const u8string& encoding, const u8string& details = {});
        explicit UnknownEncoding(uint32_t encoding, const u8string& details = {});
        const char* encoding() const noexcept;
    private:
        shared_ptr<u8string> enc;
        static u8string assemble(const u8string& encoding, const u8string& details);
    };

    // Utility functions

    u8string local_encoding(const u8string& default_encoding = "utf-8");

    // Conversion functions

    void import_string(const string& src, u8string& dst, const u8string& enc = {}, uint32_t flags = 0);
    void import_string(const string& src, u8string& dst, uint32_t enc, uint32_t flags = 0);
    void export_string(const u8string& src, string& dst, const u8string& enc = {}, uint32_t flags = 0);
    void export_string(const u8string& src, string& dst, uint32_t enc, uint32_t flags = 0);

}
