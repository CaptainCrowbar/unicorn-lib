#pragma once

#include "unicorn/core.hpp"
#include "unicorn/utf.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace RS {

    namespace Unicorn {

        namespace UnicornDetail {

            // These are only exposed in the header to enable testing

            using EncodingTag =
                #ifdef _XOPEN_SOURCE
                    U8string;
                #else
                    uint32_t;
                #endif

            U8string guess_utf(const std::string& str);
            EncodingTag lookup_encoding(const U8string& name, uint32_t flags = 0);
            EncodingTag lookup_encoding(uint32_t page, uint32_t flags = 0);

        }

        // Constants

        namespace Mbcs {

            constexpr uint32_t strict = 8; // Do not look up encoding

        }

        // Exceptions

        class UnknownEncoding:
        public std::runtime_error {
        public:
            UnknownEncoding();
            explicit UnknownEncoding(const U8string& encoding, const U8string& details = {});
            explicit UnknownEncoding(uint32_t encoding, const U8string& details = {});
            const char* encoding() const noexcept;
        private:
            std::shared_ptr<U8string> enc;
            static U8string assemble(const U8string& encoding, const U8string& details);
        };

        // Utility functions

        U8string local_encoding(const U8string& default_encoding = "utf-8");

        // Conversion functions

        void import_string(const std::string& src, U8string& dst, const U8string& enc = {}, uint32_t flags = 0);
        void import_string(const std::string& src, U8string& dst, uint32_t enc, uint32_t flags = 0);
        void export_string(const U8string& src, std::string& dst, const U8string& enc = {}, uint32_t flags = 0);
        void export_string(const U8string& src, std::string& dst, uint32_t enc, uint32_t flags = 0);

    }

}
